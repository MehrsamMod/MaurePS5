#include "command_processor.h"

CommandProcessor::CommandProcessor(VkDevice device, VkCommandBuffer cmd, VkQueue queue, MemoryTracker* memory)
    : device_(device)
    , cmd_(cmd)
    , queue_(queue)
    , memory_(memory)
    , index_buffer_base_(0)
    , draw_indirect_base_(0)
    , dispatch_indirect_base_(0)
    , index_buffer_size_(0)
    , num_instances_(1)
    , index_base_(0)
    , graphics_pipeline_(VK_NULL_HANDLE)
    , graphics_layout_(VK_NULL_HANDLE)
    , compute_pipeline_(VK_NULL_HANDLE)
    , compute_layout_(VK_NULL_HANDLE)
    , index_buffer_(VK_NULL_HANDLE)
    , index_offset_(0)
    , index_type_(VK_INDEX_TYPE_UINT32)
    , graphics_pipeline_dirty_(true)
    , compute_pipeline_dirty_(true)
    , event_(VK_NULL_HANDLE)
    , fence_(VK_NULL_HANDLE)
{
    VkEventCreateInfo ei{VK_STRUCTURE_TYPE_EVENT_CREATE_INFO};
    vkCreateEvent(device_, &ei, nullptr, &event_);
    VkFenceCreateInfo fi{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(device_, &fi, nullptr, &fence_);
}

void CommandProcessor::setGraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout)
{
    graphics_pipeline_ = pipeline;
    graphics_layout_ = layout;
    graphics_pipeline_dirty_ = true;
}

void CommandProcessor::setComputePipeline(VkPipeline pipeline, VkPipelineLayout layout)
{
    compute_pipeline_ = pipeline;
    compute_layout_ = layout;
    compute_pipeline_dirty_ = true;
}

void CommandProcessor::setDescriptorSets(VkPipelineBindPoint bindPoint, uint32_t firstSet, uint32_t count, const VkDescriptorSet* sets)
{
    VkPipelineLayout layout = (bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) ? graphics_layout_ : compute_layout_;
    if (layout != VK_NULL_HANDLE && count > 0)
        vkCmdBindDescriptorSets(cmd_, bindPoint, layout, firstSet, count, sets, 0, nullptr);
}

void CommandProcessor::setIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType type)
{
    index_buffer_ = buffer;
    index_offset_ = offset;
    index_type_ = type;
    if (buffer != VK_NULL_HANDLE)
        vkCmdBindIndexBuffer(cmd_, buffer, offset, type);
}

void CommandProcessor::setVertexBuffers(uint32_t first, uint32_t count, const VkBuffer* buffers, const VkDeviceSize* offsets)
{
    if (count > 0)
        vkCmdBindVertexBuffers(cmd_, first, count, buffers, offsets);
}

void CommandProcessor::emitGlobalBarrier()
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void CommandProcessor::emitMemoryBarrier()
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}


VkBuffer CommandProcessor::resolveBuffer(uint64_t gpu_va, VkDeviceSize* out_offset, VkDeviceSize* out_remaining) const
{
    if (!memory_)
    {
        if (out_offset) *out_offset = 0;
        if (out_remaining) *out_remaining = 0;
        return VK_NULL_HANDLE;
    }
    return memory_->findBuffer(gpu_va, out_offset, out_remaining);
}

