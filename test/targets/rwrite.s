// Declare the global entry point _main (required underscore on macOS)
.global _main

.section __TEXT,__text,regular,pure_instructions

_main:
    // Move the immediate value 0 into the w0 register (the lower 32-bits of x0)
    // In AArch64, w0 (32-bit) or x0 (64-bit) is used for function return values.
    // Since we are returning a small integer (0), using w0 (32 bits) is more efficient
    mov w0, #0

    // Return to the caller
    // The 'ret' instruction pops the return address from the link register (x30)
    // and jumps to that address. Since we have no function calls here, no need to save x30.
    ret
