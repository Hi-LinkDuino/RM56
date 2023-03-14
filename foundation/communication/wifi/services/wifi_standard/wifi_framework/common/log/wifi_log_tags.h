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

#ifndef OHOS_WIFI_LOG_TAGS_H
#define OHOS_WIFI_LOG_TAGS_H

namespace OHOS {
namespace Wifi {
const unsigned int LOG_ID_WIFI = 0xD001560;
const unsigned int LOG_ID_WIFI_HOTSPOT = LOG_ID_WIFI | 0x01;
const unsigned int LOG_ID_WIFI_SCAN = LOG_ID_WIFI | 0x02;
const unsigned int LOG_ID_WIFI_P2P = LOG_ID_WIFI | 0x03;
const unsigned int LOG_ID_WIFI_AWARE = LOG_ID_WIFI | 0x04;
const unsigned int LOG_ID_WIFI_DHCP = LOG_ID_WIFI | 0x05;
}  // namespace Wifi
}  // namespace OHOS
#endif