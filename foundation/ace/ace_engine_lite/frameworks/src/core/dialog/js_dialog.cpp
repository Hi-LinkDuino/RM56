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

#include "acelite_config.h"
#if (FEATURE_MODULE_DIALOG == 1)
#include "ace_log.h"
#include "js_async_work.h"
#include "js_dialog.h"

namespace OHOS {
namespace ACELite {
JSDialog::JSDialog()
    : dialog_(nullptr),
      dialogListener_(nullptr),
      dismissListener_(nullptr),
      title_(nullptr),
      message_(nullptr) {}

JSDialog::~JSDialog()
{
    ReleaseResource();
}

void JSDialog::ReleaseResource()
{
    ACE_FREE(title_);
    ACE_FREE(message_);
    ACE_DELETE(dialog_);
    ACE_DELETE(dismissListener_);
    ACE_DELETE(dialogListener_);
}

void JSDialog::SetTitle(char *title)
{
    title_ = title;
}
void JSDialog::SetMessage(char *message)
{
    message_ = message;
}

/**
 * This the async callback used to release the JSDialog instance.
 */
static void DialogRecycleExecutor(void *jsDialog)
{
    if (jsDialog == nullptr) {
        return;
    }
    // release resource
    delete reinterpret_cast<JSDialog *>(jsDialog);
}

bool JSDialog::ShowDialog(JSIValue thisVal,
                          JSIValue buttons,
                          JSIValue successFunc,
                          JSIValue cancelFunc,
                          JSIValue completeFunc)
{
    ACE_DELETE(dialog_);
    dialog_ = new UIDialog();
    if (dialog_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create ui dialog failed, return ");
        return false;
    }

    dialog_->SetTitle(title_);
    dialog_->SetText(message_);

    // set dialog dismiss listener
    dismissListener_ = new DismissListener(this, cancelFunc, completeFunc, thisVal);
    if (dismissListener_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create dialog dismissListener_ is nullptr, return nullptr.");
        return false;
    }
    dialog_->SetOnCancelListener(dismissListener_);

    // parser dialog button and set button listenr
    if (!ParseButton(this, buttons, successFunc, completeFunc, thisVal)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "parse dialog button error");
        return false;
    }

