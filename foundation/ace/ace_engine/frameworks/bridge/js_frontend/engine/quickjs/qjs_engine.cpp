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

#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"

#include <algorithm>
#include <regex>
#include <string>
#include <unistd.h>
#include <unordered_map>

#include "third_party/quickjs/message_server.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/linear_map.h"
#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/media_query/media_query_info.h"
#include "frameworks/bridge/common/plugin_adapter/plugin_bridge.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/common/js_api_perf.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/badge_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/chart_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/clock_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/component_api_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/image_animator_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/intl/intl_support.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/list_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_xcomponent_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/stepper_bridge.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

extern const uint32_t js_framework_size;
extern const uint8_t js_framework[];

namespace OHOS::Ace::Framework {
namespace {

#ifndef ENABLE_JS_DEBUG
const char JS_MAIN_ENTRY[] = "___mainEntry___";
const char JS_EXT[] = ".js";
const char BIN_EXT[] = ".bin";
#endif
const char MAP_EXT[] = ".map";
constexpr int32_t CUSTOM_FULL_WINDOW_LENGTH = 3;
constexpr int32_t ARGS_FULL_WINDOW_LENGTH = 2;
constexpr int32_t ARGS_READ_RESOURCE_LENGTH = 2;
constexpr int32_t MAX_READ_TEXT_LENGTH = 4096;
const std::regex URI_PARTTEN("^\\/([a-z0-9A-Z_]+\\/)*[a-z0-9A-Z_]+\\.?[a-z0-9A-Z_]*$");

static int32_t globalNodeId = 100000;

int32_t CallEvalBuf(
    JSContext* ctx, const char* buf, size_t bufLen, const char* filename, int32_t evalFlags, int32_t instanceId)
{
    JSValue val = JS_Eval(ctx, buf, bufLen, filename, evalFlags);
    int32_t ret = JS_CALL_SUCCESS;
    if (JS_IsException(val)) {
        LOGE("[Qjs Native] EvalBuf failed!");
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::EVAL_BUFFER_ERROR, instanceId);
        ret = JS_CALL_FAIL;
    }
    JS_FreeValue(ctx, val);
    js_std_loop(ctx);
    return ret;
}

JSValue CallReadObject(JSContext* ctx, const uint8_t* buf, size_t bufLen, bool persist = false, int32_t instanceId = 0,
    const char* pageUrl = nullptr)
{
    int32_t flags = persist ? (JS_READ_OBJ_ROM_DATA | JS_READ_OBJ_BYTECODE) : JS_READ_OBJ_BYTECODE;
    JSValue obj = JS_ReadObject(ctx, buf, bufLen, flags);
    if (JS_IsException(obj)) {
        LOGE("[Qjs Native] ReadObject failed!");
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::READ_OBJECT_ERROR, instanceId, pageUrl);
        return obj;
    }
    return JS_EvalFunction(ctx, obj);
}

RefPtr<JsAcePage> GetRunningPage(JSContext* ctx)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    ACE_DCHECK(instance);
    return instance->GetRunningPage();
}

RefPtr<JsAcePage> GetStagingPage(JSContext* ctx)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    ACE_DCHECK(instance);
    return instance->GetStagingPage();
}

void GetArrayValue(JSContext* ctx, JSValueConst arrayVal, std::string& result)
{
    int32_t length = QJSUtils::JsGetArrayLength(ctx, arrayVal);
    for (int32_t i = 0; i < length; ++i) {
        if (i != 0) {
            result.append(1, DOM_PICKER_SPLIT_ARRAY); // only need one char to split.
        }
        JSValue itemVal = JS_GetPropertyUint32(ctx, arrayVal, i);
        if (JS_IsString(itemVal) || JS_IsNumber(itemVal) || JS_IsBool(itemVal)) {
            ScopedString val(ctx, itemVal);
            const char* itemStr = val.get();
            result.append(itemStr);
            JS_FreeValue(ctx, itemVal);
            continue;
        }
        if (JS_IsArray(ctx, itemVal)) {
            int32_t subLength = QJSUtils::JsGetArrayLength(ctx, itemVal);
            for (int32_t j = 0; j < subLength; ++j) {
                if (j != 0) {
                    result.append(1, DOM_PICKER_SPLIT_ITEM); // only need one char to split
                }
                JSValue subItemVal = JS_GetPropertyUint32(ctx, itemVal, j);
                ScopedString subVal(ctx, subItemVal);
                const char* subItemStr = subVal.get();
                result.append(subItemStr);
                JS_FreeValue(ctx, subItemVal);
            }
            JS_FreeValue(ctx, itemVal);
            continue;
        }
        JS_FreeValue(ctx, itemVal);
    }
}

void GetAttrImage(JSContext* ctx, JSValueConst valObject, ImageProperties& imageProperties)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; ++i) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue valItem = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(valItem) || JS_IsNumber(valItem) || JS_IsBool(valItem)) {
            ScopedString styleVal(ctx, valItem);
            const char* valStr = styleVal.get();
            if (strcmp(key, "src") == 0) {
                imageProperties.src = valStr;
            } else if (strcmp(key, "width") == 0) {
                imageProperties.width = StringToDimension(valStr);
            } else if (strcmp(key, "height") == 0) {
                imageProperties.height = StringToDimension(valStr);
            } else if (strcmp(key, "top") == 0) {
                imageProperties.top = StringToDimension(valStr);
            } else if (strcmp(key, "left") == 0) {
                imageProperties.left = StringToDimension(valStr);
            } else if (strcmp(key, "duration") == 0) {
                imageProperties.duration = StringToInt(valStr);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, valItem);
    }
    js_free(ctx, pTab);
}

void GetAttrImages(JSContext* ctx, JSValueConst arrayVal, std::vector<ImageProperties>& images)
{
    int32_t length = QJSUtils::JsGetArrayLength(ctx, arrayVal);
    for (int32_t i = 0; i < length; ++i) {
        JSValue valArray = JS_GetPropertyUint32(ctx, arrayVal, i);
        ImageProperties imageProperties;
        if (JS_IsObject(valArray)) {
            GetAttrImage(ctx, valArray, imageProperties);
            images.push_back(imageProperties);
        }
        JS_FreeValue(ctx, valArray);
    }
}

bool SetDomAttributes(JSContext* ctx, JSValueConst fromMap, JsCommandDomElementOperator& command)
{
    ACE_SCOPED_TRACE("SetDomAttributes");

    bool hasShowAttr = false;
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, fromMap, &pTab, &len)) {
        return hasShowAttr;
    }

    std::vector<std::pair<std::string, std::string>> attrs;
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyString = key;
        JSValue val = JS_GetProperty(ctx, fromMap, pTab[i].atom);
        if (JS_IsString(val) || JS_IsNumber(val) || JS_IsBool(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            LOGD("SetDomAttributes: key: %{private}s, attr: %{private}s", key, valStr);
            if (keyString.compare(DOM_ID) == 0) {
                command.SetId(valStr);
            } else if (keyString.compare(DOM_TARGET) == 0) {
                command.SetTarget(valStr);
            } else if (keyString.compare(DOM_SHARE_ID) == 0) {
                command.SetShareId(valStr);
            }
            attrs.emplace_back(key, valStr);
            if (keyString.compare(DOM_SHOW) == 0) {
                hasShowAttr = true;
            }
        } else if (JS_IsArray(ctx, val)) {
            if (keyString.compare("datasets") == 0) {
                auto chartBridge = AceType::MakeRefPtr<ChartBridge>();
                chartBridge->GetAttrDatasets(ctx, val);
                command.SetDatasets(chartBridge->GetDatasets());
            } else if (keyString.compare("images") == 0) {
                std::vector<ImageProperties> images;
                GetAttrImages(ctx, val, images);
                command.SetImagesAttr(std::move(images));
            } else if (keyString.compare("segments") == 0) {
                auto chartBridge = AceType::MakeRefPtr<ChartBridge>();
                chartBridge->ParseAttrSegmentArray(ctx, val);
                command.SetSegments(chartBridge->GetSegments());
            } else {
                std::string valStr;
                GetArrayValue(ctx, val, valStr);
                LOGD("SetDomAttributes: key: %{private}s, attr: %{private}s", key, valStr.c_str());
                attrs.emplace_back(key, valStr);
            }
        } else if (JS_IsObject(val)) {
            if (keyString.compare("options") == 0) {
                auto chartBridge = AceType::MakeRefPtr<ChartBridge>();
                chartBridge->GetAttrOptionsObject(ctx, val);
                command.SetOptions(chartBridge->GetChartOptions());
            } else if (keyString.compare("segments") == 0) {
                auto chartBridge = AceType::MakeRefPtr<ChartBridge>();
                chartBridge->ParseAttrSingleSegment(ctx, val);
                command.SetSegments(chartBridge->GetSegments());
            } else if (keyString.compare(DOM_CLOCK_CONFIG) == 0) {
                auto clockBridge = AceType::MakeRefPtr<ClockBridge>();
                clockBridge->ParseClockConfig(ctx, val);
                command.SetClockConfig(clockBridge->GetClockConfig());
            } else if (keyString.compare(DOM_NODE_TAG_LABEL) == 0) {
                auto stepperBridge = AceType::MakeRefPtr<StepperBridge>();
                StepperLabels label;
                stepperBridge->GetAttrLabel(ctx, val, label);
                command.SetStepperLabel(label);
            } else if (keyString.compare(DOM_BADGE_CONFIG) == 0) {
                auto badgeBridge = AceType::MakeRefPtr<BadgeBridge>();
                badgeBridge->ParseBadgeConfig(ctx, val);
                command.SetBadgeConfig(badgeBridge->GetBadgeConfig());
            } else {
                LOGD("key %{public}s unsupported. Ignoring!", key);
            }
        } else if (JS_IsUndefined(val)) {
            LOGE("value of key[%{private}s] is undefined. Ignoring!", key);
        } else {
            LOGE("value of key[%{private}s] of unsupported type. Ignoring!", key);
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }

    command.SetAttributes(std::move(attrs));
    js_free(ctx, pTab);
    return hasShowAttr;
}

void GetAndRegisterFamily(JSContext* ctx, JSValueConst valArray, std::string& familyStyle)
{
    JSPropertyEnum* tab = nullptr;
    uint32_t aLen = 0;
    JS_GetOwnPropertyNames(ctx, &tab, &aLen, valArray, JS_GPN_STRING_MASK);

    std::string familyVal;
    std::string srcVal;
    for (uint32_t j = 0; j < aLen; j++) {
        // ValObject is one row of family object
        JSValue valObject = JS_GetProperty(ctx, valArray, tab[j].atom);
        const char* keyObject = JS_AtomToCString(ctx, tab[j].atom);
        if (JS_IsString(valObject)) {
            ScopedString styleVal(ctx, valObject);
            const char* valStr = styleVal.get();
            auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
            ACE_DCHECK(instance);
            if (keyObject != nullptr && strcmp(keyObject, "fontFamily") == 0) {
                familyVal = valStr;
                if (!srcVal.empty()) {
                    instance->GetDelegate()->RegisterFont(familyVal, srcVal);
                    familyVal.erase();
                    srcVal.erase();
                }
                if (familyStyle.length() > 0) {
                    familyStyle.append(",");
                }
                familyStyle.append(valStr);
            } else if (keyObject != nullptr && strcmp(keyObject, "src") == 0) {
                srcVal = valStr;
                // The format of font src is: url("src"), here get the src.
                srcVal = srcVal.substr(5, srcVal.length() - 7);
                if (!familyVal.empty()) {
                    instance->GetDelegate()->RegisterFont(familyVal, srcVal);
                    familyVal.erase();
                    srcVal.erase();
                }
            }
        }
        JS_FreeAtom(ctx, tab[j].atom);
        JS_FreeCString(ctx, keyObject);
        JS_FreeValue(ctx, valObject);
    }
    js_free(ctx, tab);
}

void GetStyleFamilyValue(JSContext* ctx, JSValueConst arrayVal, std::string& familyStyle)
{
    int32_t length = QJSUtils::JsGetArrayLength(ctx, arrayVal);
    for (int32_t i = 0; i < length; ++i) {
        // ValArray is one row of family array
        JSValue valArray = JS_GetPropertyUint32(ctx, arrayVal, i);
        if (JS_IsObject(valArray)) {
            GetAndRegisterFamily(ctx, valArray, familyStyle);
        }
        JS_FreeValue(ctx, valArray);
    }
}

void GetStyleAnimationName(
    JSContext* ctx, JSValueConst arrayVal, std::vector<std::unordered_map<std::string, std::string>>& styleVec)
{
    int32_t length = QJSUtils::JsGetArrayLength(ctx, arrayVal);
    for (int32_t i = 0; i < length; ++i) {
        std::unordered_map<std::string, std::string> animationNameKeyFrame;
        JSValue valArray = JS_GetPropertyUint32(ctx, arrayVal, i);
        if (JS_IsObject(valArray)) {
            JSPropertyEnum* tab = nullptr;
            uint32_t anotherLen = 0;
            JS_GetOwnPropertyNames(ctx, &tab, &anotherLen, valArray, JS_GPN_STRING_MASK);
            for (uint32_t j = 0; j < anotherLen; ++j) {
                const char* key = JS_AtomToCString(ctx, tab[j].atom);
                if (key == nullptr) {
                    JS_FreeAtom(ctx, tab[j].atom);
                    JS_FreeValue(ctx, valArray);
                    LOGW("key is null. Ignoring!");
                    continue;
                }
                JSValue valObject = JS_GetProperty(ctx, valArray, tab[j].atom);
                if (JS_IsString(valObject) || JS_IsNumber(valObject)) {
                    ScopedString styleVal(ctx, valObject);
                    const char* valStr = styleVal.get();
                    animationNameKeyFrame.emplace(key, valStr);
                } else {
                    LOGD("GetStyleAnimationName: unsupported type :%{public}d", JS_VALUE_GET_TAG(valObject));
                }
                JS_FreeAtom(ctx, tab[j].atom);
                JS_FreeCString(ctx, key);
                JS_FreeValue(ctx, valObject);
            }
            js_free(ctx, tab);
        }
        if (animationNameKeyFrame.size() > 0) {
            styleVec.emplace_back(animationNameKeyFrame);
        }
        JS_FreeValue(ctx, valArray);
    }
}

void SetDomStyle(JSContext* ctx, JSValueConst fromMap, JsCommandDomElementOperator& command)
{
    ACE_SCOPED_TRACE("SetDomStyle");

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, fromMap, &pTab, &len)) {
        return;
    }

    std::vector<std::pair<std::string, std::string>> styles;
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyString = key;
        JSValue val = JS_GetProperty(ctx, fromMap, pTab[i].atom);
        if (JS_IsString(val) || JS_IsNumber(val) || JS_IsBool(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            LOGD("SetDomStyle: key: %{private}s, style: %{private}s", key, valStr);
            styles.emplace_back(key, valStr);
        } else if (JS_IsArray(ctx, val)) {
            if (keyString.compare(DOM_TEXT_FONT_FAMILY) == 0) {
                // Deal with special case such as fontFamily, suppose all the keys in the array are the same.
                std::string familyStyle;
                GetStyleFamilyValue(ctx, val, familyStyle);
                styles.emplace_back(key, familyStyle);
            } else if (keyString.compare(DOM_ANIMATION_NAME) == 0) {
                // Deal with special case animationName, it different with fontfamily,
                // the keys in the array are different.
                std::vector<std::unordered_map<std::string, std::string>> animationStyles;
                GetStyleAnimationName(ctx, val, animationStyles);
                command.SetAnimationStyles(std::move(animationStyles));
            } else if (keyString.compare(DOM_TRANSITION_ENTER) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionEnter;
                GetStyleAnimationName(ctx, val, transitionEnter);
                command.SetTransitionEnter(std::move(transitionEnter));
            } else if (keyString.compare(DOM_TRANSITION_EXIT) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionExit;
                GetStyleAnimationName(ctx, val, transitionExit);
                command.SetTransitionExit(std::move(transitionExit));
            } else if (keyString.compare(DOM_SHARED_TRANSITION_NAME) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> sharedTransitionName;
                GetStyleAnimationName(ctx, val, sharedTransitionName);
                command.SetSharedTransitionName(std::move(sharedTransitionName));
            } else {
                LOGD("value is array, key unsupported. Ignoring!");
            }
        } else if (JS_IsUndefined(val)) {
            LOGD("value is undefined. Ignoring!");
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }

    QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto pipelineContext = instance->GetDelegate()->GetPipelineContext();
    command.SetPipelineContext(pipelineContext);
    command.SetStyles(std::move(styles));
    js_free(ctx, pTab);
}

