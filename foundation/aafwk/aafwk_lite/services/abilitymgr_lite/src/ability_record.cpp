/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_record.h"

#include "adapter.h"
#include "securec.h"
#include "utils.h"

namespace OHOS {
AbilityRecord::AbilityRecord()
    : abilityInfo_()
{
}

AbilityRecord::~AbilityRecord()
{
    AdapterFree(appName_);
    AdapterFree(appPath_);
    AdapterFree(appData_);
}

void AbilityRecord::SetAppName(const char *bundleName)
{
    AdapterFree(appName_);
    appName_ = Utils::Strdup(bundleName);
}

void AbilityRecord::SetAppPath(const char *appPath)
{
    AdapterFree(appPath_);
    appPath_ = Utils::Strdup(appPath);
}

void AbilityRecord::SetAppData(const void *appData, uint16_t dataLength)
{
    AdapterFree(appData_);
    appData_ = Utils::Memdup(appData, dataLength);
    if (appData == nullptr) {
        dataLength_ = 0;
        return;
    }
    dataLength_ = dataLength;
}

void AbilityRecord::CopyAbilityRecord(AbilityRecord &abilityRecord, AbilityRecord &newAbilityRecord)
{
    newAbilityRecord.SetAppName(abilityRecord.GetAppName());
    newAbilityRecord.SetAppPath(abilityRecord.GetAppPath());
    newAbilityRecord.SetAppData(abilityRecord.GetAppData(), abilityRecord.GetDataLength());
    newAbilityRecord.SetState(abilityRecord.GetState());
    newAbilityRecord.SetToken(abilityRecord.GetToken());
    newAbilityRecord.SetTaskId(abilityRecord.GetTaskId());
}
} // namespace OHOS
