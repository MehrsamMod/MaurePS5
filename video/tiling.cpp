#include "tiling.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace maure::gpu {

namespace {

constexpr uint32_t kMicroW = 8;
constexpr uint32_t kMicroH = 8;

uint32_t bpe(uint32_t bpp) {
    return (bpp + 7u) / 8u;
}

uint32_t micro_bytes(uint32_t bpp) {
    return kMicroW * kMicroH * bpe(bpp);
}

uint32_t micro_index_display(uint32_t x, uint32_t y) {
    const uint32_t lx = x & 7u;
    const uint32_t ly = y & 7u;
    return ((ly & 6u) << 2) | ((lx & 6u) << 1) | ((ly & 1u) << 1) | (lx & 1u);
}

uint32_t micro_index_rotated(uint32_t x, uint32_t y) {
    const uint32_t lx = x & 7u;
    const uint32_t ly = y & 7u;
    return ((lx & 6u) << 2) | ((ly & 6u) << 1) | ((lx & 1u) << 1) | (ly & 1u);
}

uint32_t micro_index_z(uint32_t x, uint32_t y) {
    // Morton Z within 8x8
    const uint32_t lx = x & 7u;
    const uint32_t ly = y & 7u;
    uint32_t m = 0;
    for (uint32_t b = 0; b < 3; ++b) {
        m |= ((lx >> b) & 1u) << (2 * b);
        m |= ((ly >> b) & 1u) << (2 * b + 1);
    }
    return m;
}

uint32_t micro_index_non_display(uint32_t x, uint32_t y) {
    return ((y & 7u) << 3) | (x & 7u);
}

uint32_t micro_index(MicroTileMode mode, uint32_t x, uint32_t y) {
    switch (mode) {
    case MicroTileMode::Displayable:
        return micro_index_display(x, y);
    case MicroTileMode::Rotated:
        return micro_index_rotated(x, y);
    case MicroTileMode::DepthSampleOrder:
    case MicroTileMode::Thick:
        return micro_index_z(x, y);
    default:
        return micro_index_non_display(x, y);
    }
}

uint32_t pipe_count(PipeConfig cfg) {
    switch (cfg) {
    case PipeConfig::P2:
        return 2;
    case PipeConfig::P4_8x16:
    case PipeConfig::P4_16x16:
    case PipeConfig::P4_16x32:
    case PipeConfig::P4_32x32:
        return 4;
    case PipeConfig::P16_32x32_8x16:
    case PipeConfig::P16_32x32_16x16:
        return 16;
    default:
        return 8;
    }
}

uint32_t thickness_of(TileMode mode) {
    switch (mode) {
    case TileMode::OneDTiledThick:
    case TileMode::TwoDTiledThick:
    case TileMode::TwoDTiledThickPrt:
    case TileMode::ThreeDTiledThick:
    case TileMode::ThreeDTiledThickPrt:
        return 4;
    case TileMode::TwoDTiledXThick:
    case TileMode::TwoDTiledXThickPrt:
    case TileMode::ThreeDTiledXThick:
        return 8;
    default:
        return 1;
    }
}

uint32_t thin_factor(TileMode mode) {
    switch (mode) {
    case TileMode::TwoDTiledThin2:
        return 2;
    case TileMode::TwoDTiledThin4:
        return 4;
    default:
        return 1;
    }
}

uint32_t block_bytes_of(TileMode mode, uint32_t var_block) {
    switch (mode) {
    case TileMode::Swizzle256B_S:
    case TileMode::Swizzle256B_D:
    case TileMode::Swizzle256B_R:
    case TileMode::Swizzle256B_Z:
        return 256;
    case TileMode::Swizzle4KB_S:
    case TileMode::Swizzle4KB_D:
    case TileMode::Swizzle4KB_R:
    case TileMode::Swizzle4KB_Z:
    case TileMode::Swizzle4KB_S_X:
    case TileMode::Swizzle4KB_D_X:
    case TileMode::Swizzle4KB_R_X:
    case TileMode::Swizzle4KB_Z_X:
        return 4096;
    case TileMode::Swizzle32KB_S:
    case TileMode::Swizzle32KB_D:
    case TileMode::Swizzle32KB_R:
    case TileMode::Swizzle32KB_Z:
    case TileMode::Swizzle32KB_S_X:
    case TileMode::Swizzle32KB_D_X:
    case TileMode::Swizzle32KB_R_X:
    case TileMode::Swizzle32KB_Z_X:
        return 32768;
    case TileMode::Swizzle64KB_S:
    case TileMode::Swizzle64KB_D:
    case TileMode::Swizzle64KB_R:
    case TileMode::Swizzle64KB_Z:
    case TileMode::Swizzle64KB_S_T:
    case TileMode::Swizzle64KB_D_T:
    case TileMode::Swizzle64KB_R_T:
    case TileMode::Swizzle64KB_Z_T:
    case TileMode::Swizzle64KB_S_X:
    case TileMode::Swizzle64KB_D_X:
    case TileMode::Swizzle64KB_R_X:
    case TileMode::Swizzle64KB_Z_X:
    case TileMode::Swizzle64KB_S_T_X:
    case TileMode::Swizzle64KB_D_T_X:
    case TileMode::Swizzle64KB_R_T_X:
    case TileMode::Swizzle64KB_Z_T_X:
        return 65536;
    case TileMode::SwizzleVar_S:
    case TileMode::SwizzleVar_D:
    case TileMode::SwizzleVar_R:
    case TileMode::SwizzleVar_Z:
        return var_block ? var_block : 65536;
    default:
        return 0;
    }
}

SwizzleType type_of(TileMode mode) {
    switch (mode) {
    case TileMode::Swizzle256B_D:
    case TileMode::Swizzle4KB_D:
    case TileMode::Swizzle4KB_D_X:
    case TileMode::Swizzle32KB_D:
    case TileMode::Swizzle32KB_D_X:
    case TileMode::Swizzle64KB_D:
    case TileMode::Swizzle64KB_D_T:
    case TileMode::Swizzle64KB_D_X:
    case TileMode::Swizzle64KB_D_T_X:
    case TileMode::SwizzleVar_D:
        return SwizzleType::Display;
    case TileMode::Swizzle256B_R:
    case TileMode::Swizzle4KB_R:
    case TileMode::Swizzle4KB_R_X:
    case TileMode::Swizzle32KB_R:
    case TileMode::Swizzle32KB_R_X:
    case TileMode::Swizzle64KB_R:
    case TileMode::Swizzle64KB_R_T:
    case TileMode::Swizzle64KB_R_X:
    case TileMode::Swizzle64KB_R_T_X:
    case TileMode::SwizzleVar_R:
        return SwizzleType::Rotated;
    case TileMode::Swizzle256B_Z:
    case TileMode::Swizzle4KB_Z:
    case TileMode::Swizzle4KB_Z_X:
    case TileMode::Swizzle32KB_Z:
    case TileMode::Swizzle32KB_Z_X:
    case TileMode::Swizzle64KB_Z:
    case TileMode::Swizzle64KB_Z_T:
    case TileMode::Swizzle64KB_Z_X:
    case TileMode::Swizzle64KB_Z_T_X:
    case TileMode::SwizzleVar_Z:
        return SwizzleType::ZOrder;
    default:
        return SwizzleType::Standard;
    }
}

bool has_xor(TileMode mode) {
    switch (mode) {
    case TileMode::Swizzle4KB_S_X:
    case TileMode::Swizzle4KB_D_X:
    case TileMode::Swizzle4KB_R_X:
    case TileMode::Swizzle4KB_Z_X:
    case TileMode::Swizzle32KB_S_X:
    case TileMode::Swizzle32KB_D_X:
    case TileMode::Swizzle32KB_R_X:
    case TileMode::Swizzle32KB_Z_X:
    case TileMode::Swizzle64KB_S_X:
    case TileMode::Swizzle64KB_D_X:
    case TileMode::Swizzle64KB_R_X:
    case TileMode::Swizzle64KB_Z_X:
    case TileMode::Swizzle64KB_S_T_X:
    case TileMode::Swizzle64KB_D_T_X:
    case TileMode::Swizzle64KB_R_T_X:
    case TileMode::Swizzle64KB_Z_T_X:
        return true;
    default:
        return false;
    }
}

bool is_thin_swizzle(TileMode mode) {
    switch (mode) {
    case TileMode::Swizzle64KB_S_T:
    case TileMode::Swizzle64KB_D_T:
    case TileMode::Swizzle64KB_R_T:
    case TileMode::Swizzle64KB_Z_T:
    case TileMode::Swizzle64KB_S_T_X:
    case TileMode::Swizzle64KB_D_T_X:
    case TileMode::Swizzle64KB_R_T_X:
    case TileMode::Swizzle64KB_Z_T_X:
        return true;
    default:
        return false;
    }
}

void macro_tile_pixels(const SurfaceInfo& info, uint32_t& out_w, uint32_t& out_h) {
    const uint32_t bw = std::max(1u, info.tile_info.bank_width);
    const uint32_t bh = std::max(1u, info.tile_info.bank_height);
    const uint32_t aspect = std::max(1u, info.tile_info.macro_aspect);
    const uint32_t banks = std::max(1u, info.tile_info.banks);
    const uint32_t pipes = pipe_count(info.tile_info.pipe_config);
    uint32_t macro_w_mt = pipes * bw * aspect;
    uint32_t macro_h_mt = (banks / aspect) * bh;
    if (macro_h_mt == 0)
        macro_h_mt = 1;
    out_w = macro_w_mt * kMicroW;
    out_h = macro_h_mt * kMicroH;
}

uint32_t compute_pipe(const SurfaceInfo& info, uint32_t x, uint32_t y) {
    const uint32_t pipes = pipe_count(info.tile_info.pipe_config);
    if (pipes <= 1)
        return 0;
    uint32_t pipe = 0;
    if (pipes >= 2)
        pipe |= ((x >> 3) ^ (y >> 3)) & 1u;
    if (pipes >= 4)
        pipe |= (((x >> 4) ^ (y >> 4)) & 1u) << 1;
    if (pipes >= 8)
        pipe |= (((x >> 5) ^ (y >> 5)) & 1u) << 2;
    if (pipes >= 16)
        pipe |= (((x >> 6) ^ (y >> 6)) & 1u) << 3;
    pipe ^= info.pipe_swizzle & (pipes - 1u);
    return pipe & (pipes - 1u);
}

uint32_t compute_bank(const SurfaceInfo& info, uint32_t x, uint32_t y) {
    const uint32_t banks = std::max(1u, info.tile_info.banks);
    if (banks <= 1)
        return 0;
    uint32_t bank = 0;
    if (banks >= 2)
        bank |= ((x >> 3) ^ (y >> 4)) & 1u;
    if (banks >= 4)
        bank |= (((x >> 4) ^ (y >> 3)) & 1u) << 1;
    if (banks >= 8)
        bank |= (((x >> 5) ^ (y >> 5)) & 1u) << 2;
    if (banks >= 16)
        bank |= (((x >> 6) ^ (y >> 6)) & 1u) << 3;
    bank ^= info.bank_swizzle & (banks - 1u);
    return bank & (banks - 1u);
}

size_t offset_linear(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z) {
    const uint32_t pitch = info.pitch ? info.pitch : info.width;
    const uint32_t bytes = bpe(info.bpp);
    return (static_cast<size_t>(z) * info.height + y) * pitch * bytes + static_cast<size_t>(x) * bytes;
}

size_t offset_1d(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z) {
    const uint32_t bytes = bpe(info.bpp);
    const uint32_t thick = thickness_of(info.tile_mode);
    const uint32_t mt_bytes = micro_bytes(info.bpp) * thick;
    const uint32_t pitch = info.pitch ? info.pitch : align_up(info.width, kMicroW);
    const uint32_t tile_x = x / kMicroW;
    const uint32_t tile_y = y / kMicroH;
    const uint32_t tpr = pitch / kMicroW;
    const uint32_t z_tile = z / thick;
    const uint32_t z_in = z % thick;
    const size_t ti =
        static_cast<size_t>(z_tile) * ((info.height + kMicroH - 1) / kMicroH) * tpr +
        static_cast<size_t>(tile_y) * tpr + tile_x;
    return ti * mt_bytes + static_cast<size_t>(z_in) * micro_bytes(info.bpp) +
           static_cast<size_t>(micro_index(info.micro_mode, x, y)) * bytes;
}

size_t offset_2d_macro(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z, uint32_t sample) {
    const uint32_t bytes = bpe(info.bpp);
    const uint32_t thick = thickness_of(info.tile_mode);
    const uint32_t factor = thin_factor(info.tile_mode);
    const uint32_t mt_bytes = micro_bytes(info.bpp) * thick * factor;
    const uint32_t pipes = pipe_count(info.tile_info.pipe_config);
    const uint32_t banks = std::max(1u, info.tile_info.banks);

    uint32_t macro_w = 0, macro_h = 0;
    macro_tile_pixels(info, macro_w, macro_h);
    if (macro_w == 0)
        macro_w = 32;
    if (macro_h == 0)
        macro_h = 16;

    const uint32_t pitch = info.pitch ? info.pitch : compute_pitch_elements(info);
    const uint32_t z_tile = z / thick;
    const uint32_t z_in = z % thick;
    const uint32_t macro_x = x / macro_w;
    const uint32_t macro_y = y / macro_h;
    const uint32_t macros_per_row = std::max(1u, pitch / macro_w);
    const uint32_t local_x = x % macro_w;
    const uint32_t local_y = y % macro_h;
    const uint32_t micro_x = local_x / kMicroW;
    const uint32_t micro_y = local_y / kMicroH;
    const uint32_t micros_per_macro_row = macro_w / kMicroW;
    const uint32_t micro_in_macro = micro_y * micros_per_macro_row + micro_x;
    const uint32_t pipe = compute_pipe(info, x, y);
    const uint32_t bank = compute_bank(info, x, y);

    const size_t macro_index =
        static_cast<size_t>(z_tile) * ((info.height + macro_h - 1u) / macro_h) * macros_per_row +
        static_cast<size_t>(macro_y) * macros_per_row + macro_x;

    const uint32_t micros_per_macro = (macro_w / kMicroW) * (macro_h / kMicroH);
    const size_t macro_bytes = static_cast<size_t>(micros_per_macro) * mt_bytes;
    const uint32_t total_pb = pipes * banks;
    size_t slice = 0;
    if (total_pb > 0)
        slice = (static_cast<size_t>(bank) * pipes + pipe) * (macro_bytes / total_pb);

    const uint32_t pix = micro_index(info.micro_mode, local_x, local_y);
    size_t off = macro_index * macro_bytes + slice +
                 static_cast<size_t>(micro_in_macro) * mt_bytes +
                 static_cast<size_t>(z_in) * micro_bytes(info.bpp) * factor +
                 static_cast<size_t>(pix) * bytes;

    if (info.num_samples > 1)
        off += static_cast<size_t>(sample % info.num_samples) * (mt_bytes / std::max(1u, info.num_samples));
    return off;
}

uint32_t morton2d(uint32_t x, uint32_t y, uint32_t bits) {
    uint32_t m = 0;
    for (uint32_t b = 0; b < bits; ++b) {
        m |= ((x >> b) & 1u) << (2 * b);
        m |= ((y >> b) & 1u) << (2 * b + 1);
    }
    return m;
}

uint32_t in_block_index(SwizzleType st, uint32_t lx, uint32_t ly, uint32_t bw, uint32_t bh) {
    switch (st) {
    case SwizzleType::Display:
        // row-major small tiles 8x8 inside block
        return (ly / 8u) * (bw / 8u) * 64u + (lx / 8u) * 64u + micro_index_display(lx, ly);
    case SwizzleType::Rotated:
        return (ly / 8u) * (bw / 8u) * 64u + (lx / 8u) * 64u + micro_index_rotated(lx, ly);
    case SwizzleType::ZOrder:
        return morton2d(lx, ly, 16);
    case SwizzleType::Standard:
    default:
        return morton2d(lx, ly, 16);
    }
}

size_t offset_swizzle(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z, uint32_t sample) {
    const uint32_t block = block_bytes_of(info.tile_mode, info.var_block_size);
    const uint32_t bytes = bpe(info.bpp);
    if (block == 0)
        return offset_2d_macro(info, x, y, z, sample);

    const SwizzleType st = type_of(info.tile_mode);
    const bool xor_on = has_xor(info.tile_mode);

    const uint32_t elems = std::max(1u, block / bytes);
    uint32_t bw = 8;
    while (bw * bw < elems && bw < 1024)
        bw *= 2;
    uint32_t bh = std::max(8u, elems / bw);
    if (is_thin_swizzle(info.tile_mode)) {
        // thin: wider aspect
        bw = std::max(bw, bh * 2);
        bh = std::max(8u, elems / bw);
    }

    const uint32_t pitch = info.pitch ? info.pitch : align_up(info.width, bw);
    uint32_t lx = x % bw;
    uint32_t ly = y % bh;
    uint32_t bx = x / bw;
    uint32_t by = y / bh;

    if (xor_on) {
        // pipe/bank-style xor on block indices
        bx ^= (by & 1u);
        lx ^= (ly & 7u);
    }

    const uint32_t blocks_per_row = std::max(1u, pitch / bw);
    const size_t block_index =
        static_cast<size_t>(z) * ((info.height + bh - 1u) / bh) * blocks_per_row +
        static_cast<size_t>(by) * blocks_per_row + bx;

    uint32_t inside = in_block_index(st, lx, ly, bw, bh);
    // clamp inside to element count
    const uint32_t max_el = block / bytes;
    if (inside >= max_el)
        inside %= max_el;

    return block_index * block + static_cast<size_t>(inside) * bytes +
           static_cast<size_t>(sample) * bytes;
}

} // namespace

