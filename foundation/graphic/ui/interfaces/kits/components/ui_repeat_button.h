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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_repeate_button.h
 *
 * @brief Defines the attributes and common functions of a repeat button.
 *
 * If a repeat button is clicked and hold, the click event is triggered continuously.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_REPEAT_BUTTON_H
#define GRAPHIC_LITE_UI_REPEAT_BUTTON_H

#include "components/ui_button.h"

namespace OHOS {
/**
 * @brief Represents a repeat button.
 *
 * If a repeat button is clicked and hold, the click event is triggered continuously.
 *
 * @see UIButton
 * @since 1.0
 * @version 1.0 */
class UIRepeatButton : public UIButton {
public:
    /**
     * @brief A constructor used to create a <b>UIRepeatButton</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIRepeatButton();

    /**
     * @brief A destructor used to delete the <b>UIRepeatButton</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIRepeatButton();

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_REPEAT_BUTTON;
    }

    /**
     * @brief Sets the interval between two consecutive click events.
     *
     * @param interval Indicates the interval to set.

     * @since 1.0
     * @version 1.0
     */
    virtual void SetInterval(uint16_t interval)
    {
        ticksInterval_ = interval;
    }

    /**
     * @brief Obtains the interval between two consecutive click events.
     *
     * @return Returns the interval between two consecutive click events.
     * @since 1.0
     * @version 1.0
     */
    virtual uint16_t GetInterval() const
    {
        return ticksInterval_;
    }

    /**
     * @fn  void UIRepeatButton::OnClickEvent(const ClickEvent& event)
     *
     * @brief   The action of click event.
     *
     * @param [in]  event   click event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     */
    bool OnClickEvent(const ClickEvent& event) override;

    /**
     * @fn  void UIRepeatButton::OnReleaseEvent(const ReleaseEvent& event)
     *
     * @brief   The action of release event.
     *
     * @param [in]  event   release event.
     */
    bool OnReleaseEvent(const ReleaseEvent& event) override;

    /**
     * @fn  void UIRepeatButton::OnLongPressEvent(const ReleaseEvent& event)
     *
     * @brief   The action of long press event.
     *
     * @param [in]  event   long press event.
     */
    bool OnLongPressEvent(const LongPressEvent& event) override;

    /**
     * @fn  void UIRepeatButton::HandleTickEvent()
     *
     * @brief   handle the event of long pressing.
     */
    void HandleTickEvent();

private:
    uint16_t ticksInterval_;
    ClickEvent event_;
    bool longPressed_;
    static UIRepeatButton* repeatButton_;
    uint32_t timerRepeatID_;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    inline void SetEvent(const ClickEvent& event);
};
} // namespace OHOS

#endif // GRAPHIC_LITE_UI_REPEAT_BUTTON_H
