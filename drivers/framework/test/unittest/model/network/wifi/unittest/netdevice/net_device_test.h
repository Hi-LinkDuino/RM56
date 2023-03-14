/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WIFI_NET_DEVICE_TEST_H
#define WIFI_NET_DEVICE_TEST_H

#include "hdf_base.h"

int32_t WiFiNetDviceTestInit(void);
int32_t WiFiNetDviceTestDeInit(void);
int32_t WiFiNetDviceTestAdd(void);
int32_t WiFiNetDviceTestDelete(void);
int32_t WiFiNetDviceTestGetDevice(void);
int32_t WiFiNetDviceTestGetCount(void);
int32_t WiFiNetDviceTestGetCap(void);
int32_t WiFiNetDviceTestSetAddr(void);
int32_t WiFiNetDviceTestRx(void);
int32_t WiFiNetDviceTestSetStatus(void);
int32_t WiFiNetDviceTestSetLinkStatus(void);
int32_t WifiNetDeviceDhcpClient(void);
int32_t WifiNetDeviceDhcpServer(void);

#endif