uint32_t align_up(uint32_t v, uint32_t a) {
    if (a <= 1)
        return v;
    return (v + a - 1u) / a * a;
}

uint32_t log2_floor(uint32_t v) {
    uint32_t r = 0;
    while (v > 1u) {
        v >>= 1;
        ++r;
    }
    return r;
}

uint32_t num_pipes(PipeConfig cfg) {
    return pipe_count(cfg);
}

uint32_t tile_thickness(TileMode mode) {
    return thickness_of(mode);
}

bool is_linear_mode(TileMode mode) {
    return mode == TileMode::LinearGeneral || mode == TileMode::LinearAligned;
}

bool is_prt_mode(TileMode mode) {
    switch (mode) {
    case TileMode::TwoDTiledThinPrt:
    case TileMode::TwoDTiledThickPrt:
    case TileMode::TwoDTiledXThickPrt:
    case TileMode::ThreeDTiledThinPrt:
    case TileMode::ThreeDTiledThickPrt:
        return true;
    default:
        return false;
    }
}

bool is_swizzle_mode(TileMode mode) {
    return block_bytes_of(mode, 65536) != 0;
}

uint32_t swizzle_block_bytes(TileMode mode, uint32_t var_block_size) {
    return block_bytes_of(mode, var_block_size);
}

