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

#include "js_accessibility_manager.h"

#include "accessibility_event_info.h"
#include "accessibility_system_ability_client.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/dom/dom_type.h"


using namespace OHOS::Accessibility;
using namespace std;

namespace OHOS::Ace::Framework {
namespace {
const int32_t EVENT_DUMP_PARAM_LENGTH = 3;
const int32_t PROPERTY_DUMP_PARAM_LENGTH = 2;
const char DUMP_ORDER[] = "-accessibility";
const char DUMP_INSPECTOR[] = "-inspector";
const char ACCESSIBILITY_FOCUSED_EVENT[] = "accessibilityfocus";
const char ACCESSIBILITY_CLEAR_FOCUS_EVENT[] = "accessibilityclearfocus";
const char TEXT_CHANGE_EVENT[] = "textchange";
const char PAGE_CHANGE_EVENT[] = "pagechange";
const char SCROLL_END_EVENT[] = "scrollend";
const char MOUSE_HOVER_ENTER[] = "mousehoverenter";
const char MOUSE_HOVER_EXIT[] = "mousehoverexit";
const char IMPORTANT_YES[] = "yes";
const char IMPORTANT_NO[] = "no";
const char IMPORTANT_NO_HIDE_DES[] = "no-hide-descendants";
constexpr int32_t DEFAULT_PARENT_ID = 2100000;
constexpr int32_t ROOT_STACK_BASE = 1100000;
constexpr int32_t CARD_NODE_ID_RATION = 10000;
constexpr int32_t CARD_ROOT_NODE_ID_RATION = 1000;
constexpr int32_t CARD_BASE = 100000;
constexpr int32_t WEIGHTED_VALUE = 13;

const int32_t FOCUS_DIRECTION_UP = 1;
const int32_t FOCUS_DIRECTION_DOWN = 1 << 1;
const int32_t FOCUS_DIRECTION_LEFT = 1 << 2;
const int32_t FOCUS_DIRECTION_RIGHT = 1 << 3;
const int32_t FOCUS_DIRECTION_FORWARD = 1 << 4;
const int32_t FOCUS_DIRECTION_BACKWARD = 1 << 5;

struct ActionTable {
    AceAction aceAction;
    ActionType action;
};

Accessibility::EventType ConvertStrToEventType(const std::string& type)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<Accessibility::EventType> eventTypeMap[] = {
        { ACCESSIBILITY_CLEAR_FOCUS_EVENT, Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT },
        { ACCESSIBILITY_FOCUSED_EVENT, Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT },
        { DOM_CLICK, Accessibility::EventType::TYPE_VIEW_CLICKED_EVENT },
        { DOM_FOCUS, Accessibility::EventType::TYPE_VIEW_FOCUSED_EVENT },
        { DOM_LONG_PRESS, Accessibility::EventType::TYPE_VIEW_LONG_CLICKED_EVENT },
        { MOUSE_HOVER_ENTER, Accessibility::EventType::TYPE_VIEW_HOVER_ENTER_EVENT },
        { MOUSE_HOVER_EXIT, Accessibility::EventType::TYPE_VIEW_HOVER_EXIT_EVENT },
        { PAGE_CHANGE_EVENT, Accessibility::EventType::TYPE_PAGE_STATE_UPDATE },
        { SCROLL_END_EVENT, Accessibility::EventType::TYPE_VIEW_SCROLLED_EVENT },
        { DOM_SELECTED, Accessibility::EventType::TYPE_VIEW_SELECTED_EVENT },
        { TEXT_CHANGE_EVENT, Accessibility::EventType::TYPE_VIEW_TEXT_UPDATE_EVENT },
        { DOM_TOUCH_END, Accessibility::EventType::TYPE_TOUCH_END },
        { DOM_TOUCH_START, Accessibility::EventType::TYPE_TOUCH_BEGIN },
    };
    Accessibility::EventType eventType = Accessibility::EventType::TYPE_VIEW_INVALID;
    int64_t idx = BinarySearchFindIndex(eventTypeMap, ArraySize(eventTypeMap), type.c_str());
    if (idx >= 0) {
        eventType = eventTypeMap[idx].value;
    }
    return eventType;
}

ActionType ConvertAceAction(AceAction aceAction)
{
    static const ActionTable actionTable[] = {
        { AceAction::ACTION_CLICK, ActionType::ACCESSIBILITY_ACTION_CLICK },
        { AceAction::ACTION_LONG_CLICK, ActionType::ACCESSIBILITY_ACTION_LONG_CLICK },
        { AceAction::ACTION_SCROLL_FORWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD },
        { AceAction::ACTION_SCROLL_BACKWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD },
        { AceAction::ACTION_FOCUS, ActionType::ACCESSIBILITY_ACTION_FOCUS },
        { AceAction::ACTION_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT },
        { AceAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT },
    };
    for (const auto& item : actionTable) {
        if (aceAction == item.aceAction) {
            return item.action;
        }
    }
    return ActionType::ACCESSIBILITY_ACTION_INVALID;
}

inline RangeInfo ConvertAccessibilityValue(const AccessibilityValue& value)
{
    return RangeInfo(static_cast<int>(value.min), static_cast<int>(value.max), static_cast<int>(value.current));
}

int32_t ConvertToCardAccessibilityId(int32_t nodeId, int32_t cardId, int32_t rootNodeId)
{
    // result is integer total ten digits, top five for agp virtualViewId, end five for ace nodeId,
    // for example agp virtualViewId is 32, ace nodeId is 1000001, convert to result is 00032 10001.
    int32_t result = 0;
    if (nodeId == rootNodeId + ROOT_STACK_BASE) {
        // for example agp virtualViewId is 32 root node is 2100000, convert to result is 00032 21000.
        result = cardId * CARD_BASE + (static_cast<int32_t>(nodeId / CARD_BASE)) * CARD_ROOT_NODE_ID_RATION +
                 nodeId % CARD_BASE;
    } else {
        result = cardId * CARD_BASE + (static_cast<int32_t>(nodeId / DOM_ROOT_NODE_ID_BASE)) * CARD_NODE_ID_RATION +
                 nodeId % DOM_ROOT_NODE_ID_BASE;
    }
    return result;
}

void UpdateAccessibilityNodeInfo(const RefPtr<AccessibilityNode>& node, AccessibilityElementInfo& nodeInfo,
    const RefPtr<JsAccessibilityManager>& manager, int windowId, NodeId rootId)
{
    LOGI("UpdateAccessibilityNodeInfo nodeId:%{public}d", node->GetNodeId());
    int leftTopX = static_cast<int>(node->GetLeft()) + SystemProperties::GetWindowPosX();
    int leftTopY = static_cast<int>(node->GetTop()) + SystemProperties::GetWindowPosY();
    int rightBottomX = leftTopX + static_cast<int>(node->GetWidth());
    int rightBottomY = leftTopY + static_cast<int>(node->GetHeight());
    if (manager->isOhosHostCard()) {
        int id = static_cast<int>(
            ConvertToCardAccessibilityId(node->GetNodeId(), manager->GetCardId(), manager->GetRootNodeId()));
        nodeInfo.SetAccessibilityId(id);
        if (node->GetParentId() == -1) {
            nodeInfo.SetParent(-1);
        } else {
            nodeInfo.SetParent(static_cast<int>(
                ConvertToCardAccessibilityId(node->GetParentId(), manager->GetCardId(), manager->GetRootNodeId())));
        }
        leftTopX = static_cast<int>(node->GetLeft() + manager->GetCardOffset().GetX());
        leftTopY = static_cast<int>(node->GetTop() + manager->GetCardOffset().GetY());
        rightBottomX = leftTopX + static_cast<int>(node->GetWidth());
        rightBottomY = leftTopY + static_cast<int>(node->GetHeight());
        Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
        nodeInfo.SetRectInScreen(bounds);
    } else {
        Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
        nodeInfo.SetRectInScreen(bounds);
        nodeInfo.SetComponentId(static_cast<int>(node->GetNodeId()));
        nodeInfo.SetParent(static_cast<int>(node->GetParentId()));
    }

    if (node->GetParentId() == -1) {
        const auto& children = node->GetChildList();
        if (!children.empty()) {
            auto lastChildNode = manager->GetAccessibilityNodeById(children.back()->GetNodeId());
            if (lastChildNode) {
                rightBottomX = leftTopX + static_cast<int>(lastChildNode->GetWidth());
                rightBottomY = leftTopY + static_cast<int>(lastChildNode->GetHeight());
                Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
                nodeInfo.SetRectInScreen(bounds);
            }
        }
    } else {
        LOGI("ACE::ParentID is %{public}d.", node->GetParentId());
    }
    if (node->GetNodeId() == 0) {
        nodeInfo.SetParent(-1);
    }
    nodeInfo.SetWindowId(windowId);
    nodeInfo.SetChecked(node->GetCheckedState());
    nodeInfo.SetEnabled(node->GetEnabledState());
    nodeInfo.SetFocused(node->GetFocusedState());
    nodeInfo.SetSelected(node->GetSelectedState());
    nodeInfo.SetCheckable(node->GetCheckableState());
    nodeInfo.SetClickable(node->GetClickableState());
    nodeInfo.SetFocusable(node->GetFocusableState());
    nodeInfo.SetScrollable(node->GetScrollableState());
    nodeInfo.SetLongClickable(node->GetLongClickableState());
    nodeInfo.SetEditable(node->GetEditable());
    nodeInfo.SetPluraLineSupported(node->GetIsMultiLine());
    nodeInfo.SetPassword(node->GetIsPassword());
    nodeInfo.SetTextLengthLimit(node->GetMaxTextLength());
    nodeInfo.SetSelectedBegin(static_cast<int>(node->GetTextSelectionStart()));
    nodeInfo.SetSelectedEnd(static_cast<int>(node->GetTextSelectionEnd()));
    nodeInfo.SetVisible(node->GetShown() && node->GetVisible() && (node->GetWidth() != 0 && node->GetHeight() != 0));
    nodeInfo.SetHint(node->GetHintText());
    std::string accessibilityLabel = node->GetAccessibilityLabel();
    nodeInfo.SetLabeled(atoi(accessibilityLabel.c_str()));
    nodeInfo.SetError(node->GetErrorText());
    nodeInfo.SetComponentResourceId(node->GetJsComponentId());
    nodeInfo.SetInspectorKey(node->GetJsComponentId());
    RangeInfo rangeInfo = ConvertAccessibilityValue(node->GetAccessibilityValue());
    nodeInfo.SetRange(rangeInfo);
    nodeInfo.SetInputType(static_cast<int>(node->GetTextInputType()));
    nodeInfo.SetComponentType(node->GetTag());
    GridInfo gridInfo(static_cast<int>(node->GetCollectionInfo().rows),
        static_cast<int>(node->GetCollectionInfo().columns), (nodeInfo.IsPluraLineSupported() ? 0 : 1));
    nodeInfo.SetGrid(gridInfo);

    int row = static_cast<int>(node->GetCollectionItemInfo().row);
    int column = static_cast<int>(node->GetCollectionItemInfo().column);
    GridItemInfo gridItemInfo(row, row, column, column, false, nodeInfo.IsSelected());
    nodeInfo.SetGridItem(gridItemInfo);
    nodeInfo.SetBundleName(AceApplicationInfo::GetInstance().GetPackageName());

    if (node->GetIsPassword()) {
        std::string strStar(node->GetText().size(), '*');
        nodeInfo.SetContent(strStar);
    } else {
        nodeInfo.SetContent(node->GetText());
    }

    if (!node->GetAccessibilityHint().empty()) {
        if (node->GetAccessibilityLabel().empty()) {
            LOGI("UpdateAccessibilityNodeInfo Label is null");
        } else {
            LOGI("UpdateAccessibilityNodeInfo Label is not null");
        }
    }

    auto supportAceActions = node->GetSupportAction();
    std::vector<ActionType> actions(supportAceActions.size());

    for (auto it = supportAceActions.begin(); it != supportAceActions.end(); ++it) {
        AccessibleAction action(ConvertAceAction(*it), "ace");
        nodeInfo.AddAction(action);
        LOGI("UpdateAccessibilityNodeInfo Action:%{public}d", ConvertAceAction(*it));
    }

    if (node->GetImportantForAccessibility() == IMPORTANT_YES) {
        actions.emplace_back(ActionType::ACCESSIBILITY_ACTION_FOCUS);
        nodeInfo.SetCheckable(true);
    } else if (node->GetImportantForAccessibility() == IMPORTANT_NO ||
               node->GetImportantForAccessibility() == IMPORTANT_NO_HIDE_DES) {
        nodeInfo.SetVisible(false);
    }

    std::vector<int32_t> childIds = node->GetChildIds();
    for (size_t i = 0; i < childIds.size(); ++i) {
        nodeInfo.AddChild(childIds[i]);
    }

#ifdef ACE_DEBUG
    std::string actionForLog;
    for (const auto& action : supportAceActions) {
        if (!actionForLog.empty()) {
            actionForLog.append(",");
        }
        actionForLog.append(std::to_string(static_cast<int32_t>(action)));
    }
    LOGD("Support action is %{public}s", actionForLog.c_str());
#endif
}

inline std::string BoolToString(bool tag)
{
    return tag ? "true" : "false";
}

std::string ConvertInputTypeToString(AceTextCategory type)
{
    switch (type) {
        case AceTextCategory::INPUT_TYPE_DEFAULT:
            return "INPUT_TYPE_DEFAULT";
        case AceTextCategory::INPUT_TYPE_TEXT:
            return "INPUT_TYPE_TEXT";
        case AceTextCategory::INPUT_TYPE_EMAIL:
            return "INPUT_TYPE_EMAIL";
        case AceTextCategory::INPUT_TYPE_DATE:
            return "INPUT_TYPE_DATE";
        case AceTextCategory::INPUT_TYPE_TIME:
            return "INPUT_TYPE_TIME";
        case AceTextCategory::INPUT_TYPE_NUMBER:
            return "INPUT_TYPE_NUMBER";
        case AceTextCategory::INPUT_TYPE_PASSWORD:
            return "INPUT_TYPE_PASSWORD";
        default:
            return "illegal input type";
    }
}

bool FindFocus(const RefPtr<AccessibilityNode>& node, RefPtr<AccessibilityNode>& resultNode)
{
    LOGI("FindFocus nodeId(%{public}d) focus(%{public}d)", node->GetNodeId(), node->GetFocusedState());
    if (node->GetAccessibilityFocusedState()) {
        resultNode = node;
        LOGI("FindFocus nodeId(%{public}d)", resultNode->GetNodeId());
        return true;
    }
    if (!node->GetChildList().empty()) {
        for (const auto& item : node->GetChildList()) {
            if (resultNode != nullptr) {
                return true;
            }
            if (FindFocus(item, resultNode)) {
                LOGI("FindFocus nodeId:%{public}d", resultNode->GetNodeId());
                return true;
            }
        }
    }

    return false;
}

void FindText(
    const RefPtr<AccessibilityNode>& node, const std::string& text, std::list<RefPtr<AccessibilityNode>>& nodeList)
{
    LOGI("FindText nodeId(%{public}d)", node->GetNodeId());
    if ((node != nullptr) && (node->GetText().find(text) != std::string::npos)) {
        LOGI("FindText find nodeId(%{public}d)", node->GetNodeId());
        nodeList.push_back(node);
    }
    if (!node->GetChildList().empty()) {
        for (const auto& child : node->GetChildList()) {
            FindText(child, text, nodeList);
        }
    }
}

} // namespace

