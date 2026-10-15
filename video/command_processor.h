#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan.h>
#include "memory_tracker.h"

#define PKT3_NOP                                   0x10
#define PKT3_SET_BASE                              0x11
#define PKT3_CLEAR_STATE                           0x12
#define PKT3_INDEX_BUFFER_SIZE                     0x13
#define PKT3_DISPATCH_DIRECT                       0x15
#define PKT3_DISPATCH_INDIRECT                     0x16
#define PKT3_ATOMIC_MEM                            0x1E
#define PKT3_OCCLUSION_QUERY                       0x1F
#define PKT3_SET_PREDICATION                       0x20
#define PKT3_COND_EXEC                             0x22
#define PKT3_PRED_EXEC                             0x23
#define PKT3_DRAW_INDIRECT                         0x24
#define PKT3_DRAW_INDEX_INDIRECT                   0x25
#define PKT3_INDEX_BASE                            0x26
#define PKT3_DRAW_INDEX_2                          0x27
#define PKT3_CONTEXT_CONTROL                       0x28
#define PKT3_DRAW_INDIRECT_MULTI                   0x2C
#define PKT3_DRAW_INDEX_AUTO                       0x2D
#define PKT3_NUM_INSTANCES                         0x2F
#define PKT3_DRAW_INDEX_MULTI_AUTO                 0x30
#define PKT3_INDIRECT_BUFFER_CONST                 0x33
#define PKT3_STRMOUT_BUFFER_UPDATE                 0x34
#define PKT3_DRAW_INDEX_OFFSET_2                   0x35
#define PKT3_WRITE_DATA                            0x37
#define PKT3_DRAW_INDEX_INDIRECT_MULTI             0x38
#define PKT3_MEM_SEMAPHORE                         0x39
#define PKT3_WAIT_REG_MEM                          0x3C
#define PKT3_INDIRECT_BUFFER                       0x3F
#define PKT3_COPY_DATA                             0x40
#define PKT3_PFP_SYNC_ME                           0x42
#define PKT3_COND_WRITE                            0x45
#define PKT3_EVENT_WRITE                           0x46
#define PKT3_RELEASE_MEM                           0x49
#define PKT3_DMA_DATA                              0x50
#define PKT3_CONTEXT_REG_RMW                       0x51
#define PKT3_ACQUIRE_MEM                           0x58
#define PKT3_REWIND                                0x59
#define PKT3_PRIME_UTCL2                           0x5D
#define PKT3_LOAD_UCONFIG_REG                      0x5E
#define PKT3_LOAD_SH_REG                           0x5F
#define PKT3_LOAD_CONTEXT_REG                      0x61
#define PKT3_LOAD_SH_REG_INDEX                     0x63
#define PKT3_SET_CONFIG_REG                        0x68
#define PKT3_SET_CONTEXT_REG                       0x69
#define PKT3_SET_SH_REG                            0x76
#define PKT3_SET_SH_REG_OFFSET                     0x77
#define PKT3_SET_UCONFIG_REG                       0x79
#define PKT3_SET_UCONFIG_REG_INDEX                 0x7A
#define PKT3_LOAD_CONST_RAM                        0x80
#define PKT3_WRITE_CONST_RAM                       0x81
#define PKT3_DUMP_CONST_RAM                        0x83
#define PKT3_INCREMENT_CE_COUNTER                  0x84
#define PKT3_INCREMENT_DE_COUNTER                  0x85
#define PKT3_WAIT_ON_CE_COUNTER                    0x86
#define PKT3_FRAME_CONTROL                         0x90
#define PKT3_HDP_FLUSH                             0x95
#define PKT3_SET_SH_REG_INDEX                      0x9B
#define PKT3_LOAD_CONTEXT_REG_INDEX                0x9F
#define PKT3_PROTECTED_FENCE_SIGNAL                0xD0
#define PKT3_FENCE_WAIT_MULTI                      0xD1

struct GPURegisters {
    uint32_t context[0x2000]{};
    uint32_t sh[0x400]{};
    uint32_t uconfig[0x200]{};
    uint32_t config[0x200]{};
};

class CommandProcessor {
public:
    CommandProcessor(VkDevice device, VkCommandBuffer cmd, VkQueue queue, MemoryTracker* memory);

    void process(const uint32_t* packet);
    void setGraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout);
    void setComputePipeline(VkPipeline pipeline, VkPipelineLayout layout);
    void setDescriptorSets(VkPipelineBindPoint bindPoint, uint32_t firstSet, uint32_t count, const VkDescriptorSet* sets);
    void setIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType type);
    void setVertexBuffers(uint32_t first, uint32_t count, const VkBuffer* buffers, const VkDeviceSize* offsets);

    MemoryTracker* memory() { return memory_; }

