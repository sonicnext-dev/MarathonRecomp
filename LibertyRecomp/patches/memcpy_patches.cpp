// memcpy_patches.cpp
// Native implementations of memory functions that were recompiled incorrectly
// due to control flow issues in the original PPC code

#include <cpu/ppc_context.h>
#include <kernel/memory.h>
#include <cstring>

// sub_82990830 is a memcpy function that the recompiler couldn't handle correctly
// because it branches to addresses that ended up in different recompiled functions.
// The function signature is: void memcpy(dst: r3, src: r4, size: r5)
// We provide a native implementation that just does the copy correctly.

// Forward-declare the __imp__ function so we can call it as a fallback
extern "C" PPC_FUNC(__imp__sub_82990830);

// Override the weak symbol with a strong one
PPC_FUNC(sub_82990830) {
    uint8_t* dst = base + ctx.r3.u32;
    uint8_t* src = base + ctx.r4.u32;
    size_t size = ctx.r5.u32;
    
    // Perform the actual memcpy using native code
    if (size > 0 && dst != nullptr && src != nullptr) {
        std::memcpy(dst, src, size);
    }
    
    // The original function stores r3 to -8(r1) at the beginning
    // but the return value for memcpy-like functions is typically the dst pointer
    // ctx.r3 already contains the destination, so we don't need to modify it
}

// sub_82990880 is also part of the memcpy family with the same issue
// It's called from sub_82990830 when r6 == 4
extern "C" PPC_FUNC(__imp__sub_82990880);

PPC_FUNC(sub_82990880) {
    // This is the same memcpy logic - just copy r5 bytes from r4 to r3
    // But first we need to handle the alignment case where r6 bytes were already copied
    uint8_t* dst = base + ctx.r3.u32;
    uint8_t* src = base + ctx.r4.u32;
    size_t size = ctx.r5.u32 - ctx.r6.u32; // r5 - r6 (remaining size after alignment)
    
    if (size > 0 && dst != nullptr && src != nullptr) {
        std::memcpy(dst, src, size);
    }
}
