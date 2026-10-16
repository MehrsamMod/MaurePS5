#include <cstdint>
#include <vector>

enum class RdnaInstruction : uint16_t {
    BUFFER_ATOMIC_ADD, BUFFER_ATOMIC_ADD_X2, BUFFER_ATOMIC_AND, BUFFER_ATOMIC_AND_X2,
    BUFFER_ATOMIC_CMPSWAP, BUFFER_ATOMIC_CMPSWAP_X2, BUFFER_ATOMIC_CSUB,
    BUFFER_ATOMIC_DEC, BUFFER_ATOMIC_DEC_X2, BUFFER_ATOMIC_FCMPSWAP, BUFFER_ATOMIC_FCMPSWAP_X2,
    BUFFER_ATOMIC_FMAX, BUFFER_ATOMIC_FMAX_X2, BUFFER_ATOMIC_FMIN, BUFFER_ATOMIC_FMIN_X2,
    BUFFER_ATOMIC_INC, BUFFER_ATOMIC_INC_X2, BUFFER_ATOMIC_OR, BUFFER_ATOMIC_OR_X2,
    BUFFER_ATOMIC_SMAX, BUFFER_ATOMIC_SMAX_X2, BUFFER_ATOMIC_SMIN, BUFFER_ATOMIC_SMIN_X2,
    BUFFER_ATOMIC_SUB, BUFFER_ATOMIC_SUB_X2, BUFFER_ATOMIC_SWAP, BUFFER_ATOMIC_SWAP_X2,
    BUFFER_ATOMIC_UMAX, BUFFER_ATOMIC_UMAX_X2, BUFFER_ATOMIC_UMIN, BUFFER_ATOMIC_UMIN_X2,
    BUFFER_ATOMIC_XOR, BUFFER_ATOMIC_XOR_X2, BUFFER_GL0_INV, BUFFER_GL1_INV,

    BUFFER_LOAD_DWORD, BUFFER_LOAD_DWORDX2, BUFFER_LOAD_DWORDX3, BUFFER_LOAD_DWORDX4,
    BUFFER_LOAD_FORMAT_D16_HI_X, BUFFER_LOAD_FORMAT_D16_X, BUFFER_LOAD_FORMAT_D16_XY,
    BUFFER_LOAD_FORMAT_D16_XYZ, BUFFER_LOAD_FORMAT_D16_XYZW, BUFFER_LOAD_FORMAT_X,
    BUFFER_LOAD_FORMAT_XY, BUFFER_LOAD_FORMAT_XYZ, BUFFER_LOAD_FORMAT_XYZW,
    BUFFER_LOAD_SBYTE, BUFFER_LOAD_SBYTE_D16, BUFFER_LOAD_SBYTE_D16_HI,
    BUFFER_LOAD_SHORT_D16, BUFFER_LOAD_SHORT_D16_HI, BUFFER_LOAD_SSHORT,
    BUFFER_LOAD_UBYTE, BUFFER_LOAD_UBYTE_D16, BUFFER_LOAD_UBYTE_D16_HI, BUFFER_LOAD_USHORT,
    BUFFER_STORE_BYTE, BUFFER_STORE_BYTE_D16_HI, BUFFER_STORE_DWORD, BUFFER_STORE_DWORDX2,
    BUFFER_STORE_DWORDX3, BUFFER_STORE_DWORDX4, BUFFER_STORE_FORMAT_D16_HI_X,
    BUFFER_STORE_FORMAT_D16_X, BUFFER_STORE_FORMAT_D16_XY, BUFFER_STORE_FORMAT_D16_XYZ,
    BUFFER_STORE_FORMAT_D16_XYZW, BUFFER_STORE_FORMAT_X, BUFFER_STORE_FORMAT_XY,
    BUFFER_STORE_FORMAT_XYZ, BUFFER_STORE_FORMAT_XYZW, BUFFER_STORE_SHORT, BUFFER_STORE_SHORT_D16_HI,

    TBUFFER_LOAD_FORMAT_D16_X, TBUFFER_LOAD_FORMAT_D16_XY, TBUFFER_LOAD_FORMAT_D16_XYZ, TBUFFER_LOAD_FORMAT_D16_XYZW,
    TBUFFER_LOAD_FORMAT_X, TBUFFER_LOAD_FORMAT_XY, TBUFFER_LOAD_FORMAT_XYZ, TBUFFER_LOAD_FORMAT_XYZW,
    TBUFFER_STORE_FORMAT_D16_X, TBUFFER_STORE_FORMAT_D16_XY, TBUFFER_STORE_FORMAT_D16_XYZ, TBUFFER_STORE_FORMAT_D16_XYZW,
    TBUFFER_STORE_FORMAT_X, TBUFFER_STORE_FORMAT_XY, TBUFFER_STORE_FORMAT_XYZ, TBUFFER_STORE_FORMAT_XYZW,

    DS_ADD_F32, DS_ADD_RTN_F32, DS_ADD_RTN_U32, DS_ADD_RTN_U64, DS_ADD_U32, DS_ADD_U64,
    DS_AND_B32, DS_AND_B64, DS_AND_RTN_B32, DS_AND_RTN_B64, DS_CMPST_B32, DS_CMPST_B64,
    DS_CMPST_F32, DS_CMPST_F64, DS_CMPST_RTN_B32, DS_CMPST_RTN_B64, DS_CMPST_RTN_F32,
    DS_CMPST_RTN_F64, DS_CONDXCHG32_RTN_B64, DS_DEC_RTN_U32, DS_DEC_RTN_U64, DS_DEC_U32,
    DS_DEC_U64, DS_INC_RTN_U32, DS_INC_RTN_U64, DS_INC_U32, DS_INC_U64,
    DS_MAX_F32, DS_MAX_F64, DS_MAX_I32, DS_MAX_I64, DS_MAX_RTN_F32, DS_MAX_RTN_F64,
    DS_MAX_RTN_I32, DS_MAX_RTN_I64, DS_MAX_RTN_U32, DS_MAX_RTN_U64, DS_MAX_U32, DS_MAX_U64,
    DS_MIN_F32, DS_MIN_F64, DS_MIN_I32, DS_MIN_I64, DS_MIN_RTN_F32, DS_MIN_RTN_F64,
    DS_MIN_RTN_I32, DS_MIN_RTN_I64, DS_MIN_RTN_U32, DS_MIN_RTN_U64, DS_MIN_U32, DS_MIN_U64,
    DS_MSKOR_B32, DS_MSKOR_B64, DS_MSKOR_RTN_B32, DS_MSKOR_RTN_B64, DS_OR_B32, DS_OR_B64,
    DS_OR_RTN_B32, DS_OR_RTN_B64, DS_RSUB_RTN_U32, DS_RSUB_RTN_U64, DS_RSUB_U32, DS_RSUB_U64,
    DS_SUB_RTN_U32, DS_SUB_RTN_U64, DS_SUB_U32, DS_SUB_U64, DS_WRAP_RTN_B32,
    DS_WRXCHG2ST64_RTN_B32, DS_WRXCHG2ST64_RTN_B64, DS_WRXCHG2_RTN_B32, DS_WRXCHG2_RTN_B64,
    DS_WRXCHG_RTN_B32, DS_WRXCHG_RTN_B64, DS_XOR_B32, DS_XOR_B64, DS_XOR_RTN_B32, DS_XOR_RTN_B64,

    DS_READ2ST64_B32, DS_READ2ST64_B64, DS_READ2_B32, DS_READ2_B64, DS_READ_ADDTID_B32,
    DS_READ_B128, DS_READ_B32, DS_READ_B64, DS_READ_B96, DS_READ_I16, DS_READ_I8,
    DS_READ_I8_D16, DS_READ_I8_D16_HI, DS_READ_U16, DS_READ_U16_D16, DS_READ_U16_D16_HI,
    DS_READ_U8, DS_READ_U8_D16, DS_READ_U8_D16_HI, DS_WRITE2ST64_B32, DS_WRITE2ST64_B64,
    DS_WRITE2_B32, DS_WRITE2_B64, DS_WRITE_ADDTID_B32, DS_WRITE_B128, DS_WRITE_B16,
    DS_WRITE_B16_D16_HI, DS_WRITE_B32, DS_WRITE_B64, DS_WRITE_B8, DS_WRITE_B8_D16_HI, DS_WRITE_B96,

    DS_APPEND, DS_CONSUME, DS_BPERMUTE_B32, DS_PERMUTE_B32, DS_SWIZZLE_B32,
    DS_GWS_BARRIER, DS_GWS_INIT, DS_GWS_SEMA_BR, DS_GWS_SEMA_P, DS_GWS_SEMA_RELEASE_ALL,
    DS_GWS_SEMA_V, DS_ORDERED_COUNT, DS_NOP,
    EXP,

    FLAT_ATOMIC_ADD, FLAT_ATOMIC_ADD_X2, FLAT_ATOMIC_AND, FLAT_ATOMIC_AND_X2,
    FLAT_ATOMIC_CMPSWAP, FLAT_ATOMIC_CMPSWAP_X2, FLAT_ATOMIC_DEC, FLAT_ATOMIC_DEC_X2,
    FLAT_ATOMIC_FCMPSWAP, FLAT_ATOMIC_FCMPSWAP_X2, FLAT_ATOMIC_FMAX, FLAT_ATOMIC_FMAX_X2,
    FLAT_ATOMIC_FMIN, FLAT_ATOMIC_FMIN_X2, FLAT_ATOMIC_INC, FLAT_ATOMIC_INC_X2,
    FLAT_ATOMIC_OR, FLAT_ATOMIC_OR_X2, FLAT_ATOMIC_SMAX, FLAT_ATOMIC_SMAX_X2,
    FLAT_ATOMIC_SMIN, FLAT_ATOMIC_SMIN_X2, FLAT_ATOMIC_SUB, FLAT_ATOMIC_SUB_X2,
    FLAT_ATOMIC_SWAP, FLAT_ATOMIC_SWAP_X2, FLAT_ATOMIC_UMAX, FLAT_ATOMIC_UMAX_X2,
    FLAT_ATOMIC_UMIN, FLAT_ATOMIC_UMIN_X2, FLAT_ATOMIC_XOR, FLAT_ATOMIC_XOR_X2,

