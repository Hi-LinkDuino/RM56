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
 * @file ui_dialog.h
 *
 * @brief Declares the UIDialog class that defines the dialog box component.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_DIALOG_H
#define GRAPHIC_LITE_UI_DIALOG_H

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

#if ENABLE_WINDOW
namespace OHOS {
/**
 * @brief Represents a dialog box.
 *
 * A dialog box displays information for user input.
 * After a button is clicked in the dialog box, the corresponding callback event is triggered.
 *
 * @since 1.0
 * @version 1.0
 */
class UIDialogClickListener;
class UIDialog : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>UIDialog</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIDialog();

    /**
     * @brief A destructor used to delete the <b>UIDialog</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIDialog();

    /**
     * @brief Enumerates buttons in a dialog box.
     *
     * @since 1.0
     * @version 1.0
     */
    enum class DialogButtonType {
        /** Left button */
        BUTTON_LEFT,
        /** Middle button */
        BUTTON_MID,
        /** Right button */
        BUTTON_RIGHT,
    };

    /**
     * @brief Sets the title for this dialog box.
     *
     * @param title Indicates the pointer to the title.
     * @since 1.0
     * @version 1.0
     */
    void SetTitle(const char* title);

    /**
     * @brief Sets the text for this dialog box.
     *
     * @param text Indicates the pointer to the text.
     * @since 1.0
     * @version 1.0
     */
    void SetText(const char* text);

    /**
     * @brief Sets a button for this dialog box.
     *
     * @param buttonType Indicates the button position.
     * @param text Indicates the pointer to the button text.
     * @param listener Indicates the pointer to the listener registered for the button.
     * @since 1.0
     * @version 1.0
     */
    void SetButton(DialogButtonType buttonType, const char* text, UIView::OnClickListener* listener);

    /**
     * @brief Shows this dialog box.
     *
     * @since 5.0
     * @version 3.0
     */
    void Show();

    /**
     * @brief Sets the button color.
     *
     * @param buttonType Indicates the button type.
     * @param color Indicates the button color to set.
     * @since 5.0
     * @version 3.0
     */
    void SetButtonColor(DialogButtonType buttonType, ColorType color);

    /**
     * @brief Sets a listener for monitoring click events occurring outside this dialog box.
     *
     * @param onCancelListener Indicates the pointer to the listener to set.
     * @since 5.0
     * @version 3.0
     */
    void SetOnCancelListener(UIView::OnClickListener* onCancelListener);

    /**
     * @brief Sets whether to close this dialog box when click events occur outside it.
     *
     * @param enable Specifies whether to close this dialog box when click events occur outside it.
     * @since 5.0
     * @version 3.0
     */
    void EnableAutoCancel(bool enable);

#ifdef ENABLE_DEBUG
    const char* GetTitle() const
    {
        return titleText_;
    }

    const char* GetText() const
    {
        return textText_;
    }

    const char* GetButtonText(DialogButtonType buttonType) const;

    UIView::OnClickListener* GetButtonListener(DialogButtonType buttonType) const;

    ColorType GetButtonColor(DialogButtonType buttonType) const;

    UIView::OnClickListener* GetOnCancelListener() const
    {
        return onCancelListener_;
    }

    bool GetEnableAutoCancel() const
    {
        return enableAutoCancel_;
    }
#endif

private:
    const static uint8_t BUTTON_HEIGHT = 40;
    const static uint8_t BUTTON_TOTAL_HEIGHT = 56;
    const static uint8_t PADDING = 24;
    const static uint8_t BUTTON_MID_PADDING = 10;
    const static uint8_t BUTTON_PADDING = 16;
    const static uint8_t BUTTON_PRESS_OPA = 26; // 10% opacity
    const static uint8_t TEXT_BUTTON_PADDING = 8;
    const static uint8_t TEXT_FONT_SIZE = 16;
    const static uint8_t TITLE_FONT_SIZE = 20;
    const static uint8_t TITLE_TOTAL_HEIGHT = 56;
    const static uint8_t BUTTON_FONT_SIZE = 16;
    const static uint16_t MAX_WIDTH_PERCENT = 60;
    const static uint16_t MAX_HEIGHT_PERCENT = 90;
    const static uint8_t TITLE_TEXT_OPA = 230; // 90% opacity
    const static uint8_t LINE_BUTTON_PADDING = 4;
    const static uint8_t LINE_WIDTH = 2;
    const static uint8_t LINE_HEIGHT = 24;
    const static uint8_t LINE_BOTTOM_PADDING = 24;
    const static uint8_t LINE_OPA = 51; // 20% opacity
    const char* TITLE_ID = "dialogTitle";
    const char* BUTTON1_ID = "dialogButton1";
    const char* BUTTON2_ID = "dialogButton2";

    void AddButton(DialogButtonType buttonType, UILabelButton* button, const char* text,
        UIView::OnClickListener* listener);
    void InitDialog();
    void SetTitleLabel();
    void SetTextLabel();
    void AddComponents();
    void MeasureSize();
    uint16_t MeasureMaxWidth();
    uint16_t MeasureTitleWidth();
    uint16_t MeasureTextWidth();
    uint16_t MeasureButtonWidth();
    void Layout();
    void LayoutButton();
    void CreateDialogWindow();
    void DestoryWindow();

    bool isShowing_;
    bool enableAutoCancel_;
    uint8_t buttonNum_;
    UILabel* title_;
    UILabel* text_;
    UILabelButton* button1_;
    UILabelButton* button2_;
    UILabelButton* button3_;
    UIViewGroup* dialogLayer_;
    uint16_t widthMax_;
    uint16_t heightMax_;
    RootView* windowRootView_;
    UIView::OnClickListener* onCancelListener_;
    UIDialogClickListener* dialogClickListener_;
    Window* window_;
    UIView* line1_;
    UIView* line2_;
    const char* titleText_;
    const char* textText_;
    ColorType colorType1_;
    ColorType colorType2_;
    ColorType colorType3_;

    friend class UIDialogLabelButton;
    friend class UIDialogClickListener;
};
}
#endif // GRAPHIC_LITE_UI_DIALOG_H
#endif // ENABLE_WINDOW