SwizzleType swizzle_type(TileMode mode) {
    return type_of(mode);
}

bool swizzle_has_xor(TileMode mode) {
    return has_xor(mode);
}

bool swizzle_is_thin(TileMode mode) {
    return is_thin_swizzle(mode);
}

uint32_t compute_pitch_elements(const SurfaceInfo& info) {
    if (info.pitch)
        return info.pitch;
    if (is_linear_mode(info.tile_mode)) {
        if (info.tile_mode == TileMode::LinearAligned)
            return align_up(info.width, 8);
        return info.width;
    }
    if (info.tile_mode == TileMode::OneDTiledThin1 || info.tile_mode == TileMode::OneDTiledThick)
        return align_up(info.width, kMicroW);

    if (is_swizzle_mode(info.tile_mode)) {
        const uint32_t block = block_bytes_of(info.tile_mode, info.var_block_size);
        const uint32_t bytes = bpe(info.bpp);
        const uint32_t elems = std::max(1u, block / bytes);
        uint32_t bw = 8;
        while (bw * bw < elems && bw < 1024)
            bw *= 2;
        return align_up(info.width, bw);
    }

    uint32_t mw = 0, mh = 0;
    macro_tile_pixels(info, mw, mh);
    if (mw == 0)
        mw = 32;
    return align_up(info.width, mw);
}