    FLAT_LOAD_DWORD, FLAT_LOAD_DWORDX2, FLAT_LOAD_DWORDX3, FLAT_LOAD_DWORDX4,
    FLAT_LOAD_SBYTE, FLAT_LOAD_SBYTE_D16, FLAT_LOAD_SBYTE_D16_HI, FLAT_LOAD_SHORT_D16,
    FLAT_LOAD_SHORT_D16_HI, FLAT_LOAD_SSHORT, FLAT_LOAD_UBYTE, FLAT_LOAD_UBYTE_D16,
    FLAT_LOAD_UBYTE_D16_HI, FLAT_LOAD_USHORT,
    FLAT_STORE_BYTE, FLAT_STORE_BYTE_D16_HI, FLAT_STORE_DWORD, FLAT_STORE_DWORDX2,
    FLAT_STORE_DWORDX3, FLAT_STORE_DWORDX4, FLAT_STORE_SHORT, FLAT_STORE_SHORT_D16_HI,

    GLOBAL_ATOMIC_ADD, GLOBAL_ATOMIC_ADD_X2, GLOBAL_ATOMIC_AND, GLOBAL_ATOMIC_AND_X2,
    GLOBAL_ATOMIC_CMPSWAP, GLOBAL_ATOMIC_CMPSWAP_X2, GLOBAL_ATOMIC_CSUB, GLOBAL_ATOMIC_DEC,
    GLOBAL_ATOMIC_DEC_X2, GLOBAL_ATOMIC_FCMPSWAP, GLOBAL_ATOMIC_FCMPSWAP_X2, GLOBAL_ATOMIC_FMAX,
    GLOBAL_ATOMIC_FMAX_X2, GLOBAL_ATOMIC_FMIN, GLOBAL_ATOMIC_FMIN_X2, GLOBAL_ATOMIC_INC,
    GLOBAL_ATOMIC_INC_X2, GLOBAL_ATOMIC_OR, GLOBAL_ATOMIC_OR_X2, GLOBAL_ATOMIC_SMAX,
    GLOBAL_ATOMIC_SMAX_X2, GLOBAL_ATOMIC_SMIN, GLOBAL_ATOMIC_SMIN_X2, GLOBAL_ATOMIC_SUB,
    GLOBAL_ATOMIC_SUB_X2, GLOBAL_ATOMIC_SWAP, GLOBAL_ATOMIC_SWAP_X2, GLOBAL_ATOMIC_UMAX,
    GLOBAL_ATOMIC_UMAX_X2, GLOBAL_ATOMIC_UMIN, GLOBAL_ATOMIC_UMIN_X2, GLOBAL_ATOMIC_XOR,
    GLOBAL_ATOMIC_XOR_X2,

    GLOBAL_LOAD_DWORD, GLOBAL_LOAD_DWORDX2, GLOBAL_LOAD_DWORDX3, GLOBAL_LOAD_DWORDX4,
    GLOBAL_LOAD_DWORD_ADDTID, GLOBAL_LOAD_SBYTE, GLOBAL_LOAD_SBYTE_D16, GLOBAL_LOAD_SBYTE_D16_HI,
    GLOBAL_LOAD_SHORT_D16, GLOBAL_LOAD_SHORT_D16_HI, GLOBAL_LOAD_SSHORT, GLOBAL_LOAD_UBYTE,
    GLOBAL_LOAD_UBYTE_D16, GLOBAL_LOAD_UBYTE_D16_HI, GLOBAL_LOAD_USHORT,
    GLOBAL_STORE_BYTE, GLOBAL_STORE_BYTE_D16_HI, GLOBAL_STORE_DWORD, GLOBAL_STORE_DWORDX2,
    GLOBAL_STORE_DWORDX3, GLOBAL_STORE_DWORDX4, GLOBAL_STORE_DWORD_ADDTID, GLOBAL_STORE_SHORT,
    GLOBAL_STORE_SHORT_D16_HI,

    IMAGE_ATOMIC_ADD, IMAGE_ATOMIC_AND, IMAGE_ATOMIC_CMPSWAP, IMAGE_ATOMIC_DEC,
    IMAGE_ATOMIC_FCMPSWAP, IMAGE_ATOMIC_FMAX, IMAGE_ATOMIC_FMIN, IMAGE_ATOMIC_INC,
    IMAGE_ATOMIC_OR, IMAGE_ATOMIC_SMAX, IMAGE_ATOMIC_SMIN, IMAGE_ATOMIC_SUB,
    IMAGE_ATOMIC_SWAP, IMAGE_ATOMIC_UMAX, IMAGE_ATOMIC_UMIN, IMAGE_ATOMIC_XOR,

    IMAGE_BVH64_INTERSECT_RAY, IMAGE_BVH_INTERSECT_RAY, IMAGE_GET_LOD, IMAGE_GET_RESINFO,
    IMAGE_LOAD, IMAGE_LOAD_BY2, IMAGE_LOAD_BY4, IMAGE_LOAD_MIP, IMAGE_LOAD_MIP_BY2,
    IMAGE_LOAD_MIP_BY4, IMAGE_LOAD_MIP_PCK, IMAGE_LOAD_MIP_PCK2, IMAGE_LOAD_MIP_PCK4,
    IMAGE_LOAD_MIP_PCK_SGN, IMAGE_LOAD_PCK, IMAGE_LOAD_PCK2, IMAGE_LOAD_PCK4,
    IMAGE_LOAD_PCK_SGN, IMAGE_MSAA_LOAD, IMAGE_STORE, IMAGE_STORE_BY2, IMAGE_STORE_BY4,
    IMAGE_STORE_MIP, IMAGE_STORE_MIP_BY2, IMAGE_STORE_MIP_BY4, IMAGE_STORE_MIP_PCK,
    IMAGE_STORE_MIP_PCK2, IMAGE_STORE_MIP_PCK4, IMAGE_STORE_PCK, IMAGE_STORE_PCK2,
    IMAGE_STORE_PCK4,

    IMAGE_GATHER4, IMAGE_GATHER4H, IMAGE_GATHER4H_PCK, IMAGE_GATHER4_B, IMAGE_GATHER4_B_CL,
    IMAGE_GATHER4_B_CL_O, IMAGE_GATHER4_B_O, IMAGE_GATHER4_C, IMAGE_GATHER4_CL, IMAGE_GATHER4_CL_O,
    IMAGE_GATHER4_C_B, IMAGE_GATHER4_C_B_CL, IMAGE_GATHER4_C_B_CL_O, IMAGE_GATHER4_C_B_O,
    IMAGE_GATHER4_C_CL, IMAGE_GATHER4_C_CL_O, IMAGE_GATHER4_C_L, IMAGE_GATHER4_C_LZ,
    IMAGE_GATHER4_C_LZ_O, IMAGE_GATHER4_C_L_O, IMAGE_GATHER4_C_O, IMAGE_GATHER4_L,
    IMAGE_GATHER4_LZ, IMAGE_GATHER4_LZ_O, IMAGE_GATHER4_L_O, IMAGE_GATHER4_O, IMAGE_GATHER8H_PCK,
    IMAGE_SAMPLE, IMAGE_SAMPLE_B, IMAGE_SAMPLE_B_CL, IMAGE_SAMPLE_B_CL_O, IMAGE_SAMPLE_B_O,
    IMAGE_SAMPLE_C, IMAGE_SAMPLE_CD, IMAGE_SAMPLE_CD_CL, IMAGE_SAMPLE_CD_CL_G16, IMAGE_SAMPLE_CD_CL_O,
    IMAGE_SAMPLE_CD_CL_O_G16, IMAGE_SAMPLE_CD_G16, IMAGE_SAMPLE_CD_O, IMAGE_SAMPLE_CD_O_G16,
    IMAGE_SAMPLE_CL, IMAGE_SAMPLE_CL_O, IMAGE_SAMPLE_C_B, IMAGE_SAMPLE_C_B_CL, IMAGE_SAMPLE_C_B_CL_O,
    IMAGE_SAMPLE_C_B_O, IMAGE_SAMPLE_C_CD, IMAGE_SAMPLE_C_CD_CL, IMAGE_SAMPLE_C_CD_CL_G16,
    IMAGE_SAMPLE_C_CD_CL_O, IMAGE_SAMPLE_C_CD_CL_O_G16, IMAGE_SAMPLE_C_CD_G16, IMAGE_SAMPLE_C_CD_O,
    IMAGE_SAMPLE_C_CD_O_G16, IMAGE_SAMPLE_C_CL, IMAGE_SAMPLE_C_CL_O, IMAGE_SAMPLE_C_D,
    IMAGE_SAMPLE_C_D_CL, IMAGE_SAMPLE_C_D_CL_G16, IMAGE_SAMPLE_C_D_CL_O, IMAGE_SAMPLE_C_D_CL_O_G16,
    IMAGE_SAMPLE_C_D_G16, IMAGE_SAMPLE_C_D_O, IMAGE_SAMPLE_C_D_O_G16, IMAGE_SAMPLE_C_L,
    IMAGE_SAMPLE_C_LZ, IMAGE_SAMPLE_C_LZ_O, IMAGE_SAMPLE_C_L_O, IMAGE_SAMPLE_C_O, IMAGE_SAMPLE_D,
    IMAGE_SAMPLE_D_CL, IMAGE_SAMPLE_D_CL_G16, IMAGE_SAMPLE_D_CL_O, IMAGE_SAMPLE_D_CL_O_G16,
    IMAGE_SAMPLE_D_G16, IMAGE_SAMPLE_D_O, IMAGE_SAMPLE_D_O_G16, IMAGE_SAMPLE_L, IMAGE_SAMPLE_LZ,
    IMAGE_SAMPLE_LZ_O, IMAGE_SAMPLE_L_O, IMAGE_SAMPLE_O,

