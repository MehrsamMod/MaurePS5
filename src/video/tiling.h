#pragma once

#include <cstdint>
#include <cstddef>

namespace maure::gpu {

enum class TileMode : uint32_t {
    LinearGeneral = 0,
    LinearAligned = 1,
    OneDTiledThin1 = 2,
    OneDTiledThick = 3,
    TwoDTiledThin1 = 4,
    TwoDTiledThin2 = 5,
    TwoDTiledThin4 = 6,
    TwoDTiledThick = 7,
    TwoDTiledXThick = 8,
    ThreeDTiledThin1 = 9,
    ThreeDTiledThick = 10,
    ThreeDTiledXThick = 11,
    TwoDTiledThinPrt = 12,
    TwoDTiledThickPrt = 13,
    TwoDTiledXThickPrt = 14,
    ThreeDTiledThinPrt = 15,
    ThreeDTiledThickPrt = 16,

    // GFX9+ swizzle modes (block size + type)
    // 256B
    Swizzle256B_S = 17,   // standard
    Swizzle256B_D = 18,   // display
    Swizzle256B_R = 19,   // rotated
    Swizzle256B_Z = 20,   // Z-order
    // 4KB
    Swizzle4KB_S = 21,
    Swizzle4KB_D = 22,
    Swizzle4KB_R = 23,
    Swizzle4KB_Z = 24,
    Swizzle4KB_S_X = 25,  // XOR variant
    Swizzle4KB_D_X = 26,
    Swizzle4KB_R_X = 27,
    Swizzle4KB_Z_X = 28,
    // 64KB
    Swizzle64KB_S = 29,
    Swizzle64KB_D = 30,
    Swizzle64KB_R = 31,
    Swizzle64KB_Z = 32,
    Swizzle64KB_S_T = 33, // thin
    Swizzle64KB_D_T = 34,
    Swizzle64KB_R_T = 35,
    Swizzle64KB_Z_T = 36,
    Swizzle64KB_S_X = 37,
    Swizzle64KB_D_X = 38,
    Swizzle64KB_R_X = 39,
    Swizzle64KB_Z_X = 40,
    Swizzle64KB_S_T_X = 41,
    Swizzle64KB_D_T_X = 42,
    Swizzle64KB_R_T_X = 43,
    Swizzle64KB_Z_T_X = 44,
    // 32KB (some ASICs)
    Swizzle32KB_S = 45,
    Swizzle32KB_D = 46,
    Swizzle32KB_R = 47,
    Swizzle32KB_Z = 48,
    Swizzle32KB_S_X = 49,
    Swizzle32KB_D_X = 50,
    Swizzle32KB_R_X = 51,
    Swizzle32KB_Z_X = 52,
    // Variable block
    SwizzleVar_S = 53,
    SwizzleVar_D = 54,
    SwizzleVar_R = 55,
    SwizzleVar_Z = 56,
    // Legacy aliases (map to S/D variants)
    SwizzleStd256B = Swizzle256B_S,
    SwizzleStd4KB = Swizzle4KB_S,
    SwizzleStd64KB = Swizzle64KB_S,
    SwizzleThin256B = Swizzle256B_S,
    SwizzleThin4KB = Swizzle4KB_S,
    SwizzleThin64KB = Swizzle64KB_S_T,
    SwizzleThick256B = Swizzle256B_Z,
    SwizzleThick4KB = Swizzle4KB_Z,
    SwizzleThick64KB = Swizzle64KB_Z_T,
};

enum class MicroTileMode : uint32_t {
    Displayable = 0,
    NonDisplayable = 1,
    DepthSampleOrder = 2,
    Rotated = 3,
    Thick = 4,
};

enum class PipeConfig : uint32_t {
    P2 = 0,
    P4_8x16 = 1,
    P4_16x16 = 2,
    P4_16x32 = 3,
    P4_32x32 = 4,
    P8_16x16_8x16 = 5,
    P8_16x32_8x16 = 6,
    P8_32x32_8x16 = 7,
    P8_16x32_16x16 = 8,
    P8_32x32_16x16 = 9,
    P8_32x32_16x32 = 10,
    P8_32x64_32x32 = 11,
    P16_32x32_8x16 = 12,
    P16_32x32_16x16 = 13,
};

enum class SwizzleType : uint32_t {
    Standard = 0,
    Display = 1,
    Rotated = 2,
    ZOrder = 3,
};

struct TileInfo {
    uint32_t banks = 4;
    uint32_t bank_width = 1;
    uint32_t bank_height = 1;
    uint32_t macro_aspect = 1;
    uint32_t tile_split = 64;
    PipeConfig pipe_config = PipeConfig::P8_32x32_16x16;
    uint32_t pipe_interleave_bytes = 256;
};

struct SurfaceInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 1;
    uint32_t pitch = 0;
    uint32_t bpp = 0;
    uint32_t num_samples = 1;
    uint32_t num_frags = 1;
    TileMode tile_mode = TileMode::TwoDTiledThin1;
    MicroTileMode micro_mode = MicroTileMode::Displayable;
    TileInfo tile_info{};
    uint32_t pipe_swizzle = 0;
    uint32_t bank_swizzle = 0;
    uint32_t var_block_size = 65536; // for SwizzleVar_*
    bool is_depth = false;
    bool dcc_enabled = false;
};

