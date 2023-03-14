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

#include "frameworks/bridge/common/dom/dom_label.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/text_span/text_span_component.h"
#include "frameworks/bridge/common/dom/input/dom_radio_util.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMLabel::DOMLabel(NodeId nodeId, const std::string& nodeName) : DOMText(nodeId, nodeName) {}

DOMLabel::~DOMLabel()
{
    BackEndEventManager<void(const TouchEventInfo&)>::GetInstance().RemoveBackEndEvent(clickMarker_);
}

void DOMLabel::PrepareSpecializedComponent()
{
    DOMText::PrepareSpecializedComponent();
    if (!labelComponent_) {
        labelComponent_ = AceType::MakeRefPtr<TouchListenerComponent>(textChild_);
    }

    clickMarker_ = BackEndEventManager<void(const TouchEventInfo&)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(const TouchEventInfo&)>::GetInstance().BindBackendEvent(
        clickMarker_, [weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto labelComponent = weak.Upgrade();
            if (!labelComponent) {
                return;
            }
            auto trigger = labelComponent->GetLabelTrigger();
            if (!trigger) {
                LOGW("label not get trigger");
                return;
            }
            trigger->Click();
        });
    labelComponent_->SetOnTouchDownId(clickMarker_);
}

RefPtr<Component> DOMLabel::GetSpecializedComponent()
{
    return labelComponent_;
}

} // namespace OHOS::Ace::Framework
