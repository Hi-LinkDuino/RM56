/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WIFI_FLOW_CONTROL_TEST_H
#define WIFI_FLOW_CONTROL_TEST_H
#include "hdf_base.h"
int32_t WiFiFlowControlTestInit(void);
int32_t WiFiFlowControlTestDeinit(void);
int32_t WiFiFlowControlTestSendData(void);
int32_t WiFiFlowControlTestGetEapolQueueId(void);
#endif