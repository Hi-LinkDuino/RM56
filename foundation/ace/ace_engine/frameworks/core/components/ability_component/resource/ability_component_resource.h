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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_RESOURCE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_RESOURCE_H

#include <string>

#include "base/memory/ace_type.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

constexpr int64_t INVALID_ID = -1;
constexpr int64_t CREATING_ID = -2;

extern const char ABILITY_COMPONENT_PARAM_AND[];
extern const char ABILITY_COMPONENT_PARAM_EQUALS[];
extern const char ABILITY_COMPONENT_PARAM_BEGIN[];
extern const char ABILITY_COMPONENT_METHOD[];
extern const char ABILITY_COMPONENT_EVENT[];

class AbilityComponentResource : public virtual AceType {
    DECLARE_ACE_TYPE(AbilityComponentResource, AceType);

public:
    using Method = std::string;

    AbilityComponentResource(const std::string& type, const WeakPtr<PipelineContext>& context)
        : type_(type), context_(context)
    {}
    virtual ~AbilityComponentResource() = default;

    void Release(const std::function<void(bool)>& onRelease = nullptr);
    std::string CallResRegisterMethod(const Method& method, const std::string& param, bool useSyncTask);

    int64_t GetId() const
    {
        return id_;
    }

    const std::string& GetHashCode() const
    {
        return hash_;
    }

protected:
    std::string MakeResourceHash() const;
    std::string MakeEventHash(const std::string& event) const;
    std::string MakeMethodHash(const std::string& method) const;

    void Reset()
    {
        id_ = INVALID_ID;
        hash_.clear();
    }

    int64_t id_ = INVALID_ID;
    std::string hash_;
    std::string type_;
    WeakPtr<PipelineContext> context_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RESOURCE_ABILITY_COMPONENT_RESOURCE_H
