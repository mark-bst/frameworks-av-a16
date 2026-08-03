/*
 * Copyright (C) 2016 The Android Open Source Project
` *
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
#define LOG_TAG "hidl_WidevineDrmFactory"

#include "DrmPlugin.h"
#include "WVUUID.h"
#include "DrmFactory.h"
#include "TypeConvert.h"
#include "WVCreatePluginFactories.h"

#include <vector>
#include <log/log.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_3 {
namespace widevine {

using ::android::hardware::drm::V1_0::Status;
using ::android::hardware::drm::V1_1::SecurityLevel;
using ::android::hardware::drm::V1_2::widevine::DrmPlugin;

    DrmFactory::DrmFactory() {
        mWVDrmFactory = createDrmFactory();
    }

    // Methods from ::android::hardware::drm::V1_0::IDrmFactory follow.
    Return<bool> DrmFactory::isCryptoSchemeSupported (
            const hidl_array<uint8_t, 16>& uuid) {
        if (mWVDrmFactory->isCryptoSchemeSupported(uuid.data())) {
            return true;
        }
        return false;
    }

	Return<bool> DrmFactory::isCryptoSchemeSupported_1_2(const hidl_array<uint8_t, 16>& uuid,
	                                                     const hidl_string &mimeType,
	                                                     SecurityLevel level) {
	    return isCryptoSchemeSupported(uuid) && isContentTypeSupported(mimeType) &&
	            level == SecurityLevel::SW_SECURE_CRYPTO;
	}

    Return<bool> DrmFactory::isContentTypeSupported (
            const hidl_string& mimeType) {
        if (mWVDrmFactory->isContentTypeSupported(String8(mimeType.c_str()))) {
            return true;
        }
        return false;
    }

    Return<void> DrmFactory::createPlugin(const hidl_array<uint8_t, 16>& uuid,
            const hidl_string& /* appPackageName */, createPlugin_cb _hidl_cb) {
        if (mWVDrmFactory->isCryptoSchemeSupported(uuid.data())) {
            android::DrmPlugin *legacyPlugin = NULL;
            status_t status = mWVDrmFactory->createDrmPlugin(
                    uuid.data(), &legacyPlugin);

            DrmPlugin *newPlugin = NULL;
            if (legacyPlugin == NULL) {
                ALOGE("Drm legacy HAL: failed to create drm plugin");
            } else {
                newPlugin = new DrmPlugin(legacyPlugin);
            }
            _hidl_cb(V1_2::widevine::toStatus(status), newPlugin);
            return Void();
        }

        _hidl_cb(Status::ERROR_DRM_CANNOT_HANDLE, NULL);
        return Void();
    }

	Return<void> DrmFactory::getSupportedCryptoSchemes(
        getSupportedCryptoSchemes_cb _hidl_cb) {
	    std::vector<hidl_array<uint8_t, 16>> schemes;
	    for (const auto &scheme : wvdrm::getSupportedCryptoSchemes()) {
	        schemes.push_back(scheme);
	    }
	    _hidl_cb(schemes);
	    return Void();
	}

	Return<void> DrmFactory::debug(const hidl_handle& fd, const hidl_vec<hidl_string>& /*args*/) {
	    if (fd.getNativeHandle() == nullptr || fd->numFds < 1) {
	        ALOGE("%s: missing fd for writing", __FUNCTION__);
	        return Void();
	    }

	    FILE* out = fdopen(dup(fd->data[0]), "w");
	    uint32_t currentSessions = 1;
	    fprintf(out, "current open sessions: %u\n", currentSessions);
	    fclose(out);
	    return Void();
	}

    IDrmFactory* HIDL_FETCH_IDrmFactory(const char* /* name */) {
        return new DrmFactory();
    }

}  // namespace widevine
}  // namespace V1_3
}  // namespace drm
}  // namespace hardware
}  // namespace android