size_t surface_size_bytes(const SurfaceInfo& info) {
    SurfaceInfo s = info;
    if (!s.depth)
        s.depth = 1;
    s.pitch = compute_pitch_elements(s);
    const uint32_t bytes = bpe(s.bpp);
    const uint32_t thick = thickness_of(s.tile_mode);

    if (is_linear_mode(s.tile_mode))
        return static_cast<size_t>(s.pitch) * s.height * s.depth * bytes * s.num_samples;

    if (s.tile_mode == TileMode::OneDTiledThin1 || s.tile_mode == TileMode::OneDTiledThick) {
        const uint32_t tw = s.pitch / kMicroW;
        const uint32_t th = (s.height + kMicroH - 1u) / kMicroH;
        const uint32_t zd = (s.depth + thick - 1u) / thick;
        return static_cast<size_t>(tw) * th * zd * micro_bytes(s.bpp) * thick * s.num_samples;
    }

    if (is_swizzle_mode(s.tile_mode)) {
        const uint32_t block = block_bytes_of(s.tile_mode, s.var_block_size);
        const uint32_t elems = std::max(1u, block / bytes);
        uint32_t bw = 8;
        while (bw * bw < elems && bw < 1024)
            bw *= 2;
        const uint32_t bh = std::max(8u, elems / bw);
        const uint32_t bx = s.pitch / bw;
        const uint32_t by = (s.height + bh - 1u) / bh;
        return static_cast<size_t>(bx) * by * s.depth * block * s.num_samples;
    }

    uint32_t mw = 0, mh = 0;
    macro_tile_pixels(s, mw, mh);
    if (mw == 0)
        mw = 32;
    if (mh == 0)
        mh = 16;
    const uint32_t factor = thin_factor(s.tile_mode);
    const uint32_t mx = s.pitch / mw;
    const uint32_t my = (s.height + mh - 1u) / mh;
    const uint32_t mz = (s.depth + thick - 1u) / thick;
    const uint32_t micros = (mw / kMicroW) * (mh / kMicroH);
    return static_cast<size_t>(mx) * my * mz * micros * micro_bytes(s.bpp) * thick * factor * s.num_samples;
}

