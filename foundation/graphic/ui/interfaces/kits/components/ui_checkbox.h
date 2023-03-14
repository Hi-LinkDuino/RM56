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
 * @file ui_checkbox.h
 *
 * @brief Defines the attributes and common functions of a check box.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_CHECKBOX_H
#define GRAPHIC_LITE_UI_CHECKBOX_H

#include "animator/animator.h"
#include "animator/interpolation.h"
#include "common/image.h"
#include "components/ui_view.h"

namespace OHOS {
/**
 * @brief Represents a check box.
 *
 * A check box permits users to make a binary choice.
 *
 * @since 1.0
 * @version 1.0
 */
#if DEFAULT_ANIMATION
class UICheckBox : public UIView, public AnimatorCallback {
#else
class UICheckBox : public UIView {
#endif

public:
    /**
     * @brief Enumerates the states of a check box.
     *
     * @since 1.0
     * @version 1.0
     */
    enum UICheckBoxState : uint8_t {
        /* An enum constant representing the state selected option */
        SELECTED,
        /* An enum constant representing the state unselected option */
        UNSELECTED,
        /* Max num of state */
        MAX_STATUS_NUM,
    };

    /**
     * @brief A constructor used to create a <b>UICheckBox</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UICheckBox();

    /**
     * @brief A destructor used to delete the <b>UICheckBox</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UICheckBox() {}

    /**
     * @brief Represents a listener for changes of a check box.
     *
     * This is an inner class of <b>UICheckBox</b>. It contains a callback function to be invoked when the check box
     * state changes.
     *
     * @see UICheckBox
     * @since 1.0
     * @version 1.0
     */
    class OnChangeListener : public HeapBase {
    public:
        /**
         * @brief Called when the state of this check box is switched. This is a virtual function, which needs your
         * implementation.
         *
         * @param state Indicates the current state of this check box. For details, see {@link UICheckBoxState}.
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnChange(UICheckBoxState state) = 0;

        /**
         * @brief A destructor used to delete the <b>OnChangeListener</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnChangeListener() {}
    };

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_CHECK_BOX;
    }

    /**
     * @brief Sets the listener for this check box.
     *
     * The listener is triggered to invoke the callback function upon click events.
     *
     * @param listener Indicates the listener to set. For details, see {@link OnChangeListener}.
     * @since 1.0
     * @version 1.0
     */
    void SetOnChangeListener(OnChangeListener* onStateChangeListener)
    {
        onStateChangeListener_ = onStateChangeListener;
    }

    /**
     * @fn  virtual bool UICheckBox::OnPreDraw(Rect& invalidatedArea) override
     *
     * @brief Do something before draw, this function will be invoked mainly to check if this view need
     *        to cover invalidate area so render manager can decide which layer to draw firstly.
     * @param [in] invalidate area.
     * @returns True if need cover.
     */
    bool OnPreDraw(Rect& invalidatedArea) const override
    {
        return false;
    }

    /**
     * @fn  virtual void UICheckBox::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
     *
     * @brief   Executes the draw action
     *          Ondraw invokes the rendering function provided by the underlying layer to draw pictures
     *          based on the selected status of the checkbox.
     *
     * @param [in] invalidatedArea The invalidated area.
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @fn  virtual void UICheckBox::OnClickEvent(const ClickEvent& event) override;
     *
     * @brief   Executes the click event action
     *          OnClickEvent will reverse the selected state of checkbox.
     *          Example: If the check box is selected, the checkbox status is changed to
     *          Unselected after the click action is taken.
     *
     * @param [in] event   The event that passed when OnClickEvent is invoked.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     */
    bool OnClickEvent(const ClickEvent& event) override;

    /**
     * @brief Sets the images for this check box.
     *
     * @param selectedImageSrc Indicates the image for this check box when selected.
     * @param unselectedImageSrc Indicates the image for this check box when unselected.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetImages(const char* selectedImageSrc, const char* unselectedImageSrc);

    /**
     * @brief Sets the images for this check box.
     *
     * @param selectedImageSrc Indicates the image for this check box when selected.
     * @param unselectedImageSrc Indicates the image for this check box when unselected.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetImages(const ImageInfo* selectedImageSrc, const ImageInfo* unselectedImageSrc);

    /**
     * @brief Obtains the state of this check box.
     *
     * @return Returns the state of this check box, as defined in {@link UICheckBoxState}.
     * @since 1.0
     * @version 1.0
     */
    UICheckBoxState GetState() const
    {
        return state_;
    };

    /**
     * @brief Sets the state for this check box.
     *
     * @param state Indicates the state of this check box. For details, see {@link UICheckBoxState}.
     * @param needAnimater Whether the state change process can be animated, the setting takes effect when the
     *        DEFAULT_ANIMATION is 1, {@link DEFAULT_ANIMATION}
     * @since 1.0
     * @version 1.0
     */
    void SetState(UICheckBoxState state, bool needAnimater = false);

    /**
     * @brief Sets the selected state color for this check box.
     *
     * @param color Indicates the selected state color of this check box.
     *
     * @since 5.0
     * @version 3.0
     */
    void SetSelectedStateColor(ColorType color);

    /**
     * @brief Obtains the selected state color of this check box.
     *
     * @return Returns the selected state color of this check box
     * @since 5.0
     * @version 3.0
     */
    ColorType GetSelectedStateColor() const;

protected:
    void ReverseState();
    virtual void CalculateSize();
    void SelectedStateSoftwareDrawing(BufferInfo& gfxDstBuffer,
                                      Rect rect,
                                      Rect trunc,
                                      int16_t borderRadius,
                                      int16_t rectLineWidth);
    void UnSelectedStateSoftwareDrawing(BufferInfo& gfxDstBuffer,
                                        Rect rect,
                                        Rect trunc,
                                        int16_t borderRadius,
                                        int16_t rectLineWidth);
#if DEFAULT_ANIMATION
    virtual void ResetCallback();
    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;
#endif
    static constexpr int16_t DEFAULT_HOT_WIDTH = 46;
    static constexpr int16_t DEFAULT_HOT_HEIGHT = 46;
    static constexpr int16_t DEFAULT_BORDER_WIDTH = 22;

    UICheckBoxState state_;
    OnChangeListener* onStateChangeListener_;
    int16_t width_;
    int16_t height_;
    int16_t borderWidth_;
    Image image_[MAX_STATUS_NUM];
    uint8_t backgroundOpacity_;
#if DEFAULT_ANIMATION
    Animator checkBoxAnimator_;
    uint32_t runTime_;
#endif
    ColorType selectedStateColor_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_CHECKBOX_H
