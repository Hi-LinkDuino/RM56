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

#include "adapter/preview/inspector/js_inspector_manager.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "adapter/preview/inspector/inspector_client.h"
#include "bridge/declarative_frontend/declarative_frontend.h"
#include "core/components_v2/inspector/shape_composed_element.h"

namespace OHOS::Ace::Framework {
namespace {

const char INSPECTOR_CURRENT_VERSION[] = "1.0";
const char INSPECTOR_DEVICE_TYPE[] = "deviceType";
const char INSPECTOR_DEFAULT_VALUE[] = "defaultValue";
const char INSPECTOR_TYPE[] = "$type";
const char INSPECTOR_ROOT[] = "root";
const char INSPECTOR_VERSION[] = "version";
const char INSPECTOR_WIDTH[] = "width";
const char INSPECTOR_HEIGHT[] = "height";
const char INSPECTOR_RESOLUTION[] = "$resolution";
const char INSPECTOR_CHILDREN[] = "$children";
const char INSPECTOR_ID[] = "$ID";
const char INSPECTOR_RECT[] = "$rect";
const char INSPECTOR_Z_INDEX[] = "$z-index";
const char INSPECTOR_ATTRS[] = "$attrs";
const char INSPECTOR_STYLES[] = "$styles";
const char INSPECTOR_INNER_DEBUGLINE[] = "debugLine";
const char INSPECTOR_DEBUGLINE[] = "$debugLine";

std::list<std::string> specialComponentNameV1 = { "dialog", "panel" };

} // namespace

std::string GetDeviceTypeStr(const DeviceType& deviceType)
{
    std::string deviceName = "";
    if (deviceType == DeviceType::TV) {
        deviceName = "TV";
    } else if (deviceType == DeviceType::WATCH) {
        deviceName = "Watch";
    } else if (deviceType == DeviceType::CAR) {
        deviceName = "Car";
    } else {
        deviceName = "Phone";
    }
    return deviceName;
}

void JsInspectorManager::InitializeCallback()
{
    auto assembleJSONTreeCallback = [weak = WeakClaim(this)](std::string& jsonTreeStr) {
        auto jsInspectorManager = weak.Upgrade();
        if (!jsInspectorManager) {
            return false;
        }
        jsInspectorManager->AssembleJSONTree(jsonTreeStr);
        return true;
    };
    InspectorClient::GetInstance().RegisterJSONTreeCallback(assembleJSONTreeCallback);
    auto assembleDefaultJSONTreeCallback = [weak = WeakClaim(this)](std::string& jsonTreeStr) {
        auto jsInspectorManager = weak.Upgrade();
        if (!jsInspectorManager) {
            return false;
        }
        jsInspectorManager->AssembleDefaultJSONTree(jsonTreeStr);
        return true;
    };
    InspectorClient::GetInstance().RegisterDefaultJSONTreeCallback(assembleDefaultJSONTreeCallback);
    auto operateComponentCallback = [weak = WeakClaim(this)](const std::string& attrsJson) {
        auto jsInspectorManager = weak.Upgrade();
        if (!jsInspectorManager) {
            return false;
        }
        return jsInspectorManager->OperateComponent(attrsJson);
    };
    InspectorClient::GetInstance().RegisterOperateComponentCallback(operateComponentCallback);
}

// resourse the child from root node to assemble the JSON tree
void JsInspectorManager::AssembleJSONTree(std::string& jsonStr)
{
    auto jsonNode = JsonUtil::Create(true);
    jsonNode->Put(INSPECTOR_TYPE, INSPECTOR_ROOT);

    auto context = GetPipelineContext().Upgrade();
    if (context) {
        float scale = context->GetViewScale();
        double rootHeight = context->GetRootHeight();
        double rootWidth = context->GetRootWidth();
        jsonNode->Put(INSPECTOR_WIDTH, std::to_string(rootWidth * scale).c_str());
        jsonNode->Put(INSPECTOR_HEIGHT, std::to_string(rootHeight * scale).c_str());
    }
    jsonNode->Put(INSPECTOR_RESOLUTION, std::to_string(SystemProperties::GetResolution()).c_str());
    auto node = GetAccessibilityNodeFromPage(0);
    if (!node) {
        LOGE("get root accessibilityNode failed");
        return;
    }
    jsonNode->Put(INSPECTOR_CHILDREN, GetChildrenJson(node));
    jsonStr = jsonNode->ToString();
}

// find children of the current node and combine them with this node to form a JSON array object.
std::unique_ptr<JsonValue> JsInspectorManager::GetChildrenJson(RefPtr<AccessibilityNode> node)
{
    auto jsonNodeArray = JsonUtil::CreateArray(false);
    if (!node) {
        LOGW("GetChildrenJson, AccessibilityNode is nullptr");
        return jsonNodeArray;
    }
    auto child = node->GetChildList();
    for (auto item = child.begin(); item != child.end(); item++) {
        jsonNodeArray->Put(GetChildJson(*item));
    }
    return jsonNodeArray;
}

std::unique_ptr<JsonValue> JsInspectorManager::GetChildJson(RefPtr<AccessibilityNode> node)
{
    auto jsonNode = JsonUtil::Create(false);
    if (!node) {
        LOGW("GetChildJson, AccessibilityNode is nullptr");
        return jsonNode;
    }
    if (node->GetTag() == "inspectDialog") {
        RemoveAccessibilityNodes(node);
        return jsonNode;
    }

    jsonNode->Put(INSPECTOR_TYPE, node->GetTag().c_str());
    jsonNode->Put(INSPECTOR_ID, node->GetNodeId());
    jsonNode->Put(INSPECTOR_Z_INDEX, node->GetZIndex());
    if (GetVersion() == AccessibilityVersion::JS_VERSION) {
        jsonNode->Put(INSPECTOR_RECT, UpdateNodeRectStrInfo(node).c_str());
        GetAttrsAndStyles(jsonNode, node);
    } else {
        jsonNode->Put(INSPECTOR_RECT, UpdateNodeRectStrInfoV2(node).c_str());
        GetAttrsAndStylesV2(jsonNode, node);
    }
    jsonNode->Put(INSPECTOR_CHILDREN, GetChildrenJson(node));
    return jsonNode;
}

// assemble the default attrs and styles for all components
void JsInspectorManager::AssembleDefaultJSONTree(std::string& jsonStr)
{
    auto jsonNode = JsonUtil::Create(true);
    std::string deviceName = GetDeviceTypeStr(SystemProperties::GetDeviceType());

    jsonNode->Put(INSPECTOR_VERSION, INSPECTOR_CURRENT_VERSION);
    jsonNode->Put(INSPECTOR_DEVICE_TYPE, deviceName.c_str());
    static const std::vector<std::string> tagNames = { DOM_NODE_TAG_BADGE, DOM_NODE_TAG_BUTTON, DOM_NODE_TAG_CAMERA,
        DOM_NODE_TAG_CANVAS, DOM_NODE_TAG_CHART, DOM_NODE_TAG_DIALOG, DOM_NODE_TAG_DIV, DOM_NODE_TAG_DIVIDER,
        DOM_NODE_TAG_FORM, DOM_NODE_TAG_GRID_COLUMN, DOM_NODE_TAG_GRID_CONTAINER, DOM_NODE_TAG_GRID_ROW,
        DOM_NODE_TAG_IMAGE, DOM_NODE_TAG_IMAGE_ANIMATOR, DOM_NODE_TAG_INPUT, DOM_NODE_TAG_LABEL, DOM_NODE_TAG_LIST,
        DOM_NODE_TAG_LIST_ITEM, DOM_NODE_TAG_LIST_ITEM_GROUP, DOM_NODE_TAG_MARQUEE, DOM_NODE_TAG_MENU,
        DOM_NODE_TAG_NAVIGATION_BAR, DOM_NODE_TAG_OPTION, DOM_NODE_TAG_PANEL, DOM_NODE_TAG_PICKER_DIALOG,
        DOM_NODE_TAG_PICKER_VIEW, DOM_NODE_TAG_PIECE, DOM_NODE_TAG_POPUP, DOM_NODE_TAG_PROGRESS, DOM_NODE_TAG_QRCODE,
        DOM_NODE_TAG_RATING, DOM_NODE_TAG_REFRESH, DOM_NODE_TAG_SEARCH, DOM_NODE_TAG_SELECT, DOM_NODE_TAG_SLIDER,
        DOM_NODE_TAG_SPAN, DOM_NODE_TAG_STACK, DOM_NODE_TAG_STEPPER, DOM_NODE_TAG_STEPPER_ITEM, DOM_NODE_TAG_SWIPER,
        DOM_NODE_TAG_SWITCH, DOM_NODE_TAG_TAB_BAR, DOM_NODE_TAG_TAB_CONTENT, DOM_NODE_TAG_TABS, DOM_NODE_TAG_TEXT,
        DOM_NODE_TAG_TEXTAREA, DOM_NODE_TAG_TOGGLE, DOM_NODE_TAG_TOOL_BAR, DOM_NODE_TAG_TOOL_BAR_ITEM,
        DOM_NODE_TAG_VIDEO };

    auto jsonDefaultValue = JsonUtil::Create(true);
    for (const auto& tag : tagNames) {
        auto jsonDefaultAttrs = JsonUtil::Create(true);
        if (!GetDefaultAttrsByType(tag, jsonDefaultAttrs)) {
            LOGW("node type %{public}s is invalid", tag.c_str());
            return;
        }
        jsonDefaultValue->Put(tag.c_str(), jsonDefaultAttrs);
    }
    jsonNode->Put(INSPECTOR_DEFAULT_VALUE, jsonDefaultValue);
    jsonStr = jsonNode->ToString();
}

bool JsInspectorManager::OperateComponent(const std::string& jsCode)
{
    auto root = JsonUtil::ParseJsonString(jsCode);
    auto operateType = root->GetString("type", "");
    auto parentID = root->GetInt("parentID", -1);
    auto slot = root->GetInt("slot", -1);
    auto newComponent = GetNewComponentWithJsCode(root);
    if (parentID <= 0) {
        auto rootElement = GetRootElement().Upgrade();
        auto child = rootElement->GetChildBySlot(-1); // rootElement only has one child,and use the default slot -1
        if (!newComponent) {
            LOGE("operateType:UpdateComponent, newComponent should not be nullptr");
            return false;
        }
        rootElement->UpdateChildWithSlot(child, newComponent, -1, -1);
        return true;
    }
    return OperateGrneralComponent(parentID, slot, operateType, newComponent);
}

bool JsInspectorManager::OperateGrneralComponent(
    int32_t parentID, int32_t slot, std::string& operateType, RefPtr<Component> newComponent)
{
    auto parentElement = GetInspectorElementById(parentID);
    if (!parentElement || !(parentElement->IsRectValid())) {
        LOGE("parentElement should not be nullptr or display");
        return false;
    }

    if (operateType == "DeleteComponent") {
        parentElement->DeleteChildWithSlot(slot);
        return true;
    }

    if (newComponent) {
        if (operateType == "AddComponent") {
            parentElement->AddChildWithSlot(slot, newComponent);
        }
        if (operateType == "UpdateComponent") {
            parentElement->UpdateChildWithSlot(slot, newComponent);
        }
        return true;
    }
    return false;
}

RefPtr<Component> JsInspectorManager::GetNewComponentWithJsCode(const std::unique_ptr<JsonValue>& root)
{
    std::string jsCode = root->GetString("jsCode", "");
    if (jsCode.length() == 0) {
        LOGE("Get jsCode Failed");
        return nullptr;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Get Context Failed");
        return nullptr;
    }
    auto frontend = context->GetFrontend();
    if (!frontend) {
        LOGE("Get frontend Failed");
        return nullptr;
    }
    auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(frontend);
    if (!declarativeFrontend) {
        LOGE("Get declarativeFrontend Failed");
        return nullptr;
    }
    auto component = declarativeFrontend->GetNewComponentWithJsCode(jsCode);
    return component;
}

RefPtr<V2::InspectorComposedElement> JsInspectorManager::GetInspectorElementById(NodeId nodeId)
{
    auto composedElement = GetComposedElementFromPage(nodeId).Upgrade();
    if (!composedElement) {
        LOGE("get composedElement failed");
        return nullptr;
    }
    auto inspectorElement = AceType::DynamicCast<V2::InspectorComposedElement>(composedElement);
    if (!inspectorElement) {
        LOGE("get inspectorElement failed");
        return nullptr;
    }
    return inspectorElement;
}

const WeakPtr<Element>& JsInspectorManager::GetRootElement()
{
    auto node = GetAccessibilityNodeFromPage(0);
    if (!node) {
        LOGE("get AccessibilityNode failed");
        return nullptr;
    }
    auto child = node->GetChildList();
    if (child.empty()) {
        LOGE("child is null");
        return nullptr;
    }
    auto InspectorComponentElement = GetInspectorElementById(child.front()->GetNodeId());
    if (!InspectorComponentElement) {
        return nullptr;
    }
    return InspectorComponentElement->GetElementParent();
}

// get attrs and styles from AccessibilityNode to JsonValue
void JsInspectorManager::GetAttrsAndStyles(std::unique_ptr<JsonValue>& jsonNode, const RefPtr<AccessibilityNode>& node)
{
    auto attrJsonNode = JsonUtil::Create(true);
    for (auto attr : node->GetAttrs()) {
        // this attr is wrong in API5,will delete in API7
        if (attr.first.find("clickEffect") != std::string::npos) {
            attr.first = ConvertStrToPropertyType(attr.first);
        }
        attrJsonNode->Put(attr.first.c_str(), attr.second.c_str());
    }
    // change debugLine to $debugLine and move out of attrs
    std::string debugLine = attrJsonNode->GetString(INSPECTOR_INNER_DEBUGLINE);
    jsonNode->Put(INSPECTOR_DEBUGLINE, debugLine.c_str());
    attrJsonNode->Delete(INSPECTOR_INNER_DEBUGLINE);
    jsonNode->Put(INSPECTOR_ATTRS, attrJsonNode);

    auto styleJsonNode = JsonUtil::Create(true);
    for (auto style : node->GetStyles()) {
        styleJsonNode->Put(ConvertStrToPropertyType(style.first).c_str(), style.second.c_str());
    }
    jsonNode->Put(INSPECTOR_STYLES, styleJsonNode);
}

void JsInspectorManager::GetAttrsAndStylesV2(
    std::unique_ptr<JsonValue>& jsonNode, const RefPtr<AccessibilityNode>& node)
{
    auto weakComposedElement = GetComposedElementFromPage(node->GetNodeId());
    auto composedElement = DynamicCast<V2::InspectorComposedElement>(weakComposedElement.Upgrade());
    if (!composedElement) {
        LOGE("return");
        return;
    }
    std::string debugLine = composedElement->GetDebugLine();
    jsonNode->Put(INSPECTOR_DEBUGLINE, debugLine.c_str());
    auto inspectorElement = AceType::DynamicCast<V2::InspectorComposedElement>(composedElement);
    if (inspectorElement) {
        auto jsonObject = inspectorElement->ToJsonObject();
        jsonNode->Put(INSPECTOR_ATTRS, jsonObject);
    }
    auto shapeComposedElement = AceType::DynamicCast<V2::ShapeComposedElement>(inspectorElement);
    if (shapeComposedElement) {
        jsonNode->Replace(INSPECTOR_TYPE, shapeComposedElement->GetShapeType().c_str());
    }
}

std::string JsInspectorManager::UpdateNodeRectStrInfo(const RefPtr<AccessibilityNode> node)
{
    auto it = std::find(specialComponentNameV1.begin(), specialComponentNameV1.end(), node->GetTag());
    if (it != specialComponentNameV1.end()) {
        node->UpdateRectWithChildRect();
    }

    PositionInfo positionInfo = { 0, 0, 0, 0 };
    if (node->GetTag() == DOM_NODE_TAG_SPAN) {
        positionInfo = { node->GetParentNode()->GetWidth(), node->GetParentNode()->GetHeight(),
            node->GetParentNode()->GetLeft(), node->GetParentNode()->GetTop() };
    } else {
        positionInfo = { node->GetWidth(), node->GetHeight(), node->GetLeft(), node->GetTop() };
    }
    if (!node->GetVisible()) {
        positionInfo = { 0, 0, 0, 0 };
    }
    // the dialog node is hidden, while the position and size of the node are not cleared.
    if (node->GetClearRectInfoFlag() == true) {
        positionInfo = { 0, 0, 0, 0 };
    }
    std::string strRec = std::to_string(positionInfo.left)
                             .append(",")
                             .append(std::to_string(positionInfo.top))
                             .append(",")
                             .append(std::to_string(positionInfo.width))
                             .append(",")
                             .append(std::to_string(positionInfo.height));
    return strRec;
}

std::string JsInspectorManager::UpdateNodeRectStrInfoV2(const RefPtr<AccessibilityNode> node)
{
    std::string strRec;
    auto weakComposedElement = GetComposedElementFromPage(node->GetNodeId());
    auto composedElement = weakComposedElement.Upgrade();
    if (!composedElement) {
        return strRec;
    }
    auto inspectorElement = AceType::DynamicCast<V2::InspectorComposedElement>(composedElement);
    if (inspectorElement) {
        strRec = inspectorElement->GetRect();
        return strRec;
    }
    return strRec;
}

std::string JsInspectorManager::ConvertStrToPropertyType(const std::string& typeValue)
{
    std::string dstStr;
    std::regex regex("([A-Z])");
    dstStr = regex_replace(typeValue, regex, "-$1");
    std::transform(dstStr.begin(), dstStr.end(), dstStr.begin(), ::tolower);
    return dstStr;
}

RefPtr<AccessibilityNodeManager> AccessibilityNodeManager::Create()
{
    return AceType::MakeRefPtr<JsInspectorManager>();
}
} // namespace OHOS::Ace::Framework