private:
    VkDevice device_;
    VkCommandBuffer cmd_;
    VkQueue queue_;
    MemoryTracker* memory_;

    GPURegisters regs_;

    uint64_t index_buffer_base_;
    uint64_t draw_indirect_base_;
    uint64_t dispatch_indirect_base_;
    uint32_t index_buffer_size_;
    uint32_t num_instances_;
    uint32_t index_base_;

    VkPipeline graphics_pipeline_;
    VkPipelineLayout graphics_layout_;
    VkPipeline compute_pipeline_;
    VkPipelineLayout compute_layout_;

    VkBuffer index_buffer_;
    VkDeviceSize index_offset_;
    VkIndexType index_type_;

    bool graphics_pipeline_dirty_;
    bool compute_pipeline_dirty_;

    VkEvent event_;
    VkFence fence_;

    void emitGlobalBarrier();
    void emitMemoryBarrier();

    VkBuffer resolveBuffer(uint64_t gpu_va, VkDeviceSize* out_offset, VkDeviceSize* out_remaining = nullptr) const;

    void handle_nop(const uint32_t* body, uint32_t count);
    void handle_set_base(const uint32_t* body, uint32_t count);
    void handle_clear_state(const uint32_t* body, uint32_t count);
    void handle_index_buffer_size(const uint32_t* body, uint32_t count);
    void handle_dispatch_direct(const uint32_t* body, uint32_t count);
    void handle_dispatch_indirect(const uint32_t* body, uint32_t count);
    void handle_atomic_mem(const uint32_t* body, uint32_t count);
    void handle_occlusion_query(const uint32_t* body, uint32_t count);
    void handle_set_predication(const uint32_t* body, uint32_t count);
    void handle_cond_exec(const uint32_t* body, uint32_t count);
    void handle_pred_exec(const uint32_t* body, uint32_t count);
    void handle_draw_indirect(const uint32_t* body, uint32_t count);
    void handle_draw_index_indirect(const uint32_t* body, uint32_t count);
    void handle_index_base(const uint32_t* body, uint32_t count);
    void handle_draw_index_2(const uint32_t* body, uint32_t count);
    void handle_context_control(const uint32_t* body, uint32_t count);
    void handle_draw_indirect_multi(const uint32_t* body, uint32_t count);
    void handle_draw_index_auto(const uint32_t* body, uint32_t count);
    void handle_num_instances(const uint32_t* body, uint32_t count);
    void handle_draw_index_multi_auto(const uint32_t* body, uint32_t count);
    void handle_indirect_buffer_const(const uint32_t* body, uint32_t count);
    void handle_strmout_buffer_update(const uint32_t* body, uint32_t count);
    void handle_draw_index_offset_2(const uint32_t* body, uint32_t count);
    void handle_write_data(const uint32_t* body, uint32_t count);
    void handle_draw_index_indirect_multi(const uint32_t* body, uint32_t count);
    void handle_mem_semaphore(const uint32_t* body, uint32_t count);
    void handle_wait_reg_mem(const uint32_t* body, uint32_t count);
    void handle_indirect_buffer(const uint32_t* body, uint32_t count);
    void handle_copy_data(const uint32_t* body, uint32_t count);
    void handle_pfp_sync_me(const uint32_t* body, uint32_t count);
    void handle_cond_write(const uint32_t* body, uint32_t count);
    void handle_event_write(const uint32_t* body, uint32_t count);
    void handle_release_mem(const uint32_t* body, uint32_t count);
    void handle_dma_data(const uint32_t* body, uint32_t count);
    void handle_context_reg_rmw(const uint32_t* body, uint32_t count);
    void handle_acquire_mem(const uint32_t* body, uint32_t count);
    void handle_rewind(const uint32_t* body, uint32_t count);
    void handle_prime_utcl2(const uint32_t* body, uint32_t count);
    void handle_load_uconfig_reg(const uint32_t* body, uint32_t count);
    void handle_load_sh_reg(const uint32_t* body, uint32_t count);
    void handle_load_context_reg(const uint32_t* body, uint32_t count);
    void handle_load_sh_reg_index(const uint32_t* body, uint32_t count);
    void handle_set_config_reg(const uint32_t* body, uint32_t count);
    void handle_set_context_reg(const uint32_t* body, uint32_t count);
    void handle_set_sh_reg(const uint32_t* body, uint32_t count);
    void handle_set_sh_reg_offset(const uint32_t* body, uint32_t count);
    void handle_set_uconfig_reg(const uint32_t* body, uint32_t count);
    void handle_set_uconfig_reg_index(const uint32_t* body, uint32_t count);
    void handle_load_const_ram(const uint32_t* body, uint32_t count);
    void handle_write_const_ram(const uint32_t* body, uint32_t count);
    void handle_dump_const_ram(const uint32_t* body, uint32_t count);
    void handle_increment_ce_counter(const uint32_t* body, uint32_t count);
    void handle_increment_de_counter(const uint32_t* body, uint32_t count);
    void handle_wait_on_ce_counter(const uint32_t* body, uint32_t count);
    void handle_frame_control(const uint32_t* body, uint32_t count);
    void handle_hdp_flush(const uint32_t* body, uint32_t count);
    void handle_set_sh_reg_index(const uint32_t* body, uint32_t count);
    void handle_load_context_reg_index(const uint32_t* body, uint32_t count);
    void handle_protected_fence_signal(const uint32_t* body, uint32_t count);
    void handle_fence_wait_multi(const uint32_t* body, uint32_t count);
    void handle_unknown(uint32_t opcode, const uint32_t* body, uint32_t count);
};