JsAccessibilityManager::~JsAccessibilityManager()
{
    auto eventType = AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED;

    UnsubscribeStateObserver(eventType);

    DeregisterInteractionOperation();
}

bool JsAccessibilityManager::SubscribeStateObserver(const int eventType)
{
    LOGI("JsAccessibilityManager::SubscribeStateObserver");
    if (!stateObserver_) {
        stateObserver_ = std::make_shared<JsAccessibilityStateObserver>();
    }

    stateObserver_->SetHandler(WeakClaim(this));

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance == nullptr) {
        return false;
    }

    bool ret = instance->SubscribeStateObserver(stateObserver_, eventType);
    LOGI("JsAccessibilityManager::SubscribeStateObserver:%{public}d", ret);
    return ret;
}

bool JsAccessibilityManager::UnsubscribeStateObserver(const int eventType)
{
    LOGI("JsAccessibilityManager::UnsubscribeStateObserver");
    if (!stateObserver_) {
        return false;
    }

    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance == nullptr) {
        return false;
    }

    bool ret = instance->UnsubscribeStateObserver(stateObserver_, eventType);
    LOGI("JsAccessibilityManager::UnsubscribeStateObserver:%{public}d", ret);
    return ret;
}

void JsAccessibilityManager::InitializeCallback()
{
    LOGI("JsAccessibilityManager::InitializeCallback");
    if (IsRegister()) {
        return;
    }

    auto client = AccessibilitySystemAbilityClient::GetInstance();
    if (!client) {
        return;
    }
    AceApplicationInfo::GetInstance().SetAccessibilityEnabled(client->IsEnabled());

    SubscribeStateObserver(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);

    auto pipelineContext = GetPipelineContext().Upgrade();
    if (!pipelineContext) {
        return;
    }

    RegisterInteractionOperation(pipelineContext->GetWindowId());
}

