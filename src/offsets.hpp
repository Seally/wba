#pragma once

#include <REL/Relocation.h>

#include "global.hpp"

namespace re {
    // SkyrimSE.exe + 0x8daab0 [1.5.97.0]  [ADDRLIB:52058]
    // SkyrimSE.exe + 0x90a1c0 [1.6.323.0] [ADDRLIB:52944]
    constexpr REL::ID IsSurvival(VERSION_SPECIFIC(52058, 52944));

    namespace TESBoundObject {
        // SkyrimSE.exe + 0x1a1730 [1.5.97.0]  [ADDRLIB:14809]
        // SkyrimSE.exe + 0x1acf50 [1.5.323.0] [ADDRLIB:14988]
        constexpr REL::ID GetWeight(VERSION_SPECIFIC(14809, 14988));
    } // namespace TESBoundObject

    namespace fix {
        namespace survivalboundarrowweight {
            constexpr std::ptrdiff_t patchOffset = VERSION_SPECIFIC(0x85, 0x77);
            constexpr std::ptrdiff_t continueOffset =
                VERSION_SPECIFIC(0xd1, 0xb1);
            constexpr std::ptrdiff_t exitOffset = VERSION_SPECIFIC(0xdb, 0xbf);
        } // namespace survivalboundarrowweight
    } // namespace fix
} // namespace re
