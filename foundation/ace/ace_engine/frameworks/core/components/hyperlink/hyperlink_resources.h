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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_RESOURCE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_RESOURCE_H

#include <string>

#include "base/memory/ace_type.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

constexpr int64_t INVALID_ID = -1;

class HyperlinkResources : public virtual AceType {
    DECLARE_ACE_TYPE(HyperlinkResources, AceType);

public:
    explicit HyperlinkResources(const WeakPtr<PipelineContext>& context) : context_(context) {}
    ~HyperlinkResources() override;
    std::string CallResRegisterMethod(const std::string& method, const std::string& param);
    void StartAbility(const std::string& address);
    void CreatePlatformResource(const WeakPtr<PipelineContext>& context);

protected:
    std::string MakeResourceHash() const;
    std::string MakeMethodHash(const std::string& method) const;
    WeakPtr<PipelineContext> context_;

private:
    void CreatePluginResource(const WeakPtr<PipelineContext>& context);
    void ReleasePluginResource();
    int64_t id_ = INVALID_ID;
    std::string hash_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_RESOURCE_H