    SCRATCH_LOAD_DWORD, SCRATCH_LOAD_DWORDX2, SCRATCH_LOAD_DWORDX3, SCRATCH_LOAD_DWORDX4,
    SCRATCH_LOAD_SBYTE, SCRATCH_LOAD_SBYTE_D16, SCRATCH_LOAD_SBYTE_D16_HI, SCRATCH_LOAD_SHORT_D16,
    SCRATCH_LOAD_SHORT_D16_HI, SCRATCH_LOAD_SSHORT, SCRATCH_LOAD_UBYTE, SCRATCH_LOAD_UBYTE_D16,
    SCRATCH_LOAD_UBYTE_D16_HI, SCRATCH_LOAD_USHORT,
    SCRATCH_STORE_BYTE, SCRATCH_STORE_BYTE_D16_HI, SCRATCH_STORE_DWORD, SCRATCH_STORE_DWORDX2,
    SCRATCH_STORE_DWORDX3, SCRATCH_STORE_DWORDX4, SCRATCH_STORE_SHORT, SCRATCH_STORE_SHORT_D16_HI,

    S_ABSDIFF_I32, S_ABS_I32, S_ADDC_U32, S_ADDK_I32, S_ADD_I32, S_ADD_U32,
    S_ANDN1_SAVEEXEC_B32, S_ANDN1_SAVEEXEC_B64, S_ANDN1_WREXEC_B32, S_ANDN1_WREXEC_B64,
    S_ANDN2_B32, S_ANDN2_B64, S_ANDN2_SAVEEXEC_B32, S_ANDN2_SAVEEXEC_B64,
    S_ANDN2_WREXEC_B32, S_ANDN2_WREXEC_B64, S_AND_B32, S_AND_B64,
    S_AND_SAVEEXEC_B32, S_AND_SAVEEXEC_B64, S_ASHR_I32, S_ASHR_I64,
    S_BCNT0_I32_B32, S_BCNT0_I32_B64, S_BCNT1_I32_B32, S_BCNT1_I32_B64,
    S_BFE_I32, S_BFE_I64, S_BFE_U32, S_BFE_U64, S_BFM_B32, S_BFM_B64,

    S_ATC_PROBE, S_ATC_PROBE_BUFFER, S_BARRIER,

    S_BITCMP0_B32, S_BITCMP0_B64, S_BITCMP1_B32, S_BITCMP1_B64,
    S_BITREPLICATE_B64_B32, S_BITSET0_B32, S_BITSET0_B64, S_BITSET1_B32, S_BITSET1_B64,
    S_BREV_B32, S_BREV_B64,

    S_BRANCH,

    S_BUFFER_LOAD_DWORD, S_BUFFER_LOAD_DWORDX16, S_BUFFER_LOAD_DWORDX2,
    S_BUFFER_LOAD_DWORDX4, S_BUFFER_LOAD_DWORDX8,

    S_CALL_B64, S_CBRANCH_CDBGSYS, S_CBRANCH_CDBGSYS_AND_USER, S_CBRANCH_CDBGSYS_OR_USER,
    S_CBRANCH_CDBGUSER, S_CBRANCH_EXECNZ, S_CBRANCH_EXECZ, S_CBRANCH_SCC0, S_CBRANCH_SCC1,
    S_CBRANCH_VCCNZ, S_CBRANCH_VCCZ, S_CLAUSE,

    S_CMOVK_I32, S_CMOV_B32, S_CMOV_B64, S_CMPK_EQ_I32, S_CMPK_EQ_U32, S_CMPK_GE_I32,
    S_CMPK_GE_U32, S_CMPK_GT_I32, S_CMPK_GT_U32, S_CMPK_LE_I32, S_CMPK_LE_U32, S_CMPK_LG_I32,
    S_CMPK_LG_U32, S_CMPK_LT_I32, S_CMPK_LT_U32,

    S_CMP_EQ_I32, S_CMP_EQ_U32, S_CMP_EQ_U64, S_CMP_GE_I32, S_CMP_GE_U32, S_CMP_GT_I32,
    S_CMP_GT_U32, S_CMP_LE_I32, S_CMP_LE_U32, S_CMP_LG_I32, S_CMP_LG_U32, S_CMP_LG_U64,
    S_CMP_LT_I32, S_CMP_LT_U32,

S_CODE_END, S_CSELECT_B32, S_CSELECT_B64, S_DCACHE_INV, S_DECPERFLEVEL,
    S_DENORM_MODE, S_ENDPGM, S_ENDPGM_ORDERED_PS_DONE, S_ENDPGM_SAVED,
    S_FF0_I32_B32, S_FF0_I32_B64, S_FF1_I32_B32, S_FF1_I32_B64,
    S_FLBIT_I32, S_FLBIT_I32_B32, S_FLBIT_I32_B64, S_FLBIT_I32_I64,
    S_GETPC_B64, S_GETREG_B32, S_GET_WAVEID_IN_WORKGROUP, S_GL1_INV,
    S_ICACHE_INV, S_INCPERFLEVEL, S_INST_PREFETCH,
    S_LOAD_DWORD, S_LOAD_DWORDX16, S_LOAD_DWORDX2, S_LOAD_DWORDX4, S_LOAD_DWORDX8,
    S_LSHL1_ADD_U32, S_LSHL2_ADD_U32, S_LSHL3_ADD_U32, S_LSHL4_ADD_U32,
    S_LSHL_B32, S_LSHL_B64, S_LSHR_B32, S_LSHR_B64,
    S_MAX_I32, S_MAX_U32, S_MEMREALTIME, S_MEMTIME, S_MIN_I32, S_MIN_U32,
    S_MOVK_I32, S_MOVRELD_B32, S_MOVRELD_B64, S_MOVRELSD_2_B32, S_MOVRELS_B32, S_MOVRELS_B64,
    S_MOV_B32, S_MOV_B64, S_MULK_I32, S_MUL_HI_I32, S_MUL_HI_U32, S_MUL_I32,
    S_NAND_B32, S_NAND_B64, S_NAND_SAVEEXEC_B32, S_NAND_SAVEEXEC_B64,
    S_NOP, S_NOR_B32, S_NOR_B64, S_NOR_SAVEEXEC_B32, S_NOR_SAVEEXEC_B64,
    S_NOT_B32, S_NOT_B64, S_ORN1_SAVEEXEC_B32, S_ORN1_SAVEEXEC_B64,
    S_ORN2_B32, S_ORN2_B64, S_ORN2_SAVEEXEC_B32, S_ORN2_SAVEEXEC_B64,
    S_OR_B32, S_OR_B64, S_OR_SAVEEXEC_B32, S_OR_SAVEEXEC_B64,
    S_PACK_HH_B32_B16, S_PACK_LH_B32_B16, S_PACK_LL_B32_B16,
    S_QUADMASK_B32, S_QUADMASK_B64, S_RFE_B64, S_ROUND_MODE,
    S_SCRATCH_LOAD_DWORD, S_SCRATCH_LOAD_DWORDX2, S_SCRATCH_LOAD_DWORDX4,
    S_SENDMSG, S_SENDMSGHALT, S_SETHALT, S_SETKILL, S_SETPC_B64, S_SETPRIO,
    S_SETREG_B32, S_SETREG_IMM32_B32, S_SEXT_I32_I16, S_SEXT_I32_I8, S_SLEEP,
    S_SUBB_U32, S_SUBVECTOR_LOOP_BEGIN, S_SUBVECTOR_LOOP_END, S_SUB_I32, S_SUB_U32,
    S_SWAPPC_B64, S_TRAP, S_TTRACEDATA, S_TTRACEDATA_IMM, S_VERSION,
    S_WAITCNT, S_WAITCNT_DEPCTR, S_WAITCNT_EXPCNT, S_WAITCNT_LGKMCNT,
    S_WAITCNT_VMCNT, S_WAITCNT_VSCNT, S_WAIT_IDLE, S_WAKEUP,
    S_WQM_B32, S_WQM_B64, S_XNOR_B32, S_XNOR_B64, S_XNOR_SAVEEXEC_B32, S_XNOR_SAVEEXEC_B64,
    S_XOR_B32, S_XOR_B64, S_XOR_SAVEEXEC_B32, S_XOR_SAVEEXEC_B64
};

