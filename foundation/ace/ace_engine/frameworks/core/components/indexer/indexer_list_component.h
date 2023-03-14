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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_LIST_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_LIST_COMPONENT_H

#include "core/components/indexer/indexer_component.h"
#include "core/components/list/list_component.h"
#include "core/components/stack/stack_component.h"

namespace OHOS::Ace {

class IndexerListComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(IndexerListComponent, SoleChildComponent);

public:
    IndexerListComponent(const RefPtr<ListComponent>& list, bool circleMode = false, bool isRightToLeft = false,
        bool bubble = true, bool multiLanguage = false)
    {
        if (list) {
            list_ = list;
            SetRightToLeft(isRightToLeft);
            indexer_ = AceType::MakeRefPtr<IndexerComponent>(list_, circleMode, bubble, multiLanguage);
            indexer_->SetController(list_->GetPositionController());
            BuildChildren();
            LOGI("[indexer] create ListComponent circle:%{public}d, RTL:%{public}d.", circleMode, isRightToLeft);
        } else {
            LOGE("[indexer] list is nullptr, create indexer failed.");
        }
    }

    IndexerListComponent(const RefPtr<ListComponent>& list, const std::vector<std::string>& label,
        bool circleMode = false, bool isRightToLeft = false, bool bubble = true, bool multiLanguage = false)
    {
        if (list) {
            list_ = list;
            SetRightToLeft(isRightToLeft);
            indexer_ = AceType::MakeRefPtr<IndexerComponent>(list_, circleMode, label, bubble, multiLanguage);
            indexer_->SetController(list_->GetPositionController());
            BuildChildren();
            LOGI("[indexer] create ListComponent circle:%{public}d, RTL:%{public}d.", circleMode, isRightToLeft);
        } else {
            LOGE("[indexer] list is nullptr, create indexer failed.");
        }
    }

    ~IndexerListComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void AppendChild(const RefPtr<Component>& child);
    void InsertChild(uint32_t position, const RefPtr<Component>& child);
    void RemoveChild(const RefPtr<Component>& child);

    void BuildChildren();

    void SetIndexerChangeEvent(const EventMarker& indexChange)
    {
        if (indexer_) {
            indexer_->SetIndexerChange(indexChange);
        }
    }

    const RefPtr<IndexerComponent>& GetIndexer() const
    {
        return indexer_;
    }

    int32_t GetItemCountInList() const
    {
        if (!list_) {
            return 0;
        }
        return list_->GetChildren().size();
    }

    void SetRightToLeft(bool isRightToLeft)
    {
        LOGD("[indexer] SetRightToLeft: %d", isRightToLeft);
        isRightToLeft_ = isRightToLeft;
    }

    void SetBubbleEnabled(bool enable)
    {
        indexer_->SetBubbleEnabled(enable);
    }

private:
    RefPtr<ListComponent> list_;
    RefPtr<IndexerComponent> indexer_;
    RefPtr<StackComponent> stack_;
    bool isRightToLeft_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_INDEXER_LIST_COMPONENT_H
