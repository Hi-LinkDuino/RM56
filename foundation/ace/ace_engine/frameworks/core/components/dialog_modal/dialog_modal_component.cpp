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

#include "core/components/dialog_modal/dialog_modal_component.h"

#include "core/components/box/box_component.h"
#include "core/components/clip/clip_component.h"
#include "core/components/dialog_modal/dialog_modal_element.h"
#include "core/components/dialog_modal/render_dialog_modal.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {
namespace {

const Dimension BG_MARGIN = 16.0_vp;
const Dimension BG_RADIUS = 16.0_vp;
const Color BG_COLOR = Color(0xfff7f7f7);
const char* DIALOG_TWEEN_NAME = "dialogModalTween";

} // namespace

RefPtr<Component> DialogModalComponent::Create(RefPtr<Component> child)
{
    auto component = AceType::MakeRefPtr<DialogModalComponent>();
    component->SetChild(child);
    component->BuildInnerChild();
    return component;
}

RefPtr<Element> DialogModalComponent::CreateElement()
{
    return AceType::MakeRefPtr<DialogModalElement>();
}

RefPtr<RenderNode> DialogModalComponent::CreateRenderNode()
{
    return RenderDialogModal::Create();
}

void DialogModalComponent::BuildInnerChild()
{
    auto bgDecoration = AceType::MakeRefPtr<Decoration>();
    bgDecoration->SetBackgroundColor(BG_COLOR);
    auto bgBox = AceType::MakeRefPtr<BoxComponent>();
    bgBox->SetBackDecoration(bgDecoration);
    bgBox->SetChild(GetChild());
    auto clip = AceType::MakeRefPtr<ClipComponent>(bgBox);
    clip->SetTopLeftRadius(Radius(BG_RADIUS));
    clip->SetTopRightRadius(Radius(BG_RADIUS));
    clip->SetBottomLeftRadius(Radius(BG_RADIUS));
    clip->SetBottomRightRadius(Radius(BG_RADIUS));
    clip->SetHeight(0.0_vp);
    auto box = AceType::MakeRefPtr<BoxComponent>();
    auto decoration = AceType::MakeRefPtr<Decoration>();
    Border border;
    border.SetBorderRadius(Radius(BG_RADIUS));
    decoration->SetBorder(border);
    box->SetMargin(Edge(BG_MARGIN));
    box->SetBackDecoration(decoration);
    box->SetChild(clip);
    auto tween = AceType::MakeRefPtr<TweenComponent>(DIALOG_TWEEN_NAME, box);
    tween->SetIsFirstFrameShow(false);
    SetChild(tween);
}

} // namespace OHOS::Ace