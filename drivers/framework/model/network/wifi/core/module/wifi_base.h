/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_WIFI_BASE_H
#define HDF_WIFI_BASE_H

#include "hdf_device_desc.h"

#define ERROR_DESC_READ_REQ_FAILED "Read request data failed"
#define ERROR_DESC_WRITE_RSP_FAILED "Write response data failed"

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_NETDEVICE_IS_USING      (-101)   /**< NetDevice is using, can not do reset. */
#define ERR_DEINIT_DRIVER_FAIL      (-102)   /**< Deinit driver failed. */
#define ERR_DEINIT_HI3881CHIP_FAIL  (-103)   /**< Deinit hi3881 chip failed. */
#define ERR_POWER_RESET_FAIL        (-104)   /**< Power reset failed. */
#define ERR_INIT_DRIVER_FAIL        (-105)   /**< Init driver failed. */
#define ERR_INIT_HI3881CHIP_FAIL    (-106)   /**< Init hi3881 chip failed. */

enum WifiClientType {
    WIFI_KERNEL_TO_WPA_CLIENT = 0,
    WIFI_KERNEL_TO_HAL_CLIENT = 1,
    WIFI_CLIENT_BUTT
};

struct HdfWifiEventToClientMap {
    struct HdfDeviceIoClient *wpaClient;                  /**< Structure of the Wpa Device Client */
    struct HdfDeviceIoClient *halClient;                  /**< Structure of the Ifeature Device Client */
};

struct HdfWifiEventToClientMap *HdfWifiGetEventToClientMap(void);

int32_t BaseInit(void);
int32_t BaseDeinit(void);

#ifdef __cplusplus
}
#endif
#endif // #ifndef HDF_WIFI_FEATURE_H