//
// Copyright 2013 Google Inc. All Rights Reserved.
//

#ifndef WV_UUID_H_
#define WV_UUID_H_

#include <array>
#include <cstdint>
#include <vector>

namespace wvdrm {

std::vector<std::array<uint8_t, 16>> getSupportedCryptoSchemes();

} // namespace wvdrm

#endif // WV_UUID_H_