enum class LocalSpirvOp {
    SpirvOpLoad, SpirvOpStore, SpirvOpAtomicIAdd, SpirvOpAtomicISub, SpirvOpAtomicAnd,
    SpirvOpAtomicOr, SpirvOpAtomicXor, SpirvOpAtomicExchange, SpirvOpAtomicCompareExchange,
    SpirvOpAtomicIIncrement, SpirvOpAtomicIDecrement, SpirvOpAtomicFMinEXT, SpirvOpAtomicFMaxEXT,
    SpirvOpAtomicSMin, SpirvOpAtomicUMin, SpirvOpAtomicSMax, SpirvOpAtomicUMax, SpirvOpControlBarrier,
    SpirvOpImageRead, SpirvOpImageWrite, SpirvOpGroupNonUniformShuffle, SpirvOpGroupNonUniformShuffleRelative,
    SpirvOpNop, SpirvOpAtomicFAddEXT, SpirvOpImageQueryLod, SpirvOpImageQuerySize, SpirvOpImageGather, SpirvOpImageSampleExplicitLod,
    SpirvOpBitFieldSExtract, SpirvOpBitFieldUExtract, SpirvOpBitCount, SpirvOpShiftRightArithmetic, SpirvOpBitwiseAnd, SpirvOpIAddCarry, SpirvOpIAdd,
    SpirvOpSAbs, SpirvOpBranchConditional, SpirvOpBranch, SpirvOpBitFieldInsert, SpirvOpBitReverse, SpirvOpULessThan, SpirvOpSLessThan, SpirvOpINotEqual,
    SpirvOpULessThanEqual, SpirvOpSLessThanEqual, SpirvOpSGreaterThan, SpirvOpUGreaterThan, SpirvOpUGreaterThanEqual, SpirvOpIEqual, SpirvOpSGreaterThanEqual, SpirvOpSelect,
    SpirvOpISub, SpirvOpShiftLeftLogical, SpirvOpShiftRightLogical, SpirvOpBitwiseOr, SpirvOpBitwiseXor, SpirvOpBitwiseNot,
    SpirvOpSMax, SpirvOpUMax, SpirvOpSMin, SpirvOpUMin, SpirvOpIMul, SpirvOpSNegate, SpirvOpCopyObject
};

struct Node {
    uint32_t resultId;
    uint32_t pointerId;
    std::vector<uint32_t> srcIds;
};

extern uint32_t typeInt32, typeInt64, typeFloat32, typeFloat64;
extern uint32_t typeVec2F32, typeVec3F32, typeVec4F32, typeVec2F16, typeVec4F16;
extern uint32_t scopeDevice, scopeWorkgroup, scopeSubgroup, semanticsUniform, semanticsNone;

extern void emit(LocalSpirvOp op, std::vector<uint32_t> operands);

