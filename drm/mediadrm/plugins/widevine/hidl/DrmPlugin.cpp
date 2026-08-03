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
#define LOG_TAG "hidl_WidevineDrmPlugin"

#include <stdio.h>
#include <inttypes.h>
#include <utils/Log.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

#include "DrmPlugin.h"
#include "TypeConvert.h"

#define UNUSED(x) (void)(x);

namespace {
const int kSecureStopIdSize = 10;

std::vector<uint8_t> uint32ToVector(uint32_t value) {
    // 10 bytes to display max value 4294967295 + one byte null terminator
    char buffer[kSecureStopIdSize];
    memset(buffer, 0, kSecureStopIdSize);
    snprintf(buffer, kSecureStopIdSize, "%" PRIu32, value);
    return std::vector<uint8_t>(buffer, buffer + sizeof(buffer));
}

}; // unnamed namespace


namespace android {
namespace hardware {
namespace drm {
namespace V1_2 {
namespace widevine {

    DrmPlugin::DrmPlugin(android::DrmPlugin *plugin)
            : mLegacyPlugin(plugin),
              mOpenSessionOkCount(0),
              mCloseSessionOkCount(0),
              mCloseSessionNotOpenedCount(0) {
    }

    // Methods from ::android::hardware::drm::V1_0::IDrmPlugin follow.
    Return<void> DrmPlugin::openSession(openSession_cb _hidl_cb) {
        Vector<uint8_t> legacySessionId;
        status_t state = mLegacyPlugin->openSession(legacySessionId);
        Status status = toStatus(state);
        if (status == Status::OK) {
            mOpenSessionOkCount++;
            status = setSecurityLevel(toHidlVec(legacySessionId), SecurityLevel::SW_SECURE_CRYPTO);
        }
        _hidl_cb(status, toHidlVec(legacySessionId));
        return Void();
    }

    Return<void> DrmPlugin::openSession_1_1(SecurityLevel securityLevel,
            openSession_1_1_cb _hidl_cb) {
        Vector<uint8_t> legacySessionId;
        status_t state = mLegacyPlugin->openSession(legacySessionId);
        Status status = toStatus(state);
        if (status == Status::OK) {
            mOpenSessionOkCount++;
            status = setSecurityLevel(toHidlVec(legacySessionId), securityLevel);
        }
        _hidl_cb(status, toHidlVec(legacySessionId));
        return Void();
    }

    Return<Status> DrmPlugin::closeSession(const hidl_vec<uint8_t>& sessionId) {
        Status status = toStatus(mLegacyPlugin->closeSession(toVector(sessionId)));
        if (status == Status::OK) {
            mCloseSessionOkCount++;
        } else {
            mCloseSessionNotOpenedCount++;
        }
        return status;
    }

    status_t DrmPlugin::getKeyRequestCommon(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData,
            const hidl_string& mimeType,
            KeyType keyType,
            const hidl_vec<KeyValue>& optionalParameters,
            Vector<uint8_t> &request,
            KeyRequestType_V1_1 &requestType,
            String8 &defaultUrl) {

        status_t status = android::OK;

        android::DrmPlugin::KeyType legacyKeyType;
        switch(keyType) {
        case KeyType::OFFLINE:
            legacyKeyType = android::DrmPlugin::kKeyType_Offline;
            break;
        case KeyType::STREAMING:
            legacyKeyType = android::DrmPlugin::kKeyType_Streaming;
            break;
        case KeyType::RELEASE:
            legacyKeyType = android::DrmPlugin::kKeyType_Release;
            break;
        default:
            status = android::BAD_VALUE;
            break;
        }

        if (status == android::OK) {
            android::KeyedVector<String8, String8> legacyOptionalParameters;
            for (size_t i = 0; i < optionalParameters.size(); i++) {
                legacyOptionalParameters.add(String8(optionalParameters[i].key.c_str()),
                        String8(optionalParameters[i].value.c_str()));
            }

            android::DrmPlugin::KeyRequestType legacyRequestType =
                    android::DrmPlugin::kKeyRequestType_Unknown;

            status = mLegacyPlugin->getKeyRequest(toVector(scope),
                    toVector(initData), String8(mimeType.c_str()), legacyKeyType,
                    legacyOptionalParameters, request, defaultUrl,
                    &legacyRequestType);

            switch(legacyRequestType) {
            case android::DrmPlugin::kKeyRequestType_Initial:
                requestType = KeyRequestType_V1_1::INITIAL;
                break;
            case android::DrmPlugin::kKeyRequestType_Renewal:
                requestType = KeyRequestType_V1_1::RENEWAL;
                break;
            case android::DrmPlugin::kKeyRequestType_Release:
                requestType = KeyRequestType_V1_1::RELEASE;
                break;
            case android::DrmPlugin::kKeyRequestType_Unknown:
            default:
                requestType = KeyRequestType_V1_1::UNKNOWN;
                break;
            }
        }
        return status;
   }

