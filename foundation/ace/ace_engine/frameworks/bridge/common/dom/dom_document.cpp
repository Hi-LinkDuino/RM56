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

#include "frameworks/bridge/common/dom/dom_document.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/common/thread_checker.h"
#include "core/components/flex/flex_item_component.h"
#include "frameworks/bridge/common/dom/dom_button.h"
#include "frameworks/bridge/common/dom/dom_calendar.h"
#include "frameworks/bridge/common/dom/dom_canvas.h"
#include "frameworks/bridge/common/dom/dom_chart.h"
#include "frameworks/bridge/common/dom/dom_clock.h"
#include "frameworks/bridge/common/dom/dom_dialog.h"
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/dom/dom_divider.h"
#include "frameworks/bridge/common/dom/dom_form.h"
#include "frameworks/bridge/common/dom/dom_grid_column.h"
#include "frameworks/bridge/common/dom/dom_grid_container.h"
#include "frameworks/bridge/common/dom/dom_grid_row.h"
#include "frameworks/bridge/common/dom/dom_image.h"
#include "frameworks/bridge/common/dom/dom_image_animator.h"
#include "frameworks/bridge/common/dom/dom_input.h"
#include "frameworks/bridge/common/dom/dom_label.h"
#include "frameworks/bridge/common/dom/dom_list.h"
#include "frameworks/bridge/common/dom/dom_list_item.h"
#include "frameworks/bridge/common/dom/dom_list_item_group.h"
#include "frameworks/bridge/common/dom/dom_marquee.h"
#include "frameworks/bridge/common/dom/dom_navigation_bar.h"
#include "frameworks/bridge/common/dom/dom_panel.h"
#include "frameworks/bridge/common/dom/dom_picker_view.h"
#include "frameworks/bridge/common/dom/dom_progress.h"
#include "frameworks/bridge/common/dom/dom_proxy.h"
#include "frameworks/bridge/common/dom/dom_qrcode.h"
#include "frameworks/bridge/common/dom/dom_refresh.h"
#ifdef WEB_SUPPORTED
#include "frameworks/bridge/common/dom/dom_rich_text.h"
#endif
#include "frameworks/bridge/common/dom/dom_search.h"
#include "frameworks/bridge/common/dom/dom_slider.h"
#include "frameworks/bridge/common/dom/dom_span.h"
#include "frameworks/bridge/common/dom/dom_stack.h"
#include "frameworks/bridge/common/dom/dom_svg.h"
#include "frameworks/bridge/common/dom/dom_svg_animate.h"
#include "frameworks/bridge/common/dom/dom_svg_animate_motion.h"
#include "frameworks/bridge/common/dom/dom_svg_animate_transform.h"
#include "frameworks/bridge/common/dom/dom_svg_circle.h"
#include "frameworks/bridge/common/dom/dom_svg_defs.h"
#include "frameworks/bridge/common/dom/dom_svg_ellipse.h"
#include "frameworks/bridge/common/dom/dom_svg_fe_colormatrix.h"
#include "frameworks/bridge/common/dom/dom_svg_fe_composite.h"
#include "frameworks/bridge/common/dom/dom_svg_fe_gaussianblur.h"
#include "frameworks/bridge/common/dom/dom_svg_fe_offset.h"
#include "frameworks/bridge/common/dom/dom_svg_filter.h"
#include "frameworks/bridge/common/dom/dom_svg_g.h"
#include "frameworks/bridge/common/dom/dom_svg_line.h"
#include "frameworks/bridge/common/dom/dom_svg_mask.h"
#include "frameworks/bridge/common/dom/dom_svg_path.h"
#include "frameworks/bridge/common/dom/dom_svg_polygon.h"
#include "frameworks/bridge/common/dom/dom_svg_polyline.h"
#include "frameworks/bridge/common/dom/dom_svg_rect.h"
#include "frameworks/bridge/common/dom/dom_svg_text.h"
#include "frameworks/bridge/common/dom/dom_svg_text_path.h"
#include "frameworks/bridge/common/dom/dom_svg_tspan.h"
#include "frameworks/bridge/common/dom/dom_svg_use.h"
#include "frameworks/bridge/common/dom/dom_swiper.h"
#include "frameworks/bridge/common/dom/dom_switch.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/common/dom/dom_textarea.h"
#include "frameworks/bridge/common/dom/dom_toggle.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "frameworks/bridge/common/dom/dom_xcomponent.h"
#endif
#ifndef WEARABLE_PRODUCT
#include "frameworks/bridge/common/dom/dom_badge.h"
#include "frameworks/bridge/common/dom/dom_camera.h"
#include "frameworks/bridge/common/dom/dom_menu.h"
#include "frameworks/bridge/common/dom/dom_navigation_menu.h"
#include "frameworks/bridge/common/dom/dom_option.h"
#include "frameworks/bridge/common/dom/dom_picker_dialog.h"
#include "frameworks/bridge/common/dom/dom_piece.h"
#include "frameworks/bridge/common/dom/dom_popup.h"
#include "frameworks/bridge/common/dom/dom_rating.h"
#include "frameworks/bridge/common/dom/dom_select.h"
#include "frameworks/bridge/common/dom/dom_stepper.h"
#include "frameworks/bridge/common/dom/dom_stepper_item.h"
#include "frameworks/bridge/common/dom/dom_tab_bar.h"
#include "frameworks/bridge/common/dom/dom_tab_content.h"
#include "frameworks/bridge/common/dom/dom_tabs.h"
#include "frameworks/bridge/common/dom/dom_tool_bar.h"
#include "frameworks/bridge/common/dom/dom_tool_bar_item.h"
#include "frameworks/bridge/common/dom/dom_video.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#ifdef WEB_SUPPORTED
#include "frameworks/bridge/common/dom/dom_web.h"
#endif
#endif
#endif