bool JsAccessibilityManager::SendAccessibilitySyncEvent(const AccessibilityEvent& accessibilityEvent)
{
    LOGI("JsAccessibilityManager::SendAccessibilitySyncEvent type:%{public}s nodeId:%{public}d",
        accessibilityEvent.eventType.c_str(), accessibilityEvent.nodeId);
    if (!IsRegister()) {
        return false;
    }

    auto client = AccessibilitySystemAbilityClient::GetInstance();
    if (!client) {
        return false;
    }
    if (!client->IsEnabled()) {
        return false;
    }

    Accessibility::EventType type = ConvertStrToEventType(accessibilityEvent.eventType);
    if (type == Accessibility::EventType::TYPE_VIEW_INVALID) {
        return false;
    }

    auto pipelineContext = GetPipelineContext().Upgrade();
    if (!pipelineContext) {
        return false;
    }
    int32_t windowId = pipelineContext->GetWindowId();
    if (windowId == 0) {
        return false;
    }

    auto node = GetAccessibilityNodeFromPage(accessibilityEvent.nodeId);
    if (!node) {
        LOGW("can't attach component by Id = %{public}d", accessibilityEvent.nodeId);
        return false;
    }

    AccessibilityEventInfo eventInfo;
    eventInfo.SetWindowId(windowId);
    eventInfo.SetSource(accessibilityEvent.nodeId);
    eventInfo.SetEventType(type);
    eventInfo.SetComponentType(node->GetTag());
    eventInfo.SetCurrentIndex(static_cast<int>(accessibilityEvent.currentItemIndex));
    eventInfo.SetItemCounts(static_cast<int>(accessibilityEvent.itemCount));
    eventInfo.SetBundleName(AceApplicationInfo::GetInstance().GetPackageName());
    eventInfo.SetPageId(node->GetPageId());
    eventInfo.AddContent(node->GetText());
    eventInfo.SetLatestContent(node->GetText());

    LOGI("SendAccessibilitySyncEvent windowId:%{public}d", windowId);
    return client->SendEvent(eventInfo);
}

void JsAccessibilityManager::SendAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent)
{
    LOGI("SendAccessibilitySyncEvent eventType:%{public}s", accessibilityEvent.eventType.c_str());
    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        return;
    }

    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), accessibilityEvent] {
            auto jsAccessibilityManager = weak.Upgrade();
            if (!jsAccessibilityManager) {
                return;
            }
            jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent);
        },
        TaskExecutor::TaskType::BACKGROUND);
}

void JsAccessibilityManager::UpdateNodeChildIds(const RefPtr<AccessibilityNode>& node)
{
    if (!node) {
        return;
    }
    LOGI("JsAccessibilityManager::UpdateNodeChildIds");
    node->ActionUpdateIds();
    const auto& children = node->GetChildList();
    std::vector<int32_t> childrenVec;
    auto cardId = GetCardId();
    auto rootNodeId = GetRootNodeId();

    // get last stack children to barrier free service.
    if ((node->GetNodeId() == GetRootNodeId() + ROOT_STACK_BASE) && !children.empty() && !IsDeclarative()) {
        auto lastChildNodeId = children.back()->GetNodeId();
        if (isOhosHostCard()) {
            childrenVec.emplace_back(ConvertToCardAccessibilityId(lastChildNodeId, cardId, rootNodeId));
        } else {
            childrenVec.emplace_back(lastChildNodeId);
        }
    } else {
        childrenVec.resize(children.size());
        if (isOhosHostCard()) {
            std::transform(children.begin(), children.end(), childrenVec.begin(),
                [cardId, rootNodeId](const RefPtr<AccessibilityNode>& child) {
                    return ConvertToCardAccessibilityId(child->GetNodeId(), cardId, rootNodeId);
                });
        } else {
            std::transform(children.begin(), children.end(), childrenVec.begin(),
                [](const RefPtr<AccessibilityNode>& child) { return child->GetNodeId(); });
        }
    }
    node->SetChildIds(childrenVec);
}