void AddDomEvent(JSContext* ctx, JSValueConst fromArray, JsCommandDomElementOperator& command)
{
    ACE_SCOPED_TRACE("AddDomEvent");

    std::vector<std::string> eventsMap;
    int32_t length = QJSUtils::JsGetArrayLength(ctx, fromArray);
    for (int32_t i = 0; i < length; i++) {
        JSValue val = JS_GetPropertyUint32(ctx, fromArray, i);
        if (JS_IsString(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            eventsMap.emplace_back(valStr);
        } else {
            LOGW("value of unsupported type. Ignoring!");
        }
        JS_FreeValue(ctx, val);
    }
    command.AddEvents(std::move(eventsMap));
}

JSValue JsRemoveElement(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((argv == nullptr) || (argc != REMOVE_ELEMENT_ARGS_LEN)) {
        LOGE("JsRemoveElement: the arg is error when call removeElement");
        return JS_EXCEPTION;
    }

    auto page = GetRunningPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    int32_t instanceId = 0;
    JS_ToInt32(ctx, &instanceId, argv[REMOVE_ELEMENT_INSTANCE_ID]);
    if (page->GetPageId() != instanceId) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        page = instance->GetDelegate()->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsRemoveElement fail to get page, pageId: %{public}d", instanceId);
            return JS_EXCEPTION;
        }
    }

    int32_t nodeId = 0;
    JS_ToInt32(ctx, &nodeId, argv[0]);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    page->PushCommand(Referenced::MakeRefPtr<JsCommandRemoveDomElement>(nodeId));
    return JS_NULL;
}

JSValue JsUpdateElementAttrs(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((argv == nullptr) || (argc != UPLOAD_ELEMENT_ARGS_LEN)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }

    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    int32_t instanceId = 0;
    JS_ToInt32(ctx, &instanceId, argv[UPLOAD_ELEMENT_INSTANCE_ID]);
    if (page->GetPageId() != instanceId) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        page = instance->GetDelegate()->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsUpdateElementAttrs fail to get page, pageId :%{public}d", instanceId);
            return JS_EXCEPTION;
        }
    }

    int32_t nodeId = 0;
    JS_ToInt32(ctx, &nodeId, argv[0]);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(ctx, argv[1], *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
    return JS_NULL;
}

JSValue JsUpdateElementStyles(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((argv == nullptr) || (argc != UPLOAD_ELEMENT_ARGS_LEN)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }

    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    int32_t instanceId = 0;
    JS_ToInt32(ctx, &instanceId, argv[UPLOAD_ELEMENT_INSTANCE_ID]);
    if (page->GetPageId() != instanceId) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        page = instance->GetDelegate()->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsUpdateElementStyles fail to get page, pageId: %{public}d", instanceId);
            return JS_EXCEPTION;
        }
    }

    int32_t nodeId = 0;
    JS_ToInt32(ctx, &nodeId, argv[0]);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(ctx, argv[1], *command);
    page->PushCommand(command);
    return JS_NULL;
}

JSValue JsDomCreateBody(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((argv == nullptr) || (argc != 5)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }

    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    int32_t nodeId = DOM_ROOT_NODE_ID_BASE + page->GetPageId();
    LOGD("JsDomCreateBody: pageId: %{private}d, nodeId: %{private}d:", page->GetPageId(), nodeId);

    ScopedString tag(ctx, argv[1]);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomBody>(tag.get(), nodeId);

    SetDomAttributes(ctx, argv[2], *command);
    SetDomStyle(ctx, argv[3], *command);
    AddDomEvent(ctx, argv[4], *command);
    page->PushCommand(command);
    return JS_NULL;
}

int32_t CreateDomElement(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("CreateDomElement");
    if (argv == nullptr) {
        LOGE("the arg is error");
        return -1;
    }

    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("the page is nullptr");
        return -1;
    }
    int32_t nodeId = ++globalNodeId;
    ScopedString tag(ctx, argv[0]);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomElement>(tag.get(), nodeId);
    page->PushCommand(command);
    // Flush command as fragment immediately when pushed too many commands.
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
    return  globalNodeId;
}

JSValue JsDomAddElement(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("JsDomAddElement");

    if ((argv == nullptr) || (argc != ADD_ELEMENT_ARGS_LEN)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }

    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    int32_t instanceId = 0;
    JS_ToInt32(ctx, &instanceId, argv[ADD_ELEMENT_INSTANCE_ID]);
    if (page->GetPageId() != instanceId) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        page = instance->GetDelegate()->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsDomAddElement fail to get page, pageId: %{public}d", instanceId);
            return JS_EXCEPTION;
        }
    }

    int32_t parentNodeId;
    JS_ToInt32(ctx, &parentNodeId, argv[0]);
    parentNodeId = parentNodeId ? parentNodeId : (DOM_ROOT_NODE_ID_BASE + page->GetPageId());

    int32_t nodeId;
    JS_ToInt32(ctx, &nodeId, argv[1]);
    ScopedString tag(ctx, argv[2]);
    LOGD("JsDomAddElement: pageId: %{private}d, parentNodeId: %{private}d, nodeId: %{private}d, tag: %{private}s",
        page->GetPageId(), parentNodeId, nodeId, tag.get());
    auto command = Referenced::MakeRefPtr<JsCommandAddDomElement>(tag.get(), nodeId, parentNodeId);
    SetDomAttributes(ctx, argv[3], *command);
    SetDomStyle(ctx, argv[4], *command);
    AddDomEvent(ctx, argv[5], *command);

    ScopedString customFlag(ctx, argv[6]);
    if (customFlag.get()) {
        std::unique_ptr<JsonValue> customPtr = JsonUtil::ParseJsonString(customFlag.get());
        if (customPtr && customPtr->IsBool()) {
            command->SetIsCustomComponent(customPtr->GetBool());
        }
    }
    int32_t itemIndex;
    JS_ToInt32(ctx, &itemIndex, argv[7]);
    command->SetForIndex(itemIndex);
    page->PushCommand(command);
    // Flush command as fragment immediately when pushed too many commands.
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
    return JS_NULL;
}

// JS Framework calls this function after initial loading of the page has been done.
JSValue JsOnCreateFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsOnCreateFinish");
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }
    page->SetPageCreated();
    return JS_NULL;
}

// JS Framework calls this function after JS framework has finished processing an event.
JSValue JsOnUpdateFinish(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsOnUpdateFinish");
    auto page = GetRunningPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }

    if (page->CheckPageCreated()) {
        QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        instance->GetDelegate()->TriggerPageUpdate(page->GetPageId());
    }
    return JS_NULL;
}

std::string JsParseRouteUrl(JSContext* ctx, JSValueConst argv, const std::string& key)
{
    std::string pageRoute;
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsString()) {
        pageRoute = argsPtr->GetValue(key)->GetString();
    }
    LOGI("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());
    return pageRoute;
}

std::string JsParseRouteUrlSpecial(JSContext* ctx, JSValueConst argv)
{
    std::string pageRoute;
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());

    if (argsPtr->Contains(ROUTE_KEY_URI)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    } else if (argsPtr->Contains(ROUTE_KEY_PATH)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_PATH)->GetString();
    }
    LOGI("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());
    return pageRoute;
}

std::string JsParseRouteParams(JSContext* ctx, JSValueConst argv, const std::string& key)
{
    std::string params;
    ScopedString args(ctx, argv);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsObject()) {
        params = argsPtr->GetValue(key)->ToString();
    }
    return params;
}

int32_t JsParseIntParams(JSContext* ctx, JSValueConst argv, const std::string& key)
{
    ScopedString args(ctx, argv);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsNumber()) {
        return argsPtr->GetValue(key)->GetInt();
    }

    return 0;
}

bool JsParseRouteOverwrite(JSContext* ctx, JSValueConst argv, const std::string& key)
{
    ScopedString args(ctx, argv);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->Contains(key)) {
        return true;
    }
    return false;
}

std::vector<ButtonInfo> JsParseDialogButtons(JSContext* ctx, JSValueConst argv, const std::string& key)
{
    std::vector<ButtonInfo> dialogButtons;
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsArray()) {
        for (int32_t i = 0; i < argsPtr->GetValue(key)->GetArraySize(); ++i) {
            auto button = argsPtr->GetValue(key)->GetArrayItem(i);
            if (!button || !button->GetValue("text")->IsString()) {
                continue;
            }
            ButtonInfo buttonInfo;
            if (button->GetValue("text")) {
                buttonInfo.text = button->GetValue("text")->GetString();
            }
            if (button->GetValue("color")) {
                buttonInfo.textColor = button->GetValue("color")->GetString();
            }
            dialogButtons.emplace_back(buttonInfo);
        }
    }
    return dialogButtons;
}

JSValue EnableAlertBeforeBackPage(JSContext* ctx, JSValueConst argv)
{
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (argsPtr != nullptr && argsPtr->IsObject()) {
        if (argsPtr->GetValue(PROMPT_KEY_MESSAGE) == nullptr || !argsPtr->GetValue(PROMPT_KEY_MESSAGE)->IsString()) {
            LOGE("enableAlertBeforeBackPage message is null");
            const std::string fail = JsParseRouteUrl(ctx, argv, COMMON_FAIL);
            const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
            instance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})", false);
            instance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})", false);
            return JS_NULL;
        }
    } else {
        LOGE("enableAlertBeforeBackPage message is null");
        std::string callBackStr(args.get());
        // Get callbackId and clear redundant symbols
        if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
            callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
            instance->CallJs(callBackStr,
                R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:massage is null"}],"method":"fail"})", false);
        }
        return JS_NULL;
    }

    const std::string message = JsParseRouteUrl(ctx, argv, PROMPT_KEY_MESSAGE);
    const std::string success = JsParseRouteUrl(ctx, argv, COMMON_SUCCESS);
    const std::string fail = JsParseRouteUrl(ctx, argv, COMMON_FAIL);
    const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
    auto callback = [instance, success, fail, complete](int32_t callbackType) {
        switch (callbackType) {
            case 0:
                instance->CallJs(success, R"({"errMsg":"enableAlertBeforeBackPage:ok"})", false);
                instance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:ok"})", false);
                break;
            case 1:
                instance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})", false);
                instance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})", false);
                break;
            default:
                break;
        }
    };
    instance->GetDelegate()->EnableAlertBeforeBackPage(message, std::move(callback));
    return JS_NULL;
}

JSValue DisableAlertBeforeBackPage(JSContext* ctx, JSValueConst argv)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    instance->GetDelegate()->DisableAlertBeforeBackPage();

    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr->IsObject()) {
        const std::string success = JsParseRouteUrl(ctx, argv, COMMON_SUCCESS);
        const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
        instance->CallJs(success, R"({"errMsg":"disableAlertBeforeBackPage:ok"})", false);
        instance->CallJs(complete, R"({"errMsg":"disableAlertBeforeBackPage:ok"})", false);
        return JS_NULL;
    }

    std::string callBackStr(args.get());
    // Get callbackId and clear redundant symbols
    if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
        callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
        instance->CallJs(
            callBackStr, R"({"arguments":[{"errMsg":"disableAlertBeforeBackPage:ok"}],"method":"success"})", false);
    }
    return JS_NULL;
}

JSValue PostponePageTransition(JSContext* ctx, JSValueConst argv)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    instance->GetDelegate()->PostponePageTransition();
    return JS_NULL;
}

JSValue LaunchPageTransition(JSContext* ctx, JSValueConst argv)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    instance->GetDelegate()->LaunchPageTransition();
    return JS_NULL;
}

JSValue JsHandlePageRoute(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    std::string uri = "";
    if (methodName == ROUTE_PAGE_BACK) {
        uri = JsParseRouteUrlSpecial(ctx, argv);
    } else {
        uri = JsParseRouteUrl(ctx, argv, ROUTE_KEY_URI);
    }
    std::string params = JsParseRouteParams(ctx, argv, ROUTE_KEY_PARAMS);
    bool dontOverwrite = JsParseRouteOverwrite(ctx, argv, ROUTE_KEY_DONT_OVERWRITE);

    std::unique_ptr<JsonValue> routerParamsData = JsonUtil::Create(true);
    routerParamsData->Put("paramsData", JsonUtil::ParseJsonString(params));
    routerParamsData->Put("dontOverwrite", dontOverwrite);
    params = routerParamsData->ToString();

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    // Operator map for page route.
    static const LinearMapNode<JSValue (*)(const std::string&, const std::string&, QjsEngineInstance&)>
        pageRouteOperators[] = {
            { ROUTE_PAGE_BACK,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    LOGD("JsBackRoute uri = %{private}s", uri.c_str());
                    instance.GetDelegate()->Back(uri);
                    return JS_NULL;
                } },
            { ROUTE_PAGE_CLEAR,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    LOGD("Clear Page Route");
                    instance.GetDelegate()->Clear();
                    return JS_NULL;
                } },
            { ROUTE_PAGE_GET_LENGTH,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    int32_t routeLength = instance.GetDelegate()->GetStackSize();
                    std::string indexLength = std::to_string(routeLength);
                    LOGD("JsGetLengthRoute routeLength=%{private}s", indexLength.c_str());
                    auto ctx = instance.GetQjsContext();
                    return JS_NewString(ctx, indexLength.c_str());
                } },
            { ROUTE_PAGE_GET_STATE,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    int32_t routeIndex = 0;
                    std::string routeName;
                    std::string routePath;
                    instance.GetDelegate()->GetState(routeIndex, routeName, routePath);
                    LOGD("JsGetStateRoute index=%{private}d name=%{private}s path=%{private}s", routeIndex,
                        routeName.c_str(), routePath.c_str());

                    auto ctx = instance.GetQjsContext();
                    JSValue routeData = JS_NewObject(ctx);
                    JS_SetPropertyStr(ctx, routeData, "index", JS_NewInt32(ctx, routeIndex));
                    JS_SetPropertyStr(ctx, routeData, "name", JS_NewString(ctx, routeName.c_str()));
                    JS_SetPropertyStr(ctx, routeData, "path", JS_NewString(ctx, routePath.c_str()));
                    return routeData;
                } },
            { ROUTE_PAGE_PUSH,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    LOGD("JsPushRoute uri = %{private}s", uri.c_str());
                    instance.GetDelegate()->Push(uri, params);
                    return JS_NULL;
                } },
            { ROUTE_PAGE_REPLACE,
                [](const std::string& uri, const std::string& params, QjsEngineInstance& instance) {
                    LOGD("JsReplaceRoute uri = %{private}s", uri.c_str());
                    instance.GetDelegate()->Replace(uri, params);
                    return JS_NULL;
                } },
        };
    auto operatorIter = BinarySearchFindIndex(pageRouteOperators, ArraySize(pageRouteOperators), methodName.c_str());
    if (operatorIter != -1) {
        return pageRouteOperators[operatorIter].value(uri, params, *instance);
    } else if (methodName == ROUTE_ENABLE_ALERT_BEFORE_BACK_PAGE) {
        return EnableAlertBeforeBackPage(ctx, argv);
    } else if (methodName == ROUTE_DISABLE_ALERT_BEFORE_BACK_PAGE) {
        return DisableAlertBeforeBackPage(ctx, argv);
    } else if (methodName == ROUTE_POSTPONE) {
        return PostponePageTransition(ctx, argv);
    } else if (methodName == ROUTE_LAUNCH) {
        return LaunchPageTransition(ctx, argv);
    } else {
        LOGW("system.router not support method = %{private}s", methodName.c_str());
    }
    return JS_NULL;
}