namespace OHOS::Ace::Framework {
namespace {

// avoid same with root node id
constexpr int32_t PROXY_ID = 10000000;
const char PROXY_PREFIX[] = "proxy_";
const int32_t ROOT_STACK_ID = -1;
const char ROOT_STACK_NAME[] = "ComposedStack";

template<class T>
RefPtr<DOMNode> DOMNodeCreator(NodeId nodeId, const std::string& tag, int32_t)
{
    return AceType::MakeRefPtr<T>(nodeId, tag);
}

template<class T>
RefPtr<DOMNode> DOMListItemCreator(NodeId nodeId, const std::string& tag, int32_t itemIndex)
{
    return AceType::MakeRefPtr<T>(nodeId, tag, itemIndex);
}

} // namespace

DOMDocument::~DOMDocument()
{
    CHECK_RUN_ON(UI);
    LOG_DESTROY();
}

RefPtr<DOMNode> DOMDocument::CreateNodeWithId(const std::string& tag, NodeId nodeId, int32_t itemIndex)
{
    LOGD("DOMDocument:created new DOMNode %{public}s, id %{public}d, itemIndex %{public}d", tag.c_str(), nodeId,
         itemIndex);

    static const LinearMapNode<RefPtr<DOMNode>(*)(NodeId, const std::string&, int32_t)> domNodeCreators[] = {
        { DOM_NODE_TAG_ANIMATE, &DOMNodeCreator<DOMSvgAnimate> },
        { DOM_NODE_TAG_ANIMATE_MOTION, &DOMNodeCreator<DOMSvgAnimateMotion> },
        { DOM_NODE_TAG_ANIMATE_TRANSFORM, &DOMNodeCreator<DOMSvgAnimateTransform> },
        { DOM_NODE_TAG_BADGE, &DOMNodeCreator<DOMBadge> },
        { DOM_NODE_TAG_BUTTON, &DOMNodeCreator<DOMButton> },
        { DOM_NODE_TAG_CALENDAR, &DOMNodeCreator<DomCalendar> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_CAMERA, &DOMNodeCreator<DOMCamera> },
#endif
        { DOM_NODE_TAG_CANVAS, &DOMNodeCreator<DOMCanvas> },
        { DOM_NODE_TAG_CHART, &DOMNodeCreator<DOMChart> },
        { DOM_NODE_TAG_CIRCLE, &DOMNodeCreator<DOMSvgCircle> },
        { DOM_NODE_TAG_CLOCK, &DOMNodeCreator<DOMClock> },
        { DOM_NODE_TAG_DEFS, &DOMNodeCreator<DOMSvgDefs> },
        { DOM_NODE_TAG_DIALOG, &DOMNodeCreator<DOMDialog> },
        { DOM_NODE_TAG_DIV, &DOMNodeCreator<DOMDiv> },
        { DOM_NODE_TAG_DIVIDER, &DOMNodeCreator<DOMDivider> },
        { DOM_NODE_TAG_ELLIPSE, &DOMNodeCreator<DOMSvgEllipse> },
        { DOM_NODE_TAG_FE_COLORMATRIX, &DOMNodeCreator<DOMSvgFeColorMatrix> },
        { DOM_NODE_TAG_FE_COMPOSITE, &DOMNodeCreator<DOMSvgFeComposite> },
        { DOM_NODE_TAG_FE_GAUSSIANBLUR, &DOMNodeCreator<DOMSvgFeGaussianBlur> },
        { DOM_NODE_TAG_FE_OFFSET, &DOMNodeCreator<DOMSvgFeOffset> },
        { DOM_NODE_TAG_FILTER, &DOMNodeCreator<DOMSvgFilter> },
        { DOM_NODE_TAG_FORM, &DOMNodeCreator<DOMForm> },
        { DOM_NODE_TAG_G, &DOMNodeCreator<DOMSvgG> },
        { DOM_NODE_TAG_GRID_COLUMN, &DOMNodeCreator<DomGridColumn> },
        { DOM_NODE_TAG_GRID_CONTAINER, &DOMNodeCreator<DomGridContainer> },
        { DOM_NODE_TAG_GRID_ROW, &DOMNodeCreator<DomGridRow> },
        { DOM_NODE_TAG_IMAGE, &DOMNodeCreator<DOMImage> },
        { DOM_NODE_TAG_IMAGE_ANIMATOR, &DOMNodeCreator<DOMImageAnimator> },
        { DOM_NODE_TAG_INPUT, &DOMNodeCreator<DOMInput> },
        { DOM_NODE_TAG_LABEL, &DOMNodeCreator<DOMLabel> },
        { DOM_NODE_TAG_LINE, &DOMNodeCreator<DOMSvgLine> },
        { DOM_NODE_TAG_LIST, &DOMNodeCreator<DOMList> },
        { DOM_NODE_TAG_LIST_ITEM, &DOMListItemCreator<DOMListItem> },
        { DOM_NODE_TAG_LIST_ITEM_GROUP, &DOMListItemCreator<DOMListItemGroup> },
        { DOM_NODE_TAG_MARQUEE, &DOMNodeCreator<DOMMarquee> },
        { DOM_NODE_TAG_MASK, &DOMNodeCreator<DOMSvgMask> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_MENU, &DOMNodeCreator<DOMMenu> },
#endif
        { DOM_NODE_TAG_NAVIGATION_BAR, &DOMNodeCreator<DomNavigationBar> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_NAVIGATION_MENU, &DOMNodeCreator<DOMNavigationMenu> },
        { DOM_NODE_TAG_OPTION, &DOMNodeCreator<DOMOption> },
        { DOM_NODE_TAG_PANEL, &DOMNodeCreator<DOMPanel> },
#endif
        { DOM_NODE_TAG_PATH, &DOMNodeCreator<DOMSvgPath> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_PICKER_DIALOG, &DOMNodeCreator<DOMPickerDialog> },
#endif
        { DOM_NODE_TAG_PICKER_VIEW, &DOMNodeCreator<DOMPickerView> },
        { DOM_NODE_TAG_POLYGON, &DOMNodeCreator<DOMSvgPolygon> },
        { DOM_NODE_TAG_POLYLINE, &DOMNodeCreator<DOMSvgPolyline> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_POPUP, &DOMNodeCreator<DOMPopup> },
#endif
        { DOM_NODE_TAG_PROGRESS, &DOMNodeCreator<DOMProgress> },
        { DOM_NODE_TAG_QRCODE, &DOMNodeCreator<DOMQrcode> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_RATING, &DOMNodeCreator<DOMRating> },
#endif
        { DOM_NODE_TAG_RECT, &DOMNodeCreator<DOMSvgRect> },
        { DOM_NODE_TAG_REFRESH, &DOMNodeCreator<DOMRefresh> },
#ifdef WEB_SUPPORTED
        { DOM_NODE_TAG_RICH_TEXT, &DOMNodeCreator<DOMRichText> },
#endif
        { DOM_NODE_TAG_SEARCH, &DOMNodeCreator<DOMSearch> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_SELECT, &DOMNodeCreator<DOMSelect> },
#endif
        { DOM_NODE_TAG_SLIDER, &DOMNodeCreator<DOMSlider> },
        { DOM_NODE_TAG_SPAN, &DOMNodeCreator<DOMSpan> },
        { DOM_NODE_TAG_STACK, &DOMNodeCreator<DOMStack> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_STEPPER, &DOMNodeCreator<DOMStepper> },
        { DOM_NODE_TAG_STEPPER_ITEM, &DOMListItemCreator<DOMStepperItem> },
#endif
        { DOM_NODE_TAG_SVG, &DOMNodeCreator<DOMSvg> },
        { DOM_NODE_TAG_SVG_TEXT, &DOMNodeCreator<DOMSvgText> },
        { DOM_NODE_TAG_SWIPER, &DOMNodeCreator<DOMSwiper> },
        { DOM_NODE_TAG_SWITCH, &DOMNodeCreator<DOMSwitch> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_TAB_BAR, &DOMNodeCreator<DOMTabBar> },
        { DOM_NODE_TAG_TAB_CONTENT, &DOMNodeCreator<DOMTabContent> },
        { DOM_NODE_TAG_TABS, &DOMNodeCreator<DOMTabs> },
#endif
        { DOM_NODE_TAG_TEXT, &DOMNodeCreator<DOMText> },
        { DOM_NODE_TAG_TEXTAREA, &DOMNodeCreator<DOMTextarea> },
        { DOM_NODE_TAG_TEXT_PATH, &DOMNodeCreator<DOMSvgTextPath> },
        { DOM_NODE_TAG_TOGGLE, &DOMNodeCreator<DOMToggle> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_TOOL_BAR, &DOMNodeCreator<DOMToolBar> },
        { DOM_NODE_TAG_TOOL_BAR_ITEM, &DOMNodeCreator<DOMToolBarItem> },
#endif
        { DOM_NODE_TAG_TSPAN, &DOMNodeCreator<DOMSvgTspan> },
        { DOM_NODE_TAG_USE, &DOMNodeCreator<DOMSvgUse> },
#ifndef WEARABLE_PRODUCT
        { DOM_NODE_TAG_VIDEO, &DOMNodeCreator<DOMVideo> },
#ifdef WEB_SUPPORTED
        { DOM_NODE_TAG_WEB, &DOMNodeCreator<DOMWeb> },
#endif
#endif
#ifdef XCOMPONENT_SUPPORTED
        { DOM_NODE_TAG_XCOMPONENT, &DOMNodeCreator<DOMXComponent> },
#endif
    };
#ifndef WEARABLE_PRODUCT
    static const LinearMapNode<RefPtr<DOMNode>(*)(NodeId, const std::string&, int32_t)> phoneNodeCreators[] = {
        { DOM_NODE_TAG_PIECE, &DOMNodeCreator<DOMPiece> },
    };
#endif

    RefPtr<DOMNode> domNode;
    int64_t creatorIndex = BinarySearchFindIndex(domNodeCreators, ArraySize(domNodeCreators), tag.c_str());
    if (creatorIndex >= 0) {
        domNode = domNodeCreators[creatorIndex].value(nodeId, tag, itemIndex);
#ifndef WEARABLE_PRODUCT
    } else {
        if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
            creatorIndex = BinarySearchFindIndex(phoneNodeCreators, ArraySize(phoneNodeCreators), tag.c_str());
            if (creatorIndex >= 0) {
                domNode = phoneNodeCreators[creatorIndex].value(nodeId, tag, itemIndex);
            }
        } else {
            LOGW("node type %{public}s is invalid", tag.c_str());
            return nullptr;
        }
#endif
    }
    if (!domNode) {
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        if (std::strcmp(tag.c_str(), DOM_NODE_TAG_WEB) == 0 || std::strcmp(tag.c_str(), DOM_NODE_TAG_XCOMPONENT) == 0 ||
            std::strcmp(tag.c_str(), DOM_NODE_TAG_RICH_TEXT) == 0) {
            LOGW("[Engine Log] Unable to use the %{public}s component in the Previewer. Perform this operation on the "
                 "emulator or a real device instead.",
                tag.c_str());
        }
#endif
        LOGW("node type %{public}s is invalid", tag.c_str());
        return nullptr;
    }

