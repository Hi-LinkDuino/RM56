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

#ifndef POWER_SPEC_H
#define POWER_SPEC_H

#include <map>
#include <string>
#include <vector>

#include "interface_profile.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
#define SNIFF_LEVEL_LOW_MAX_INTERVAL 800
#define SNIFF_LEVEL_LOW_MIN_INTERVAL 400
#define SNIFF_LEVEL_LOW_ATTEMPT 4
#define SNIFF_LEVEL_LOW_TIMEOUT 1

#define SNIFF_LEVEL_MID_MAX_INTERVAL 150
#define SNIFF_LEVEL_MID_MIN_INTERVAL 50
#define SNIFF_LEVEL_MID_ATTEMPT 4
#define SNIFF_LEVEL_MID_TIMEOUT 1

#define SNIFF_LEVEL_HIG_MAX_INTERVAL 18
#define SNIFF_LEVEL_HIG_MIN_INTERVAL 14
#define SNIFF_LEVEL_HIG_ATTEMPT 1
#define SNIFF_LEVEL_HIG_TIMEOUT 0

#define SSR0_MAX_LATENCY 0
#define SSR0_MIN_REMOTE_TIMEOUT 0
#define SSR0_MIN_LOCAL_TIMEOUT 0

#define SSR1_MAX_LATENCY 0
#define SSR1_MIN_REMOTE_TIMEOUT 0
#define SSR1_MIN_LOCAL_TIMEOUT 2

#define SSR2_MAX_LATENCY 1200
#define SSR2_MIN_REMOTE_TIMEOUT 2
#define SSR2_MIN_LOCAL_TIMEOUT 2

#define SSR3_MAX_LATENCY 360
#define SSR3_MIN_REMOTE_TIMEOUT 160
#define SSR3_MIN_LOCAL_TIMEOUT 1600

#define SNIFF_DELAYSET_TIMEOUT_1000_MS 1000
#define SNIFF_DELAYSET_TIMEOUT_3000_MS 3000
#define SNIFF_DELAYSET_TIMEOUT_5000_MS 5000
#define SNIFF_DELAYSET_TIMEOUT_7000_MS 7000
#define SNIFF_DELAYSET_TIMEOUT_10000_MS 10000

enum {
    POWER_MODE_PARK,
    POWER_MODE_HOLD,
    POWER_MODE_SNIFF,
    POWER_MODE_ACTIVE,
};

enum class PowerModeLevel : int {
    NO_ACTION = 0,
    LEVEL_LOW = 1,
    LEVEL_MID = 2,
    LEVEL_HIG = 3,
    LEVEL_ACTIVE = 4,
};

enum class PowerSsrLevel : int {
    SSR0 = 0,
    SSR1 = 1,
    SSR2 = 2,
    SSR3 = 3,
    NO_ACTION = 4,
};

/**
 * @brief Represents set btm power param information.
 *
 * @since 6
 */
struct PowerParam {
public:
    /**
     * @brief A constructor used to create an <b>PowerParam</b> instance.
     *
     * @since 6
     */
    PowerParam() = delete;
    /**
     * @brief A destructor used to delete the <b>PowerParam</b> instance.
     *
     * @since 6
     */
    ~PowerParam(){};
    /**
     * @brief A constructor used to create an <b>PowerInfo</b> instance.
     *
     * @param max Max interval.
     * @param min Min interval.
     * @param attempt Attempt time.
     * @param timeout Timeout period.
     * @since 6
     */
    PowerParam(int max, int min, int attempt, int timeout)
        : maxInterval_(max), minInterval_(min), attempt_(attempt), timeout_(timeout){};

    int maxInterval_ {0};
    int minInterval_ {0};
    int attempt_ {0};
    int timeout_ {0};
};

