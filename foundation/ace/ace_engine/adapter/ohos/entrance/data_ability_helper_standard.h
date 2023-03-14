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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_DATA_ABILITY_HELPER_STANDARD_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_DATA_ABILITY_HELPER_STANDARD_H

#include "base/resource/data_ability_helper.h"

namespace OHOS::AppExecFwk {
    class DataAbilityHelper;
    class Context;
}

namespace OHOS::AbilityRuntime {
    class Context;
}

namespace OHOS {
    class Uri;
}

namespace OHOS::Ace {

class DataAbilityHelperStandard : public DataAbilityHelper {
    DECLARE_ACE_TYPE(DataAbilityHelperStandard, DataAbilityHelper)

public:
    DataAbilityHelperStandard(const std::shared_ptr<OHOS::AppExecFwk::Context>& context,
        const std::shared_ptr<OHOS::AbilityRuntime::Context>& runtimeContext, bool useStageModel);
    ~DataAbilityHelperStandard() override = default;

    int32_t OpenFile(const std::string& uriStr, const std::string& mode) override;

private:
    bool useStageModel_ = false;
    std::shared_ptr<AppExecFwk::DataAbilityHelper> dataAbilityHelper_;
    std::weak_ptr<OHOS::AbilityRuntime::Context> runtimeContext_;
    std::shared_ptr<OHOS::Uri> uri_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_DATA_ABILITY_HELPER_STANDARD_H
