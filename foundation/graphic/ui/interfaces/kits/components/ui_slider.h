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
 * @file ui_slider.h
 *
 * @brief Defines the attributes and common functions of a slider.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_SLIDER_H
#define GRAPHIC_LITE_UI_SLIDER_H

#include "common/image.h"
#include "components/ui_box_progress.h"

namespace OHOS {
/**
 * @brief Represents a slider.
 *
 * Users can drag or click the knob to adjust the progress of an event.
 *
 * @see UIBoxProgress
 * @since 1.0
 * @version 1.0
 */
class UISlider : public UIBoxProgress {
public:
    /**
     * @brief A constructor used to create a <b>UISlider</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UISlider();

    /**
     * @brief A destructor used to delete the <b>UISlider</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UISlider();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_SLIDER;
    }

    /**
     * @brief Sets the width for this knob.
     *
     * The width of this knob is the same as its height. \n
     * By default, the width of this knob is the same as the height of the horizontal slider
     * or the width of the vertical slider. \n
     *
     * @param width Indicates the knob width to set.
     * @see GetKnobWidth
     * @since 1.0
     * @version 1.0
     */
    void SetKnobWidth(int16_t width)
    {
        knobWidth_ = width;
    }

    /**
     * @brief Obtains the knob width.
     *
     * @return Returns the knob width.
     * @see SetKnobWidth
     * @since 1.0
     * @version 1.0
     */
    int16_t GetKnobWidth()
    {
        return knobWidth_;
    }

    /**
     * @brief Sets the image as pixel maps for this slider's knob.
     *
     * @param knobImage Indicates the knob image to set.
     * @since 6
     */
    void SetKnobImage(const ImageInfo* knobImage);

    /**
     * @brief Sets the image for this slider's knob.
     *
     * @param knobImage Indicates the knob image to set.
     * @since 6
     */
    void SetKnobImage(const char* knobImage);

    /**
     * @brief Sets the color for this slider's knob.
     *
     * @param knobColor Indicates the knob color to set.
     * @since 6
     */
    void SetKnobColor(const ColorType knobColor)
    {
        SetKnobStyle(STYLE_BACKGROUND_COLOR, knobColor.full);
    }

    /**
     * @brief Sets the corner radius for this slider's knob.
     *
     * @param knobRadius Indicates the knob corner radius to set.
     * @since 6
     */
    void SetKnobRadius(int16_t knobRadius)
    {
        SetKnobStyle(STYLE_BORDER_RADIUS, knobRadius);
    }

    /**
     * @brief Sets the knob style.
     *
     * @param style Indicates the knob style to set. For details, see {@link Style}.
     * @see GetKnobStyle
     * @since 1.0
     * @version 1.0
     */
    void SetKnobStyle(const Style& style);

    /**
     * @brief Sets a knob style.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @since 1.0
     * @version 1.0
     */
    void SetKnobStyle(uint8_t key, int64_t value);

    /**
     * @brief Obtains the knob style.
     *
     * @return Returns the knob style.
     * @since 1.0
     * @version 1.0
     */
    const Style& GetKnobStyle() const;

    /**
     * @brief Obtains the value of a knob style.
     *
     * @param key Indicates the key of the style.
     * @return Returns the value of the style.
     * @since 1.0
     * @version 1.0
     */
    int64_t GetKnobStyle(uint8_t key) const;

    /**
     * @brief Sets the images as pixel maps for this slider, including the background, foreground images.
     *
     * @param backgroundImage Indicates the background image to set.
     * @param foregroundImage Indicates the foreground image to set.
     * @since 1.0
     * @version 1.0
     */
    void SetImage(const ImageInfo* backgroundImage, const ImageInfo* foregroundImage);

    /**
     * @brief Sets the images for this slider, including the background, foreground images.
     *
     * @param backgroundImage Indicates the background image to set.
     * @param foregroundImage Indicates the foreground image to set.
     * @since 1.0
     * @version 1.0
     */
    void SetImage(const char* backgroundImage, const char* foregroundImage);

