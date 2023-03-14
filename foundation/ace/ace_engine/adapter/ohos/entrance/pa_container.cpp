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

#include "adapter/ohos/entrance/pa_container.h"

#include "flutter/lib/ui/ui_dart_state.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/file_asset_provider.h"
#include "adapter/ohos/entrance/pa_engine/engine/common/js_backend_engine_loader.h"
#include "adapter/ohos/entrance/pa_engine/pa_backend.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/ace_engine.h"
#include "core/common/flutter/flutter_asset_manager.h"
#include "core/common/flutter/flutter_task_executor.h"
#include "core/common/platform_window.h"
#include "core/common/text_field_manager.h"
#include "core/common/watch_dog.h"
#include "core/common/window.h"

namespace OHOS::Ace::Platform {
namespace {

constexpr char ARK_PA_ENGINE_SHARED_LIB[] = "libace_engine_pa_ark.z.so";
constexpr char QUICK_JS_PA_ENGINE_SHARED_LIB[] = "libace_engine_pa_qjs.z.so";

const char* GetPaEngineSharedLibrary(bool isArkApp)
{
    if (isArkApp) {
        return ARK_PA_ENGINE_SHARED_LIB;
    } else {
        return QUICK_JS_PA_ENGINE_SHARED_LIB;
    }
}

} // namespace

PaContainer::PaContainer(int32_t instanceId, BackendType type, bool isArkApp, void* paAbility,
    std::unique_ptr<PlatformEventCallback> callback)
    : instanceId_(instanceId), type_(type), isArkApp_(isArkApp), paAbility_(paAbility)
{
    ACE_DCHECK(callback);
    auto flutterTaskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    flutterTaskExecutor->InitPlatformThread();
    flutterTaskExecutor->InitJsThread();
    taskExecutor_ = flutterTaskExecutor;

    InitializeBackend();

    platformEventCallback_ = std::move(callback);
}

void PaContainer::InitializeBackend()
{
    // create backend
    backend_ = Backend::Create();
    auto paBackend = AceType::DynamicCast<PaBackend>(backend_);
    if (!paBackend) {
        LOGE("create paBackend error");
        return;
    }

    // set JS engine, init in JS thread
    auto& loader = JsBackendEngineLoader::Get(GetPaEngineSharedLibrary(isArkApp_));
    auto jsEngine = loader.CreateJsBackendEngine(instanceId_);
    jsEngine->AddExtraNativeObject("ability", paAbility_);
    jsEngine->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
    jsEngine->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
    paBackend->SetJsEngine(jsEngine);

    ACE_DCHECK(backend_);
    backend_->Initialize(type_, taskExecutor_);
}

RefPtr<PaContainer> PaContainer::GetContainer(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    return AceType::DynamicCast<PaContainer>(container);
}

void PaContainer::CreateContainer(int32_t instanceId, BackendType type, bool isArkApp, void* paAbility,
    std::unique_ptr<PlatformEventCallback> callback)
{
    auto aceContainer = AceType::MakeRefPtr<PaContainer>(instanceId, type, isArkApp, paAbility, std::move(callback));
    AceEngine::Get().AddContainer(instanceId, aceContainer);

    auto back = aceContainer->GetBackend();
    if (back) {
        back->UpdateState(Backend::State::ON_CREATE);
        back->SetJsMessageDispatcher(aceContainer);
    }
}

bool PaContainer::RunPa(int32_t instanceId, const std::string& content, const OHOS::AAFwk::Want& want)
{
    LOGI("PA: PaContainer::RunPa start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is null");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is null");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is null");
        return false;
    }
    paBackend->RunPa(content, want);
    LOGI("PA: PaContainer::RunPa end");
    return true;
}

