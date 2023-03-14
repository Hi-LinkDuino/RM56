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
 * @file ui_box_progress.h
 *
 * @brief Defines the attributes and common functions of a linear progress bar.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_BOX_PROGRESS_H
#define GRAPHIC_LITE_UI_BOX_PROGRESS_H

#include "components/ui_abstract_progress.h"

namespace OHOS {
/**
 * @brief Represents a linear progress bar.
 *
 * This class is used to set the range and current value to display the linear progress bar
 * which can be in multiple directions.
 *
 * @see UIAbstractProgress
 * @since 1.0
 * @version 1.0
 */
class UIBoxProgress : public UIAbstractProgress {
public:
    /**
     * @brief Enumerates the directions of the progress bar.
     */
    enum class Direction : uint8_t {
        /** Filling from left to right */
        DIR_LEFT_TO_RIGHT,
        /** Filling from right to left */
        DIR_RIGHT_TO_LEFT,
        /** Filling from top to bottom */
        DIR_TOP_TO_BOTTOM,
        /** Filling from bottom to top */
        DIR_BOTTOM_TO_TOP,
    };

    /**
     * @brief A constructor used to create a <b>UIBoxProgress</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIBoxProgress();

    /**
     * @brief A destructor used to delete the <b>UIBoxProgress</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIBoxProgress() {}

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_BOX_PROGRESS;
    }

    /**
     * @brief Sets the width for the view holding this progress bar.
     *
     * The width of the view must be greater than or equal to the actual width of the progress bar
     * to ensure a normal display. \n
     *
     * @param width Indicates the width of the view.
     * @see SetHeight | SetValidWidth | SetValidHeight
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override
    {
        UIView::SetWidth(width);
        if (!isValidWidthSet_) {
            progressWidth_ = width;
        }
    }

    /**
     * @brief Sets the height for this view.
     *
     * The height of the view must be greater than or equal to the actual height of the progress bar
     * to ensure a normal display. \n
     *
     * @param height Indicates the height to set.
     * @see SetWidth | SetValidWidth | SetValidHeight
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override
    {
        UIView::SetHeight(height);
        if (!isValidHeightSet_) {
            progressHeight_ = height;
        }
    }

    /**
     * @brief Sets the direction for this progress bar.
     *
     * @param direction Indicates the direction to set. The default direction is from left to right.
     * For details, see {@link Direction}.
     * @see GetDirection
     * @since 1.0
     * @version 1.0
     */
    void SetDirection(const Direction& direction)
    {
        direction_ = direction;
    }

    /**
     * @brief Obtains the direction of this progress bar.
     *
     * @return Returns the direction of this progress bar, as defined in {@link Direction}.
     * @see SetDirection
     * @since 1.0
     * @version 1.0
     */
    Direction GetDirection() const
    {
        return direction_;
    }

    /**
     * @brief Sets the actual width for this progress bar.
     *
     * The progress bar is centered in the view after the setting. By default, the width of the progress bar
     * is the same as that of the view. \n
     * If the width of the progress bar is greater than that of the view, the excess part cannot be displayed. \n
     *
     * @param width Indicates the actual width of this progress bar.
     * @see GetValidWidth
     * @since 1.0
     * @version 1.0
     */
    void SetValidWidth(int16_t width)
    {
        progressWidth_ = width;
        isValidWidthSet_ = true;
    }

    /**
     * @brief Obtains the actual width of this progress bar.
     *
     * @return Returns the actual width of this progress bar.
     * @see SetValidWidth
     * @since 1.0
     * @version 1.0
     */
    int16_t GetValidWidth() const
    {
        return progressWidth_;
    }

    /**
     * @brief Sets the actual height for this progress bar.
     *
     * The progress bar is centered in the view after the setting. By default, the height of the progress bar
     * is the same as that of the view. \n
     * If the height of the progress bar is greater than that of the view, the excess part cannot be displayed. \n
     *
     * @param height Indicates the actual height to set.
     * @see GetValidHeight
     * @since 1.0
     * @version 1.0
     */
    void SetValidHeight(int16_t height)
    {
        progressHeight_ = height;
        isValidHeightSet_ = true;
    }

    /**
     * @brief Obtains the actual height of this progress bar.
     *
     * @return Returns the actual height of this progress bar.
     * @see SetValidHeight
     * @since 1.0
     * @version 1.0
     */
    int16_t GetValidHeight() const
    {
        return progressHeight_;
    }

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

#ifdef OH_UI_PROGRESS_TOUCHABLE
    /**
     * @fn void OnDragEvent(const DragEvent& event) override
     *
     * @brief revice drag event, Switch to specified view when drag
     *
     * @param event The drag event
     */
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    bool OnClickEvent(const ClickEvent& event) override;
#endif

protected:
#ifdef OH_UI_PROGRESS_TOUCHABLE
    int16_t CalcValue(const int16_t x, const int16_t y, const char *tag);
#endif
    void GetBackgroundParam(Point& startPoint, int16_t& width, int16_t& height, uint16_t& radius, const Style& style);
    void DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, Rect& coords);
    void DrawRoundCap(BufferInfo& gfxDstBuffer,
                      const Image* image,
                      const Point& imgPos,
                      const Rect& rect,
                      const Rect& invalidatedArea,
                      uint16_t radius,
                      const Style& style);
    void DrawValidRect(BufferInfo& gfxDstBuffer,
                       const Image* image,
                       const Rect& rect,
                       const Rect& invalidatedArea,
                       const Style& style,
                       uint16_t radius);

    uint16_t progressWidth_;
    uint16_t progressHeight_;
    Direction direction_;
    bool isValidWidthSet_ : 1;
    bool isValidHeightSet_ : 1;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_BOX_PROGRESS_H