    Return<void> DrmPlugin::getKeyRequest(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
            KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
            getKeyRequest_cb _hidl_cb) {

        KeyRequestType_V1_1 requestType = KeyRequestType_V1_1::UNKNOWN;
        Vector<uint8_t> legacyRequest;
        String8 defaultUrl;

        status_t status = getKeyRequestCommon(
                scope, initData, mimeType, keyType, optionalParameters,
                legacyRequest, requestType, defaultUrl);

        _hidl_cb(toStatus(status), toHidlVec(legacyRequest), static_cast<drm::V1_0::KeyRequestType>(requestType),
                 defaultUrl.c_str());
        return Void();
    }

    Return<void> DrmPlugin::getKeyRequest_1_1(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
            KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
            getKeyRequest_1_1_cb _hidl_cb) {

        KeyRequestType_V1_1 requestType = KeyRequestType_V1_1::UNKNOWN;
        Vector<uint8_t> legacyRequest;
        String8 defaultUrl;

        status_t status = getKeyRequestCommon(
                scope, initData, mimeType, keyType, optionalParameters,
                legacyRequest, requestType, defaultUrl);

        _hidl_cb(toStatus(status), toHidlVec(legacyRequest), requestType,
                 defaultUrl.c_str());
        return Void();
    }

    Return<void> DrmPlugin::getKeyRequest_1_2(const hidl_vec<uint8_t>& scope,
	        const hidl_vec<uint8_t>& initData, const hidl_string& mimeType,
	        KeyType keyType, const hidl_vec<KeyValue>& optionalParameters,
	        getKeyRequest_1_2_cb _hidl_cb) {

        KeyRequestType_V1_1 requestType = KeyRequestType_V1_1::UNKNOWN;
        Vector<uint8_t> legacyRequest;
        String8 defaultUrl;

        status_t status = getKeyRequestCommon(
                scope, initData, mimeType, keyType, optionalParameters,
                legacyRequest, requestType, defaultUrl);

        _hidl_cb(toStatus_1_2(toStatus(status)), toHidlVec(legacyRequest), requestType,
                 defaultUrl.c_str());
        return Void();
    }

    Return<void> DrmPlugin::provideKeyResponse(const hidl_vec<uint8_t>& scope,
            const hidl_vec<uint8_t>& response, provideKeyResponse_cb _hidl_cb) {

        Vector<uint8_t> keySetId;
        status_t status = mLegacyPlugin->provideKeyResponse(toVector(scope),
                toVector(response), keySetId);
        _hidl_cb(toStatus(status), toHidlVec(keySetId));
        return Void();
    }

    Return<Status> DrmPlugin::removeKeys(const hidl_vec<uint8_t>& sessionId) {
        return toStatus(mLegacyPlugin->removeKeys(toVector(sessionId)));
    }

    Return<Status> DrmPlugin::restoreKeys(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keySetId) {
        status_t legacyStatus = mLegacyPlugin->restoreKeys(toVector(sessionId),
                toVector(keySetId));
        return toStatus(legacyStatus);
    }

    Return<void> DrmPlugin::queryKeyStatus(const hidl_vec<uint8_t>& sessionId,
            queryKeyStatus_cb _hidl_cb) {

        android::KeyedVector<String8, String8> legacyInfoMap;
        status_t status = mLegacyPlugin->queryKeyStatus(toVector(sessionId),
                legacyInfoMap);

        Vector<KeyValue> infoMapVec;
        for (size_t i = 0; i < legacyInfoMap.size(); i++) {
            KeyValue keyValuePair;
            keyValuePair.key = String8(legacyInfoMap.keyAt(i));
            keyValuePair.value = String8(legacyInfoMap.valueAt(i));
            infoMapVec.push_back(keyValuePair);
        }
        _hidl_cb(toStatus(status), toHidlVec(infoMapVec));
        return Void();
    }

