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
#define LOG_TAG "hidl_WidevineCryptoFactory"

#include "CryptoPlugin.h"
#include "CryptoFactory.h"
#include "TypeConvert.h"
#include "WVCreatePluginFactories.h"

#include <log/log.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_3 {
namespace widevine {

using ::android::hardware::drm::V1_0::Status;
using ::android::hardware::drm::V1_2::widevine::CryptoPlugin;

    CryptoFactory::CryptoFactory() {
		mWVCryptoFactory = createCryptoFactory();
    }

    // Methods from ::android::hardware::drm::V1_0::ICryptoFactory follow.
    Return<bool> CryptoFactory::isCryptoSchemeSupported(
            const hidl_array<uint8_t, 16>& uuid) {
        if (mWVCryptoFactory->isCryptoSchemeSupported(uuid.data())) {
            return true;
        }
        return false;
    }

    Return<void> CryptoFactory::createPlugin(const hidl_array<uint8_t, 16>& uuid,
            const hidl_vec<uint8_t>& initData, createPlugin_cb _hidl_cb) {

        if (mWVCryptoFactory->isCryptoSchemeSupported(uuid.data())) {
            android::CryptoPlugin *legacyPlugin = NULL;
            status_t status = mWVCryptoFactory->createPlugin(uuid.data(),
                        initData.data(), initData.size(), &legacyPlugin);

            CryptoPlugin *newPlugin = NULL;
            if (legacyPlugin == NULL) {
                ALOGE("Crypto legacy HAL:  failed to create crypto plugin");
            } else {
                newPlugin = new CryptoPlugin(legacyPlugin);
            }
            _hidl_cb(V1_2::widevine::toStatus(status), newPlugin);
            return Void();
        }

        _hidl_cb(Status::ERROR_DRM_CANNOT_HANDLE, NULL);
        return Void();
    }

    ICryptoFactory* HIDL_FETCH_ICryptoFactory(const char* /* name */) {
        return new CryptoFactory();
    }

}  // namespace widevine
}  // namespace V1_3
}  // namespace drm
}  // namespace hardware
}  // namespace android
