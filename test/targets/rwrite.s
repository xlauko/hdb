.global _main              // Declare the global entry point _main for macOS (underscore is required)

.section __TEXT,__text,regular,pure_instructions
    // .section __TEXT,__text: This section directive tells the assembler that the following code is placed in the __TEXT segment.
    // __TEXT is the main executable segment on macOS, and __text is the specific subsection where code (pure instructions) resides.
    // 'regular' and 'pure_instructions' indicate that this section contains only regular executable code (without data or mixed content).

_main:
    // Start of function
    // Save the frame pointer (x29) and link register (x30) onto the stack
    // x29 is used as the frame pointer, and x30 holds the return address for this function
    // Pre-decrement the stack pointer by 16 to allocate space and store both registers
    stp     x29, x30, [sp, #-16]!

    // Set up the new stack frame by copying the current stack pointer (sp) to the frame pointer (x29)
    // This marks the current location of the stack for use within this function
    mov     x29, sp

    // System call: getpid()
    // The system call number for getpid() in AArch64 macOS is 20
    // Load this syscall number into x8, the designated register for system call numbers in AArch64
    mov     x8, #20                // Load the system call number (20) into x8 (getpid())

    // Make the system call using the svc (supervisor call) instruction
    // The syscall number is already in x8, and the result (PID) will be returned in x0
    // SVC is used to switch to privileged mode and execute the syscall
    svc     #0                     // Issue the system call, triggering a switch to kernel mode

    // Store the returned process ID from the getpid syscall
    // The result of the syscall is placed in x0 by convention (x0 holds return values)
    // We move it to w8 to store it for future use or debugging
    mov     w8, w0                 // Move the process ID (returned in x0) into w8 for later use

    // Prepare to return 0 from the main function
    // By convention, the return value of the main function is stored in w0
    // Since w0 is used to return values to the caller (OS), we move 0 into w0 (indicating successful execution)
    mov     w0, wzr                // Set w0 to 0 (wzr is the zero register which always contains 0)

    // Restore the stack and return to the caller
    // We need to restore the original frame pointer (x29) and link register (x30)
    // ldp is used to load the previous values of x29 and x30 from the stack
    // Post-increment the stack pointer by 16 (effectively deallocating the 16 bytes we allocated earlier)
    ldp     x29, x30, [sp], #16    // Restore frame pointer and return address, deallocate stack space

    // Return from the function
    // The ret instruction uses the address in x30 (the link register) to return control to the caller
    ret                            // Return to the caller (OS) using the value in x30 as the return address
