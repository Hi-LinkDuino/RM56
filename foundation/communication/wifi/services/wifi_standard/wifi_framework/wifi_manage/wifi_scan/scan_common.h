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
#ifndef OHOS_SCAN_COMMON_H
#define OHOS_SCAN_COMMON_H

#include <functional>
#include <string>
#include <vector>
#include "wifi_scan_msg.h"
#include "wifi_internal_msg.h"

namespace OHOS {
namespace Wifi {
#define FRIEND_GTEST(test_typename) friend class test_typename##Test
static const int MIN_SYSTEM_SCAN_INTERVAL = 20;
static const int MAX_SYSTEM_SCAN_INTERVAL = 160;
static const int MAX_SCAN_CONFIG_STORE_INDEX = 10000;
static const int SECOND_TO_MILLI_SECOND = 1000;

enum ScanBandType {
    SCAN_BAND_UNSPECIFIED = 0,    /* not specified */
    SCAN_BAND_24_GHZ = 1,         /* 2.4 GHz band */
    SCAN_BAND_5_GHZ = 2,          /* 5 GHz band without DFS channels */
    SCAN_BAND_BOTH = 3,           /* both bands without DFS channels */
    SCAN_BAND_5_GHZ_DFS_ONLY = 4, /* 5 GHz band with DFS channels */
    SCAN_BAND_5_GHZ_WITH_DFS = 6, /* 5 GHz band with DFS channels */
    SCAN_BAND_BOTH_WITH_DFS = 7,  /* both bands with DFS channels */
};

/* Scan Parameter Configuration */
struct ScanConfig {
    std::vector<std::string> hiddenNetworkSsid; /* Hidden network can be detected */
                                                /* only after users manually add them */
    ScanBandType scanBand;                      /* A frequency segment */
    std::vector<int> scanFreqs;                 /*
                                                 * Only APs with specified frequencies are
                                                 * scanned. If this parameter is not set,all
                                                 * frequencies's ap are scanned
                                                 */
    int backScanPeriod;                         /* Scan interval for background scan */
    bool fullScanFlag;                          /* Flag indicating whether the request is full scan */
    bool externFlag;                            /* Flag indicating whether the request is an external scan */
    bool scanningWithParamFlag;                 /* Flag Indicating whether scanning with parameter */
    std::string ssid;                           /* The network name */
    std::string bssid;                          /* The address of the access point */
    int scanStyle;                              /* Type of scan to perform */

    ScanConfig()
    {
        scanBand = SCAN_BAND_UNSPECIFIED;
        backScanPeriod = 0;
        fullScanFlag = false;
        externFlag = false;
        scanningWithParamFlag = false;
        scanStyle = 0xFF;
    }
};

/* Parameters used by the scan service */
struct InterScanConfig {
    std::vector<std::string> hiddenNetworkSsid; /* Hidden network can be detected */
                                                /* only after users manually add them */
    std::vector<int> scanFreqs;                 /*
                                                 * Only AP with specified frequencies are scanned.
                                                 * If this parameter is not set,all frequencies's
                                                 * ap are scanned
                                                 */
    bool fullScanFlag;                          /* Flag of scan without specifying parameters */
    int backScanPeriod;                         /* Scan interval for background scan */
    int bssidsNumPerScan;                       /* Maximum number of records contained in each scan */
    int maxScansCache;                          /* Maximum number of cached scans */
    int maxBackScanPeriod;                      /* Maximum background scan interval */
    int scanStyle;                              /* Type of scan to perform */

    InterScanConfig()
    {
        fullScanFlag = false;
        backScanPeriod = 0;
        bssidsNumPerScan = 0;
        maxScansCache = 0;
        maxBackScanPeriod = 0;
        scanStyle = 0xFF;
    }
};

/* Saved parameters, which are filtered after the scanning result is received */
struct StoreScanConfig {
    std::string ssid;           /* Specifies the SSID in parameter scanning. */
    std::string bssid;          /* Specifies the BSSID carried in parameter scanning. */
    std::vector<int> scanFreqs; /* Only APs with specified frequencies are scanned. */
    int64_t scanTime;           /* Scan Start Time */
    bool fullScanFlag;          /* Flag of scan without specifying parameters */
    bool externFlag;            /* Flag indicating whether the request is an external scan. */
    bool scanningWithParamFlag; /* Flag Indicating whether scanning with parameter */
    StoreScanConfig()
    {
        scanTime = 0;
        fullScanFlag = false;
        externFlag = false;
        scanningWithParamFlag = false;
    }
};

/* PNO Scanning Parameter Configuration */
struct PnoScanConfig {
    int scanInterval;                           /* PNO scanning interval */
    int minRssi2Dot4Ghz;                        /* Minimum 2.4 GHz network signal strength */
    int minRssi5Ghz;                            /* Minimum 5 GHz network signal strength */
    std::vector<std::string> hiddenNetworkSsid; /* Hidden network name */
    std::vector<std::string> savedNetworkSsid;  /* Saved network name */
    std::vector<int> freqs;                     /* Scan at a specified frequency */