JSValue JsShowToast(JSContext* ctx, JSValueConst argv)
{
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    std::string message;
    int32_t duration = 0;
    std::string bottom;
    if (argsPtr != nullptr) {
        if (argsPtr->GetValue(PROMPT_KEY_MESSAGE) != nullptr) {
            if (argsPtr->GetValue(PROMPT_KEY_MESSAGE)->IsString()) {
                message = argsPtr->GetValue(PROMPT_KEY_MESSAGE)->GetString();
            } else {
                message = argsPtr->GetValue(PROMPT_KEY_MESSAGE)->ToString();
            }
        }
        if (argsPtr->GetValue(PROMPT_KEY_DURATION) != nullptr && argsPtr->GetValue(PROMPT_KEY_DURATION)->IsNumber()) {
            duration = argsPtr->GetValue(PROMPT_KEY_DURATION)->GetInt();
        }
        if (argsPtr->GetValue(PROMPT_KEY_BOTTOM) != nullptr) {
            if (argsPtr->GetValue(PROMPT_KEY_BOTTOM)->IsString()) {
                bottom = argsPtr->GetValue(PROMPT_KEY_BOTTOM)->GetString();
            } else if (argsPtr->GetValue(PROMPT_KEY_BOTTOM)->IsNumber()) {
                bottom = std::to_string(argsPtr->GetValue(PROMPT_KEY_BOTTOM)->GetInt());
            }
        }
    }
    LOGD("JsShowToast message = %{private}s duration = %{private}d bottom = %{private}s", message.c_str(), duration,
        bottom.c_str());
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    ACE_DCHECK(instance);
    instance->GetDelegate()->ShowToast(message, duration, bottom);
    return JS_NULL;
}

JSValue JsShowDialog(JSContext* ctx, JSValueConst argv)
{
    const std::string title = JsParseRouteUrl(ctx, argv, PROMPT_KEY_TITLE);
    const std::string message = JsParseRouteUrl(ctx, argv, PROMPT_KEY_MESSAGE);
    std::vector<ButtonInfo> buttons = JsParseDialogButtons(ctx, argv, PROMPT_KEY_BUTTONS);
    const std::string success = JsParseRouteUrl(ctx, argv, COMMON_SUCCESS);
    const std::string cancel = JsParseRouteUrl(ctx, argv, COMMON_CANCEL);
    const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
    bool autoCancel = true;

    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->GetValue(PROMPT_DIALOG_AUTO_CANCEL) != nullptr &&
        argsPtr->GetValue(PROMPT_DIALOG_AUTO_CANCEL)->IsBool()) {
        autoCancel = argsPtr->GetValue(PROMPT_DIALOG_AUTO_CANCEL)->GetBool();
    }

    std::set<std::string> callbacks;
    if (!success.empty()) {
        callbacks.emplace(COMMON_SUCCESS);
    }
    if (!cancel.empty()) {
        callbacks.emplace(COMMON_CANCEL);
    }
    if (!complete.empty()) {
        callbacks.emplace(COMMON_COMPLETE);
    }

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto callback = [instance, success, cancel, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                instance->CallJs(success.c_str(),
                    std::string("{\"index\":").append(std::to_string(successType)).append("}").c_str(), false);
                break;
            case 1:
                instance->CallJs(cancel.c_str(), std::string("\"cancel\",null").c_str(), false);
                break;
            case 2:
                instance->CallJs(complete.c_str(), std::string("\"complete\",null").c_str(), false);
                break;
            default:
                break;
        }
    };
    instance->GetDelegate()->ShowDialog(title, message, buttons, autoCancel, std::move(callback), callbacks);
    return JS_NULL;
}

JSValue JsShowActionMenu(JSContext* ctx, JSValueConst argv)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));

    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr == nullptr) {
        LOGE("argsPtr is nullptr");
        return JS_NULL;
    }

    std::vector<ButtonInfo> buttons = JsParseDialogButtons(ctx, argv, PROMPT_KEY_BUTTONS);
    // The number of buttons cannot be zero or more than six
    if (buttons.empty() || buttons.size() > 6) {
        LOGE("buttons is invalid");
        if (argsPtr->IsObject()) {
            const std::string fail = JsParseRouteUrl(ctx, argv, COMMON_FAIL);
            const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
            instance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})", false);
            instance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})", false);
        } else {
            std::string callBackStr(args.get());
            // Get callbackId and clear redundant symbols
            if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
                callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
                instance->CallJs(callBackStr,
                    R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:buttons is invalid"}],"method":"fail"})",
                    false);
            }
        }
        return JS_NULL;
    }

    const std::string title = JsParseRouteUrl(ctx, argv, PROMPT_KEY_TITLE);
    const std::string success = JsParseRouteUrl(ctx, argv, COMMON_SUCCESS);
    const std::string fail = JsParseRouteUrl(ctx, argv, COMMON_FAIL);
    const std::string complete = JsParseRouteUrl(ctx, argv, COMMON_COMPLETE);
    auto callback = [instance, success, fail, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                instance->CallJs(success.c_str(), std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                    .append(std::to_string(successType)).append("}").c_str(), false);
                instance->CallJs(complete.c_str(), std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                    .append(std::to_string(successType)).append("}").c_str(), false);
                break;
            case 1:
                instance->CallJs(fail, R"({"errMsg":"showActionMenu:fail cancel"})", false);
                instance->CallJs(complete, R"({"errMsg":"showActionMenu:fail cancel"})", false);
                break;
            default:
                LOGE("callbackType is invalid");
                break;
        }
    };
    instance->GetDelegate()->ShowActionMenu(title, buttons, std::move(callback));
    return JS_NULL;
}

JSValue JsHandlePrompt(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    if (methodName == PROMPT_SHOW_TOAST) {
        return JsShowToast(ctx, argv);
    } else if (methodName == PROMPT_SHOW_DIALOG) {
        return JsShowDialog(ctx, argv);
    } else if (methodName == PROMPT_SHOW_ACTION_MENU) {
        return JsShowActionMenu(ctx, argv);
    } else {
        LOGW("system.prompt not support method = %{private}s", methodName.c_str());
    }
    return JS_NULL;
}

JSValue JsHandleAnimationFrame(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }
    const char* callbackIdJsStr = JS_ToCString(ctx, argv);
    if (callbackIdJsStr == nullptr) {
        LOGW("system animation callbackId is null");
        return JS_NULL;
    }
    if (methodName == ANIMATION_REQUEST_ANIMATION_FRAME) {
        instance->GetDelegate()->RequestAnimationFrame(std::string(callbackIdJsStr));
    } else if (methodName == ANIMATION_CANCEL_ANIMATION_FRAME) {
        instance->GetDelegate()->CancelAnimationFrame(std::string(callbackIdJsStr));
    } else {
        LOGW("animationFrame not support method = %{private}s", methodName.c_str());
    }
    JS_FreeCString(ctx, callbackIdJsStr);
    return JS_NULL;
}

JSValue JsHandleAnimator(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }
    const char* args = JS_ToCString(ctx, argv);
    auto page = GetStagingPage(ctx);
    if (!page) {
        LOGE("JsHandleAnimator failed, page is null.");
        return JS_NULL;
    }
    auto resultAnimator = JSValue();
    if (methodName == ANIMATOR_CREATE_ANIMATOR) {
        int32_t bridgeId = AnimatorBridgeUtils::JsCreateBridgeId();
        resultAnimator = AnimatorBridgeUtils::CreateAnimatorContext(ctx, page->GetPageId(), bridgeId);
        auto animatorBridge = AceType::MakeRefPtr<AnimatorBridge>(ctx, resultAnimator);
        auto task = AceType::MakeRefPtr<AnimatorTaskCreate>(animatorBridge, args);
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
    }
    return resultAnimator;
}

JSValue JsAddListener(JSContext* ctx, JSValueConst argv)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (JS_IsObject(argv) && JS_IsArray(ctx, argv)) {
        JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
        if (len == 0) {
            js_free(ctx, pTab);
            return JS_NULL;
        }
        JSValue jsListenerId = JS_GetProperty(ctx, argv, pTab[0].atom);
        const char* listenerId = JS_ToCString(ctx, jsListenerId);
        auto mediaQuery = instance->GetDelegate()->GetMediaQueryInfoInstance();
        if (mediaQuery) {
            mediaQuery->SetListenerId(std::string(listenerId));
        }
        JS_FreeCString(ctx, listenerId);
        JS_FreeValue(ctx, jsListenerId);
        js_free(ctx, pTab);
    }
    return JS_NULL;
}

void JsParseCallbackResult(JSContext* ctx, JSValueConst jsResult, const std::string& key, std::string& result)
{
    ScopedString args(ctx, jsResult);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(args.get());
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr) {
        if (argsPtr->GetValue(key)->IsString()) {
            result = argsPtr->GetValue(key)->GetString();
        } else if (argsPtr->GetValue(key)->IsNumber()) {
            result = argsPtr->GetValue(key)->ToString();
        }
    }
}

JSValue JsHandleCallback(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    if (methodName == CALLBACK_NATIVE) {
        if (JS_IsObject(argv) && JS_IsArray(ctx, argv)) {
            JSValue jsCallbackId = JS_GetPropertyUint32(ctx, argv, 0);
            JSValue jsResult = JS_GetPropertyUint32(ctx, argv, 1);
            const char* callbackId = JS_ToCString(ctx, jsCallbackId);
            std::string result = JS_ToCString(ctx, jsResult);
            JsParseCallbackResult(ctx, jsResult, KEY_STEPPER_PENDING_INDEX, result);
            auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
            ACE_DCHECK(instance);
            instance->GetDelegate()->SetCallBackResult(std::string(callbackId), result);

            JS_FreeCString(ctx, callbackId);
            JS_FreeValue(ctx, jsCallbackId);
            JS_FreeValue(ctx, jsResult);
        }
    } else if (methodName == APP_DESTROY_FINISH) {
        LOGD("JsHandleCallback: appDestroyFinish should release resource here");
    } else {
        LOGW("internal.jsResult not support method = %{private}s", methodName.c_str());
    }
    js_std_loop(ctx);
    return JS_NULL;
}

JSValue JsHandleImage(JSContext* ctx, JSValueConst argv)
{
    auto src = JsParseRouteUrl(ctx, argv, "src");
    auto success = JsParseRouteUrl(ctx, argv, "success");
    auto fail = JsParseRouteUrl(ctx, argv, "fail");

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }

    auto&& callback = [instance, success, fail](bool callbackType, int32_t width, int32_t height) {
        if (callbackType) {
            instance->CallJs(success,
                std::string("{\"width\":")
                    .append(std::to_string(width))
                    .append(", \"height\":")
                    .append(std::to_string(height))
                    .append("}"),
                false);
        } else {
            instance->CallJs(fail, std::string("\"fail\",null"), false);
        }
    };
    instance->GetDelegate()->HandleImage(src, callback);
    return JS_NULL;
}

bool ParseResourceStringParam(std::string& str, const char* paramName, JSContext* ctx, const JSValue& jsObject)
{
    if (JS_IsObject(jsObject)) {
        JSValue strValue = JS_GetPropertyStr(ctx, jsObject, paramName);
        if (JS_IsString(strValue)) {
            ScopedString uriValueStr(ctx, strValue);
            str = uriValueStr.get();
            return true;
        }
        JS_FreeValue(ctx, strValue);
    }
    return false;
}

bool ParseResourceNumberParam(int32_t& num, const char* paramName, JSContext* ctx, const JSValue& jsObject)
{
    if (JS_IsObject(jsObject)) {
        JSValue numValue = JS_GetPropertyStr(ctx, jsObject, paramName);
        if (JS_IsUndefined(numValue)) {
            JS_FreeValue(ctx, numValue);
            return false;
        }
        ScopedString numValueStr(ctx, numValue);
        num = ParseResourceInputNumberParam(numValueStr.get());
        return true;
    }
    return false;
}

void ParseResourceParam(std::string& uri, int32_t& position, int32_t& length, JSContext* ctx, const JSValue& jsObject)
{
    ParseResourceStringParam(uri, READ_KEY_URI, ctx, jsObject);
    ParseResourceNumberParam(position, READ_KEY_POSITION, ctx, jsObject);
    ParseResourceNumberParam(length, READ_KEY_LENGTH, ctx, jsObject);
}

JSValue JsReadArrayBuffer(JSContext* ctx, JSValueConst argv)
{
    if (!JS_IsObject(argv) || !JS_IsArray(ctx, argv)) {
        LOGE("JsReadArrayBuffer: argv is not illegal");
        return JS_NULL;
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
    if (len < ARGS_READ_RESOURCE_LENGTH) {
        LOGE("JsReadArrayBuffer: invalid callback value");
        js_free(ctx, pTab);
        return JS_EXCEPTION;
    }

    JSValue jsObject = JS_GetProperty(ctx, argv, pTab[0].atom);
    std::string uri;
    int32_t position = 1;
    int32_t length = MAX_READ_TEXT_LENGTH;
    ParseResourceParam(uri, position, length, ctx, jsObject);
    JSValue jsCallbackId = JS_GetProperty(ctx, argv, pTab[1].atom);
    JS_FreeValue(ctx, jsObject);
    js_free(ctx, pTab);
    ScopedString callbackId(ctx, jsCallbackId);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        LOGE("JsReadArrayBuffer file uri pattern not correct");
        instance->CallJs(callbackId.get(),
            R"({"arguments":["file uri pattern not correct",202],"method":"fail"})");
        JS_FreeValue(ctx, jsCallbackId);
        return JS_NULL;
    }

    std::vector<uint8_t> binaryContent;
    if (!(instance->GetDelegate()->GetResourceData(uri, binaryContent))) {
        instance->CallJs(callbackId.get(),
            R"({"arguments":["read file failed",301],"method":"fail"})");
        JS_FreeValue(ctx, jsCallbackId);
        return JS_NULL;
    }

    auto fileLength = static_cast<int32_t>(binaryContent.size());
    if ((position > fileLength) || (position <= 0) || (length <= 0)) {
        instance->CallJs(callbackId.get(),
            R"({"arguments":["wrong start position or wrong read length", 202],"method":"fail"})");
        JS_FreeValue(ctx, jsCallbackId);
        return JS_NULL;
    }

    length = position + length - 1 > fileLength ? fileLength - position + 1 : length;
    JSValue binaryData = JS_NewObject(ctx);
    JSValue binaryArray = JS_NewArrayBufferCopy(ctx, &binaryContent[position - 1], length);
    JS_SetPropertyStr(ctx, binaryData, "buffer", binaryArray);
    instance->CallJs(callbackId.get(), R"({"arguments":["read array buffer success"],"method":"success"})");
    js_std_loop(ctx);
    return binaryData;
}

JSValue JsReadText(JSContext* ctx, JSValueConst argv)
{
    if (!JS_IsObject(argv) || !JS_IsArray(ctx, argv)) {
        LOGE("JsReadText: argv is not illegal");
        return JS_NULL;
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
    if (len < ARGS_READ_RESOURCE_LENGTH) {
        LOGE("JsReadText: invalid callback value");
        js_free(ctx, pTab);
        return JS_EXCEPTION;
    }

    JSValue jsObject = JS_GetProperty(ctx, argv, pTab[0].atom);
    std::string uri;
    ParseResourceStringParam(uri, READ_KEY_URI, ctx, jsObject);
    JSValue jsCallbackId = JS_GetProperty(ctx, argv, pTab[1].atom);
    ScopedString callbackId(ctx, jsCallbackId);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        instance->CallJs(callbackId.get(),
            R"({"arguments":["file uri pattern not correct",202],"method":"fail"})");
        JS_FreeValue(ctx, jsCallbackId);
        JS_FreeValue(ctx, jsObject);
        js_free(ctx, pTab);
        return JS_NULL;
    }

    std::string fileText;
    if (!(instance->GetDelegate()->GetResourceData(uri, fileText))) {
        instance->CallJs(callbackId.get(), R"({"arguments":["read file failed",301],"method":"fail"})");
        JS_FreeValue(ctx, jsCallbackId);
        JS_FreeValue(ctx, jsObject);
        js_free(ctx, pTab);
        return JS_NULL;
    }

    int32_t position = 0;
    int32_t length = 0;
    auto fileLength = ParseUtf8TextLength(fileText);
    if (!ParseResourceNumberParam(position, READ_KEY_POSITION, ctx, jsObject)) {
        position = 1;
    }

    if (!ParseResourceNumberParam(length, READ_KEY_LENGTH, ctx, jsObject) || (length > fileLength - position + 1)) {
        length = (fileLength - position + 1 <= 0) ? 0 : fileLength - position + 1;
    }

    if (fileLength == 0) {
        if ((position <= 0) || (length < 0)) {
            instance->CallJs(callbackId.get(),
                R"({"arguments":["wrong start position or wrong read length",202],"method":"fail"})");
            JS_FreeValue(ctx, jsCallbackId);
            JS_FreeValue(ctx, jsObject);
            js_free(ctx, pTab);
            return JS_NULL;
        }
    } else {
        if ((position > fileLength) || (position <= 0) || (length < 0)) {
            instance->CallJs(callbackId.get(),
                R"({"arguments":["wrong start position or wrong read length",202],"method":"fail"})");
            JS_FreeValue(ctx, jsCallbackId);
            JS_FreeValue(ctx, jsObject);
            js_free(ctx, pTab);
            return JS_NULL;
        }

        auto substrPos = ParseUtf8TextSubstrStartPos(fileText, position);
        auto substrEndPos = ParseUtf8TextSubstrEndPos(fileText, position + length - 1);
        if (substrPos - 1 < 0 || substrEndPos - substrPos < 0) {
            LOGE("JsReadText: Invalid value of substrPos: %{public}d and substrEndPos: %{public}d", substrPos,
                substrEndPos);
            return JS_NULL;
        }
        fileText =
            fileText.substr(static_cast<uint32_t>(substrPos - 1), static_cast<uint32_t>(substrEndPos - substrPos + 1));
        HandleEscapeCharaterInUtf8TextForJson(fileText);
    }

    instance->CallJs(callbackId.get(),
        std::string("{\"arguments\":[").append("{\"text\":\"").append(fileText).append("\"}],\"method\":\"success\"}"),
        false);
    JS_FreeValue(ctx, jsCallbackId);
    JS_FreeValue(ctx, jsObject);
    js_std_loop(ctx);
    js_free(ctx, pTab);
    return JS_NULL;
}