    Return<void> DrmPlugin::getProvisionRequest(
            const hidl_string& certificateType,
            const hidl_string& certificateAuthority,
            getProvisionRequest_cb _hidl_cb) {

        Vector<uint8_t> legacyRequest;
        String8 legacyDefaultUrl;
        status_t status = mLegacyPlugin->getProvisionRequest(
                String8(certificateType.c_str()), String8(certificateAuthority.c_str()),
                legacyRequest, legacyDefaultUrl);

        _hidl_cb(toStatus(status), toHidlVec(legacyRequest),
                hidl_string(legacyDefaultUrl));
        return Void();
    }

	Return<void> DrmPlugin::getProvisionRequest_1_2(
	        const hidl_string& certificateType,
	        const hidl_string& certificateAuthority,
	        getProvisionRequest_1_2_cb _hidl_cb) {

		Vector<uint8_t> legacyRequest;
        String8 legacyDefaultUrl;
        status_t status = mLegacyPlugin->getProvisionRequest(
                String8(certificateType.c_str()), String8(certificateAuthority.c_str()),
                legacyRequest, legacyDefaultUrl);

        _hidl_cb(toStatus_1_2(toStatus(status)), toHidlVec(legacyRequest),
                hidl_string(legacyDefaultUrl));
        return Void();
	}

    Return<void> DrmPlugin::provideProvisionResponse(
            const hidl_vec<uint8_t>& response,
            provideProvisionResponse_cb _hidl_cb) {

        Vector<uint8_t> certificate;
        Vector<uint8_t> wrappedKey;

        status_t legacyStatus = mLegacyPlugin->provideProvisionResponse(
                toVector(response), certificate, wrappedKey);

        _hidl_cb(toStatus(legacyStatus), toHidlVec(certificate),
                toHidlVec(wrappedKey));
        return Void();
    }

    Return<void> DrmPlugin::getNumberOfSessions(getNumberOfSessions_cb _hidl_cb) {
            uint32_t currentSessions = mOpenSessionOkCount - mCloseSessionOkCount;
            uint32_t maxSessions = 10;
            _hidl_cb(Status::OK, currentSessions, maxSessions);
            return Void();
    }

    Return<void> DrmPlugin::getSecurityLevel(const hidl_vec<uint8_t>& sessionId,
                getSecurityLevel_cb _hidl_cb) {
        if (sessionId.size() == 0) {
            _hidl_cb(Status::BAD_VALUE, SecurityLevel::UNKNOWN);
            return Void();
        }

        std::vector<uint8_t> sid = stdtoVector(sessionId);
        std::map<std::vector<uint8_t>, SecurityLevel>::iterator itr =
                mSecurityLevel.find(sid);
        if (itr == mSecurityLevel.end()) {
            ALOGE("Session id not found");
            _hidl_cb(Status::ERROR_DRM_INVALID_STATE, SecurityLevel::UNKNOWN);
            return Void();
        }

        _hidl_cb(Status::OK, itr->second);
        return Void();
    }

    Return<Status> DrmPlugin::setSecurityLevel(const hidl_vec<uint8_t>& sessionId,
                SecurityLevel level) {
        if (sessionId.size() == 0) {
            ALOGE("Invalid empty session id");
            return Status::BAD_VALUE;
        }

        if (level > SecurityLevel::SW_SECURE_CRYPTO) {
            ALOGE("Cannot set security level > max");
            return Status::ERROR_DRM_CANNOT_HANDLE;
        }

        std::vector<uint8_t> sid = stdtoVector(sessionId);
        std::map<std::vector<uint8_t>, SecurityLevel>::iterator itr =
                mSecurityLevel.find(sid);
        if (itr != mSecurityLevel.end()) {
            mSecurityLevel[sid] = level;
        } else {
            if (!mSecurityLevel.insert(
                    std::pair<std::vector<uint8_t>, SecurityLevel>(sid, level)).second) {
                ALOGE("Failed to set security level");
                return Status::ERROR_DRM_INVALID_STATE;
            }
        }
        return Status::OK;
    }

