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

#ifndef WIDEVINE_DRM_PLUGIN_H_
#define WIDEVINE_DRM_PLUGIN_H_

#include <android/hardware/drm/1.2/IDrmPlugin.h>
#include <android/hardware/drm/1.2/IDrmPluginListener.h>

#include <map>
#include <stdio.h>
#include <utils/List.h>
#include <hidl/Status.h>
#include <media/drm/DrmAPI.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_2 {
namespace widevine {

namespace drm = ::android::hardware::drm;
using drm::V1_0::EventType;
using drm::V1_0::IDrmPluginListener;
using drm::V1_0::KeyRequestType;
using drm::V1_0::KeyStatus;
using drm::V1_0::KeyStatusType;
using drm::V1_0::KeyType;
using drm::V1_0::KeyValue;
using drm::V1_0::SecureStop;
using drm::V1_0::SecureStopId;
using drm::V1_0::SessionId;
using drm::V1_0::Status;
using drm::V1_1::DrmMetricGroup;
using drm::V1_1::HdcpLevel;
using drm::V1_1::SecureStopRelease;
using drm::V1_1::SecurityLevel;
using drm::V1_2::IDrmPlugin;
using drm::V1_2::KeySetId;
using drm::V1_2::OfflineLicenseState;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

typedef drm::V1_1::KeyRequestType KeyRequestType_V1_1;
typedef drm::V1_2::IDrmPluginListener IDrmPluginListener_V1_2;
typedef drm::V1_2::KeyStatus KeyStatus_V1_2;
typedef drm::V1_2::KeyStatusType KeyStatusType_V1_2;
typedef drm::V1_2::Status Status_V1_2;
typedef drm::V1_2::HdcpLevel HdcpLevel_V1_2;

struct DrmPlugin : public IDrmPlugin, android::DrmPluginListener {

    DrmPlugin(android::DrmPlugin *plugin);
    ~DrmPlugin() {delete mLegacyPlugin;}

    // Methods from ::android::hardware::drm::V1_1::IDrmPlugin follow.
    Return<void> openSession(openSession_cb _hidl_cb) override;
    Return<void> openSession_1_1(SecurityLevel securityLevel,
            openSession_1_1_cb _hidl_cb) override;

    Return<Status> closeSession(const hidl_vec<uint8_t>& sessionId) override;

    Return<void> getKeyRequest(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
            KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
            getKeyRequest_cb _hidl_cb) override;

    Return<void> getKeyRequest_1_1(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
            KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
            getKeyRequest_1_1_cb _hidl_cb) override;

	Return<void> getKeyRequest_1_2(const hidl_vec<uint8_t>& scope,
	        const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
	        KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
	        getKeyRequest_1_2_cb _hidl_cb) override;

    Return<void> provideKeyResponse(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& response, provideKeyResponse_cb _hidl_cb) override;

    Return<Status> removeKeys(const hidl_vec<uint8_t>& sessionId) override;

    Return<Status> restoreKeys(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keySetId) override;

    Return<void> queryKeyStatus(const hidl_vec<uint8_t>& sessionId,
            queryKeyStatus_cb _hidl_cb) override;

    Return<void> getProvisionRequest(const hidl_string& certificateType,
            const hidl_string& certificateAuthority,
            getProvisionRequest_cb _hidl_cb) override;

	Return<void> getProvisionRequest_1_2(const hidl_string& certificateType,
        const hidl_string& certificateAuthority,
        getProvisionRequest_1_2_cb _hidl_cb) override;

    Return<void> provideProvisionResponse(const hidl_vec<uint8_t>& response,
            provideProvisionResponse_cb _hidl_cb) override;

    Return<void> getHdcpLevels(getHdcpLevels_cb _hidl_cb) {
        HdcpLevel connectedLevel = HdcpLevel::HDCP_NONE;
        HdcpLevel maxLevel = HdcpLevel::HDCP_NO_OUTPUT;
        _hidl_cb(Status::OK, connectedLevel, maxLevel);
        return Void();
    }

    Return<void> getHdcpLevels_1_2(getHdcpLevels_1_2_cb _hidl_cb) {
        HdcpLevel_V1_2 connectedLevel = HdcpLevel_V1_2::HDCP_NONE;
        HdcpLevel_V1_2 maxLevel = HdcpLevel_V1_2::HDCP_NO_OUTPUT;
        _hidl_cb(Status_V1_2::OK, connectedLevel, maxLevel);
        return Void();
    }

    Return<void> getNumberOfSessions(getNumberOfSessions_cb _hidl_cb) override;

    Return<void> getSecurityLevel(const hidl_vec<uint8_t>& sessionId,
            getSecurityLevel_cb _hidl_cb) override;

    Return<void> getMetrics(getMetrics_cb _hidl_cb) override;

    Return<void> getOfflineLicenseKeySetIds(getOfflineLicenseKeySetIds_cb _hidl_cb) override;

