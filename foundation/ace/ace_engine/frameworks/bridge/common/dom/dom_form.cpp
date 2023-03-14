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

#include "frameworks/bridge/common/dom/dom_form.h"

namespace OHOS::Ace::Framework {

DOMForm::DOMForm(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    std::list<RefPtr<Component>> components;
    columnContainer_ = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, components);
}

void DOMForm::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (child) {
        columnContainer_->InsertChild(slot, child->GetRootComponent());
    }
}

void DOMForm::PrepareSpecializedComponent() {}

bool DOMForm::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_ID) {
        formId_ = attr.second;
    }
    return false;
}

bool DOMForm::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_FORM_EVENT_SUBMIT) {
        onSubmit_ = AceAsyncEvent<void(const std::string&)>::Create(
            EventMarker(GetNodeIdForEvent(), event, pageId), pipelineContext_);
        return true;
    } else if (event == DOM_FORM_EVENT_RESET) {
        onReset_ = AceAsyncEvent<void()>::Create(EventMarker(GetNodeIdForEvent(), event, pageId), pipelineContext_);
        return true;
    }
    return false;
}

void DOMForm::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (child) {
        columnContainer_->RemoveChild(child->GetRootComponent());
    }
}

void DOMForm::Submit()
{
    if (!onSubmit_) {
        LOGE("the submit callback is nullptr");
        return;
    }
    // call on the ui thread.
    auto json = JsonUtil::Create(false);
    for (const auto& weak : formValueList_) {
        auto formValue = weak.Upgrade();
        if (formValue) {
            const auto& pair = formValue->GetFormValue();
            if (pair.first.empty()) {
                continue;
            }
            json->Put(pair.first.c_str(), pair.second.c_str());
        }
    }
    auto value = JsonUtil::Create(true);
    value->Put("value", json);
    value->Put("formId", formId_.c_str());
    std::string result = std::string(R"("submit",)").append(value->ToString()).append(", null");
    onSubmit_(result);
}

void DOMForm::Reset()
{
    // call on the ui thread.
    for (const auto& weak : formValueList_) {
        auto formValue = weak.Upgrade();
        if (formValue) {
            formValue->Reset();
        }
    }
    if (onReset_) {
        onReset_();
    }
}

} // namespace OHOS::Ace::Framework