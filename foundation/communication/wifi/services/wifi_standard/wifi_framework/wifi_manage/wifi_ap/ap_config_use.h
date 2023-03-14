/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_AP_CONFIG_UTIL_H
#define OHOS_AP_CONFIG_UTIL_H

#include <vector>
#include <map>
#include "ap_define.h"
#include "ap_macro.h"
#include "wifi_ap_msg.h"

namespace OHOS {
namespace Wifi {
constexpr int FREP_2G_MIN = 2412;
constexpr int FREP_2G_MAX = 2472;
constexpr int FREP_5G_MIN = 5170;
constexpr int FREP_5G_MAX = 5825;
constexpr int CHANNEL_14_FREP = 2484;
constexpr int CHANNEL_14 = 14;
constexpr int CENTER_FREP_DIFF = 5;
constexpr int CHANNEL_2G_MIN = 1;
constexpr int CHANNEL_5G_MIN = 34;

using ChannelsTable = std::map<BandType, std::vector<int32_t>>;
class ApConfigUse {
    FRIEND_GTEST(ApIdleState);

public:
    /**
     * @Description  Convert the frequency in the container into a channel.
     * @param freqVector - frequency vector input
     * @param chanVector - Channel vector output
     * @return None
     */
    virtual void TransformFrequencyIntoChannel(const std::vector<int> &freqVector, std::vector<int> &chanVector) const;
    /**
     * @Description  Printf the hostapd cfg to log display.
     * @param apConfig - configuration input
     * @return None
     */
    virtual void LogConfig(HotspotConfig &apConfig) const;
    /**
     * @Description  Check is a valid 2.4G frequency.
     * @param freq - Frequency input
     * @return true: is valid    false: bad frequency
     */
    virtual bool IsValid24GHz(int freq) const;
    /**
     * @Description  Check is a valid 5G frequency.
     * @param freq - Frequency input
     * @return true: is valid    false: bad frequency
     */
    virtual bool IsValid5GHz(int freq) const;

    /**
     * @Description  Obtain and report available channel information.
     * @param apConfig - configuration input
     * @param validChanTable - Valid channel tables.
     * @return None
     */
    virtual void CheckBandChannel(HotspotConfig &apConfig, const ChannelsTable &validChanTable) const;

    /**
     * @Description  construction method
     * @param None
     * @return None
     */
    ApConfigUse();
    /**
     * @Description  destructor method
     * @param None
     * @return None
     */
    virtual ~ApConfigUse();
    /**
     * @Description  Convert frequency to channel number.
     * @param freq - frequency to convert
     * @return success: channel num    failed: -1
     */
    virtual int TransformFrequencyIntoChannel(const int freq) const;

private:
    DISALLOW_COPY_AND_ASSIGN(ApConfigUse)
};
}  // namespace Wifi
}  // namespace OHOS

#endif