size_t addr_from_coord(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z, uint32_t sample) {
    if (is_linear_mode(info.tile_mode))
        return offset_linear(info, x, y, z) + static_cast<size_t>(sample) * bpe(info.bpp);
    if (info.tile_mode == TileMode::OneDTiledThin1 || info.tile_mode == TileMode::OneDTiledThick)
        return offset_1d(info, x, y, z);
    if (is_swizzle_mode(info.tile_mode))
        return offset_swizzle(info, x, y, z, sample);
    return offset_2d_macro(info, x, y, z, sample);
}

size_t linear_offset(const SurfaceInfo& info, uint32_t x, uint32_t y, uint32_t z) {
    SurfaceInfo l = info;
    l.tile_mode = TileMode::LinearGeneral;
    l.pitch = info.width;
    return offset_linear(l, x, y, z);
}

static AddrResult copy_xy(const SurfaceInfo& info, bool to_linear,
                          const void* src, size_t src_size,
                          void* dst, size_t dst_size) {
    AddrResult r;
    if (!src || !dst) {
        r.error = "null pointer";
        return r;
    }
    if (!info.width || !info.height || !info.bpp) {
        r.error = "invalid surface";
        return r;
    }

    SurfaceInfo s = info;
    if (!s.depth)
        s.depth = 1;
    s.pitch = compute_pitch_elements(s);

    SurfaceInfo lin = s;
    lin.tile_mode = TileMode::LinearGeneral;
    lin.pitch = s.width;

    const size_t need_tiled = surface_size_bytes(s);
    const size_t need_lin = surface_size_bytes(lin);

    if (to_linear) {
        if (src_size < need_tiled) {
            r.error = "tiled buffer too small";
            return r;
        }
        if (dst_size < need_lin) {
            r.error = "linear buffer too small";
            return r;
        }
    } else {
        if (src_size < need_lin) {
            r.error = "linear buffer too small";
            return r;
        }
        if (dst_size < need_tiled) {
            r.error = "tiled buffer too small";
            return r;
        }
        std::memset(dst, 0, need_tiled);
    }

    if (is_linear_mode(s.tile_mode)) {
        std::memcpy(dst, src, std::min({src_size, dst_size, need_lin}));
        r.ok = true;
        return r;
    }

    const uint8_t* in = static_cast<const uint8_t*>(src);
    uint8_t* out = static_cast<uint8_t*>(dst);
    const uint32_t bytes = bpe(s.bpp);

    for (uint32_t z = 0; z < s.depth; ++z) {
        for (uint32_t y = 0; y < s.height; ++y) {
            for (uint32_t x = 0; x < s.width; ++x) {
                const size_t t_off = addr_from_coord(s, x, y, z, 0);
                const size_t l_off = linear_offset(s, x, y, z);
                if (to_linear) {
                    if (t_off + bytes > src_size || l_off + bytes > dst_size) {
                        r.error = "offset out of range";
                        return r;
                    }
                    std::memcpy(out + l_off, in + t_off, bytes);
                } else {
                    if (l_off + bytes > src_size || t_off + bytes > dst_size) {
                        r.error = "offset out of range";
                        return r;
                    }
                    std::memcpy(out + t_off, in + l_off, bytes);
                }
            }
        }
    }
    r.ok = true;
    return r;
}

