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

#include "inspector.h"

#include "inspector_composed_element.h"
#include "shape_composed_element.h"

#include "core/components/root/root_element.h"

namespace OHOS::Ace::V2 {
namespace {
const char INSPECTOR_TYPE[] = "$type";
const char INSPECTOR_ROOT[] = "root";
const char INSPECTOR_WIDTH[] = "width";
const char INSPECTOR_HEIGHT[] = "height";
const char INSPECTOR_RESOLUTION[] = "$resolution";
const char INSPECTOR_CHILDREN[] = "$children";
const char INSPECTOR_ID[] = "$ID";
const char INSPECTOR_RECT[] = "$rect";
const char INSPECTOR_Z_INDEX[] = "$z-index";
const char INSPECTOR_ATTRS[] = "$attrs";
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
const char INSPECTOR_DEBUGLINE[] = "$debugLine";
#endif

RefPtr<V2::InspectorComposedElement> GetInspectorByKey(const RefPtr<RootElement>& root, const std::string& key)
{
    std::queue<RefPtr<Element>> elements;
    elements.push(root);
    RefPtr<V2::InspectorComposedElement> inspectorElement;
    while (!elements.empty()) {
        auto current = elements.front();
        elements.pop();
        inspectorElement = AceType::DynamicCast<V2::InspectorComposedElement>(current);
        if (inspectorElement != nullptr) {
            if (key == inspectorElement->GetKey()) {
                return inspectorElement;
            }
        }
        const auto& children = current->GetChildren();
        for (auto& child : children) {
            elements.push(child);
        }
    }
    return nullptr;
}

void DumpElementTree(
    int32_t depth, const RefPtr<Element>& element, std::map<int32_t, std::list<RefPtr<Element>>>& depthElementMap)
{
    if (element->GetChildren().empty()) {
        return;
    }
    const auto& children = element->GetChildren();
    depthElementMap[depth].insert(depthElementMap[depth].end(), children.begin(), children.end());
    for (auto& depthElement : children) {
        DumpElementTree(depth + 1, depthElement, depthElementMap);
    }
}
} // namespace

std::string Inspector::GetInspectorNodeByKey(const RefPtr<PipelineContext>& context, const std::string& key)
{
    auto inspectorElement = GetInspectorByKey(context->GetRootElement(), key);
    if (inspectorElement == nullptr) {
        LOGE("no inspector with key:%{public}s is found", key.c_str());
        return "";
    }

    auto jsonNode = JsonUtil::Create(true);
    jsonNode->Put(INSPECTOR_TYPE, inspectorElement->GetTag().c_str());
    jsonNode->Put(INSPECTOR_ID, std::stoi(inspectorElement->GetId()));
    jsonNode->Put(INSPECTOR_Z_INDEX, inspectorElement->GetZIndex());
    jsonNode->Put(INSPECTOR_RECT, inspectorElement->GetRenderRect().ToBounds().c_str());
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string debugLine = inspectorElement->GetDebugLine();
    jsonNode->Put(INSPECTOR_DEBUGLINE, debugLine.c_str());
#endif
    auto jsonObject = inspectorElement->ToJsonObject();
    jsonNode->Put(INSPECTOR_ATTRS, jsonObject);
    return jsonNode->ToString();
}

std::string Inspector::GetInspectorTree(const RefPtr<PipelineContext>& context)
{
    auto jsonRoot = JsonUtil::Create(true);
    jsonRoot->Put(INSPECTOR_TYPE, INSPECTOR_ROOT);

    float scale = context->GetViewScale();
    double rootHeight = context->GetRootHeight();
    double rootWidth = context->GetRootWidth();
    jsonRoot->Put(INSPECTOR_WIDTH, std::to_string(rootWidth * scale).c_str());
    jsonRoot->Put(INSPECTOR_HEIGHT, std::to_string(rootHeight * scale).c_str());
    jsonRoot->Put(INSPECTOR_RESOLUTION, std::to_string(SystemProperties::GetResolution()).c_str());

    auto root = AceType::DynamicCast<Element>(context->GetRootElement());
    if (root == nullptr) {
        return jsonRoot->ToString();
    }

    std::map<int32_t, std::list<RefPtr<Element>>> depthElementMap;
    depthElementMap[0].emplace_back(root);
    DumpElementTree(1, root, depthElementMap);

    size_t height = 0;
    std::unordered_map<int32_t, std::vector<std::pair<RefPtr<Element>, std::string>>> elementJSONInfoMap;
    for (int depth = static_cast<int32_t>(depthElementMap.size()); depth > 0; depth--) {
        const auto& depthElements = depthElementMap[depth];
        for (const auto& element : depthElements) {
            auto inspectorElement = AceType::DynamicCast<V2::InspectorComposedElement>(element);
            if (inspectorElement == nullptr) {
                continue;
            }

            auto jsonNode = JsonUtil::Create(true);
            jsonNode->Put(INSPECTOR_TYPE, inspectorElement->GetTag().c_str());
            jsonNode->Put(INSPECTOR_ID, std::stoi(inspectorElement->GetId()));
            jsonNode->Put(INSPECTOR_Z_INDEX, inspectorElement->GetZIndex());
            jsonNode->Put(INSPECTOR_RECT, inspectorElement->GetRenderRect().ToBounds().c_str());
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
            std::string debugLine = inspectorElement->GetDebugLine();
            jsonNode->Put(INSPECTOR_DEBUGLINE, debugLine.c_str());
#endif
            auto jsonObject = inspectorElement->ToJsonObject();
            jsonNode->Put(INSPECTOR_ATTRS, jsonObject);
            auto shapeComposedElement = AceType::DynamicCast<V2::ShapeComposedElement>(element);
            if (shapeComposedElement != nullptr) {
                int type = StringUtils::StringToInt(shapeComposedElement->GetShapeType());
                jsonNode->Replace(INSPECTOR_TYPE, SHAPE_TYPE_STRINGS[type]);
            }
            if (!element->GetChildren().empty()) {
                if (height > 0) {
                    auto jsonNodeArray = JsonUtil::CreateArray(true);
                    auto childNodeJSONVec = elementJSONInfoMap[height - 1];
                    for (auto iter = childNodeJSONVec.begin(); iter != childNodeJSONVec.end(); iter++) {
                        auto parent = iter->first->GetElementParent().Upgrade();
                        while (parent) {
                            if (AceType::TypeName(parent) == AceType::TypeName(element) &&
                                parent->GetRetakeId() == element->GetRetakeId()) {
                                auto childJSONValue = JsonUtil::ParseJsonString(iter->second);
                                jsonNodeArray->Put(childJSONValue);
                                break;
                            } else {
                                parent = parent->GetElementParent().Upgrade();
                            }
                        }
                    }
                    if (jsonNodeArray->GetArraySize()) {
                        jsonNode->Put(INSPECTOR_CHILDREN, jsonNodeArray);
                    }
                }
            }
            elementJSONInfoMap[height].emplace_back(element, jsonNode->ToString());
        }
        if (elementJSONInfoMap.find(height) != elementJSONInfoMap.end()) {
            height++;
        }
    }

    auto jsonNodeArray = JsonUtil::CreateArray(true);
    auto firstDepthNodeVec = elementJSONInfoMap[elementJSONInfoMap.size() - 1];
    for (const auto& nodeJSONInfo : firstDepthNodeVec) {
        auto nodeJSONValue = JsonUtil::ParseJsonString(nodeJSONInfo.second);
        jsonNodeArray->Put(nodeJSONValue);
    }
    jsonRoot->Put(INSPECTOR_CHILDREN, jsonNodeArray);
    return jsonRoot->ToString();
}

bool Inspector::SendEventByKey(
    const RefPtr<PipelineContext>& context, const std::string& key, int action, const std::string& params)
{
    auto inspectorElement = GetInspectorByKey(context->GetRootElement(), key);
    if (inspectorElement == nullptr) {
        LOGE("no inspector with key:%s is found", key.c_str());
        return false;
    }

    Rect rect = inspectorElement->GetRenderRect();

    context->GetTaskExecutor()->PostTask(
        [weak = AceType::WeakClaim(AceType::RawPtr(context)), rect, action, params, inspectorElement]() {
            auto context = weak.Upgrade();
            if (!context) {
                return;
            }

            TouchEvent point { .x = static_cast<float>(rect.Left() + rect.Width() / 2),
                .y = static_cast<float>(rect.Top() + rect.Height() / 2),
                .type = TouchType::DOWN,
                .time = std::chrono::high_resolution_clock::now() };
            context->OnTouchEvent(point);

            switch (action) {
                case static_cast<int>(AceAction::ACTION_CLICK): {
                    TouchEvent upPoint { .x = point.x,
                        .y = point.y,
                        .type = TouchType::UP,
                        .time = std::chrono::high_resolution_clock::now() };
                    context->OnTouchEvent(upPoint);
                    break;
                }
                case static_cast<int>(AceAction::ACTION_LONG_CLICK): {
                    CancelableCallback<void()> inspectorTimer;
                    auto&& callback = [weak, point]() {
                        auto refPtr = weak.Upgrade();
                        if (refPtr) {
                            TouchEvent upPoint { .x = point.x,
                                .y = point.y,
                                .type = TouchType::UP,
                                .time = std::chrono::high_resolution_clock::now() };
                            refPtr->OnTouchEvent(upPoint);
                        }
                    };
                    inspectorTimer.Reset(callback);
                    auto taskExecutor =
                        SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
                    taskExecutor.PostDelayedTask(inspectorTimer, 1000);
                    break;
                }
                default:
                    break;
            }
        },
        TaskExecutor::TaskType::UI);

    return true;
}

bool Inspector::SendKeyEvent(const RefPtr<PipelineContext>& context, const JsKeyEvent& event)
{
    KeyEvent keyEvent(event.code, event.action);
    keyEvent.metaKey = event.metaKey;
    keyEvent.deviceId = event.deviceId;
    keyEvent.repeatTime = 0;
    keyEvent.SetTimeStamp(event.timeStamp);
    keyEvent.sourceType = static_cast<SourceType>(event.sourceDevice);
    return context->GetTaskExecutor()->PostTask(
        [context, keyEvent]() { context->OnKeyEvent(keyEvent); }, TaskExecutor::TaskType::UI);
}
} // namespace OHOS::Ace::V2