void JsAccessibilityManager::DumpHandleEvent(const std::vector<std::string>& params)
{
    if (params.empty()) {
        DumpLog::GetInstance().Print("Error: params is empty!");
        return;
    }
    if (params.size() != EVENT_DUMP_PARAM_LENGTH) {
        DumpLog::GetInstance().Print("Error: params length is illegal!");
        return;
    }
    if (params[0] != DUMP_ORDER && params[0] != DUMP_INSPECTOR) {
        DumpLog::GetInstance().Print("Error: not accessibility dump order!");
        return;
    }
    int32_t ID = StringUtils::StringToInt(params[1]);
    auto node = GetAccessibilityNodeFromPage(ID);
    if (!node) {
        DumpLog::GetInstance().Print("Error: can't find node with ID");
        return;
    }
    auto action = static_cast<AceAction>(StringUtils::StringToInt(params[2]));
    static const ActionTable actionEventTable[] = {
        { AceAction::ACTION_CLICK, ActionType::ACCESSIBILITY_ACTION_CLICK },
        { AceAction::ACTION_LONG_CLICK, ActionType::ACCESSIBILITY_ACTION_LONG_CLICK },
        { AceAction::ACTION_SCROLL_FORWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD },
        { AceAction::ACTION_SCROLL_BACKWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD },
        { AceAction::ACTION_FOCUS, ActionType::ACCESSIBILITY_ACTION_FOCUS },
        { AceAction::ACTION_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS },
    };
    ActionType op = ActionType::ACCESSIBILITY_ACTION_INVALID;
    for (const auto& item : actionEventTable) {
        if (action == item.aceAction) {
            op = item.action;
            break;
        }
    }
    auto context = GetPipelineContext().Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this), op, node, context]() {
                auto jsAccessibilityManager = weak.Upgrade();
                if (!jsAccessibilityManager) {
                    return;
                }
                jsAccessibilityManager->AccessibilityActionEvent(op, {}, node, context);
            },
            TaskExecutor::TaskType::UI);
    }
}

void JsAccessibilityManager::DumpProperty(const std::vector<std::string>& params)
{
    if (!DumpLog::GetInstance().GetDumpFile()) {
        return;
    }

    if (params.empty()) {
        DumpLog::GetInstance().Print("Error: params cannot be empty!");
        return;
    }
    if (params.size() != PROPERTY_DUMP_PARAM_LENGTH) {
        DumpLog::GetInstance().Print("Error: params length is illegal!");
        return;
    }
    if (params[0] != DUMP_ORDER && params[0] != DUMP_INSPECTOR) {
        DumpLog::GetInstance().Print("Error: not accessibility dump order!");
        return;
    }

    auto node = GetAccessibilityNodeFromPage(StringUtils::StringToInt(params[1]));
    if (!node) {
        DumpLog::GetInstance().Print("Error: can't find node with ID " + params[1]);
        return;
    }

    const auto& supportAceActions = node->GetSupportAction();
    std::string actionForDump;
    for (const auto& action : supportAceActions) {
        if (!actionForDump.empty()) {
            actionForDump.append(",");
        }
        actionForDump.append(std::to_string(static_cast<int32_t>(action)));
    }

    const auto& charValue = node->GetChartValue();

    DumpLog::GetInstance().AddDesc("ID: ", node->GetNodeId());
    DumpLog::GetInstance().AddDesc("parent ID: ", node->GetParentId());
    DumpLog::GetInstance().AddDesc("child IDs: ", GetNodeChildIds(node));
    DumpLog::GetInstance().AddDesc("component type: ", node->GetTag());
    DumpLog::GetInstance().AddDesc("input type: ", node->GetInputType());
    DumpLog::GetInstance().AddDesc("text: ", node->GetText());
    DumpLog::GetInstance().AddDesc("width: ", node->GetWidth());
    DumpLog::GetInstance().AddDesc("height: ", node->GetHeight());
    DumpLog::GetInstance().AddDesc("left: ", node->GetLeft() + GetCardOffset().GetX());
    DumpLog::GetInstance().AddDesc("top: ", node->GetTop() + GetCardOffset().GetY());
    DumpLog::GetInstance().AddDesc("enabled: ", BoolToString(node->GetEnabledState()));
    DumpLog::GetInstance().AddDesc("checked: ", BoolToString(node->GetCheckedState()));
    DumpLog::GetInstance().AddDesc("selected: ", BoolToString(node->GetSelectedState()));
    DumpLog::GetInstance().AddDesc("focusable: ", BoolToString(node->GetFocusableState()));
    DumpLog::GetInstance().AddDesc("focused: ", BoolToString(node->GetFocusedState()));
    DumpLog::GetInstance().AddDesc("checkable: ", BoolToString(node->GetCheckableState()));
    DumpLog::GetInstance().AddDesc("clickable: ", BoolToString(node->GetClickableState()));
    DumpLog::GetInstance().AddDesc("long clickable: ", BoolToString(node->GetLongClickableState()));
    DumpLog::GetInstance().AddDesc("scrollable: ", BoolToString(node->GetScrollableState()));
    DumpLog::GetInstance().AddDesc("editable: ", BoolToString(node->GetEditable()));
    DumpLog::GetInstance().AddDesc("hint text: ", node->GetHintText());
    DumpLog::GetInstance().AddDesc("error text: ", node->GetErrorText());
    DumpLog::GetInstance().AddDesc("js component id: ", node->GetJsComponentId());
    DumpLog::GetInstance().AddDesc("accessibility label: ", node->GetAccessibilityLabel());
    DumpLog::GetInstance().AddDesc("accessibility hint: ", node->GetAccessibilityHint());
    DumpLog::GetInstance().AddDesc("max text length: ", node->GetMaxTextLength());
    DumpLog::GetInstance().AddDesc("text selection start: ", node->GetTextSelectionStart());
    DumpLog::GetInstance().AddDesc("text selection end: ", node->GetTextSelectionEnd());
    DumpLog::GetInstance().AddDesc("is multi line: ", BoolToString(node->GetIsMultiLine()));
    DumpLog::GetInstance().AddDesc("is password", BoolToString(node->GetIsPassword()));
    DumpLog::GetInstance().AddDesc("text input type: ", ConvertInputTypeToString(node->GetTextInputType()));
    DumpLog::GetInstance().AddDesc("min value: ", node->GetAccessibilityValue().min);
    DumpLog::GetInstance().AddDesc("max value: ", node->GetAccessibilityValue().max);
    DumpLog::GetInstance().AddDesc("current value: ", node->GetAccessibilityValue().current);
    DumpLog::GetInstance().AddDesc("collection info rows: ", node->GetCollectionInfo().rows);
    DumpLog::GetInstance().AddDesc("collection info columns: ", node->GetCollectionInfo().columns);
    DumpLog::GetInstance().AddDesc("collection item info, row: ", node->GetCollectionItemInfo().row);
    DumpLog::GetInstance().AddDesc("collection item info, column: ", node->GetCollectionItemInfo().column);
    DumpLog::GetInstance().AddDesc("chart has value: ", BoolToString(charValue && !charValue->empty()));
    DumpLog::GetInstance().AddDesc("accessibilityGroup: ", BoolToString(node->GetAccessible()));
    DumpLog::GetInstance().AddDesc("accessibilityImportance: ", node->GetImportantForAccessibility());
    DumpLog::GetInstance().AddDesc("support action: ", actionForDump);

    DumpLog::GetInstance().Print(0, node->GetTag(), node->GetChildList().size());
}

void JsAccessibilityManager::SetCardViewParams(const std::string& key, bool focus)
{
    LOGD("SetCardViewParams key=%{public}s  focus=%{public}d", key.c_str(), focus);
    callbackKey_ = key;
    if (!callbackKey_.empty()) {
        InitializeCallback();
    }
}