bool PaContainer::OnCreate(int32_t instanceId, const OHOS::AAFwk::Want& want)
{
    LOGI("PA: PaContainer::OnCreate start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnCreate(want);
    LOGI("PA: PaContainer::OnCreate end");
    return true;
}

bool PaContainer::OnDelete(int32_t instanceId, int64_t formId)
{
    LOGI("PA: PaContainer::OnDelete start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnDelete(formId);
    LOGI("PA: PaContainer::OnDelete end");
    return true;
}

bool PaContainer::OnTriggerEvent(int32_t instanceId, int64_t formId, const std::string& message)
{
    LOGI("PA: PaContainer::OnTriggerEvent start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnTriggerEvent(formId, message);
    LOGI("PA: PaContainer::OnTriggerEvent end");
    return true;
}

bool PaContainer::OnUpdate(int32_t instanceId, int64_t formId)
{
    LOGI("PA: PaContainer::OnUpdate start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnUpdate(formId);
    LOGI("PA: PaContainer::OnUpdate end");
    return true;
}

bool PaContainer::OnCastTemptoNormal(int32_t instanceId, int64_t formId)
{
    LOGI("PA: PaContainer::OnCastTemptoNormal start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnCastTemptoNormal(formId);
    LOGI("PA: PaContainer::OnCastTemptoNormal end");
    return true;
}

bool PaContainer::OnVisibilityChanged(int32_t instanceId, const std::map<int64_t, int32_t>& formEventsMap)
{
    LOGI("PA: PaContainer::OnVisibilityChanged start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return false;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return false;
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return false;
    }
    paBackend->OnVisibilityChanged(formEventsMap);
    LOGI("PA: PaContainer::OnVisibilityChanged end");
    return true;
}

AppExecFwk::FormProviderData PaContainer::GetFormData(int32_t instanceId)
{
    LOGI("PA: PaContainer::GetFormData start");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("PA: container is nullptr");
        return AppExecFwk::FormProviderData();
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("PA: aceContainer is nullptr");
        return AppExecFwk::FormProviderData();
    }
    auto paBackend = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (!paBackend) {
        LOGE("PA: paBackend is nullptr");
        return AppExecFwk::FormProviderData();
    }
    return paBackend->GetFormData();
}

void PaContainer::DestroyContainer(int32_t instanceId)
{
    LOGI("DestroyContainer with id %{private}d", instanceId);
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    if (!aceContainer) {
        LOGE("Get paBackend error");
        return;
    }
    auto back = aceContainer->GetBackend();
    if (back) {
        back->UpdateState(Backend::State::ON_DESTROY);
    }
    return;
}

void PaContainer::AddAssetPath(
    int32_t instanceId, const std::string& packagePath, const std::vector<std::string>& paths)
{
    auto container = AceType::DynamicCast<PaContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }

    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        AceType::DynamicCast<PaBackend>(container->GetBackend())->SetAssetManager(flutterAssetManager);
    }
    if (flutterAssetManager && !packagePath.empty()) {
        auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
        if (assetProvider->Initialize(packagePath, paths)) {
            LOGI("Push AssetProvider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }
}

void PaContainer::AddLibPath(int32_t instanceId, const std::string& libPath)
{
    auto container = AceType::DynamicCast<PaContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }

    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        AceType::DynamicCast<PaBackend>(container->GetBackend())->SetAssetManager(flutterAssetManager);
    }
    if (flutterAssetManager) {
        flutterAssetManager->SetLibPath(libPath);
    }
}

int32_t PaContainer::Insert(int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value)
{
    LOGI("Insert with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->Insert(uri, value);
    }
    return ret;
}

std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> PaContainer::Query(int32_t instanceId, const Uri& uri,
    const std::vector<std::string>& columns, const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Query with id %{public}d", instanceId);
    std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> ret;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->Query(uri, columns, predicates);
    }
    return ret;
}

int32_t PaContainer::Update(int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
    const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Update with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->Update(uri, value, predicates);
    }
    return ret;
}

int32_t PaContainer::Delete(
    int32_t instanceId, const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates)
{
    LOGI("Delete with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->Delete(uri, predicates);
    }
    return ret;
}

int32_t PaContainer::BatchInsert(
    int32_t instanceId, const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values)
{
    LOGI("BatchInsert with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->BatchInsert(uri, values);
    }
    return ret;
}

std::string PaContainer::GetType(int32_t instanceId, const Uri& uri)
{
    LOGI("GetType with id %{public}d", instanceId);
    std::string ret;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->GetType(uri);
    }
    return ret;
}