JSValue JsHandleReadResource(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    LOGD("JsHandleReadResource");
    if (methodName == READ_TEXT) {
        return JsReadText(ctx, argv);
    } else if (methodName == READ_ARRAY_BUFFER) {
        return JsReadArrayBuffer(ctx, argv);
    } else {
        LOGW("system.resource not support method = %{private}s", methodName.c_str());
    }
    return JS_NULL;
}

void JsSetTimer(JSContext* ctx, JSValueConst argv, bool isInterval)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (JS_IsObject(argv) && JS_IsArray(ctx, argv)) {
        JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
        if (len < 2) {
            LOGW("JsSetTimer: invalid callback value");
            return;
        }

        JSValue jsCallBackId = JS_GetProperty(ctx, argv, pTab[0].atom);
        JSValue jsDelay = JS_GetProperty(ctx, argv, pTab[1].atom);
        const char* callBackId = JS_ToCString(ctx, jsCallBackId);
        const char* delay = JS_ToCString(ctx, jsDelay);
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        LOGD("JsSetTimer callbackId: %{private}s, delay: %{private}s", callBackId, delay);
        instance->GetDelegate()->WaitTimer(std::string(callBackId), std::string(delay), isInterval, true);

        JS_FreeCString(ctx, callBackId);
        JS_FreeCString(ctx, delay);
        JS_FreeValue(ctx, jsCallBackId);
        JS_FreeValue(ctx, jsDelay);
        js_free(ctx, pTab);
    }
}

void JsClearTimeout(JSContext* ctx, JSValueConst argv)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (JS_IsObject(argv) && JS_IsArray(ctx, argv)) {
        JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
        if (len < 1) {
            LOGW("JsClearTimeout: invalid callback value");
            return;
        }

        JSValue jsCallBackId = JS_GetProperty(ctx, argv, pTab[0].atom);
        const char* callBackId = JS_ToCString(ctx, jsCallBackId);
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        ACE_DCHECK(instance);
        LOGD("ClearTimeOut callBackId: %{private}s", callBackId);
        instance->GetDelegate()->ClearTimer(std::string(callBackId));

        JS_FreeCString(ctx, callBackId);
        JS_FreeValue(ctx, jsCallBackId);
        js_free(ctx, pTab);
    }
    js_std_loop(ctx);
}

JSValue JsSetLocale(JSContext* ctx, JSValueConst argv)
{
    ScopedString args(ctx, argv);
    std::unique_ptr<JsonValue> localeJson = JsonUtil::ParseJsonString(args.get());
    if (localeJson) {
        std::string language;
        if (localeJson->GetValue(LOCALE_LANGUAGE) != nullptr && localeJson->GetValue(LOCALE_LANGUAGE)->IsString()) {
            language = localeJson->GetValue(LOCALE_LANGUAGE)->GetString();
        }
        std::string countryOrRegion;
        if (localeJson->GetValue(LOCALE_COUNTRY_OR_REGION) != nullptr &&
            localeJson->GetValue(LOCALE_COUNTRY_OR_REGION)->IsString()) {
            countryOrRegion = localeJson->GetValue(LOCALE_COUNTRY_OR_REGION)->GetString();
        }
        if (!countryOrRegion.empty() && !language.empty()) {
            auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
            ACE_DCHECK(instance);
            instance->ChangeLocale(language, countryOrRegion);
        }
    }
    return JS_NULL;
}

JSValue JsHandleSetTimeout(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    if (methodName == SET_TIMEOUT) {
        JsSetTimer(ctx, argv, false);
    } else if (methodName == CLEAR_TIMEOUT || methodName == CLEAR_INTERVAL) {
        JsClearTimeout(ctx, argv);
    } else if (methodName == SET_INTERVAL) {
        JsSetTimer(ctx, argv, true);
    } else {
        LOGW("Unsupported method for timer module!");
        return JS_NULL;
    }
    return JS_NULL;
}

JSValue JsHandleMediaQuery(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    if (methodName == ADD_LISTENER) {
        JsAddListener(ctx, argv);
    } else if (methodName == GET_DEVICE_TYPE) {
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        if (instance != nullptr) {
            auto mediaQuery = instance->GetDelegate()->GetMediaQueryInfoInstance();
            if (mediaQuery) {
                return JS_NewString(ctx, mediaQuery->GetDeviceType().c_str());
            }
        }
    } else {
        LOGW("system.mediaquery not support method = %{private}s", methodName.c_str());
    }
    return JS_NULL;
}

JSValue GetAppInfo(JSContext* ctx)
{
    QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    JSValue appID = JS_NewString(ctx, instance->GetDelegate()->GetAppID().c_str());
    JSValue appName = JS_NewString(ctx, instance->GetDelegate()->GetAppName().c_str());
    JSValue versionName = JS_NewString(ctx, instance->GetDelegate()->GetVersionName().c_str());
    JSValue versionCode = JS_NewInt32(ctx, instance->GetDelegate()->GetVersionCode());

    JSValue resData = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, resData, "appID", appID);
    JS_SetPropertyStr(ctx, resData, "appName", appName);
    JS_SetPropertyStr(ctx, resData, "versionName", versionName);
    JS_SetPropertyStr(ctx, resData, "versionCode", versionCode);

    return resData;
}

JSValue Terminate(JSContext* ctx)
{
    QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    WeakPtr<PipelineContext> pipelineContextWeak = instance->GetDelegate()->GetPipelineContext();
    auto uiTaskExecutor = instance->GetDelegate()->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->Finish();
        }
    });
    return JS_NULL;
}

void ParseFullWindowParams(JSContext* ctx, JSValue params, std::string& duration)
{
    JSPropertyEnum* tab = nullptr;
    uint32_t paramLen = 0;
    if (JS_IsObject(params)) {
        JS_GetOwnPropertyNames(ctx, &tab, &paramLen, params, JS_GPN_STRING_MASK);
        const char* jsDurationKey = JS_AtomToCString(ctx, tab[0].atom);
        if (jsDurationKey == nullptr) {
            JS_FreeAtom(ctx, tab[0].atom);
            js_free(ctx, tab);
            LOGE("jsDurationKey is null.");
            return;
        }
        if (std::strcmp(jsDurationKey, "duration") == 0) {
            JSValue valObject = JS_GetProperty(ctx, params, tab[0].atom);
            if (JS_IsString(valObject) || JS_IsNumber(valObject)) {
                ScopedString styleVal(ctx, valObject);
                const char* valDuration = styleVal.get();
                duration = valDuration;
            }
            JS_FreeValue(ctx, valObject);
        }
        JS_FreeAtom(ctx, tab[0].atom);
        JS_FreeCString(ctx, jsDurationKey);
    }
    js_free(ctx, tab);
}

JSValue RequestFullWindow(JSContext* ctx, JSValueConst argv)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    int32_t duration = -1; // default scene
    if (JS_IsObject(argv) && JS_IsArray(ctx, argv)) {
        JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
        if (len < ARGS_FULL_WINDOW_LENGTH) {
            LOGW("RequestFullWindow: invalid callback value");
            js_free(ctx, pTab);
            return JS_NULL;
        }

        if (len == CUSTOM_FULL_WINDOW_LENGTH) {
            JSValue jsDuration = JS_GetProperty(ctx, argv, pTab[0].atom);
            std::string valDuration;
            ParseFullWindowParams(ctx, jsDuration, valDuration);
            if (!valDuration.empty()) {
                duration = StringToInt(valDuration);
            }
            if (duration < 0) {
                duration = -1; // default scene
            }
            JS_FreeValue(ctx, jsDuration);
        }
        js_free(ctx, pTab);
    }
    QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    WeakPtr<PipelineContext> pipelineContextWeak = instance->GetDelegate()->GetPipelineContext();
    auto uiTaskExecutor = instance->GetDelegate()->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, duration]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->RequestFullWindow(duration);
        }
    });
    return JS_NULL;
}

void GetPackageInfoCallback(JSContext* ctx, JSValueConst jsMessage, const char* callbackId)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (!JS_IsObject(jsMessage)) {
        LOGE("GetPackageInfoCallback: jsMessage is not Object");
        instance->CallJs(
            callbackId, std::string("{\"arguments\":[\"jsMessage is not Object\",200],\"method\":\"fail\"}"), false);
        return;
    }
    JSValue jsPackageName = JS_GetPropertyStr(ctx, jsMessage, APP_PACKAGE_NAME);
    if (!JS_IsString(jsPackageName)) {
        LOGE("GetPackageInfoCallback: package name is not String");
        instance->CallJs(callbackId,
            std::string("{\"arguments\":[\"packageName is not available string\",202],\"method\":\"fail\"}"), false);
    } else {
        ScopedString packageName(ctx, jsPackageName);
        AceBundleInfo bundleInfo;
        if (AceApplicationInfo::GetInstance().GetBundleInfo(packageName.get(), bundleInfo)) {
            auto infoList = JsonUtil::Create(true);
            infoList->Put("versionName", bundleInfo.versionName.c_str());
            infoList->Put("versionCode", std::to_string(bundleInfo.versionCode).c_str());
            instance->CallJs(callbackId,
                std::string("{\"arguments\":[").append(infoList->ToString()).append("],\"method\":\"success\"}"),
                false);
        } else {
            LOGE("can not get info by GetBundleInfo");
            instance->CallJs(
                callbackId, std::string("{\"arguments\":[\"can not get info\",200],\"method\":\"fail\"}"), false);
        }
    }
    JS_FreeValue(ctx, jsPackageName);
}

JSValue GetPackageInfo(JSContext* ctx, JSValueConst argv)
{
    if (!JS_IsObject(argv)) {
        LOGE("GetPackageInfo: argv is not Object");
        return JS_NULL;
    }

    if (!JS_IsArray(ctx, argv)) {
        LOGE("GetPackageInfo: argv is not Array");
        return JS_NULL;
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;

    JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
    if (len < 2) {
        LOGE("GetPackageInfo: invalid callback value");
        js_free(ctx, pTab);
        return JS_EXCEPTION;
    }

    JSValue jsMessage = JS_GetProperty(ctx, argv, pTab[0].atom);
    JSValue jsCallbackId = JS_GetProperty(ctx, argv, pTab[1].atom);
    const char* callbackId = JS_ToCString(ctx, jsCallbackId);
    LOGD("system app getPackageInfo callBackID is %{public}s", callbackId);
    GetPackageInfoCallback(ctx, jsMessage, callbackId);

    JS_FreeCString(ctx, callbackId);
    JS_FreeValue(ctx, jsMessage);
    JS_FreeValue(ctx, jsCallbackId);
    js_free(ctx, pTab);

    return JS_NULL;
}

JSValue SetScreenOnVisible(JSContext* ctx, JSValueConst argv)
{
    if (!JS_IsObject(argv) || !JS_IsArray(ctx, argv)) {
        LOGE("SetScreenOnVisible: argv is not illegal");
        return JS_NULL;
    }
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
    if (len < 2) {
        LOGE("SetScreenOnVisible: invalid callback value");
        js_free(ctx, pTab);
        return JS_EXCEPTION;
    }
    JSValue jsObject = JS_GetProperty(ctx, argv, pTab[0].atom);
    std::string flag = "false";
    if (JS_IsObject(jsObject)) {
        JSValue flagValue = JS_GetPropertyStr(ctx, jsObject, APP_SCREEN_ON_VISIBLE_FLAG);
        if (JS_IsBool(flagValue)) {
            ScopedString flagValueStr(ctx, flagValue);
            flag = flagValueStr.get();
        }
    }
    JSValue jsCallbackId = JS_GetProperty(ctx, argv, pTab[1].atom);
    ScopedString callbackId(ctx, jsCallbackId);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (StringToBool(flag)) {
        instance->CallJs(callbackId.get(), R"({"arguments":[], "method":"success"})");
    } else {
        instance->CallJs(
            callbackId.get(), R"({"arguments":["fail to set false flag in rich platform", 200],"method":"fail"})");
    }
    JS_FreeValue(ctx, jsObject);
    JS_FreeValue(ctx, jsCallbackId);
    js_free(ctx, pTab);
    return JS_NULL;
}

JSValue SetSwipeToDismiss(JSContext* ctx, JSValueConst argv)
{
    if (!JS_IsObject(argv) || !JS_IsArray(ctx, argv)) {
        LOGE("SetSwipeToDismiss: argv is not illegal");
        return JS_NULL;
    }

    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    JS_GetOwnPropertyNames(ctx, &pTab, &len, argv, JS_GPN_STRING_MASK);
    if (len < 2) {
        LOGE("SetSwipeToDismiss: invalid callback value");
        js_free(ctx, pTab);
        return JS_EXCEPTION;
    }

    bool forbideQuit = false;
    JSValue jsObject = JS_GetProperty(ctx, argv, pTab[0].atom);
    if (JS_IsBool(jsObject)) {
        forbideQuit = JS_ToBool(ctx, jsObject);
    }

    QjsEngineInstance* instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    WeakPtr<PipelineContext> pipelineContextWeak = instance->GetDelegate()->GetPipelineContext();
    auto uiTaskExecutor = instance->GetDelegate()->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, forbideQuit]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->SetForbidePlatformQuit(forbideQuit);
        }
    });

    JS_FreeValue(ctx, jsObject);
    js_free(ctx, pTab);
    return JS_NULL;
}

JSValue JsHandleAppApi(JSContext* ctx, JSValueConst argv, const std::string& methodName)
{
    if (methodName == APP_GET_INFO) {
        return GetAppInfo(ctx);
    } else if (methodName == APP_TERMINATE) {
        return Terminate(ctx);
    } else if (methodName == APP_GET_PACKAGE_INFO) {
        return GetPackageInfo(ctx, argv);
    } else if (methodName == APP_REQUEST_FULL_WINDOW) {
        return RequestFullWindow(ctx, argv);
    } else if (methodName == APP_SCREEN_ON_VISIBLE) {
        return SetScreenOnVisible(ctx, argv);
    } else if (methodName == APP_SET_SWIPE_TO_DISMISS) {
        return SetSwipeToDismiss(ctx, argv);
    } else {
        return JS_NULL;
    }
}

