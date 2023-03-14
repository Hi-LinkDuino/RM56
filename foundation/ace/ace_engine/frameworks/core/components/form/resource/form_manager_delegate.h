/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_MANAGER_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_MANAGER_DELEGATE_H

#include <list>

#include "core/components/common/layout/constants.h"
#include "core/components/form/resource/form_manager_resource.h"
#include "core/components/form/resource/form_request_data.h"

#ifdef OHOS_STANDARD_SYSTEM
#include "form_js_info.h"
#include "ohos/aafwk/content/want.h"
#endif

namespace OHOS::Ace {
class FormCallbackClient;
class FormManagerDelegate : public FormManagerResource {
    DECLARE_ACE_TYPE(FormManagerDelegate, FormManagerResource);

public:
    using onFormAcquiredCallbackForJava
        = std::function<void(int64_t, const std::string&, const std::string&, const std::string&)>;
    using OnFormUpdateCallbackForJava = std::function<void(int64_t, const std::string&)>;
    using OnFormAcquiredCallback
        = std::function<void(int64_t, const std::string&, const std::string&,
         const std::string&, const std::map<std::string, std::pair<int, int32_t>>&, const AppExecFwk::FormJsInfo&)>;
    using OnFormUpdateCallback = std::function<void(int64_t, const std::string&,
        const std::map<std::string, std::pair<int, int32_t>>&)>;
    using OnFormErrorCallback = std::function<void(const std::string&, const std::string&)>;
    using OnFormUninstallCallback = std::function<void(int64_t)>;

    enum class State: char {
        WAITINGFORSIZE,
        CREATING,
        CREATED,
        CREATEFAILED,
        RELEASED,
    };

    FormManagerDelegate() = delete;
    ~FormManagerDelegate() override;
    explicit FormManagerDelegate(const WeakPtr<PipelineContext>& context)
        : FormManagerResource("formAdaptor", context),
          state_(State::WAITINGFORSIZE) {}

    void AddForm(const WeakPtr<PipelineContext>& context, const RequestFormInfo& info);
    void ReleasePlatformResource();

    void AddFormAcquireCallback(const OnFormAcquiredCallback& callback);
    void AddFormUpdateCallback(const OnFormUpdateCallback& callback);
    void AddFormErrorCallback(const OnFormErrorCallback& callback);
    void AddFormUninstallCallback(const OnFormUninstallCallback& callback);

    void OnActionEvent(const std::string& action);

#ifdef OHOS_STANDARD_SYSTEM
    void ProcessFormUpdate(const AppExecFwk::FormJsInfo &formJsInfo);
    void ProcessFormUninstall(const int64_t formId);
    void OnDeathReceived();
#endif

private:
    void CreatePlatformResource(const WeakPtr<PipelineContext>& context, const RequestFormInfo& info);
    void Stop();
    void RegisterEvent();
    void UnregisterEvent();
    std::string ConvertRequestInfo(const RequestFormInfo& info) const;

    void OnFormAcquired(const std::string& param);
    void OnFormUpdate(const std::string& param);
    void OnFormError(const std::string& param);

    onFormAcquiredCallbackForJava onFormAcquiredCallbackForJava_;
    OnFormUpdateCallbackForJava onFormUpdateCallbackForJava_;
    OnFormAcquiredCallback onFormAcquiredCallback_;
    OnFormUpdateCallback onFormUpdateCallback_;
    OnFormErrorCallback onFormErrorCallback_;
    OnFormUninstallCallback onFormUninstallCallback_;

    State state_ { State::WAITINGFORSIZE };
#ifdef OHOS_STANDARD_SYSTEM
    int64_t runningCardId_ = -1;
    AAFwk::Want wantCache_;
    bool hasCreated_ = false;
    std::shared_ptr<FormCallbackClient> formCallbackClient_;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_MANAGER_DELEGATE_H
