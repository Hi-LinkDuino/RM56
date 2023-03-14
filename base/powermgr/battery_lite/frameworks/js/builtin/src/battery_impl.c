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

#include "battery_impl.h"
#include "battery_info.h"

int32_t GetBatSocImpl(void)
{
    return GetBatSoc();
}

BatteryChargeState GetChargingStatusImpl(void)
{
    return GetChargingStatus();
}

BatteryHealthState GetHealthStatusImpl(void)
{
    return GetHealthStatus();
}

BatteryPluggedType GetPluggedTypeImpl(void)
{
    return GetPluggedType();
}

int32_t GetBatVoltageImpl(void)
{
    return GetBatVoltage();
}

char *GetBatTechnologyImpl(void)
{
    return GetBatTechnology();
}

int32_t GetBatTemperatureImpl(void)
{
    return GetBatTemperature();
}
