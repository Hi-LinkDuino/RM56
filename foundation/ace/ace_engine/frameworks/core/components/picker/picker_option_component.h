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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_OPTION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_OPTION_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/picker/picker_theme.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class PickerOptionComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(PickerOptionComponent, SoleChildComponent);

public:
    PickerOptionComponent();
    ~PickerOptionComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void Initialize();

    bool GetSelected() const
    {
        return selected_;
    }
    void SetSelected(bool value)
    {
        selected_ = value;
    }

    bool GetAutoLayout() const
    {
        return autoLayout_;
    }
    void SetAutoLayout(bool value)
    {
        autoLayout_ = value;
    }

    const std::string& GetText() const
    {
        return text_;
    }
    void SetText(const std::string& value)
    {
        text_ = value;
    }

    const RefPtr<PickerTheme>& GetTheme() const
    {
        return theme_;
    }
    void SetTheme(const RefPtr<PickerTheme>& value)
    {
        theme_ = value;
    }

    uint32_t GetIndex() const
    {
        return index_;
    }
    void SetIndex(uint32_t value)
    {
        index_ = value;
    }

    const RefPtr<TextComponent>& GetTextComponent()
    {
        return textComponent_;
    }

    const RefPtr<BoxComponent>& GetBoxComponent()
    {
        return boxComponent_;
    }

    bool GetAlignTop() const
    {
        return alignTop_;
    }
    void SetAlignTop(bool value)
    {
        alignTop_ = value;
    }

    bool GetAlignBottom() const
    {
        return alignBottom_;
    }
    void SetAlignBottom(bool value)
    {
        alignBottom_ = value;
    }

    bool GetDisappear() const
    {
        return disappear_;
    }
    void SetDisappear(bool value)
    {
        disappear_ = value;
    }

    const Dimension& GetFixHeight() const
    {
        return fixHeight_;
    }
    void SetFixHeight(const Dimension& value)
    {
        fixHeight_ = value;
    }

private:
    bool selected_ = false;
    bool autoLayout_ = false;
    bool alignTop_ = false;
    bool alignBottom_ = false;
    bool disappear_ = false;
    std::string text_;
    uint32_t index_ = 0;
    RefPtr<PickerTheme> theme_;
    Dimension fixHeight_;

    RefPtr<BoxComponent> boxComponent_;
    RefPtr<TextComponent> textComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_OPTION_COMPONENT_H
