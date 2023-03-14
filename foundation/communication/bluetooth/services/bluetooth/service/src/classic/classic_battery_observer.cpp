/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "classic_battery_observer.h"

#include "interface_adapter_classic.h"
#include "interface_adapter_manager.h"
#include "log.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
ClassicBatteryObserverHf::ClassicBatteryObserverHf(utility::Dispatcher &dispatcher) : dispatcher_(&dispatcher)
{
    LOG_DEBUG("[ClassicBatteryObserverHf]::%{public}s", __func__);
}

ClassicBatteryObserverHf::~ClassicBatteryObserverHf()
{
    LOG_DEBUG("[ClassicBatteryObserverHf]::%{public}s", __func__);
}

void ClassicBatteryObserverHf::OnBatteryLevelChanged(const RawAddress &device, int batteryLevel)
{
    HILOGI("address: %{public}s, batteryLevel: %{public}d", GET_ENCRYPT_ADDR(device), batteryLevel);
    dispatcher_->PostTask(std::bind(&ClassicBatteryObserverHf::SetBatteryLevel, this, device, batteryLevel));
}

void ClassicBatteryObserverHf::SetBatteryLevel(const RawAddress &device, int batteryLevel) const
{
    LOG_DEBUG("[ClassicBatteryObserverAg]::%{public}s", __func__);
    IAdapterClassic *classicAdapter = (IAdapterClassic *)IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BREDR);
    classicAdapter->SetDeviceBatteryLevel(device, batteryLevel);
}

ClassicBatteryObserverAg::ClassicBatteryObserverAg(utility::Dispatcher &dispatcher) : dispatcher_(&dispatcher)
{
    LOG_DEBUG("[ClassicBatteryObserverAg]::%{public}s", __func__);
}

ClassicBatteryObserverAg::~ClassicBatteryObserverAg()
{
    LOG_DEBUG("[ClassicBatteryObserverAg]::%{public}s", __func__);
}

void ClassicBatteryObserverAg::OnHfBatteryLevelChanged(const RawAddress &device, int indValue)
{
    HILOGI("address: %{public}s, batteryLevel: %{public}d", GET_ENCRYPT_ADDR(device), indValue);
    dispatcher_->PostTask(std::bind(&ClassicBatteryObserverAg::SetBatteryLevel, this, device, indValue));
}

void ClassicBatteryObserverAg::SetBatteryLevel(const RawAddress &device, int batteryLevel) const
{
    LOG_DEBUG("[ClassicBatteryObserverAg]::%{public}s", __func__);
    IAdapterClassic *classicAdapter = (IAdapterClassic *)IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BREDR);
    classicAdapter->SetDeviceBatteryLevel(device, batteryLevel);
}
}  // namespace bluetooth
}  // namespace OHOS