void CommandProcessor::process(const uint32_t* packet)
{
    const uint32_t header = packet[0];
    const uint32_t type   = (header >> 30) & 0x3;
    const uint32_t opcode = (header >> 8)  & 0xFF;
    const uint32_t count  = (header >> 16) & 0x3FFF;
    if (type != 3)
        return;
    const uint32_t* body = packet + 1;
    switch (opcode)
    {
    case PKT3_NOP: handle_nop(body, count); break;
    case PKT3_SET_BASE: handle_set_base(body, count); break;
    case PKT3_CLEAR_STATE: handle_clear_state(body, count); break;
    case PKT3_INDEX_BUFFER_SIZE: handle_index_buffer_size(body, count); break;
    case PKT3_DISPATCH_DIRECT: handle_dispatch_direct(body, count); break;
    case PKT3_DISPATCH_INDIRECT: handle_dispatch_indirect(body, count); break;
    case PKT3_ATOMIC_MEM: handle_atomic_mem(body, count); break;
    case PKT3_OCCLUSION_QUERY: handle_occlusion_query(body, count); break;
    case PKT3_SET_PREDICATION: handle_set_predication(body, count); break;
    case PKT3_COND_EXEC: handle_cond_exec(body, count); break;
    case PKT3_PRED_EXEC: handle_pred_exec(body, count); break;
    case PKT3_DRAW_INDIRECT: handle_draw_indirect(body, count); break;
    case PKT3_DRAW_INDEX_INDIRECT: handle_draw_index_indirect(body, count); break;
    case PKT3_INDEX_BASE: handle_index_base(body, count); break;
    case PKT3_DRAW_INDEX_2: handle_draw_index_2(body, count); break;
    case PKT3_CONTEXT_CONTROL: handle_context_control(body, count); break;
    case PKT3_DRAW_INDIRECT_MULTI: handle_draw_indirect_multi(body, count); break;
    case PKT3_DRAW_INDEX_AUTO: handle_draw_index_auto(body, count); break;
    case PKT3_NUM_INSTANCES: handle_num_instances(body, count); break;
    case PKT3_DRAW_INDEX_MULTI_AUTO: handle_draw_index_multi_auto(body, count); break;
    case PKT3_INDIRECT_BUFFER_CONST: handle_indirect_buffer_const(body, count); break;
    case PKT3_STRMOUT_BUFFER_UPDATE: handle_strmout_buffer_update(body, count); break;
    case PKT3_DRAW_INDEX_OFFSET_2: handle_draw_index_offset_2(body, count); break;
    case PKT3_WRITE_DATA: handle_write_data(body, count); break;
    case PKT3_DRAW_INDEX_INDIRECT_MULTI: handle_draw_index_indirect_multi(body, count); break;
    case PKT3_MEM_SEMAPHORE: handle_mem_semaphore(body, count); break;
    case PKT3_WAIT_REG_MEM: handle_wait_reg_mem(body, count); break;
    case PKT3_INDIRECT_BUFFER: handle_indirect_buffer(body, count); break;
    case PKT3_COPY_DATA: handle_copy_data(body, count); break;
    case PKT3_PFP_SYNC_ME: handle_pfp_sync_me(body, count); break;
    case PKT3_COND_WRITE: handle_cond_write(body, count); break;
    case PKT3_EVENT_WRITE: handle_event_write(body, count); break;
    case PKT3_RELEASE_MEM: handle_release_mem(body, count); break;
    case PKT3_DMA_DATA: handle_dma_data(body, count); break;
    case PKT3_CONTEXT_REG_RMW: handle_context_reg_rmw(body, count); break;
    case PKT3_ACQUIRE_MEM: handle_acquire_mem(body, count); break;
    case PKT3_REWIND: handle_rewind(body, count); break;
    case PKT3_PRIME_UTCL2: handle_prime_utcl2(body, count); break;
    case PKT3_LOAD_UCONFIG_REG: handle_load_uconfig_reg(body, count); break;
    case PKT3_LOAD_SH_REG: handle_load_sh_reg(body, count); break;
    case PKT3_LOAD_CONTEXT_REG: handle_load_context_reg(body, count); break;
    case PKT3_LOAD_SH_REG_INDEX: handle_load_sh_reg_index(body, count); break;
    case PKT3_SET_CONFIG_REG: handle_set_config_reg(body, count); break;
    case PKT3_SET_CONTEXT_REG: handle_set_context_reg(body, count); break;
    case PKT3_SET_SH_REG: handle_set_sh_reg(body, count); break;
    case PKT3_SET_SH_REG_OFFSET: handle_set_sh_reg_offset(body, count); break;
    case PKT3_SET_UCONFIG_REG: handle_set_uconfig_reg(body, count); break;
    case PKT3_SET_UCONFIG_REG_INDEX: handle_set_uconfig_reg_index(body, count); break;
    case PKT3_LOAD_CONST_RAM: handle_load_const_ram(body, count); break;
    case PKT3_WRITE_CONST_RAM: handle_write_const_ram(body, count); break;
    case PKT3_DUMP_CONST_RAM: handle_dump_const_ram(body, count); break;
    case PKT3_INCREMENT_CE_COUNTER: handle_increment_ce_counter(body, count); break;
    case PKT3_INCREMENT_DE_COUNTER: handle_increment_de_counter(body, count); break;
    case PKT3_WAIT_ON_CE_COUNTER: handle_wait_on_ce_counter(body, count); break;
    case PKT3_FRAME_CONTROL: handle_frame_control(body, count); break;
    case PKT3_HDP_FLUSH: handle_hdp_flush(body, count); break;
    case PKT3_SET_SH_REG_INDEX: handle_set_sh_reg_index(body, count); break;
    case PKT3_LOAD_CONTEXT_REG_INDEX: handle_load_context_reg_index(body, count); break;
    case PKT3_PROTECTED_FENCE_SIGNAL: handle_protected_fence_signal(body, count); break;
    case PKT3_FENCE_WAIT_MULTI: handle_fence_wait_multi(body, count); break;
    default: handle_unknown(opcode, body, count); break;
    }
}

