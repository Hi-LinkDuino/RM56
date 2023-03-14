/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_MOCK_AP_CONFIG_USE_H
#define OHOS_MOCK_AP_CONFIG_USE_H

#include <gmock/gmock.h>
#include <vector>

#include "ap_macro.h"
#include "wifi_msg.h"
#include "ap_config_use.h"
namespace OHOS {
namespace Wifi {
class MockApConfigUse : public ApConfigUse {
public:
    MOCK_CONST_METHOD2(
        TransformFrequencyIntoChannel, void(const std::vector<int> &FreqVector, std::vector<int> &ChanVector));
    MOCK_CONST_METHOD1(LogConfig, void(HotspotConfig &apConfig));
    MOCK_CONST_METHOD1(IsValid24GHz, bool(int freq));
    MOCK_CONST_METHOD1(IsValid5GHz, bool(int freq));
    MOCK_CONST_METHOD2(CheckBandChannel, void(HotspotConfig &apConfig, const ChannelsTable &validChanTable));
    MOCK_CONST_METHOD1(TransformFrequencyIntoChannel, int(int freq));
};
} // namespace Wifi
} // namespace OHOS
#endif