void JsAccessibilityManager::UpdateViewScale()
{
    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        return;
    }
    float scaleX = 1.0;
    float scaleY = 1.0;
    if (context->GetViewScale(scaleX, scaleY)) {
        scaleX_ = scaleX;
        scaleY_ = scaleY;
    }
}

void JsAccessibilityManager::HandleComponentPostBinding()
{
    for (auto targetIter = nodeWithTargetMap_.begin(); targetIter != nodeWithTargetMap_.end();) {
        auto nodeWithTarget = targetIter->second.Upgrade();
        if (nodeWithTarget) {
            if (nodeWithTarget->GetTag() == ACCESSIBILITY_TAG_POPUP) {
                auto idNodeIter = nodeWithIdMap_.find(targetIter->first);
                if (idNodeIter != nodeWithIdMap_.end()) {
                    auto nodeWithId = idNodeIter->second.Upgrade();
                    if (nodeWithId) {
                        nodeWithId->SetAccessibilityHint(nodeWithTarget->GetText());
                    } else {
                        nodeWithIdMap_.erase(idNodeIter);
                    }
                }
            }
            ++targetIter;
        } else {
            // clear the disabled node in the maps
            nodeWithTargetMap_.erase(targetIter++);
        }
    }

    // clear the disabled node in the maps
    for (auto idItem = nodeWithIdMap_.begin(); idItem != nodeWithIdMap_.end();) {
        if (!idItem->second.Upgrade()) {
            nodeWithIdMap_.erase(idItem++);
        } else {
            ++idItem;
        }
    }
}

RefPtr<AccessibilityNodeManager> AccessibilityNodeManager::Create()
{
    return AceType::MakeRefPtr<JsAccessibilityManager>();
}

void JsAccessibilityManager::JsInteractionOperation::SearchElementInfoByAccessibilityId(
    const long elementId, const int requestId, AccessibilityElementOperatorCallback& callback, const int mode)
{
    LOGI("JsAccessibilityManager::SearchElementInfoByAccessibilityId elementId(%{public}ld)", elementId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [jsAccessibilityManager, elementId, requestId, &callback, mode]() {
                if (!jsAccessibilityManager) {
                    std::list<AccessibilityElementInfo> infos;
                    AccessibilityElementInfo nodeInfo;
                    infos.push_back(nodeInfo);
                    callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
                    return;
                }
                jsAccessibilityManager->SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
            },
            TaskExecutor::TaskType::UI);
    }
}

void JsAccessibilityManager::SearchElementInfoByAccessibilityId(
    const long elementId, const int requestId, AccessibilityElementOperatorCallback& callback, const int mode)
{
    LOGI("SearchElementInfoByAccessibilityId elementId(%{public}ld) mode(%{public}d)", elementId, mode);
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    NodeId nodeId = static_cast<NodeId>(elementId);
    // get root node
    if (elementId == -1) {
        nodeId = 0;
    }
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        LOGW("AccessibilityNodeInfo can't attach component by Id = %{public}d", nodeId);
        return;
    }
    LOGI("SearchElementInfoByAccessibilityId nodeId(%{public}d)", node->GetNodeId());
    AccessibilityElementInfo nodeInfo;
    std::list<AccessibilityElementInfo> infos;
    jsAccessibilityManager->UpdateNodeChildIds(node);
    UpdateAccessibilityNodeInfo(node, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_,
        jsAccessibilityManager->GetRootNodeId());
    infos.push_back(nodeInfo);

    // parent and me.
    RefPtr<AccessibilityNode> parentNode;
    uint32_t umode = mode;
    if (umode & static_cast<uint32_t>(PREFETCH_PREDECESSORS)) {
        if (node->GetParentId() != -1 && node->GetParentId() != DEFAULT_PARENT_ID) {
            auto parentNode = node->GetParentNode();
            UpdateAccessibilityNodeInfo(parentNode, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_,
                jsAccessibilityManager->GetRootNodeId());
            infos.push_back(nodeInfo);

            // sister/brothers.
            if (umode & static_cast<uint32_t>(PREFETCH_SIBLINGS)) {
                auto childs = parentNode->GetChildList();
                for (const auto& item : node->GetChildList()) {
                    UpdateAccessibilityNodeInfo(item, nodeInfo, jsAccessibilityManager,
                        jsAccessibilityManager->windowId_, jsAccessibilityManager->GetRootNodeId());
                    infos.push_back(nodeInfo);
                }
            }
        }
    }
    // childs.
    if (umode & static_cast<uint32_t>(PREFETCH_CHILDREN)) {
        auto childs = node->GetChildList();
        for (const auto& item : node->GetChildList()) {
            UpdateAccessibilityNodeInfo(item, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_,
                jsAccessibilityManager->GetRootNodeId());
            infos.push_back(nodeInfo);
        }
    }

    LOGI("SetSearchElementInfoByAccessibilityIdResult");
    callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
}

void JsAccessibilityManager::JsInteractionOperation::SearchElementInfosByText(const long elementId,
    const std::string& text, const int requestId, AccessibilityElementOperatorCallback& callback)
{
    LOGI("JsAccessibilityManager::SearchElementInfosByText elementId(%{public}ld)", elementId);
    if (text.empty()) {
        LOGW("Text is null");
        return;
    }
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [jsAccessibilityManager, elementId, text, requestId, &callback]() {
                AccessibilityElementInfo nodeInfo;
                if (!jsAccessibilityManager) {
                    std::list<AccessibilityElementInfo> infos;
                    AccessibilityElementInfo nodeInfo;
                    infos.push_back(nodeInfo);
                    callback.SetSearchElementInfoByTextResult(infos, requestId);
                    return;
                }

                jsAccessibilityManager->SearchElementInfosByText(elementId, text, requestId, callback);
            },
            TaskExecutor::TaskType::UI);
    }
}

void JsAccessibilityManager::SearchElementInfosByText(const long elementId, const std::string& text,
    const int requestId, AccessibilityElementOperatorCallback& callback)
{
    LOGI("SearchElementInfosByText text(%{public}s)", text.c_str());
    if (text.empty()) {
        LOGW("Text is null");
        return;
    }
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    NodeId nodeId = static_cast<NodeId>(elementId);
    if (elementId == -1) {
        return;
    }

    LOGI("SearchElementInfosByText nodeId(%{public}d)", nodeId);
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        return;
    }

    std::list<AccessibilityElementInfo> infos;
    std::list<RefPtr<AccessibilityNode>> nodeList;
    FindText(node, text, nodeList);
    if (!nodeList.empty()) {
        for (const auto& node : nodeList) {
            LOGI(" FindText end nodeId:%{public}d", node->GetNodeId());
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityNodeInfo(node, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_,
                jsAccessibilityManager->GetRootNodeId());
            infos.emplace_back(nodeInfo);
        }
    }

    LOGI("SetSearchElementInfoByTextResult infos.size(%{public}d)", infos.size());
    callback.SetSearchElementInfoByTextResult(infos, requestId);
}