void CommandProcessor::handle_nop(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_set_base(const uint32_t* body, uint32_t count)
{
    if (count < 1)
        return;
    uint32_t base_index = body[0] & 0xF;
    uint64_t address = 0;
    if (count >= 2)
        address = body[1];
    if (count >= 3)
        address |= (uint64_t)body[2] << 32;
    switch (base_index)
    {
    case 0: index_buffer_base_ = address; break;
    case 1: draw_indirect_base_ = address; break;
    case 2: dispatch_indirect_base_ = address; break;
    default: break;
    }
}

void CommandProcessor::handle_clear_state(const uint32_t* body, uint32_t count)
{
    std::memset(regs_.context, 0, sizeof(regs_.context));
    std::memset(regs_.sh, 0, sizeof(regs_.sh));
    std::memset(regs_.uconfig, 0, sizeof(regs_.uconfig));
    std::memset(regs_.config, 0, sizeof(regs_.config));
    index_buffer_base_ = 0;
    draw_indirect_base_ = 0;
    dispatch_indirect_base_ = 0;
    num_instances_ = 1;
    index_base_ = 0;
    index_buffer_size_ = 0;
    graphics_pipeline_dirty_ = true;
    compute_pipeline_dirty_ = true;
    emitGlobalBarrier();
}

void CommandProcessor::handle_index_buffer_size(const uint32_t* body, uint32_t count)
{
    if (count >= 1)
        index_buffer_size_ = body[0];
}

void CommandProcessor::handle_dispatch_direct(const uint32_t* body, uint32_t count)
{
    if (count < 3 || compute_pipeline_ == VK_NULL_HANDLE)
        return;
    if (compute_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_);
        compute_pipeline_dirty_ = false;
    }
    vkCmdDispatch(cmd_, body[0], body[1], body[2]);
}

void CommandProcessor::handle_dispatch_indirect(const uint32_t* body, uint32_t count)
{
    if (compute_pipeline_ == VK_NULL_HANDLE)
        return;
    if (compute_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_);
        compute_pipeline_dirty_ = false;
    }
    VkDeviceSize offset = 0;
    if (count >= 1)
        offset = body[0];
    VkDeviceSize baseOffset = 0;
    VkBuffer buffer = resolveBuffer(dispatch_indirect_base_, &baseOffset);
    if (buffer != VK_NULL_HANDLE)
        vkCmdDispatchIndirect(cmd_, buffer, baseOffset + offset);
}