    Return<void> DrmPlugin::getMetrics(getMetrics_cb _hidl_cb) {
        // Set the open session count metric.
        DrmMetricGroup::Attribute openSessionOkAttribute = {
          "status", DrmMetricGroup::ValueType::INT64_TYPE, (int64_t) Status::OK, 0.0, ""
        };
        DrmMetricGroup::Value openSessionMetricValue = {
          "count", DrmMetricGroup::ValueType::INT64_TYPE, mOpenSessionOkCount, 0.0, ""
        };
        DrmMetricGroup::Metric openSessionMetric = {
          "open_session", { openSessionOkAttribute }, { openSessionMetricValue }
        };

        // Set the close session count metric.
        DrmMetricGroup::Attribute closeSessionOkAttribute = {
          "status", DrmMetricGroup::ValueType::INT64_TYPE, (int64_t) Status::OK, 0.0, ""
        };
        DrmMetricGroup::Value closeSessionMetricValue = {
          "count", DrmMetricGroup::ValueType::INT64_TYPE, mCloseSessionOkCount, 0.0, ""
        };
        DrmMetricGroup::Metric closeSessionMetric = {
          "close_session", { closeSessionOkAttribute }, { closeSessionMetricValue }
        };

        // Set the close session, not opened metric.
        DrmMetricGroup::Attribute closeSessionNotOpenedAttribute = {
          "status", DrmMetricGroup::ValueType::INT64_TYPE,
          (int64_t) Status::ERROR_DRM_SESSION_NOT_OPENED, 0.0, ""
        };
        DrmMetricGroup::Value closeSessionNotOpenedMetricValue = {
          "count", DrmMetricGroup::ValueType::INT64_TYPE, mCloseSessionNotOpenedCount, 0.0, ""
        };
        DrmMetricGroup::Metric closeSessionNotOpenedMetric = {
          "close_session", { closeSessionNotOpenedAttribute }, { closeSessionNotOpenedMetricValue }
        };

        DrmMetricGroup metrics = { { openSessionMetric, closeSessionMetric,
                                    closeSessionNotOpenedMetric } };

        _hidl_cb(Status::OK, hidl_vec<DrmMetricGroup>({metrics}));
        return Void();
    }

    Return<void> DrmPlugin::getOfflineLicenseKeySetIds(getOfflineLicenseKeySetIds_cb _hidl_cb) {
        std::vector<KeySetId> keySetIds;
        _hidl_cb(Status::ERROR_DRM_UNKNOWN, keySetIds);
        return Void();
    }

    Return<Status> DrmPlugin::removeOfflineLicense(const KeySetId& keySetId) {
        UNUSED(keySetId);
        return Status::BAD_VALUE;
    }

    Return<void> DrmPlugin::getOfflineLicenseState(const KeySetId& keySetId,
            getOfflineLicenseState_cb _hidl_cb) {
        UNUSED(keySetId);
        _hidl_cb(Status::BAD_VALUE, OfflineLicenseState::UNKNOWN);
        return Void();
    }

    Return<void> DrmPlugin::getSecureStops(getSecureStops_cb _hidl_cb) {
        List<Vector<uint8_t> > legacySecureStops;
        status_t status = mLegacyPlugin->getSecureStops(legacySecureStops);

        Vector<SecureStop> secureStopsVec;
        List<Vector<uint8_t> >::iterator iter = legacySecureStops.begin();

        while (iter != legacySecureStops.end()) {
            SecureStop secureStop;
            secureStop.opaqueData = toHidlVec(*iter++);
            secureStopsVec.push_back(secureStop);
        }

        _hidl_cb(toStatus(status), toHidlVec(secureStopsVec));
        return Void();
    }

    Return<void> DrmPlugin::getSecureStop(const hidl_vec<uint8_t>& secureStopId,
            getSecureStop_cb _hidl_cb) {

        Vector<uint8_t> legacySecureStop;
        status_t status = mLegacyPlugin->getSecureStop(toVector(secureStopId),
                legacySecureStop);

        SecureStop secureStop;
        secureStop.opaqueData = toHidlVec(legacySecureStop);
        _hidl_cb(toStatus(status), secureStop);
        return Void();
    }

