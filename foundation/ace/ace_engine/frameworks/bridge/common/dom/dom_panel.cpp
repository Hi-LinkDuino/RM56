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

#include "frameworks/bridge/common/dom/dom_panel.h"

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

template<typename T>
inline T ConvertStrToEnum(const char* key, const LinearMapNode<T>* map, size_t length, T defaultValue)
{
    int64_t index = BinarySearchFindIndex(map, length, key);
    return index != -1 ? map[index].value : defaultValue;
}

} // namespace

DOMPanel::DOMPanel(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    panelChild_ = AceType::MakeRefPtr<PanelComponent>(std::to_string(nodeId), nodeName);
    if (IsRightToLeft()) {
        panelChild_->SetTextDirection(TextDirection::RTL);
    }
}

void DOMPanel::PrepareSpecializedComponent()
{
    // adjust panel mode
    if (type_ == PanelType::TEMP_DISPLAY && mode_ == PanelMode::MINI) {
        mode_ = PanelMode::HALF;
    } else if (type_ == PanelType::MINI_BAR && mode_ == PanelMode::HALF) {
        mode_ = PanelMode::MINI;
    }
    panelChild_->SetPanelMode(mode_);
    panelChild_->SetPanelType(type_);
    panelChild_->SetHasDragBar(hasDragBar_);
    panelChild_->SetMiniHeight(miniHeight_);
    panelChild_->SetHalfHeight(halfHeight_);
    panelChild_->SetFullHeight(fullHeight_);
    if (declaration_) {
        panelChild_->SetHasBoxStyle(declaration_->HasBoxStyle());
        panelChild_->SetHasDecorationStyle(declaration_->HasDecorationStyle());
        panelChild_->SetHasBackgroundColor(declaration_->HasBackGroundColor());
        panelChild_->SetHasBorderStyle(declaration_->HasBorderStyle());
    }
    if (boxComponent_) {
        panelChild_->SetBoxStyle(boxComponent_);
    } else {
        boxComponent_ = AceType::MakeRefPtr<BoxComponent>();
        panelChild_->SetBoxStyle(boxComponent_);
    }
}

bool DOMPanel::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    const LinearMapNode<void (*)(const std::string&, DOMPanel&)> specialAttrSetters[] = {
        { DOM_PANEL_ATTR_DRAG_BAR,
            [](const std::string& val, DOMPanel& panel) { panel.hasDragBar_ = StringToBool(val); } },
        { DOM_PANEL_ATTR_FULL_HEIGHT,
            [](const std::string& val, DOMPanel& panel) {
                panel.fullHeight_.second = true;
                panel.fullHeight_.first = StringToDimension(val);
            } },
        { DOM_PANEL_ATTR_HALF_HEIGHT,
            [](const std::string& val, DOMPanel& panel) {
                panel.halfHeight_.second = true;
                panel.halfHeight_.first = StringToDimension(val);
            } },
        { DOM_PANEL_ATTR_MIN_HEIGHT,
            [](const std::string& val, DOMPanel& panel) {
                panel.miniHeight_.second = true;
                panel.miniHeight_.first = StringToDimension(val);
            } },
        { DOM_PANEL_ATTR_MODE,
            [](const std::string& val, DOMPanel& panel) {
                const LinearMapNode<PanelMode> modeTable[] = {
                    { "full", PanelMode::FULL },
                    { "half", PanelMode::HALF },
                    { "mini", PanelMode::MINI },
                };
                panel.mode_ = ConvertStrToEnum(val.c_str(), modeTable, ArraySize(modeTable), PanelMode::FULL);
            } },
        { DOM_SHOW, [](const std::string& val, DOMPanel& panel) { panel.isShow_ = StringToBool(val); } },
        { DOM_PANEL_ATTR_TYPE,
            [](const std::string& val, DOMPanel& panel) {
                const LinearMapNode<PanelType> typeTable[] = {
                    { "foldable", PanelType::FOLDABLE_BAR },
                    { "minibar", PanelType::MINI_BAR },
                    { "temporary", PanelType::TEMP_DISPLAY },
                };
                panel.type_ = ConvertStrToEnum(val.c_str(), typeTable, ArraySize(typeTable), PanelType::FOLDABLE_BAR);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(specialAttrSetters, ArraySize(specialAttrSetters), attr.first.c_str());
    if (operatorIter != -1) {
        specialAttrSetters[operatorIter].value(attr.second, *this);
        return true;
    } else {
        return false;
    }
}

void DOMPanel::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_PANEL_METHOD_SHOW) {
        const auto& controller = panelChild_->GetPanelController();
        if (!controller) {
            return;
        }
        controller->ShowPanel();
    } else if (method == DOM_DIALOG_METHOD_CLOSE) {
        const auto& controller = panelChild_->GetPanelController();
        if (!controller) {
            return;
        }
        controller->ClosePanel();
    } else {
        LOGW("no such method available.");
    }
}

void DOMPanel::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!display_) {
        display_ = AceType::MakeRefPtr<DisplayComponent>(child->GetRootComponent());
    }
    display_->SetVisible(isShow_ ? VisibleType::VISIBLE : VisibleType::GONE);
    if (declaration_) {
        auto& opacityStyle = static_cast<CommonOpacityStyle&>(declaration_->GetStyle(StyleTag::COMMON_OPACITY_STYLE));
        if (opacityStyle.IsValid()) {
            display_->SetOpacity(opacityStyle.opacity);
        }
    }
    panelChild_->SetChild(display_);
}

void DOMPanel::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    panelChild_->SetChild(nullptr);
}

bool DOMPanel::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_PANEL_EVENT_SIZE_CHANGED) {
        panelChild_->SetOnSizeChanged(EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

RefPtr<Component> DOMPanel::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(panelChild_);
    return box;
}

} // namespace OHOS::Ace::Framework
