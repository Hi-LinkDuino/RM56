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

#include "core/common/container_scope.h"
#include "core/components/form/sub_container.h"

#include <cstdint>

#include "ashmem.h"

#include "adapter/ohos/entrance/file_asset_provider.h"
#include "frameworks/core/common/flutter/flutter_asset_manager.h"
#include "frameworks/core/common/flutter/flutter_task_executor.h"
#include "frameworks/core/components/form/form_element.h"
#include "frameworks/core/components/form/form_window.h"
#include "frameworks/core/components/form/render_form.h"
#include "frameworks/core/components/transform/transform_element.h"

namespace OHOS::Ace {

namespace {

const int32_t THEME_ID_DEFAULT = 117440515;

} // namespace

void SubContainer::Initialize()
{
    if (!outSidePipelineContext_.Upgrade()) {
        LOGE("no pipeline context for create form component container.");
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

    frontend_ = AceType::MakeRefPtr<CardFrontend>();
    frontend_->Initialize(FrontendType::JS_CARD, taskExecutor_);
}

void SubContainer::Destroy()
{
    if (!pipelineContext_) {
        LOGE("no context find for inner card");
        return;
    }

    if (!taskExecutor_) {
        LOGE("no taskExecutor find for inner card");
        return;
    }

    auto outPipelineContext = outSidePipelineContext_.Upgrade();
    if (outPipelineContext) {
        outPipelineContext->RemoveTouchPipeline(WeakPtr<PipelineContext>(pipelineContext_));
    }

    assetManager_.Reset();
    pipelineContext_.Reset();
}

void SubContainer::UpdateRootElmentSize()
{
    auto formComponet = AceType::DynamicCast<FormComponent>(formComponent_);
    Dimension rootWidth = 0.0_vp;
    Dimension rootHeight = 0.0_vp;
    if (formComponet) {
        rootWidth = formComponet->GetWidth();
        rootHeight = formComponet->GetHeight();
    }

    if (rootWidht_ == rootWidth && rootHeight_ == rootHeight) {
        LOGE("size not changed, should not change");
        return;
    }

    surfaceWidth_ = outSidePipelineContext_.Upgrade()->NormalizeToPx(rootWidth);
    surfaceHeight_ = outSidePipelineContext_.Upgrade()->NormalizeToPx(rootHeight);
    if (pipelineContext_) {
        pipelineContext_->SetRootSize(density_, rootWidth.Value(), rootHeight.Value());
    }
}

void SubContainer::UpdateSurfaceSize()
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

void SubContainer::RunCard(const int64_t id, const std::string path, const std::string module, const std::string data,
    const std::map<std::string, std::pair<int, int32_t>> imageDataMap, const std::string formSrc)
{
    if (id == runningCardId_) {
        LOGE("the card is showing, no need run again");
        return;
    }
    runningCardId_ = id;
    if (onFormAcquiredCallback_) {
        onFormAcquiredCallback_(id);
    }

    frontend_->ResetPageLoadState();
    LOGI("run card path:%{private}s, module:%{private}s, data:%{private}s", path.c_str(), module.c_str(), data.c_str());
    RefPtr<FlutterAssetManager> flutterAssetManager;
    flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();

    if (flutterAssetManager) {
        frontend_->SetAssetManager(flutterAssetManager);
        assetManager_ = flutterAssetManager;

        auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
        std::string temp1 = "assets/js/" + module + "/";
        std::string temp2 = "assets/js/share/";
        std::string temp3 = formSrc;
        std::vector<std::string> basePaths;
        basePaths.push_back(temp1);
        basePaths.push_back(temp2);
        basePaths.push_back("./");
        basePaths.push_back("./js/");
        if (assetProvider->Initialize(path, basePaths)) {
            LOGI("push card asset provider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }
    frontend_->SetFormSrc(formSrc);
    frontend_->SetCardWindowConfig(GetWindowConfig());
    auto&& window = std::make_unique<FormWindow>(outSidePipelineContext_);

    pipelineContext_ = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor_, assetManager_, nullptr, frontend_, instanceId_);
    ContainerScope scope(instanceId_);
    density_ = outSidePipelineContext_.Upgrade()->GetDensity();
    auto eventManager = outSidePipelineContext_.Upgrade()->GetEventManager();
    pipelineContext_->SetEventManager(eventManager);
    ProcessSharedImage(imageDataMap);
    UpdateRootElmentSize();
    pipelineContext_->SetIsJsCard(true);

    ResourceInfo cardResourceInfo;
    ResourceConfiguration resConfig;
    resConfig.SetDensity(density_);
    cardResourceInfo.SetThemeId(THEME_ID_DEFAULT);
    cardResourceInfo.SetPackagePath(path);
    cardResourceInfo.SetResourceConfiguration(resConfig);
    auto cardThemeManager = pipelineContext_->GetThemeManager();
    if (cardThemeManager) {
        // Init resource, load theme map, do not parse yet.
        cardThemeManager->InitResource(cardResourceInfo);
        cardThemeManager->LoadSystemTheme(cardResourceInfo.GetThemeId());
        auto weakTheme = AceType::WeakClaim(AceType::RawPtr(cardThemeManager));
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

    auto&& actionEventHandler = [weak = WeakClaim(this)](const std::string& action) {
        auto container = weak.Upgrade();
        if (!container) {
            LOGE("ActionEventHandler sub container is null!");
            return;
        }
        auto form = AceType::DynamicCast<FormElement>(container->GetFormElement().Upgrade());
        if (!form) {
            LOGE("ActionEventHandler form is null!");
            return;
        }

        form->OnActionEvent(action);
    };
    pipelineContext_->SetActionEventHandler(actionEventHandler);

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

    auto cardFronted = AceType::DynamicCast<CardFrontend>(frontend_);
    if (frontend_) {
        frontend_->SetDensity(density_);
        UpdateSurfaceSize();
    }

    auto form = AceType::DynamicCast<FormElement>(GetFormElement().Upgrade());
    if (!form) {
        LOGE("set draw delegate could not get form element");
        return;
    }
    auto renderNode = form->GetRenderNode();
    if (!renderNode) {
        LOGE("set draw delegate could not get render node");
        return;
    }
    auto formRender = AceType::DynamicCast<RenderForm>(renderNode);
    if (!formRender) {
        LOGE("set draw delegate could not get render form");
        return;
    }
    pipelineContext_->SetDrawDelegate(formRender->GetDrawDelegate());

    frontend_->RunPage(0, "", data);
}

void SubContainer::ProcessSharedImage(const std::map<std::string, std::pair<int, int32_t>> imageDataMap)
{
    std::vector<std::string> picNameArray;
    std::vector<int> fileDescriptorArray;
    std::vector<int> byteLenArray;
    if (!imageDataMap.empty()) {
        for (auto& imageData : imageDataMap) {
            picNameArray.push_back(imageData.first);
            fileDescriptorArray.push_back(imageData.second.first);
            byteLenArray.push_back(imageData.second.second);
        }
        GetNamesOfSharedImage(picNameArray);
        UpdateSharedImage(picNameArray, byteLenArray, fileDescriptorArray);
    }
}

void SubContainer::GetNamesOfSharedImage(std::vector<std::string>& picNameArray)
{
    if (picNameArray.empty()) {
        LOGE("picNameArray is null!");
        return;
    }
    auto pipelineCtx = GetPipelineContext();
    if (!pipelineCtx) {
        LOGE("pipeline context is null!");
        return;
    }
    RefPtr<SharedImageManager> sharedImageManager;
    if (!pipelineCtx->GetSharedImageManager()) {
        sharedImageManager = AceType::MakeRefPtr<SharedImageManager>(pipelineCtx->GetTaskExecutor());
        GetPipelineContext()->SetSharedImageManager(sharedImageManager);
    } else {
        sharedImageManager = pipelineCtx->GetSharedImageManager();
    }
    auto nameSize = picNameArray.size();
    for (uint32_t i = 0; i < nameSize; i++) {
        // get name of picture
        auto name = picNameArray[i];
        sharedImageManager->AddPictureNamesToReloadMap(std::move(name));
    }
}

void SubContainer::UpdateSharedImage(
    std::vector<std::string>& picNameArray, std::vector<int32_t>& byteLenArray, std::vector<int>& fileDescriptorArray)
{
    auto pipelineCtx = GetPipelineContext();
    if (!pipelineCtx) {
        LOGE("pipeline context is null! when try UpdateSharedImage");
        return;
    }
    if (picNameArray.empty() || byteLenArray.empty() || fileDescriptorArray.empty()) {
        LOGE("array is null! when try UpdateSharedImage");
        return;
    }
    auto nameArraySize = picNameArray.size();
    if (nameArraySize != byteLenArray.size()) {
        LOGE("nameArraySize does not equal to fileDescriptorArraySize, please check!");
        return;
    }
    if (nameArraySize != fileDescriptorArray.size()) {
        LOGE("nameArraySize does not equal to fileDescriptorArraySize, please check!");
        return;
    }
    // now it can be assured that all three arrays are of the same size

    std::string picNameCopy;
    for (uint32_t i = 0; i < nameArraySize; i++) {
        // get name of picture
        auto picName = picNameArray[i];
        // save a copy of picName and ReleaseStringUTFChars immediately to avoid memory leak
        picNameCopy = picName;

        // get fd ID
        auto fd = fileDescriptorArray[i];

        auto newFd = dup(fd);
        if (newFd < 0) {
            LOGE("dup fd fail, fail reason: %{public}s, fd: %{public}d, picName: %{private}s, length: %{public}d",
                strerror(errno), fd, picNameCopy.c_str(), byteLenArray[i]);
            continue;
        }

        auto ashmem = Ashmem(newFd, byteLenArray[i]);
        GetImageDataFromAshmem(picNameCopy, ashmem, pipelineCtx, byteLenArray[i]);
        ashmem.UnmapAshmem();
        ashmem.CloseAshmem();
    }
}

void SubContainer::GetImageDataFromAshmem(
    const std::string& picName, Ashmem& ashmem, const RefPtr<PipelineContext>& pipelineContext, int len)
{
    bool ret = ashmem.MapReadOnlyAshmem();
    // if any exception causes a [return] before [AddSharedImage], the memory image will not show because [RenderImage]
    // will never be notified to start loading.
    if (!ret) {
        LOGE("MapReadOnlyAshmem fail, fail reason: %{public}s, picName: %{private}s, length: %{public}d, "
             "fd: %{public}d",
            strerror(errno), picName.c_str(), len, ashmem.GetAshmemFd());
        return;
    }
    const uint8_t* imageData = reinterpret_cast<const uint8_t*>(ashmem.ReadFromAshmem(len, 0));
    if (imageData == nullptr) {
        LOGE("imageData is nullptr, errno is: %{public}s, picName: %{private}s, length: %{public}d, fd: %{public}d",
            strerror(errno), picName.c_str(), len, ashmem.GetAshmemFd());
        return;
    }
    RefPtr<SharedImageManager> sharedImageManager = pipelineContext->GetSharedImageManager();
    if (sharedImageManager) {
        // read image data from shared memory and save a copy to sharedImageManager
        sharedImageManager->AddSharedImage(picName, std::vector<uint8_t>(imageData, imageData + len));
    }
}

void SubContainer::UpdateCard(const std::string content,
    const std::map<std::string, std::pair<int, int32_t>> imageDataMap)
{
    if (!frontend_) {
        LOGE("update card fial due to could not find card front end");
        return;
    }
    if (allowUpdate_) {
        frontend_->UpdateData(std::move(content));
        ProcessSharedImage(imageDataMap);
    }
}

bool SubContainer::Dump(const std::vector<std::string>& params)
{
    if (pipelineContext_) {
        pipelineContext_->Dump(params);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