    auto result = domNodes_.try_emplace(nodeId, domNode);
    if (!result.second) {
        LOGW("the node has already in the map");
        return nullptr;
    }
    domNode->SetIsRootNode(nodeId == rootNodeId_);
    domNode->SetPageId(rootNodeId_ - DOM_ROOT_NODE_ID_BASE);
    return domNode;
}

RefPtr<DOMProxy> DOMDocument::CreateProxyNodeWithId(const std::string& tag, NodeId nodeId)
{
    // generate proxy tag and id, proxy id marked different from root tag id
    std::string proxyTag = std::string(PROXY_PREFIX) + tag;
    NodeId proxyId = PROXY_ID + nodeId;
    auto proxy = AceType::MakeRefPtr<DOMProxy>(proxyId, proxyTag);
    auto result = domNodes_.try_emplace(proxyId, proxy);
    if (!result.second) {
        LOGW("the node has already in the map");
        return nullptr;
    }
    proxyRelatedNode_.emplace(nodeId);
    return proxy;
}

RefPtr<DOMNode> DOMDocument::GetDOMNodeById(NodeId nodeId) const
{
    const auto itNode = domNodes_.find(nodeId);
    if (itNode == domNodes_.end()) {
        LOGE("the node is not in the map");
        return nullptr;
    }
    return itNode->second;
}

