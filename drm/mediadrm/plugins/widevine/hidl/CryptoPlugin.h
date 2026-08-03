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

#ifndef WIDEVINE_CRYPTO_PLUGIN_H_
#define WIDEVINE_CRYPTO_PLUGIN_H_

#include <android/hidl/memory/1.0/IMemory.h>
#include <android/hardware/drm/1.2/ICryptoPlugin.h>
#include <hidl/Status.h>
#include <media/hardware/CryptoAPI.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_2 {
namespace widevine {

namespace drm = ::android::hardware::drm;
using drm::V1_0::DestinationBuffer;
using drm::V1_0::Mode;
using drm::V1_0::Pattern;
using drm::V1_0::BufferType;
using drm::V1_0::SharedBuffer;
using drm::V1_0::Status;
using drm::V1_0::SubSample;
using drm::V1_2::ICryptoPlugin;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::sp;

typedef drm::V1_2::Status Status_V1_2;

struct CryptoPlugin : public ICryptoPlugin {
    CryptoPlugin(android::CryptoPlugin *plugin) : mLegacyPlugin(plugin) {}

    ~CryptoPlugin() {delete mLegacyPlugin;}

    // Methods from ::android::hardware::drm::V1_2::ICryptoPlugin
    // follow.

    Return<bool> requiresSecureDecoderComponent(const hidl_string& mime)
            override;

    Return<void> notifyResolution(uint32_t width, uint32_t height) override;

    Return<Status> setMediaDrmSession(const hidl_vec<uint8_t>& sessionId)
            override;

    Return<void> setSharedBufferBase(const ::android::hardware::hidl_memory& base,
        uint32_t bufferId) override;

    Return<void> decrypt(bool secure, const hidl_array<uint8_t, 16>& keyId,
            const hidl_array<uint8_t, 16>& iv, Mode mode, const Pattern& pattern,
            const hidl_vec<SubSample>& subSamples, const SharedBuffer& source,
            uint64_t offset, const DestinationBuffer& destination,
            decrypt_cb _hidl_cb) override;

	Return<void> decrypt_1_2(bool secure, const hidl_array<uint8_t, 16>& keyId,
            const hidl_array<uint8_t, 16>& iv, Mode mode, const Pattern& pattern,
            const hidl_vec<SubSample>& subSamples, const SharedBuffer& source,
            uint64_t offset, const DestinationBuffer& destination,
            decrypt_1_2_cb _hidl_cb) override; // use unique_lock

private:
    android::CryptoPlugin *mLegacyPlugin;
    std::mutex mSharedBufferLock;
    std::map<uint32_t, sp<IMemory>> mSharedBufferMap;

    CryptoPlugin() = delete;
    CryptoPlugin(const CryptoPlugin &) = delete;
    void operator=(const CryptoPlugin &) = delete;
};

}  // namespace widevine
}  // namespace V1_2
}  // namespace drm
}  // namespace hardware
}  // namespace android

#endif  // WIDEVINE_CRYPTO_PLUGIN_H_
