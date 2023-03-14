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

#include "core/components/plugin/plugin_sub_container.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "core/common/container_scope.h"
#include "core/common/plugin_manager.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"
#include "frameworks/core/common/flutter/flutter_asset_manager.h"
#include "frameworks/core/common/flutter/flutter_task_executor.h"
#include "frameworks/core/components/plugin/plugin_element.h"
#include "frameworks/core/components/plugin/plugin_window.h"
#include "frameworks/core/components/plugin/render_plugin.h"
#include "frameworks/core/components/transform/transform_element.h"

namespace OHOS::Ace {
namespace {
const int32_t THEME_ID_DEFAULT = 117440515;
constexpr char DECLARATIVE_JS_ENGINE_SHARED_LIB[] = "libace_engine_declarative.z.so";
constexpr char DECLARATIVE_ARK_ENGINE_SHARED_LIB[] = "libace_engine_declarative_ark.z.so";
} // namespace

const char* GetDeclarativeSharedLibrary(bool isArkApp)
{
    if (isArkApp) {
        return DECLARATIVE_ARK_ENGINE_SHARED_LIB;
    } else {
        return DECLARATIVE_JS_ENGINE_SHARED_LIB;
    }
}

void PluginSubContainer::Initialize()
{
    ContainerScope scope(instanceId_);

    if (!outSidePipelineContext_.Upgrade()) {
        LOGE("no pipeline context for create plugin component container.");
        return;
    }

    auto executor = outSidePipelineContext_.Upgrade()->GetTaskExecutor();
    if (!executor) {
        LOGE("could not got main pipeline executor");
        return;
    }
    auto taskExecutor = AceType::DynamicCast<FlutterTaskExecutor>(executor);
    if (!taskExecutor) {
        LOGE("main pipeline context executor is not flutter taskexecutor");
        return;
    }
    taskExecutor_ = Referenced::MakeRefPtr<FlutterTaskExecutor>(taskExecutor);

    frontend_ = AceType::MakeRefPtr<PluginFrontend>();
    if (!frontend_) {
        LOGE("PluginSubContainer::Initialize:frontend_ is nullptr");
        return;
    }

    // set JS engine，init in JS thread
    auto loader = PluginManager::GetInstance().GetJsEngineLoader();
    if (!loader) {
        loader = &Framework::JsEngineLoader::GetDeclarative(GetDeclarativeSharedLibrary(isArkApp_));
        PluginManager::GetInstance().SetJsEngineLoader(loader);
    }
    auto jsEngine = loader->CreateJsEngine(instanceId_);
    if (!jsEngine) {
        LOGE("PluginSubContainer::Initialize:jsEngine is nullptr");
        return;
    }
    if (!PluginManager::GetInstance().GetAceAbility()) {
        jsEngine->AddExtraNativeObject("ability", PluginManager::GetInstance().GetAceAbility());
    }

    frontend_->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
    frontend_->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
    EngineHelper::AddEngine(instanceId_, jsEngine);
    frontend_->SetJsEngine(jsEngine);
    frontend_->Initialize(FrontendType::JS_PLUGIN, taskExecutor_);
}

void PluginSubContainer::Destroy()
{
    ContainerScope scope(instanceId_);

    if (!pipelineContext_) {
        LOGE("no context find for inner plugin");
        return;
    }

    if (!taskExecutor_) {
        LOGE("no taskExecutor find for inner plugin");
        return;
    }

    auto outPipelineContext = outSidePipelineContext_.Upgrade();
    if (outPipelineContext) {
        outPipelineContext->RemoveTouchPipeline(WeakPtr<PipelineContext>(pipelineContext_));
    }
    assetManager_.Reset();
    pipelineContext_.Reset();
    EngineHelper::RemoveEngine(instanceId_);
}

void PluginSubContainer::UpdateRootElmentSize()
{
    auto pluginComponet = AceType::DynamicCast<PluginComponent>(pluginComponent_);
    Dimension rootWidth = 0.0_vp;
    Dimension rootHeight = 0.0_vp;
    if (pluginComponet) {
        rootWidth = pluginComponet->GetWidth();
        rootHeight = pluginComponet->GetHeight();
    }

    if (rootWidth_ == rootWidth && rootHeight_ == rootHeight) {
        LOGE("size not changed, should not change");
        return;
    }

    surfaceWidth_ = outSidePipelineContext_.Upgrade()->NormalizeToPx(rootWidth);
    surfaceHeight_ = outSidePipelineContext_.Upgrade()->NormalizeToPx(rootHeight);
    if (pipelineContext_) {
        pipelineContext_->SetRootSize(density_, rootWidth.Value(), rootHeight.Value());
    }
}

void PluginSubContainer::UpdateSurfaceSize()
{
    if (!taskExecutor_) {
        LOGE("update surface size fail could not post task to ui thread");
        return;
    }
    auto weakContext = AceType::WeakClaim(AceType::RawPtr(pipelineContext_));
    taskExecutor_->PostTask(
        [weakContext, surfaceWidth = surfaceWidth_, surfaceHeight = surfaceHeight_]() {
          auto context = weakContext.Upgrade();
          if (context == nullptr) {
              LOGE("context is nullptr");
              return;
          }
          if (NearZero(surfaceWidth) && NearZero(surfaceHeight)) {
              LOGE("surface is zero, should not update");
              return;
          }
          context->OnSurfaceChanged(surfaceWidth, surfaceHeight);
        },
        TaskExecutor::TaskType::UI);
}

void PluginSubContainer::RunPlugin(
    const std::string& path, const std::string& module, const std::string& source, const std::string& data)
{
    ContainerScope scope(instanceId_);

    frontend_->ResetPageLoadState();
    auto flutterAssetManager = SetAssetManager(path, module);

    auto&& window = std::make_unique<PluginWindow>(outSidePipelineContext_);
    pipelineContext_ = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor_, assetManager_,
        outSidePipelineContext_.Upgrade()->GetPlatformResRegister(),
        frontend_, instanceId_);