    PnoScanConfig()
    {
        scanInterval = 0;
        minRssi2Dot4Ghz = 0;
        minRssi5Ghz = 0;
    }
};

/* Scan command */
enum ScanCommond {
    CMD_SCAN_PREPARE = 0,      /* Preparations and driver loading */
    CMD_START_COMMON_SCAN = 1, /* Start a common scan */
    CMD_STOP_COMMON_SCAN = 2,  /* Stopping a common scan */
    CMD_START_PNO_SCAN = 3,    /* Start PNO scanning */
    CMD_STOP_PNO_SCAN = 4,     /* Stop PNO Scanning */
    CMD_RESTART_PNO_SCAN = 5,  /* Restart PNO Scanning */
    CMD_SCAN_FINISH = 6,       /* Stop PNO scanning and uninstall the driver */
};

/* Monitored scanning events */
enum ScanEventType {
    HARDWARE_LOAD_EVENT = 20,   /* The hardware is successfully loaded */
    SCAN_RESULT_EVENT = 21,     /* The common scan is successful. The scan result can be obtained */
    PNO_SCAN_RESULT_EVENT = 22, /* The PNO scanning result can be obtained */
    SCAN_FAILED_EVENT = 23,     /* Common scan failure */
    HARDWARE_UNLOAD_EVENT = 24, /* Hardware uninstalled successfully */
};

/* Internal event */
enum ScanInnerEventType {
    WAIT_SCAN_RESULT_TIMER = 100,     /* Scan result waiting timer */
    WAIT_PNO_SCAN_RESULT_TIMER = 101, /* PNO scan result waiting timer */
    SOFTWARE_PNO_SCAN_TIMER = 102,    /* PNO software scanning timer */

    /* ---------------------------Used for ScanServices ------------------- */
    SYSTEM_SCAN_TIMER = 200,       /* Automatic scanning */
    DISCONNECTED_SCAN_TIMER = 201, /* Automatic scanning in disconnected state */
    RESTART_PNO_SCAN_TIMER = 202,  /* Restart after PNO scanning fails */
    SCAN_INNER_EVENT_INVALID       /* Invalid value */
};

/* State machine status reporting */
enum ScanStatus {
    SCAN_STARTED_STATUS = 0,  /* Started successfully */
    SCAN_FINISHED_STATUS = 1, /* End processing completed */
    COMMON_SCAN_SUCCESS = 2,  /* Notify the scan result after the common scan is complete */
    COMMON_SCAN_FAILED = 3,   /* Common scan failure */
    PNO_SCAN_INFO = 4,      /* The PNO scan is complete and the scanning result is notified */
    PNO_SCAN_FAILED = 5,      /* Failed to start the PNO scanning */
    SCAN_INNER_EVENT = 6,     /* Report internal events */
    SCAN_STATUS_INVALID       /* Invalid value */
};

struct ScanStatusReport {
    ScanStatus status;                           /* status code */
    std::vector<int> requestIndexList;           /*
                                                  * Request index list,
                                                  * which is used for common scanning of reported events.
                                                  */
    std::vector<InterScanInfo> scanInfoList; /*
                                                  * Scan result,
                                                  * which is used for reporting
                                                  * common and PNO scan results.
                                                  */
    ScanInnerEventType innerEvent;               /* Internal event, which is used by timer events. */

    ScanStatusReport()
    {
        status = SCAN_STATUS_INVALID;
        innerEvent = SCAN_INNER_EVENT_INVALID;
    }
};

using ScanStatusReportHandler = std::function<void(ScanStatusReport &scanStatusReport)>;
using ScanInfoHandler = std::function<void(std::vector<InterScanInfo> &scanInfoList)>;
using PnoScanInfoHandler = std::function<void(std::vector<InterScanInfo> &pnoScanInfoList)>;
}  // namespace Wifi
}  // namespace OHOS
#endif