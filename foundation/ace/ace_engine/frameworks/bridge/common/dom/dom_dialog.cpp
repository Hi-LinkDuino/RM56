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

#include "frameworks/bridge/common/dom/dom_dialog.h"

#include "base/log/event_report.h"
#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMDialog::DOMDialog(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    dialogChild_ = AceType::MakeRefPtr<CustomDialogComponent>(std::to_string(nodeId), nodeName);
}

void DOMDialog::PrepareSpecializedComponent()
{
    dialogChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    if (dialogWidth_.second) {
        dialogChild_->SetWidth(dialogWidth_.first);
    }
    if (dialogHeight_.second) {
        dialogChild_->SetHeight(dialogHeight_.first);
    }
    dialogChild_->SetIsDragable(dragable_);
    if (display_) {
        display_->SetVisible(isShow_ ? VisibleType::VISIBLE : VisibleType::GONE);
    }

    if (customizedMargin_) {
        auto margin = Edge(marginLeft_, marginTop_, marginRight_, marginBottom_);
        dialogChild_->SetMargin(margin);
    }
}

bool DOMDialog::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMDialog&)> styleOperators[] = {
        { DOM_HEIGHT,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.dialogHeight_.first = dialog.ParseDimension(val);
                dialog.dialogHeight_.second = true;
            } },
        { DOM_MARGIN_BOTTOM,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.marginBottom_ = dialog.ParseDimension(val);
                dialog.customizedMargin_ = true;
            } },
        { DOM_MARGIN_END,
            [](const std::string& val, DOMDialog& dialog) {
                if (dialog.IsRightToLeft()) {
                    dialog.marginLeft_ = dialog.ParseDimension(val);
                } else {
                    dialog.marginRight_ = dialog.ParseDimension(val);
                }
                dialog.customizedMargin_ = true;
            } },
        { DOM_MARGIN_LEFT,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.marginLeft_ = dialog.ParseDimension(val);
                dialog.customizedMargin_ = true;
            } },
        { DOM_MARGIN_RIGHT,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.marginRight_ = dialog.ParseDimension(val);
                dialog.customizedMargin_ = true;
            } },
        { DOM_MARGIN_START,
            [](const std::string& val, DOMDialog& dialog) {
                if (dialog.IsRightToLeft()) {
                    dialog.marginRight_ = dialog.ParseDimension(val);
                } else {
                    dialog.marginLeft_ = dialog.ParseDimension(val);
                }
                dialog.customizedMargin_ = true;
            } },
        { DOM_MARGIN_TOP,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.marginTop_ = dialog.ParseDimension(val);
                dialog.customizedMargin_ = true;
            } },
        { DOM_WIDTH,
            [](const std::string& val, DOMDialog& dialog) {
                dialog.dialogWidth_.first = dialog.ParseDimension(val);
                dialog.dialogWidth_.second = true;
            } },
    };
    auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), style.first.c_str());
    if (operatorIter != -1) {
        styleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DOMDialog::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_SHOW) {
        isShow_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_DIALOG_STYLE_DRAGABLE) {
        dragable_ = StringToBool(attr.second);
        return true;
    }
    return false;
}

void DOMDialog::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_DIALOG_METHOD_SHOW) {
        const auto& controller = dialogChild_->GetDialogController();
        if (!controller) {
            return;
        }
        controller->ShowDialog();
    } else if (method == DOM_DIALOG_METHOD_CLOSE) {
        const auto& controller = dialogChild_->GetDialogController();
        if (!controller) {
            return;
        }
        controller->CloseDialog();
    } else {
        LOGW("no such method available.");
    }
}

void DOMDialog::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!display_) {
        display_ = AceType::MakeRefPtr<DisplayComponent>(child->GetRootComponent());
    }
    display_->SetVisible(isShow_ ? VisibleType::VISIBLE : VisibleType::GONE);
    dialogChild_->SetChild(display_);
}

void DOMDialog::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    dialogChild_->SetChild(nullptr);
}

bool DOMDialog::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_DIALOG_EVENT_CANCEL) {
        dialogChild_->SetOnCancel(EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    } else if (event == DOM_DIALOG_METHOD_SHOW) {
        dialogChild_->SetOnShow(EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    } else if (event == DOM_DIALOG_METHOD_CLOSE) {
        dialogChild_->SetOnClose(EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }

    LOGE("unsupported event: %{public}s.", event.c_str());
    EventReport::SendComponentException(ComponentExcepType::DIALOG_EVENT_ERR);
    return false;
}

RefPtr<Component> DOMDialog::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(dialogChild_);
    return box;
}

} // namespace OHOS::Ace::Framework
