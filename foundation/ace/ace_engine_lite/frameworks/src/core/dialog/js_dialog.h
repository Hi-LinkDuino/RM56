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

#ifndef OHOS_ACELITE_JS_DIALOG_H
#define OHOS_ACELITE_JS_DIALOG_H

#include "acelite_config.h"
#if (FEATURE_MODULE_DIALOG == 1)
#include "jsi.h"
#include "js_fwk_common.h"
#include "non_copyable.h"
#include "dialog/js_dialog.h"
#include "event_listener.h"
#include "ui_dialog.h"

namespace OHOS {
namespace ACELite {
class DialogListener;
class DismissListener;
/**
 * @brief: The module of dialog, show dialog in window
 */
class JSDialog final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(JSDialog);
    JSDialog();
    ~JSDialog();
    void ReleaseResource();

    void SetTitle(char *title);
    void SetMessage(char *message);

    bool ShowDialog(JSIValue thisVal,
                    JSIValue buttons,
                    JSIValue successFunc,
                    JSIValue cancelFunc,
                    JSIValue completeFunc);
    static void DispatchReleaseRequest(const JSDialog *jsDialog);

    const UIDialog *GetUIDialog() const
    {
        return dialog_;
    }

private:
    UIDialog *dialog_;
    DialogListener *dialogListener_;
    DismissListener *dismissListener_;
    char *title_;
    char *message_;

    bool ParseButton(JSDialog *jsDialog,
                     JSIValue buttonArrayObject,
                     JSIValue successFuncObject,
                     JSIValue completeFuncObject,
                     JSIValue context);
    ColorType ParseButtonColor(const char * const buttonColorText);
};

class DialogListener final: public UIView::OnClickListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DialogListener);
    DialogListener(JSDialog *jsDialog, uint8_t index, JSIValue successFunc, JSIValue completeFunc, JSIValue context);
    ~DialogListener();
    bool OnClick(UIView &view, const ClickEvent &event) override;

private:
    JSDialog *jsDialog_;
    uint8_t buttonIndex_;
    JSIValue jsSuccessFunc_;
    JSIValue jsCompleteFunc_;
    JSIValue jsContext_;
};

class DismissListener final : public UIView::OnClickListener {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DismissListener);
    DismissListener(JSDialog *jsDialog, JSIValue cancelFunc, JSIValue completeFunc, JSIValue context);
    ~DismissListener();
    bool OnClick(UIView &view, const ClickEvent &event) override;

private:
    JSDialog *jsDialog_;
    JSIValue jsCancelFunc_;
    JSIValue jsCompleteFunc_;
    JSIValue jsContext_;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_MODULE_DIALOG
#endif // OHOS_ACELITE_DIALOG_MODULE_H