    Return<Status> removeOfflineLicense(const KeySetId &keySetId) override;

    Return<void> getOfflineLicenseState(const KeySetId &keySetId,
            getOfflineLicenseState_cb _hidl_cb) override;

    Return<void> getSecureStops(getSecureStops_cb _hidl_cb) override;

    Return<void> getSecureStop(const hidl_vec<uint8_t>& secureStopId,
            getSecureStop_cb _hidl_cb) override;

	Return<Status> releaseSecureStop(const hidl_vec<uint8_t>& secureStopId)
            override;

    Return<Status> releaseAllSecureStops() override;

    Return<Status> releaseSecureStops(const SecureStopRelease& ssRelease);

    Return<void> getSecureStopIds(getSecureStopIds_cb _hidl_cb);

    Return<Status> removeSecureStop(const hidl_vec<uint8_t>& secureStopId);

    Return<Status> removeAllSecureStops();

    Return<void> getPropertyString(const hidl_string& propertyName,
            getPropertyString_cb _hidl_cb) override;

    Return<void> getPropertyByteArray(const hidl_string& propertyName,
            getPropertyByteArray_cb _hidl_cb) override;

    Return<Status> setPropertyString(const hidl_string& propertyName,
            const hidl_string& value) override;

    Return<Status> setPropertyByteArray(const hidl_string& propertyName,
            const hidl_vec<uint8_t>& value) override;

    Return<Status> setCipherAlgorithm(const hidl_vec<uint8_t>& sessionId,
            const hidl_string& algorithm) override;

    Return<Status> setMacAlgorithm(const hidl_vec<uint8_t>& sessionId,
            const hidl_string& algorithm) override;

    Return<void> encrypt(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& input,
            const hidl_vec<uint8_t>& iv, encrypt_cb _hidl_cb) override;

    Return<void> decrypt(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& input,
            const hidl_vec<uint8_t>& iv, decrypt_cb _hidl_cb) override;

    Return<void> sign(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& message,
            sign_cb _hidl_cb) override;

    Return<void> verify(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& message,
            const hidl_vec<uint8_t>& signature, verify_cb _hidl_cb) override;

    Return<void> signRSA(const hidl_vec<uint8_t>& sessionId,
            const hidl_string& algorithm, const hidl_vec<uint8_t>& message,
            const hidl_vec<uint8_t>& wrappedkey, signRSA_cb _hidl_cb) override;

    Return<void> setListener(const sp<IDrmPluginListener>& listener) override;

    Return<void> sendEvent(EventType eventType,
            const hidl_vec<uint8_t>& sessionId, const hidl_vec<uint8_t>& data)
            override;

    Return<void> sendExpirationUpdate(const hidl_vec<uint8_t>& sessionId,
            int64_t expiryTimeInMS) override;

    Return<void> sendKeysChange(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<KeyStatus>& keyStatusList, bool hasNewUsableKey) override;

	Return<void> sendKeysChange_1_2(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<KeyStatus_V1_2>& keyStatusList, bool hasNewUsableKey) override;

	Return<void> sendSessionLostState(const hidl_vec<uint8_t>& sessionId)  override;

    // Methods from android::DrmPluginListener follow
    virtual void sendEvent(android::DrmPlugin::EventType eventType, int extra,
            Vector<uint8_t> const *sessionId, Vector<uint8_t> const *data);

    virtual void sendExpirationUpdate(Vector<uint8_t> const *sessionId,
            int64_t expiryTimeInMS);

    virtual void sendKeysChange(Vector<uint8_t> const *sessionId,
            Vector<android::DrmPlugin::KeyStatus> const *keyStatusList,
            bool hasNewUsableKey);

private:
    Return<Status> setSecurityLevel(const hidl_vec<uint8_t>& sessionId,
            SecurityLevel level);

    status_t getKeyRequestCommon(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData,
            const hidl_string& mimeType,
            KeyType keyType,
            const hidl_vec<KeyValue>& optionalParameters,
            Vector<uint8_t> &request,
            KeyRequestType_V1_1 &requestType,
            String8 &defaultUrl);

    std::map<std::vector<uint8_t>, SecurityLevel> mSecurityLevel;
    android::DrmPlugin *mLegacyPlugin;
    sp<IDrmPluginListener> mListener;
    sp<IDrmPluginListener_V1_2> mListenerV1_2;
    int64_t mOpenSessionOkCount;
    int64_t mCloseSessionOkCount;
    int64_t mCloseSessionNotOpenedCount;

    DrmPlugin() = delete;
    DrmPlugin(const DrmPlugin &) = delete;
    void operator=(const DrmPlugin &) = delete;
};

}  // namespace widevine
}  // namespace V1_2
}  // namespace drm
}  // namespace hardware
}  // namespace android

#endif  // WIDEVINE_DRM_PLUGIN_H_
