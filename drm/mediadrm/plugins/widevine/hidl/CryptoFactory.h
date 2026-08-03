/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WIDEVINE_CRYPTO_FACTORY_H_
#define WIDEVINE_CRYPTO_FACTORY_H_

#include <android/hardware/drm/1.0/ICryptoPlugin.h>
#include <android/hardware/drm/1.3/ICryptoFactory.h>
#include <hidl/Status.h>
#include <media/hardware/CryptoAPI.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_3 {
namespace widevine {

using ::android::hardware::drm::V1_3::ICryptoFactory;
using ::android::hardware::drm::V1_0::ICryptoPlugin;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct CryptoFactory : public ICryptoFactory {
    CryptoFactory();
    virtual ~CryptoFactory() {}

    // Methods from ::android::hardware::drm::V1_0::ICryptoFactory follow.

    Return<bool> isCryptoSchemeSupported(const hidl_array<uint8_t, 16>& uuid)
            override;

    Return<void> createPlugin(const hidl_array<uint8_t, 16>& uuid,
            const hidl_vec<uint8_t>& initData, createPlugin_cb _hidl_cb)
            override;

private:
	android::CryptoFactory* mWVCryptoFactory;

    CryptoFactory(const CryptoFactory &) = delete;
    void operator=(const CryptoFactory &) = delete;
};

extern "C" ICryptoFactory* HIDL_FETCH_ICryptoFactory(const char* name);

}  // namespace widevine
}  // namespace V1_3
}  // namespace drm
}  // namespace hardware
}  // namespace android

#endif  // WIDEVINE_CRYPTO_FACTORY_H_
