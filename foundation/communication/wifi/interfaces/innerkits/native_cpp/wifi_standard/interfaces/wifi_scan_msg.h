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

#ifndef OHOS_WIFI_SCAN_MSG_H
#define OHOS_WIFI_SCAN_MSG_H

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

namespace OHOS {
namespace Wifi {
#define MIN_SCAN_INTERVAL 20
#define DEFAULT_MAX_SCAN_INTERVAL 160
#define SCAN_SCENE_SCREEN_OFF 0       // Screen off state
#define SCAN_SCENE_SCANNING 1         // scanning state
#define SCAN_SCENE_CONNECTING 2       // connecting state
#define SCAN_SCENE_DISCONNCTED 3      // disconnected state
#define SCAN_SCENE_CONNECTED 4        // connected state
#define SCAN_SCENE_ASSOCIATING 5      // associating state
#define SCAN_SCENE_ASSOCIATED 6       // associated state
#define SCAN_SCENE_OBTAINING_IP 7     // Obtaining IP state
#define SCAN_SCENE_DEEP_SLEEP 8       // Deep sleep state
#define SCAN_SCENE_FREQUENCY_ORIGIN 9 // Scan frequency, origin.
#define SCAN_SCENE_FREQUENCY_CUSTOM 10 // Scan frequency, custom.
#define SCAN_SCENE_CUSTOM (SCAN_SCENE_FREQUENCY_CUSTOM + 1)

/* SCAN_SCENE_CUSTOM~253 Custom Scenario */
#define SCAN_SCENE_ALL 254 /* all Scenario */
#define SCAN_SCENE_MAX 255 /* invalid value */

/* Scanning mode of the control policy */
enum class ScanMode {
    APP_FOREGROUND_SCAN = 0, /* Scan initiated by the foreground application */
    APP_BACKGROUND_SCAN = 1, /* Scan initiated by background applications */
    SYS_FOREGROUND_SCAN = 2, /* System foreground scan */
    SYS_BACKGROUND_SCAN = 3, /* System background scan */
    ALL_EXTERN_SCAN = 4,     /* All external scans, including the first four */
    PNO_SCAN = 5,            /* PNO scan */
    SYSTEM_TIMER_SCAN = 6,   /* Scheduled system scan */
    ANYTIME_SCAN = 7,        /* Scan at any time */
    BAND_24GHZ_SCAN = 8,     /* 2.4 GHz scan */
    BAND_5GHZ_SCAN = 9,      /* 5G scan */
    SCAN_MODE_MAX            /* Invalid value */
};

enum class WifiSecurity {
    OPEN = 0,
    WEP = 1,
    PSK = 2,
    EAP = 3,
    SAE = 4,
    EAP_SUITE_B = 5,
    OWE = 6,
    WAPI_CERT = 7,
    WAPI_PSK = 8,
    INVALID = -1
};

enum class WifiChannelWidth {
    WIDTH_20MHZ = 0,
    WIDTH_40MHZ = 1,
    WIDTH_80MHZ = 2,
    WIDTH_160MHZ = 3,
    WIDTH_80MHZ_PLUS = 4,
    WIDTH_INVALID
};

struct WifiInfoElem {
    unsigned int id;
    std::vector<char> content;

    WifiInfoElem() : id(0)
    {}

    ~WifiInfoElem()
    {}
};

enum class ScanHandleNotify {
    SCAN_FAIL = 0,
    SCAN_OK = 1,
};

struct WifiScanParams {
    std::string ssid;
    std::string bssid;
    std::vector<int> freqs;
    unsigned int band;
    int scanStyle;

    WifiScanParams()
    {
        band = 0;
        scanStyle = 0xFF;
    }
};

/* scan result info */
struct WifiScanInfo {
    std::string bssid;
    std::string ssid;
    /**
     * Network performance, including authentication,
     * key management, and encryption mechanisms
     * supported by the access point
     */
    std::string capabilities;
    int frequency;
    int band;  /* ap band: 1 - 2.4GHZ, 2 - 5GHZ */
    WifiChannelWidth channelWidth;
    int centerFrequency0;
    int centerFrequency1;
    int rssi; /* signal level */
    WifiSecurity securityType;
    std::vector<WifiInfoElem> infoElems;
    int64_t features;
    int64_t timestamp;

