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
#include "dialog_module.h"
#include "ace_log.h"
#include "js_profiler.h"
#include "jsi/internal/jsi_internal.h"
#include "ui_dialog.h"

#include "ui_scroll_view.h"
#include "root_view.h"
#include "screen.h"

namespace OHOS {
namespace ACELite {
const char * const DialogModule::TITLE_KEY = "title";
const char * const DialogModule::MSG_KEY = "message";
const char * const DialogModule::BUTTON_KEY = "buttons";
const char * const DialogModule::SUCCESS_FUNC_KEY = "success";
const char * const DialogModule::CANCEL_FUNC_KEY = "cancel";
const char * const DialogModule::COMPLETE_FUNC_KEY = "complete";

JSIValue DialogModule::ShowDialog(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum)
{
    // js calling allow argsNum > 1, but do not process.
    if (argsNum < 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ShowDialog args num(%{public}d) is invalid.", argsNum);
        return AS_JSI_VALUE(jerry_create_error(JERRY_ERROR_TYPE,
            reinterpret_cast<const jerry_char_t *>("params should only be one object.")));
    }

    // support show another dialog when first dialog showing. second dialog cover first one.
    // click second dialog disappear, first one keep going showing
    JSDialog *jsDialog = new JSDialog();
    if (jsDialog == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "dialog is nullptr");
        return AS_JSI_VALUE(jerry_create_error(JERRY_ERROR_TYPE,
            reinterpret_cast<const jerry_char_t *>("create dialog error.")));
    }

    /* parse dialog title */
    char *dialogTitle = JSI::GetStringProperty(args[0], TITLE_KEY);
    jsDialog->SetTitle(dialogTitle);
    HILOG_INFO(HILOG_MODULE_ACE, "dialog title = %{public}s", dialogTitle);

    /* parse dialog message */
    char *message = JSI::GetStringProperty(args[0], MSG_KEY);
    jsDialog->SetMessage(message);
    HILOG_INFO(HILOG_MODULE_ACE, "dialog message =%{public}s", message);

    /* parse dialog button */
    JSIValue buttons = JSI::GetNamedProperty(args[0], BUTTON_KEY);

    /* parse dialog js function */
    JSIValue successFunc = JSI::GetNamedProperty(args[0], SUCCESS_FUNC_KEY);
    JSIValue cancelFunc = JSI::GetNamedProperty(args[0], CANCEL_FUNC_KEY);
    JSIValue completeFunc = JSI::GetNamedProperty(args[0], COMPLETE_FUNC_KEY);

    bool result = jsDialog->ShowDialog(thisVal, buttons, successFunc, cancelFunc, completeFunc);
    if (!result) {
        delete jsDialog;
        jsDialog = nullptr;
    }

    JSI::ReleaseValueList(buttons, successFunc, cancelFunc, completeFunc);

    return AS_JSI_VALUE(UNDEFINED);
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_MODULE_DIALOG