    Return<Status> DrmPlugin::releaseSecureStop(
            const hidl_vec<uint8_t>& secureStopId) {
        status_t legacyStatus =
            mLegacyPlugin->releaseSecureStops(toVector(secureStopId));
        return toStatus(legacyStatus);
    }

    Return<Status> DrmPlugin::releaseAllSecureStops() {
        return toStatus(mLegacyPlugin->releaseAllSecureStops());
    }

    Return<Status> DrmPlugin::releaseSecureStops(const SecureStopRelease& ssRelease) {
        if (ssRelease.opaqueData.size() == 0) {
            return Status::BAD_VALUE;
        }

        Status status = Status::OK;
        std::vector<uint8_t> input = stdtoVector(ssRelease.opaqueData);

        // The format of opaqueData is shared between the server
        // and the drm service. The clearkey implementation consists of:
        //    count - number of secure stops
        //    list of fixed length secure stops
        size_t countBufferSize = sizeof(uint32_t);
        if (input.size() < countBufferSize) {
            // SafetyNet logging
            android_errorWriteLog(0x534e4554, "144766455");
            return Status::BAD_VALUE;
        }
        uint32_t count = 0;
        sscanf(reinterpret_cast<char*>(input.data()), "%04" PRIu32, &count);

        // Avoid divide by 0 below.
        if (count == 0) {
            return Status::BAD_VALUE;
        }

        size_t secureStopSize = (input.size() - countBufferSize) / count;
        uint8_t buffer[secureStopSize];
        size_t offset = countBufferSize; // skip the count
        for (size_t i = 0; i < count; ++i, offset += secureStopSize) {
            memcpy(buffer, input.data() + offset, secureStopSize);
            std::vector<uint8_t> id(buffer, buffer + secureStopSize);

            status = toStatus(mLegacyPlugin->releaseSecureStops(toVector(stdtoHidlVec(id))));
            if (Status::OK != status) break;
        }

        return status;
    }

    Return<void> DrmPlugin::getSecureStopIds(getSecureStopIds_cb _hidl_cb) {
        List<Vector<uint8_t> > legacySecureStops;
        status_t status = mLegacyPlugin->getSecureStops(legacySecureStops);

        uint32_t id = 0;
        std::vector<SecureStopId> ids;
        List<Vector<uint8_t> >::iterator iter = legacySecureStops.begin();

        while (iter != legacySecureStops.end()) {
            ++id;
            ids.push_back(uint32ToVector(id));
        }
        _hidl_cb(toStatus(status), stdtoHidlVec(ids));
        return Void();
    }

    Return<Status> DrmPlugin::removeSecureStop(const hidl_vec<uint8_t>& secureStopId) {
        return releaseSecureStop(secureStopId);
    }

    Return<Status> DrmPlugin::removeAllSecureStops() {
        return releaseAllSecureStops();
    }

    Return<void> DrmPlugin::getPropertyString(const hidl_string& propertyName,
            getPropertyString_cb _hidl_cb) {
        String8 legacyValue;
        status_t status = mLegacyPlugin->getPropertyString(
                String8(propertyName.c_str()), legacyValue);
        _hidl_cb(toStatus(status), legacyValue.c_str());
        return Void();
    }

    Return<void> DrmPlugin::getPropertyByteArray(const hidl_string& propertyName,
            getPropertyByteArray_cb _hidl_cb) {
        Vector<uint8_t> legacyValue;
        status_t status = mLegacyPlugin->getPropertyByteArray(
                String8(propertyName.c_str()), legacyValue);
        _hidl_cb(toStatus(status), toHidlVec(legacyValue));
        return Void();
    }

    Return<Status> DrmPlugin::setPropertyString(const hidl_string& propertyName,
            const hidl_string& value) {
        status_t legacyStatus =
            mLegacyPlugin->setPropertyString(String8(propertyName.c_str()),
                    String8(value.c_str()));
        return toStatus(legacyStatus);
    }

    Return<Status> DrmPlugin::setPropertyByteArray(
            const hidl_string& propertyName, const hidl_vec<uint8_t>& value) {
        status_t legacyStatus =
            mLegacyPlugin->setPropertyByteArray(String8(propertyName.c_str()),
                    toVector(value));
        return toStatus(legacyStatus);
    }

