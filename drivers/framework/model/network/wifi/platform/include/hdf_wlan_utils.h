/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_WLAN_UTILS_H
#define HDF_WLAN_UTILS_H
#include "hdf_wifi_product.h"
#include "wifi_mac80211_ops.h"

const char *GetPlatformDriverName(void);

struct HdfWifiNetDeviceData *GetPlatformData(const struct NetDevice *netDev);

struct HdfChipDriver *GetChipDriver(const struct NetDevice *netDev);

struct WlanChannel* WifiGetChannel(struct WlanHwCapability* capality, int32_t freq);

struct WlanHwCapability* GetHwCapability(struct NetDevice *netDev);

NetDevice *AllocPlatformNetDevice(struct HdfWlanDevice *device);
int32_t RenewNetDevice(NetDevice **netDev);
int32_t ReleasePlatformNetDevice(struct NetDevice *netDev);

int32_t GetPlatformIfName(uint8_t id, char *ifName, uint32_t ifNameSize);
char* HdfWlanGetIfNames(const uint8_t chipId, uint8_t *ifNameCount);

#endif