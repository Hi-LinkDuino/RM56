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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_RENDER_SEARCH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_RENDER_SEARCH_H

#include <functional>

#include "core/components/box/render_box.h"
#include "core/components/button/render_button.h"
#include "core/components/image/render_image.h"
#include "core/components/padding/render_padding.h"
#include "core/components/search/search_component.h"
#include "core/components/text_field/render_text_field.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class SearchNodeType {
    NONE = 0,
    IMAGE,
    BUTTON,
    SEARCH,
};

class RenderSearch : public RenderNode, public ValueChangeObserver {
    DECLARE_ACE_TYPE(RenderSearch, RenderNode, ValueChangeObserver);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnValueChanged(bool needFireChangeEvent = true, bool needFireSelectChangeEvent = true) override;
    void FireSubmitEvent(const std::string& searchKey);

    bool HandleEnterEvent();
    bool HandleFocusEvent(bool vertical, bool reverse);

    const RefPtr<SearchComponent>&  GetSearchComponent() const
    {
        return searchComponent_;
    }

protected:
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    bool MouseHoverTest(const Point& parentLocalPoint) override;

    Dimension closeIconSize_;
    Dimension closeIconHotZoneHorizontal_;
    bool showCloseIcon_ = false;
    bool needReverse_ = false;
    std::string searchText_;
    std::function<void(const std::string&)> changeEvent_;
    std::function<void(const std::string&)> submitEvent_;
    RefPtr<RenderImage> renderCloseIcon_;
    RefPtr<RenderBox> renderSearchBox_;
    RefPtr<TextEditController> textEditController_;
    RefPtr<Decoration> decoration_;
    RefPtr<SearchComponent> searchComponent_;
    // Overlay color for hover and press.
    Color overlayColor_ = Color::TRANSPARENT;

    Rect closeIconRect_;
    Rect closeIconHotZoneRect_;
    Rect searchTextRect_;
    Offset searchBoxOffset_;
    SearchNodeType focusRender_ = SearchNodeType::NONE;
    SearchNodeType hoverOrPressRender_ = SearchNodeType::NONE;

private:
    void CreateRenderImage(const RefPtr<SearchComponent>& searchComponent);
    void CreateRenderButton(const RefPtr<SearchComponent>& searchComponent);

    void InitRect(const RefPtr<RenderTextField>& renderTextField);
    void CancelTextFieldHover();
    void HandleClick();

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RawRecognizer> rawRecognizer_;

    Color hoverColor_;
    Color pressColor_;

    TextStyle placeHoldStyle_;
    TextStyle editingStyle_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_RENDER_SEARCH_H
