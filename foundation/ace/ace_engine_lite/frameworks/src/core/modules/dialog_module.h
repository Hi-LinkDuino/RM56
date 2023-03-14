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

#ifndef OHOS_ACELITE_DIALOG_MODULE_H
#define OHOS_ACELITE_DIALOG_MODULE_H

#include "acelite_config.h"
#if (FEATURE_MODULE_DIALOG == 1)
#include "jsi.h"
#include "js_fwk_common.h"
#include "non_copyable.h"

#include "ui_dialog.h"
#include "dialog/js_dialog.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief: The module of dialog, show dialog in window
 */
class DialogModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DialogModule);
    DialogModule() {}
    ~DialogModule() {}
    static JSIValue ShowDialog(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum);

private:
    static const char * const TITLE_KEY;
    static const char * const MSG_KEY;
    static const char * const BUTTON_KEY;
    static const char * const SUCCESS_FUNC_KEY;
    static const char * const CANCEL_FUNC_KEY;
    static const char * const COMPLETE_FUNC_KEY;
};

void InitDialogModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "showDialog", DialogModule::ShowDialog);
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_MODULE_DIALOG
#endif // OHOS_ACELITE_DIALOG_MODULE_H
