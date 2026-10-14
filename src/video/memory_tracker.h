#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

struct BufferMapping {
    uint64_t gpu_va;
    VkBuffer buffer;
    VkDeviceSize size;
};

class MemoryTracker {
public:
    void addBuffer(uint64_t gpu_va, VkBuffer buffer, VkDeviceSize size);
    void removeBuffer(VkBuffer buffer);
    void removeByVA(uint64_t gpu_va);
    void clear();

    VkBuffer findBuffer(uint64_t gpu_va, VkDeviceSize* out_offset, VkDeviceSize* out_remaining = nullptr) const;
    bool resolve(uint64_t gpu_va, VkBuffer* out_buffer, VkDeviceSize* out_offset, VkDeviceSize* out_remaining = nullptr) const;

private:
    std::vector<BufferMapping> mappings_;
};
