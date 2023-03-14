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

#include "core/components/container_modal/render_container_modal.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/container_modal/container_modal_component.h"
#include "core/components/image/image_component.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderContainerModal::Create()
{
    return AceType::MakeRefPtr<RenderContainerModal>();
}

void RenderContainerModal::UpdateStyle(const RefPtr<Component>& component) const
{
    auto containerModal = AceType::DynamicCast<ContainerModalComponent>(component);
    if (!containerModal) {
        return;
    }
    auto themeManager = GetThemeManager();
    if (!themeManager) {
        LOGE("get theme manager failed");
        return;
    }
    auto themeConstants = themeManager->GetThemeConstants();
    if (!themeConstants) {
        LOGE("get theme constants failed");
        return;
    }
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("get pipeline context failed");
        return;
    }
    auto labelId = context->GetAppLabelId();
    auto appLabelComponent = containerModal->GetTitleLabel();
    if (appLabelComponent && labelId != 0) {
        auto appLabel = themeConstants->GetString(labelId);
        appLabelComponent->SetData(appLabel);
    }

    auto iconId = context->GetAppIconId();
    auto appIconComponent = containerModal->GetTitleIcon();
    if (appIconComponent && iconId != 0) {
        auto appIconSrc = themeConstants->GetMediaPath(iconId);
        appIconComponent->SetSrc(appIconSrc);
    }
}

void RenderContainerModal::Update(const RefPtr<Component>& component)
{
    auto containerModal = AceType::DynamicCast<ContainerModalComponent>(component);
    if (!containerModal) {
        LOGE("containerModal update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    UpdateStyle(component);
    MarkNeedLayout();
}

bool RenderContainerModal::ParseRequiredRenderNodes()
{
    auto containerBox = AceType::DynamicCast<RenderBox>(GetFirstChild());
    if (!containerBox) {
        LOGE("Container box render node is null!");
        return false;
    }
    containerBox_ = AceType::WeakClaim(AceType::RawPtr(containerBox));
    return true;
}

void RenderContainerModal::PerformLayout()
{
    // Only 2 children are allowed in RenderContainerModal
    if (GetChildren().empty()) {
        LOGE("Children size wrong in container modal");
        return;
    }
    ParseRequiredRenderNodes();

    // ContainerModalComponent's size is as large as the root's.
    auto maxSize = GetLayoutParam().GetMaxSize();
    auto columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::PANEL);
    columnInfo->GetParent()->BuildColumnWidth();
    SetLayoutSize(maxSize);

    ContainerBoxLayout();
}

void RenderContainerModal::ContainerBoxLayout()
{
    auto maxSize = GetLayoutParam().GetMaxSize();
    // layout container box
    auto containerBox = containerBox_.Upgrade();
    if (!containerBox) {
        LOGE("Container box render node is null, layout failed.");
        return;
    }

    LayoutParam containerLayoutParam;
    containerLayoutParam.SetMinSize(Size());
    containerLayoutParam.SetMaxSize(Size(maxSize.Width(), maxSize.Height()));
    containerBox->Layout(containerLayoutParam);
    containerBox->SetPosition(Offset(0.0, 0.0));
}

} // namespace OHOS::Ace
