/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

int32_t DispInit(uint32_t devId);
int32_t DispOn(uint32_t devId);
int32_t DispOff(uint32_t devId);
int32_t DispSetBacklight(uint32_t devId, uint32_t level);
int32_t DispGetInfo(uint32_t devId, struct DispInfo *info);
void *DispMmap(uint32_t size);
void DispFlush(void);

#endif /* DISP_HAL_H */
