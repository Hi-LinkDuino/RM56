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

#include "power_spec.h"

namespace OHOS {
namespace bluetooth {
const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_AG = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::LEVEL_MID, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_HF = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::LEVEL_MID, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_A2DP_SRC = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_A2DP_SINK = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_3000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_AVRCP_CT = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_MAP_MSE = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_PBAP_PCE = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_1000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_PBAP_PSE = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_7000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_SPP = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_GATT_CLIENT = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_10000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_10000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<RequestStatus, PowerInfo> PowerSpec::MODE_SPEC_GATT_SERVER = {
    {RequestStatus::CONNECT_ON, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::SCO_ON, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::SCO_OFF, PowerInfo(PowerModeLevel::NO_ACTION, 0)},
    {RequestStatus::IDLE, PowerInfo(PowerModeLevel::LEVEL_LOW, SNIFF_DELAYSET_TIMEOUT_5000_MS)},
    {RequestStatus::BUSY, PowerInfo(PowerModeLevel::LEVEL_ACTIVE, 0)},
};

const std::map<std::string, std::map<RequestStatus, PowerInfo>> PowerSpec::POWER_MODE_SPEC = {
    {PROFILE_NAME_HFP_AG, MODE_SPEC_AG},
    {PROFILE_NAME_HFP_HF, MODE_SPEC_HF},
    {PROFILE_NAME_A2DP_SRC, MODE_SPEC_A2DP_SRC},
    {PROFILE_NAME_A2DP_SINK, MODE_SPEC_A2DP_SINK},
    {PROFILE_NAME_AVRCP_CT, MODE_SPEC_AVRCP_CT},
    {PROFILE_NAME_MAP_MSE, MODE_SPEC_MAP_MSE},
    {PROFILE_NAME_PBAP_PCE, MODE_SPEC_PBAP_PCE},
    {PROFILE_NAME_PBAP_PSE, MODE_SPEC_PBAP_PSE},
    {PROFILE_NAME_SPP, MODE_SPEC_SPP},
    {PROFILE_NAME_GATT_CLIENT, MODE_SPEC_GATT_CLIENT},
    {PROFILE_NAME_GATT_SERVER, MODE_SPEC_GATT_SERVER},
};

const std::map<PowerModeLevel, PowerParam> PowerSpec::POWER_PARAM = {
    {PowerModeLevel::LEVEL_LOW,
        PowerParam(SNIFF_LEVEL_LOW_MAX_INTERVAL, SNIFF_LEVEL_LOW_MIN_INTERVAL, SNIFF_LEVEL_LOW_ATTEMPT,
            SNIFF_LEVEL_LOW_TIMEOUT)},
    {PowerModeLevel::LEVEL_MID,
        PowerParam(SNIFF_LEVEL_MID_MAX_INTERVAL, SNIFF_LEVEL_MID_MIN_INTERVAL, SNIFF_LEVEL_MID_ATTEMPT,
            SNIFF_LEVEL_MID_TIMEOUT)},
    {PowerModeLevel::LEVEL_HIG,
        PowerParam(SNIFF_LEVEL_HIG_MAX_INTERVAL, SNIFF_LEVEL_HIG_MIN_INTERVAL, SNIFF_LEVEL_HIG_ATTEMPT,
            SNIFF_LEVEL_HIG_TIMEOUT)},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_AG = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_HF = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_A2DP_SRC = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
    {RequestStatus::IDLE, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_A2DP_SINK = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_AVRCP_CT = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_MAP_MSE = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_PBAP_PCE = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_PBAP_PSE = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_SPP = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_GATT_CLIENT = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<RequestStatus, PowerSsrLevel> PowerSpec::SSR_SPEC_GATT_SERVER = {
    {RequestStatus::CONNECT_ON, PowerSsrLevel::SSR2},
    {RequestStatus::SCO_ON, PowerSsrLevel::SSR0},
    {RequestStatus::SCO_OFF, PowerSsrLevel::SSR2},
};

const std::map<std::string, std::map<RequestStatus, PowerSsrLevel>> PowerSpec::SSR_SPEC = {
    {PROFILE_NAME_HFP_AG, SSR_SPEC_AG},
    {PROFILE_NAME_HFP_HF, SSR_SPEC_HF},
    {PROFILE_NAME_A2DP_SRC, SSR_SPEC_A2DP_SRC},
    {PROFILE_NAME_A2DP_SINK, SSR_SPEC_A2DP_SINK},
    {PROFILE_NAME_AVRCP_CT, SSR_SPEC_AVRCP_CT},
    {PROFILE_NAME_MAP_MSE, SSR_SPEC_MAP_MSE},
    {PROFILE_NAME_PBAP_PCE, SSR_SPEC_PBAP_PCE},
    {PROFILE_NAME_PBAP_PSE, SSR_SPEC_PBAP_PSE},
    {PROFILE_NAME_SPP, SSR_SPEC_SPP},
    {PROFILE_NAME_GATT_CLIENT, SSR_SPEC_GATT_CLIENT},
    {PROFILE_NAME_GATT_SERVER, SSR_SPEC_GATT_SERVER},
};

const std::map<PowerSsrLevel, PowerSsrParam> PowerSpec::POWER_SSR_PARAMS = {
    {PowerSsrLevel::SSR0, PowerSsrParam(SSR0_MAX_LATENCY, SSR0_MIN_REMOTE_TIMEOUT, SSR0_MIN_LOCAL_TIMEOUT)},
    {PowerSsrLevel::SSR1, PowerSsrParam(SSR1_MAX_LATENCY, SSR1_MIN_REMOTE_TIMEOUT, SSR1_MIN_LOCAL_TIMEOUT)},
    {PowerSsrLevel::SSR2, PowerSsrParam(SSR2_MAX_LATENCY, SSR2_MIN_REMOTE_TIMEOUT, SSR2_MIN_LOCAL_TIMEOUT)},
    {PowerSsrLevel::SSR3, PowerSsrParam(SSR3_MAX_LATENCY, SSR3_MIN_REMOTE_TIMEOUT, SSR3_MIN_LOCAL_TIMEOUT)},
};

PowerInfo PowerSpec::GetPowerSpec(const std::string &profileName, RequestStatus status)
{
    auto profileIts = POWER_MODE_SPEC.find(profileName);
    if (profileIts != POWER_MODE_SPEC.end()) {
        auto its = profileIts->second.find(status);
        if (its != profileIts->second.end()) {
            return its->second;
        }
    }
    return PowerInfo(PowerModeLevel::NO_ACTION, 0);
}

PowerParam PowerSpec::GetPowerParam(PowerModeLevel level)
{
    auto its = POWER_PARAM.find(level);
    if (its != POWER_PARAM.end()) {
        return its->second;
    }
    return PowerParam(0, 0, 0, 0);
}

PowerSsrLevel PowerSpec::GetPowerSsrLevel(const std::string &profileName, RequestStatus status)
{
    auto profileIts = SSR_SPEC.find(profileName);
    if (profileIts != SSR_SPEC.end()) {
        auto its = profileIts->second.find(status);
        if (its != profileIts->second.end()) {
            return its->second;
        }
    }
    return PowerSsrLevel::NO_ACTION;
}

PowerSsrParam PowerSpec::GetPowerSsrParam(PowerSsrLevel level)
{
    auto iter = POWER_SSR_PARAMS.find(level);
    if (iter != POWER_SSR_PARAMS.end()) {
        return iter->second;
    }
    return PowerSsrParam(0, 0, 0);
}
}  // namespace bluetooth
}  // namespace OHOS