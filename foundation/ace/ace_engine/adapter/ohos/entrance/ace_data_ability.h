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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_DATA_ABILITY_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_DATA_ABILITY_H

#include <string>
#include <vector>

#include "ability.h"
#include "ability_loader.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS::Ace {

class AceDataAbility final : public OHOS::AppExecFwk::Ability {
public:
    AceDataAbility()
    {
        abilityId_ = instanceId_;
        instanceId_++;
    }
    virtual ~AceDataAbility() = default;

    void OnStart(const OHOS::AAFwk::Want& want) override;
    void OnStop() override;

    int32_t Insert(const Uri& uri, const NativeRdb::ValuesBucket& value) override;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> Query(
        const Uri& uri, const std::vector<std::string>& columns,
        const NativeRdb::DataAbilityPredicates& predicates) override;
    int32_t Update(const Uri& uri, const NativeRdb::ValuesBucket& value,
        const NativeRdb::DataAbilityPredicates& predicates) override;
    int32_t Delete(const Uri& uri, const NativeRdb::DataAbilityPredicates& predicates) override;

    int32_t BatchInsert(const Uri& uri, const std::vector<NativeRdb::ValuesBucket>& values) override;
    std::string GetType(const Uri& uri) override;
    std::vector<std::string> GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter) override;
    int32_t OpenFile(const Uri& uri, const std::string& mode) override;
    int32_t OpenRawFile(const Uri& uri, const std::string& mode) override;
    Uri NormalizeUri(const Uri& uri) override;
    Uri DenormalizeUri(const Uri& uri) override;

private:
    int32_t abilityId_ = 200000;

    static int32_t instanceId_;
    static const std::string START_PARAMS_KEY;
    static const std::string URI;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_DATA_ABILITY_H