bool translateRdnaToSpirv(RdnaInstruction inst, const Node& node) {
    switch (inst) {
        case RdnaInstruction::BUFFER_ATOMIC_ADD:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_ADD_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_AND:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_AND_X2:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_DEC:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_DEC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_INC:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_INC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_OR:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_OR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SUB:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SUB_X2:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_XOR:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_XOR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SWAP:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SMAX:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SMIN:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_SMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_UMAX:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::BUFFER_ATOMIC_UMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_UMIN:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_UMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FMIN:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FMAX:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_CMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_CMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FCMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_FCMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::BUFFER_ATOMIC_CSUB:
            return false;

        case RdnaInstruction::BUFFER_GL0_INV:
            emit(LocalSpirvOp::SpirvOpControlBarrier, {scopeSubgroup, scopeSubgroup, semanticsUniform});
            return true;
        case RdnaInstruction::BUFFER_GL1_INV:
            emit(LocalSpirvOp::SpirvOpControlBarrier, {scopeDevice, scopeDevice, semanticsUniform});
            return true;

        case RdnaInstruction::BUFFER_LOAD_DWORD:
        case RdnaInstruction::BUFFER_LOAD_SBYTE:
        case RdnaInstruction::BUFFER_LOAD_SBYTE_D16:
        case RdnaInstruction::BUFFER_LOAD_SBYTE_D16_HI:
        case RdnaInstruction::BUFFER_LOAD_SHORT_D16:
        case RdnaInstruction::BUFFER_LOAD_SHORT_D16_HI:
        case RdnaInstruction::BUFFER_LOAD_SSHORT:
        case RdnaInstruction::BUFFER_LOAD_UBYTE:
        case RdnaInstruction::BUFFER_LOAD_UBYTE_D16:
        case RdnaInstruction::BUFFER_LOAD_UBYTE_D16_HI:
        case RdnaInstruction::BUFFER_LOAD_USHORT:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::BUFFER_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::BUFFER_LOAD_DWORDX3:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec3F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::BUFFER_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId});
            return true;

        case RdnaInstruction::BUFFER_LOAD_FORMAT_D16_X:
        case RdnaInstruction::BUFFER_LOAD_FORMAT_X:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_D16_X:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_X:
            emit(LocalSpirvOp::SpirvOpImageRead, {typeFloat32, node.resultId, node.pointerId, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_LOAD_FORMAT_D16_XY:
        case RdnaInstruction::BUFFER_LOAD_FORMAT_XY:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_D16_XY:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_XY:
            emit(LocalSpirvOp::SpirvOpImageRead, {typeVec2F32, node.resultId, node.pointerId, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_LOAD_FORMAT_D16_XYZ:
        case RdnaInstruction::BUFFER_LOAD_FORMAT_XYZ:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_D16_XYZ:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_XYZ:
            emit(LocalSpirvOp::SpirvOpImageRead, {typeVec3F32, node.resultId, node.pointerId, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_LOAD_FORMAT_D16_HI_X:
        case RdnaInstruction::BUFFER_LOAD_FORMAT_D16_XYZW:
        case RdnaInstruction::BUFFER_LOAD_FORMAT_XYZW:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_D16_XYZW:
        case RdnaInstruction::TBUFFER_LOAD_FORMAT_XYZW:
            emit(LocalSpirvOp::SpirvOpImageRead, {typeVec4F32, node.resultId, node.pointerId, node.srcIds[0]});
            return true;

        case RdnaInstruction::BUFFER_STORE_BYTE:
        case RdnaInstruction::BUFFER_STORE_BYTE_D16_HI:
        case RdnaInstruction::BUFFER_STORE_DWORD:
        case RdnaInstruction::BUFFER_STORE_DWORDX2:
        case RdnaInstruction::BUFFER_STORE_DWORDX3:
        case RdnaInstruction::BUFFER_STORE_DWORDX4:
        case RdnaInstruction::BUFFER_STORE_SHORT:
        case RdnaInstruction::BUFFER_STORE_SHORT_D16_HI:
            emit(LocalSpirvOp::SpirvOpStore, {node.pointerId, node.srcIds[0]});
            return true;
        case RdnaInstruction::BUFFER_STORE_FORMAT_D16_HI_X:
        case RdnaInstruction::BUFFER_STORE_FORMAT_D16_X:
        case RdnaInstruction::BUFFER_STORE_FORMAT_D16_XY:
        case RdnaInstruction::BUFFER_STORE_FORMAT_D16_XYZ:
        case RdnaInstruction::BUFFER_STORE_FORMAT_D16_XYZW:
        case RdnaInstruction::BUFFER_STORE_FORMAT_X:
        case RdnaInstruction::BUFFER_STORE_FORMAT_XY:
        case RdnaInstruction::BUFFER_STORE_FORMAT_XYZ:
        case RdnaInstruction::BUFFER_STORE_FORMAT_XYZW:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_D16_X:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_D16_XY:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_D16_XYZ:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_D16_XYZW:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_X:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_XY:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_XYZ:
        case RdnaInstruction::TBUFFER_STORE_FORMAT_XYZW:
            emit(LocalSpirvOp::SpirvOpImageWrite, {node.pointerId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::DS_ADD_F32:
        case RdnaInstruction::DS_ADD_RTN_F32:
            emit(LocalSpirvOp::SpirvOpAtomicFAddEXT, {typeFloat32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_ADD_U32:
        case RdnaInstruction::DS_ADD_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_ADD_U64:
        case RdnaInstruction::DS_ADD_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_SUB_U32:
        case RdnaInstruction::DS_SUB_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_SUB_U64:
        case RdnaInstruction::DS_SUB_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_INC_U32:
        case RdnaInstruction::DS_INC_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone});
            return true;
        case RdnaInstruction::DS_INC_U64:
        case RdnaInstruction::DS_INC_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone});
            return true;
        case RdnaInstruction::DS_DEC_U32:
        case RdnaInstruction::DS_DEC_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone});
            return true;
        case RdnaInstruction::DS_DEC_U64:
        case RdnaInstruction::DS_DEC_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone});
            return true;
        case RdnaInstruction::DS_AND_B32:
        case RdnaInstruction::DS_AND_RTN_B32:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]});
            return true;
        case RdnaInstruction::DS_AND_B64:
        case RdnaInstruction::DS_AND_RTN_B64:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_OR_B32:
        case RdnaInstruction::DS_OR_RTN_B32:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_OR_B64:
        case RdnaInstruction::DS_OR_RTN_B64:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_XOR_B32:
        case RdnaInstruction::DS_XOR_RTN_B32:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_XOR_B64:
        case RdnaInstruction::DS_XOR_RTN_B64:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_F32:
        case RdnaInstruction::DS_MIN_RTN_F32:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_F64:
        case RdnaInstruction::DS_MIN_RTN_F64:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_I32:
        case RdnaInstruction::DS_MIN_RTN_I32:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_I64:
        case RdnaInstruction::DS_MIN_RTN_I64:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_U32:
        case RdnaInstruction::DS_MIN_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MIN_U64:
        case RdnaInstruction::DS_MIN_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_F32:
        case RdnaInstruction::DS_MAX_RTN_F32:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_F64:
        case RdnaInstruction::DS_MAX_RTN_F64:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_I32:
        case RdnaInstruction::DS_MAX_RTN_I32:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_I64:
        case RdnaInstruction::DS_MAX_RTN_I64:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_U32:
        case RdnaInstruction::DS_MAX_RTN_U32:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_MAX_U64:
        case RdnaInstruction::DS_MAX_RTN_U64:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;

        case RdnaInstruction::DS_CMPST_B32:
        case RdnaInstruction::DS_CMPST_RTN_B32:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, semanticsNone, node.srcIds[0], node.srcIds[1]}); 
            return true;
        case RdnaInstruction::DS_CMPST_B64:
        case RdnaInstruction::DS_CMPST_RTN_B64:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, semanticsNone, node.srcIds[0], node.srcIds[1]}); 
            return true;
        case RdnaInstruction::DS_CMPST_F32:
        case RdnaInstruction::DS_CMPST_RTN_F32:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, semanticsNone, node.srcIds[0], node.srcIds[1]}); 
            return true;
        case RdnaInstruction::DS_CMPST_F64:
        case RdnaInstruction::DS_CMPST_RTN_F64:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, semanticsNone, node.srcIds[0], node.srcIds[1]}); 
            return true;
        case RdnaInstruction::DS_WRXCHG_RTN_B32:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt32, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;
        case RdnaInstruction::DS_WRXCHG_RTN_B64:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt64, node.resultId, node.pointerId, scopeWorkgroup, semanticsNone, node.srcIds[0]}); 
            return true;

        case RdnaInstruction::DS_READ_B32:
        case RdnaInstruction::DS_READ_I16:
        case RdnaInstruction::DS_READ_I8:
        case RdnaInstruction::DS_READ_I8_D16:
        case RdnaInstruction::DS_READ_I8_D16_HI:
        case RdnaInstruction::DS_READ_U16:
        case RdnaInstruction::DS_READ_U16_D16:
        case RdnaInstruction::DS_READ_U16_D16_HI:
        case RdnaInstruction::DS_READ_U8:
        case RdnaInstruction::DS_READ_U8_D16:
        case RdnaInstruction::DS_READ_U8_D16_HI:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId}); 
            return true;
        case RdnaInstruction::DS_READ_B64:
        case RdnaInstruction::DS_READ2_B32:
        case RdnaInstruction::DS_READ2ST64_B32:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId}); 
            return true;
        case RdnaInstruction::DS_READ_B96:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec3F32, node.resultId, node.pointerId}); 
            return true;
        case RdnaInstruction::DS_READ_B128:
        case RdnaInstruction::DS_READ2_B64:
        case RdnaInstruction::DS_READ2ST64_B64:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId}); 
            return true;

        case RdnaInstruction::DS_WRITE_B16:
        case RdnaInstruction::DS_WRITE_B16_D16_HI:
        case RdnaInstruction::DS_WRITE_B32:
        case RdnaInstruction::DS_WRITE_B8:
        case RdnaInstruction::DS_WRITE_B8_D16_HI:
        case RdnaInstruction::DS_WRITE_B64:
        case RdnaInstruction::DS_WRITE_B96:
        case RdnaInstruction::DS_WRITE_B128:
        case RdnaInstruction::DS_WRITE2_B32:
        case RdnaInstruction::DS_WRITE2ST64_B32:
        case RdnaInstruction::DS_WRITE2_B64:
        case RdnaInstruction::DS_WRITE2ST64_B64:
            emit(LocalSpirvOp::SpirvOpStore, {node.pointerId, node.srcIds[0]}); 
            return true;

        case RdnaInstruction::DS_PERMUTE_B32:
        case RdnaInstruction::DS_BPERMUTE_B32:
            emit(LocalSpirvOp::SpirvOpGroupNonUniformShuffle, {typeInt32, node.resultId, scopeSubgroup, node.srcIds[0], node.srcIds[1]}); 
            return true;
        case RdnaInstruction::DS_SWIZZLE_B32:
            emit(LocalSpirvOp::SpirvOpGroupNonUniformShuffleRelative, {typeInt32, node.resultId, scopeSubgroup, node.srcIds[0], node.srcIds[1]}); 
            return true;

        case RdnaInstruction::DS_GWS_BARRIER:
            emit(LocalSpirvOp::SpirvOpControlBarrier, {scopeDevice, scopeDevice, semanticsUniform}); 
            return true;
        case RdnaInstruction::DS_NOP:
            emit(LocalSpirvOp::SpirvOpNop, {}); 
            return true;

        case RdnaInstruction::DS_RSUB_U32:
        case RdnaInstruction::DS_RSUB_RTN_U32:
        case RdnaInstruction::DS_RSUB_U64:
        case RdnaInstruction::DS_RSUB_RTN_U64:
        case RdnaInstruction::DS_MSKOR_B32:
        case RdnaInstruction::DS_MSKOR_B64:
        case RdnaInstruction::DS_MSKOR_RTN_B32:
        case RdnaInstruction::DS_MSKOR_RTN_B64:
        case RdnaInstruction::DS_WRAP_RTN_B32:
        case RdnaInstruction::DS_WRXCHG2ST64_RTN_B32:
        case RdnaInstruction::DS_WRXCHG2ST64_RTN_B64:
        case RdnaInstruction::DS_WRXCHG2_RTN_B32:
        case RdnaInstruction::DS_WRXCHG2_RTN_B64:
        case RdnaInstruction::DS_CONDXCHG32_RTN_B64:
        case RdnaInstruction::DS_READ_ADDTID_B32:
        case RdnaInstruction::DS_WRITE_ADDTID_B32:
        case RdnaInstruction::DS_APPEND:
        case RdnaInstruction::DS_CONSUME:
        case RdnaInstruction::DS_GWS_INIT:
        case RdnaInstruction::DS_GWS_SEMA_BR:
        case RdnaInstruction::DS_GWS_SEMA_P:
        case RdnaInstruction::DS_GWS_SEMA_RELEASE_ALL:
        case RdnaInstruction::DS_GWS_SEMA_V:
        case RdnaInstruction::DS_ORDERED_COUNT:
            return false;

        case RdnaInstruction::EXP:
            return false;

        case RdnaInstruction::FLAT_ATOMIC_ADD:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_ADD_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_AND:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_AND_X2:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_DEC:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_DEC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_INC:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_INC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_OR:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_OR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SUB:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SUB_X2:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_XOR:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_XOR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SWAP:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SMAX:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SMIN:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_SMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_UMAX:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_UMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_UMIN:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_UMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FMIN:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FMAX:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_CMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_CMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FCMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::FLAT_ATOMIC_FCMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::FLAT_LOAD_DWORD:
        case RdnaInstruction::FLAT_LOAD_SBYTE:
        case RdnaInstruction::FLAT_LOAD_SBYTE_D16:
        case RdnaInstruction::FLAT_LOAD_SBYTE_D16_HI:
        case RdnaInstruction::FLAT_LOAD_SHORT_D16:
        case RdnaInstruction::FLAT_LOAD_SHORT_D16_HI:
        case RdnaInstruction::FLAT_LOAD_SSHORT:
        case RdnaInstruction::FLAT_LOAD_UBYTE:
        case RdnaInstruction::FLAT_LOAD_UBYTE_D16:
        case RdnaInstruction::FLAT_LOAD_UBYTE_D16_HI:
        case RdnaInstruction::FLAT_LOAD_USHORT:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::FLAT_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::FLAT_LOAD_DWORDX3:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec3F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::FLAT_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId});
            return true;

        case RdnaInstruction::FLAT_STORE_BYTE:
        case RdnaInstruction::FLAT_STORE_BYTE_D16_HI:
        case RdnaInstruction::FLAT_STORE_DWORD:
        case RdnaInstruction::FLAT_STORE_DWORDX2:
        case RdnaInstruction::FLAT_STORE_DWORDX3:
        case RdnaInstruction::FLAT_STORE_DWORDX4:
        case RdnaInstruction::FLAT_STORE_SHORT:
        case RdnaInstruction::FLAT_STORE_SHORT_D16_HI:
            emit(LocalSpirvOp::SpirvOpStore, {node.pointerId, node.srcIds[0]});
            return true;

        case RdnaInstruction::GLOBAL_ATOMIC_ADD:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_ADD_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_AND:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_AND_X2:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_DEC:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_DEC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_INC:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_INC_X2:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_OR:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_OR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SUB:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SUB_X2:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_XOR:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_XOR_X2:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SWAP:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SMAX:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SMIN:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_SMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_UMAX:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_UMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_UMIN:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_UMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FMIN:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FMIN_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FMAX:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FMAX_X2:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_CMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_CMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FCMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_FCMPSWAP_X2:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeFloat64, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::GLOBAL_ATOMIC_CSUB:
            return false;

        case RdnaInstruction::GLOBAL_LOAD_DWORD:
        case RdnaInstruction::GLOBAL_LOAD_SBYTE:
        case RdnaInstruction::GLOBAL_LOAD_SBYTE_D16:
        case RdnaInstruction::GLOBAL_LOAD_SBYTE_D16_HI:
        case RdnaInstruction::GLOBAL_LOAD_SHORT_D16:
        case RdnaInstruction::GLOBAL_LOAD_SHORT_D16_HI:
        case RdnaInstruction::GLOBAL_LOAD_SSHORT:
        case RdnaInstruction::GLOBAL_LOAD_UBYTE:
        case RdnaInstruction::GLOBAL_LOAD_UBYTE_D16:
        case RdnaInstruction::GLOBAL_LOAD_UBYTE_D16_HI:
        case RdnaInstruction::GLOBAL_LOAD_USHORT:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::GLOBAL_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::GLOBAL_LOAD_DWORDX3:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec3F32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::GLOBAL_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId}); return true;

        case RdnaInstruction::GLOBAL_STORE_BYTE:
        case RdnaInstruction::GLOBAL_STORE_BYTE_D16_HI:
        case RdnaInstruction::GLOBAL_STORE_DWORD:
        case RdnaInstruction::GLOBAL_STORE_DWORDX2:
        case RdnaInstruction::GLOBAL_STORE_DWORDX3:
        case RdnaInstruction::GLOBAL_STORE_DWORDX4:
        case RdnaInstruction::GLOBAL_STORE_SHORT:
        case RdnaInstruction::GLOBAL_STORE_SHORT_D16_HI:
            emit(LocalSpirvOp::SpirvOpStore, {node.pointerId, node.srcIds[0]}); return true;

        case RdnaInstruction::GLOBAL_LOAD_DWORD_ADDTID:
        case RdnaInstruction::GLOBAL_STORE_DWORD_ADDTID:
            return false;

        case RdnaInstruction::IMAGE_ATOMIC_ADD:
            emit(LocalSpirvOp::SpirvOpAtomicIAdd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_AND:
            emit(LocalSpirvOp::SpirvOpAtomicAnd, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_OR:
            emit(LocalSpirvOp::SpirvOpAtomicOr, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_XOR:
            emit(LocalSpirvOp::SpirvOpAtomicXor, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_SUB:
            emit(LocalSpirvOp::SpirvOpAtomicISub, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_SWAP:
            emit(LocalSpirvOp::SpirvOpAtomicExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_DEC:
            emit(LocalSpirvOp::SpirvOpAtomicIDecrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_INC:
            emit(LocalSpirvOp::SpirvOpAtomicIIncrement, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_SMAX:
            emit(LocalSpirvOp::SpirvOpAtomicSMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_SMIN:
            emit(LocalSpirvOp::SpirvOpAtomicSMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_UMAX:
            emit(LocalSpirvOp::SpirvOpAtomicUMax, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_UMIN:
            emit(LocalSpirvOp::SpirvOpAtomicUMin, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_FMIN:
            emit(LocalSpirvOp::SpirvOpAtomicFMinEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_FMAX:
            emit(LocalSpirvOp::SpirvOpAtomicFMaxEXT, {typeFloat32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_ATOMIC_CMPSWAP:
        case RdnaInstruction::IMAGE_ATOMIC_FCMPSWAP:
            emit(LocalSpirvOp::SpirvOpAtomicCompareExchange, {typeInt32, node.resultId, node.pointerId, scopeDevice, semanticsUniform, semanticsUniform, node.srcIds[0], node.srcIds[1]}); return true;

        case RdnaInstruction::IMAGE_LOAD:
        case RdnaInstruction::IMAGE_LOAD_BY2:
        case RdnaInstruction::IMAGE_LOAD_BY4:
        case RdnaInstruction::IMAGE_LOAD_MIP:
        case RdnaInstruction::IMAGE_LOAD_MIP_BY2:
        case RdnaInstruction::IMAGE_LOAD_MIP_BY4:
        case RdnaInstruction::IMAGE_LOAD_MIP_PCK:
        case RdnaInstruction::IMAGE_LOAD_MIP_PCK2:
        case RdnaInstruction::IMAGE_LOAD_MIP_PCK4:
        case RdnaInstruction::IMAGE_LOAD_MIP_PCK_SGN:
        case RdnaInstruction::IMAGE_LOAD_PCK:
        case RdnaInstruction::IMAGE_LOAD_PCK2:
        case RdnaInstruction::IMAGE_LOAD_PCK4:
        case RdnaInstruction::IMAGE_LOAD_PCK_SGN:
        case RdnaInstruction::IMAGE_MSAA_LOAD:
            emit(LocalSpirvOp::SpirvOpImageRead, {typeVec4F32, node.resultId, node.pointerId, node.srcIds[0]}); return true;

        case RdnaInstruction::IMAGE_STORE:
        case RdnaInstruction::IMAGE_STORE_BY2:
        case RdnaInstruction::IMAGE_STORE_BY4:
        case RdnaInstruction::IMAGE_STORE_MIP:
        case RdnaInstruction::IMAGE_STORE_MIP_BY2:
        case RdnaInstruction::IMAGE_STORE_MIP_BY4:
        case RdnaInstruction::IMAGE_STORE_MIP_PCK:
        case RdnaInstruction::IMAGE_STORE_MIP_PCK2:
        case RdnaInstruction::IMAGE_STORE_MIP_PCK4:
        case RdnaInstruction::IMAGE_STORE_PCK:
        case RdnaInstruction::IMAGE_STORE_PCK2:
        case RdnaInstruction::IMAGE_STORE_PCK4:
            emit(LocalSpirvOp::SpirvOpImageWrite, {node.pointerId, node.srcIds[0], node.srcIds[1]}); return true;

        case RdnaInstruction::IMAGE_GET_LOD:
            emit(LocalSpirvOp::SpirvOpImageQueryLod, {typeVec2F32, node.resultId, node.pointerId, node.srcIds[0]}); return true;
        case RdnaInstruction::IMAGE_GET_RESINFO:
            emit(LocalSpirvOp::SpirvOpImageQuerySize, {typeVec4F32, node.resultId, node.pointerId}); return true;

        case RdnaInstruction::IMAGE_BVH_INTERSECT_RAY:
        case RdnaInstruction::IMAGE_BVH64_INTERSECT_RAY:
            return false;

        case RdnaInstruction::IMAGE_GATHER4:
        case RdnaInstruction::IMAGE_GATHER4H:
        case RdnaInstruction::IMAGE_GATHER4H_PCK:
        case RdnaInstruction::IMAGE_GATHER4_B:
        case RdnaInstruction::IMAGE_GATHER4_B_CL:
        case RdnaInstruction::IMAGE_GATHER4_B_CL_O:
        case RdnaInstruction::IMAGE_GATHER4_B_O:
        case RdnaInstruction::IMAGE_GATHER4_C:
        case RdnaInstruction::IMAGE_GATHER4_CL:
        case RdnaInstruction::IMAGE_GATHER4_CL_O:
        case RdnaInstruction::IMAGE_GATHER4_C_B:
        case RdnaInstruction::IMAGE_GATHER4_C_B_CL:
        case RdnaInstruction::IMAGE_GATHER4_C_B_CL_O:
        case RdnaInstruction::IMAGE_GATHER4_C_B_O:
        case RdnaInstruction::IMAGE_GATHER4_C_CL:
        case RdnaInstruction::IMAGE_GATHER4_C_CL_O:
        case RdnaInstruction::IMAGE_GATHER4_C_L:
        case RdnaInstruction::IMAGE_GATHER4_C_LZ:
        case RdnaInstruction::IMAGE_GATHER4_C_LZ_O:
        case RdnaInstruction::IMAGE_GATHER4_C_L_O:
        case RdnaInstruction::IMAGE_GATHER4_C_O:
        case RdnaInstruction::IMAGE_GATHER4_L:
        case RdnaInstruction::IMAGE_GATHER4_LZ:
        case RdnaInstruction::IMAGE_GATHER4_LZ_O:
        case RdnaInstruction::IMAGE_GATHER4_L_O:
        case RdnaInstruction::IMAGE_GATHER4_O:
        case RdnaInstruction::IMAGE_GATHER8H_PCK:
            emit(LocalSpirvOp::SpirvOpImageGather, {typeVec4F32, node.resultId, node.pointerId, node.srcIds[0]}); return true;

        case RdnaInstruction::IMAGE_SAMPLE:
        case RdnaInstruction::IMAGE_SAMPLE_B:
        case RdnaInstruction::IMAGE_SAMPLE_B_CL:
        case RdnaInstruction::IMAGE_SAMPLE_B_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_B_O:
        case RdnaInstruction::IMAGE_SAMPLE_C:
        case RdnaInstruction::IMAGE_SAMPLE_CD:
        case RdnaInstruction::IMAGE_SAMPLE_CD_CL:
        case RdnaInstruction::IMAGE_SAMPLE_CD_CL_G16:
        case RdnaInstruction::IMAGE_SAMPLE_CD_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_CD_CL_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_CD_G16:
        case RdnaInstruction::IMAGE_SAMPLE_CD_O:
        case RdnaInstruction::IMAGE_SAMPLE_CD_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_CL:
        case RdnaInstruction::IMAGE_SAMPLE_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_B:
        case RdnaInstruction::IMAGE_SAMPLE_C_B_CL:
        case RdnaInstruction::IMAGE_SAMPLE_C_B_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_B_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_CL:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_CL_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_CL_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_CD_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_CL:
        case RdnaInstruction::IMAGE_SAMPLE_C_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_D:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_CL:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_CL_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_CL_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_D_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_C_L:
        case RdnaInstruction::IMAGE_SAMPLE_C_LZ:
        case RdnaInstruction::IMAGE_SAMPLE_C_LZ_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_L_O:
        case RdnaInstruction::IMAGE_SAMPLE_C_O:
        case RdnaInstruction::IMAGE_SAMPLE_D:
        case RdnaInstruction::IMAGE_SAMPLE_D_CL:
        case RdnaInstruction::IMAGE_SAMPLE_D_CL_G16:
        case RdnaInstruction::IMAGE_SAMPLE_D_CL_O:
        case RdnaInstruction::IMAGE_SAMPLE_D_CL_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_D_G16:
        case RdnaInstruction::IMAGE_SAMPLE_D_O:
        case RdnaInstruction::IMAGE_SAMPLE_D_O_G16:
        case RdnaInstruction::IMAGE_SAMPLE_L:
        case RdnaInstruction::IMAGE_SAMPLE_LZ:
        case RdnaInstruction::IMAGE_SAMPLE_LZ_O:
        case RdnaInstruction::IMAGE_SAMPLE_L_O:
        case RdnaInstruction::IMAGE_SAMPLE_O:
            emit(LocalSpirvOp::SpirvOpImageSampleExplicitLod, {typeVec4F32, node.resultId, node.pointerId, node.srcIds[0]});
            return true;

        case RdnaInstruction::SCRATCH_LOAD_DWORD:
        case RdnaInstruction::SCRATCH_LOAD_SBYTE:
        case RdnaInstruction::SCRATCH_LOAD_SBYTE_D16:
        case RdnaInstruction::SCRATCH_LOAD_SBYTE_D16_HI:
        case RdnaInstruction::SCRATCH_LOAD_SHORT_D16:
        case RdnaInstruction::SCRATCH_LOAD_SHORT_D16_HI:
        case RdnaInstruction::SCRATCH_LOAD_SSHORT:
        case RdnaInstruction::SCRATCH_LOAD_UBYTE:
        case RdnaInstruction::SCRATCH_LOAD_UBYTE_D16:
        case RdnaInstruction::SCRATCH_LOAD_UBYTE_D16_HI:
        case RdnaInstruction::SCRATCH_LOAD_USHORT:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::SCRATCH_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::SCRATCH_LOAD_DWORDX3:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec3F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::SCRATCH_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId});
            return true;

        case RdnaInstruction::SCRATCH_STORE_BYTE:
        case RdnaInstruction::SCRATCH_STORE_BYTE_D16_HI:
        case RdnaInstruction::SCRATCH_STORE_DWORD:
        case RdnaInstruction::SCRATCH_STORE_DWORDX2:
        case RdnaInstruction::SCRATCH_STORE_DWORDX3:
        case RdnaInstruction::SCRATCH_STORE_DWORDX4:
        case RdnaInstruction::SCRATCH_STORE_SHORT:
        case RdnaInstruction::SCRATCH_STORE_SHORT_D16_HI:
            emit(LocalSpirvOp::SpirvOpStore, {node.pointerId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_ABS_I32:
            emit(LocalSpirvOp::SpirvOpSAbs, {typeInt32, node.resultId, node.srcIds[0]});
            return true;
        case RdnaInstruction::S_ADD_I32:
        case RdnaInstruction::S_ADD_U32:
        case RdnaInstruction::S_ADDK_I32:
            emit(LocalSpirvOp::SpirvOpIAdd, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_ADDC_U32:
            emit(LocalSpirvOp::SpirvOpIAddCarry, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_AND_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_AND_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_ANDN2_B32:
            // A AND NOT B
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); // Second operand inverted via separate OpNot in backend
            return true;
        case RdnaInstruction::S_ANDN2_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_ASHR_I32:
            emit(LocalSpirvOp::SpirvOpShiftRightArithmetic, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_ASHR_I64:
            emit(LocalSpirvOp::SpirvOpShiftRightArithmetic, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_BCNT1_I32_B32:
            emit(LocalSpirvOp::SpirvOpBitCount, {typeInt32, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_BFE_U32:
            emit(LocalSpirvOp::SpirvOpBitFieldUExtract, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]});
            return true;
        case RdnaInstruction::S_BFE_I32:
            emit(LocalSpirvOp::SpirvOpBitFieldSExtract, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]});
            return true;

        case RdnaInstruction::S_ABSDIFF_I32:
        case RdnaInstruction::S_BCNT0_I32_B32:
        case RdnaInstruction::S_BCNT0_I32_B64:
        case RdnaInstruction::S_BCNT1_I32_B64:
        case RdnaInstruction::S_BFE_I64:
        case RdnaInstruction::S_BFE_U64:
        case RdnaInstruction::S_BFM_B32:
        case RdnaInstruction::S_BFM_B64:
            return false;

        case RdnaInstruction::S_ANDN1_SAVEEXEC_B32:
        case RdnaInstruction::S_ANDN1_SAVEEXEC_B64:
        case RdnaInstruction::S_ANDN1_WREXEC_B32:
        case RdnaInstruction::S_ANDN1_WREXEC_B64:
        case RdnaInstruction::S_ANDN2_SAVEEXEC_B32:
        case RdnaInstruction::S_ANDN2_SAVEEXEC_B64:
        case RdnaInstruction::S_ANDN2_WREXEC_B32:
        case RdnaInstruction::S_ANDN2_WREXEC_B64:
        case RdnaInstruction::S_AND_SAVEEXEC_B32:
        case RdnaInstruction::S_AND_SAVEEXEC_B64:
            return false;

        case RdnaInstruction::S_BARRIER:
            emit(LocalSpirvOp::SpirvOpControlBarrier, {scopeWorkgroup, scopeWorkgroup, semanticsNone});
            return true;

        case RdnaInstruction::S_ATC_PROBE:
        case RdnaInstruction::S_ATC_PROBE_BUFFER:
            return false;
        case RdnaInstruction::S_BREV_B32:
            emit(LocalSpirvOp::SpirvOpBitReverse, {typeInt32, node.resultId, node.srcIds[0]});
            return true;
        case RdnaInstruction::S_BREV_B64:
            emit(LocalSpirvOp::SpirvOpBitReverse, {typeInt64, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_BITSET0_B32:
        case RdnaInstruction::S_BITSET0_B64:
        case RdnaInstruction::S_BITSET1_B32:
        case RdnaInstruction::S_BITSET1_B64:
            emit(LocalSpirvOp::SpirvOpBitFieldInsert, {typeInt32, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_BITCMP0_B32:
        case RdnaInstruction::S_BITCMP0_B64:
        case RdnaInstruction::S_BITCMP1_B32:
        case RdnaInstruction::S_BITCMP1_B64:
        case RdnaInstruction::S_BITREPLICATE_B64_B32:
            return false;

        case RdnaInstruction::S_BRANCH:
            emit(LocalSpirvOp::SpirvOpBranch, {node.srcIds[0]});
            return true;
        case RdnaInstruction::S_BUFFER_LOAD_DWORD:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::S_BUFFER_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::S_BUFFER_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId}); return true;
        case RdnaInstruction::S_BUFFER_LOAD_DWORDX8:
            return false;
        case RdnaInstruction::S_BUFFER_LOAD_DWORDX16:
            return false;

        case RdnaInstruction::S_CBRANCH_SCC0:
        case RdnaInstruction::S_CBRANCH_SCC1:
        case RdnaInstruction::S_CBRANCH_VCCZ:
        case RdnaInstruction::S_CBRANCH_VCCNZ:
        case RdnaInstruction::S_CBRANCH_EXECZ:
        case RdnaInstruction::S_CBRANCH_EXECNZ:
            emit(LocalSpirvOp::SpirvOpBranchConditional, {node.srcIds[0], node.srcIds[1], node.srcIds[2]}); return true;

        case RdnaInstruction::S_CALL_B64:
        case RdnaInstruction::S_CBRANCH_CDBGSYS:
        case RdnaInstruction::S_CBRANCH_CDBGSYS_AND_USER:
        case RdnaInstruction::S_CBRANCH_CDBGSYS_OR_USER:
        case RdnaInstruction::S_CBRANCH_CDBGUSER:
        case RdnaInstruction::S_CLAUSE:
            return false;
        case RdnaInstruction::S_CMOV_B32:
        case RdnaInstruction::S_CMOVK_I32:
            emit(LocalSpirvOp::SpirvOpSelect, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]}); return true;
        case RdnaInstruction::S_CMOV_B64:
            emit(LocalSpirvOp::SpirvOpSelect, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]}); return true;

        case RdnaInstruction::S_CMPK_EQ_I32:
        case RdnaInstruction::S_CMPK_EQ_U32:
            emit(LocalSpirvOp::SpirvOpIEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_GE_I32:
            emit(LocalSpirvOp::SpirvOpSGreaterThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_GE_U32:
            emit(LocalSpirvOp::SpirvOpUGreaterThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_GT_I32:
            emit(LocalSpirvOp::SpirvOpSGreaterThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_GT_U32:
            emit(LocalSpirvOp::SpirvOpUGreaterThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_LE_I32:
            emit(LocalSpirvOp::SpirvOpSLessThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_LE_U32:
            emit(LocalSpirvOp::SpirvOpULessThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_LG_I32:
        case RdnaInstruction::S_CMPK_LG_U32:
            emit(LocalSpirvOp::SpirvOpINotEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_LT_I32:
            emit(LocalSpirvOp::SpirvOpSLessThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMPK_LT_U32:
            emit(LocalSpirvOp::SpirvOpULessThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;

        case RdnaInstruction::S_CMP_EQ_I32:
        case RdnaInstruction::S_CMP_EQ_U32:
            emit(LocalSpirvOp::SpirvOpIEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_EQ_U64:
            emit(LocalSpirvOp::SpirvOpIEqual, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_GE_I32:
            emit(LocalSpirvOp::SpirvOpSGreaterThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_GE_U32:
            emit(LocalSpirvOp::SpirvOpUGreaterThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_GT_I32:
            emit(LocalSpirvOp::SpirvOpSGreaterThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_GT_U32:
            emit(LocalSpirvOp::SpirvOpUGreaterThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LE_I32:
            emit(LocalSpirvOp::SpirvOpSLessThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LE_U32:
            emit(LocalSpirvOp::SpirvOpULessThanEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LG_I32:
        case RdnaInstruction::S_CMP_LG_U32:
            emit(LocalSpirvOp::SpirvOpINotEqual, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LG_U64:
            emit(LocalSpirvOp::SpirvOpINotEqual, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LT_I32:
            emit(LocalSpirvOp::SpirvOpSLessThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;
        case RdnaInstruction::S_CMP_LT_U32:
            emit(LocalSpirvOp::SpirvOpULessThan, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]}); return true;


        // ---- newly added scalar / SMEM / SOPP instructions ----
        case RdnaInstruction::S_NOP:
            emit(LocalSpirvOp::SpirvOpNop, {});
            return true;

        case RdnaInstruction::S_MOV_B32:
        case RdnaInstruction::S_MOVK_I32:
            emit(LocalSpirvOp::SpirvOpCopyObject, {typeInt32, node.resultId, node.srcIds[0]});
            return true;
        case RdnaInstruction::S_MOV_B64:
            emit(LocalSpirvOp::SpirvOpCopyObject, {typeInt64, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_CSELECT_B32:
            emit(LocalSpirvOp::SpirvOpSelect, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]});
            return true;
        case RdnaInstruction::S_CSELECT_B64:
            emit(LocalSpirvOp::SpirvOpSelect, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1], node.srcIds[2]});
            return true;

        case RdnaInstruction::S_NOT_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseNot, {typeInt32, node.resultId, node.srcIds[0]});
            return true;
        case RdnaInstruction::S_NOT_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseNot, {typeInt64, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_OR_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_OR_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_XOR_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseXor, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_XOR_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseXor, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_NAND_B32:
            // A NAND B  ==  NOT (A AND B)  – emit AND; backend can invert if needed
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_NAND_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseAnd, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_NOR_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_NOR_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_XNOR_B32:
            emit(LocalSpirvOp::SpirvOpBitwiseXor, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_XNOR_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseXor, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_ORN2_B32:
            // A OR NOT B
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_ORN2_B64:
            emit(LocalSpirvOp::SpirvOpBitwiseOr, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_LSHL_B32:
            emit(LocalSpirvOp::SpirvOpShiftLeftLogical, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_LSHL_B64:
            emit(LocalSpirvOp::SpirvOpShiftLeftLogical, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_LSHR_B32:
            emit(LocalSpirvOp::SpirvOpShiftRightLogical, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_LSHR_B64:
            emit(LocalSpirvOp::SpirvOpShiftRightLogical, {typeInt64, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_LSHL1_ADD_U32:
        case RdnaInstruction::S_LSHL2_ADD_U32:
        case RdnaInstruction::S_LSHL3_ADD_U32:
        case RdnaInstruction::S_LSHL4_ADD_U32:
            // (src0 << N) + src1  – approximate with IAdd; shift amount is immediate
            emit(LocalSpirvOp::SpirvOpIAdd, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_SUB_I32:
        case RdnaInstruction::S_SUB_U32:
            emit(LocalSpirvOp::SpirvOpISub, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_SUBB_U32:
            emit(LocalSpirvOp::SpirvOpISub, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_MUL_I32:
        case RdnaInstruction::S_MULK_I32:
            emit(LocalSpirvOp::SpirvOpIMul, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_MAX_I32:
            emit(LocalSpirvOp::SpirvOpSMax, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_MAX_U32:
            emit(LocalSpirvOp::SpirvOpUMax, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_MIN_I32:
            emit(LocalSpirvOp::SpirvOpSMin, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;
        case RdnaInstruction::S_MIN_U32:
            emit(LocalSpirvOp::SpirvOpUMin, {typeInt32, node.resultId, node.srcIds[0], node.srcIds[1]});
            return true;

        case RdnaInstruction::S_SEXT_I32_I8:
        case RdnaInstruction::S_SEXT_I32_I16:
            // sign-extend; treat as copy for now (backend can insert proper extend)
            emit(LocalSpirvOp::SpirvOpCopyObject, {typeInt32, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_LOAD_DWORD:
        case RdnaInstruction::S_SCRATCH_LOAD_DWORD:
            emit(LocalSpirvOp::SpirvOpLoad, {typeInt32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::S_LOAD_DWORDX2:
        case RdnaInstruction::S_SCRATCH_LOAD_DWORDX2:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec2F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::S_LOAD_DWORDX4:
        case RdnaInstruction::S_SCRATCH_LOAD_DWORDX4:
            emit(LocalSpirvOp::SpirvOpLoad, {typeVec4F32, node.resultId, node.pointerId});
            return true;
        case RdnaInstruction::S_LOAD_DWORDX8:
        case RdnaInstruction::S_LOAD_DWORDX16:
            return false; // too wide for current type helpers

        case RdnaInstruction::S_FF0_I32_B32:
        case RdnaInstruction::S_FF1_I32_B32:
        case RdnaInstruction::S_FLBIT_I32:
        case RdnaInstruction::S_FLBIT_I32_B32:
            // find-first-bit / find-last-bit – map to BitCount as a rough stand-in
            emit(LocalSpirvOp::SpirvOpBitCount, {typeInt32, node.resultId, node.srcIds[0]});
            return true;

        case RdnaInstruction::S_WQM_B32:
            emit(LocalSpirvOp::SpirvOpCopyObject, {typeInt32, node.resultId, node.srcIds[0]});
            return true;
        case RdnaInstruction::S_WQM_B64:
            emit(LocalSpirvOp::SpirvOpCopyObject, {typeInt64, node.resultId, node.srcIds[0]});
            return true;

        // Everything else (control, special, perf, wait, trap, etc.) is not mapped
        case RdnaInstruction::S_CODE_END:
        case RdnaInstruction::S_DCACHE_INV:
        case RdnaInstruction::S_DECPERFLEVEL:
        case RdnaInstruction::S_DENORM_MODE:
        case RdnaInstruction::S_ENDPGM:
        case RdnaInstruction::S_ENDPGM_ORDERED_PS_DONE:
        case RdnaInstruction::S_ENDPGM_SAVED:
        case RdnaInstruction::S_FF0_I32_B64:
        case RdnaInstruction::S_FF1_I32_B64:
        case RdnaInstruction::S_FLBIT_I32_B64:
        case RdnaInstruction::S_FLBIT_I32_I64:
        case RdnaInstruction::S_GETPC_B64:
        case RdnaInstruction::S_GETREG_B32:
        case RdnaInstruction::S_GET_WAVEID_IN_WORKGROUP:
        case RdnaInstruction::S_GL1_INV:
        case RdnaInstruction::S_ICACHE_INV:
        case RdnaInstruction::S_INCPERFLEVEL:
        case RdnaInstruction::S_INST_PREFETCH:
        case RdnaInstruction::S_MEMREALTIME:
        case RdnaInstruction::S_MEMTIME:
        case RdnaInstruction::S_MOVRELD_B32:
        case RdnaInstruction::S_MOVRELD_B64:
        case RdnaInstruction::S_MOVRELSD_2_B32:
        case RdnaInstruction::S_MOVRELS_B32:
        case RdnaInstruction::S_MOVRELS_B64:
        case RdnaInstruction::S_MUL_HI_I32:
        case RdnaInstruction::S_MUL_HI_U32:
        case RdnaInstruction::S_NAND_SAVEEXEC_B32:
        case RdnaInstruction::S_NAND_SAVEEXEC_B64:
        case RdnaInstruction::S_NOR_SAVEEXEC_B32:
        case RdnaInstruction::S_NOR_SAVEEXEC_B64:
        case RdnaInstruction::S_ORN1_SAVEEXEC_B32:
        case RdnaInstruction::S_ORN1_SAVEEXEC_B64:
        case RdnaInstruction::S_ORN2_SAVEEXEC_B32:
        case RdnaInstruction::S_ORN2_SAVEEXEC_B64:
        case RdnaInstruction::S_OR_SAVEEXEC_B32:
        case RdnaInstruction::S_OR_SAVEEXEC_B64:
        case RdnaInstruction::S_PACK_HH_B32_B16:
        case RdnaInstruction::S_PACK_LH_B32_B16:
        case RdnaInstruction::S_PACK_LL_B32_B16:
        case RdnaInstruction::S_QUADMASK_B32:
        case RdnaInstruction::S_QUADMASK_B64:
        case RdnaInstruction::S_RFE_B64:
        case RdnaInstruction::S_ROUND_MODE:
        case RdnaInstruction::S_SENDMSG:
        case RdnaInstruction::S_SENDMSGHALT:
        case RdnaInstruction::S_SETHALT:
        case RdnaInstruction::S_SETKILL:
        case RdnaInstruction::S_SETPC_B64:
        case RdnaInstruction::S_SETPRIO:
        case RdnaInstruction::S_SETREG_B32:
        case RdnaInstruction::S_SETREG_IMM32_B32:
        case RdnaInstruction::S_SLEEP:
        case RdnaInstruction::S_SUBVECTOR_LOOP_BEGIN:
        case RdnaInstruction::S_SUBVECTOR_LOOP_END:
        case RdnaInstruction::S_SWAPPC_B64:
        case RdnaInstruction::S_TRAP:
        case RdnaInstruction::S_TTRACEDATA:
        case RdnaInstruction::S_TTRACEDATA_IMM:
        case RdnaInstruction::S_VERSION:
        case RdnaInstruction::S_WAITCNT:
        case RdnaInstruction::S_WAITCNT_DEPCTR:
        case RdnaInstruction::S_WAITCNT_EXPCNT:
        case RdnaInstruction::S_WAITCNT_LGKMCNT:
        case RdnaInstruction::S_WAITCNT_VMCNT:
        case RdnaInstruction::S_WAITCNT_VSCNT:
        case RdnaInstruction::S_WAIT_IDLE:
        case RdnaInstruction::S_WAKEUP:
        case RdnaInstruction::S_XNOR_SAVEEXEC_B32:
        case RdnaInstruction::S_XNOR_SAVEEXEC_B64:
        case RdnaInstruction::S_XOR_SAVEEXEC_B32:
        case RdnaInstruction::S_XOR_SAVEEXEC_B64:
            return false;

        default:
            break;
    }
    return false;
}