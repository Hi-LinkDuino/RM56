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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_ITEM_H

#include "core/components_v2/indexer/popup_list_item_component.h"
#include "core/components/box/render_box.h"
#include "core/components/text/render_text.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {
class RenderPopupListItem : public RenderNode {
    DECLARE_ACE_TYPE(V2::RenderPopupListItem, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    RenderPopupListItem() = default;
    ~RenderPopupListItem() override = default;

    // override
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void UpdateBoxSelected();
    void UpdateBoxNormal();

private:
    RefPtr<PopupListItemComponent> component_;
    RefPtr<BoxComponent> boxComponent_;
    RefPtr<TextComponent> textComponent_;
    RefPtr<RenderBox> renderBox_;
    RefPtr<RenderText> renderText_;

    ACE_DISALLOW_COPY_AND_MOVE(RenderPopupListItem);
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_POPUP_LIST_ITEM_H