void DOMDocument::RemoveNodes(const RefPtr<DOMNode>& node, bool scheduleUpdate)
{
    if (!node) {
        LOGE("remove null node");
        return;
    }
    auto children = node->GetChildList();
    for (auto it = children.begin(); it != children.end();) {
        RemoveNodes(*it++, scheduleUpdate);
    }
    auto parentId = node->GetParentId();
    RefPtr<DOMNode> parentNode;
    if (parentId != -1) {
        parentNode = GetDOMNodeById(parentId);
        if (parentNode) {
            parentNode->RemoveNode(node);
        }
    }
    // Fixed positioned node and navigation bar node need to delete the relative proxy node.
    if (node->GetPosition() == PositionType::FIXED && rootStackComponent_) {
        rootStackComponent_->RemoveChild(node->GetRootComponent());
        auto context = node->GetPipelineContext().Upgrade();
        if (context && scheduleUpdate) {
            context->ScheduleUpdate(rootComposedStack_);
        }
        auto proxyId = node->GetNodeId() + PROXY_ID;
        auto proxy = GetDOMNodeById(proxyId);
        if (proxy) {
            if (parentNode) {
                parentNode->RemoveNode(proxy);
            }
            domNodes_.erase(proxyId);
            proxyRelatedNode_.erase(node->GetNodeId());
        }
    }
    if (node->GetTag() == DOM_NODE_TAG_NAVIGATION_BAR && rootStackComponent_ &&
        rootStackComponent_->HasNavigationBar()) {
        auto rootColumn = AceType::DynamicCast<ColumnComponent>(rootStackComponent_->GetChildren().front());
        if (rootColumn) {
            rootColumn->RemoveChild(node->GetRootComponent());
            auto proxyId = node->GetNodeId() + PROXY_ID;
            auto proxy = GetDOMNodeById(proxyId);
            if (proxy) {
                if (parentNode) {
                    parentNode->RemoveNode(proxy);
                }
                domNodes_.erase(proxyId);
                proxyRelatedNode_.erase(node->GetNodeId());
            }
        }
    }
    domNodes_.erase(node->GetNodeId());
}

