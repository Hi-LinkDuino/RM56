/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HDI_DEVICE_COMMON_H
#define HDI_DEVICE_COMMON_H
#include <memory>
#include <unordered_map>
#include "display_type.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const int32_t INVALID_MODE_ID = -1;
const uint32_t DRM_INVALID_ID = 0xFFFFFFFF;
template<typename T> using IdMapPtr = std::unordered_map<uint32_t, std::shared_ptr<T>>;
class DrmEncoder;
class DrmCrtc;
class DrmPlane;
class DrmDevice;
class DrmConnector;
class VsyncCallBack;
class DrmVsyncWorker;
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_DEVICE_COMMON_H