struct PowerSsrParam {
    /**
     * @brief A constructor used to create an <b>PowerSsrParam</b> instance.
     *
     * @since 6
     */
    PowerSsrParam() = delete;
    /**
     * @brief A destructor used to delete the <b>PowerSsrParam</b> instance.
     *
     * @since 6
     */
    ~PowerSsrParam(){};
    /**
     * @brief A constructor used to create an <b>PowerSsrParam</b> instance.
     *
     * @param maxLat Max latency.
     * @param minRmtTo Min remote timeout.
     * @param minLocTo Min local timeout.
     * @since 6
     */
    PowerSsrParam(int maxLat, int minRmtTo, int minLocTo)
        : maxLat_(maxLat), minRmtTo_(minRmtTo), minLocTo_(minLocTo) {};

    int maxLat_ {0};
    int minRmtTo_ {0};
    int minLocTo_ {0};
};

/**
 * @brief Represents power mode information.
 *
 * @since 6
 */
struct PowerInfo {
public:
    /**
     * @brief A constructor used to create an <b>PowerInfo</b> instance.
     *
     * @since 6
     */
    PowerInfo() = delete;
    /**
     * @brief A destructor used to delete the <b>PowerInfo</b> instance.
     *
     * @since 6
     */
    ~PowerInfo(){};
    /**
     * @brief A constructor used to create an <b>PowerInfo</b> instance.
     *
     * @param mode power mode.
     * @param timer timer out.
     * @since 6
     */
    PowerInfo(PowerModeLevel mode, int timer) : powerMode_(mode), timeout_(timer){};

    PowerModeLevel powerMode_ {PowerModeLevel::NO_ACTION};
    int timeout_ {};
};

class PowerSpec {
public:
    /**
     * @brief A constructor used to create an <b>PowerSpec</b> instance.
     *
     * @since 6
     */
    PowerSpec(){};
    /**
     * @brief A destructor used to delete the <b>PowerSpec</b> instance.
     *
     * @since 6
     */
    ~PowerSpec(){};

    /**
     * @brief Get PowerInfo.
     *
     * @param profileName Profile name.
     * @param status Request status.
     * @return Returns PowerInfo.
     * @since 6
     */
    static PowerInfo GetPowerSpec(const std::string &profileName, RequestStatus status);

    /**
     * @brief Get PowerParam, base on PowerModeLevel.
     *
     * @param level PowerModeLevel.
     * @return Returns PowerParam.
     * @since 6
     */
    static PowerParam GetPowerParam(PowerModeLevel level);

    /**
     * @brief Get PowerSsrLevel.
     *
     * @param profileName Profile name.
     * @param status Request status.
     * @return Returns PowerSsrLevel.
     * @since 6
     */
    static PowerSsrLevel GetPowerSsrLevel(const std::string &profileName, RequestStatus status);

    /**
     * @brief Get PowerSsrParam, base on PowerSsrLevel.
     *
     * @param level PowerSsrLevel.
     * @return Returns PowerSsrParam.
     * @since 6
     */
    static PowerSsrParam GetPowerSsrParam(PowerSsrLevel level);

private:
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_AG;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_HF;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_A2DP_SRC;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_A2DP_SINK;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_AVRCP_CT;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_MAP_MSE;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_PBAP_PCE;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_PBAP_PSE;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_SPP;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_GATT_CLIENT;
    static const std::map<RequestStatus, PowerInfo> MODE_SPEC_GATT_SERVER;
    static const std::map<std::string, std::map<RequestStatus, PowerInfo>> POWER_MODE_SPEC;
    static const std::map<PowerModeLevel, PowerParam> POWER_PARAM;

    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_AG;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_HF;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_A2DP_SRC;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_A2DP_SINK;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_AVRCP_CT;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_MAP_MSE;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_PBAP_PCE;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_PBAP_PSE;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_SPP;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_GATT_CLIENT;
    static const std::map<RequestStatus, PowerSsrLevel> SSR_SPEC_GATT_SERVER;
    static const std::map<std::string, std::map<RequestStatus, PowerSsrLevel>> SSR_SPEC;
    static const std::map<PowerSsrLevel, PowerSsrParam> POWER_SSR_PARAMS;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // POWER_SPEC_H