    /**
     * @brief Sets the colors for this slider, including the background, foreground colors.
     *
     * @param backgroundColor Indicates the background color to set.
     * @param foregroundColor Indicates the foreground color to set.
     * @since 1.0
     * @version 1.0
     */
    void SetSliderColor(const ColorType backgroundColor, const ColorType foregroundColor)
    {
        SetBackgroundStyle(STYLE_BACKGROUND_COLOR, backgroundColor.full);
        SetForegroundStyle(STYLE_BACKGROUND_COLOR, foregroundColor.full);
    }

    /**
     * @brief Sets the corner radiuses for this slider, including the background, foreground corner radiuses.
     *
     * @param backgroundRadius Indicates the background corner radius to set.
     * @param foregroundRadius Indicates the foreground corner radius to set.
     * @since 1.0
     * @version 1.0
     */
    void SetSliderRadius(int16_t backgroundRadius, int16_t foregroundRadius)
    {
        SetBackgroundStyle(STYLE_BORDER_RADIUS, backgroundRadius);
        SetForegroundStyle(STYLE_BORDER_RADIUS, foregroundRadius);
    }

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Obtains the rotation factor.
     *
     * @return Returns the rotation factor.
     * @since 5.0
     * @version 3.0
     */
    float GetRotateFactor()
    {
        return rotateFactor_;
    }

    /**
     * @brief Sets the rotation factor.
     *
     * @param factor Indicates the rotation factor to set.
     * @since 5.0
     * @version 3.0
     */
    void SetRotateFactor(float factor)
    {
        if (MATH_ABS(factor) > MAX_ROTATE_FACTOR) {
            rotateFactor_ = (factor > 0) ? MAX_ROTATE_FACTOR : -MAX_ROTATE_FACTOR;
            return;
        }
        rotateFactor_ = factor;
    }

    bool OnRotateEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif
    bool OnClickEvent(const ClickEvent& event) override;

    bool OnDragEvent(const DragEvent& event) override;

    bool OnDragEndEvent(const DragEvent& event) override;

    bool OnPreDraw(Rect& invalidatedArea) const override
    {
        return false;
    }

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Represents the listener for a slider change.
     *
     * This is an inner class of <b>UISlider</b> used to listen for slider events and invoke the callback function.
     *
     * @see UISlider
     * @since 1.0
     * @version 1.0
     */
    class UISliderEventListener : public HeapBase {
    public:
        /**
         * @brief A destructor used to delete the <b> UISliderEventListener </b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual ~UISliderEventListener() {}
        /**
         * @brief Called when the slider is dragged or clicked. This is a virtual function, which needs your
         *        implementation.
         *
         * @param value Indicates the current value of the slider.
         * @since 1.0
         * @version 1.0
         */
        virtual void OnChange(int32_t value) {}
        /**
         * @brief Called when the slider is released. This is a virtual function, which needs your implementation.
         *
         * @param value Indicates the current value of the slider.
         * @since 1.0
         * @version 1.0
         */
        virtual void OnRelease(int32_t value) {}
    };

    /**
     * @brief Sets the listener for a slider change.
     *
     * When a user drags or clicks the slider, listening is triggered and the <b>OnChange</b> callback is invoked.
     * When a user releases the slider, the <b>OnRelease</b> callback is invoked.
     *
     * @param listener Indicates the listener to set. For details, see {@link UISliderEventListener}.
     * @since 1.0
     * @version 1.0
     */
    void SetSliderEventListener(UISliderEventListener* listener)
    {
        listener_ = listener;
    }

protected:
    bool InitImage() override;

private:
    static constexpr uint8_t MAX_ROTATE_FACTOR = 128;

    int16_t knobWidth_;
    bool knobStyleAllocFlag_;
    Style* knobStyle_;
    Image* knobImage_;

    void DrawKnob(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, const Rect& foregroundRect);
    void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, Rect& coords);
    int32_t CalculateCurrentValue(int16_t length, int16_t totalLength);
    int32_t UpdateCurrentValue(const Point& knobPosition);
#if ENABLE_ROTATE_INPUT
    float rotateFactor_;
    float cachedRotation_;
#endif
    UISliderEventListener* listener_;
}; // class UISlider
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_SLIDER_H
