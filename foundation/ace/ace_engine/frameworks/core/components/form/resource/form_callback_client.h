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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_CALLBACK_CLIENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_CALLBACK_CLIENT_H

#include "form_callback_interface.h"

namespace OHOS::Ace {

class FormCallbackClient : public OHOS::AppExecFwk::FormCallbackInterface,
    public std::enable_shared_from_this<FormCallbackClient> {
public:
    FormCallbackClient() = default;
    virtual ~FormCallbackClient() = default;

    void ProcessFormUpdate(const AppExecFwk::FormJsInfo &formJsInfo) override
    {
        auto delegate = delegate_.Upgrade();
        if (delegate) {
            delegate->ProcessFormUpdate(formJsInfo);
        }
    }

    void ProcessFormUninstall(const int64_t formId) override
    {
        auto delegate = delegate_.Upgrade();
        if (delegate) {
            delegate->ProcessFormUninstall(formId);
        }
    }

    void OnDeathReceived() override
    {
        auto delegate = delegate_.Upgrade();
        if (delegate) {
            delegate->OnDeathReceived();
        }
    }

    void SetFormManagerDelegate(WeakPtr<FormManagerDelegate> delegate)
    {
        delegate_ = delegate;
    }

private:
    WeakPtr<FormManagerDelegate> delegate_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_CALLBACK_CLIENT_H
