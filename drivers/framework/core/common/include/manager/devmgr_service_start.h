/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVMGR_SERVICE_START_H
#define DEVMGR_SERVICE_START_H

enum {
    DEV_MGR_SLOW_LOAD = 0,
    DEV_MGR_QUICK_LOAD,
};

int DeviceManagerStart(void);
int DeviceManagerStartStep2(void);
void DeviceManagerSetQuickLoad(int isQuickLoad);
int DeviceManagerIsQuickLoad(void);

#endif /* DEVMGR_SERVICE_START_H */
