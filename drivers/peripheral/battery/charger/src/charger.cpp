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

#include "charger_thread.h"

#include "errors.h"
#include "battery_log.h"

int main(int argc, char** argv)
{
    using namespace OHOS::HDI::Battery;
    using namespace OHOS::HDI::Battery::V1_0;
    BATTERY_HILOGD(FEATURE_CHARGING, "starting battery charger");
    std::unique_ptr<ChargerThread> chargerThread = std::make_unique<ChargerThread>();
    if (chargerThread == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique ChargerThread return nullptr");
        return ERR_NO_MEMORY;
    }
    chargerThread->StartThread(nullptr);

    BATTERY_HILOGD(FEATURE_CHARGING, "battery charger thread started");
    return ERR_OK;
}
