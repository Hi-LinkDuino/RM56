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
 * @file ui_picker.h
 *
 * @brief Defines the attributes and functions of the <b>UIPicker</b> class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_PICKER_H
#define UI_PICKER_H

#include "components/text_adapter.h"
#include "components/ui_list.h"

namespace OHOS {
class UIPicker;

class PickerListScrollListener : public ListScrollListener {
public:
    PickerListScrollListener(UIPicker* picker, UIList* list);
    virtual ~PickerListScrollListener() {}

    void OnItemSelected(int16_t index, UIView* view) override;

    void OnScrollEnd(int16_t index, UIView* view) override;

    void SetSelectView(UIView* view)
    {
        selectView_ = view;
        lastSelectView_ = view;
    }

    const UIView* GetSelectView() const
    {
        return selectView_;
    }

    void SetSelectIndex(uint16_t index)
    {
        selectIndex_ = index;
    }

    uint16_t GetSelectIndex() const
    {
        return selectIndex_;
    }

    void SetInitStatus(bool status)
    {
        isInitted_ = status;
    }

private:
    UIList* listView_;
    UIPicker* pickerView_;
    UIView* selectView_;
    UIView* lastSelectView_;
    uint16_t selectIndex_;
    bool isInitted_;
};

/**
 * @brief Defines a picker. Multiple texts or numbers can be put into a sliding list for selection.
 *        The selected text or numbers are highlighted.
 *
 * @since 1.0
 * @version 1.0
 */
class UIPicker : public UIViewGroup {
public:
    /**
     * @brief A constructor used to create a <b>UIPicker</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIPicker();

    /**
     * @brief A destructor used to delete the <b>UIPicker</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIPicker();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type. For details, see {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_PICKER;
    }

    bool OnPreDraw(Rect& invalidatedArea) const override
    {
        return false;
    }

    /**
     * @brief Sets dynamic text data in the picker by using a string array.
     *
     * @param value[] Indicates the array of text data.
     * @param count   Indicates the array size.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual bool SetValues(const char* value[], uint16_t count);

    /**
     * @brief Sets the numeric data in the picker by using a given numeric range.
     *
     * All integers in the range are automatically generated based on the start value and end value and placed in
     * the picker in sequence. The start value must be smaller or equal to the end value.
     *
     * @param start Indicates the start integer.
     * @param end   Indicates the end integer.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual bool SetValues(int16_t start, int16_t end);

    /**
     * @brief Clears all values in the picker.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void ClearValues();

    /**
     * @brief Sets the font IDs of dynamic text, which is the string array set through {@link SetValues}.
     *
     * @param backgroundFontId Indicates the font ID of the background text.
     * @param highlightFontId  Indicates the font ID of the highlighted text.
     * @since 1.0
     * @version 1.0
     */
    void SetFontId(uint8_t backgroundFontId, uint8_t highlightFontId);

    /**
     * @brief Sets the font name and size for the background text.
     *
     * @param name Indicates the pointer to the font name to set.
     * @param size Indicates the font size to set.
     * @since 1.0
     * @version 1.0
     */
    void SetBackgroundFont(const char* name, uint8_t size);

    /**
     * @brief Sets the font name and size for the highlighted text.
     *
     * @param name Indicates the pointer to the font name to set.
     * @param size Indicates the font size to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHighlightFont(const char* name, uint8_t size);

    /**
     * @brief Obtains the font ID of the background text.
     *
     * @return Returns the font ID.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetBackgroundFontId() const
    {
        return backgroundFontId_;
    }

    /**
     * @brief Obtains the font ID of the highlighted text.
     *
     * @return Returns the font ID.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetHighlightFontId() const
    {
        return highlightFontId_;
    }

    /**
     * @brief Sets the text color.
     *
     * @param backgroundColor Indicates the color of the background text.
     * @param highlightColor  Indicates the color of the highlighted text.
     * @since 1.0
     * @version 1.0
     */
    void SetTextColor(ColorType backgroundColor, ColorType highlightColor);

