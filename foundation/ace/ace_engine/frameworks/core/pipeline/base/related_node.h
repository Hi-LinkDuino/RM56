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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RELATED_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RELATED_NODE_H

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class RenderNode;
class RelatedContainer : public virtual AceType {
    DECLARE_ACE_TYPE(RelatedContainer, AceType);

public:
    virtual void OnRelatedStart() {}
    virtual void OnRelatedPreScroll(const Offset& delta, Offset& consumed) {}
    virtual void OnRelatedScroll(const Offset& delta, Offset& consumed) {}
    virtual void OnRelatedEnd() {}
};

class RelatedChild {
public:
    void InitRelatedParent(const WeakPtr<RenderNode>& weakParent);
    bool IsRelatedEventEnable()
    {
        return enableRelatedEvent_ && relatedParent_.Upgrade();
    };
    void RelatedEventStart();
    bool RelatedScrollEventPrepare(const Offset& delta);
    bool RelatedScrollEventDoing(const Offset& delta);
    void RelatedEventEnd();

protected:
    WeakPtr<RelatedContainer> relatedParent_;

private:
    bool enableRelatedEvent_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RELATED_NODE_H