JSValue JsGetDeviceInfo(JSContext* ctx, JSValueConst argv)
{
    const char* callbackId = JS_ToCString(ctx, argv);
    if (callbackId == nullptr) {
        LOGE("system device getInfo callBackID is null");
    } else {
        LOGD("system device getInfo callBackID = %{public}s", callbackId);
        auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
        std::pair<std::string, bool> ret = PluginBridge::GetDeviceInfo();
        if (ret.second) {
            instance->CallJs(callbackId,
                std::string("{\"arguments\":[").append(ret.first).append("],\"method\":\"success\"}"), false);
        } else {
            instance->CallJs(callbackId,
                std::string("{\"arguments\":[").append(ret.first).append(",200],\"method\":\"fail\"}"), false);
        }
        JS_FreeCString(ctx, callbackId);
    }

    return JS_NULL;
}

JSValue GetLocale(JSContext* ctx)
{
    JSValue language = JS_NewString(ctx, AceApplicationInfo::GetInstance().GetLanguage().c_str());
    JSValue countryOrRegion = JS_NewString(ctx, AceApplicationInfo::GetInstance().GetCountryOrRegion().c_str());
    JSValue dir = JS_NewString(
        ctx, AceApplicationInfo::GetInstance().IsRightToLeft() ? LOCALE_TEXT_DIR_RTL : LOCALE_TEXT_DIR_LTR);
    JSValue unicodeSetting = JS_NewString(ctx, AceApplicationInfo::GetInstance().GetUnicodeSetting().c_str());

    JSValue resData = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, resData, LOCALE_LANGUAGE, language);
    JS_SetPropertyStr(ctx, resData, LOCALE_COUNTRY_OR_REGION, countryOrRegion);
    JS_SetPropertyStr(ctx, resData, LOCALE_TEXT_DIR, dir);
    JS_SetPropertyStr(ctx, resData, LOCALE_UNICODE_SETTING, unicodeSetting);

    return resData;
}

JSValue JsCallComponent(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((argv == nullptr) || (argc != 3)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("JsCallComponent failed, instance is null.");
        return JS_NULL;
    }
    auto page = GetRunningPage(ctx);
    if (!page) {
        LOGE("JsCallComponent failed, page is null.");
        return JS_NULL;
    }
    int32_t nodeId;
    JS_ToInt32(ctx, &nodeId, argv[0]);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    ScopedString methodName(ctx, argv[1]);
    ScopedString args(ctx, argv[2]);
    LOGD("nodeId = %{private}d, methodName = %{private}s, args = %{private}s", nodeId, methodName.get(), args.get());
    auto engine = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (engine == nullptr) {
        return JS_NULL;
    }
    // handle canvas getContext
    if (std::strcmp(methodName.get(), "getContext") == 0) {
        auto bridge = AceType::DynamicCast<CanvasBridge>(page->GetBridgeById(nodeId));
        if (bridge) {
            bridge->HandleJsContext(ctx, nodeId, args.get(), engine);
            return bridge->GetRenderContext();
        }
        return JS_NULL;
    } else if (std::strcmp(methodName.get(), "toDataURL") == 0) {
        auto bridge = AceType::DynamicCast<CanvasBridge>(page->GetBridgeById(nodeId));
        if (bridge) {
            bridge->HandleToDataURL(ctx, nodeId, args.get());
            return bridge->GetDataURL();
        }
        return JS_NULL;
    } else if (std::strcmp(methodName.get(), "getScrollOffset") == 0) {
        // handle getScrollOffset method of scroll view, like div, stack and list.
        return ComponentApiBridge::JsGetScrollOffset(ctx, nodeId);
    } else if (std::strcmp(methodName.get(), "getBoundingClientRect") == 0) {
        return ComponentApiBridge::JsGetBoundingRect(ctx, nodeId);
    } else if (std::strcmp(methodName.get(), "getInspector") == 0) {
        return ComponentApiBridge::JsGetInspector(ctx, nodeId);
    } else if (std::strcmp(methodName.get(), "scrollTo") == 0) {
        ComponentApiBridge::JsScrollTo(ctx, args.get(), nodeId);
    } else if (std::strcmp(methodName.get(), "getXComponentContext") == 0) {
        auto bridge = AceType::DynamicCast<QjsXComponentBridge>(page->GetXComponentBridgeById(nodeId));
        if (bridge) {
            bridge->HandleContext(ctx, nodeId, args.get());
            return bridge->GetRenderContext();
        }
    }

    auto resultValue = JSValue();
    // handle animation animate method
    if (std::strcmp(methodName.get(), "animate") == 0) {
        resultValue = AnimationBridgeUtils::CreateAnimationContext(ctx, page->GetPageId(), nodeId);
        auto animationBridge = AceType::MakeRefPtr<AnimationBridge>(ctx, resultValue, nodeId);
        auto task = AceType::MakeRefPtr<AnimationBridgeTaskCreate>(animationBridge, args.get());
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    } else if (std::strcmp(methodName.get(), "currentOffset") == 0) {
        // handle list currentOffset method
        return ListBridge::JsGetCurrentOffset(ctx, nodeId);
    } else if (std::strcmp(methodName.get(), "getState") == 0) {
        // handle image animator getState method.
        return ImageAnimatorBridge::JsGetState(ctx, nodeId);
    } else if (std::strcmp(methodName.get(), "createIntersectionObserver") == 0) {
        return ComponentApiBridge::JsCreateObserver(ctx, args.get(), nodeId);
    } else if (std::strcmp(methodName.get(), "addChild") == 0) {
        auto sPage = GetStagingPage(ctx);
        if (sPage == nullptr) {
            return JS_EXCEPTION;
        }
        int32_t childNodeId = 0;
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args.get());
        if (argsValue && argsValue->IsArray()) {
            std::unique_ptr<JsonValue> cNodeId = argsValue->GetArrayItem(0)->GetValue("__nodeId");
            if (cNodeId && cNodeId->IsNumber()) {
                childNodeId = cNodeId->GetInt();
            }
        }
        auto domDocument = sPage->GetDomDocument();
        if (domDocument) {
            RefPtr<DOMNode> node = domDocument->GetDOMNodeById(childNodeId);
            if (node == nullptr) {
                LOGE("node is nullptr");
            }
            auto command = Referenced::MakeRefPtr<JsCommandAppendElement>(node ? node->GetTag() : "", childNodeId,
			    nodeId);
            sPage->PushCommand(command);
            if (!sPage->CheckPageCreated() && sPage->GetCommandSize() > FRAGMENT_SIZE) {
                sPage->FlushCommands();
            }
        }
    } else {
        page->PushCommand(Referenced::MakeRefPtr<JsCommandCallDomElementMethod>(nodeId, methodName.get(), args.get()));
    }
    // focus method should delayed util show attribute update.
    if (page->CheckPageCreated() && strlen(DOM_FOCUS) >= strlen(methodName.get()) &&
        strcmp(DOM_FOCUS, methodName.get()) != 0) {
        instance->GetDelegate()->TriggerPageUpdate(page->GetPageId(), true);
    }
    return resultValue;
}

JSValue JsCallConfiguration(JSContext* ctx, const std::string& method, JSValueConst argv)
{
    if (CONFIGURATION_GET_LOCALE == method) {
        return GetLocale(ctx);
    } else if (CONFIGURATION_SET_LOCALE == method) {
        return JsSetLocale(ctx, argv);
    }
    return JS_NULL;
}


JSValue JsHandleOffscreenCanvas(JSContext* ctx, JSValueConst value, JSValueConst argv, const std::string& methodName)
{
    LOGD("JsHandleOffscreenCanvas: methodName = %{public}s", methodName.c_str());

    auto page = GetRunningPage(ctx);
    if (!page) {
        LOGE("JsHandleOffscreenCanvas failed, page is null.");
        return JS_NULL;
    }

    if (methodName == OFFSCREEN_CANVAS_CREATE) {
        int32_t width = JsParseIntParams(ctx, argv, "width");
        int32_t height = JsParseIntParams(ctx, argv, "height");

        auto bridge = AceType::MakeRefPtr<OffscreenCanvasBridge>(ctx, width, height);
        page->PushOffscreenCanvasBridge(bridge->bridgeId_, bridge);

        return bridge->GetBridge(ctx);

    } else {
        LOGE("JsHandleOffscreenCanvas: methodName not find");
    }

    return JS_NULL;
}
JSValue JsHandleModule(std::string moduleName, std::string methodName, JSContext* ctx, JSValueConst value,
    JSValueConst* argv)
{
    static const LinearMapNode<JSValue (*)(JSContext*, JSValueConst, JSValueConst*,
        const std::string&)> jsHandleMap[] = {
            { "animation",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleAnimationFrame(ctx, argv[1], methodName);
                } },
            { "internal.jsResult",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleCallback(ctx, argv[1], methodName);
                } },
            { "ohos.animator",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleAnimator(ctx, argv[1], methodName);
                } },
            { "system.app",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleAppApi(ctx, argv[1], methodName);
                } },
            { "system.configuration",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsCallConfiguration(ctx, methodName, argv[1]);
                } },
            { "system.device",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    if (methodName == "getInfo") {
                        return JsGetDeviceInfo(ctx, argv[1]);
                    } else {
                        return JS_NULL;
                    }
                } },
            { "system.image",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleImage(ctx, argv[1]);
                } },
            { "system.mediaquery",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleMediaQuery(ctx, argv[1], methodName);
                } },
            { "system.offscreenCanvas",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv,  const std::string& methodName) {
                    return JsHandleOffscreenCanvas(ctx, value, argv[1], methodName);
                } },
            { "system.prompt",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandlePrompt(ctx, argv[1], methodName);
                } },
            { "system.resource",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleReadResource(ctx, argv[1], methodName);
                } },
            { "system.router",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandlePageRoute(ctx, argv[1], methodName);
                } },
            { "timer",
                [](JSContext* ctx, JSValueConst value, JSValueConst* argv, const std::string& methodName) {
                    return JsHandleSetTimeout(ctx, argv[1], methodName);
                } },
    };

    auto jsHandleIter = BinarySearchFindIndex(jsHandleMap, ArraySize(jsHandleMap), moduleName.c_str());
    if (jsHandleIter != -1) {
        return jsHandleMap[jsHandleIter].value(ctx, value, argv, methodName);
    }

    return JS_NULL;
}

JSValue JsCallNative(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("JsCallNative");

    if ((argv == nullptr) || (argc != 2)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }

    ScopedString moduleAndMethod(ctx, argv[0]);
    ScopedString args(ctx, argv[1]);
    LOGD("JsCallNative moduleAndMethod = %{private}s, args = %{private}s", moduleAndMethod.get(), args.get());

    std::unique_ptr<JsonValue> moduleAndMethodPtr = JsonUtil::ParseJsonString(moduleAndMethod.get());
    if (!moduleAndMethodPtr) {
        LOGE("Get moduleAndMethod from argv failed");
        return JS_NULL;
    }

    std::unique_ptr<JsonValue> modulePtr = moduleAndMethodPtr->GetValue("module");
    if (!modulePtr) {
        LOGE("Get module from moduleAndMethodPtr failed");
        return JS_NULL;
    }

    std::unique_ptr<JsonValue> methodPtr = moduleAndMethodPtr->GetValue("method");
    if (!methodPtr) {
        LOGE("Get method from moduleAndMethodPtr failed");
        return JS_NULL;
    }

    const std::string moduleName = modulePtr->GetString();
    const std::string methodName = methodPtr->GetString();

    LOGD("JsCallNative moduleName = %{private}s, methodName = %{private}s", moduleName.c_str(), methodName.c_str());
    js_std_loop(ctx);
    return JsHandleModule(moduleName, methodName, ctx, value, argv);
}

#if defined(ENABLE_JS_DEBUG) || defined(ENABLE_JS_DEBUG_PREVIEW)
JSValue JsCompileAndRunBundle(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv)
{
    ScopedString str(ctx, argv[0]);
    ScopedString url(ctx, argv[1]);
    JSValue CppToJSRet = JS_Eval(ctx, str.get(), strlen(str.get()), url.get(), JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(CppToJSRet)) {
        LOGE("Qjs JsCompileAndRunBundle FAILED !!");
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::COMPILE_AND_RUN_BUNDLE_ERROR);
        return JS_NULL;
    }

    js_std_loop(ctx);
    return CppToJSRet;
}
#endif

std::string ParseLogContent(JSContext* ctx, int32_t argc, JSValueConst* argv)
{
    std::string formatStr = ScopedString(ctx, argv[0]).get();
    if (argc == 1 || formatStr.empty()) {
        return formatStr;
    }
    std::string ret;
    size_t len = formatStr.size();
    size_t pos = 0;
    int32_t count = 1;
    for (; pos < len; ++pos) {
        if (count >= argc) {
            break;
        }
        if (formatStr[pos] == '%') {
            if (pos + 1 >= len) {
                break;
            }
            switch (formatStr[pos + 1]) {
                case 's':
                case 'j':
                case 'd':
                case 'O':
                case 'o':
                case 'i':
                case 'f':
                case 'c':
                    ret += ScopedString(ctx, argv[count++]).get();
                    ++pos;
                    break;
                case '%':
                    ret += formatStr[pos];
                    ++pos;
                    break;
                default:
                    ret += formatStr[pos];
                    break;
            }
        } else {
            ret += formatStr[pos];
        }
    }
    if (pos < len) {
        ret += formatStr.substr(pos, len - pos);
    }
    return ret;
}

JSValue AppLogPrint(JSContext* ctx, JsLogLevel level, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("AppLogPrint(level=%d)", static_cast<int32_t>(level));

    // Should have 1 parameters.
    if ((argv == nullptr) || (argc == 0)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }
    std::string printLog = ParseLogContent(ctx, argc, argv);
    switch (level) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", printLog.c_str());
            break;
        default:
            break;
    }

    return JS_NULL;
}

JSValue AppLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppDebugLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppInfoLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::INFO, value, argc, argv);
}

JSValue AppWarnLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::WARNING, value, argc, argv);
}

JSValue AppErrorLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::ERROR, value, argc, argv);
}

int32_t GetNodeId(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__nodeId");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        id = 0;
    }
    JS_FreeValue(ctx, nodeId);
    return id;
}

JSValue JsSetAttribute(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetRunningPage(ctx);
    if (page == nullptr) {
        return JS_EXCEPTION;
    }
    if (!JS_IsString(argv[0]) || !JS_IsString(argv[1])) {
        LOGE("args is not string ");
        return JS_NULL;
    }
    ScopedString key(ctx, argv[0]);
    JSValue attr = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, attr, key.get(), argv[1]);

    int32_t nodeId = GetNodeId(ctx, value);
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(ctx, attr, *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
    return JS_NULL;
}

JSValue JsSetStyle(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetRunningPage(ctx);
    if (page == nullptr) {
    return JS_EXCEPTION;
    }
    if (!JS_IsString(argv[0]) || !JS_IsString(argv[1])) {
        LOGE("args is not string ");
        return JS_NULL;
    }
    ScopedString key(ctx, argv[0]);
    JSValue style = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, style, key.get(), argv[1]);
    int32_t nodeId = GetNodeId(ctx, value);
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(ctx, style, *command);
    page->PushCommand(command);
    return JS_NULL;
}

JSValue JsAppendChild(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_EXCEPTION;
    }

    int32_t id = GetNodeId(ctx, argv[0]);
    auto domDocument = page->GetDomDocument();
    if (domDocument) {
        RefPtr<DOMNode> node = domDocument->GetDOMNodeById(id);
        if (node == nullptr) {
            LOGE("node is nullptr");
        }
        int32_t parentNodeId = 0;
        JSValue bridgeId = JS_GetPropertyStr(ctx, value, "__nodeId");
        if (JS_IsInteger(bridgeId)) {
            if ((JS_ToInt32(ctx, &parentNodeId, bridgeId)) < 0) {
                parentNodeId = 0;
            }
        }
        RefPtr<DOMNode> parentNode = domDocument->GetDOMNodeById(parentNodeId);
        if (parentNode == nullptr) {
            LOGE("node is nullptr");
        }
        if (node != nullptr && parentNode != nullptr) {
            parentNode->RemoveNode(node);
        }
        auto command = Referenced::MakeRefPtr<JsCommandAppendElement>(node ? node->GetTag() : "", id, parentNodeId);
        page->PushCommand(command);
        if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
            page->FlushCommands();
        }
    }
    return JS_NULL;
}

JSValue JsFocus(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("JsFocus failed, instance is null.");
        return JS_NULL;
    }
    instance->GetDelegate()->TriggerPageUpdate(page->GetPageId(), true);
    return JS_NULL;
}

