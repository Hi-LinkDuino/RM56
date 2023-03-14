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

#ifndef HAL_LIGHT_CONTROLLER_H
#define HAL_LIGHT_CONTROLLER_H

#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "light_if.h"
#include "light_type.h"
#include "osal_mem.h"
#include "osal_mutex.h"

enum LightIoCmd {
    LIGHT_IO_CMD_GET_INFO_LIST = 0,
    LIGHT_IO_CMD_OPS           = 1,
    LIGHT_IO_CMD_END,
};

enum LightOpsCmd {
    LIGHT_OPS_IO_CMD_ENABLE    = 0,
    LIGHT_OPS_IO_CMD_DISABLE   = 1,
    LIGHT_OPS_IO_CMD_END,
};

struct LightDevice {
    bool initState;
    uint32_t lightNum;
    struct LightInfo *lightInfoEntry;
    struct HdfIoService *ioService;
    struct OsalMutex mutex;
};

#endif /* HAL_LIGHT_CONTROLLER_H */