void JsAccessibilityManager::JsInteractionOperation::FindFocusedElementInfo(
    const long elementId, const int focusType, const int requestId, AccessibilityElementOperatorCallback& callback)
{
    LOGI("JsAccessibilityManager::FindFocusedElementInfo elementId(%{public}ld)", elementId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [jsAccessibilityManager, elementId, focusType, requestId, &callback]() {
                if (!jsAccessibilityManager) {
                    AccessibilityElementInfo nodeInfo;
                    nodeInfo.SetValidElement(false);
                    callback.SetFindFocusedElementInfoResult(nodeInfo, requestId);
                    return;
                }
                jsAccessibilityManager->FindFocusedElementInfo(elementId, focusType, requestId, callback);
            },
            TaskExecutor::TaskType::UI);
    }
}

void JsAccessibilityManager::FindFocusedElementInfo(
    const long elementId, const int focusType, const int requestId, AccessibilityElementOperatorCallback& callback)
{
    LOGI("FindFocusedElementInfo elementId(%{public}ld)", elementId);
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    if (!jsAccessibilityManager) {
        LOGI("FindFocusedElementInfo jsAccessibilityManager is null");
        return;
    }

    AccessibilityElementInfo nodeInfo;
    NodeId nodeId = static_cast<NodeId>(elementId);
    if (elementId == -1) {
        nodeId = jsAccessibilityManager->GetRootNodeId();
        nodeId = 0;
    }

    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        nodeInfo.SetValidElement(false);
        callback.SetFindFocusedElementInfoResult(nodeInfo, requestId);
        return;
    }

    RefPtr<AccessibilityNode> resultNode = nullptr;
    bool status = FindFocus(node, resultNode);
    LOGI("FindFocus status(%{public}d)", status);
    if ((status) && (resultNode != nullptr)) {
        LOGI("FindFocus nodeId:%{public}d", resultNode->GetNodeId());    
        UpdateAccessibilityNodeInfo(resultNode, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_,
            jsAccessibilityManager->GetRootNodeId());
    }

    LOGI("SetFindFocusedElementInfoResult");
    callback.SetFindFocusedElementInfoResult(nodeInfo, requestId);
}

void JsAccessibilityManager::JsInteractionOperation::ExecuteAction(const long elementId, const int action,
    const std::map<std::string, std::string> actionArguments, const int requestId,
    AccessibilityElementOperatorCallback& callback)
{
    LOGI("JsAccessibilityManager::ExecuteAction, id:%{public}ld, action:%{public}d, request:%{public}d.", 
        elementId, action, requestId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        return;
    }

    ActionType actionInfo = static_cast<ActionType>(action);
    context->GetTaskExecutor()->PostTask(
        [jsAccessibilityManager, elementId, actionInfo, actionArguments, requestId, &callback] {
            if (!jsAccessibilityManager) {
                return;
            }
            jsAccessibilityManager->ExecuteAction(elementId, actionInfo, actionArguments, requestId, callback);
        },
        TaskExecutor::TaskType::UI);
}

bool JsAccessibilityManager::AccessibilityActionEvent(const ActionType& action,
    const std::map<std::string, std::string> actionArguments, const RefPtr<AccessibilityNode>& node,
    const RefPtr<PipelineContext>& context)
{
    LOGI("JsAccessibilityManager::AccessibilityActionEvent");
    if (!node || !context) {
        return false;
    }
    switch (action) {
        case ActionType::ACCESSIBILITY_ACTION_CLICK: {
            node->SetClicked(true);
            if (!node->GetClickEventMarker().IsEmpty()) {
                context->SendEventToFrontend(node->GetClickEventMarker());
                node->ActionClick();
                return true;
            }
            return node->ActionClick();
        }
        case ActionType::ACCESSIBILITY_ACTION_LONG_CLICK: {
            if (!node->GetLongPressEventMarker().IsEmpty()) {
                context->SendEventToFrontend(node->GetLongPressEventMarker());
                node->ActionLongClick();
                return true;
            }
            return node->ActionLongClick();
        }
        case ActionType::ACCESSIBILITY_ACTION_FOCUS: {
            context->AccessibilityRequestFocus(std::to_string(node->GetNodeId()));
            if (!node->GetFocusEventMarker().IsEmpty()) {
                context->SendEventToFrontend(node->GetFocusEventMarker());
                node->ActionFocus();
                return true;
            }
            return node->ActionFocus();
        }
        case ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS: {
            return RequestAccessibilityFocus(node);
        }
        case ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
            return ClearAccessibilityFocus(node);
        }
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD:
            return node->ActionScrollForward();
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD:
            return node->ActionScrollBackward();
        default:
            return false;
    }
}

void JsAccessibilityManager::ExecuteAction(const long elementId, const ActionType& action,
    const std::map<std::string, std::string> actionArguments, const int requestId,
    AccessibilityElementOperatorCallback& callback)
{
    LOGI("ExecuteAction action:%{public}d", action);
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();

    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage((NodeId)elementId);
    if (!node) {
        LOGW("AccessibilityNodeInfo can't attach component by Id = %{public}d", (NodeId)elementId);
        callback.SetExecuteActionResult(false, requestId);
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        callback.SetExecuteActionResult(false, requestId);
        return;
    }

    bool actionResult = AccessibilityActionEvent(action, actionArguments, node, context);

    LOGI("SetExecuteActionResult actionResult= %{public}d", actionResult);
    callback.SetExecuteActionResult(actionResult, requestId);
}

void JsAccessibilityManager::JsInteractionOperation::ClearFocus()
{
    LOGI("JsAccessibilityManager::ClearFocus");
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        return;
    }

    context->GetTaskExecutor()->PostTask(
        [jsAccessibilityManager] {
            if (!jsAccessibilityManager) {
                return;
            }
            jsAccessibilityManager->ClearCurrentFocus();
        },
        TaskExecutor::TaskType::UI);
}

void JsAccessibilityManager::JsInteractionOperation::OutsideTouch() {}

bool JsAccessibilityManager::IsRegister()
{
    return isReg_;
}

void JsAccessibilityManager::Register(bool state)
{
    isReg_ = state;
}

int JsAccessibilityManager::RegisterInteractionOperation(const int windowId)
{
    LOGI("JsAccessibilityManager::RegisterInteractionOperation windowId:%{public}d", windowId);
    if (IsRegister()) {
        return 0;
    }
    
    SetWindowId(windowId);
    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance == nullptr) {
        return (-1);
    }

    interactionOperation_ = std::make_shared<JsInteractionOperation>();
    interactionOperation_->SetHandler(WeakClaim(this));
    int32_t userId = AceApplicationInfo::GetInstance().GetUserId();
    int retReg = instance->RegisterElementOperator(windowId, interactionOperation_, userId);
    LOGI("RegisterInteractionOperation end windowId:%{public}d, ret:%{public}d", windowId, retReg);
    Register(!retReg);

    return retReg;
}

void JsAccessibilityManager::DeregisterInteractionOperation()
{
    LOGI("JsAccessibilityManager::DeregisterInteractionOperation");
    if (!IsRegister()) {
        return;
    }
    int windowId = GetWindowId();

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance == nullptr) {
        return;
    }
    Register(false);
    currentFocusNodeId_ = -1;
    LOGI("DeregisterInteractionOperation windowId:%{public}d", windowId);
    return instance->DeregisterElementOperator(windowId);
}

