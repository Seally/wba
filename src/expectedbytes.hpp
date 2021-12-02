#pragma once

#include <cstdint>

#include "global.hpp"

namespace re {
    namespace fix {
        namespace survivalboundarrowweight {
            constexpr std::uint8_t expectedPatchBytes[] = {
            // clang-format off
                // .text:00000001401A17B5 [1.5.97.0]
                // .text:00000001401ACFC7 [1.6.323.0]

#ifdef SKYRIM_VERSION_SE
                // [1.5.97.0]  loc_1401A17B5:
                0x48, 0x8d, 0xbb, 0xb0, 0x00, 0x00, 0x00, // lea     rdi, [rbx+0B0h]     ; rbx is TESBoundObject*
                0xeb, 0x43,                               // jmp     short loc_1401A1801
#elif SKYRIM_VERSION_AE
                0xb8, 0xb0, 0x00, 0x00, 0x00,             // mov     eax, 0B0h
                0xeb, 0x33,                               // jmp     short loc_1401AD001
#endif
                // [1.6.323.0]  loc_1401ACFC7:
                // clang-format on
            };

            constexpr std::uint8_t expectedContinueBytes[] = {
            // clang-format off
                // .text:00000001401A1801 [1.5.97.0]
#ifdef SKYRIM_VERSION_SE
                0x48, 0x85, 0xff,                         // test    rdi, rdi
#elif SKYRIM_VERSION_AE
                0x48, 0x8d, 0x0c, 0x03,                   // lea     rcx, [rbx+rax]
                0x48, 0x85, 0xc9,                         // test    rcx, rcx
#endif
                // clang-format on
            };

            constexpr std::uint8_t expectedExitBytes[] = {
            // clang-format off
                // .text:00000001401A180B [1.5.97.0]
#ifdef SKYRIM_VERSION_SE
                0x48, 0x8b, 0x7c, 0x24, 0x40,             // mov     rdi, [rsp+38h+arg_0]
#elif SKYRIM_VERSION_AE
                0x0f, 0x28, 0xc6,                         // movaps  xmm0, xmm6
                0x0f, 0x28, 0x74, 0x24, 0x20,             // movaps  xmm6, [rsp+38h+var_18]
#endif
                // clang-format on
            };
        } // namespace survivalboundarrowweight
    } // end namespace fix
} // end namespace re