    WifiScanInfo()
    {
        frequency = 0;
        band = 0;
        channelWidth = WifiChannelWidth::WIDTH_INVALID;
        centerFrequency0 = 0;
        centerFrequency1 = 0;
        rssi = 0;
        securityType = WifiSecurity::INVALID;
        features = 0;
        timestamp = 0;
    }
};

typedef struct tagScanForbidMode {
    int scanScene;     /* current scanned scene */
    int forbidTime;    /*
                        * Specifies the scanning duration.
                        * If the value is 0, all invalid values are restricted.
                        */
    int forbidCount;   /*
                        * Indicates the number of scanning times after a scanning scenario is entered.
                        * If the value is 0, all scanning times are restricted.
                        */
    ScanMode scanMode; /* Restricted Scan Mode */
    tagScanForbidMode()
    {
        forbidTime = 0;
        forbidCount = 0;
        scanMode = ScanMode::SCAN_MODE_MAX;
    }

    ~tagScanForbidMode()
    {}
} ScanForbidMode;

enum class IntervalMode {
    INTERVAL_FIXED = 0,     /*
                             * The interval is set to 120 and the count is set to 4.
                             * For example, the interval is set to 120 and the count is set to 4.
                             */
    INTERVAL_EXP = 1,       /*
                             * Exponential interval. The value of interval is the initial value.
                             * After the value is multiplied by 2, the last fixed interval is used.
                             */
    INTERVAL_CONTINUE = 2,  /*
                             * If the number of consecutive count times is less than interval,
                             * the subsequent interval must be greater than interval.
                             */
    INTERVAL_BLOCKLIST = 3, /*
                             * If the number of consecutive count times is less than the value of interval,
                             * the user is added to the blocklist and cannot be scanned.
                             */
    INTERVAL_MAX            /* invalid value */
};

typedef struct tagScanInterval {
    IntervalMode intervalMode; /* Interval mode, which can be interval or count. */
    int interval;              /* Interval, in seconds. */
    int count;                 /* Number of times allowed in the interval */
    tagScanInterval()
    {
        intervalMode = IntervalMode::INTERVAL_FIXED;
        interval = 0;
        count = 0;
    }
} ScanInterval;

typedef struct tagScanIntervalMode {
    int scanScene;             /*
                                * This parameter can be set to SCAN_SCENE_ALL
                                * if the configuration takes effect at intervals.
                                */
    ScanMode scanMode;         /* scan mode */
    bool isSingle;             /*
                                * Indicates whether to limit the time of a single application.
                                * If this parameter is set to false, the time of all applications is recorded.
                                */
    IntervalMode intervalMode; /* Interval mode, which can be interval or count. */
    int interval;              /* Interval, in seconds. */
    int count;                 /* Number of times allowed in the interval */
    tagScanIntervalMode()
    {
        scanScene = SCAN_SCENE_ALL;
        scanMode = ScanMode::SCAN_MODE_MAX;
        isSingle = false;
        intervalMode = IntervalMode::INTERVAL_FIXED;
        interval = 0;
        count = 0;
    }
} ScanIntervalMode;

typedef std::vector<ScanForbidMode> ScanForbidList;
typedef std::vector<ScanIntervalMode> ScanIntervalList;

typedef struct tagScanControlInfo {
    ScanForbidList scanForbidList;       /* Scanning forbidden list corresponding to the scenario */
    ScanIntervalList scanIntervalList; /*
                                        * Interval for scanning mode.
                                        * The value cannot be set to 2.4 GHz, 5 GHz, or anytime scan.
                                        */
} ScanControlInfo;

struct SystemScanIntervalMode {
    ScanIntervalMode scanIntervalMode;
    int expScanCount; /* INTERVAL_EXP scan mode,Number of Scanned Times */
    SystemScanIntervalMode()
    {
        expScanCount = 0;
    }
};

struct PnoScanIntervalMode {
    ScanIntervalMode scanIntervalMode;
    time_t fixedCurrentTime;
    int fixedScanCount;
    time_t fixedScanTime;
    PnoScanIntervalMode()
    {
        fixedCurrentTime = 0;
        fixedCurrentTime = 0;
        fixedScanTime = 0;
        fixedScanCount = 0;
    }
};
}  // namespace Wifi
}  // namespace OHOS
#endif