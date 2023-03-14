/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_H
#define UI_TEST_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "common/screen.h"

namespace OHOS {
static constexpr uint16_t TITLE_LABEL_DEFAULT_HEIGHT = 29;
static constexpr uint16_t FONT_DEFAULT_SIZE = 20;
static constexpr uint16_t VIEW_DISTANCE_TO_LEFT_SIDE = 48;
static constexpr uint16_t VIEW_DISTANCE_TO_TOP_SIDE = 48;
static constexpr uint16_t VIEW_DISTANCE_TO_LEFT_SIDE2 = 24;
static constexpr uint16_t TEXT_DISTANCE_TO_LEFT_SIDE = 48;
static constexpr uint16_t TEXT_DISTANCE_TO_TOP_SIDE = 11;
static constexpr uint16_t HALF_OPA_OPAQUE = OPA_OPAQUE / 2;
static constexpr uint16_t VIEW_STYLE_BORDER_WIDTH = 2;
static constexpr uint16_t VIEW_STYLE_BORDER_RADIUS = 8;
static constexpr uint16_t BUTTON_LABEL_SIZE = 16;
static constexpr uint8_t BUTTON_STYLE_BORDER_RADIUS_VALUE = 20;
static constexpr uint32_t BUTTON_STYLE_BACKGROUND_COLOR_VALUE = 0xFF333333;
static constexpr uint32_t BUTTON_STYLE_BACKGROUND_COLOR_PRESS = 0xFF2D2D2D;
static constexpr int16_t BACK_BUTTON_HEIGHT = 64;
static constexpr uint16_t BUTTON_WIDHT1 = 80;
static constexpr uint16_t BUTTON_HEIGHT1 = 40;
static constexpr uint16_t BUTTON_WIDHT2 = 120;
static constexpr uint16_t BUTTON_HEIGHT2 = 40;
static constexpr uint16_t BUTTON_WIDHT3 = 150;
static constexpr uint16_t BUTTON_HEIGHT3 = 40;

class UITest : public HeapBase {
public:
    UITest() {}

    virtual ~UITest() {}

    /**
     * @brief Set up display environment.
     *
     */
    virtual void SetUp() = 0;

    /**
     * @brief Tear down display environment.
     *
     */
    virtual void TearDown()
    {
        positionX_ = 0;
        positionY_ = 0;
    }

    /**
     * @brief Get test view to add to root view.
     *
     * @returns test container view.
     *
     */
    virtual const UIView* GetTestView() = 0;

    void DeleteChildren(UIView* view)
    {
        if (view == nullptr) {
            return;
        }
        while (view != nullptr) {
            UIView* tempView = view;
            view = view->GetNextSibling();
            if (tempView->IsViewGroup() && (tempView->GetViewType() != UI_DIGITAL_CLOCK)) {
                DeleteChildren(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
            }
            if (tempView->GetParent()) {
                static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
            }
            delete tempView;
        }
    }

    UILabel* GetTitleLabel(const char* titleName)
    {
        if (titleName == nullptr) {
            return nullptr;
        }
        UILabel* label = new UILabel();
        if (label == nullptr) {
            return nullptr;
        }
        // 2: half of screen width
        label->SetPosition(0, 0, Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText(titleName);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        return label;
    }

protected:
    int16_t positionX_ = 0;
    int16_t positionY_ = 0;
};
} // namespace OHOS
#endif