    density_ = outSidePipelineContext_.Upgrade()->GetDensity();
    UpdateRootElmentSize();
    pipelineContext_->SetIsJsPlugin(true);

    SetPluginComponentTheme(path, flutterAssetManager);
    SetActionEventHandler();

    auto weakContext = AceType::WeakClaim(AceType::RawPtr(pipelineContext_));
    taskExecutor_->PostTask(
        [weakContext]() {
            auto context = weakContext.Upgrade();
            if (context == nullptr) {
                LOGE("context or root box is nullptr");
                return;
            }
            context->SetupRootElement();
        },
        TaskExecutor::TaskType::UI);
    frontend_->AttachPipelineContext(pipelineContext_);

    if (frontend_) {
        frontend_->SetDensity(density_);
        UpdateSurfaceSize();
    }

    auto plugin = AceType::DynamicCast<PluginElement>(GetPluginElement().Upgrade());
    if (!plugin) {
        LOGE("set draw delegate could not get plugin element");
        return;
    }
    auto renderNode = plugin->GetRenderNode();
    if (!renderNode) {
        LOGE("set draw delegate could not get render node");
        return;
    }
    auto pluginRender = AceType::DynamicCast<RenderPlugin>(renderNode);
    if (!pluginRender) {
        LOGE("set draw delegate could not get render plugin");
        return;
    }
    pipelineContext_->SetDrawDelegate(pluginRender->GetDrawDelegate());

    frontend_->SetInstanceName(module);
    frontend_->RunPage(0, source, data);
}

void PluginSubContainer::SetPluginComponentTheme(
    const std::string& path, const RefPtr<FlutterAssetManager>& flutterAssetManager)
{
    ResourceInfo pluginResourceInfo;
    ResourceConfiguration resConfig;
    resConfig.SetDensity(density_);
    pluginResourceInfo.SetThemeId(THEME_ID_DEFAULT);
    pluginResourceInfo.SetPackagePath(path);
    pluginResourceInfo.SetResourceConfiguration(resConfig);
    pipelineContext_->SetThemeManager(AceType::MakeRefPtr<ThemeManager>());
    auto pluginThemeManager = pipelineContext_->GetThemeManager();
    if (pluginThemeManager) {
        // Init resource, load theme map, do not parse yet.
        pluginThemeManager->InitResource(pluginResourceInfo);
        pluginThemeManager->LoadSystemTheme(pluginResourceInfo.GetThemeId());
        auto weakTheme = AceType::WeakClaim(AceType::RawPtr(pluginThemeManager));
        auto weakAsset = AceType::WeakClaim(AceType::RawPtr(flutterAssetManager));
        taskExecutor_->PostTask(
            [weakTheme, weakAsset]() {
                auto themeManager = weakTheme.Upgrade();
                if (themeManager == nullptr) {
                    LOGE("themeManager or aceView is null!");
                    return;
                }
                themeManager->ParseSystemTheme();
                themeManager->SetColorScheme(ColorScheme::SCHEME_LIGHT);
                themeManager->LoadCustomTheme(weakAsset.Upgrade());
            },
            TaskExecutor::TaskType::UI);
    }
}

void PluginSubContainer::SetActionEventHandler()
{
    auto&& actionEventHandler = [weak = WeakClaim(this)](const std::string& action) {
        auto container = weak.Upgrade();
        if (!container) {
            LOGE("ActionEventHandler sub container is null!");
            return;
        }
        auto plugin = AceType::DynamicCast<PluginElement>(container->GetPluginElement().Upgrade());
        if (!plugin) {
            LOGE("ActionEventHandler plugin is null!");
            return;
        }

        plugin->OnActionEvent(action);
    };
    pipelineContext_->SetActionEventHandler(actionEventHandler);
}

RefPtr<FlutterAssetManager> PluginSubContainer::SetAssetManager(const std::string& path, const std::string& module)
{
    RefPtr<FlutterAssetManager> flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
    if (flutterAssetManager) {
        frontend_->SetAssetManager(flutterAssetManager);
        assetManager_ = flutterAssetManager;

        auto assetProvider = AceType::MakeRefPtr<Plugin::FileAssetProvider>();
        std::string temp1 = "assets/js/" + module + "/";
        std::string temp2 = "assets/js/share/";
        std::vector<std::string> basePaths;
        basePaths.push_back(temp1);
        basePaths.push_back(temp2);
        basePaths.push_back("");

        if (assetProvider->Initialize(path, basePaths)) {
            LOGD("push plugin asset provider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }

    return flutterAssetManager;
}

void PluginSubContainer::UpdatePlugin(const std::string& content)
{
    ContainerScope scope(instanceId_);

    if (!frontend_) {
        LOGE("update plugin fial due to could not find plugin front end");
        return;
    }
    frontend_->UpdatePlugin(content);
}
} // namespace OHOS::Ace