JSValue JsAnimate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    int32_t nodeId = GetNodeId(ctx, value);
    ScopedString args(ctx, argv[0]);
    auto resultValue = AnimationBridgeUtils::CreateAnimationContext(ctx, page->GetPageId(), nodeId);
    auto animationBridge = AceType::MakeRefPtr<AnimationBridge>(ctx, resultValue, nodeId);
    auto task = AceType::MakeRefPtr<AnimationBridgeTaskCreate>(animationBridge, args.get());
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return JS_NULL;
}

JSValue JsGetBoundingClientRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    int32_t nodeId = GetNodeId(ctx, value);
    ComponentApiBridge::JsGetBoundingRect(ctx, nodeId);
    return JS_NULL;
}

JSValue JsGetInspector(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto page = GetStagingPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    int32_t nodeId = GetNodeId(ctx, value);
    ComponentApiBridge::JsGetInspector(ctx, nodeId);
    return JS_NULL;
}

JSValue JsCreateElement(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int32_t newNodeId = CreateDomElement(ctx, value, argc, argv);
    JSValue node = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, node, "__nodeId",  JS_NewInt32(ctx, newNodeId));
    JS_SetPropertyStr(ctx, node, "setAttribute", JS_NewCFunction(ctx, JsSetAttribute, "setAttribute", 2));
    JS_SetPropertyStr(ctx, node, "setStyle", JS_NewCFunction(ctx, JsSetStyle, "setStyle", 2));
    JS_SetPropertyStr(ctx, node, "addChild", JS_NewCFunction(ctx, JsAppendChild, "addChild", 1));
    JS_SetPropertyStr(ctx, node, "focus", JS_NewCFunction(ctx, JsFocus, "focus", 1));
    JS_SetPropertyStr(ctx, node, "animate", JS_NewCFunction(ctx, JsAnimate, "animate", 1));
    JS_SetPropertyStr(ctx, node, "getBoundingClientRect",
        JS_NewCFunction(ctx, JsGetBoundingClientRect, "getBoundingClientRect", 1));
    JS_SetPropertyStr(ctx, node, "getInspector", JS_NewCFunction(ctx, JsGetInspector, "getInspector", 1));
    return node;
}

JSValue JsLogPrint(JSContext* ctx, JsLogLevel level, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("JsLogPrint(level=%d)", static_cast<int32_t>(level));

    // Should have 1 parameters.
    if ((argv == nullptr) || (argc == 0)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }
    std::string printLog = ParseLogContent(ctx, argc, argv);
    switch (level) {
        case JsLogLevel::DEBUG:
            LOGD("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::INFO:
            LOGI("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::WARNING:
            LOGW("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::ERROR:
            LOGE("ace Log: %{public}s", printLog.c_str());
            break;
        default:
            break;
    }

    return JS_NULL;
}

JSValue JsLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue JsDebugLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue JsInfoLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::INFO, value, argc, argv);
}

JSValue JsWarnLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::WARNING, value, argc, argv);
}

JSValue JsErrorLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::ERROR, value, argc, argv);
}

JSValue JsPerfPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    std::string ret = JsApiPerf::GetInstance().PrintToLogs();

    JSValue retString = JS_NewString(ctx, ret.c_str());

    return retString;
}

JSValue JsPerfSleep(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int32_t valInt = 0;
    JS_ToInt32(ctx, &valInt, argv[0]);

    usleep(valInt);

    return JS_NULL;
}

JSValue JsPerfBegin(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int64_t currentTime = GetMicroTickCount();
    ScopedString functionName(ctx, argv[0]);

    JsApiPerf::GetInstance().InsertJsBeginLog(functionName.get(), currentTime);

    return JS_NULL;
}

JSValue JsPerfEnd(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int64_t currentTime = GetMicroTickCount();
    ScopedString functionName(ctx, argv[0]);

    JsApiPerf::GetInstance().InsertJsEndLog(functionName.get(), currentTime);

    return JS_NULL;
}

JSValue JSHiViewReport(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // Should have 2 parameters.
    if ((argv == nullptr) || (argc != 2)) {
        LOGE("the arg is error %{public}d", argc);
        return JS_EXCEPTION;
    }
    if (JS_IsInteger(argv[0]) && JS_IsString(argv[1])) {
        ScopedString eventId(ctx, argv[0]);
        ScopedString eventJsonStr(ctx, argv[1]);
        std::string jsonStr = eventJsonStr.get();
        EventReport::JsEventReport(StringToInt(eventId.get()), jsonStr);
        LOGD("JsEventReport success");
        return JS_NULL;
    } else {
        LOGE("parameter type error ");
        return JS_EXCEPTION;
    }
}

JSValue JsPluralRulesFormat(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // Should have one parameters.
    if ((argv == nullptr) || (argc != 1)) {
        LOGE("the arg is error %{public}d", argc);
        return JS_NULL;
    }
    if (JS_IsInteger(argv[0])) {
        ScopedString choice(ctx, argv[0]);
        JSValue result =
            JS_NewString(ctx, Localization::GetInstance()->PluralRulesFormat(StringToDouble(choice.get())).c_str());
        return result;
    } else {
        LOGE("parameter type error ");
        return JS_NULL;
    }
}

JSValue JsLoadIntl(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("QjsEngine::LoadIntl");
    LOGI("JsLoadIntl");

    JSValue ret = CallReadObject(ctx, js_intl_support, js_intl_support_size, true);
    if (JS_IsException(ret)) {
        LOGE("JsLoadIntl failed!");
    }

    return ret;
}

JSValue JsLoadLocaleData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ACE_SCOPED_TRACE("QjsEngine::LoadLocaleData");
    LOGI("Add LocaleData");

    ScopedString lib(ctx, argv[0]);    // select which lib(intl or intl-relative-time-format) to use
    ScopedString locale(ctx, argv[1]); // select locale

    std::string key;
    if (strcmp(lib.get(), "intl") == 0) {
        key.append("intl_");
    } else {
        key.append("intl_relative_");
    }
    key.append(locale.get());
    std::replace(key.begin(), key.end(), '-', '_');

    std::unordered_map<std::string, std::vector<const void*>> data = GetLocaleDatasMap();
    if (data.find(key) != data.end()) {
        std::vector<const void*> val = data[key];
        const uint32_t* len = static_cast<const uint32_t*>(val[0]);
        const uint8_t* rawData = static_cast<const uint8_t*>(val[1]);
        JSValue ret = CallReadObject(ctx, rawData, *len, true);
        LOGI("load data: %s", key.c_str());
        return ret;
    } else {
        LOGE("load data: %s not found!", key.c_str());
        return JS_NULL;
    }
}

// Follow definition in quickjs.
#define JS_CFUNC_DEF_CPP(name, length, func)                            \
    {                                                                   \
        name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, \
        {                                                               \
            {                                                           \
                length, JS_CFUNC_generic,                               \
                {                                                       \
                    func                                                \
                }                                                       \
            }                                                           \
        }                                                               \
    }

const JSCFunctionListEntry JS_ACE_FUNCS[] = {
    JS_CFUNC_DEF_CPP("domCreateBody", 5, JsDomCreateBody),
    JS_CFUNC_DEF_CPP("domAddElement", 9, JsDomAddElement),
    JS_CFUNC_DEF_CPP("updateElementAttrs", 3, JsUpdateElementAttrs),
    JS_CFUNC_DEF_CPP("updateElementStyles", 3, JsUpdateElementStyles),
    JS_CFUNC_DEF_CPP("onCreateFinish", 0, JsOnCreateFinish),
    JS_CFUNC_DEF_CPP("onUpdateFinish", 0, JsOnUpdateFinish),
    JS_CFUNC_DEF_CPP("removeElement", 2, JsRemoveElement),
    JS_CFUNC_DEF_CPP("callNative", 1, JsCallNative),
    JS_CFUNC_DEF_CPP("callComponent", 3, JsCallComponent),
    JS_CFUNC_DEF_CPP("loadIntl", 0, JsLoadIntl),
    JS_CFUNC_DEF_CPP("loadLocaleData", 0, JsLoadLocaleData),
#if defined(ENABLE_JS_DEBUG) || defined(ENABLE_JS_DEBUG_PREVIEW)
    JS_CFUNC_DEF_CPP("compileAndRunBundle", 4, JsCompileAndRunBundle),
#endif
};

int32_t JsAceInit(JSContext* ctx, JSModuleDef* moduleDef)
{
    return JS_SetModuleExportList(ctx, moduleDef, JS_ACE_FUNCS, countof(JS_ACE_FUNCS));
}

JSModuleDef* InitAceModules(JSContext* ctx)
{
    LOGD("QjsEngine: InitAceModules");
    JSModuleDef* jsModule = JS_NewCModule(ctx, "ace", JsAceInit);
    if (jsModule == nullptr) {
        return nullptr;
    }
    JS_AddModuleExportList(ctx, jsModule, JS_ACE_FUNCS, countof(JS_ACE_FUNCS));
    return jsModule;
}

JSValue JsCallNativeHandler(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 2 || argv == nullptr || !JS_IsString(argv[0]) || !JS_IsString(argv[1])) {
        LOGE("JsCallNativeHandler: invalid parameters");
        return JS_NULL;
    }

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }

    ScopedString event(ctx, argv[0]);
    ScopedString params(ctx, argv[1]);
    instance->GetDelegate()->CallNativeHandler(event.str(), params.str());
    return JS_NULL;
}

void InitJsConsoleObject(JSContext* ctx, const JSValue& globalObj)
{
    JSValue console, aceConsole;
    // app log method
    console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, AppLogPrint, "log", 1));
    JS_SetPropertyStr(ctx, console, "debug", JS_NewCFunction(ctx, AppDebugLogPrint, "debug", 1));
    JS_SetPropertyStr(ctx, console, "info", JS_NewCFunction(ctx, AppInfoLogPrint, "info", 1));
    JS_SetPropertyStr(ctx, console, "warn", JS_NewCFunction(ctx, AppWarnLogPrint, "warn", 1));
    JS_SetPropertyStr(ctx, console, "error", JS_NewCFunction(ctx, AppErrorLogPrint, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "console", console);

    // js framework log method
    aceConsole = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, aceConsole, "log", JS_NewCFunction(ctx, JsLogPrint, "log", 1));
    JS_SetPropertyStr(ctx, aceConsole, "debug", JS_NewCFunction(ctx, JsDebugLogPrint, "debug", 1));
    JS_SetPropertyStr(ctx, aceConsole, "info", JS_NewCFunction(ctx, JsInfoLogPrint, "info", 1));
    JS_SetPropertyStr(ctx, aceConsole, "warn", JS_NewCFunction(ctx, JsWarnLogPrint, "warn", 1));
    JS_SetPropertyStr(ctx, aceConsole, "error", JS_NewCFunction(ctx, JsErrorLogPrint, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "aceConsole", aceConsole);
    JS_SetPropertyStr(ctx, globalObj, "callNativeHandler",
        JS_NewCFunction(ctx, JsCallNativeHandler, "callNativeHandler", 1));
}

void InitJsDocumentObject(JSContext* ctx, const JSValue& globalObj)
{
    JSValue dom = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, dom, "createElement", JS_NewCFunction(ctx, JsCreateElement, "createElement", 1));
    JS_SetPropertyStr(ctx, globalObj, "dom", dom);
}

bool InitJsContext(JSContext* ctx, size_t maxStackSize, int32_t instanceId, const QjsEngineInstance* qjsEngineInstance,
    const std::unordered_map<std::string, void*>& extraNativeObject)
{
    LOGI("QjsEngine: InitJsContext");
    if (ctx == nullptr) {
        LOGE("Qjs cannot allocate JS context");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    // Note: default 256KB is not enough
    JS_SetMaxStackSize(ctx, maxStackSize);

    // Inject ace native functions module
    InitAceModules(ctx);

    JSValue globalObj, perfUtil;
    globalObj = JS_GetGlobalObject(ctx);
    perfUtil = JS_NewObject(ctx);

    InitJsConsoleObject(ctx, globalObj);
    InitJsDocumentObject(ctx, globalObj);

    JS_SetPropertyStr(ctx, perfUtil, "printlog", JS_NewCFunction(ctx, JsPerfPrint, "printlog", 0));
    JS_SetPropertyStr(ctx, perfUtil, "sleep", JS_NewCFunction(ctx, JsPerfSleep, "sleep", 1));
    JS_SetPropertyStr(ctx, perfUtil, "begin", JS_NewCFunction(ctx, JsPerfBegin, "begin", 1));
    JS_SetPropertyStr(ctx, perfUtil, "end", JS_NewCFunction(ctx, JsPerfEnd, "end", 1));
    JS_SetPropertyStr(ctx, globalObj, "perfutil", perfUtil);

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    for (const auto& [key, value] : extraNativeObject) {
        auto nativeObjectInfo = std::make_unique<NativeObjectInfo>();
        nativeObjectInfo->nativeObject = value;
        JSValue abilityValue = JS_NewExternal(ctx, nativeObjectInfo.release(),
            [](JSContext* ctx, void *data, void *hint) {
                std::unique_ptr<NativeObjectInfo> info(static_cast<NativeObjectInfo*>(data));
            }, nullptr);
        JS_SetPropertyStr(ctx, globalObj, key.c_str(), abilityValue);
    }
#endif

    JSValue hiView;
    hiView = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, hiView, "report", JS_NewCFunction(ctx, JSHiViewReport, "report", 2));
    JS_SetPropertyStr(ctx, globalObj, "hiView", hiView);

    JSValue i18nPluralRules;
    i18nPluralRules = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, i18nPluralRules, "select", JS_NewCFunction(ctx, JsPluralRulesFormat, "select", 1));
    JS_SetPropertyStr(ctx, globalObj, "i18nPluralRules", i18nPluralRules);

    const char* str = "import * as ace from 'ace';\n"
                      "var global = globalThis;\n"
                      "global.ace = ace;\n"
#if defined(ENABLE_JS_DEBUG) || defined(ENABLE_JS_DEBUG_PREVIEW)
                      "global.compileAndRunBundle = ace.compileAndRunBundle;\n"
#endif
                      "global.callNative = ace.callNative;\n";

    if (JS_CALL_FAIL == CallEvalBuf(ctx, str, strlen(str), "<input>", JS_EVAL_TYPE_MODULE, instanceId)) {
        LOGW("Qjs created JS context but failed to init Ace modules!");
    }

    JS_FreeValue(ctx, globalObj);
    return true;
}

JSValue LoadJsFramework(JSContext* ctx, const uint8_t buf[], const uint32_t bufSize, int32_t instanceId)
{
    ACE_SCOPED_TRACE("LoadJsFramework");

    LOGI("Qjs loading JS framework");
    JSValue ret = CallReadObject(ctx, buf, bufSize, true, instanceId);
    if (JS_IsException(ret)) {
        LOGD("Qjs loading JSFramework failed!");
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::LOAD_JS_FRAMEWORK_ERROR, instanceId);
    }
    js_std_loop(ctx);
    return ret;
}

} // namespace

