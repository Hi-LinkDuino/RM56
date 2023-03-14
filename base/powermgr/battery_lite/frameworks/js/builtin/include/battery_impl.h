/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_IMPL_H
#define BATTERY_IMPL_H

#include <stdint.h>
#include "battery_info.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t GetBatSocImpl(void);
BatteryChargeState GetChargingStatusImpl(void);
BatteryHealthState GetHealthStatusImpl(void);
BatteryPluggedType GetPluggedTypeImpl(void);
int32_t GetBatVoltageImpl(void);
char *GetBatTechnologyImpl(void);
int32_t GetBatTemperatureImpl(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */
#endif /* BATTERY_IMPL_H */
