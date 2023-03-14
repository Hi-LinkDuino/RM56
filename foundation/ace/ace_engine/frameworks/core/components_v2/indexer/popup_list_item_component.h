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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_ITEM_COMPONENT_H

#include <string>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/components/box/box_component.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace::V2 {
inline constexpr double POPUP_BOX_SIZE = 56.0;
inline constexpr double POPUP_FONT_SIZE = 24.0;
inline constexpr double POPUP_ZERO_SIZE = 0.0;
inline constexpr uint32_t POPUP_BG_COLOR = 0xFFF1F3F5;
inline constexpr uint32_t POPUP_FONT_COLOR = 0xFF254FF7;
inline constexpr double POPUP_BG_OPACITY = 0.9;

class ACE_EXPORT PopupListItemComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(V2::PopupListItemComponent, ComponentGroup);

public:
    PopupListItemComponent() = default;
    explicit PopupListItemComponent(const std::string& label) {
        label_ = label;
        BuildItem();
    };
    ~PopupListItemComponent() = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

protected:
    void BuildItem();

private:
    RefPtr<BoxComponent> boxComponent_;
    RefPtr<Decoration> boxDecoration_;
    RefPtr<TextComponent> boxText_;
    TextStyle textStyle_;

    std::string label_ = "";

    ACE_DISALLOW_COPY_AND_MOVE(PopupListItemComponent);
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_POPUP_LIST_ITEM_COMPONENT_H
