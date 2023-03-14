/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACE_DIALOG_CALLBACK_INTERFACE_H
#define OHOS_ACE_DIALOG_CALLBACK_INTERFACE_H

#include "dialog_callback.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Ace {
class IDialogCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ace.DialogCallback");

    enum {
        UI_SERVICE_DIALOG_CALLBACK,
    };

    /**
     * @brief Called back to dialog
     *
     */
    virtual void OnDialogCallback(int32_t id, const std::string& event, const std::string& params) = 0;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_ACE_DIALOG_CALLBACK_INTERFACE_H