void DOMDocument::AddNodeWithId(const std::string& key, const RefPtr<DOMNode>& domNode)
{
    nodeWithIdVec_.emplace_back(key, domNode);
}

void DOMDocument::AddNodeWithTarget(const std::string& key, const RefPtr<DOMNode>& domNode)
{
    // Always save only one (the last) 'target' dom node
    for (auto iter = nodeWithTargetVec_.begin(); iter != nodeWithTargetVec_.end();) {
        if (iter->first == key) {
            iter = nodeWithTargetVec_.erase(iter);
        } else {
            ++iter;
        }
    }
    nodeWithTargetVec_.emplace_back(key, domNode);
}

void DOMDocument::HandleComponentPostBinding()
{
    LOGD("HandleComponentPostBinding");
    for (auto iterTarget = nodeWithTargetVec_.begin(); iterTarget != nodeWithTargetVec_.end();) {
        auto targetNode = iterTarget->second.Upgrade();
        if (!targetNode) {
            LOGW("DOM:: Node with target %{public}s has been invalid", iterTarget->first.c_str());
            iterTarget = nodeWithTargetVec_.erase(iterTarget);
            continue;
        }

        for (auto iterId = nodeWithIdVec_.begin(); iterId != nodeWithIdVec_.end();) {
            auto idNode = iterId->second.Upgrade();
            if (!idNode) {
                LOGW("DOM:: Node with id %{public}s has been invalid, add targetNode fail", iterId->first.c_str());
                iterId = nodeWithIdVec_.erase(iterId);
                continue;
            }

            if (iterTarget->first != iterId->first) {
                ++iterId;
                continue;
            }

            auto component = targetNode->GetSpecializedComponent();
            if (AceType::InstanceOf<TouchListenerComponent>(component)) {
                auto labelNode = AceType::DynamicCast<DOMLabel>(targetNode);
                if (labelNode) {
                    labelNode->SetTargetNode(idNode);
                }
#ifndef WEARABLE_PRODUCT
            } else if (AceType::InstanceOf<PopupComponent>(component)) {
                auto popupNode = AceType::DynamicCast<DOMPopup>(targetNode);
                if (popupNode) {
                    // DOMPopup bind node with ID node
                    popupNode->BindIdNode(idNode);
                }
            } else if (AceType::InstanceOf<MenuComponent>(component)) {
                auto menuNode = AceType::DynamicCast<DOMMenu>(targetNode);
                if (menuNode) {
                    // DOMMenu bind node with ID node
                    menuNode->BindIdNode(idNode);
                }
            } else if (AceType::InstanceOf<DOMTabBar>(targetNode)) {
                auto tabBarNode = AceType::DynamicCast<DOMTabBar>(targetNode);
                // DOMTabBar in navigation bar bind tabs with ID node
                tabBarNode->BindToTabs(idNode);
#endif
            } else {
                LOGW("component type not supported");
            }
            ++iterId;
        }
        ++iterTarget;
    }
}