void CommandProcessor::handle_atomic_mem(const uint32_t* body, uint32_t count)
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void CommandProcessor::handle_occlusion_query(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_set_predication(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_cond_exec(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_pred_exec(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_draw_indirect(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    uint32_t drawCount = 1;
    uint32_t stride = 16;
    VkDeviceSize offset = 0;
    if (count >= 1)
        drawCount = body[0] ? body[0] : 1;
    if (count >= 2)
        offset = body[1];
    if (count >= 3)
        stride = body[2] ? body[2] : 16;
    VkDeviceSize baseOffset = 0;
    VkBuffer buffer = resolveBuffer(draw_indirect_base_, &baseOffset);
    if (buffer != VK_NULL_HANDLE)
        vkCmdDrawIndirect(cmd_, buffer, baseOffset + offset, drawCount, stride);
}

void CommandProcessor::handle_draw_index_indirect(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    if (index_buffer_ != VK_NULL_HANDLE)
        vkCmdBindIndexBuffer(cmd_, index_buffer_, index_offset_, index_type_);
    uint32_t drawCount = 1;
    uint32_t stride = 20;
    VkDeviceSize offset = 0;
    if (count >= 1)
        drawCount = body[0] ? body[0] : 1;
    if (count >= 2)
        offset = body[1];
    if (count >= 3)
        stride = body[2] ? body[2] : 20;
    VkDeviceSize baseOffset = 0;
    VkBuffer buffer = resolveBuffer(draw_indirect_base_, &baseOffset);
    if (buffer != VK_NULL_HANDLE)
        vkCmdDrawIndexedIndirect(cmd_, buffer, baseOffset + offset, drawCount, stride);
}

void CommandProcessor::handle_index_base(const uint32_t* body, uint32_t count)
{
    if (count >= 2)
    {
        index_buffer_base_ = body[0] | ((uint64_t)body[1] << 32);
        index_base_ = 0;
    }
    else if (count >= 1)
    {
        index_base_ = body[0];
    }
}

void CommandProcessor::handle_draw_index_2(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    if (index_buffer_ != VK_NULL_HANDLE)
        vkCmdBindIndexBuffer(cmd_, index_buffer_, index_offset_, index_type_);
    uint32_t indexCount = 0;
    uint32_t instanceCount = num_instances_;
    if (count >= 1)
        indexCount = body[0];
    if (count >= 2)
        instanceCount = body[1] ? body[1] : num_instances_;
    vkCmdDrawIndexed(cmd_, indexCount, instanceCount, index_base_, 0, 0);
}

void CommandProcessor::handle_context_control(const uint32_t* body, uint32_t count)
{
    graphics_pipeline_dirty_ = true;
    compute_pipeline_dirty_ = true;
}

void CommandProcessor::handle_draw_indirect_multi(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    uint32_t drawCount = 1;
    uint32_t stride = 16;
    VkDeviceSize offset = 0;
    if (count >= 1)
        drawCount = body[0] ? body[0] : 1;
    if (count >= 2)
        offset = body[1];
    if (count >= 3)
        stride = body[2] ? body[2] : 16;
    VkDeviceSize baseOffset = 0;
    VkBuffer buffer = resolveBuffer(draw_indirect_base_, &baseOffset);
    if (buffer != VK_NULL_HANDLE)
        vkCmdDrawIndirect(cmd_, buffer, baseOffset + offset, drawCount, stride);
}

void CommandProcessor::handle_draw_index_auto(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    uint32_t vertexCount = 0;
    uint32_t instanceCount = num_instances_;
    if (count >= 1)
        vertexCount = body[0];
    if (count >= 2)
        instanceCount = body[1] ? body[1] : num_instances_;
    vkCmdDraw(cmd_, vertexCount, instanceCount, 0, 0);
}

void CommandProcessor::handle_num_instances(const uint32_t* body, uint32_t count)
{
    if (count >= 1)
        num_instances_ = body[0] ? body[0] : 1;
}

void CommandProcessor::handle_draw_index_multi_auto(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    uint32_t vertexCount = 0;
    uint32_t instanceCount = num_instances_;
    uint32_t firstVertex = 0;
    if (count >= 1)
        vertexCount = body[0];
    if (count >= 2)
        instanceCount = body[1] ? body[1] : num_instances_;
    if (count >= 3)
        firstVertex = body[2];
    vkCmdDraw(cmd_, vertexCount, instanceCount, firstVertex, 0);
}

void CommandProcessor::handle_indirect_buffer_const(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_strmout_buffer_update(const uint32_t* body, uint32_t count)
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT | VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
        VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void CommandProcessor::handle_draw_index_offset_2(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    if (index_buffer_ != VK_NULL_HANDLE)
        vkCmdBindIndexBuffer(cmd_, index_buffer_, index_offset_, index_type_);
    uint32_t indexCount = 0;
    uint32_t instanceCount = num_instances_;
    int32_t vertexOffset = 0;
    uint32_t firstInstance = 0;
    if (count >= 1)
        indexCount = body[0];
    if (count >= 2)
        instanceCount = body[1] ? body[1] : num_instances_;
    if (count >= 3)
        vertexOffset = static_cast<int32_t>(body[2]);
    if (count >= 4)
        firstInstance = body[3];
    vkCmdDrawIndexed(cmd_, indexCount, instanceCount, index_base_, vertexOffset, firstInstance);
}

void CommandProcessor::handle_write_data(const uint32_t* body, uint32_t count)
{
    if (count < 3)
    {
        emitMemoryBarrier();
        return;
    }
    uint64_t dstAddr = body[1] | ((uint64_t)body[2] << 32);
    uint32_t numDwords = (count > 2) ? (count - 2) : 0;
    VkDeviceSize offset = 0;
    VkDeviceSize remaining = 0;
    VkBuffer dstBuffer = resolveBuffer(dstAddr, &offset, &remaining);
    if (dstBuffer != VK_NULL_HANDLE && numDwords > 0)
    {
        VkDeviceSize bytes = numDwords * sizeof(uint32_t);
        if (remaining > 0 && bytes > remaining)
            bytes = remaining;
        std::vector<uint32_t> data(numDwords);
        for (uint32_t i = 0; i < numDwords; ++i)
            data[i] = body[3 + i];
        vkCmdUpdateBuffer(cmd_, dstBuffer, offset, bytes, data.data());
    }
    emitMemoryBarrier();
}


void CommandProcessor::handle_draw_index_indirect_multi(const uint32_t* body, uint32_t count)
{
    if (graphics_pipeline_ == VK_NULL_HANDLE)
        return;
    if (graphics_pipeline_dirty_)
    {
        vkCmdBindPipeline(cmd_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
        graphics_pipeline_dirty_ = false;
    }
    if (index_buffer_ != VK_NULL_HANDLE)
        vkCmdBindIndexBuffer(cmd_, index_buffer_, index_offset_, index_type_);
    uint32_t drawCount = 1;
    uint32_t stride = 20;
    VkDeviceSize offset = 0;
    if (count >= 1)
        drawCount = body[0] ? body[0] : 1;
    if (count >= 2)
        offset = body[1];
    if (count >= 3)
        stride = body[2] ? body[2] : 20;
    VkDeviceSize baseOffset = 0;
    VkBuffer buffer = resolveBuffer(draw_indirect_base_, &baseOffset);
    if (buffer != VK_NULL_HANDLE)
        vkCmdDrawIndexedIndirect(cmd_, buffer, baseOffset + offset, drawCount, stride);
}

void CommandProcessor::handle_mem_semaphore(const uint32_t* body, uint32_t count)
{
    if (event_ != VK_NULL_HANDLE)
        vkCmdSetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    emitGlobalBarrier();
}

void CommandProcessor::handle_wait_reg_mem(const uint32_t* body, uint32_t count)
{
    emitGlobalBarrier();
    if (event_ != VK_NULL_HANDLE)
    {
        vkCmdWaitEvents(cmd_, 1, &event_,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0, nullptr, 0, nullptr, 0, nullptr);
        vkCmdResetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    }
}

void CommandProcessor::handle_indirect_buffer(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
    emitGlobalBarrier();
}

void CommandProcessor::handle_copy_data(const uint32_t* body, uint32_t count)
{
    if (count < 5)
    {
        emitMemoryBarrier();
        return;
    }
    uint64_t srcAddr = body[1] | ((uint64_t)body[2] << 32);
    uint64_t dstAddr = body[3] | ((uint64_t)body[4] << 32);
    VkDeviceSize size = 4;
    if (count >= 6)
        size = body[5] ? body[5] : 4;

    VkDeviceSize srcOffset = 0, dstOffset = 0, srcRemain = 0, dstRemain = 0;
    VkBuffer srcBuffer = resolveBuffer(srcAddr, &srcOffset, &srcRemain);
    VkBuffer dstBuffer = resolveBuffer(dstAddr, &dstOffset, &dstRemain);

    if (srcBuffer != VK_NULL_HANDLE && dstBuffer != VK_NULL_HANDLE)
    {
        if (srcRemain > 0 && size > srcRemain)
            size = srcRemain;
        if (dstRemain > 0 && size > dstRemain)
            size = dstRemain;
        VkBufferCopy region{};
        region.srcOffset = srcOffset;
        region.dstOffset = dstOffset;
        region.size = size;
        vkCmdCopyBuffer(cmd_, srcBuffer, dstBuffer, 1, &region);
    }
    emitMemoryBarrier();
}


void CommandProcessor::handle_pfp_sync_me(const uint32_t* body, uint32_t count)
{
    emitGlobalBarrier();
}

void CommandProcessor::handle_cond_write(const uint32_t* body, uint32_t count)
{
    if (count < 4)
    {
        emitMemoryBarrier();
        return;
    }
    uint64_t dstAddr = body[2] | ((uint64_t)body[3] << 32);
    VkDeviceSize offset = 0;
    VkBuffer dstBuffer = resolveBuffer(dstAddr, &offset);
    if (dstBuffer != VK_NULL_HANDLE && count >= 5)
    {
        uint32_t value = body[4];
        vkCmdUpdateBuffer(cmd_, dstBuffer, offset, sizeof(uint32_t), &value);
    }
    emitMemoryBarrier();
}


void CommandProcessor::handle_event_write(const uint32_t* body, uint32_t count)
{
    if (event_ != VK_NULL_HANDLE)
        vkCmdSetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    emitMemoryBarrier();
}

void CommandProcessor::handle_release_mem(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
    if (event_ != VK_NULL_HANDLE)
        vkCmdSetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    if (count >= 3)
    {
        uint64_t addr = body[1] | ((uint64_t)body[2] << 32);
        VkDeviceSize offset = 0;
        VkBuffer buf = resolveBuffer(addr, &offset);
        if (buf != VK_NULL_HANDLE)
        {
            uint32_t value = (count >= 4) ? body[3] : 0;
            vkCmdUpdateBuffer(cmd_, buf, offset, sizeof(uint32_t), &value);
        }
    }
}


void CommandProcessor::handle_dma_data(const uint32_t* body, uint32_t count)
{
    if (count < 6)
    {
        emitMemoryBarrier();
        return;
    }
    uint64_t srcAddr = body[1] | ((uint64_t)body[2] << 32);
    uint64_t dstAddr = body[3] | ((uint64_t)body[4] << 32);
    uint32_t bytes = body[5];

    VkDeviceSize srcOffset = 0, dstOffset = 0, srcRemain = 0, dstRemain = 0;
    VkBuffer srcBuffer = resolveBuffer(srcAddr, &srcOffset, &srcRemain);
    VkBuffer dstBuffer = resolveBuffer(dstAddr, &dstOffset, &dstRemain);

    if (srcBuffer != VK_NULL_HANDLE && dstBuffer != VK_NULL_HANDLE && bytes > 0)
    {
        VkDeviceSize size = bytes;
        if (srcRemain > 0 && size > srcRemain)
            size = srcRemain;
        if (dstRemain > 0 && size > dstRemain)
            size = dstRemain;
        VkBufferCopy region{};
        region.srcOffset = srcOffset;
        region.dstOffset = dstOffset;
        region.size = size;
        vkCmdCopyBuffer(cmd_, srcBuffer, dstBuffer, 1, &region);
    }
    else if (dstBuffer != VK_NULL_HANDLE && bytes > 0)
    {
        VkDeviceSize size = bytes;
        if (dstRemain > 0 && size > dstRemain)
            size = dstRemain;
        vkCmdFillBuffer(cmd_, dstBuffer, dstOffset, size, 0);
    }
    emitMemoryBarrier();
}


void CommandProcessor::handle_context_reg_rmw(const uint32_t* body, uint32_t count)
{
    if (count < 3)
        return;
    uint32_t offset = body[0];
    uint32_t and_mask = body[1];
    uint32_t or_mask = body[2];
    if (offset < 0x2000)
    {
        regs_.context[offset] = (regs_.context[offset] & and_mask) | or_mask;
        graphics_pipeline_dirty_ = true;
    }
}

void CommandProcessor::handle_acquire_mem(const uint32_t* body, uint32_t count)
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void CommandProcessor::handle_rewind(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_prime_utcl2(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_load_uconfig_reg(const uint32_t* body, uint32_t count)
{
    graphics_pipeline_dirty_ = true;
}

void CommandProcessor::handle_load_sh_reg(const uint32_t* body, uint32_t count)
{
    graphics_pipeline_dirty_ = true;
    compute_pipeline_dirty_ = true;
}

void CommandProcessor::handle_load_context_reg(const uint32_t* body, uint32_t count)
{
    graphics_pipeline_dirty_ = true;
}

void CommandProcessor::handle_load_sh_reg_index(const uint32_t* body, uint32_t count)
{
    graphics_pipeline_dirty_ = true;
    compute_pipeline_dirty_ = true;
}

void CommandProcessor::handle_set_config_reg(const uint32_t* body, uint32_t count)
{
    if (count < 1)
        return;
    uint32_t offset = body[0];
    for (uint32_t i = 1; i <= count; ++i)
    {
        uint32_t idx = offset + (i - 1);
        if (idx < 0x200)
            regs_.config[idx] = body[i];
    }
}

void CommandProcessor::handle_set_context_reg(const uint32_t* body, uint32_t count)
{
    if (count < 1)
        return;
    uint32_t offset = body[0];
    for (uint32_t i = 1; i <= count; ++i)
    {
        uint32_t idx = offset + (i - 1);
        if (idx < 0x2000)
            regs_.context[idx] = body[i];
    }
    graphics_pipeline_dirty_ = true;
}

void CommandProcessor::handle_set_sh_reg(const uint32_t* body, uint32_t count)
{
    if (count < 1)
        return;
    uint32_t offset = body[0];
    for (uint32_t i = 1; i <= count; ++i)
    {
        uint32_t idx = offset + (i - 1);
        if (idx < 0x400)
            regs_.sh[idx] = body[i];
    }
    graphics_pipeline_dirty_ = true;
    compute_pipeline_dirty_ = true;
}

void CommandProcessor::handle_set_sh_reg_offset(const uint32_t* body, uint32_t count)
{
    handle_set_sh_reg(body, count);
}

void CommandProcessor::handle_set_uconfig_reg(const uint32_t* body, uint32_t count)
{
    if (count < 1)
        return;
    uint32_t offset = body[0];
    for (uint32_t i = 1; i <= count; ++i)
    {
        uint32_t idx = offset + (i - 1);
        if (idx < 0x200)
            regs_.uconfig[idx] = body[i];
    }
}

void CommandProcessor::handle_set_uconfig_reg_index(const uint32_t* body, uint32_t count)
{
    handle_set_uconfig_reg(body, count);
}

void CommandProcessor::handle_load_const_ram(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_write_const_ram(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_dump_const_ram(const uint32_t* body, uint32_t count)
{
    emitMemoryBarrier();
}

void CommandProcessor::handle_increment_ce_counter(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_increment_de_counter(const uint32_t* body, uint32_t count)
{
}

void CommandProcessor::handle_wait_on_ce_counter(const uint32_t* body, uint32_t count)
{
    emitGlobalBarrier();
    if (event_ != VK_NULL_HANDLE)
    {
        vkCmdWaitEvents(cmd_, 1, &event_,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0, nullptr, 0, nullptr, 0, nullptr);
    }
}

void CommandProcessor::handle_frame_control(const uint32_t* body, uint32_t count)
{
    emitGlobalBarrier();
    if (event_ != VK_NULL_HANDLE)
        vkCmdSetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

void CommandProcessor::handle_hdp_flush(const uint32_t* body, uint32_t count)
{
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(cmd_,
        VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void CommandProcessor::handle_set_sh_reg_index(const uint32_t* body, uint32_t count)
{
    handle_set_sh_reg(body, count);
}

void CommandProcessor::handle_load_context_reg_index(const uint32_t* body, uint32_t count)
{
    handle_load_context_reg(body, count);
}

void CommandProcessor::handle_protected_fence_signal(const uint32_t* body, uint32_t count)
{
    if (event_ != VK_NULL_HANDLE)
        vkCmdSetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    emitGlobalBarrier();
}

void CommandProcessor::handle_fence_wait_multi(const uint32_t* body, uint32_t count)
{
    if (event_ != VK_NULL_HANDLE)
    {
        vkCmdWaitEvents(cmd_, 1, &event_,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0, nullptr, 0, nullptr, 0, nullptr);
        vkCmdResetEvent(cmd_, event_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    }
    emitGlobalBarrier();
}

void CommandProcessor::handle_unknown(uint32_t opcode, const uint32_t* body, uint32_t count)
{
}
