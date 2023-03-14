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

#ifndef HAL_VIBRATOR_CONTROLLER_H
#define HAL_VIBRATOR_CONTROLLER_H

#include "hdf_io_service_if.h"
#include "osal_mutex.h"

enum VibratorIoCmd {
    VIBRATOR_IO_START_ONCE    = 0,
    VIBRATOR_IO_START_EFFECT  = 1,
    VIBRATOR_IO_STOP          = 2,
    VIBRATOR_IO_END,
};

struct VibratorDevice {
    bool initState;
    struct HdfIoService *ioService;
    struct OsalMutex mutex;
};

#endif /* HAL_VIBRATOR_CONTROLLER_H */