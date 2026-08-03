//
// Copyright 2013 Google Inc. All Rights Reserved.
//

#include "WVUUID.h"

#include <string.h>

namespace wvdrm {

std::vector<std::array<uint8_t, 16>> getSupportedCryptoSchemes() {
    const std::array<uint8_t, 16> kWidevineUUID{
        0xED,0xEF,0x8B,0xA9,0x79,0xD6,0x4A,0xCE,
        0xA3,0xC8,0x27,0xDC,0xD5,0x1D,0x21,0xED
    };

    // To be used in mpd to specify drm scheme for players
    const std::array<uint8_t, 16> kOldNetflixWidevineUUID{
        0x29,0x70,0x1F,0xE4,0x3C,0xC7,0x4A,0x34,
        0x8C,0x5B,0xAE,0x90,0xC7,0x43,0x9A,0x47
    };

    return {kWidevineUUID, kOldNetflixWidevineUUID};
}

} // namespace wvdrm
