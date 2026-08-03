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

#ifndef ANDROID_HARDWARE_DRM_V1_2_TYPECONVERT
#define ANDROID_HARDWARE_DRM_V1_2_TYPECONVERT

#include <android/hardware/drm/1.2/types.h>
#include <media/stagefright/MediaErrors.h>
#include <utils/Vector.h>

namespace android {
namespace hardware {
namespace drm {
namespace V1_2 {
namespace widevine {

using ::android::hardware::drm::V1_0::Status;
using ::android::hardware::hidl_vec;

typedef ::android::hardware::drm::V1_2::Status Status_V1_2;

template<typename T> const hidl_vec<T> toHidlVec(const Vector<T> &Vector) {
    hidl_vec<T> vec;
    vec.setToExternal(const_cast<T *>(Vector.array()), Vector.size());
    return vec;
}

template<typename T> hidl_vec<T> toHidlVec(Vector<T> &Vector) {
    hidl_vec<T> vec;
    vec.setToExternal(Vector.editArray(), Vector.size());
    return vec;
}

template<typename T> const Vector<T> toVector(const hidl_vec<T> &vec) {
    Vector<T> vector;
    vector.appendArray(vec.data(), vec.size());
    return *const_cast<const Vector<T> *>(&vector);
}

template<typename T> Vector<T> toVector(hidl_vec<T> &vec) {
    Vector<T> vector;
    vector.appendArray(vec.data(), vec.size());
    return vector;
}

template<typename T, size_t SIZE> const Vector<T> toVector(
        const hidl_array<T, SIZE> &array) {
    Vector<T> vector;
    vector.appendArray(array.data(), array.size());
    return vector;
}

template<typename T, size_t SIZE> Vector<T> toVector(
        hidl_array<T, SIZE> &array) {
    Vector<T> vector;
    vector.appendArray(array.data(), array.size());
    return vector;
}


template<typename T> const hidl_vec<T> stdtoHidlVec(const std::vector<T> &vec) {
    hidl_vec<T> hVec;
    hVec.setToExternal(const_cast<T *>(vec.data()), vec.size());
    return hVec;
}

template<typename T> hidl_vec<T> stdtoHidlVec(std::vector<T> &vec) {
    hidl_vec<T> hVec;
    hVec.setToExternal(vec.data(), vec.size());
    return hVec;
}

template<typename T> const std::vector<T> stdtoVector(const hidl_vec<T> &hVec) {
    std::vector<T> vec;
    vec.assign(hVec.data(), hVec.data() + hVec.size());
    return *const_cast<const std::vector<T> *>(&vec);
}

template<typename T> std::vector<T> stdtoVector(hidl_vec<T> &hVec) {
    std::vector<T> vec;
    vec.assign(hVec.data(), hVec.data() + hVec.size());
    return vec;
}

template<typename T, size_t SIZE> const std::vector<T> stdtoVector(
        const hidl_array<T, SIZE> &hArray) {
    std::vector<T> vec;
    vec.assign(hArray.data(), hArray.data() + hArray.size());
    return vec;
}

template<typename T, size_t SIZE> std::vector<T> stdtoVector(
        hidl_array<T, SIZE> &hArray) {
    std::vector<T> vec;
    vec.assign(hArray.data(), hArray.data() + hArray.size());
    return vec;
}

Status toStatus(status_t legacyStatus);

inline Status toStatus_1_0(Status_V1_2 status) {
  switch (status) {
    case Status_V1_2::ERROR_DRM_INSUFFICIENT_SECURITY:
    case Status_V1_2::ERROR_DRM_FRAME_TOO_LARGE:
    case Status_V1_2::ERROR_DRM_SESSION_LOST_STATE:
      return Status::ERROR_DRM_UNKNOWN;
    default:
      return static_cast<Status>(status);
  }
}

inline Status_V1_2 toStatus_1_2(Status status) {
    return static_cast<Status_V1_2>(status);
}

}  // namespace widevine
}  // namespace V1_2
}  // namespace drm
}  // namespace hardware
}  // namespace android

#endif // ANDROID_HARDWARE_DRM_V1_1_TYPECONVERT