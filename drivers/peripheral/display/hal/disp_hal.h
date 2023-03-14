/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef DISP_HAL_H
#define DISP_HAL_H
#include "disp_common.h"
#include "display_type.h"

#define DISP_SERVICE_NAME    "hdf_disp"

enum DispCmd {
    DISP_CMD_GET_PANELINFO = 1,
    DISP_CMD_SET_POWERSTATUS,
    DISP_CMD_SET_BACKLIGHT,
    DISP_CMD_GET_POWERSTATUS,
    DISP_CMD_GET_BACKLIGHT,
};

struct DispPara {
    uint32_t devId;
    void *par;
    uint32_t size;
};

typedef struct {
    int32_t (*SetPowerStatus)(uint32_t devId, DispPowerStatus pStatus);
    int32_t (*GetPowerStatus)(uint32_t devId, DispPowerStatus *pStatus);
    int32_t (*SetBacklight)(uint32_t devId, uint32_t level);
    int32_t (*GetBacklight)(uint32_t devId, uint32_t *level);
    int32_t (*GetInfo)(uint32_t devId, struct DispInfo *info);
} HalFuncs;

HalFuncs *GetHalFuncs(void);
#endif // DISP_HAL_H