void DOMDocument::HandlePageLoadFinish()
{
    for (auto iter : domNodes_) {
        if (iter.second) {
            iter.second->OnPageLoadFinish();
        }
    }
}

void DOMDocument::SetUpRootComponent(const RefPtr<DOMNode>& node)
{
    LOGD("DOMDocument SetUpRootComponent");
    if (!node) {
        LOGE("fail to set up root component due to node is null");
        EventReport::SendComponentException(ComponentExcepType::SET_ROOT_DOM_NODE_ERR);
        return;
    }
    std::list<RefPtr<Component>> stackChildren { node->GetRootComponent() };
    rootStackComponent_ = AceType::MakeRefPtr<StackComponent>(
        Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, stackChildren);
    auto context = pipelineContext_.Upgrade();
    if (context && !context->IsFullScreenModal()) {
        rootStackComponent_->SetMainStackSize(MainStackSize::MIN);
        rootStackComponent_->SetStackFit(StackFit::FIRST_CHILD);
    } else {
        rootStackComponent_->SetMainStackSize(MainStackSize::MAX);
    }
    rootComposedStack_ =
        AceType::MakeRefPtr<ComposedComponent>(std::to_string(ROOT_STACK_ID - rootNodeId_), ROOT_STACK_NAME);
    rootComposedStack_->SetChild(rootStackComponent_);
}

} // namespace OHOS::Ace::Framework
