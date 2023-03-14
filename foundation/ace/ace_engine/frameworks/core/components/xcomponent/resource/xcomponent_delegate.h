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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_XCOMPONENT_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_XCOMPONENT_DELEGATE_H

#include <list>

#include "core/components/common/layout/constants.h"
#include "core/components/xcomponent/resource/xcomponent_resource.h"
#include "core/components/xcomponent/xcomponent_component.h"

namespace OHOS::Ace {
class XComponentDelegate : public XComponentResource {
    DECLARE_ACE_TYPE(XComponentDelegate, XComponentResource);

public:
    using CreatedCallback = std::function<void()>;
    using ReleasedCallback = std::function<void(bool)>;
    using EventCallback = std::function<void()>;
    enum class State: char {
        WAITINGFORSIZE,
        CREATING,
        CREATED,
        CREATEFAILED,
        RELEASED,
    };

    XComponentDelegate() = delete;
    ~XComponentDelegate() override;
    XComponentDelegate(const WeakPtr<XComponentComponent>& xcomponentComponent,
        const WeakPtr<PipelineContext>& context, ErrorCallback&& onError, const std::string& type)
        : XComponentResource(type, context, std::move(onError)),
        xcomponentComponent_(xcomponentComponent), state_(State::WAITINGFORSIZE) {
        ACE_DCHECK(!type.empty());
    }

    void CreatePlatformResource(const Size& size, const Offset& position,
        const WeakPtr<PipelineContext>& context);
    void CreatePluginResource(const Size& size, const Offset& position,
        const WeakPtr<PipelineContext>& context);
    void AddCreatedCallback(const CreatedCallback& createdCallback);
    void RemoveCreatedCallback();
    void AddReleasedCallback(const ReleasedCallback& releasedCallback);
    void RemoveReleasedCallback();

    WeakPtr<XComponentComponent> GetXComponent()
    {
        return xcomponentComponent_;
    }

private:
    void ReleasePlatformResource();
    WeakPtr<XComponentComponent> xcomponentComponent_;
    std::list<CreatedCallback> createdCallbacks_;
    std::list<ReleasedCallback> releasedCallbacks_;
    State state_ {State::WAITINGFORSIZE};
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_XCOMPONENT_DELEGATE_H