void JsAccessibilityManager::JsAccessibilityStateObserver::OnStateChanged(const bool state)
{
    LOGI("JsAccessibilityManager::OnStateChanged changed:%{public}d", state);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [jsAccessibilityManager, state]() {
                if (state) {
                    jsAccessibilityManager->RegisterInteractionOperation(jsAccessibilityManager->GetWindowId());
                } else {
                    jsAccessibilityManager->DeregisterInteractionOperation();
                }
                AceApplicationInfo::GetInstance().SetAccessibilityEnabled(state);
            },
            TaskExecutor::TaskType::UI);
    }
}

void JsAccessibilityManager::JsInteractionOperation::FocusMoveSearch(
    const long elementId, const int direction, const int requestId, AccessibilityElementOperatorCallback& callback)
{
    LOGI("JsAccessibilityManager::FocusMoveSearch elementId:%{public}ld", elementId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        return;
    }

    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        return;
    }

    context->GetTaskExecutor()->PostTask(
        [jsAccessibilityManager, elementId, direction, requestId, &callback] {
            if (!jsAccessibilityManager) {
                return;
            }
            jsAccessibilityManager->FocusMoveSearch(elementId, direction, requestId, callback);
        },
        TaskExecutor::TaskType::UI);
}

void JsAccessibilityManager::FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback)
{
    LOGI("FocusMoveSearch elementId:%{public}ld", elementId);
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    AccessibilityElementInfo nodeInfo;
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage((NodeId)elementId);
    if (!node) {
        LOGW("AccessibilityNodeInfo can't attach component by Id = %{public}d", (NodeId)elementId);
        callback.SetFocusMoveSearchResult(nodeInfo, requestId);
        return;
    }
    LOGI("FocusMoveSearch nodeId:%{public}d", node->GetNodeId());

    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        LOGI("FocusMoveSearch context is null");
        callback.SetFocusMoveSearchResult(nodeInfo, requestId);
        return;
    }

    // get root node.
    auto rootNode = node;
    while (rootNode->GetParentNode()) {
        rootNode = rootNode->GetParentNode();
        LOGI("FocusMoveSearch nodeId:%{public}d isRoot:%{public}d", rootNode->GetNodeId(), rootNode->IsRootNode());
        if (!rootNode->GetParentNode()) {
            break;
        }
    }

    std::list<RefPtr<AccessibilityNode>> nodeList;
    AddFocusableNode(nodeList, rootNode);
    RefPtr<AccessibilityNode> resultNode;

    switch (direction) {
        case FOCUS_DIRECTION_FORWARD:
        case FOCUS_DIRECTION_BACKWARD:
            // forward and backward
            resultNode = FindNodeInRelativeDirection(nodeList, node, direction);
            break;
        case FOCUS_DIRECTION_UP:
        case FOCUS_DIRECTION_DOWN:
        case FOCUS_DIRECTION_LEFT:
        case FOCUS_DIRECTION_RIGHT:
            // up, down, left and right
            resultNode = FindNodeInAbsoluteDirection(nodeList, node, direction);
            break;
        default:
            break;
    }

    if (resultNode) {
        LOGI("FocusMoveSearch end nodeId:%{public}d", resultNode->GetNodeId());
        jsAccessibilityManager->UpdateNodeChildIds(resultNode);
        UpdateAccessibilityNodeInfo(resultNode, nodeInfo, jsAccessibilityManager, windowId_, rootNode->GetNodeId());
    }
    LOGI("SetFocusMoveSearchResult");
    callback.SetFocusMoveSearchResult(nodeInfo, requestId);
}

void JsAccessibilityManager::AddFocusableNode(
    std::list<RefPtr<AccessibilityNode>>& nodeList, const RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::AddFocusableNode");
    const std::string importance = node->GetImportantForAccessibility();
    if (CanAccessibilityFocused(node)) {
        nodeList.push_back(node);
    }
    if (!node->GetAccessible() && importance != "no-hide-descendants") {
        for (auto& child : node->GetChildList()) {
            AddFocusableNode(nodeList, child);
        }
    }
}

bool JsAccessibilityManager::CanAccessibilityFocused(const RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::CanAccessibilityFocused");
    return node != nullptr && !node->IsRootNode() && node->GetVisible() &&
           node->GetImportantForAccessibility() != "no" &&
           node->GetImportantForAccessibility() != "no-hide-descendants";
}

RefPtr<AccessibilityNode> JsAccessibilityManager::FindNodeInRelativeDirection(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction)
{
    LOGI("JsAccessibilityManager::FindNodeInRelativeDirection");
    switch (direction) {
        case FOCUS_DIRECTION_FORWARD:
            return GetNextFocusableNode(nodeList, node);
        case FOCUS_DIRECTION_BACKWARD:
            return GetPreviousFocusableNode(nodeList, node);
        default:
            break;
    }

    return nullptr;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::FindNodeInAbsoluteDirection(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction)
{
    LOGI("JsAccessibilityManager::FindNodeInAbsoluteDirection");
    auto tempBest = node->GetRect();
    auto nodeRect = node->GetRect();

    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
            tempBest.SetLeft(node->GetLeft() + node->GetWidth() + 1);
            break;
        case FOCUS_DIRECTION_RIGHT:
            tempBest.SetLeft(node->GetLeft() - node->GetWidth() - 1);
            break;
        case FOCUS_DIRECTION_UP:
            tempBest.SetTop(node->GetTop() + node->GetHeight() + 1);
            break;
        case FOCUS_DIRECTION_DOWN:
            tempBest.SetTop(node->GetTop() + node->GetHeight() - 1);
            break;
        default:
            break;
    }

    RefPtr<AccessibilityNode> nearestNode = nullptr;
    for (auto nodeItem = nodeList.begin(); nodeItem != nodeList.end(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId() || (*nodeItem)->IsRootNode()) {
            continue;
        }
        auto itemRect = (*nodeItem)->GetRect();
        if (CheckBetterRect(nodeRect, direction, itemRect, tempBest)) {
            tempBest = itemRect;
            nearestNode = (*nodeItem);
        }
    }

    return nearestNode;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::GetNextFocusableNode(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::GetNextFocusableNode");
    auto nodeItem = nodeList.begin();
    for (; nodeItem != nodeList.end(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId()) {
            break;
        }
    }

    if (nodeItem != nodeList.end() && ++nodeItem != nodeList.end()) {
        return (*nodeItem);
    }
    if (nodeList.size() > 0) {
        return (*nodeList.begin());
    }

    return nullptr;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::GetPreviousFocusableNode(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::GetPreviousFocusableNode");
    auto nodeItem = nodeList.rbegin();
    for (; nodeItem != nodeList.rend(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId()) {
            break;
        }
    }

    if (nodeItem != nodeList.rend() && ++nodeItem != nodeList.rend()) {
        return (*nodeItem);
    }

    if (nodeList.size() > 0) {
        return (*nodeList.rbegin());
    }
    return nullptr;
}

bool JsAccessibilityManager::CheckBetterRect(Rect nodeRect, const int direction, Rect itemRect, Rect tempBest)
{
    if (!IsCandidateRect(nodeRect, itemRect, direction)) {
        return false;
    }

    if (!IsCandidateRect(nodeRect, tempBest, direction)) {
        return true;
    }

    // now both of item and tempBest are all at the direction of node.
    if (OutrightBetter(nodeRect, direction, itemRect, tempBest)) {
        return true;
    }

    if (OutrightBetter(nodeRect, direction, tempBest, itemRect)) {
        return false;
    }

    // otherwise, do fudge-tastic comparison of the major and minor axis
    return (GetWeightedDistanceFor(
                MajorAxisDistance(nodeRect, itemRect, direction), MinorAxisDistance(nodeRect, itemRect, direction)) <
            GetWeightedDistanceFor(
                MajorAxisDistance(nodeRect, tempBest, direction), MinorAxisDistance(nodeRect, tempBest, direction)));
}

bool JsAccessibilityManager::IsCandidateRect(Rect nodeRect, Rect itemRect, const int direction)
{
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
            return nodeRect.Left() > itemRect.Left() && nodeRect.Right() > itemRect.Right();
        case FOCUS_DIRECTION_RIGHT:
            return nodeRect.Left() < itemRect.Left() && nodeRect.Right() < itemRect.Right();
        case FOCUS_DIRECTION_UP:
            return nodeRect.Top() > itemRect.Top() && nodeRect.Bottom() > itemRect.Bottom();
        case FOCUS_DIRECTION_DOWN:
            return nodeRect.Top() < itemRect.Top() && nodeRect.Bottom() < itemRect.Bottom();
        default:
            break;
    }
    return false;
}

// Check whether rect1 is outright better than rect2.
bool JsAccessibilityManager::OutrightBetter(Rect nodeRect, const int direction, Rect Rect1, Rect Rect2)
{
    bool rect1InSrcBeam = CheckRectBeam(nodeRect, Rect1, direction);
    bool rect2InSrcBeam = CheckRectBeam(nodeRect, Rect2, direction);
    if (rect2InSrcBeam || !rect1InSrcBeam) {
        return false;
    }

    if (!IsToDirectionOf(nodeRect, Rect2, direction)) {
        return true;
    }

    // for direction left or right
    if (direction == FOCUS_DIRECTION_LEFT || direction == FOCUS_DIRECTION_RIGHT) {
        return true;
    }

    return (MajorAxisDistance(nodeRect, Rect1, direction) < MajorAxisDistanceToFarEdge(nodeRect, Rect2, direction));
}

bool JsAccessibilityManager::CheckRectBeam(Rect nodeRect, Rect itemRect, const int direction)
{
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
        case FOCUS_DIRECTION_RIGHT:
            return nodeRect.Top() < itemRect.Bottom() && itemRect.Top() < nodeRect.Bottom();
        case FOCUS_DIRECTION_UP:
        case FOCUS_DIRECTION_DOWN:
            return nodeRect.Left() < itemRect.Right() && itemRect.Left() < nodeRect.Right();
        default:
            break;
    }
    return false;
}