AddrResult macro_tiled_2d_to_linear(const SurfaceInfo& info,
                                    const void* tiled_src, size_t tiled_size,
                                    void* linear_dst, size_t linear_size) {
    SurfaceInfo s = info;
    s.tile_mode = TileMode::TwoDTiledThin1;
    return copy_xy(s, true, tiled_src, tiled_size, linear_dst, linear_size);
}

AddrResult linear_to_macro_tiled_2d(const SurfaceInfo& info,
                                    const void* linear_src, size_t linear_size,
                                    void* tiled_dst, size_t tiled_size) {
    SurfaceInfo s = info;
    s.tile_mode = TileMode::TwoDTiledThin1;
    return copy_xy(s, false, linear_src, linear_size, tiled_dst, tiled_size);
}

AddrResult detile(const SurfaceInfo& info,
                  const void* tiled_src, size_t tiled_size,
                  void* linear_dst, size_t linear_size) {
    return copy_xy(info, true, tiled_src, tiled_size, linear_dst, linear_size);
}

AddrResult tile(const SurfaceInfo& info,
                const void* linear_src, size_t linear_size,
                void* tiled_dst, size_t tiled_size) {
    return copy_xy(info, false, linear_src, linear_size, tiled_dst, tiled_size);
}

DccInfo compute_dcc_info(const SurfaceInfo& info) {
    DccInfo d{};
    const uint32_t bytes = bpe(info.bpp);
    switch (bytes) {
    case 1:
        d.compress_block_w = 16;
        d.compress_block_h = 16;
        break;
    case 2:
        d.compress_block_w = 16;
        d.compress_block_h = 8;
        break;
    case 4:
        d.compress_block_w = 8;
        d.compress_block_h = 8;
        break;
    case 8:
        d.compress_block_w = 8;
        d.compress_block_h = 4;
        break;
    case 16:
        d.compress_block_w = 4;
        d.compress_block_h = 4;
        break;
    default:
        d.compress_block_w = 8;
        d.compress_block_h = 8;
        break;
    }
    const uint32_t pitch = compute_pitch_elements(info);
    const uint32_t bw = (pitch + d.compress_block_w - 1u) / d.compress_block_w;
    const uint32_t bh = (info.height + d.compress_block_h - 1u) / d.compress_block_h;
    const uint32_t bd = info.depth ? info.depth : 1u;
    d.key_size = static_cast<uint64_t>(bw) * bh * bd;
    d.key_base_align = 256;
    d.independent_64b_blocks = true;
    return d;
}