std::vector<std::string> PaContainer::GetFileTypes(
    int32_t instanceId, const Uri& uri, const std::string& mimeTypeFilter)
{
    LOGI("GetFileTypes with id %{public}d", instanceId);
    std::vector<std::string> ret;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->GetFileTypes(uri, mimeTypeFilter);
    }
    return ret;
}

int32_t PaContainer::OpenFile(int32_t instanceId, const Uri& uri, const std::string& mode)
{
    LOGI("OpenFile with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->OpenFile(uri, mode);
    }
    return ret;
}

int32_t PaContainer::OpenRawFile(int32_t instanceId, const Uri& uri, const std::string& mode)
{
    LOGI("OpenRawFile with id %{public}d", instanceId);
    int32_t ret = 0;
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->OpenRawFile(uri, mode);
    }
    return ret;
}

Uri PaContainer::NormalizeUri(int32_t instanceId, const Uri& uri)
{
    LOGI("NormalizeUri with id %{public}d", instanceId);
    Uri ret("");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->NormalizeUri(uri);
    }
    return ret;
}

Uri PaContainer::DenormalizeUri(int32_t instanceId, const Uri& uri)
{
    LOGI("DenormalizeUri with id %{public}d", instanceId);
    Uri ret("");
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d", instanceId);
        return ret;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = AceType::DynamicCast<PaBackend>(aceContainer->GetBackend());
    if (back) {
        ret = back->DenormalizeUri(uri);
    }
    return ret;
}

void PaContainer::Dispatch(
    const std::string& group, std::vector<uint8_t>&& data, int32_t id, bool replyToComponent) const
{
    return;
}

void PaContainer::DispatchPluginError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    return;
}

bool PaContainer::Dump(const std::vector<std::string>& params)
{
    return false;
}

sptr<IRemoteObject> PaContainer::OnConnect(int32_t instanceId, const OHOS::AAFwk::Want& want)
{
    LOGI("OnConnect with id %{private}d", instanceId);
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no pa container with id %{private}d", instanceId);
        return nullptr;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = aceContainer->GetBackend();
    if (back) {
        auto paBackend = AceType::DynamicCast<PaBackend>(back);
        if (paBackend == nullptr) {
            LOGE("DynamicCast paBackend failed with id %{private}d", instanceId);
            return nullptr;
        }
        return paBackend->OnConnect(want);
    }
    return nullptr;
}

void PaContainer::OnDisConnect(int32_t instanceId, const OHOS::AAFwk::Want& want)
{
    LOGI("OnDisConnect with id %{private}d", instanceId);
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no pa container with id %{private}d", instanceId);
        return;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = aceContainer->GetBackend();
    if (back) {
        auto paBackend = AceType::DynamicCast<PaBackend>(back);
        if (paBackend == nullptr) {
            LOGE("DynamicCast paBackend failed with id %{private}d", instanceId);
            return;
        }
        paBackend->OnDisConnect(want);
    }
}

void PaContainer::OnCommand(const OHOS::AAFwk::Want &want, int startId, int32_t instanceId)
{
    LOGI("OnCommand with id %{private}d", instanceId);
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no pa container with id %{private}d", instanceId);
        return;
    }
    auto aceContainer = AceType::DynamicCast<PaContainer>(container);
    auto back = aceContainer->GetBackend();
    if (back) {
        auto paBackend = AceType::DynamicCast<PaBackend>(back);
        if (paBackend == nullptr) {
            LOGE("DynamicCast paBackend failed with id %{private}d", instanceId);
            return;
        }
        paBackend->OnCommand(want, startId);
    }
}

} // namespace OHOS::Ace::Platform
