/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef PNP_MESSAGE_REPORT_H
#define PNP_MESSAGE_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PNP_REPORT_MSG_LEN_MAX      32
#define PNP_REPORT_RESERVED         255

enum PnpReportType {
    DEVICE_PULG,
    EVENT_REPORT,
    REPORT_TYPE_INVALID
};

enum PnpReportEventId {
    CAPTURE_THRESHOLD = 0,
    LOAD_ADAPTER,
    SERVICE_STATUS,
};

enum PnpServiceStatus {
    SERVICE_INVALID = 0,
    SERVICE_ENABLE,
    SERVICE_INIT,
};

enum DevPlugState {
    HDF_AUDIO_DEVICE_OFFLINE = 0,
    HDF_AUDIO_DEVICE_ONLINE,
};

enum DevPlugDevType {
    DEV_PRIMARY = 0,
    DEV_USB,
    DEV_A2DP,
};

enum AdapterLoadStatus {
    LOAD_SUCCESS = 0,
    LOAD_FAILURE = 1,
};

struct PnpReportDevPlugMsg {
    uint8_t eventType;
    uint8_t state;
    uint8_t deviceType;
    uint8_t deviceCap;
    uint8_t id;
};

struct PnpReportEventMsg {
    uint8_t eventType;
    uint8_t eventId;
    uint8_t eventValue;
    uint8_t deviceType;
    uint8_t reserve;      /* Reserved fields are not used for the time being */
};

struct PnpReportMsg {
    enum PnpReportType reportType;
    union {
        struct PnpReportDevPlugMsg devPlugMsg;
        struct PnpReportEventMsg eventMsg;
    };
};

int32_t PnpReportMsgDeSerialize(uint8_t *msgStr, enum PnpReportType msgType, struct PnpReportMsg *pnpReportMsg);
char *PnpReportMsgSerialize(struct PnpReportMsg *pnpReportMsg);

#ifdef __cplusplus
    }
#endif
#endif /* PNP_MESSAGE_REPORT_H */