    Return<Status> DrmPlugin::setCipherAlgorithm(
            const hidl_vec<uint8_t>& sessionId, const hidl_string& algorithm) {
        status_t legacyStatus =
            mLegacyPlugin->setCipherAlgorithm(toVector(sessionId),
                String8(algorithm.c_str()));
        return toStatus(legacyStatus);
    }

    Return<Status> DrmPlugin::setMacAlgorithm(
            const hidl_vec<uint8_t>& sessionId, const hidl_string& algorithm) {
        status_t legacyStatus =
            mLegacyPlugin->setMacAlgorithm(toVector(sessionId),
                String8(algorithm.c_str()));
        return toStatus(legacyStatus);
    }

    Return<void> DrmPlugin::encrypt(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& input,
            const hidl_vec<uint8_t>& iv, encrypt_cb _hidl_cb) {

        Vector<uint8_t> legacyOutput;
        status_t status = mLegacyPlugin->encrypt(toVector(sessionId),
                toVector(keyId), toVector(input), toVector(iv), legacyOutput);
        _hidl_cb(toStatus(status), toHidlVec(legacyOutput));
        return Void();
    }

    Return<void> DrmPlugin::decrypt(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& input,
            const hidl_vec<uint8_t>& iv, decrypt_cb _hidl_cb) {

        Vector<uint8_t> legacyOutput;
        status_t status = mLegacyPlugin->decrypt(toVector(sessionId),
                toVector(keyId), toVector(input), toVector(iv), legacyOutput);
        _hidl_cb(toStatus(status), toHidlVec(legacyOutput));
        return Void();
    }

    Return<void> DrmPlugin::sign(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& message,
            sign_cb _hidl_cb) {
        Vector<uint8_t> legacySignature;
        status_t status = mLegacyPlugin->sign(toVector(sessionId),
                toVector(keyId), toVector(message), legacySignature);
        _hidl_cb(toStatus(status), toHidlVec(legacySignature));
        return Void();
    }

    Return<void> DrmPlugin::verify(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<uint8_t>& keyId, const hidl_vec<uint8_t>& message,
            const hidl_vec<uint8_t>& signature, verify_cb _hidl_cb) {

        bool match;
        status_t status = mLegacyPlugin->verify(toVector(sessionId),
                toVector(keyId), toVector(message), toVector(signature),
                match);
        _hidl_cb(toStatus(status), match);
        return Void();
    }

    Return<void> DrmPlugin::signRSA(const hidl_vec<uint8_t>& sessionId,
            const hidl_string& algorithm, const hidl_vec<uint8_t>& message,
            const hidl_vec<uint8_t>& wrappedKey, signRSA_cb _hidl_cb) {

        Vector<uint8_t> legacySignature;
        status_t status = mLegacyPlugin->signRSA(toVector(sessionId),
                String8(algorithm.c_str()), toVector(message), toVector(wrappedKey),
                legacySignature);
        _hidl_cb(toStatus(status), toHidlVec(legacySignature));
        return Void();
    }

    Return<void> DrmPlugin::setListener(const sp<IDrmPluginListener>& listener) {
        mListener = listener;
        mListenerV1_2 = IDrmPluginListener_V1_2::castFrom(listener);
        mLegacyPlugin->setListener(listener == NULL ? NULL : this);
        return Void();
    }

    Return<void> DrmPlugin::sendEvent(EventType eventType,
            const hidl_vec<uint8_t>& sessionId, const hidl_vec<uint8_t>& data) {
        if (mListenerV1_2 != NULL) {
            mListenerV1_2->sendEvent(eventType, sessionId, data);
        } else if (mListener != NULL) {
            mListener->sendEvent(eventType, sessionId, data);
        }
        return Void();
    }

    Return<void> DrmPlugin::sendExpirationUpdate(
            const hidl_vec<uint8_t>& sessionId, int64_t expiryTimeInMS) {
        if (mListenerV1_2 != NULL) {
            mListenerV1_2->sendExpirationUpdate(sessionId, expiryTimeInMS);
        } else if (mListener != NULL) {
            mListener->sendExpirationUpdate(sessionId, expiryTimeInMS);
        }
        return Void();
    }