struct DccInfo {
    uint64_t key_size = 0;
    uint64_t key_base_align = 256;
    uint32_t compress_block_w = 0;
    uint32_t compress_block_h = 0;
    bool independent_64b_blocks = true;
};

struct AddrResult {
    bool ok = false;
    const char* error = nullptr;
};

uint32_t align_up(uint32_t v, uint32_t a);
uint32_t log2_floor(uint32_t v);
uint32_t num_pipes(PipeConfig cfg);
uint32_t tile_thickness(TileMode mode);
bool is_linear_mode(TileMode mode);
bool is_prt_mode(TileMode mode);
bool is_swizzle_mode(TileMode mode);
uint32_t swizzle_block_bytes(TileMode mode, uint32_t var_block_size = 65536);
SwizzleType swizzle_type(TileMode mode);
bool swizzle_has_xor(TileMode mode);
bool swizzle_is_thin(TileMode mode);

uint32_t compute_pitch_elements(const SurfaceInfo& info);
size_t surface_size_bytes(const SurfaceInfo& info);

size_t addr_from_coord(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z = 0, uint32_t sample = 0);
size_t linear_offset(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z = 0);

AddrResult macro_tiled_2d_to_linear(const SurfaceInfo& info,
                                    const void* tiled_src, size_t tiled_size,
                                    void* linear_dst, size_t linear_size);

AddrResult linear_to_macro_tiled_2d(const SurfaceInfo& info,
                                    const void* linear_src, size_t linear_size,
                                    void* tiled_dst, size_t tiled_size);

AddrResult detile(const SurfaceInfo& info,
                  const void* tiled_src, size_t tiled_size,
                  void* linear_dst, size_t linear_size);

AddrResult tile(const SurfaceInfo& info,
                const void* linear_src, size_t linear_size,
                void* tiled_dst, size_t tiled_size);

DccInfo compute_dcc_info(const SurfaceInfo& info);
size_t dcc_key_offset(const SurfaceInfo& info, const DccInfo& dcc, uint32_t x, uint32_t y, uint32_t z = 0);

AddrResult dcc_decompress_to_linear(const SurfaceInfo& info,
                                    const void* dcc_keys, size_t dcc_key_size,
                                    const void* color_data, size_t color_size,
                                    void* linear_dst, size_t linear_size);

constexpr uint8_t DCC_KEY_UNCOMPRESSED = 0xFF;
constexpr uint8_t DCC_KEY_ZERO = 0x00;

} // namespace maure::gpu