    /**
     * @brief Obtains the color of the background text.
     *
     * @return Returns the color.
     * @since 1.0
     * @version 1.0
     */
    ColorType GetBackgroundTextColor() const
    {
        return backgroundColor_;
    }

    /**
     * @brief Obtains the color of the highlighted text.
     *
     * @return Returns the color of the highlighted text.
     * @since 1.0
     * @version 1.0
     */
    ColorType GetHighlightTextColor() const
    {
        return highlightColor_;
    }

    /**
     * @brief Sets the index of the item currently selected in the picker.
     *
     * @param index Indicates the index to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetSelected(uint16_t index);

    /**
     * @brief Obtains the index of the item currently selected in the picker.
     *
     * @return Returns the index.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetSelected() const;

    /**
     * @brief Sets the height of each item in the picker.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetItemHeight(int16_t height);

    /**
     * @brief Sets the width for this component.
     *
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief Sets the height for this component.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

    /**
     * @brief Sets whether a picker can slide cyclically.
     *
     * @param state Specifies whether the picker can slide cyclically. Value <b>true</b> indicates that the picker
     *              can slide cyclically, and value <b>false</b> indicates that the picker cannot
     *              slide cyclically. The default value is <b>false</b>.
     * @since 1.0
     * @version 1.0
     */
    void SetLoopState(bool state);

    /**
     * @brief Defines the listener used by a picker. This listener is triggered when an item
     *        is selected after sliding stops.
     *
     * @since 1.0
     * @version 1.0
     */
    class SelectedListener : public HeapBase {
    public:
        /**
         * @brief A constructor used to create a <b>SelectedListener</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        SelectedListener() {}

        /**
         * @brief A destructor used to delete the <b>SelectedListener</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual ~SelectedListener() {}

        /**
         * @brief Called when an item is selected after sliding stops. This function is implemented by applications.
         *
         * @param picker Indicates the picker instance.
         * @since 1.0
         * @version 1.0
         */
        virtual void OnPickerStoped(UIPicker& picker) {}
    };

    /**
     * @brief Registers a listener for a selected event.
     *
     * @param pickerListener Indicates the listener for a selected event in the picker. For details,
     *                       see {@link SelectedListener}.
     *
     * @since 1.0
     * @version 1.0
     */
    void RegisterSelectedListener(SelectedListener* pickerListener)
    {
        pickerListener_ = pickerListener;
    }

    /**
     * @brief Sets the text direction.
     *
     * @param direct Indicates the text direction to set. For details, see {@link UITextLanguageDirect}.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetDirect(UITextLanguageDirect direct);

    /**
     * @brief Sets the text formatter.
     *
     * @param formatter Indicates the pointer to the text formatter. For details, see {@link TextFormatter}.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetTextFormatter(TextFormatter* formatter);

    /**
     * @brief Sets the easing function that specifies a scroll animation after a finger lifts the screen.
     *
     * @param func Indicates the easing function to set. The default function is {@link EasingEquation::CubicEaseOut}.
     *             For details, see {@link EasingEquation}.
     * @since 5.0
     * @version 3.0
     */
    void SetDragFunc(EasingFunc func)
    {
        list_.SetDragFunc(func);
    }

    /**
     * @brief Sets the rebound size, which is the distance a knob moves after being released when it reaches the end of
     *        a scrollbar.
     *
     * @param size Indicates the rebound size to set.
     * @since 5.0
     * @version 3.0
     */
    void SetReboundSize(uint16_t size)
    {
        list_.SetReboundSize(size);
    }

    /**
     * @brief 设置自动对齐动画时长，单位为毫秒，默认为100毫秒。该功能依赖EnableAutoAlign()方法，自动对齐设置为true情况下才生效。
     *
     * @param value 自动对齐动画时长,0表示无动画。
     * @since 5.0
     * @version 3.0
     */
    void SetAutoAlignTime(uint16_t time)
    {
        list_.SetAutoAlignTime(time);
    }