// -----------------------
// Start QjsEngineInstance
// -----------------------
void QjsEngineInstance::FlushCommandBuffer(void* context, const std::string& command)
{
    LOGI("flush command buffer");
    auto ctx = static_cast<JSContext*>(context);
    int32_t result = CallEvalBuf(ctx, command.c_str(), command.size(), "<input>", JS_EVAL_TYPE_GLOBAL, instanceId_);
    if (JS_CALL_FAIL == result) {
        LOGE("failed to flush command buffer");
    }
}
bool QjsEngineInstance::InitJsEnv(
    JSRuntime* runtime, JSContext* context, const std::unordered_map<std::string, void*>& extraNativeObject)
{
    LOGI("InitJsEnv");
    if (runtime == nullptr) {
        runtime = JS_NewRuntime();
    }
    if (runtime_ != nullptr) {
        JS_FreeRuntime(runtime_);
    }
    runtime_ = runtime;
    if (runtime_ == nullptr) {
        LOGE("Qjs cannot allocate JS runtime");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    if (context == nullptr) {
        context = JS_NewContext(runtime_);
    }
    if (context_ != nullptr) {
        JS_FreeContext(context_);
    }
    context_ = context;
    if (!InitJsContext(context_, MAX_STACK_SIZE, instanceId_, this, extraNativeObject)) {
        LOGE("Qjs cannot allocate JS context");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        JS_FreeRuntime(runtime_);
        return false;
    }

    auto groupJsBridge = AceType::DynamicCast<QuickJsGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (!groupJsBridge || JS_CALL_FAIL == groupJsBridge->InitializeGroupJsBridge(context_)) {
        LOGE("Qjs Initialize GroupJsBridge failed!");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    JSValue retVal = LoadJsFramework(GetQjsContext(), js_framework, js_framework_size, instanceId_);
    bool result = JS_IsException(retVal) ? false : true;
    if (context) {
        JS_FreeValue(context, retVal);
    }

    std::string intlHook("                                       \
        globalThis.Intl = {                                      \
            NumberFormat: function() {                           \
                delete globalThis.Intl;                          \
                ace.loadIntl();                                  \
                return Intl.NumberFormat(...arguments);          \
            },                                                   \
            DateTimeFormat: function() {                         \
               delete globalThis.Intl;                           \
               ace.loadIntl();                                   \
               return Intl.DateTimeFormat(...arguments);         \
            },                                                   \
            RelativeTimeFormat: function() {                     \
                delete globalThis.Intl;                          \
                ace.loadIntl();                                  \
                return new Intl.RelativeTimeFormat(...arguments);\
            }                                                    \
        }                                                        \
    ");

    if (JS_CALL_FAIL ==
        CallEvalBuf(context, intlHook.c_str(), intlHook.size(), "<input>", JS_EVAL_TYPE_GLOBAL, instanceId_)) {
        LOGE("Intl init failed!");
    }

    return result;
}

void QjsEngineInstance::FreeGroupJsBridge()
{
    // free JSValue reference of channel bridge and animation bridge
    if (!frontendDelegate_) {
        LOGE("frontend delegate is null.");
        return;
    }

    auto groupJsBridge = AceType::DynamicCast<QuickJsGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (!groupJsBridge) {
        LOGE("group js bridge is null.");
        return;
    }
    groupJsBridge->Uninitialize();
}

QjsEngineInstance::~QjsEngineInstance()
{
    if (runningPage_) {
        runningPage_->OnJsEngineDestroy();
    }

    if (stagingPage_) {
        stagingPage_->OnJsEngineDestroy();
    }
    FreeGroupJsBridge();

    if (context_) {
        JS_FreeContext(context_);
    }
    if (runtime_) {
        JS_FreeRuntime(runtime_);
    }
}

JSValue QjsEngineInstance::FireJsEvent(const std::string& param)
{
    LOGI("FireJsEvent");
    JSContext* ctx = GetQjsContext();
    ACE_DCHECK(ctx);
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue callJsFunc = QJSUtils::GetPropertyStr(ctx, globalObj, "callJS");
    if (!JS_IsFunction(ctx, callJsFunc)) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        JS_FreeValue(ctx, globalObj);
        return JS_UNDEFINED;
    }

    if (!runningPage_) {
        LOGE("runningPage_ is null!");
        JS_FreeValue(ctx, globalObj);
        return JS_UNDEFINED;
    }
    JSValueConst argv[] = {
        QJSUtils::NewString(ctx, std::to_string(runningPage_->GetPageId()).c_str()),
        QJSUtils::ParseJSON(ctx, param.c_str(), param.size(), nullptr),
    };

    JSValue retVal = JS_Call(ctx, callJsFunc, globalObj, countof(argv), argv);

    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);

    // It is up to the caller to check this value. No exception checks here.
    return retVal;
}

void QjsEngineInstance::CallJs(const std::string& callbackId, const std::string& args, bool keepAlive, bool isGlobal)
{
    ACE_SCOPED_TRACE("QjsEngineInstance::CallJS");

    std::string keepAliveStr = keepAlive ? "true" : "false";
    std::string callBuff = std::string("[{\"args\": [\"")
                               .append(callbackId)
                               .append("\",")
                               .append(args)
                               .append(",")
                               .append(keepAliveStr)
                               .append("], \"method\":\"callback\"}]");
    LOGD("CallJs string: %{private}s", callBuff.c_str());

    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue callJsFunc = QJSUtils::GetPropertyStr(ctx, globalObj, "callJS");
    if (!JS_IsFunction(ctx, callJsFunc)) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    int32_t instanceId = isGlobal ? DEFAULT_APP_ID : stagingPage_->GetPageId();
    JSValueConst argv[] = {
        QJSUtils::NewString(ctx, std::to_string(instanceId).c_str()),
        QJSUtils::ParseJSON(ctx, callBuff.c_str(), callBuff.size(), nullptr),
    };
    JSValue retVal = QJSUtils::Call(ctx, callJsFunc, globalObj, countof(argv), argv);

    if (JS_IsException(retVal)) {
        LOGE("JS framework excute callback failed");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::JS_CALLBACK_ERROR, instanceId_, stagingPage_->GetUrl().c_str(),
            stagingPage_);
        return;
    }
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
}

void QjsEngineInstance::CallAnimationStartJs(JSValue animationContext)
{
    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "onstart");
    if (!JS_IsFunction(ctx, proto)) {
        LOGD("cannot find 'CallAnimationStartJs' function from global object, this should not happen!");
        return;
    }
    LOGD("animation onstart event call");
    JSValue retVal = QJSUtils::Call(ctx, proto, animationContext, 0, {});
    JSValue globalObj = JS_GetGlobalObject(ctx);

    if (JS_IsException(retVal)) {
        LOGE("JS framework excute callAnimationStart failed");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::JS_CALLBACK_ERROR, instanceId_, stagingPage_->GetUrl().c_str(),
            stagingPage_);
        return;
    }
    js_std_loop(ctx);
}

void QjsEngineInstance::CallAnimationFinishJs(JSValue animationContext)
{
    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "onfinish");
    if (!JS_IsFunction(ctx, proto)) {
        LOGD("cannot find 'CallAnimationFinishJs' function from global object, this should not happen!");
        return;
    }
    LOGD("animation onfinish event call");
    JSValue retVal = QJSUtils::Call(ctx, proto, animationContext, 0, {});
    JSValue globalObj = JS_GetGlobalObject(ctx);

    if (JS_IsException(retVal)) {
        LOGE("JS framework excute callAnimationFininsh failed");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::JS_CALLBACK_ERROR, instanceId_, stagingPage_->GetUrl().c_str(),
            stagingPage_);
        return;
    }
    js_std_loop(ctx);
}

void QjsEngineInstance::CallAnimationCancelJs(JSValue animationContext)
{
    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "oncancel");
    if (!JS_IsFunction(ctx, proto)) {
        return;
    }

    LOGD("animation oncancel event call");
    JSValue retVal = QJSUtils::Call(ctx, proto, animationContext, 0, {});
    JSValue globalObj = JS_GetGlobalObject(ctx);

    if (JS_IsException(retVal)) {
        LOGE("JS framework excute callAnimationCancel failed");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::JS_CALLBACK_ERROR, instanceId_, stagingPage_->GetUrl().c_str(),
            stagingPage_);
        return;
    }
    js_std_loop(ctx);
}

void QjsEngineInstance::CallAnimationRepeatJs(JSValue animationContext)
{
    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "onrepeat");
    if (!JS_IsFunction(ctx, proto)) {
        return;
    }

    LOGD("animation onrepeat event call");
    JSValue retVal = QJSUtils::Call(ctx, proto, animationContext, 0, {});
    JSValue globalObj = JS_GetGlobalObject(ctx);

    if (JS_IsException(retVal)) {
        LOGE("JS framework excute callAnimationRepeatJs failed");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::JS_CALLBACK_ERROR, instanceId_, stagingPage_->GetUrl().c_str(),
            stagingPage_);
        return;
    }
    js_std_loop(ctx);
}

void QjsEngineInstance::CallAnimationFrameJs(JSValue animationContext, const char* str)
{
    JSContext* ctx = GetQjsContext();
    QJSHandleScope handleScope(ctx);
    auto proto = QJSUtils::GetPropertyStr(ctx, animationContext, "onframe");
    if (!JS_IsFunction(ctx, proto)) {
        return;
    }

    LOGD("animation onframe event call");
    JSValue value = QJSUtils::NewString(ctx, str);
    JSValueConst argv[] = { value };
    QJSUtils::Call(ctx, proto, animationContext, 1, argv);
    js_std_loop(ctx);
}

// -----------------------
// Start QjsEngine
// -----------------------
bool QjsEngine::Initialize(const RefPtr<FrontendDelegate>& delegate)
{
    ACE_SCOPED_TRACE("QjsEngine::Initialize");
    LOGI("Initialize");

    JSRuntime* runtime = nullptr;
    JSContext* context = nullptr;

    // put JS_NewContext as early as possible to make stack_top in context
    // closer to the top stack frame pointer of JS thread.
    runtime = JS_NewRuntime();
    if (runtime != nullptr) {
        context = JS_NewContext(runtime);
    }

    LOGD("QjsEngine initialize");

#ifdef ENABLE_JS_DEBUG
    LOGI("QjsEngine debug mode");
    std::string instanceName = GetInstanceName();
    if (instanceName.empty()) {
        LOGE("GetInstanceName fail, %s", instanceName.c_str());
        return false;
    }
    constexpr int32_t COMPONENT_NAME_MAX_LEN = 100;
    char componentName[COMPONENT_NAME_MAX_LEN];
    if (!DBG_CopyComponentNameFromAce(instanceName.c_str(), componentName, COMPONENT_NAME_MAX_LEN)) {
        LOGE("GetInstanceName strcpy_s fail");
        return false;
    }
    DBG_SetComponentNameAndInstanceId(componentName, strlen(componentName), instanceId_);
    DBG_SetNeedDebugBreakPoint(NeedDebugBreakPoint());
#endif
    ACE_DCHECK(delegate);

    engineInstance_ = AceType::MakeRefPtr<QjsEngineInstance>(delegate, instanceId_);
    nativeEngine_ = new QuickJSNativeEngine(runtime, context, static_cast<void*>(this));
    engineInstance_->SetQuickJSNativeEngine(nativeEngine_);
    bool ret = engineInstance_->InitJsEnv(runtime, context, GetExtraNativeObject());

    SetPostTask(nativeEngine_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    nativeEngine_->CheckUVLoop();
#endif
    RegisterWorker();
    if (delegate && delegate->GetAssetManager()) {
        std::string packagePath = delegate->GetAssetManager()->GetLibPath();
        if (!packagePath.empty()) {
            nativeEngine_->SetPackagePath(packagePath);
        }
    }

    return ret;
}

void QjsEngine::SetPostTask(NativeEngine* nativeEngine)
{
    LOGI("SetPostTask");
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& postTask = [weakDelegate, nativeEngine = nativeEngine_, id = instanceId_](bool needSync) {
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->PostJsTask([nativeEngine, needSync, id]() {
            ContainerScope scope(id);
            if (nativeEngine == nullptr) {
                return;
            }
            nativeEngine->Loop(LOOP_NOWAIT, needSync);
        });
    };
    nativeEngine_->SetPostTask(postTask);
}

void QjsEngine::RegisterInitWorkerFunc()
{
    auto&& initWorkerFunc = [](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterInitWorkerFunc called");
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto qjsNativeEngine = static_cast<QuickJSNativeEngine*>(nativeEngine);
        if (qjsNativeEngine == nullptr) {
            LOGE("qjsNativeEngine is nullptr");
            return;
        }

        JSContext* ctx = qjsNativeEngine->GetContext();
        if (ctx == nullptr) {
            LOGE("ctx is nullptr");
            return;
        }

        // Create a stack-allocated handle scope.
        QJSHandleScope handleScope(ctx);

        // Note: default 256KB is not enough
        JS_SetMaxStackSize(ctx, MAX_STACK_SIZE);

        JSValue globalObj = JS_GetGlobalObject(ctx);
        InitJsConsoleObject(ctx, globalObj);
        JS_FreeValue(ctx, globalObj);
    };
    nativeEngine_->SetInitWorkerFunc(initWorkerFunc);
}

void QjsEngine::RegisterAssetFunc()
{
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& assetFunc = [weakDelegate](const std::string& uri, std::vector<uint8_t>& content, std::string& ami) {
        LOGI("WorkerCore RegisterAssetFunc called");
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->GetResourceData(uri, content, ami);
    };
    nativeEngine_->SetGetAssetFunc(assetFunc);
}

void QjsEngine::RegisterWorker()
{
    RegisterInitWorkerFunc();
    RegisterAssetFunc();
}

QjsEngine::~QjsEngine()
{
    if (nativeEngine_ != nullptr) {
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        nativeEngine_->CancelCheckUVLoop();
#endif
        delete nativeEngine_;
    }
    if (engineInstance_ && engineInstance_->GetQjsRuntime()) {
        JS_RunGC(engineInstance_->GetQjsRuntime());
    }
}

void QjsEngine::GetLoadOptions(std::string& optionStr, bool isMainPage, const RefPtr<JsAcePage>& page)
{
    auto mediaQuery = engineInstance_->GetDelegate()->GetMediaQueryInfoInstance();
    auto renderOption = MediaQueryInfo::GetMediaQueryJsonInfo();
    if (mediaQuery) {
        renderOption->Put("isInit", mediaQuery->GetIsInit());
        renderOption->Put("bundleUrl", page->GetUrl().c_str());
    }
    renderOption->Put("appInstanceId", "10002");
    renderOption->Put("pcPreview", PC_PREVIEW);
    renderOption->Put("packageName", engineInstance_->GetDelegate()->GetAppID().c_str());

    // get resourceConfig
    engineInstance_->GetDelegate()->GetResourceConfiguration(renderOption);

    // get i18n message
    std::string language = AceApplicationInfo::GetInstance().GetLanguage();
    std::string region = AceApplicationInfo::GetInstance().GetCountryOrRegion();
    engineInstance_->GetDelegate()->GetI18nData(renderOption);
    std::string local = language + "_" + region;
    renderOption->Put("language", local.c_str());

    if (isMainPage) {
        JSContext* ctx = engineInstance_->GetQjsContext();
        std::string commonsJsContent;
        if (engineInstance_->GetDelegate()->GetAssetContent("commons.js", commonsJsContent)) {
            auto commonsJsResult = CallEvalBuf(ctx, commonsJsContent.c_str(), commonsJsContent.size(),
                "commons.js", JS_EVAL_TYPE_MODULE, instanceId_);
            if (commonsJsResult == JS_CALL_FAIL) {
                LOGE("fail to excute load commonsjs script");
            }
        }
        std::string vendorsJsContent;
        if (engineInstance_->GetDelegate()->GetAssetContent("vendors.js", vendorsJsContent)) {
            bool vendorsJsResult = CallEvalBuf(ctx, vendorsJsContent.c_str(), vendorsJsContent.size(),
                "vendors.js", JS_EVAL_TYPE_MODULE, instanceId_);
            if (vendorsJsResult == JS_CALL_FAIL) {
                LOGE("fail to excute load vendorsjs script");
            }
        }
        std::string code;
        std::string appMap;
        if (engineInstance_->GetDelegate()->GetAssetContent("app.js.map", appMap)) {
            page->SetAppMap(appMap);
        } else {
            LOGW("sourceMap of app.js is missing!");
        }

        if (engineInstance_->GetDelegate()->GetAssetContent("app.js", code)) {
            renderOption->Put("appCreate", "true");
            renderOption->Put("appCode", code.c_str());
        } else {
            LOGE("app.js is missing!");
        }
    } else {
        renderOption->Put("appCreate", "false");
    }
    optionStr = renderOption->ToString();
}

void QjsEngine::LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage)
{
    LOGI("QjsEngine LoadJs");
    ACE_SCOPED_TRACE("QjsEngine::LoadJs");
    ACE_DCHECK(engineInstance_);

    engineInstance_->SetStagingPage(page);
    if (isMainPage) {
        ACE_DCHECK(!engineInstance_->GetRunningPage());
        engineInstance_->SetRunningPage(page);
    }
    JSContext* ctx = engineInstance_->GetQjsContext();

    // Create a stack-allocated handle scope.
    QJSHandleScope handleScope(ctx);

    // Memorize the context that this JSContext is working with.
    JS_SetContextOpaque(ctx, reinterpret_cast<void*>(AceType::RawPtr(engineInstance_)));

    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue createInstanceFunc = QJSUtils::GetPropertyStr(ctx, globalObj, "createInstance");
    if (!JS_IsFunction(ctx, createInstanceFunc)) {
        LOGD("createInstance is not found, cannot load js!");
        JS_FreeValue(ctx, globalObj);
        return;
    }
#ifdef ENABLE_JS_DEBUG
    std::string jsContent;
    if (!engineInstance_->GetDelegate()->GetAssetContent(url, jsContent)) {
        LOGE("js file load failed!");
        JS_FreeValue(ctx, globalObj);
        return;
    }
#else
    JSValue jsCode = JS_UNDEFINED;
    auto pos = url.rfind(JS_EXT);
    if (pos != std::string::npos && pos == url.length() - (sizeof(JS_EXT) - 1)) {
        std::vector<uint8_t> binContent;
        if (engineInstance_->GetDelegate()->GetAssetContent(url.substr(0, pos) + BIN_EXT, binContent)) {
            jsCode =
                CallReadObject(ctx, binContent.data(), binContent.size(), false, instanceId_, page->GetUrl().c_str());
            if (!JS_IsException(jsCode)) {
                JS_FreeValue(ctx, jsCode);
                jsCode = QJSUtils::GetPropertyStr(ctx, globalObj, JS_MAIN_ENTRY);
                JS_SetPropertyStr(ctx, globalObj, JS_MAIN_ENTRY, JS_UNDEFINED);
            } else {
                QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::READ_OBJECT_ERROR, instanceId_, page->GetUrl().c_str(),
                    page);
            }
        }
    }

    if (JS_IsFunction(ctx, jsCode)) {
        LOGI("Using binary source code");
    } else {
        std::string jsContent;
        if (!engineInstance_->GetDelegate()->GetAssetContent(url, jsContent)) {
            LOGE("js file load failed!");
            JS_FreeValue(ctx, globalObj);
            return;
        }
        jsCode = QJSUtils::NewStringLen(ctx, jsContent.c_str(), jsContent.size());
    }
