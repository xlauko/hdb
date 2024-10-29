#ifndef DEFINE_REGISTER
#error "This file is intended for textual inclusion with the\
DEFINE_REGISTER macro defined"
#endif

// Offsets for ARM64 General-Purpose Registers using arm_thread_state64_t
#define GPR_OFFSET(reg) (offsetof(arm_thread_state64_t, reg))

// ARM64 register definitions using appropriate offsets and sizes
#define DEFINE_GPR_64(name, dwarf_id) \
    DEFINE_REGISTER(name, dwarf_id, 8, gpr, uint)

#define DEFINE_GPR_32(name, super) \
    DEFINE_REGISTER(name, -1, 4, sub_gpr, uint)

// Offsets for ARM64 Floating Point and SIMD Registers using arm_neon_state64_t
#define FPR_OFFSET(reg) (offsetof(arm_neon_state64_t, reg))
#define FPR_SIZE(reg) (sizeof(((arm_neon_state64_t*)0)->reg))

#define DEFINE_FPR(name, dwarf_id, user_name) \
    DEFINE_REGISTER(name, dwarf_id, FPR_SIZE(user_name), FPR_OFFSET(user_name),\
    register_type::fpr, register_format::uint)

#define DEFINE_FP_VREG(number) \
    DEFINE_REGISTER(v ## number, (17 + number), 16,\
     (FPR_OFFSET(__v[number])), register_type::fpr,\
     register_format::vector)

// ARM64 General-Purpose Register Definitions
DEFINE_GPR_64(x0, 0),
DEFINE_GPR_64(x1, 1),
DEFINE_GPR_64(x2, 2),
DEFINE_GPR_64(x3, 3),
DEFINE_GPR_64(x4, 4),
DEFINE_GPR_64(x5, 5),
DEFINE_GPR_64(x6, 6),
DEFINE_GPR_64(x7, 7),
DEFINE_GPR_64(x8, 8),
DEFINE_GPR_64(x9, 9),
DEFINE_GPR_64(x10, 10),
DEFINE_GPR_64(x11, 11),
DEFINE_GPR_64(x12, 12),
DEFINE_GPR_64(x13, 13),
DEFINE_GPR_64(x14, 14),
DEFINE_GPR_64(x15, 15),
DEFINE_GPR_64(x16, 16),
DEFINE_GPR_64(x17, 17),
DEFINE_GPR_64(x18, 18),
DEFINE_GPR_64(x19, 19),
DEFINE_GPR_64(x20, 20),
DEFINE_GPR_64(x21, 21),
DEFINE_GPR_64(x22, 22),
DEFINE_GPR_64(x23, 23),
DEFINE_GPR_64(x24, 24),
DEFINE_GPR_64(x25, 25),
DEFINE_GPR_64(x26, 26),
DEFINE_GPR_64(x27, 27),
DEFINE_GPR_64(x28, 28),
DEFINE_GPR_64(fp, 29),  // Frame pointer
DEFINE_GPR_64(lr, 30),  // Link register
DEFINE_GPR_64(sp, 31),  // Stack pointer
DEFINE_GPR_64(pc, 32),  // Program counter

// Define 32-bit equivalents for registers
DEFINE_GPR_32(w0, x0), DEFINE_GPR_32(w1, x1),
DEFINE_GPR_32(w2, x2), DEFINE_GPR_32(w3, x3),
DEFINE_GPR_32(w4, x4), DEFINE_GPR_32(w5, x5),
DEFINE_GPR_32(w6, x6), DEFINE_GPR_32(w7, x7),
DEFINE_GPR_32(w8, x8), DEFINE_GPR_32(w9, x9),
DEFINE_GPR_32(w10, x10), DEFINE_GPR_32(w11, x11),
DEFINE_GPR_32(w12, x12), DEFINE_GPR_32(w13, x13),
DEFINE_GPR_32(w14, x14), DEFINE_GPR_32(w15, x15),
DEFINE_GPR_32(w16, x16), DEFINE_GPR_32(w17, x17),
DEFINE_GPR_32(w18, x18), DEFINE_GPR_32(w19, x19),
DEFINE_GPR_32(w20, x20), DEFINE_GPR_32(w21, x21),
DEFINE_GPR_32(w22, x22), DEFINE_GPR_32(w23, x23),
DEFINE_GPR_32(w24, x24), DEFINE_GPR_32(w25, x25),
DEFINE_GPR_32(w26, x26), DEFINE_GPR_32(w27, x27),
DEFINE_GPR_32(w28, x28), DEFINE_GPR_32(w29, fp),
DEFINE_GPR_32(w30, lr),

// ARM64 Floating Point and SIMD Register Definitions
// DEFINE_FP_VREG(0), DEFINE_FP_VREG(1), DEFINE_FP_VREG(2), DEFINE_FP_VREG(3),
// DEFINE_FP_VREG(4), DEFINE_FP_VREG(5), DEFINE_FP_VREG(6), DEFINE_FP_VREG(7),
// DEFINE_FP_VREG(8), DEFINE_FP_VREG(9), DEFINE_FP_VREG(10), DEFINE_FP_VREG(11),
// DEFINE_FP_VREG(12), DEFINE_FP_VREG(13), DEFINE_FP_VREG(14), DEFINE_FP_VREG(15),
// DEFINE_FP_VREG(16), DEFINE_FP_VREG(17), DEFINE_FP_VREG(18), DEFINE_FP_VREG(19),
// DEFINE_FP_VREG(20), DEFINE_FP_VREG(21), DEFINE_FP_VREG(22), DEFINE_FP_VREG(23),
// DEFINE_FP_VREG(24), DEFINE_FP_VREG(25), DEFINE_FP_VREG(26), DEFINE_FP_VREG(27),
// DEFINE_FP_VREG(28), DEFINE_FP_VREG(29), DEFINE_FP_VREG(30), DEFINE_FP_VREG(31),

// ARM64 Debug Register Offsets and Definitions using arm_debug_state64_t
// #define DR_OFFSET(number) (offsetof(arm_debug_state64_t, __bvr[number]))
// #define DEFINE_DR(number)\
//     DEFINE_REGISTER(dr ## number, -1, 8, DR_OFFSET(number),\
//      register_type::dr, register_format::uint)

// DEFINE_DR(0), DEFINE_DR(1), DEFINE_DR(2), DEFINE_DR(3),
// DEFINE_DR(4), DEFINE_DR(5), DEFINE_DR(6), DEFINE_DR(7)
