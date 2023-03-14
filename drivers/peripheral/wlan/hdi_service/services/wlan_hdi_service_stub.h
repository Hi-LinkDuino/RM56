/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WLAN_HDI_SERVICE_STUD_H
#define WLAN_HDI_SERVICE_STUD_H

#include <hdf_base.h>
#include <hdf_sbuf.h>
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <hdf_device_desc.h>
#include <osal_mem.h>
#include "osal_mutex.h"
#include "hdf_dlist.h"
#include "securec.h"
#include "wifi_hal_ap_feature.h"
#include "wifi_hal_base_feature.h"
#include "wifi_hal_sta_feature.h"
#include "wifi_hal.h"
#include "wifi_hal_cmd.h"

struct HdfDeviceObject;
struct HdfDeviceIoClient;

enum InterFacesCmd {
    WLAN_SERVICE_CONSTRUCT = 0,
    WLAN_SERVICE_DECONSTRUCT,
    WLAN_SERVICE_START,
    WLAN_SERVICE_STOP,
    WLAN_SERVICE_GET_SUPPORT_FEATURE,
    WLAN_SERVICE_GET_SUPPORT_COMBO,
    WLAN_SERVICE_CREATE_FEATURE,
    WLAN_SERVICE_GET_FEATURE_NAME,
    WLAN_SERVICE_REGISTER_CALLBACK,
    WLAN_SERVICE_UNREGISTER_CALLBACK,
    WLAN_SERVICE_DESTROY_FEATURE,
    WLAN_SERVICE_RESET_DRIVER,
    WLAN_SERVICE_GET_ASSCOCIATE_STA,
    WLAN_SERVICE_SET_COUNTRY_CODE,
    WLAN_SERVICE_GET_NETWORK_NAME,
    WLAN_SERVICE_GET_FEATURE_TYPE,
    WLAN_SERVICE_SET_MAC_ADDR,
    WLAN_SERVICE_GET_MAC_ADDR,
    WLAN_SERVICE_GET_FREQ_WITHBAND,
    WLAN_SERVICE_SET_TX_POWR,
    WLAN_SERVICE_GET_CHIPID,
    WLAN_SERVICE_GET_NAME_BYCHIPID,
    WLAN_SERVICE_SET_SACN_MACADDR,
    WLAN_SERVICE_GET_NETDEV_INFO,
    WLAN_SERVICE_START_SCAN,
};

struct WifiHdi {
    int32_t (*callback)(struct HdfDeviceObject *device, struct HdfRemoteService *callback, uint32_t event,
        void *data, const char *ifName);
};

struct FeatureInfo {
    char *ifName;
    int32_t type;
};

struct HdfWlanRemoteNode {
    const struct HdfDeviceIoClient *client;
    struct HdfRemoteService* callbackObj;
    struct DListHead node;
};

struct HdfWlanStubData {
    struct DListHead remoteListHead;
    struct OsalMutex mutex;
};

struct HdfWlanStubData* HdfStubDriver(void);

int32_t WlanHdiServiceOnRemoteRequest(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply);

#endif