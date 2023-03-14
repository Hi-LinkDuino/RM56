/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef WIFI_NAPI_HOTSPOT_H_
#define WIFI_NAPI_HOTSPOT_H_

#include "wifi_napi_utils.h"

namespace OHOS {
namespace Wifi {
napi_value EnableHotspot(napi_env env, napi_callback_info info);
napi_value DisableHotspot(napi_env env, napi_callback_info info);
napi_value IsHotspotActive(napi_env env, napi_callback_info info);
napi_value IsHotspotDualBandSupported(napi_env env, napi_callback_info info);
napi_value SetHotspotConfig(napi_env env, napi_callback_info info);
napi_value GetHotspotConfig(napi_env env, napi_callback_info info);
napi_value GetStations(napi_env env, napi_callback_info info);
}  // namespace Wifi
}  // namespace OHOS

#endif