    /**
     * @brief Sets the drag acceleration.
     *
     * @param value Indicates the drag acceleration to set. The default value is <b>10</b>. A larger drag acceleration
     *              indicates a higher inertial scroll velocity.
     * @since 5.0
     * @version 3.0
     */
    void SetDragACCLevel(uint16_t value)
    {
        list_.SetDragACCLevel(value);
    }

    /**
     * @brief Sets the compensation distance after a finger lifts the screen.
     *
     * @param value Indicates the compensation distance to set. The default value is <b>0</b>.
     * @since 5.0
     * @version 3.0
     */
    void SetSwipeACCLevel(uint16_t value)
    {
        list_.SetSwipeACCLevel(value);
    }

    /**
     * @brief Sets the blank size for this scroll view.
     *
     *
     * @param value Indicates the blank size to set. The default value is <b>0</b>. Taking a vertical scroll as an
     *              example, the value <b>0</b> indicates that the head node can only scroll downwards the top of the
     *              view and the tail node scroll upwards the bottom; the value <b>10</b> indicates that the head node
     *              can continue scrolling down by 10 pixels after it reaches the top of the view.
     * @since 5.0
     * @version 3.0
     */
    void SetScrollBlankSize(uint16_t size)
    {
        scrollBlankSize_ = size;
        isScrollBlankSizeSet_ = true;
        Refresh();
    }

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Requests the focus on the view.
     *
     * @since 5.0
     * @version 3.0
     */
    void RequestFocus() override
    {
        list_.RequestFocus();
    }

    /**
     * @brief Clears the focus on the view.
     *
     * @since 5.0
     * @version 3.0
     */
    void ClearFocus() override
    {
        list_.ClearFocus();
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
        list_.SetRotateFactor(factor);
    }

    /**
     * @brief Obtains the rotation factor.
     *
     * @return Returns the rotation factor.
     * @since 5.0
     * @version 3.0
     */
    float GetRotateFactor()
    {
        return list_.GetRotateFactor();
    }

    /**
     * @brief 设置触发惯性滑动的组件大小比例阈值.
     *
     * @param threshold 设置触发惯性滑动的比例阈值.
     *                  旋转表冠结束，如果最后一次旋转位移数值大于组件的宽或高除以threshold，则触发惯性滑动.
     * @since 6
     */
    void SetRotateThrowThreshold(uint8_t threshold)
    {
        list_.SetRotateThrowThreshold(threshold);
    }
#endif

protected:
    bool RefreshSelected(uint16_t index);
    void RefreshList();
    virtual void ClearList();
    virtual void Refresh();
    virtual void InitTextAdapter()
    {
        if (textAdapter_ == nullptr) {
            textAdapter_ = new TextAdapter();
            if (textAdapter_ == nullptr) {
                GRAPHIC_LOGE("new TextAdapter fail");
                return;
            }
        }
    }
    virtual void ClearTextAdapter();

    bool isWidthSet_ : 1;
    bool isHeightSet_ : 1;
    TextAdapter* textAdapter_;
    uint16_t maxCount_;
    PickerListScrollListener* listListener_;

private:
    friend class PickerListScrollListener;
    bool RefreshValues(const char* value[], uint16_t count);
    bool RefreshValues(int16_t start, int16_t end);

    bool isScrollBlankSizeSet_ : 1;
    uint16_t scrollBlankSize_;

    uint16_t backgroundFontId_;
    uint16_t highlightFontId_;
    uint8_t backgroundFontSize_;
    uint8_t highlightFontSize_;
    char* backgroundFontName_;
    char* highlightFontName_;

    uint16_t itemsWidth_;
    uint16_t itemsHeight_;
    const char** rangeValue_;
    uint16_t rangeValueCount_;
    int16_t startValue_;
    int16_t endValue_;
    ColorType backgroundColor_;
    ColorType highlightColor_;
    List<const char*> dataList_;
    bool isSetAdaptered_ : 1;
    UIList list_;

    SelectedListener* pickerListener_;
    UITextLanguageDirect direct_;
};
} // namespace OHOS
#endif
