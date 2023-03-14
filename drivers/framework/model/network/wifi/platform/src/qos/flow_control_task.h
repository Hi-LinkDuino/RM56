/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WIFI_FLOW_CONTROL_TASK_H
#define WIFI_FLOW_CONTROL_TASK_H
#include "flow_control.h"

#define MAX_EXIT_THREAD_COUNT 10 /* 10MS */
int32_t CreateFlowControlTask(struct FlowControlModule *fcm);
void DestroyFlowControlTask(struct FlowControlModule *fcm);

#define TX_THREAD_NAME "hdf_wifi_tx"
#define RX_THREAD_NAME "hdf_wifi_rx"
#endif /* WIFI_FLOW_CONTROL_TASK_H */