#endif
    std::string pageMap;
    if (engineInstance_->GetDelegate()->GetAssetContent(url + MAP_EXT, pageMap)) {
        page->SetPageMap(pageMap);
    } else {
        LOGW("source map of page load failed!");
    }
    std::string jsonData = page->GetPageParams();
    if (jsonData.empty()) {
        jsonData = "{}";
    }
    std::string optionStr;
    GetLoadOptions(optionStr, isMainPage, page);
    JSValue instanceId = QJSUtils::NewString(ctx, std::to_string(page->GetPageId()).c_str());
    JSValue renderOptions = QJSUtils::ParseJSON(ctx, optionStr.c_str(), optionStr.size(), nullptr);
    JSValue data = QJSUtils::ParseJSON(ctx, jsonData.c_str(), jsonData.size(), nullptr);
    JSValue info = QJSUtils::NewObject(ctx);
#ifdef ENABLE_JS_DEBUG
    JSValue jsSrc = QJSUtils::NewStringLen(ctx, jsContent.c_str(), jsContent.length());
    JSValueConst argv[] = { instanceId, jsSrc, renderOptions, data, info };
#else
    JSValueConst argv[] = { instanceId, jsCode, renderOptions, data, info };
#endif
    JSValue retVal = QJSUtils::Call(ctx, createInstanceFunc, JS_UNDEFINED, countof(argv), argv);

    if (JS_IsException(retVal)) {
        LOGE("JS framework load js bundle failed!");
        JS_FreeValue(ctx, globalObj);
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::LOAD_JS_BUNDLE_ERROR, instanceId_, page->GetUrl().c_str(), page);
        return;
    }

    JS_FreeValue(ctx, retVal);
    JS_FreeValue(ctx, globalObj);
    js_std_loop(ctx);
}

void QjsEngine::UpdateRunningPage(const RefPtr<JsAcePage>& page)
{
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetRunningPage(page);
    js_std_loop(engineInstance_->GetQjsContext());
}

void QjsEngine::UpdateStagingPage(const RefPtr<JsAcePage>& page)
{
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
    js_std_loop(engineInstance_->GetQjsContext());
}

void QjsEngine::ResetStagingPage()
{
    ACE_DCHECK(engineInstance_);
    auto runningPage = engineInstance_->GetRunningPage();
    engineInstance_->ResetStagingPage(runningPage);
    js_std_loop(engineInstance_->GetQjsContext());
}

void QjsEngine::DestroyPageInstance(int32_t pageId)
{
    LOGI("QjsEngine DestroyPageInstance");
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue destroyInstanceFunc = QJSUtils::GetPropertyStr(ctx, globalObj, "destroyInstance");
    if (!JS_IsFunction(ctx, destroyInstanceFunc)) {
        LOGE("destroyInstance is not found, cannot destroy page instance!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValue instanceId = QJSUtils::NewString(ctx, std::to_string(pageId).c_str());
    JSValueConst argv[] = { instanceId };
    JSValue retVal = QJSUtils::Call(ctx, destroyInstanceFunc, JS_UNDEFINED, countof(argv), argv);

    if (JS_IsException(retVal)) {
        LOGE("Qjs DestroyPageInstance FAILED!");
        JS_FreeValue(ctx, globalObj);

        auto page = engineInstance_->GetDelegate()->GetPage(pageId);
        if (page) {
            QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::DESTROY_PAGE_ERROR, instanceId_, page->GetUrl().c_str(),
                page);
        } else {
            QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::DESTROY_PAGE_ERROR, instanceId_,
                engineInstance_->GetRunningPage()->GetUrl().c_str(), engineInstance_->GetRunningPage());
        }
        return;
    }

    RunGarbageCollection();
    JS_FreeValue(ctx, globalObj);
    js_std_loop(engineInstance_->GetQjsContext());
}

void QjsEngine::UpdateApplicationState(const std::string& packageName, Frontend::State state)
{
    LOGI("UpdateApplicationState: destroy app instance from jsfwk, packageName %{public}s", packageName.c_str());
    JSContext* ctx = engineInstance_->GetQjsContext();
    ACE_DCHECK(ctx);

    std::string stateType;
    switch (state) {
        case Frontend::State::ON_CREATE:
            break;
        case Frontend::State::ON_DESTROY:
            stateType = "appDestroy";
            break;
        case Frontend::State::ON_SHOW:
            stateType = "appShow";
            break;
        case Frontend::State::ON_HIDE:
            stateType = "appHide";
            break;
        default:
            LOGE("error State: %d", state);
            break;
    }

    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appDestroyFunc = QJSUtils::GetPropertyStr(ctx, globalObj, stateType.c_str());
    if (!JS_IsFunction(ctx, appDestroyFunc)) {
        LOGE("appDestroyFunc is not found, cannot destroy page instance!");
        JS_FreeValue(ctx, globalObj);
        return;
    }

    JSValue name = QJSUtils::NewString(ctx, packageName.c_str());
    JSValueConst argv[] = { name };
    JSValue retVal = QJSUtils::Call(ctx, appDestroyFunc, JS_UNDEFINED, countof(argv), argv);

    if (JS_IsException(retVal)) {
        LOGE("Qjs appDestroyFunc FAILED!");
        QJSUtils::JsStdDumpErrorAce(ctx, JsErrorType::DESTROY_APP_ERROR, instanceId_, nullptr,
            engineInstance_->GetRunningPage());
    }

    JS_FreeValue(ctx, globalObj);
    js_std_loop(engineInstance_->GetQjsContext());
}

bool QjsEngine::OnStartContinuation()
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appObj = QJSUtils::GetPropertyStr(ctx, globalObj, "aceapp");
    if (!JS_IsObject(appObj)) {
        LOGE("aceapp is not found.");
        return false;
    }
    JSValue defObj = QJSUtils::GetPropertyStr(ctx, appObj, "$def");
    if (!JS_IsObject(defObj)) {
        LOGE("$def is not found.");
        return false;
    }
    JSValue func = QJSUtils::GetPropertyStr(ctx, defObj, "onStartContinuation");
    if (!JS_IsFunction(ctx, func)) {
        LOGE("onStartContinuation is not found.");
        JS_FreeValue(ctx, globalObj);
        return false;
    }
    JSValueConst argv[] = {};
    JSValue ret = QJSUtils::Call(ctx, func, defObj, countof(argv), argv);
    std::string result = JS_ToCString(ctx, ret);
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
    return (result == "true");
}

void QjsEngine::OnCompleteContinuation(int32_t code)
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appObj = QJSUtils::GetPropertyStr(ctx, globalObj, "aceapp");
    if (!JS_IsObject(appObj)) {
        LOGE("aceapp is not found.");
        return;
    }
    JSValue defObj = QJSUtils::GetPropertyStr(ctx, appObj, "$def");
    if (!JS_IsObject(defObj)) {
        LOGE("$def is not found.");
        return;
    }
    JSValue func = QJSUtils::GetPropertyStr(ctx, defObj, "onCompleteContinuation");
    if (!JS_IsFunction(ctx, func)) {
        LOGE("onCompleteContinuation is not found.");
        JS_FreeValue(ctx, globalObj);
        return;
    }
    JSValueConst argv[] = { JS_NewInt32(ctx, code) };
    QJSUtils::Call(ctx, func, defObj, countof(argv), argv);
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
}

void QjsEngine::OnRemoteTerminated()
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appObj = QJSUtils::GetPropertyStr(ctx, globalObj, "aceapp");
    if (!JS_IsObject(appObj)) {
        LOGE("aceapp is not found.");
        return;
    }
    JSValue defObj = QJSUtils::GetPropertyStr(ctx, appObj, "$def");
    if (!JS_IsObject(defObj)) {
        LOGE("$def is not found.");
        return;
    }
    JSValue func = QJSUtils::GetPropertyStr(ctx, defObj, "onRemoteTerminated");
    if (!JS_IsFunction(ctx, func)) {
        LOGE("onRemoteTerminated is not found.");
        JS_FreeValue(ctx, globalObj);
        return;
    }
    JSValueConst argv[] = {};
    QJSUtils::Call(ctx, func, defObj, countof(argv), argv);
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
}

void QjsEngine::OnSaveData(std::string& data)
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appObj = QJSUtils::GetPropertyStr(ctx, globalObj, "aceapp");
    if (!JS_IsObject(appObj)) {
        LOGE("aceapp is not found.");
        return;
    }
    JSValue defObj = QJSUtils::GetPropertyStr(ctx, appObj, "$def");
    if (!JS_IsObject(defObj)) {
        LOGE("$def is not found.");
        return;
    }
    JSValue func = QJSUtils::GetPropertyStr(ctx, defObj, "onSaveData");
    if (!JS_IsFunction(ctx, func)) {
        LOGE("onSaveData is not found.");
        JS_FreeValue(ctx, globalObj);
        return;
    }
    JSValue object = JS_NewObject(ctx);
    JSValueConst argv[] = { object };
    JSValue ret = QJSUtils::Call(ctx, func, defObj, countof(argv), argv);
    if (JS_ToCString(ctx, ret) == std::string("true")) {
        data = ScopedString::Stringify(ctx, object);
    }
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
}

bool QjsEngine::OnRestoreData(const std::string& data)
{
    JSContext* ctx = engineInstance_->GetQjsContext();
    QJSHandleScope handleScope(ctx);
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue appObj = QJSUtils::GetPropertyStr(ctx, globalObj, "aceapp");
    if (!JS_IsObject(appObj)) {
        LOGE("aceapp is not found.");
        return false;
    }
    JSValue defObj = QJSUtils::GetPropertyStr(ctx, appObj, "$def");
    if (!JS_IsObject(defObj)) {
        LOGE("$def is not found.");
        return false;
    }
    JSValue func = QJSUtils::GetPropertyStr(ctx, defObj, "onRestoreData");
    if (!JS_IsFunction(ctx, func)) {
        LOGE("onRestoreData is not found.");
        JS_FreeValue(ctx, globalObj);
        return false;
    }
    JSValue jsonObj = JS_ParseJSON(ctx, data.c_str(), data.length(), "");
    if (JS_IsUndefined(jsonObj) || JS_IsException(jsonObj)) {
        LOGE("Parse json for restore data failed.");
        return false;
    }
    JSValueConst argv[] = { jsonObj };
    JSValue ret = QJSUtils::Call(ctx, func, defObj, countof(argv), argv);
    std::string result = JS_ToCString(ctx, ret);
    js_std_loop(ctx);
    JS_FreeValue(ctx, globalObj);
    return (result == "true");
}

void QjsEngine::TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval)
{
    if (isInterval) {
        engineInstance_->CallJs(callbackId, std::string("{}"), true, true);
        engineInstance_->GetDelegate()->WaitTimer(callbackId, delay, isInterval, false);
    } else {
        engineInstance_->CallJs(callbackId, std::string("{}"), false, true);
        engineInstance_->GetDelegate()->ClearTimer(callbackId);
    }
}

void QjsEngine::MediaQueryCallback(const std::string& callbackId, const std::string& args)
{
    JsEngine::MediaQueryCallback(callbackId, args);
    if (!callbackId.empty() && engineInstance_) {
        engineInstance_->CallJs(callbackId.c_str(), args.c_str(), true, false);
    }
}

void QjsEngine::RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp)
{
    if (engineInstance_) {
        engineInstance_->CallJs(callbackId.c_str(), std::to_string(timeStamp).c_str(), false, true);
        engineInstance_->GetDelegate()->CancelAnimationFrame(callbackId);
    }
}

void QjsEngine::JsCallback(const std::string& callbackId, const std::string& args)
{
    if (engineInstance_) {
        engineInstance_->CallJs(callbackId.c_str(), args.c_str(), true, false);
    }
}

void QjsEngine::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEvent\"}]");
    LOGD("FireAsyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    JSValue cppToJsRet = engineInstance_->FireJsEvent(callBuf);
    if (JS_IsException(cppToJsRet)) {
        LOGE("Qjs FireAsyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
        QJSUtils::JsStdDumpErrorAce(engineInstance_->GetQjsContext(), JsErrorType::FIRE_EVENT_ERROR, instanceId_,
            nullptr, engineInstance_->GetRunningPage());
    }
    js_std_loop(engineInstance_->GetQjsContext());
    JS_FreeValue(engineInstance_->GetQjsContext(), cppToJsRet);
}

void QjsEngine::FireSyncEvent(const std::string& eventId, const std::string& param)
{
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEventSync\"}]");
    LOGD("FireSyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    JSValue cppToJsRet = engineInstance_->FireJsEvent(callBuf.c_str());
    if (JS_IsException(cppToJsRet)) {
        LOGE("Qjs FireSyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
        QJSUtils::JsStdDumpErrorAce(engineInstance_->GetQjsContext(), JsErrorType::FIRE_EVENT_ERROR, instanceId_,
            nullptr, engineInstance_->GetRunningPage());
    }
    js_std_loop(engineInstance_->GetQjsContext());
    JS_FreeValue(engineInstance_->GetQjsContext(), cppToJsRet);
}

void QjsEngine::FireExternalEvent(const std::string& componentId, const uint32_t nodeId)
{
    ACE_DCHECK(engineInstance_);
    auto context = engineInstance_->GetQjsContext();

    auto page = engineInstance_->GetRunningPage();
    if (page == nullptr) {
        LOGE("FireExternalEvent GetRunningPage is nullptr");
        return;
    }
    std::string arguments;
    auto bridge = AceType::DynamicCast<QjsXComponentBridge>(page->GetXComponentBridgeById(nodeId));
    if (bridge) {
        bridge->HandleContext(context, nodeId, arguments);
        return;
    }
}

void QjsEngine::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
{
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetJsMessageDispatcher(dispatcher);
}

void QjsEngine::RunGarbageCollection()
{
    if (engineInstance_ && engineInstance_->GetQjsRuntime()) {
        JS_RunGC(engineInstance_->GetQjsRuntime());
    }
}

RefPtr<GroupJsBridge> QjsEngine::GetGroupJsBridge()
{
    return AceType::MakeRefPtr<QuickJsGroupJsBridge>();
}

} // namespace OHOS::Ace::Framework