    dialog_->EnableAutoCancel(true);
    // show dialog
    dialog_->Show();
    return true;
}


bool JSDialog::ParseButton(JSDialog *jsDialog,
                           JSIValue buttonArrayObject,
                           JSIValue successFuncObject,
                           JSIValue completeFuncObject,
                           JSIValue context)
{
    if (jsDialog == nullptr) {
        return false;
    }
    UIDialog *dialog = const_cast<UIDialog *>(jsDialog->GetUIDialog());
    if (dialog == nullptr) {
        return false;
    }
    /* parse dialog buttons */
    uint32_t len = JSI::GetArrayLength(buttonArrayObject);
    const uint8_t maxButtonNum = 3;
    const char * const buttonTextKey = "text";
    const char * const buttonColorKey = "color";
    // support up to 3 buttons
    for (uint32_t index = 0; index < len; index++) {
        if (index >= maxButtonNum) {
            HILOG_WARN(HILOG_MODULE_ACE, "dialog support up to 3 buttons, please check dialog button num");
            break;
        }
        // parse button text
        JSIValue buttonObject = JSI::GetPropertyByIndex(buttonArrayObject, index);
        char *buttonText = JSI::GetStringProperty(buttonObject, buttonTextKey);

        // parse button text color
        char *buttonColorText = JSI::GetStringProperty(buttonObject, buttonColorKey);
        ColorType color = ParseButtonColor(buttonColorText);
        dialog->SetButtonColor(static_cast<UIDialog::DialogButtonType>(index), color);
        // set button click listener
        dialogListener_ = new DialogListener(jsDialog, index, successFuncObject, completeFuncObject, context);
        if (dialogListener_ == nullptr) {
            JSI::ReleaseString(buttonText);
            JSI::ReleaseString(buttonColorText);
            JSI::ReleaseValue(buttonObject);
            return false;
        }
        dialog->SetButton(static_cast<UIDialog::DialogButtonType>(index), buttonText, dialogListener_);
        // release button JSI value
        JSI::ReleaseString(buttonText);
        JSI::ReleaseString(buttonColorText);
        JSI::ReleaseValue(buttonObject);
    }
    return true;
}

ColorType JSDialog::ParseButtonColor(const char * const buttonColorText)
{
    uint32_t color = 0;  // default color
    uint8_t alpha = 0;
    if (!ParseColor(buttonColorText, color, alpha)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input dialog button color error, please check. default color instead");
        return Color::GetColorFromRGB(0, 0, 0);
    }
    HILOG_INFO(HILOG_MODULE_ACE, "dialog buttonColorText = %{public}s, colorVal = %{public}d", buttonColorText, color);
    uint8_t red8 = uint8_t((color & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
    uint8_t green8 = uint8_t((color & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
    uint8_t blue8 = uint8_t((color & TEXT_BLUE_COLOR_MASK));
    return Color::GetColorFromRGB(red8, green8, blue8);
}

/**
 * Call this function if the dialog is dismissed in any scenario.
 */
void JSDialog::DispatchReleaseRequest(const JSDialog *jsDialog)
{
    if (!JsAsyncWork::DispatchAsyncWork(DialogRecycleExecutor, const_cast<JSDialog *>(jsDialog))) {
        // dispatch the release request failed, JSDialog resource leaked
        HILOG_ERROR(HILOG_MODULE_ACE, "dispatch the release request failed, resource leaked [%{private}p]", jsDialog);
    }
}

DialogListener::DialogListener(JSDialog *jsDialog,
                               uint8_t index,
                               JSIValue successFunc,
                               JSIValue completeFunc,
                               JSIValue context)
    : jsDialog_(jsDialog),
      buttonIndex_(index),
      jsSuccessFunc_(JSI::AcquireValue(successFunc)),
      jsCompleteFunc_(JSI::AcquireValue(completeFunc)),
      jsContext_(JSI::AcquireValue(context)) {}

DialogListener::~DialogListener() {}

bool DialogListener::OnClick(UIView &view, const ClickEvent &event)
{
    HILOG_INFO(HILOG_MODULE_ACE, "dialog button on click");
    JSIValue args[1];
    args[0] = JSI::CreateObject();
    const char * const indexStr = "index";

    JSI::SetNumberProperty(args[0], indexStr, buttonIndex_);
    if (JSI::ValueIsFunction(jsSuccessFunc_)) {
        JSI::CallFunction(jsSuccessFunc_, jsContext_, args, 1);
    }
    if (JSI::ValueIsFunction(jsCompleteFunc_)) {
        JSI::CallFunction(jsCompleteFunc_, jsContext_, nullptr, 0);
    }
    JSI::ReleaseValueList(args[0], jsSuccessFunc_, jsCompleteFunc_, jsContext_);
    // the dialog is going to be dismissed, request to do the recycling
    JSDialog::DispatchReleaseRequest(jsDialog_);
    return true;
}

DismissListener::DismissListener(JSDialog *jsDialog, JSIValue cancelFunc, JSIValue completeFunc, JSIValue context)
    : jsDialog_(jsDialog),
      jsCancelFunc_(JSI::AcquireValue(cancelFunc)),
      jsCompleteFunc_(JSI::AcquireValue(completeFunc)),
      jsContext_(JSI::AcquireValue(context)) {}

DismissListener::~DismissListener() {}
bool DismissListener::OnClick(UIView &view, const ClickEvent &event)
{
    HILOG_INFO(HILOG_MODULE_ACE, "dialog dismiss button on click");
    if (JSI::ValueIsFunction(jsCancelFunc_)) {
        JSI::CallFunction(jsCancelFunc_, jsContext_, nullptr, 0);
    }
    if (JSI::ValueIsFunction(jsCompleteFunc_)) {
        JSI::CallFunction(jsCompleteFunc_, jsContext_, nullptr, 0);
    }
    JSI::ReleaseValueList(jsCancelFunc_, jsCompleteFunc_, jsContext_);
    // the dialog is going to be dismissed, request to do the recycling
    JSDialog::DispatchReleaseRequest(jsDialog_);
    return true;
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_MODULE_DIALOG