bool JsAccessibilityManager::IsToDirectionOf(Rect nodeRect, Rect itemRect, const int direction)
{
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
            return nodeRect.Left() >= itemRect.Right();
        case FOCUS_DIRECTION_RIGHT:
            return nodeRect.Right() <= itemRect.Left();
        case FOCUS_DIRECTION_UP:
            return nodeRect.Top() >= itemRect.Bottom();
        case FOCUS_DIRECTION_DOWN:
            return nodeRect.Bottom() <= itemRect.Top();
        default:
            break;
    }
    return false;
}

double JsAccessibilityManager::MajorAxisDistanceToFarEdge(Rect nodeRect, Rect itemRect, const int direction)
{
    double distance = 0.0;
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
            distance = nodeRect.Left() - itemRect.Left();
            break;
        case FOCUS_DIRECTION_RIGHT:
            distance = nodeRect.Right() - itemRect.Right();
            break;
        case FOCUS_DIRECTION_UP:
            distance = nodeRect.Top() - itemRect.Top();
            break;
        case FOCUS_DIRECTION_DOWN:
            distance = nodeRect.Bottom() - itemRect.Bottom();
            break;
        default:
            break;
    }

    return distance > 1.0 ? distance : 1.0;
}

double JsAccessibilityManager::MajorAxisDistance(Rect nodeRect, Rect itemRect, const int direction)
{
    double distance = 0.0;
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
            distance = nodeRect.Left() - itemRect.Right();
            break;
        case FOCUS_DIRECTION_RIGHT:
            distance = nodeRect.Right() - itemRect.Left();
            break;
        case FOCUS_DIRECTION_UP:
            distance = nodeRect.Top() - itemRect.Bottom();
            break;
        case FOCUS_DIRECTION_DOWN:
            distance = nodeRect.Bottom() - itemRect.Top();
            break;
        default:
            break;
    }

    return distance > 0.0 ? distance : 0.0;
}

double JsAccessibilityManager::MinorAxisDistance(Rect nodeRect, Rect itemRect, const int direction)
{
    double distance = 0.0;
    switch (direction) {
        case FOCUS_DIRECTION_LEFT:
        case FOCUS_DIRECTION_RIGHT:
            distance = (nodeRect.Top() + nodeRect.Bottom()) / 2 - (itemRect.Top() + itemRect.Bottom()) / 2;
            break;
        case FOCUS_DIRECTION_UP:
        case FOCUS_DIRECTION_DOWN:
            distance = (nodeRect.Left() + nodeRect.Right()) / 2 - (itemRect.Left() + itemRect.Right()) / 2;
            break;
        default:
            break;
    }

    return distance > 0.0 ? distance : -distance;
}

double JsAccessibilityManager::GetWeightedDistanceFor(double majorAxisDistance, double minorAxisDistance)
{
    return WEIGHTED_VALUE * majorAxisDistance * majorAxisDistance + minorAxisDistance * minorAxisDistance;
}

bool JsAccessibilityManager::RequestAccessibilityFocus(const RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::RequestAccessibilityFocus");
    auto requestNodeId = node->GetNodeId();
    if (currentFocusNodeId_ == requestNodeId) {
        LOGW("This node is focused.");
        return false;
    }

    ClearCurrentFocus();
    currentFocusNodeId_ = requestNodeId;
    node->SetFocusedState(true);
    node->SetAccessibilityFocusedState(true);
    LOGI("RequestAccessibilityFocus SetFocusedState true nodeId:%{public}d", node->GetNodeId());
    return node->ActionAccessibilityFocus(true);
}

bool JsAccessibilityManager::ClearAccessibilityFocus(const RefPtr<AccessibilityNode>& node)
{
    LOGI("JsAccessibilityManager::ClearAccessibilityFocus");
    auto requestNodeId = node->GetNodeId();
    if (currentFocusNodeId_ != requestNodeId) {
        LOGW("This node is not focused.");
        return false;
    }

    currentFocusNodeId_ = -1;
    node->SetFocusedState(false);
    node->SetAccessibilityFocusedState(false);
    return node->ActionAccessibilityFocus(false);
}

bool JsAccessibilityManager::ClearCurrentFocus()
{
    LOGI("JsAccessibilityManager::ClearCurrentFocus");
    auto currentFocusNode = GetAccessibilityNodeFromPage(currentFocusNodeId_);
    if (currentFocusNode != nullptr) {
        currentFocusNodeId_ = -1;
        currentFocusNode->SetFocusedState(false);
        currentFocusNode->SetAccessibilityFocusedState(false);
        LOGI("ClearCurrentFocus SetFocusedState false nodeId:%{public}d", currentFocusNode->GetNodeId());
        return currentFocusNode->ActionAccessibilityFocus(false);
    }
    return false;
}

} // namespace OHOS::Ace::Framework
