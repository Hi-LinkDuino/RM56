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

#include "adapter/ohos/entrance/data_ability_helper_standard.h"

#include "data_ability_helper.h"

namespace OHOS::Ace {

DataAbilityHelperStandard::DataAbilityHelperStandard(const std::shared_ptr<OHOS::AppExecFwk::Context>& context,
    const std::shared_ptr<OHOS::AbilityRuntime::Context>& runtimeContext, bool useStageModel)
{
    useStageModel_ = useStageModel;
    if (useStageModel) {
        runtimeContext_ = runtimeContext;
    } else {
        dataAbilityHelper_ = AppExecFwk::DataAbilityHelper::Creator(context);
    }
}

int32_t DataAbilityHelperStandard::OpenFile(const std::string& uriStr, const std::string& mode)
{
    LOGD("DataAbilityHelperStandard::OpenFile start uri: %{private}s, mode: %{private}s", uriStr.c_str(), mode.c_str());
    Uri uri = Uri(uriStr);
    if (useStageModel_ && !dataAbilityHelper_) {
        uri_ = std::make_shared<Uri>(uriStr);
        dataAbilityHelper_ = AppExecFwk::DataAbilityHelper::Creator(runtimeContext_.lock(), uri_, false);
    }

    if (dataAbilityHelper_) {
        return dataAbilityHelper_->OpenFile(uri, mode);
    }
    LOGE("DataAbilityHelperStandard::OpenFile fail, data ability helper is not exist.");
    return -1;
}

} // namespace OHOS::Ace