size_t dcc_key_offset(const SurfaceInfo& info, const DccInfo& dcc, uint32_t x, uint32_t y, uint32_t z) {
    const uint32_t pitch = compute_pitch_elements(info);
    const uint32_t bw = (pitch + dcc.compress_block_w - 1u) / dcc.compress_block_w;
    const uint32_t bx = x / dcc.compress_block_w;
    const uint32_t by = y / dcc.compress_block_h;
    return (static_cast<size_t>(z) * ((info.height + dcc.compress_block_h - 1u) / dcc.compress_block_h) + by) * bw + bx;
}

AddrResult dcc_decompress_to_linear(const SurfaceInfo& info,
                                    const void* dcc_keys, size_t dcc_key_size,
                                    const void* color_data, size_t color_size,
                                    void* linear_dst, size_t linear_size) {
    AddrResult r;
    if (!dcc_keys || !color_data || !linear_dst) {
        r.error = "null pointer";
        return r;
    }

    SurfaceInfo s = info;
    if (!s.depth)
        s.depth = 1;
    s.pitch = compute_pitch_elements(s);
    const DccInfo dcc = compute_dcc_info(s);
    if (dcc_key_size < dcc.key_size) {
        r.error = "dcc key buffer too small";
        return r;
    }

    SurfaceInfo lin = s;
    lin.tile_mode = TileMode::LinearGeneral;
    lin.pitch = s.width;
    const size_t need_lin = surface_size_bytes(lin);
    if (linear_size < need_lin) {
        r.error = "linear buffer too small";
        return r;
    }

    std::vector<uint8_t> scratch;
    const uint8_t* src_color = static_cast<const uint8_t*>(color_data);
    if (!is_linear_mode(s.tile_mode)) {
        scratch.resize(need_lin);
        auto dr = detile(s, color_data, color_size, scratch.data(), scratch.size());
        if (!dr.ok) {
            r.error = dr.error ? dr.error : "detile failed";
            return r;
        }
        src_color = scratch.data();
        s.tile_mode = TileMode::LinearGeneral;
        s.pitch = s.width;
    } else if (color_size < need_lin) {
        r.error = "color buffer too small";
        return r;
    }

    const uint8_t* keys = static_cast<const uint8_t*>(dcc_keys);
    uint8_t* out = static_cast<uint8_t*>(linear_dst);
    const uint32_t bytes = bpe(s.bpp);

    for (uint32_t z = 0; z < s.depth; ++z) {
        for (uint32_t y = 0; y < s.height; ++y) {
            for (uint32_t x = 0; x < s.width; ++x) {
                const size_t koff = dcc_key_offset(s, dcc, x, y, z);
                const uint8_t key = keys[koff];
                const size_t lo = linear_offset(s, x, y, z);
                if (key == DCC_KEY_UNCOMPRESSED)
                    std::memcpy(out + lo, src_color + lo, bytes);
                else if (key == DCC_KEY_ZERO)
                    std::memset(out + lo, 0, bytes);
                else {
                    const uint32_t bx = (x / dcc.compress_block_w) * dcc.compress_block_w;
                    const uint32_t by = (y / dcc.compress_block_h) * dcc.compress_block_h;
                    std::memcpy(out + lo, src_color + linear_offset(s, bx, by, z), bytes);
                }
            }
        }
    }
    r.ok = true;
    return r;
}

} // namespace maure::gpu
