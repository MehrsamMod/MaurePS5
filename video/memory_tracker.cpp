#include "memory_tracker.h"
#include <algorithm>

void MemoryTracker::addBuffer(uint64_t gpu_va, VkBuffer buffer, VkDeviceSize size)
{
    if (buffer == VK_NULL_HANDLE || size == 0)
        return;

    for (auto& m : mappings_)
    {
        if (m.buffer == buffer || m.gpu_va == gpu_va)
        {
            m.gpu_va = gpu_va;
            m.buffer = buffer;
            m.size = size;
            return;
        }
    }

    BufferMapping m{};
    m.gpu_va = gpu_va;
    m.buffer = buffer;
    m.size = size;
    mappings_.push_back(m);
}

void MemoryTracker::removeBuffer(VkBuffer buffer)
{
    mappings_.erase(
        std::remove_if(mappings_.begin(), mappings_.end(),
            [buffer](const BufferMapping& m) { return m.buffer == buffer; }),
        mappings_.end());
}

void MemoryTracker::removeByVA(uint64_t gpu_va)
{
    mappings_.erase(
        std::remove_if(mappings_.begin(), mappings_.end(),
            [gpu_va](const BufferMapping& m) {
                return gpu_va >= m.gpu_va && gpu_va < m.gpu_va + m.size;
            }),
        mappings_.end());
}

void MemoryTracker::clear()
{
    mappings_.clear();
}

VkBuffer MemoryTracker::findBuffer(uint64_t gpu_va, VkDeviceSize* out_offset, VkDeviceSize* out_remaining) const
{
    for (const auto& m : mappings_)
    {
        if (gpu_va >= m.gpu_va && gpu_va < m.gpu_va + m.size)
        {
            VkDeviceSize offset = static_cast<VkDeviceSize>(gpu_va - m.gpu_va);
            if (out_offset)
                *out_offset = offset;
            if (out_remaining)
                *out_remaining = m.size - offset;
            return m.buffer;
        }
    }
    if (out_offset)
        *out_offset = 0;
    if (out_remaining)
        *out_remaining = 0;
    return VK_NULL_HANDLE;
}

bool MemoryTracker::resolve(uint64_t gpu_va, VkBuffer* out_buffer, VkDeviceSize* out_offset, VkDeviceSize* out_remaining) const
{
    VkBuffer buf = findBuffer(gpu_va, out_offset, out_remaining);
    if (out_buffer)
        *out_buffer = buf;
    return buf != VK_NULL_HANDLE;
}