    Return<void> DrmPlugin::sendKeysChange(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<KeyStatus>& keyStatusList, bool hasNewUsableKey) {
        if (mListenerV1_2 != NULL) {
            mListenerV1_2->sendKeysChange(sessionId, keyStatusList, hasNewUsableKey);
        } else if (mListener != NULL) {
            mListener->sendKeysChange(sessionId, keyStatusList, hasNewUsableKey);
        }
        return Void();
    }

    Return<void> DrmPlugin::sendKeysChange_1_2(const hidl_vec<uint8_t>& sessionId,
            const hidl_vec<KeyStatus_V1_2>& keyStatusList, bool hasNewUsableKey) {
        if (mListenerV1_2 != nullptr) {
            mListenerV1_2->sendKeysChange_1_2(sessionId, keyStatusList, hasNewUsableKey);
        }
        return Void();
    }

    Return<void> DrmPlugin::sendSessionLostState(
            const hidl_vec<uint8_t>& sessionId) {
        if (mListenerV1_2 != NULL) {
            mListenerV1_2->sendSessionLostState(sessionId);
        }
        return Void();
    }


    // Methods from android::DrmPluginListener
    void DrmPlugin::sendEvent(android::DrmPlugin::EventType legacyEventType,
            int /*unused*/, Vector<uint8_t> const *sessionId,
            Vector<uint8_t> const *data) {

        EventType eventType;
        bool sendEvent = true;
        switch(legacyEventType) {
        case android::DrmPlugin::kDrmPluginEventProvisionRequired:
            eventType = EventType::PROVISION_REQUIRED;
            break;
        case android::DrmPlugin::kDrmPluginEventKeyNeeded:
            eventType = EventType::KEY_NEEDED;
            break;
        case android::DrmPlugin::kDrmPluginEventKeyExpired:
            eventType = EventType::KEY_EXPIRED;
            break;
        case android::DrmPlugin::kDrmPluginEventVendorDefined:
            eventType = EventType::VENDOR_DEFINED;
            break;
        case android::DrmPlugin::kDrmPluginEventSessionReclaimed:
            eventType = EventType::SESSION_RECLAIMED;
            break;
        default:
            sendEvent = false;
            break;
        }
        if (sendEvent) {
            Vector<uint8_t> emptyVector;
            mListener->sendEvent(eventType,
                    toHidlVec(sessionId == NULL ? emptyVector: *sessionId),
                    toHidlVec(data == NULL ? emptyVector: *data));
        }
    }

    void DrmPlugin::sendExpirationUpdate(Vector<uint8_t> const *sessionId,
            int64_t expiryTimeInMS) {
        mListener->sendExpirationUpdate(toHidlVec(*sessionId), expiryTimeInMS);
    }

    void DrmPlugin::sendKeysChange(Vector<uint8_t> const *sessionId,
            Vector<android::DrmPlugin::KeyStatus> const *legacyKeyStatusList,
            bool hasNewUsableKey) {

        Vector<KeyStatus> keyStatusVec;
        for (size_t i = 0; i < legacyKeyStatusList->size(); i++) {
            const android::DrmPlugin::KeyStatus &legacyKeyStatus =
                legacyKeyStatusList->itemAt(i);

            KeyStatus keyStatus;

            switch(legacyKeyStatus.mType) {
            case android::DrmPlugin::kKeyStatusType_Usable:
                keyStatus.type = KeyStatusType::USABLE;
                break;
            case android::DrmPlugin::kKeyStatusType_Expired:
                keyStatus.type = KeyStatusType::EXPIRED;
                break;
            case android::DrmPlugin::kKeyStatusType_OutputNotAllowed:
                keyStatus.type = KeyStatusType::OUTPUTNOTALLOWED;
                break;
            case android::DrmPlugin::kKeyStatusType_StatusPending:
                keyStatus.type = KeyStatusType::STATUSPENDING;
                break;
            case android::DrmPlugin::kKeyStatusType_InternalError:
            default:
                keyStatus.type = KeyStatusType::INTERNALERROR;
                break;
            }

            keyStatus.keyId = toHidlVec(legacyKeyStatus.mKeyId);
            keyStatusVec.push_back(keyStatus);
        }
        mListener->sendKeysChange(toHidlVec(*sessionId),
                toHidlVec(keyStatusVec), hasNewUsableKey);
    }

}  // namespace widevine
}  // namespace V1_2
}  // namespace drm
}  // namespace hardware
}  // namespace android
