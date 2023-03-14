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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_CPP_PA_CONTAINER_H
#define FOUNDATION_ACE_ADAPTER_OHOS_CPP_PA_CONTAINER_H

#include <memory>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "form_provider_info.h"
#include "iremote_object.h"
#include "values_bucket.h"
#include "want.h"

#include "adapter/ohos/entrance/platform_event_callback.h"
#include "base/resource/asset_manager.h"
#include "base/thread/task_executor.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_view.h"
#include "core/common/container.h"
#include "core/common/js_message_dispatcher.h"
#include "frameworks/core/common/backend.h"

namespace OHOS::Ace::Platform {

class PaContainer : public Container, public JsMessageDispatcher {
    DECLARE_ACE_TYPE(PaContainer, Container, JsMessageDispatcher);

public:
    PaContainer(int32_t instanceId, BackendType type, bool isArkApp, void* paAbility,
        std::unique_ptr<PlatformEventCallback> callback);
    ~PaContainer() override = default;

    void Initialize() override {}

    void Destroy() override {}

    int32_t GetInstanceId() const override
    {
        return instanceId_;
    }

    RefPtr<AssetManager> GetAssetManager() const override
    {
        return assetManager_;
    }

    RefPtr<Frontend> GetFrontend() const override
    {
        return nullptr;
    }

    RefPtr<PlatformResRegister> GetPlatformResRegister() const override
    {
        return nullptr;
    }

    RefPtr<PipelineContext> GetPipelineContext() const override
    {
        return nullptr;
    }

    int32_t GetViewWidth() const override
    {
        return 0;
    }

    int32_t GetViewHeight() const override
    {
        return 0;
    }

    void* GetView() const override
    {
        return nullptr;
    }

    RefPtr<Backend> GetBackend() const
    {
        return backend_;
    }

    RefPtr<TaskExecutor> GetTaskExecutor() const override
    {
        return taskExecutor_;
    }

    void Dispatch(
        const std::string& group, std::vector<uint8_t>&& data, int32_t id, bool replyToComponent) const override;

    void DispatchPluginError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const override;

    bool Dump(const std::vector<std::string>& params) override;

    void DispatchSync(
        const std::string& group, std::vector<uint8_t>&& data, uint8_t** resData, int64_t& position) const override
    {}

    void OnFinish()
    {
        if (platformEventCallback_) {
            platformEventCallback_->OnFinish();
        }
    }

    std::string GetHostClassName() const override
    {
        return "";
    }

    static bool Register();
    static void CreateContainer(int32_t instanceId, BackendType type, bool isArkApp, void* paAbility,
        std::unique_ptr<PlatformEventCallback> callback);
    static void DestroyContainer(int32_t instanceId);
    static RefPtr<PaContainer> GetContainer(int32_t instanceId);
    static bool RunPa(int32_t instanceId, const std::string& content, const OHOS::AAFwk::Want& want);
    static void AddAssetPath(int32_t instanceId, const std::string& packagePath, const std::vector<std::string>& paths);
    static void AddLibPath(int32_t instanceId, const std::string& libPath);
    static bool OnCreate(int32_t instanceId, const OHOS::AAFwk::Want& want);
    static bool OnDelete(int32_t instanceId, int64_t formId);
    static bool OnTriggerEvent(int32_t instanceId, int64_t formId, const std::string& message);
    static bool OnUpdate(int32_t instanceId, int64_t formId);
    static bool OnCastTemptoNormal(int32_t instanceId, int64_t formId);
    static bool OnVisibilityChanged(int32_t instanceId, const std::map<int64_t, int32_t>& formEventsMap);
    static AppExecFwk::FormProviderData GetFormData(int32_t instanceId);
    static int32_t Insert(int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value);
    static std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> Query(int32_t instanceId, const Uri& uri,
        const std::vector<std::string>& columns, const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    static int32_t Update(int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    static int32_t Delete(int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates);

    static int32_t BatchInsert(
        int32_t instanceId, const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values);
    static std::string GetType(int32_t instanceId, const Uri& uri);
    static std::vector<std::string> GetFileTypes(int32_t instanceId, const Uri& uri, const std::string& mimeTypeFilter);
    static int32_t OpenFile(int32_t instanceId, const Uri& uri, const std::string& mode);
    static int32_t OpenRawFile(int32_t instanceId, const Uri& uri, const std::string& mode);
    static Uri NormalizeUri(int32_t instanceId, const Uri& uri);
    static Uri DenormalizeUri(int32_t instanceId, const Uri& uri);
    static sptr<IRemoteObject> OnConnect(int32_t instanceId, const OHOS::AAFwk::Want& want);
    static void OnDisConnect(int32_t instanceId, const OHOS::AAFwk::Want& want);
    static void OnCommand(const OHOS::AAFwk::Want &want, int startId, int32_t instanceId);

private:
    void InitializeBackend();
    void InitializeCallback();

    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<AssetManager> assetManager_;
    RefPtr<Backend> backend_;

    int32_t instanceId_ = 0;
    BackendType type_ = BackendType::SERVICE;
    bool isArkApp_ = false;
    std::unique_ptr<PlatformEventCallback> platformEventCallback_;
    void* paAbility_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(PaContainer);
};

} // namespace OHOS::Ace::Platform

#endif // FOUNDATION_ACE_ADAPTER_OHOS_CPP_PA_CONTAINER_H
