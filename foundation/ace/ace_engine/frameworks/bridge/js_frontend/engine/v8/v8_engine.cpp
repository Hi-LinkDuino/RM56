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

#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

#include <dirent.h>
#include <dlfcn.h>
#include <regex>
#include <unistd.h>

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/common/thread_checker.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/plugin_adapter/plugin_bridge.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/common/utils/v8/v8_helper.h"
#include "frameworks/bridge/js_frontend/engine/common/js_api_perf.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_animator_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_badge_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_chart_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_clock_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_component_api_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_image_animator_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_input_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_list_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_stepper_bridge.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_xcomponent_bridge.h"

#if !defined(USE_EXTERNAL_V8_SNAPSHOT)
extern const char _binary_strip_native_min_js_bin_start[];
extern const char _binary_strip_native_min_js_bin_end[];
#endif

namespace OHOS::Ace::Framework {
namespace {

const int32_t CUSTOM_FULL_WINDOW_LENGTH = 2;
const int32_t MAX_READ_TEXT_LENGTH = 4096;
const std::regex URI_PARTTEN("^\\/([a-z0-9A-Z_]+\\/)*[a-z0-9A-Z_]+\\.?[a-z0-9A-Z_]*$");
constexpr int32_t V8_MAX_STACK_SIZE = 1 * 1024 * 1024;
void* g_debugger = nullptr;
bool g_flagNeedDebugBreakPoint = false;
static int32_t globalNodeId = 100000;
using StartDebug = void (*)(
    const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context, std::string componentName,
    const bool isDebugMode, const int32_t instanceId);
using WaitingForIde = void (*)();
using StopDebug = void (*)();

bool CallEvalBuf(v8::Isolate* isolate, const char* src, int32_t instanceId, const char* filename = nullptr)
{
    ACE_DCHECK(isolate);
    CHECK_RUN_ON(JS);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("CallEvalBuf, CurrentContext is empty!");
        return false;
    }
    v8::TryCatch tryCatch(isolate);
    if (src == nullptr || src[0] == '\0') {
        LOGE("src is null");
        return false;
    }

    const char* origin = filename;
    if (!origin) {
        origin = "<anonymous>";
    }

    v8::ScriptOrigin scriptOrigin(v8::String::NewFromUtf8(isolate, origin).ToLocalChecked());

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, src).ToLocalChecked();
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source, &scriptOrigin).ToLocal(&script)) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::COMPILE_ERROR, instanceId);
        return false;
    }

    v8::Local<v8::Value> res;
    if (!script->Run(context).ToLocal(&res)) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::RUNTIME_ERROR, instanceId);
        return false;
    }
    return true;
}

RefPtr<JsAcePage> GetStagingPage()
{
    LOGD("Enter GetStagingPage");
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::STAGING_PAGE));
    return *stagingPage;
}

RefPtr<JsAcePage> GetRunningPage()
{
    LOGD("Enter GetRunningPage");
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    auto runningPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    return *runningPage;
}

void GetArrayValue(v8::Local<v8::Context> context, v8::Local<v8::Value> arrayVal, std::string& result)
{
    LOGD("Enter GetArrayValue");
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> valObj;
    bool succ = arrayVal->ToObject(context).ToLocal(&valObj);
    if (!succ) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = valObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        if (i != 0) {
            result.append(1, DOM_PICKER_SPLIT_ARRAY);
        }
        v8::Local<v8::Value> itemKey = properties->Get(context, i).ToLocalChecked();
        v8::Local<v8::Value> itemVal = valObj->Get(context, itemKey).ToLocalChecked();
        if (itemVal->IsString() || itemVal->IsNumber() || itemVal->IsBoolean()) {
            v8::String::Utf8Value val(isolate, itemVal);
            if (*val) {
                result.append(*val);
            }
            continue;
        }
        if (itemVal->IsArray()) {
            v8::Local<v8::Array> itemArray = v8::Local<v8::Array>::Cast(itemVal);
            uint32_t subLen = itemArray->Length();
            for (uint32_t j = 0; j < subLen; j++) {
                if (j != 0) {
                    result.append(1, DOM_PICKER_SPLIT_ITEM);
                }
                v8::Local<v8::Value> subItemVal = itemArray->Get(context, j).ToLocalChecked();
                v8::String::Utf8Value subItemStr(isolate, subItemVal);
                if (*subItemStr) {
                    result.append(*subItemStr);
                }
            }
            continue;
        }
    }
}

void GetAttrImage(v8::Local<v8::Context> context, v8::Local<v8::Value> objectVal, ImageProperties& imageProperties)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> valObj = objectVal->ToObject(context).ToLocalChecked();
    v8::Local<v8::Array> valObjProperties = valObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t objLen = valObjProperties->Length();
    for (uint32_t i = 0; i < objLen; i++) {
        v8::Local<v8::Value> valObjKey;
        bool succ = valObjProperties->Get(context, i).ToLocal(&valObjKey);
        if (!succ) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value valObjKeyStr(isolate, valObjKey);
        const char* keyStr = *valObjKeyStr;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> valObjVal = valObj->Get(context, valObjKey).ToLocalChecked();
        if (valObjVal->IsString() || valObjVal->IsNumber() || valObjVal->IsBoolean()) {
            v8::String::Utf8Value valObjValStr(isolate, valObjVal);
            const char* valStr = *valObjValStr;
            if (valStr == nullptr) {
                continue;
            }
            if (strcmp(keyStr, "src") == 0) {
                imageProperties.src = valStr;
            } else if (strcmp(keyStr, "width") == 0) {
                imageProperties.width = StringToDimension(valStr);
            } else if (strcmp(keyStr, "height") == 0) {
                imageProperties.height = StringToDimension(valStr);
            } else if (strcmp(keyStr, "top") == 0) {
                imageProperties.top = StringToDimension(valStr);
            } else if (strcmp(keyStr, "left") == 0) {
                imageProperties.left = StringToDimension(valStr);
            } else if (strcmp(keyStr, "duration") == 0) {
                imageProperties.duration = StringToInt(valStr);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetAttrImages(v8::Local<v8::Context> context, v8::Local<v8::Value> arrayVal, std::vector<ImageProperties>& images)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> arrayObj = arrayVal->ToObject(context).ToLocalChecked();
    v8::Local<v8::Array> properties = arrayObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key = properties->Get(context, i).ToLocalChecked();
        v8::Local<v8::Value> val = arrayObj->Get(context, key).ToLocalChecked();
        ImageProperties imageProperties;
        if (val->IsObject()) {
            GetAttrImage(context, val, imageProperties);
            images.push_back(imageProperties);
        }
    }
}

bool SetDomAttributes(
    v8::Local<v8::Context> context, v8::Local<v8::Value> fromMap, JsCommandDomElementOperator& command)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    LOGD("Enter SetDomAttributes");
    bool hasShowAttr = false;
    if (fromMap.IsEmpty() || !fromMap->IsObject()) {
        LOGE("none found attrs");
        return false;
    }
    v8::Local<v8::Object> mapObj;
    bool succ = fromMap->ToObject(context).ToLocal(&mapObj);
    if (!succ) {
        LOGE("Value to Object fail");
        return false;
    }
    v8::Local<v8::Array> properties = mapObj->GetOwnPropertyNames(context).ToLocalChecked();

    // begin to set attributes:
    std::vector<std::pair<std::string, std::string>> attrs;
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        succ = properties->Get(context, i).ToLocal(&key);
        if (!succ) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = mapObj->Get(context, key).ToLocalChecked();
        if (val->IsString() || val->IsNumber() || val->IsBoolean()) {
            v8::String::Utf8Value valV8Str(isolate, val);
            const char* valStr = *valV8Str;
            if (valStr == nullptr) {
                continue;
            }
            LOGD("SetDomAttributes: key %{private}s, attr: %{private}s", keyStr, valStr);
            if (strcmp(keyStr, DOM_ID) == 0) {
                command.SetId(valStr);
            } else if (strcmp(keyStr, DOM_TARGET) == 0) {
                command.SetTarget(valStr);
            } else if (strcmp(keyStr, DOM_SHARE_ID) == 0) {
                command.SetShareId(valStr);
            }
            attrs.emplace_back(keyStr, valStr);
            if (strcmp(keyStr, DOM_SHOW) == 0) {
                hasShowAttr = true;
            }
        } else if (val->IsArray()) {
            if (strcmp(keyStr, "datasets") == 0) {
                auto chartBridge = AceType::MakeRefPtr<V8ChartBridge>();
                chartBridge->GetAttrDatasets(context, val);
                command.SetDatasets(chartBridge->GetDatasets());
            } else if (strcmp(keyStr, "images") == 0) {
                std::vector<ImageProperties> images;
                GetAttrImages(context, val, images);
                command.SetImagesAttr(std::move(images));
            } else if (strcmp(keyStr, "segments") == 0) {
                auto chartBridge = AceType::MakeRefPtr<V8ChartBridge>();
                chartBridge->ParseAttrSegmentArray(context, val);
                command.SetSegments(chartBridge->GetSegments());
            } else if (strcmp(keyStr, "menuoptions") == 0) {
                auto inputBridge = AceType::MakeRefPtr<V8InputBridge>();
                inputBridge->ParseInputOptions(context, val);
                command.SetInputOptions(inputBridge->GetInputOptions());
            } else {
                std::string valStr;
                GetArrayValue(context, val, valStr);
                LOGD("SetDomAttributes: key: %{private}s, attr: %{private}s", keyStr, valStr.c_str());
                attrs.emplace_back(keyStr, valStr);
            }
        } else if (val->IsObject()) {
            if (strcmp(keyStr, "options") == 0) {
                auto chartBridge = AceType::MakeRefPtr<V8ChartBridge>();
                chartBridge->GetAttrOptionsObject(context, val);
                command.SetOptions(chartBridge->GetChartOptions());
            } else if (strcmp(keyStr, DOM_CLOCK_CONFIG) == 0) {
                auto clockBridge = AceType::MakeRefPtr<V8ClockBridge>();
                clockBridge->ParseClockConfig(context, val);
                command.SetClockConfig(clockBridge->GetClockConfig());
            } else if (strcmp(keyStr, DOM_STEPPER_LABEL) == 0) {
                auto stepperBridge = AceType::MakeRefPtr<V8StepperBridge>();
                StepperLabels label;
                stepperBridge->GetAttrLabel(context, val, label);
                command.SetStepperLabel(label);
            } else if (strcmp(keyStr, "segments") == 0) {
                auto chartBridge = AceType::MakeRefPtr<V8ChartBridge>();
                chartBridge->ParseAttrSingleSegment(context, val);
                command.SetSegments(chartBridge->GetSegments());
            } else if (strcmp(keyStr, DOM_BADGE_CONFIG) == 0) {
                auto badgeConfig = AceType::MakeRefPtr<V8BadgeBridge>();
                badgeConfig->ParseBadgeConfig(context, val);
                command.SetBadgeConfig(badgeConfig->GetBadgeConfig());
            } else {
                LOGD("key unsupported. Ignoring!");
            }
        } else if (val->IsUndefined()) {
            LOGE("value of key[%{private}s] is undefined. Ignoring!", keyStr);
        } else {
            LOGE("value of key[%{private}s] unsupported type. Ignoring!", keyStr);
        }
    }

    command.SetAttributes(std::move(attrs));
    return hasShowAttr;
}

void GetAndRegisterFamily(v8::Local<v8::Context> context, v8::Local<v8::Value> val, std::string& familyStyle)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetAndRegisterFamily");
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> valObj = val->ToObject(context).ToLocalChecked();
    std::string familyVal;
    std::string srcVal;
    v8::Local<v8::Array> properties = valObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        bool succ = properties->Get(context, i).ToLocal(&key);
        if (!succ) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyJsStr(isolate, key);
        const char* keyStr = *keyJsStr;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> valValue = valObj->Get(context, key).ToLocalChecked();
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        if (valValue->IsString()) {
            v8::String::Utf8Value valJsStr(isolate, valValue);
            const char* valStr = *valJsStr;
            if (valStr == nullptr) {
                continue;
            }
            if (strcmp(keyStr, "fontFamily") == 0) {
                familyVal = valStr;
                if (!srcVal.empty()) {
                    (*delegate)->RegisterFont(familyVal, srcVal);
                    familyVal.erase();
                    srcVal.erase();
                }
                if (familyStyle.length() > 0) {
                    familyStyle.append(",");
                }
                familyStyle.append(valStr);
            } else if (strcmp(keyStr, "src") == 0) {
                srcVal = valStr;
                // The format of font src is: url("src"), here get the src.
                srcVal = srcVal.substr(URL_SOURCE_START_IDX, srcVal.length() - URL_SOURCE_SUFFIX_LEN);
                if (!familyVal.empty()) {
                    (*delegate)->RegisterFont(familyVal, srcVal);
                    familyVal.erase();
                    srcVal.erase();
                }
            }
        }
    }
}

void GetStyleFamilyValue(v8::Local<v8::Context> context, v8::Local<v8::Value> arrayVal, std::string& familyStyle)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetStyleFamilyValue");
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> arrayObj = arrayVal->ToObject(context).ToLocalChecked();
    v8::Local<v8::Array> properties = arrayObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key = properties->Get(context, i).ToLocalChecked();
        v8::Local<v8::Value> val = arrayObj->Get(context, key).ToLocalChecked();
        // ValObj is one row of family object
        if (val->IsObject()) {
            GetAndRegisterFamily(context, val, familyStyle);
        }
    }
}

void GetStyleAnimationName(v8::Local<v8::Context> context, v8::Local<v8::Value> arrayVal,
    std::vector<std::unordered_map<std::string, std::string>>& styleVec)
{
    CHECK_RUN_ON(JS);
    LOGD("GetStyleAnimationName");
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> arrayObj = arrayVal->ToObject(context).ToLocalChecked();

    v8::Local<v8::Array> properties = arrayObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    for (uint32_t i = 0; i < len; i++) {
        std::unordered_map<std::string, std::string> animationNameKeyFrame;
        v8::Local<v8::Value> key = properties->Get(context, i).ToLocalChecked();
        v8::Local<v8::Value> val = arrayObj->Get(context, key).ToLocalChecked();
        if (val->IsObject()) {
            v8::Local<v8::Object> valObj = val->ToObject(context).ToLocalChecked();
            v8::Local<v8::Array> valObjProperties = valObj->GetOwnPropertyNames(context).ToLocalChecked();
            uint32_t objLen = valObjProperties->Length();
            for (uint32_t j = 0; j < objLen; j++) {
                v8::Local<v8::Value> valObjKey;
                bool succ = valObjProperties->Get(context, j).ToLocal(&valObjKey);
                if (!succ) {
                    LOGW("key is null. Ignoring!");
                    continue;
                }
                v8::Local<v8::Value> valObjVal = valObj->Get(context, valObjKey).ToLocalChecked();
                v8::String::Utf8Value valObjKeyStr(isolate, valObjKey);
                v8::String::Utf8Value valObjValStr(isolate, valObjVal);
                const char* keyStr = *valObjKeyStr;
                const char* valStr = *valObjValStr;
                if ((keyStr != nullptr) && (valStr != nullptr) && (valObjVal->IsString() || valObjVal->IsNumber())) {
                    animationNameKeyFrame.emplace(keyStr, valStr);
                } else {
                    LOGD("GetStyleAnimationName: unsupported type :%{public}s", valStr);
                }
            }
        }
        if (animationNameKeyFrame.size() > 0) {
            styleVec.emplace_back(animationNameKeyFrame);
        }
    }
}

bool SetDomStyle(v8::Local<v8::Context> context, v8::Local<v8::Value> fromMap, JsCommandDomElementOperator& command)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    LOGD("Enter SetDomStyle");
    if (fromMap.IsEmpty() || !fromMap->IsObject()) {
        LOGE("none found attrs");
        return false;
    }
    v8::Local<v8::Object> mapObj;
    bool succ = fromMap->ToObject(context).ToLocal(&mapObj);
    if (!succ) {
        LOGE("Value to Object fail");
        return false;
    }
    v8::Local<v8::Array> properties = mapObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    // begin to set dom styles:
    std::vector<std::pair<std::string, std::string>> styles;
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        succ = properties->Get(context, i).ToLocal(&key);
        if (!succ) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = mapObj->Get(context, key).ToLocalChecked();
        if (val->IsString() || val->IsNumber() || val->IsBoolean()) {
            v8::String::Utf8Value valV8Str(isolate, val);
            const char* valStr = *valV8Str;
            if (valStr != nullptr) {
                LOGD("SetDomStyle: key: %{private}s, style: %{private}s", keyStr, valStr);
                styles.emplace_back(keyStr, valStr);
            }
        } else if (val->IsArray()) {
            if (strcmp(keyStr, DOM_TEXT_FONT_FAMILY) == 0) {
                // Deal with special case such as fontFamily, suppose all the keys in the array are the same.
                std::string familyStyle;
                GetStyleFamilyValue(context, val, familyStyle);
                styles.emplace_back(keyStr, familyStyle);
            } else if (strcmp(keyStr, DOM_ANIMATION_NAME) == 0) {
                // Deal with special case animationName, it different with fontfamily,
                // the keys in the array are different.
                std::vector<std::unordered_map<std::string, std::string>> animationNameStyles;
                GetStyleAnimationName(context, val, animationNameStyles);
                command.SetAnimationStyles(std::move(animationNameStyles));
            } else if (strcmp(keyStr, DOM_TRANSITION_ENTER) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionEnter;
                GetStyleAnimationName(context, val, transitionEnter);
                command.SetTransitionEnter(std::move(transitionEnter));
            } else if (strcmp(keyStr, DOM_TRANSITION_EXIT) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionExit;
                GetStyleAnimationName(context, val, transitionExit);
                command.SetTransitionExit(std::move(transitionExit));
            } else if (strcmp(keyStr, DOM_SHARED_TRANSITION_NAME) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> sharedTransitionName;
                GetStyleAnimationName(context, val, sharedTransitionName);
                command.SetSharedTransitionName(std::move(sharedTransitionName));
            } else {
                LOGD("value is array, key unsupported. Ignoring!");
            }
        } else if (val->IsUndefined()) {
            LOGD("value is undefined. Ignoring!");
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    auto pipelineContext = (*delegate)->GetPipelineContext();
    command.SetPipelineContext(pipelineContext);
    command.SetStyles(std::move(styles));
    return true;
}

void AddDomEvent(v8::Local<v8::Context> context, v8::Local<v8::Value> fromMap, JsCommandDomElementOperator& command)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    LOGD("Enter AddDomEvent");

    if (fromMap.IsEmpty() || !fromMap->IsObject()) {
        return;
    }
    v8::Local<v8::Object> mapObj;
    bool succ = fromMap->ToObject(context).ToLocal(&mapObj);
    if (!succ) {
        LOGE("Value to Object fail");
        return;
    }

    v8::Local<v8::Array> properties = mapObj->GetOwnPropertyNames(context).ToLocalChecked();
    uint32_t len = properties->Length();
    std::vector<std::string> eventsMap;
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        succ = properties->Get(context, i).ToLocal(&key);
        if (!succ) {
            LOGD("key not exist");
            continue;
        }
        v8::Local<v8::Value> val;
        succ = mapObj->Get(context, key).ToLocal(&val);
        if (!succ) {
            LOGD("val not exist");
            continue;
        }
        if (val->IsString()) {
            v8::String::Utf8Value valV8Str(isolate, val);
            const char* valStr = *valV8Str;
            if (valStr != nullptr) {
                eventsMap.emplace_back(valStr);
            }
        } else {
            LOGW("value of unsupported type. Ignoring!");
        }
    }
    command.AddEvents(std::move(eventsMap));
}

void GetAppInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetAppInfo");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("get app info, context is empty!");
        return;
    }

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    v8::Local<v8::String> appID = v8::String::NewFromUtf8(isolate, (*delegate)->GetAppID().c_str()).ToLocalChecked();
    v8::Local<v8::String> appName =
        v8::String::NewFromUtf8(isolate, (*delegate)->GetAppName().c_str()).ToLocalChecked();
    v8::Local<v8::String> versionName =
        v8::String::NewFromUtf8(isolate, (*delegate)->GetVersionName().c_str()).ToLocalChecked();
    v8::Local<v8::Integer> versionCode = v8::Integer::New(isolate, (*delegate)->GetVersionCode());

    // return the result as an object
    v8::Local<v8::Object> res = v8::Object::New(isolate);
    res->Set(context, v8::String::NewFromUtf8(isolate, "appID").ToLocalChecked(), appID).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "appName").ToLocalChecked(), appName).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "versionName").ToLocalChecked(), versionName).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "versionCode").ToLocalChecked(), versionCode).ToChecked();
    args.GetReturnValue().Set(res);
}

void Terminate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->Finish();
        }
    });
}

void RequestFullWindow(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }

    int32_t duration = -1; // default scene
    if (args[index]->IsArray()) {
        v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[index]);
        uint32_t len = arrayVal->Length();
        if (len == CUSTOM_FULL_WINDOW_LENGTH) {
            v8::Local<v8::Value> jsParam = arrayVal->Get(context, 0).ToLocalChecked();
            if (jsParam->IsObject()) {
                v8::Local<v8::Object> valObj = jsParam->ToObject(context).ToLocalChecked();
                v8::Local<v8::Array> valObjProperties = valObj->GetOwnPropertyNames(context).ToLocalChecked();
                v8::Local<v8::Value> valObjKey;
                bool succ = valObjProperties->Get(context, 0).ToLocal(&valObjKey);
                v8::String::Utf8Value jsDurationKey(isolate, valObjKey);
                if (succ && *jsDurationKey && (std::strcmp(*jsDurationKey, APP_REQUEST_FULL_WINDOW_DURATION) == 0)) {
                    v8::Local<v8::Value> valObjVal = valObj->Get(context, valObjKey).ToLocalChecked();
                    v8::String::Utf8Value jsDurationValue(isolate, valObjVal);
                    if (*jsDurationValue) {
                        duration = StringToInt(*jsDurationValue);
                    }
                }
            }
            if (duration < 0) {
                duration = -1; // default scene
            }
        }
    }
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, duration]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->RequestFullWindow(duration);
        }
    });
}

void GetPackageInfoCallback(
    v8::Local<v8::Context> context, const v8::Local<v8::Value>& jsMessage, const char* callbackId)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetPackageInfoCallback");
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);

    if (jsMessage.IsEmpty() || !jsMessage->IsObject()) {
        LOGE("GetPackageInfoCallback: jsMessage is not Object");
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            std::string("{\"arguments\":[\"jsMessage is not Object\",200],\"method\":\"fail\"}"), false);
        return;
    }

    auto messageObject = jsMessage->ToObject(context).ToLocalChecked();
    v8::Local<v8::String> tempStr;
    if (messageObject->Get(context, v8::String::NewFromUtf8(isolate, APP_PACKAGE_NAME).ToLocalChecked())
            .ToLocalChecked()
            ->IsString()) {
        tempStr = messageObject->Get(context, v8::String::NewFromUtf8(isolate, APP_PACKAGE_NAME).ToLocalChecked())
                      .ToLocalChecked()
                      ->ToString(context)
                      .ToLocalChecked();
    } else {
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            std::string("{\"arguments\":[\"packageName is not available string\",202],\"method\":\"fail\"}"), false);
        return;
    }

    v8::String::Utf8Value utf8Str(isolate, tempStr);
    if (*utf8Str == nullptr) {
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            std::string("{\"arguments\":[\"packageName is not available string\",202],\"method\":\"fail\"}"), false);
        return;
    }
    std::string packageName(*utf8Str);
    AceBundleInfo bundleInfo;
    if (AceApplicationInfo::GetInstance().GetBundleInfo(packageName, bundleInfo)) {
        auto infoList = JsonUtil::Create(true);
        infoList->Put("versionName", bundleInfo.versionName.c_str());
        infoList->Put("versionCode", std::to_string(bundleInfo.versionCode).c_str());
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            std::string("{\"arguments\":[").append(infoList->ToString()).append("],\"method\":\"success\"}"), false);
    } else {
        LOGE("can not get info by GetBundleInfo");
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            std::string("{\"arguments\":[\"can not get info\",200],\"method\":\"fail\"}"), false);
    }
}

void GetPackageInfo(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetPackageInfo");
    if (!args[index]->IsObject()) {
        LOGE("GetPackageInfo: argv is not Object");
        return;
    }

    if (!args[index]->IsArray()) {
        LOGE("GetPackageInfo: argv is not Array");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("GetPackageInfo, context is empty!");
        return;
    }
    v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[index]);
    uint32_t len = arrayVal->Length();
    if (len < PAG_INFO_ARGS_LEN) {
        LOGE("GetPackageInfo: invalid callback value");
        return;
    }
    v8::Local<v8::Value> jsMessage = arrayVal->Get(context, PAG_INFO_ARGS_MSG_IDX).ToLocalChecked();
    v8::Local<v8::Value> jsCallBackId = arrayVal->Get(context, 1).ToLocalChecked();
    v8::String::Utf8Value callbackId(isolate, jsCallBackId);
    LOGD("system app getPackageInfo callBackID is %{public}s", *callbackId);

    if (*callbackId != nullptr) {
        GetPackageInfoCallback(context, jsMessage, *callbackId);
    }
}

void SetScreenOnVisible(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    if (isolate == nullptr) {
        LOGE("SetScreenOnVisible: isolate is nullptr");
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("SetScreenOnVisible, context is empty!");
        return;
    }
    std::map<std::string, std::string> params;
    std::string callbackId;
    if (!GetParamsWithCallbackId(context, v8::Local<v8::Array>::Cast(args[index]), params, callbackId)) {
        LOGE("SetScreenOnVisible, get params or callbackId failed!");
        return;
    }
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);
    std::string flag = "fail";
    auto iter = params.find(APP_SCREEN_ON_VISIBLE_FLAG);
    if (iter != params.end()) {
        flag = iter->second;
    }
    if (!StringToBool(flag)) {
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            R"({"arguments":["fail to set false flag in rich platform", 200],"method":"fail"})");
    } else {
        V8EngineInstance::CallJs(isolate, pContext, callbackId, R"({"arguments":[],"method":"success"})");
    }
    pContext.Reset();
}

void SetSwipeToDismiss(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    if (isolate == nullptr) {
        LOGE("SetSwipeToDismiss: isolate is nullptr");
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("SetSwipeToDismiss, context is empty!");
        return;
    }

    if (!args[index]->IsArray()) {
        LOGE("SetSwipeToDismiss: argv is not Array");
        return;
    }

    v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[index]);
    uint32_t len = arrayVal->Length();
    if (len < 2) {
        LOGE("SetSwipeToDismiss: invalid callback value");
        return;
    }

    bool forbideQuit = false;
    v8::Local<v8::Value> flag = arrayVal->Get(context, 0).ToLocalChecked();
    if (flag->IsBoolean()) {
        forbideQuit = flag->BooleanValue(isolate);
    }

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, forbideQuit]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->SetForbidePlatformQuit(forbideQuit);
        }
    });
}

void JsHandleAppApi(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    LOGD("Enter JsHandleAppApi");
    if (methodName == APP_GET_INFO) {
        GetAppInfo(args);
    } else if (methodName == APP_TERMINATE) {
        Terminate(args);
    } else if (methodName == APP_GET_PACKAGE_INFO) {
        GetPackageInfo(args, index);
    } else if (methodName == APP_REQUEST_FULL_WINDOW) {
        RequestFullWindow(args, index);
    } else if (methodName == APP_SCREEN_ON_VISIBLE) {
        SetScreenOnVisible(args, index);
    } else if (methodName == APP_SET_SWIPE_TO_DISMISS) {
        SetSwipeToDismiss(args, index);
    } else {
        LOGW("system.app not support method = %{public}s", methodName.c_str());
    }
}

void GetLocale(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter GetLocale");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGW("get configuration, context is empty!");
        return;
    }

    v8::Local<v8::String> language =
        v8::String::NewFromUtf8(isolate, AceApplicationInfo::GetInstance().GetLanguage().c_str()).ToLocalChecked();
    v8::Local<v8::String> countryOrRegion =
        v8::String::NewFromUtf8(isolate, AceApplicationInfo::GetInstance().GetCountryOrRegion().c_str())
            .ToLocalChecked();

    v8::Local<v8::String> dir = v8::String::NewFromUtf8(
        isolate, AceApplicationInfo::GetInstance().IsRightToLeft() ? LOCALE_TEXT_DIR_RTL : LOCALE_TEXT_DIR_LTR)
                                    .ToLocalChecked();
    v8::Local<v8::String> unicodeSetting =
        v8::String::NewFromUtf8(isolate, AceApplicationInfo::GetInstance().GetUnicodeSetting().c_str())
            .ToLocalChecked();

    v8::Local<v8::Object> res = v8::Object::New(isolate);
    res->Set(context, v8::String::NewFromUtf8(isolate, LOCALE_LANGUAGE).ToLocalChecked(), language).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, LOCALE_COUNTRY_OR_REGION).ToLocalChecked(), countryOrRegion)
        .ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, LOCALE_TEXT_DIR).ToLocalChecked(), dir).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, LOCALE_UNICODE_SETTING).ToLocalChecked(), unicodeSetting)
        .ToChecked();
    args.GetReturnValue().Set(res);
}

void JsSetLocale(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsGetLocale");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argStr(isolate, args[index]);
    if (!(*argStr)) {
        return;
    }
    std::unique_ptr<JsonValue> localeJson = JsonUtil::ParseJsonString(*argStr);

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
            auto delegate =
                static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
            (*delegate)->ChangeLocale(language, countryOrRegion);
        }
    }
}

void JsSetTimer(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index, bool isInterval)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsSetTimer");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("set time out or interval, context is empty!");
        return;
    }

    if (args[index]->IsArray()) {
        v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[index]);
        uint32_t len = arrayVal->Length();
        if (len < 2) {
            LOGW("JsSetTimer: invalid callback value");
            return;
        }
        v8::Local<v8::Value> jsCallBackId = arrayVal->Get(context, 0).ToLocalChecked();
        v8::Local<v8::Value> jsDelay = arrayVal->Get(context, 1).ToLocalChecked();
        v8::String::Utf8Value callBackId(isolate, jsCallBackId);
        v8::String::Utf8Value delay(isolate, jsDelay);
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        if (*callBackId && *delay) {
            (*delegate)->WaitTimer(*callBackId, *delay, isInterval, true);
        }
    }
}

void JsClearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsClearTimeout");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("clear timeout or clear interval, context is empty!");
        return;
    }

    if (args[index]->IsArray()) {
        v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[index]);
        uint32_t len = arrayVal->Length();
        if (len < 1) {
            LOGW("JsClearTimeout: invalid callback value");
            return;
        }
        v8::Local<v8::Value> jsCallBackId = arrayVal->Get(context, 0).ToLocalChecked();
        v8::String::Utf8Value callBackId(isolate, jsCallBackId);
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        if (*callBackId) {
            (*delegate)->ClearTimer(*callBackId);
        }
    }
}

void JsAddListener(v8::Local<v8::Context> context, v8::Local<v8::Value> val)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = context->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));

    if (val->IsObject() && val->IsArray()) {
        v8::Local<v8::Array> valArray = v8::Local<v8::Array>::Cast(val);
        uint32_t len = valArray->Length();
        if (len == 0) {
            return;
        }
        v8::Local<v8::Value> jsListenerId = valArray->Get(context, 0).ToLocalChecked();
        v8::String::Utf8Value listenerIdStr(isolate, jsListenerId);
        auto mediaQuery = (*delegate)->GetMediaQueryInfoInstance();
        if (mediaQuery && *listenerIdStr) {
            mediaQuery->SetListenerId(std::string(*listenerIdStr));
        }
    }
}

std::string JsParseRouteUrl(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& key, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsParseRouteUrl");
    std::string pageRoute;
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return "";
    }
    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);

    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsString()) {
        pageRoute = argsPtr->GetValue(key)->GetString();
    }
    LOGD("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());

    return pageRoute;
}

std::string JsParseRouteUrlSpecial(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    std::string pageRoute;
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return "";
    }
    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);

    if (argsPtr->Contains(ROUTE_KEY_URI)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    } else if (argsPtr->Contains(ROUTE_KEY_PATH)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_PATH)->GetString();
    }
    LOGD("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());

    return pageRoute;
}

bool JsParseRouteOverwrite(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& key, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return false;
    }

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
    if (argsPtr != nullptr && argsPtr->Contains(key)) {
        return true;
    }
    return false;
}

std::string JsParseRouteParams(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& key, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return "";
    }

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
    std::string params;
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsObject()) {
        params = argsPtr->GetValue(key)->ToString();
    }
    return params;
}

int32_t JsParseIntParams(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& key, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return 0;
    }

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsNumber()) {
        return argsPtr->GetValue(key)->GetInt();
    }
    return 0;
}

void EnableAlertBeforeBackPage(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter EnableAlertBeforeBackPage");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        LOGE("disableAlertBeforeBackPage args is null");
        return;
    }

    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
    if (argsPtr != nullptr && argsPtr->IsObject()) {
        if (argsPtr->GetValue(PROMPT_KEY_MESSAGE) == nullptr || !argsPtr->GetValue(PROMPT_KEY_MESSAGE)->IsString()) {
            LOGE("enableAlertBeforeBackPage message is null");
            const std::string fail = JsParseRouteUrl(args, COMMON_FAIL, index);
            const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
            engineInstance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})", false);
            engineInstance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})", false);
            return;
        }
    } else {
        LOGE("enableAlertBeforeBackPage message is null");
        std::string callBackStr(*argsJsStr);
        // Get callbackId and clear redundant symbols
        if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
            callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
            engineInstance->CallJs(callBackStr,
                R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:massage is null"}],"method":"fail"})", false);
        }
        return;
    }

    const std::string message = JsParseRouteUrl(args, PROMPT_KEY_MESSAGE, index);
    const std::string success = JsParseRouteUrl(args, COMMON_SUCCESS, index);
    const std::string fail = JsParseRouteUrl(args, COMMON_FAIL, index);
    const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
    auto callback = [engineInstance, success, fail, complete](int32_t callbackType) {
        switch (callbackType) {
            case 1:
                engineInstance->CallJs(success, R"({"errMsg":"enableAlertBeforeBackPage:ok"})", false);
                engineInstance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:ok"})", false);
                break;
            case 0:
                engineInstance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})", false);
                engineInstance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})", false);
                break;
            default:
                LOGE("callbackType is invalid");
                break;
        }
    };

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->EnableAlertBeforeBackPage(message, std::move(callback));
}

void DisableAlertBeforeBackPage(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter EnableAlertBeforeBackPage");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->DisableAlertBeforeBackPage();

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        LOGE("disableAlertBeforeBackPage args is null");
        return;
    }
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);

    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());
    if (argsPtr->IsObject()) {
        const std::string success = JsParseRouteUrl(args, COMMON_SUCCESS, index);
        const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
        engineInstance->CallJs(success, R"({"errMsg":"disableAlertBeforeBackPage:ok"})", false);
        engineInstance->CallJs(complete, R"({"errMsg":"disableAlertBeforeBackPage:ok"})", false);
        return;
    }

    std::string callBackStr(*argsJsStr);
    // Get callbackId and clear redundant symbols
    if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
        callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
        engineInstance->CallJs(
            callBackStr, R"({"arguments":[{"errMsg":"disableAlertBeforeBackPage:ok"}],"method":"success"})", false);
    }
}

void PostponePageTransition(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter PostponePageTransition");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostponePageTransition();
}

void LaunchPageTransition(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter LaunchPageTransition");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->LaunchPageTransition();
}

void JsHandlePageRoute(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsHandlePageRoute");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    std::string uri;
    if (methodName == ROUTE_PAGE_BACK) {
        uri = JsParseRouteUrlSpecial(args, index);
    } else {
        uri = JsParseRouteUrl(args, ROUTE_KEY_URI, index);
    }

    std::string params = JsParseRouteParams(args, ROUTE_KEY_PARAMS, index);
    bool dontOverwrite = JsParseRouteOverwrite(args, ROUTE_KEY_DONT_OVERWRITE, index);

    std::unique_ptr<JsonValue> routerParamsData = JsonUtil::Create(true);
    routerParamsData->Put("paramsData", JsonUtil::ParseJsonString(params));
    routerParamsData->Put("dontOverwrite", dontOverwrite);
    params = routerParamsData->ToString();

    static const std::unordered_map<std::string,
        void (*)(const v8::FunctionCallbackInfo<v8::Value>&, const std::string&, const std::string&, v8::Isolate*)>
        pageRouteOperators = {
            { ROUTE_PAGE_PUSH,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    LOGD("JsPushRoute uri = %{private}s", uri.c_str());
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    (*delegate)->Push(uri, params);
                } },
            { ROUTE_PAGE_REPLACE,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    LOGD("JsReplaceRoute uri = %{private}s", uri.c_str());
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    (*delegate)->Replace(uri, params);
                } },
            { ROUTE_PAGE_BACK,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    LOGD("JsBackRoute uri = %{private}s", uri.c_str());
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    (*delegate)->Back(uri);
                } },
            { ROUTE_PAGE_CLEAR,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    LOGD("JsClearPageRoute");
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    (*delegate)->Clear();
                } },
            { ROUTE_PAGE_GET_LENGTH,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    int32_t routeLength = (*delegate)->GetStackSize();
                    std::string indexLength = std::to_string(routeLength);
                    LOGD("JsGetLengthRoute routeLength = %{private}s", indexLength.c_str());
                    v8::Local<v8::String> lenStr =
                        v8::String::NewFromUtf8(isolate, indexLength.c_str()).ToLocalChecked();
                    args.GetReturnValue().Set(lenStr);
                } },
            { ROUTE_PAGE_GET_STATE,
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& uri, const std::string& params,
                    v8::Isolate* isolate) {
                    int32_t routeIndex = 0;
                    std::string routeName;
                    std::string routePath;
                    ACE_DCHECK(isolate);
                    auto delegate =
                        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
                    (*delegate)->GetState(routeIndex, routeName, routePath);
                    LOGD("JsGetStateRoute index=%{private}d name=%{private}s path=%{private}s", routeIndex,
                        routeName.c_str(), routePath.c_str());
                    auto context = isolate->GetCurrentContext();
                    if (context.IsEmpty()) {
                        LOGE("get router state, context is empty!");
                        return;
                    }
                    v8::Local<v8::Object> res = v8::Object::New(isolate);
                    v8::Local<v8::Integer> index = v8::Integer::New(isolate, routeIndex);
                    v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, routeName.c_str()).ToLocalChecked();
                    v8::Local<v8::String> path = v8::String::NewFromUtf8(isolate, routePath.c_str()).ToLocalChecked();
                    res->Set(context, v8::String::NewFromUtf8(isolate, "index").ToLocalChecked(), index).ToChecked();
                    res->Set(context, v8::String::NewFromUtf8(isolate, "name").ToLocalChecked(), name).ToChecked();
                    res->Set(context, v8::String::NewFromUtf8(isolate, "path").ToLocalChecked(), path).ToChecked();
                    args.GetReturnValue().Set(res);
                } },
        };
    // execute the page operation according to the actions defined at pageRouteOperators.
    auto operatorIter = pageRouteOperators.find(methodName);
    if (operatorIter != pageRouteOperators.end()) {
        operatorIter->second(args, uri, params, isolate);
    } else if (methodName == ROUTE_ENABLE_ALERT_BEFORE_BACK_PAGE) {
        EnableAlertBeforeBackPage(args, index);
    } else if (methodName == ROUTE_DISABLE_ALERT_BEFORE_BACK_PAGE) {
        DisableAlertBeforeBackPage(args, index);
    } else if (methodName == ROUTE_POSTPONE) {
        PostponePageTransition(args, index);
    } else if (methodName == ROUTE_LAUNCH) {
        LaunchPageTransition(args, index);
    } else {
        LOGW("system.router not support method = %{private}s", methodName.c_str());
    }
}

std::vector<ButtonInfo> JsParseDialogButtons(
    const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& key, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter ParseDialogButtons");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    std::vector<ButtonInfo> dialogButtons;
    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return dialogButtons;
    }
    std::string argsStr(*argsJsStr);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsArray()) {
        for (int32_t i = 0; i < argsPtr->GetValue(key)->GetArraySize(); i++) {
            auto button = argsPtr->GetValue(key)->GetArrayItem(i);
            if (!button->GetValue("text")->IsString()) {
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

void JsShowToast(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsShowToast");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return;
    }
    std::string argsStr(*argsJsStr);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);
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

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->ShowToast(message, duration, bottom);
}

void JsShowDialog(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsShowDialog");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    const std::string title = JsParseRouteUrl(args, PROMPT_KEY_TITLE, index);
    const std::string message = JsParseRouteUrl(args, PROMPT_KEY_MESSAGE, index);
    std::vector<ButtonInfo> buttons = JsParseDialogButtons(args, PROMPT_KEY_BUTTONS, index);
    const std::string success = JsParseRouteUrl(args, COMMON_SUCCESS, index);
    const std::string cancel = JsParseRouteUrl(args, COMMON_CANCEL, index);
    const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
    bool autoCancel = true;

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        return;
    }
    std::string argsStr(*argsJsStr);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);
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

    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());

    auto callback = [engineInstance, success, cancel, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                engineInstance->CallJs(success.c_str(),
                    std::string("{\"index\":").append(std::to_string(successType)).append("}").c_str(), false);
                break;
            case 1:
                engineInstance->CallJs(cancel.c_str(), std::string("\"cancel\",null").c_str(), false);
                break;
            case 2:
                engineInstance->CallJs(complete.c_str(), std::string("\"complete\",null").c_str(), false);
                break;
            default:
                break;
        }
    };

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->ShowDialog(title, message, buttons, autoCancel, std::move(callback), callbacks);
}

void JsShowActionMenu(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsShowActionMenu");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());

    v8::String::Utf8Value argsJsStr(isolate, args[index]);
    if (!(*argsJsStr)) {
        LOGE("JsShowActionMenu args is null");
        return;
    }
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
    if (argsPtr == nullptr) {
        LOGE("argsPtr is nullptr");
        return;
    }

    std::vector<ButtonInfo> buttons = JsParseDialogButtons(args, PROMPT_KEY_BUTTONS, index);
    if (buttons.empty() || buttons.size() > 6) { // The number of buttons cannot be zero or more than six
        LOGE("buttons is invalid");
        if (argsPtr->IsObject()) {
            const std::string fail = JsParseRouteUrl(args, COMMON_FAIL, index);
            const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
            engineInstance->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})", false);
            engineInstance->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})", false);
        } else {
            std::string callBackStr(*argsJsStr);
            // Get callbackId and clear redundant symbols
            if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
                callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
                engineInstance->CallJs(callBackStr,
                    R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:buttons is invalid"}],"method":"fail"})",
                    false);
            }
        }
        return;
    }

    const std::string title = JsParseRouteUrl(args, PROMPT_KEY_TITLE, index);
    const std::string success = JsParseRouteUrl(args, COMMON_SUCCESS, index);
    const std::string fail = JsParseRouteUrl(args, COMMON_FAIL, index);
    const std::string complete = JsParseRouteUrl(args, COMMON_COMPLETE, index);
    auto callback = [engineInstance, success, fail, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                engineInstance->CallJs(success,
                    std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                        .append(std::to_string(successType))
                        .append("}")
                        .c_str(),
                    false);
                engineInstance->CallJs(complete,
                    std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                        .append(std::to_string(successType))
                        .append("}")
                        .c_str(),
                    false);
                break;
            case 1:
                engineInstance->CallJs(fail, R"({"errMsg":"showActionMenu:fail cancel"})", false);
                engineInstance->CallJs(complete, R"({"errMsg\":\"showActionMenu:fail cancel"})", false);
                break;
            default:
                LOGE("callbackType is invalid");
                break;
        }
    };

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->ShowActionMenu(title, buttons, std::move(callback));
}

void JsHandlePrompt(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsHandlePrompt");
    if (methodName == PROMPT_SHOW_TOAST) {
        JsShowToast(args, index);
    } else if (methodName == PROMPT_SHOW_DIALOG) {
        JsShowDialog(args, index);
    } else if (methodName == PROMPT_SHOW_ACTION_MENU) {
        JsShowActionMenu(args, index);
    } else {
        LOGW("system.prompt not support method = %{private}s", methodName.c_str());
    }
}

void JsParseCallbackResult(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value> jsResult,
    const std::string& key, std::string& result)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value argsJsStr(isolate, jsResult);
    if (!(*argsJsStr)) {
        return;
    }
    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argsStr);
    if (argsPtr && argsPtr->GetValue(key) != nullptr) {
        if (argsPtr->GetValue(key)->IsString()) {
            result = argsPtr->GetValue(key)->GetString();
        } else if (argsPtr->GetValue(key)->IsNumber()) {
            result = argsPtr->GetValue(key)->ToString();
        }
    }
}

void JsHandleCallback(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsHandleCallback");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("internal js result, context is empty!");
        return;
    }

    if (methodName == CALLBACK_NATIVE) {
        if (args[index]->IsObject() && args[index]->IsArray()) {
            v8::Local<v8::Object> obj = args[index]->ToObject(context).ToLocalChecked();
            v8::Local<v8::Array> properties = obj->GetOwnPropertyNames(context).ToLocalChecked();
            if (properties->Length() < PROPERTIES_ARGS_LEN) {
                LOGW("JsHandleCallback: invalid callback value");
                return;
            }
            v8::Local<v8::Value> jsCallBackIdKey = properties->Get(context, PROPERTIES_ARGS_CBID_IDX).ToLocalChecked();
            v8::Local<v8::Value> jsCallBackId = obj->Get(context, jsCallBackIdKey).ToLocalChecked();
            v8::Local<v8::Value> jsResultKey = properties->Get(context, PROPERTIES_ARGS_RST_IDX).ToLocalChecked();
            v8::Local<v8::Value> jsResult = obj->Get(context, jsResultKey).ToLocalChecked();

            v8::String::Utf8Value callBackIdJsStr(isolate, jsCallBackId);
            v8::String::Utf8Value resultJsStr(isolate, jsResult);
            if (!(*callBackIdJsStr) || !(*resultJsStr)) {
                return;
            }
            std::string callBackId(*callBackIdJsStr);
            std::string result(*resultJsStr);

            JsParseCallbackResult(args, jsResult, KEY_STEPPER_PENDING_INDEX, result);
            auto delegate =
                static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
            (*delegate)->SetCallBackResult(callBackId, result);

        } else if (methodName == APP_DESTROY_FINISH) {
            LOGD("JsHandleCallback: appDestroyFinish should release resource here");
        } else {
            LOGW("internal.jsResult not support method = %{private}s", methodName.c_str());
        }
    }
}

void JsHandleAnimationFrame(
    const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("animation frame, context is empty!");
        return;
    }
    v8::String::Utf8Value callbackIdJsStr(isolate, args[index]);
    if (!(*callbackIdJsStr)) {
        LOGW("system animation callbackId is null");
        return;
    }
    if (methodName == ANIMATION_REQUEST_ANIMATION_FRAME) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->RequestAnimationFrame(std::string(*callbackIdJsStr));
    } else if (methodName == ANIMATION_CANCEL_ANIMATION_FRAME) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->CancelAnimationFrame(std::string(*callbackIdJsStr));
    } else {
        LOGW("animationFrame not support method = %{private}s", methodName.c_str());
    }
}

void JsHandleAnimator(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value jsArguments(isolate, args[index]);
    if (!(*jsArguments)) {
        LOGE("args is invalid");
        return;
    }
    std::string arguments(*jsArguments);
    auto page = GetStagingPage();

    if (page == nullptr) {
        LOGE("GetStagingPage is nullptr");
        return;
    }
    auto context = isolate->GetCurrentContext();
    if (methodName == ANIMATOR_CREATE_ANIMATOR) {
        int32_t bridgeId = V8AnimatorBridgeUtils::JsCreateBridgeId();
        auto resultAnimator = V8AnimatorBridgeUtils::CreateAnimatorContext(context, page->GetPageId(), bridgeId);
        auto animatorBridge = AceType::MakeRefPtr<V8AnimatorBridge>(context, isolate, resultAnimator);
        auto task = AceType::MakeRefPtr<V8AnimatorTaskCreate>(isolate, animatorBridge, arguments);
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
        args.GetReturnValue().Set(resultAnimator);
    }
}

void JsHandleImage(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    LOGD("JsHandleImage");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    auto src = JsParseRouteUrl(args, "src", index);
    auto success = JsParseRouteUrl(args, "success", index);
    auto fail = JsParseRouteUrl(args, "fail", index);

    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());

    auto&& callback = [engineInstance, success, fail](bool callbackType, int32_t width, int32_t height) {
        if (callbackType) {
            engineInstance->CallJs(success,
                std::string("{\"width\":")
                    .append(std::to_string(width))
                    .append(", \"height\":")
                    .append(std::to_string(height))
                    .append("}"),
                false);
        } else {
            engineInstance->CallJs(fail, std::string("\"fail\",null"), false);
        }
    };
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->HandleImage(src, std::move(callback));
}

bool ParseResourceStringParam(std::string& str, const char* paramName, const std::map<std::string, std::string>& params)
{
    std::string strName(paramName);
    auto iter = params.find(strName);
    if (iter != params.end()) {
        str = iter->second;
        return true;
    }
    return false;
}

bool ParseResourceNumberParam(int32_t& num, const char* paramName, const std::map<std::string, std::string>& params)
{
    std::string numName(paramName);
    auto iter = params.find(numName);
    if (iter != params.end()) {
        num = ParseResourceInputNumberParam(iter->second);
        return true;
    }
    return false;
}

void ParseResourceParam(
    std::string& uri, int32_t& position, int32_t& length, const std::map<std::string, std::string>& params)
{
    ParseResourceStringParam(uri, READ_KEY_URI, params);
    ParseResourceNumberParam(position, READ_KEY_POSITION, params);
    ParseResourceNumberParam(length, READ_KEY_LENGTH, params);
}

void JsReadArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsReadArrayBuffer, context is empty!");
        return;
    }

    std::map<std::string, std::string> params;
    std::string callbackId;
    if (!GetParamsWithCallbackId(context, v8::Local<v8::Array>::Cast(args[index]), params, callbackId)) {
        LOGE("JsReadArrayBuffer, get params or callbackId failed!");
        return;
    }
    std::string uri;
    int32_t position = 1;
    int32_t length = MAX_READ_TEXT_LENGTH;
    ParseResourceParam(uri, position, length, params);
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        LOGE("JsReadArrayBuffer uri regex match failed");
        V8EngineInstance::CallJs(
            isolate, pContext, callbackId, R"({"arguments":["file uri pattern not correct", 200],"method":"fail"})");
        args.GetReturnValue().SetNull();
        return;
    }

    std::vector<uint8_t> binaryContent;
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (!((*delegate)->GetResourceData(uri, binaryContent))) {
        LOGE("JsReadArrayBuffer get buffer data failed");
        V8EngineInstance::CallJs(
            isolate, pContext, callbackId, R"({"arguments":["get buffer data failed", 301],"method":"fail"})");
        args.GetReturnValue().SetNull();
        return;
    }

    auto fileLength = static_cast<int32_t>(binaryContent.size());
    if (position > fileLength || position <= 0 || length <= 0) {
        LOGE("JsReadText position fileLength failed");
        V8EngineInstance::CallJs(isolate, pContext, callbackId,
            R"({"arguments":["wrong start position or wrong read length", 301],"method":"fail"})");
        args.GetReturnValue().SetNull();
        return;
    }

    length = position + length - 1 > fileLength ? fileLength - position + 1 : length;
    auto binaryData = v8::Object::New(isolate);
    std::unique_ptr<v8::BackingStore> backStore = v8::ArrayBuffer::NewBackingStore(
        &binaryContent[position - 1], length, [](void*, size_t, void*) {}, nullptr);
    v8::Local<v8::ArrayBuffer> binaryArray = v8::ArrayBuffer::New(isolate, std::move(backStore));
    binaryData->Set(context, v8::String::NewFromUtf8(isolate, "buffer").ToLocalChecked(), binaryArray).ToChecked();
    args.GetReturnValue().Set(binaryData);
    V8EngineInstance::CallJs(
        isolate, pContext, callbackId, R"({"arguments":["read array buffer success"],"method":"success"})");
    pContext.Reset();
}

void JsReadText(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsReadText, context is empty!");
        return;
    }

    std::map<std::string, std::string> params;
    std::string callbackId;
    if (!GetParamsWithCallbackId(context, v8::Local<v8::Array>::Cast(args[index]), params, callbackId)) {
        LOGE("JsReadText, get params or callbackId failed!");
        return;
    }
    std::string uri;
    ParseResourceStringParam(uri, READ_KEY_URI, params);
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        LOGE("JsReadText uri regex match failed");
        V8EngineInstance::CallJs(
            isolate, pContext, callbackId, R"({"arguments":["file uri pattern not correct", 202],"method":"fail"})");
        return;
    }

    std::string fileText;
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (!((*delegate)->GetResourceData(uri, fileText))) {
        LOGE("JsReadText get text data failed");
        V8EngineInstance::CallJs(
            isolate, pContext, callbackId, R"({"arguments":["get text data failed", 301],"method":"fail"})");
        return;
    }

    auto fileLength = ParseUtf8TextLength(fileText);
    int32_t position = 0;
    int32_t length = 0;
    if (!ParseResourceNumberParam(position, READ_KEY_POSITION, params)) {
        position = 1;
    }

    if (!ParseResourceNumberParam(length, READ_KEY_LENGTH, params) || (length > fileLength - position + 1)) {
        length = (fileLength - position + 1 <= 0) ? 0 : fileLength - position + 1;
    }

    if (fileLength == 0) {
        if ((position <= 0) || (length < 0)) {
            V8EngineInstance::CallJs(isolate, pContext, callbackId,
                R"({"arguments":["wrong start position or wrong read length", 202],"method":"fail"})");
            return;
        }
    } else {
        if ((position > fileLength) || (position <= 0) || (length < 0)) {
            V8EngineInstance::CallJs(isolate, pContext, callbackId,
                R"({"arguments":["wrong start position or wrong read length", 202],"method":"fail"})");
            return;
        }

        auto substrPos = ParseUtf8TextSubstrStartPos(fileText, position);
        auto substrEndPos = ParseUtf8TextSubstrEndPos(fileText, position + length - 1);
        fileText = fileText.substr(substrPos - 1, substrEndPos - substrPos + 1);
        HandleEscapeCharaterInUtf8TextForJson(fileText);
    }

    V8EngineInstance::CallJs(isolate, pContext, callbackId,
        std::string("{\"arguments\":[").append("{\"text\":\"").append(fileText).append("\"}],\"method\":\"success\"}"),
        false);
    pContext.Reset();
}

void JsHandleReadResource(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    LOGD("JsHandleReadResource");
    if (methodName == READ_TEXT) {
        JsReadText(args, index);
    } else if (methodName == READ_ARRAY_BUFFER) {
        JsReadArrayBuffer(args, index);
    } else {
        LOGW("system.resource not support method = %{private}s", methodName.c_str());
    }
}

void JsHandleOffscreenCanvas(
    const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    auto page = GetRunningPage();
    if (page == nullptr) {
        LOGE("GetRunningPage is nullptr");
        return;
    }

    if (methodName == OFFSCREEN_CANVAS_CREATE) {
        int32_t width = JsParseIntParams(args, "width", index);
        int32_t height = JsParseIntParams(args, "height", index);

        v8::Isolate* isolate = args.GetIsolate();
        ACE_DCHECK(isolate);
        v8::HandleScope handleScope(isolate);

        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto pipelineContext = (*delegate)->GetPipelineContext();
        auto bridge = AceType::MakeRefPtr<V8OffscreenCanvasBridge>(pipelineContext, width, height);
        page->PushOffscreenCanvasBridge(bridge->GetBridgeId(), bridge);
        bridge->GetBridge(args);
    }
}

void JsCallConfiguration(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    LOGD("Enter JsCallConfiguration");
    if (methodName == CONFIGURATION_GET_LOCALE) {
        GetLocale(args);
    } else if (methodName == CONFIGURATION_SET_LOCALE) {
        JsSetLocale(args, index);
    }
}

void JsHandleSetTimeout(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    LOGD("Enter JsHandleSetTimeout");
    if (methodName == SET_TIMEOUT) {
        JsSetTimer(args, index, false);
    } else if (methodName == CLEAR_TIMEOUT || methodName == CLEAR_INTERVAL) {
        JsClearTimeout(args, index);
    } else if (methodName == SET_INTERVAL) {
        JsSetTimer(args, index, true);
    } else {
        LOGW("unsupported method for timer module!");
    }
}

void JsGetDeviceInfo(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    if (methodName != "getInfo") {
        LOGW("system.device module not support method:%{public}s!", methodName.c_str());
        return;
    }

    LOGD("Enter JsGetDeviceInfo");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("get device info, context is empty!");
        return;
    }

    v8::String::Utf8Value callBackIdJsStr(isolate, args[index]);
    const char* callBackId = *callBackIdJsStr;
    if (callBackId == nullptr) {
        LOGE("device.system device getInfo callBackID is null");
    } else {
        LOGD("system device getInfo callBackID = %{private}s", callBackId);
        std::pair<std::string, bool> ret = PluginBridge::GetDeviceInfo();
        v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
        pContext.Reset(isolate, context);
        if (ret.second) {
            V8EngineInstance::CallJs(isolate, pContext, callBackId,
                std::string("{\"arguments\":[").append(ret.first).append("],\"method\":\"success\"}"), false);
        } else {
            V8EngineInstance::CallJs(isolate, pContext, callBackId,
                std::string("{\"arguments\":[").append(ret.first).append(",200],\"method\":\"fail\"}"), false);
        }
        pContext.Reset();
    }
}

void JsHandleGridLayout(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("grid api, context is empty!");
        return;
    }
    if (methodName == GRID_GET_SYSTEM_LAYOUT_INFO) {
        v8::Local<v8::String> gridInfo =
            v8::String::NewFromUtf8(isolate, GridSystemManager::GetInstance().GetCurrentGridInfo().ToString().c_str())
                .ToLocalChecked();
        args.GetReturnValue().Set(v8::JSON::Parse(context, gridInfo).ToLocalChecked());
    } else {
        LOGW("grid not support method = %{public}s", methodName.c_str());
    }
}

void JsHandleMediaQuery(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName, int32_t index)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("media query, context is empty!");
        return;
    }
    if (methodName == ADD_LISTENER) {
        JsAddListener(context, args[index]);
    } else if (methodName == GET_DEVICE_TYPE) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto mediaQuery = (*delegate)->GetMediaQueryInfoInstance();
        if (mediaQuery) {
            v8::Local<v8::String> res =
                v8::String::NewFromUtf8(isolate, mediaQuery->GetDeviceType().c_str()).ToLocalChecked();
            args.GetReturnValue().Set(res);
        }
    } else {
        LOGW("system.mediaquery not support method = %{public}s", methodName.c_str());
    }
}

// function callback for aceObj's function: domCreateBody
void JsDomCreateBody(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsDomCreateBody");
    if (args.Length() != CREATE_BODY_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 5 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("dom create body, context is empty!");
        return;
    }

    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    const int32_t pageId = page->GetPageId();
    int32_t nodeId = DOM_ROOT_NODE_ID_BASE + pageId;
    LOGD("JsDomCreateBody: pageId: %{private}d, nodeId: %{private}d:", pageId, nodeId);
    v8::String::Utf8Value tagJsStr(isolate, args[CREATE_BODY_TAG_IDX]);
    if (!(*tagJsStr)) {
        return;
    }
    std::string tag(*tagJsStr);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomBody>(tag.c_str(), nodeId);
    SetDomAttributes(context, args[CREATE_BODY_ATTR_IDX], *command);
    SetDomStyle(context, args[CREATE_BODY_STYLE_IDX], *command);
    AddDomEvent(context, args[CREATE_BODY_EVENT_IDX], *command);
    page->PushCommand(command);
}

// function callback for aceObj's function: domAddElement
void JsDomAddElement(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsDomAddElement");
    if (args.Length() != ADD_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 9 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("Dom Add Element, context is empty");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    const int32_t instanceId = args[ADD_ELEMENT_INSTANCE_ID]->Int32Value(context).ToChecked();
    if (page->GetPageId() != instanceId) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        page = (*delegate)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsDomAddElement fail to get page, pageId: %{public}d", instanceId);
            return;
        }
    }

    const int32_t pageId = page->GetPageId();
    int32_t parentNodeId = args[ADD_ELEMENT_PARID_IDX]->Int32Value(context).ToChecked();
    parentNodeId = parentNodeId == 0 ? (DOM_ROOT_NODE_ID_BASE + pageId) : parentNodeId;

    int32_t nodeId = args[ADD_ELEMENT_NODEID_IDX]->Int32Value(context).ToChecked();
    v8::String::Utf8Value tagJsStr(isolate, args[ADD_ELEMENT_TAG_IDX]);
    if (!(*tagJsStr)) {
        return;
    }
    std::string tag(*tagJsStr);
    LOGD("JsDomAddElement: pageId: %{private}d, parentNodeId: %{private}d, nodeId: %{private}d, tag: %{private}s",
        pageId, parentNodeId, nodeId, tag.c_str());
    auto command = Referenced::MakeRefPtr<JsCommandAddDomElement>(tag.c_str(), nodeId, parentNodeId);
    SetDomAttributes(context, args[ADD_ELEMENT_ATTR_IDX], *command);
    SetDomStyle(context, args[ADD_ELEMENT_STYLE_IDX], *command);
    AddDomEvent(context, args[ADD_ELEMENT_EVENT_INDEX], *command);
    int32_t itemIndex = args[ADD_ELEMENT_ITEM_INDEX]->Int32Value(context).ToChecked();
    if (args[ADD_ELEMENT_CUSTOM_INDEX]->IsBoolean()) {
        bool isCustomComponent = args[ADD_ELEMENT_CUSTOM_INDEX]->ToBoolean(isolate)->Value();
        command->SetIsCustomComponent(isCustomComponent);
    }
    command->SetForIndex(itemIndex);
    page->PushCommand(command);
    // Flush command as fragment immediately when pushed too many commands.
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
}

// function callback for aceObj's function: updateElementAttrs
void JsUpdateElementAttrs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsUpdateElementsAttrs");

    if (args.Length() != UPLOAD_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 3 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("Upload Element Attrs, context is empty");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    const int32_t instanceId = args[UPLOAD_ELEMENT_INSTANCE_ID]->Int32Value(context).ToChecked();
    if (page->GetPageId() != instanceId) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        page = (*delegate)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsUpdateElementAttrs fail to get page, pageId :%{public}d", instanceId);
            return;
        }
    }

    int32_t nodeId = args[UPLOAD_ELEMENT_NID_IDX]->Int32Value(context).ToChecked();
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(context, args[UPLOAD_ELEMENT_DOM_IDX], *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
}

// function callback for aceObj's function: updateElementStyles
void JsUpdateElementStyles(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsUpdateElementStyles");

    if (args.Length() != UPLOAD_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 3 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("Upload element styles, context is empty");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    const int32_t instanceId = args[UPLOAD_ELEMENT_INSTANCE_ID]->Int32Value(context).ToChecked();
    if (page->GetPageId() != instanceId) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        page = (*delegate)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsUpdateElementStyles fail to get page, pageId :%{public}d", instanceId);
            return;
        }
    }

    int32_t nodeId = args[UPLOAD_ELEMENT_NID_IDX]->Int32Value(context).ToChecked();
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(context, args[UPLOAD_ELEMENT_DOM_IDX], *command);
    page->PushCommand(command);
}

// function callback for aceObj's function: onCreateFinish
void JsOnCreateFinish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsOnCreateFinish");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    page->SetPageCreated();
}

v8::Local<v8::String> CharToV8String(v8::Isolate* isolate, const char* str, bool isInternalized = false)
{
    if (str != nullptr && strlen(str) != 0) {
        v8::NewStringType type = isInternalized ? v8::NewStringType::kInternalized : v8::NewStringType::kNormal;
        v8::MaybeLocal<v8::String> ret = v8::String::NewFromUtf8(isolate, str, type);
        if (!ret.IsEmpty()) {
            return ret.ToLocalChecked();
        }
    }
    return v8::String::Empty(isolate);
}

void JsCompileAndRunBundle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    if (g_debugger != nullptr) {
        WaitingForIde waitingForIde = (WaitingForIde)dlsym(g_debugger, "WaitingForIde");
        if (waitingForIde != nullptr) {
            waitingForIde();
        }
    }

    LOGI("JsCompileAndRunBundle");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    int len = args.Length();
    if (len <= 0 || !args[0]->IsString()) {
        LOGE("len %d", len);
        args.GetReturnValue().SetUndefined();
        return;
    }
    v8::String::Utf8Value tagJsStr(isolate, args[0]);
    if (!(*tagJsStr)) {
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::string tag(*tagJsStr);
    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate, tag.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    if (source.IsEmpty()) {
        LOGE("source.IsEmpty");
        args.GetReturnValue().SetUndefined();
        return;
    }
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope ctxScope(v8::Local<v8::Context>::New(isolate, isolate->GetCurrentContext()));
    v8::TryCatch tryCatch(isolate);

    v8::String::Utf8Value pageStr(isolate, args[1]);
    if (!(*pageStr)) {
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::string page(*pageStr);

    v8::Local<v8::String> name = CharToV8String(isolate, page.c_str(), true);
    v8::ScriptOrigin origin(name);
    v8::MaybeLocal<v8::Script> maybeScript = v8::Script::Compile(isolate->GetCurrentContext(), source, &origin);
    if (maybeScript.IsEmpty()) {
        LOGE("Compile maybeScript.IsEmpty");
        return;
    }
    v8::Local<v8::Script> script = maybeScript.ToLocalChecked();
    v8::MaybeLocal<v8::Value> maybeResult = script->Run(isolate->GetCurrentContext());
    if (maybeResult.IsEmpty()) {
        LOGE("Run maybeResult.IsEmpty");
        return;
    }
    v8::Local<v8::Value> result = maybeResult.ToLocalChecked();
    args.GetReturnValue().Set(result);
}

// function callback for aceObj's function: onUpdateFinish
void JsOnUpdateFinish(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsOnUpdateFinish");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    if (page->CheckPageCreated()) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->TriggerPageUpdate(page->GetPageId());
    }
}

void JsHandleModule(std::string moduleName, std::string methodName, const v8::FunctionCallbackInfo<v8::Value>& args)
{
    static const std::unordered_map<std::string,
        void (*)(const v8::FunctionCallbackInfo<v8::Value>&, const std::string&)>
        jsHandleMap = { { "system.app", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                            const std::string& methodName) { JsHandleAppApi(args, methodName, 1); } },
            { "system.router", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                   const std::string& methodName) { JsHandlePageRoute(args, methodName, 1); } },
            { "system.prompt", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                   const std::string& methodName) { JsHandlePrompt(args, methodName, 1); } },
            { "internal.jsResult", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                       const std::string& methodName) { JsHandleCallback(args, methodName, 1); } },
            { "system.configuration",
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName) {
                    JsCallConfiguration(args, methodName, 1);
                } },
            { "system.image", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                  const std::string& methodName) { JsHandleImage(args, 1); } },
            { "timer", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                           const std::string& methodName) { JsHandleSetTimeout(args, methodName, 1); } },
            { "system.device", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                   const std::string& methodName) { JsGetDeviceInfo(args, methodName, 1); } },
            { "system.grid", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                 const std::string& methodName) { JsHandleGridLayout(args, methodName); } },
            { "system.mediaquery", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                       const std::string& methodName) { JsHandleMediaQuery(args, methodName, 1); } },
            { "animation", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                               const std::string& methodName) { JsHandleAnimationFrame(args, methodName, 1); } },
            { "system.resource", [](const v8::FunctionCallbackInfo<v8::Value>& args,
                                     const std::string& methodName) { JsHandleReadResource(args, methodName, 1); } },
            { "system.offscreenCanvas",
                [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName) {
                    JsHandleOffscreenCanvas(args, methodName, 1);
                } },
            { "ohos.animator", [](const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& methodName) {
                 JsHandleAnimator(args, methodName, 1);
             } } };

    auto jsHandleIter = jsHandleMap.find(moduleName);
    if (jsHandleIter != jsHandleMap.end()) {
        jsHandleIter->second(args, methodName);
        return;
    }

    return;
}

// function callback for aceObj's function: removeElement
void JsRemoveElement(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsRemoveElement");
    if (args.Length() != REMOVE_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 2 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("Remove element, context is empty");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }

    const int32_t instanceId = args[REMOVE_ELEMENT_INSTANCE_ID]->Int32Value(context).ToChecked();
    if (page->GetPageId() != instanceId) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        page = (*delegate)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsRemoveElement fail to get page, pageId: %{public}d", instanceId);
            return;
        }
    }

    int32_t nodeId = args[REMOVE_ELEMENT_ID_IDX]->Int32Value(context).ToChecked();
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    page->PushCommand(Referenced::MakeRefPtr<JsCommandRemoveDomElement>(nodeId));
}

// function callback for aceObj's function: callNative
void JsCallNative(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsCallNative");
    if (args.Length() != NATIVE_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have 2 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    v8::String::Utf8Value moduleAndMethodJsStr(isolate, args[NATIVE_ARGS_METHOD_IDX]);
    v8::String::Utf8Value argumentsJsStr(isolate, args[NATIVE_ARGS_IDX]);
    if (!(*moduleAndMethodJsStr) || !(*argumentsJsStr)) {
        return;
    }
    std::string moduleAndMethod(*moduleAndMethodJsStr);
    std::string arguments(*argumentsJsStr);
    LOGD(
        "JsCallNative moduleAndMethod = %{private}s arguments=%{private}s", moduleAndMethod.c_str(), arguments.c_str());

    std::unique_ptr<JsonValue> moduleAndMethodPtr = JsonUtil::ParseJsonString(moduleAndMethod);
    if (moduleAndMethodPtr == nullptr) {
        LOGE("Get moduleAndMethod from argv failed");
        return;
    }

    std::unique_ptr<JsonValue> modulePtr = moduleAndMethodPtr->GetValue("module");
    if (modulePtr == nullptr) {
        LOGE("Get module from moduleAndMethodPtr failed");
        return;
    }

    std::unique_ptr<JsonValue> methodPtr = moduleAndMethodPtr->GetValue("method");
    if (methodPtr == nullptr) {
        LOGE("Get method from moduleAndMethodPtr failed");
        return;
    }

    const std::string moduleName = modulePtr->GetString();
    const std::string methodName = methodPtr->GetString();

    JsHandleModule(moduleName, methodName, args);
}

// function callback for aceObj's function: callComponent
void JsCallComponent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsCallComponent");
    if (args.Length() != COMPONENT_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("Call Component, context is empty");
        return;
    }
    auto page = GetRunningPage();
    if (page == nullptr) {
        LOGE("GetRunningPage is nullptr");
        return;
    }
    int32_t nodeId = args[COMPONENT_ARGS_ID_IDX]->Int32Value(context).ToChecked();
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    v8::String::Utf8Value jsMethodName(isolate, args[COMPONENT_ARGS_METHOD_IDX]);
    v8::String::Utf8Value jsArguments(isolate, args[COMPONENT_ARGS_IDX]);
    if (!(*jsMethodName) || !(*jsArguments)) {
        return;
    }
    std::string methodName(*jsMethodName);
    std::string arguments(*jsArguments);
    LOGD("nodeId = %{private}d, methodName = %{private}s, args = %{private}s", nodeId, methodName.c_str(),
        arguments.c_str());
    auto engine = reinterpret_cast<V8EngineInstance*>(args.Data().As<v8::External>()->Value());
    if (std::strcmp(methodName.c_str(), "getContext") == 0) {
        auto bridge = AceType::DynamicCast<V8CanvasBridge>(page->GetBridgeById(nodeId));
        if (bridge) {
            bridge->HandleContext(context, nodeId, arguments, engine);
            args.GetReturnValue().Set(bridge->GetRenderContext());
            return;
        }
    } else if (std::strcmp(methodName.c_str(), "getXComponentContext") == 0) {
        auto bridge = AceType::DynamicCast<V8XComponentBridge>(page->GetXComponentBridgeById(nodeId));
        if (bridge) {
            bridge->HandleContext(context, nodeId, arguments, engine);
            args.GetReturnValue().Set(bridge->GetRenderContext());
            return;
        }
    } else if (std::strcmp(methodName.c_str(), "toDataURL") == 0) {
        auto bridge = AceType::DynamicCast<V8CanvasBridge>(page->GetBridgeById(nodeId));
        if (bridge) {
            bridge->HandleToDataURL(context, nodeId, arguments);
            args.GetReturnValue().Set(bridge->GetDataURL());
            return;
        }
    } else if (std::strcmp(methodName.c_str(), "getBoundingClientRect") == 0) {
        auto rect = V8ComponentApiBridge::JsGetBoundingRect(isolate, nodeId);
        args.GetReturnValue().Set(rect);
    } else if (std::strcmp(methodName.c_str(), "getScrollOffset") == 0) {
        // handle getScrollOffset method of scroll view, like div, stack and list.
        auto offset = V8ComponentApiBridge::JsGetScrollOffset(isolate, nodeId);
        args.GetReturnValue().Set(offset);
    } else if (std::strcmp(methodName.c_str(), "scrollTo") == 0) {
        V8ComponentApiBridge::JsScrollTo(args, arguments, nodeId);
    }

    if (std::strcmp(methodName.c_str(), "animate") == 0) {
        LOGD("animate args = %{private}s", arguments.c_str());
        auto resultValue = V8AnimationBridgeUtils::CreateAnimationContext(context, page->GetPageId(), nodeId);
        auto animationBridge = AceType::MakeRefPtr<V8AnimationBridge>(context, isolate, resultValue, nodeId);
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        auto task = AceType::MakeRefPtr<V8AnimationBridgeTaskCreate>(*delegate, animationBridge, arguments);
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
        args.GetReturnValue().Set(resultValue);
    } else if (std::strcmp(methodName.c_str(), "currentOffset") == 0) {
        // handle list currentOffset method
        auto offset = V8ListBridge::JsGetCurrentOffset(isolate, nodeId);
        args.GetReturnValue().Set(offset);
    } else if (std::strcmp(methodName.c_str(), "createIntersectionObserver") == 0) {
        auto visibleObserver = V8ComponentApiBridge::JsAddVisibleListener(args, arguments, nodeId);
        args.GetReturnValue().Set(visibleObserver);
    } else if (std::strcmp(methodName.c_str(), "getState") == 0) {
        // handle image-animator getState method
        auto state = V8ImageAnimatorBridge::JsGetState(isolate, nodeId);
        args.GetReturnValue().Set(state);
    } else if (std::strcmp(methodName.c_str(), "addChild") == 0) {
        auto sPage = GetStagingPage();
        if (sPage == nullptr) {
            LOGE("GetStagingPage return nullptr");
            return;
        }
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(arguments);
        if (!argsValue || !argsValue->IsArray()) {
            LOGE("argsValue is nullptr or not array");
            return;
        }
        std::unique_ptr<JsonValue> indexValue = argsValue->GetArrayItem(0)->GetValue("__nodeId");
        if (!indexValue || !indexValue->IsNumber()) {
            LOGE("indexValue is nullptr or not number");
            return;
        }
        int32_t childId = indexValue->GetInt();
        auto domDocument = sPage->GetDomDocument();
        if (domDocument) {
            RefPtr<DOMNode> node = domDocument->GetDOMNodeById(childId);
            if (node == nullptr) {
                LOGE("node is nullptr");
                return;
            }
            auto command = Referenced::MakeRefPtr<JsCommandAppendElement>(node->GetTag(), node->GetNodeId(), nodeId);
            sPage->PushCommand(command);
            if (!sPage->CheckPageCreated() && sPage->GetCommandSize() > FRAGMENT_SIZE) {
                sPage->FlushCommands();
            }
        }
    } else {
        page->PushCommand(Referenced::MakeRefPtr<JsCommandCallDomElementMethod>(nodeId, methodName, arguments));
    }
    // focus method should be delayed util show attribute update.
    if (page->CheckPageCreated() && strcmp(DOM_FOCUS, methodName.c_str()) != 0) {
        auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->TriggerPageUpdate(page->GetPageId(), true);
    }
}

std::string ParseLogContent(const std::vector<std::string>& params)
{
    std::string ret;
    if (params.empty()) {
        return ret;
    }
    std::string formatStr = params[0];
    int32_t size = params.size();
    int32_t len = formatStr.size();
    int32_t pos = 0;
    int32_t count = 1;
    for (; pos < len; ++pos) {
        if (count >= size) {
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
                    ret += params[count++];
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

void GetLogLevelAndContent(const v8::FunctionCallbackInfo<v8::Value>& args, JsLogLevel& logLevel, std::string& content)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty() || args.Length() == 0) {
        LOGE("Get LogLevel and Content, context is empty");
        return;
    }
    if (args.Length() == 1) {
        v8::Local<v8::Value> str;
        if (args[0]->ToString(context).ToLocal(&str)) {
            v8::String::Utf8Value utf8(isolate, str);
            if (*utf8) {
                content += *utf8;
            }
        } else {
            content += "Error converting argument 1";
        }
    } else {
        std::vector<std::string> params;
        for (int32_t i = 0; i < args.Length(); i++) {
            v8::Local<v8::Value> str;
            if (args[i]->ToString(context).ToLocal(&str)) {
                v8::String::Utf8Value utf8(isolate, str);
                if (*utf8) {
                    params.emplace_back(*utf8);
                }
            }
        }
        content = ParseLogContent(params);
    }
    logLevel = static_cast<JsLogLevel>(args.Data()->Int32Value(context).ToChecked());
}

void AppLogPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    JsLogLevel logLevel = JsLogLevel::INFO;
    std::string fullString;
    GetLogLevelAndContent(args, logLevel, fullString);
    switch (logLevel) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", fullString.c_str());
            break;
        default:
            LOGE("the log level is error");
    }
}

void JsLogPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    JsLogLevel logLevel = JsLogLevel::INFO;
    std::string fullString;
    GetLogLevelAndContent(args, logLevel, fullString);
    switch (logLevel) {
        case JsLogLevel::DEBUG:
            LOGD("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::INFO:
            LOGI("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::WARNING:
            LOGW("ace Log: %{public}s", fullString.c_str());
            break;
        case JsLogLevel::ERROR:
            LOGE("ace Log: %{public}s", fullString.c_str());
            break;
        default:
            break;
    }
}

void JsPerfEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsPerfEnd");
    if (args.Length() != PERF_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfEnd, CurrentContext is empty!");
        return;
    }

    v8::String::Utf8Value jsMethodName(isolate, args[PERF_ARGS_METHOD_IDX]);
    if (!(*jsMethodName)) {
        return;
    }
    std::string methodName(*jsMethodName);

    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsEndLog(methodName, currentTime);
}

void JsPerfBegin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsPerfEnd");
    if (args.Length() != PERF_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfBegin, CurrentContext is empty!");
        return;
    }

    v8::String::Utf8Value jsMethodName(isolate, args[PERF_ARGS_METHOD_IDX]);
    if (!(*jsMethodName)) {
        return;
    }
    std::string methodName(*jsMethodName);

    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsBeginLog(methodName, currentTime);
}

void JsPerfSleep(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsPerfEnd");
    if (args.Length() != 1) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("CurrentContext is empty!");
        return;
    }

    int32_t valInt = args[0]->Int32Value(context).ToChecked();
    usleep(valInt);
}

void JsPerfPrint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);

    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfPrint, CurrentContext is empty!");
        return;
    }

    // return the result
    std::string ret = JsApiPerf::GetInstance().PrintToLogs();
    v8::Local<v8::String> dataValue = v8::String::NewFromUtf8(isolate, ret.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(dataValue);
}

// function callback for hiViewObj's function: report
void JsHiViewReport(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsHiViewReport");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    if (args.Length() != HIVIEW_ARGS_LEN) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }
    if (args[HIVIEW_ARGS_ID_IDX]->IsNumber() && args[HIVIEW_ARGS_JSON_IDX]->IsString()) {
        v8::String::Utf8Value eventId(isolate, args[HIVIEW_ARGS_ID_IDX]);
        v8::String::Utf8Value eventJsonStr(isolate, args[HIVIEW_ARGS_JSON_IDX]);
        if (*eventId && *eventJsonStr) {
            EventReport::JsEventReport(StringToInt(*eventId), *eventJsonStr);
            LOGD("JsEventReport success");
        }
    } else {
        LOGE("parameter type error");
    }
}

// function callback for JsPluralRulesFormat's function: select
void JsPluralRulesFormat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsPluralRulesFormat");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    if (args.Length() != 1) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }
    if (args[0]->IsNumber()) {
        v8::String::Utf8Value choice(isolate, args[0]);
        if (*choice) {
            const char* result = Localization::GetInstance()->PluralRulesFormat(StringToDouble(*choice)).c_str();
            if (result == nullptr) {
                LOGE("result == nullptr");
                v8::Local<v8::Value> val = v8::Null(isolate);
                args.GetReturnValue().Set(val);
                return;
            }
            v8::Local<v8::String> resultV8 = v8::String::NewFromUtf8(isolate, result).ToLocalChecked();
            args.GetReturnValue().Set(resultV8);
            LOGD("JsPluralRulesFormat success");
        }
    } else {
        LOGE("parameter type error");
    }
}

inline int32_t GetNodeId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    int32_t id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
        .ToLocalChecked()
        ->Int32Value(ctx)
        .ToChecked();
    return id < 0 ? 0 : id;
}

void JsFocus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
        (*delegate)->TriggerPageUpdate(page->GetPageId(), true);
}

void JsAnimate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsAnimate, CurrentContext is empty!");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    v8::String::Utf8Value jsArguments(isolate, args[0]);
    if (!(*jsArguments)) {
        return;
    }
    std::string arguments(*jsArguments);
    int32_t nodeId = GetNodeId(context, args.Holder());
    auto resultValue = V8AnimationBridgeUtils::CreateAnimationContext(context, page->GetPageId(), nodeId);
    auto animationBridge = AceType::MakeRefPtr<V8AnimationBridge>(context, isolate, resultValue, nodeId);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    auto task = AceType::MakeRefPtr<V8AnimationBridgeTaskCreate>(*delegate, animationBridge, arguments);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    args.GetReturnValue().Set(resultValue);
}

void JsGetBoundingClientRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsGetBoundingClientRect, CurrentContext is empty!");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    int32_t nodeId = GetNodeId(context, args.Holder());
    auto rect = V8ComponentApiBridge::JsGetBoundingRect(isolate, nodeId);
    args.GetReturnValue().Set(rect);
}


void JsSetAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsSetAttribute, CurrentContext is empty!");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    auto attr = v8::Object::New(isolate);
    attr->Set(context, args[0], args[1]).ToChecked();
    int32_t nodeId = GetNodeId(context, args.Holder());
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(context, attr, *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
}

void JsSetStyle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsSetStyle, CurrentContext is empty!");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    if (!args[0]->IsString() || !args[1]->IsString()) {
        LOGE("args is not string ");
        return;
    }
    auto style = v8::Object::New(isolate);
    style->Set(context, args[0], args[1]).ToChecked();
    int32_t nodeId = GetNodeId(context, args.Holder());
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(context, style, *command);
    page->PushCommand(command);
}

void JsAppendChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsAppendChild, CurrentContext is empty!");
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    int32_t parentNodeId = GetNodeId(context, args.Holder());
    auto object = args[0]->ToObject(context).ToLocalChecked();
    int32_t id = object->Get(context, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    int32_t childId = id < 0 ? 0 : id;
    auto domDocument = page->GetDomDocument();
    if (domDocument) {
        RefPtr<DOMNode> node = domDocument->GetDOMNodeById(childId);
        if (node == nullptr) {
            LOGE("node is nullptr");
            return;
        }
        auto command = Referenced::MakeRefPtr<JsCommandAppendElement>(node->GetTag(), node->GetNodeId(), parentNodeId);
        page->PushCommand(command);
        if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
            page->FlushCommands();
        }
    }
}

void CreateDomElement(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter CreateDomElement");
    v8::Isolate* isolate = args.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    ACE_SCOPED_TRACE("CreateDomElement");
    if (args.Length() != 1) {
        LOGE("argc error, argc = %{private}d", args.Length());
        return;
    }
    auto page = GetStagingPage();
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return;
    }
    int32_t nodeId = ++globalNodeId;
    v8::String::Utf8Value tagJsStr(isolate, args[0]);
    if (!(*tagJsStr)) {
        return;
    }
    std::string tag(*tagJsStr);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomElement>(tag.c_str(), nodeId);
    page->PushCommand(command);
    // Flush command as fragment immediately when pushed too many commands.
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
}

void JsCreateElement(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CreateDomElement(args);
    v8::Isolate* isolate = args.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsPerfBegin, CurrentContext is empty!");
        return;
    }
    auto nodeObj = v8::Object::New(isolate);
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked(),
        v8::Int32::New(isolate, globalNodeId)).ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "setAttribute").ToLocalChecked(),
        v8::Function::New(context, JsSetAttribute, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "setStyle").ToLocalChecked(),
        v8::Function::New(context, JsSetStyle, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "addChild").ToLocalChecked(),
        v8::Function::New(context, JsAppendChild, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "focus").ToLocalChecked(),
        v8::Function::New(context, JsFocus, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "animate").ToLocalChecked(),
        v8::Function::New(context, JsAnimate, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    nodeObj->Set(context, v8::String::NewFromUtf8(isolate, "getBoundingClientRect").ToLocalChecked(),
        v8::Function::New(context, JsGetBoundingClientRect, v8::Local<v8::Value>(), 1).ToLocalChecked())
            .ToChecked();
    args.GetReturnValue().Set(nodeObj);
}
} // namespace

// -----------------------
// Start V8EngineInstance
// -----------------------
void V8EngineInstance::FlushCommandBuffer(void* context, const std::string& command)
{
    CHECK_RUN_ON(JS);
    LOGI("flush command buffer");
    auto v8Context = *(static_cast<v8::Local<v8::Context>*>(context));
    v8::Isolate* isolate = v8Context->GetIsolate();
    ACE_DCHECK(isolate);
    bool result = CallEvalBuf(isolate, command.c_str(), instanceId_);
    if (!result) {
        LOGE("failed to flush command");
    }
}

void V8EngineInstance::CallJs(const std::string& callbackId, const std::string& args, bool keepAlive, bool isGlobal)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = GetV8Isolate();
    v8::HandleScope handleScope(isolate);
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, GetV8Context());
    V8EngineInstance::CallJs(isolate, pContext, callbackId, args, keepAlive, isGlobal);
    pContext.Reset();
}

void V8EngineInstance::CallJs(v8::Isolate* isolate,
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pcontext, const std::string& callbackId,
    const std::string& args, bool keepAlive, bool isGlobal)
{
    CHECK_RUN_ON(JS);
    std::string keepAliveStr = keepAlive ? "true" : "false";
    std::string callBuff = std::string("[{\"args\": [\"")
                               .append(callbackId)
                               .append("\",")
                               .append(args)
                               .append(",")
                               .append(keepAliveStr)
                               .append("], \"method\":\"callback\"}]");
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = pcontext.Get(isolate);
    if (context.IsEmpty()) {
        LOGE("CallJs, context Is Empty");
        return;
    }

    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, "callJS").ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        return;
    }

    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);

    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::STAGING_PAGE));
    int32_t instanceId = isGlobal ? DEFAULT_APP_ID : (*stagingPage)->GetPageId();

    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::String> callBufStr = v8::String::NewFromUtf8(isolate, callBuff.c_str()).ToLocalChecked();
    v8::Local<v8::Value> jsonBuf;
    if (!v8::JSON::Parse(context, callBufStr).ToLocal(&jsonBuf)) {
        LOGE("Cannot parse callBuf to json format correctly, CallJs stop!");
        return;
    }
    v8::Local<v8::Value> argv[] = { v8::Integer::New(isolate, instanceId), jsonBuf };
    int32_t len = sizeof(argv) / sizeof(argv[0]);
    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (succ) {
        LOGD("func callJs success!");
    } else {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::FIRE_EVENT_ERROR, instanceId, *stagingPage);
        LOGD("func callJs fail!");
    }

    while (v8::platform::PumpMessageLoop(V8Engine::GetPlatform().get(), isolate)) {
        continue;
    }
}

bool V8EngineInstance::InitJsEnv()
{
    CHECK_RUN_ON(JS);
    LOGI("Enter InitJsEnv");
    // create Isolate
    create_params_.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
#if defined(USE_EXTERNAL_V8_SNAPSHOT)
#else
    static v8::StartupData snapshotBlob = {
        .data = _binary_strip_native_min_js_bin_start,
        .raw_size = _binary_strip_native_min_js_bin_end - _binary_strip_native_min_js_bin_start,
    };
    create_params_.snapshot_blob = &snapshotBlob;
#endif
    isolate_ = v8::Isolate::New(create_params_);
    if (isolate_ == nullptr) {
        LOGE("JS Engine cannot allocate JS runtime");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    isolate_->SetStackLimit(V8_MAX_STACK_SIZE);
    isolate_->SetData(V8EngineInstance::FRONTEND_DELEGATE, static_cast<void*>(&frontendDelegate_));
    isolate_->SetData(V8EngineInstance::RUNNING_PAGE, static_cast<void*>(&runningPage_));
    isolate_->SetData(V8EngineInstance::STAGING_PAGE, static_cast<void*>(&stagingPage_));
    isolate_->SetData(V8EngineInstance::DISPATCHER, static_cast<void*>(&dispatcher_));

    // create context
    InitJsContext();
    return true;
}

void V8EngineInstance::InitJsContext()
{
    CHECK_RUN_ON(JS);
    LOGI("Enter InitJsContext");
    v8::Isolate::Scope isolateScope(isolate_);
    v8::HandleScope handleScope(isolate_);

    InitGlobalObjectTemplate();

    v8::Local<v8::Context> localContext = v8::Context::New(isolate_, nullptr, globalObjectTemplate_.Get(isolate_));

    v8::Context::Scope contextScope(localContext);

    InitJsConsoleObject(localContext, isolate_);
    InitJsDocumentObject(localContext, isolate_);
    InitJsPerfUtilObject(localContext);

    const char* str = "var global = globalThis;\n"
                      "global.callNative = ace.callNative;\n";
    const char* debugStr = "global.compileAndRunBundle = ace.compileAndRunBundle;\n";
    const std::string& addStr = std::string(str) + std::string(debugStr);
    str = addStr.c_str();

    bool evalAceModule = CallEvalBuf(isolate_, str, instanceId_);
    if (!evalAceModule) {
        LOGW("JS Engine created JS context but failed to init Ace modules!");
    }

    auto groupJsBridge = DynamicCast<V8GroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (groupJsBridge == nullptr || groupJsBridge->InitializeGroupJsBridge(localContext) == JS_CALL_FAIL) {
        LOGE("JS Engine Initialize GroupJsBridge failed!");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
    }

    context_.Reset(isolate_, localContext);
}

void V8EngineInstance::InitJsConsoleObject(v8::Local<v8::Context>& localContext, v8::Isolate* isolate)
{
    if (isolate == nullptr) {
        LOGE("isolate is nullptr");
        return;
    }
    v8::Local<v8::Object> global = localContext->Global();
    // app log method
    v8::Local<v8::Value> console =
        global->Get(localContext, v8::String::NewFromUtf8(isolate, "console").ToLocalChecked()).ToLocalChecked();
    v8::Local<v8::Object> consoleObj = console->ToObject(localContext).ToLocalChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "log").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "debug").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "info").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::INFO)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "warn").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::WARNING)))
                .ToLocalChecked())
        .ToChecked();
    consoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(),
            v8::Function::New(
                localContext, AppLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::ERROR)))
                .ToLocalChecked())
        .ToChecked();
    // js framework log method
    auto aceConsoleObj = v8::Object::New(isolate);
    global->Set(localContext, v8::String::NewFromUtf8(isolate, "aceConsole").ToLocalChecked(), aceConsoleObj)
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "log").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "debug").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::DEBUG)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "info").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::INFO)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "warn").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::WARNING)))
                .ToLocalChecked())
        .ToChecked();
    aceConsoleObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(),
            v8::Function::New(
                localContext, JsLogPrint, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::ERROR)))
                .ToLocalChecked())
        .ToChecked();
}

void V8EngineInstance::InitJsDocumentObject(v8::Local<v8::Context>& localContext, v8::Isolate* isolate)
{
    v8::Local<v8::Object> global = localContext->Global();
    auto  documentObj = v8::Object::New(isolate);
    global->Set(localContext, v8::String::NewFromUtf8(isolate, "dom").ToLocalChecked(), documentObj).ToChecked();
    documentObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate, "createElement").ToLocalChecked(),
            v8::Function::New(
                localContext, JsCreateElement, v8::Integer::New(isolate, static_cast<int32_t>(JsLogLevel::ERROR)))
                .ToLocalChecked())
        .ToChecked();
}

void V8EngineInstance::InitJsPerfUtilObject(v8::Local<v8::Context>& localContext)
{
    v8::Local<v8::Object> global = localContext->Global();

    auto perfUtilObj = v8::Object::New(isolate_);
    global->Set(localContext, v8::String::NewFromUtf8(isolate_, "perfutil").ToLocalChecked(), perfUtilObj).ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_, "printlog").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfPrint).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_, "sleep").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfSleep).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_, "begin").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfBegin).ToLocalChecked())
        .ToChecked();
    perfUtilObj
        ->Set(localContext, v8::String::NewFromUtf8(isolate_, "end").ToLocalChecked(),
            v8::Function::New(localContext, JsPerfEnd).ToLocalChecked())
        .ToChecked();
}

void V8EngineInstance::InitGlobalObjectTemplate()
{
    v8::Isolate::Scope isolateScope(isolate_);
    v8::HandleScope handleScope(isolate_);
    v8::Local<v8::ObjectTemplate> globalObj = v8::ObjectTemplate::New(isolate_);

    // Add ace modules
    v8::Local<v8::External> engineInstance = v8::External::New(isolate_, static_cast<void*>(this));
    v8::Local<v8::ObjectTemplate> aceObj = v8::ObjectTemplate::New(isolate_);
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "domCreateBody").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsDomCreateBody));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "domAddElement").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsDomAddElement));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "updateElementAttrs").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsUpdateElementAttrs));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "updateElementStyles").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsUpdateElementStyles));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "onCreateFinish").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsOnCreateFinish));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "onUpdateFinish").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsOnUpdateFinish));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "removeElement").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsRemoveElement));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "callNative").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsCallNative, engineInstance));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "callComponent").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsCallComponent, engineInstance));
    aceObj->Set(v8::String::NewFromUtf8(isolate_, "compileAndRunBundle").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsCompileAndRunBundle));

    globalObj->Set(v8::String::NewFromUtf8(isolate_, "ace").ToLocalChecked(), aceObj);

    // Add hiview
    v8::Local<v8::ObjectTemplate> hiViewObj = v8::ObjectTemplate::New(isolate_);
    hiViewObj->Set(v8::String::NewFromUtf8(isolate_, "report").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsHiViewReport));
    globalObj->Set(v8::String::NewFromUtf8(isolate_, "hiView").ToLocalChecked(), hiViewObj);

    // Add i18nPluralRules
    v8::Local<v8::ObjectTemplate> i18nPluralRulesObj = v8::ObjectTemplate::New(isolate_);
    i18nPluralRulesObj->Set(v8::String::NewFromUtf8(isolate_, "select").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, JsPluralRulesFormat));
    globalObj->Set(v8::String::NewFromUtf8(isolate_, "i18nPluralRules").ToLocalChecked(), i18nPluralRulesObj);

    globalObjectTemplate_.Reset(isolate_, globalObj);
}

bool V8EngineInstance::FireJsEvent(const std::string& param)
{
    CHECK_RUN_ON(JS);
    LOGI("Enter FireJsEvent");
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = GetV8Context();
    if (context.IsEmpty()) {
        LOGE("FireJsEvent, context Is Empty");
        return false;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, "callJS").ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGE("cannot find 'callJS' function from global object, this should not happen!");
        return false;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);

    if (runningPage_ == nullptr) {
        LOGE("runningPage_ is null!");
        return false;
    }
    std::string pageId = std::to_string(runningPage_->GetPageId());
    v8::Local<v8::String> jsParam = v8::String::NewFromUtf8(isolate_, param.c_str()).ToLocalChecked();
    v8::Local<v8::Value> jsParamJson;
    if (!v8::JSON::Parse(context, jsParam).ToLocal(&jsParamJson)) {
        LOGE("Cannot parse jsParam to json format correctly, callJS stop!");
        return false;
    }
    v8::Local<v8::Value> argv[] = { v8::String::NewFromUtf8(isolate_, pageId.c_str()).ToLocalChecked(), jsParamJson };
    int32_t len = sizeof(argv) / sizeof(argv[0]);

    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (!succ) {
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::FIRE_EVENT_ERROR, instanceId_, runningPage_);
        return false;
    }

    while (v8::platform::PumpMessageLoop(V8Engine::GetPlatform().get(), isolate)) {
        continue;
    }

    return succ;
}

V8EngineInstance::~V8EngineInstance()
{
    CHECK_RUN_ON(JS);
    if (runningPage_) {
        runningPage_->OnJsEngineDestroy();
    }

    if (stagingPage_) {
        stagingPage_->OnJsEngineDestroy();
    }
    context_.Reset();
    globalObjectTemplate_.Reset();
    delete create_params_.array_buffer_allocator;

    // Destroy group bridge
    auto groupJsBridge = frontendDelegate_->GetGroupJsBridge();
    if (groupJsBridge != nullptr) {
        groupJsBridge->Destroy();
    }
    isolate_->Dispose();
}

void LoadDebuggerSo()
{
    LOGI("LoadDebuggerSo");
    const std::string soDir = "libv8_debugger.z.so";
    g_debugger = dlopen(soDir.c_str(), RTLD_LAZY);
    if (g_debugger == nullptr) {
        LOGE("cannot find debugger so");
    }
}

void StartDebuggerAgent(
    const std::unique_ptr<v8::Platform>& platform, const v8::Local<v8::Context>& context, std::string componentName,
    const bool isDebugMode, const int32_t instanceId)
{
    LOGI("StartAgent");
    if (g_debugger == nullptr) {
        LOGE("g_debugger is null");
        return;
    }

    StartDebug startDebug = (StartDebug)dlsym(g_debugger, "StartDebug");
    if (startDebug == nullptr) {
        LOGE("StartDebug=NULL, dlerror=%s", dlerror());
        return;
    }
    startDebug(platform, context, componentName, isDebugMode, instanceId);
}

// -----------------------
// Start V8Engine
// -----------------------
V8Engine::V8Engine(int32_t instanceId) : instanceId_(instanceId) {}

std::unique_ptr<v8::Platform>& V8Engine::GetPlatform()
{
    return V8Helper::GetPlatform();
}

bool V8Engine::Initialize(const RefPtr<FrontendDelegate>& delegate)
{
    ACE_SCOPED_TRACE("V8Engine::Initialize");
    LOGI("V8Engine initialize");
    ACE_DCHECK(delegate);
    CHECK_RUN_ON(JS);

    // Initialize V8 engine
    GetPlatform();

    // if load debugger.so successfully, debug mode
    if (IsDebugVersion()) {
        LoadDebuggerSo();
        LOGI("debug mode");
    }
    // Initialize engine instance
    engineInstance_ = AceType::MakeRefPtr<V8EngineInstance>(delegate, instanceId_);
    bool res = engineInstance_->InitJsEnv();
    if (!res) {
        return false;
    }

    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    {
        ACE_DCHECK(isolate);
        v8::Isolate::Scope isolateScope(isolate);
        v8::HandleScope handleScope(isolate);
        auto context = engineInstance_->GetV8Context();
        if (context.IsEmpty()) {
            LOGE("Initialize, context Is Empty");
            return false;
        }
        // debug mode
        if (g_debugger != nullptr) {
            std::string instanceName = GetInstanceName();
            if (instanceName.empty()) {
                LOGE("GetInstanceName fail, %s", instanceName.c_str());
                return false;
            }
            StartDebuggerAgent(GetPlatform(), context, instanceName, NeedDebugBreakPoint(), instanceId_);
        }
    }
    nativeEngine_ = std::make_shared<V8NativeEngine>(
        GetPlatform().get(), isolate, engineInstance_->GetContext(), static_cast<void*>(this));
    engineInstance_->SetV8NativeEngine(nativeEngine_);
    SetPostTask();
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

    return true;
}

void V8Engine::SetPostTask()
{
    LOGI("SetPostTask");
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    std::weak_ptr<V8NativeEngine> weakNativeEngine(nativeEngine_);
    auto&& postTask = [weakDelegate, weakNativeEngine, id = instanceId_](bool needSync) {
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->PostJsTask([weakNativeEngine, needSync, id]() {
            ContainerScope scope(id);
            auto nativeEngine = weakNativeEngine.lock();
            if (!nativeEngine) {
                LOGE("native v8 engine weak pointer invalid");
                return;
            }
            nativeEngine->Loop(LOOP_NOWAIT, needSync);
        });
    };
    nativeEngine_->SetPostTask(postTask);
}

void V8Engine::RegisterInitWorkerFunc()
{
    auto weakInstance = AceType::WeakClaim(AceType::RawPtr(engineInstance_));
    auto&& initWorkerFunc = [weakInstance](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterInitWorkerFunc called");
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto v8NativeEngine = static_cast<V8NativeEngine*>(nativeEngine);
        if (v8NativeEngine == nullptr) {
            LOGE("v8NativeEngine is nullptr");
            return;
        }
        auto instance = weakInstance.Upgrade();
        if (instance == nullptr) {
            LOGE("instance is nullptr");
            return;
        }
        v8::Isolate* isolate = v8NativeEngine->GetIsolate();
        if (isolate == nullptr) {
            LOGE("isolate is nullptr");
            return;
        }
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Context> localContext = v8NativeEngine->GetContext();
        auto dispatcher = instance->GetJsMessageDispatcher();
        isolate->SetData(V8EngineInstance::DISPATCHER, static_cast<void*>(&dispatcher));
        instance->InitJsConsoleObject(localContext, isolate);
    };
    nativeEngine_->SetInitWorkerFunc(initWorkerFunc);
}

void V8Engine::RegisterAssetFunc()
{
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& assetFunc = [weakDelegate](const std::string& uri, std::vector<uint8_t>& content, std::string& ami) {
        LOGI("WorkerCore RegisterAssetFunc called");
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        delegate->GetResourceData(uri, content);
    };
    nativeEngine_->SetGetAssetFunc(assetFunc);
}

void V8Engine::RegisterOffWorkerFunc()
{
    auto&& offWorkerFunc = [](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterOffWorkerFunc called");
    };
    nativeEngine_->SetOffWorkerFunc(offWorkerFunc);
}

void V8Engine::RegisterWorker()
{
    RegisterInitWorkerFunc();
    RegisterAssetFunc();
    RegisterOffWorkerFunc();
}

V8Engine::~V8Engine()
{
    CHECK_RUN_ON(JS);
    if (g_debugger != nullptr) {
        StopDebug stopDebug = (StopDebug)dlsym(g_debugger, "StopDebug");
        if (stopDebug != nullptr) {
            stopDebug();
        }
    }

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    if (nativeEngine_) {
        nativeEngine_->CancelCheckUVLoop();
    }
#endif

    if (g_debugger != nullptr) {
        dlclose(g_debugger);
    }
}

void V8Engine::GetLoadOptions(std::string& optionStr, bool isMainPage, const RefPtr<JsAcePage>& page)
{
    CHECK_RUN_ON(JS);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));

    auto mediaQuery = engineInstance_->GetDelegate()->GetMediaQueryInfoInstance();
    auto renderOption = MediaQueryInfo::GetMediaQueryJsonInfo();
    if (mediaQuery) {
        renderOption->Put("isInit", mediaQuery->GetIsInit());
        renderOption->Put("bundleUrl", page->GetUrl().c_str());
    }
    renderOption->Put("pcPreview", PC_PREVIEW);
    renderOption->Put("appInstanceId", "10002");
    renderOption->Put("packageName", (*delegate)->GetAppID().c_str());

    // get resourceConfig
    (*delegate)->GetResourceConfiguration(renderOption);

    // get i18n message
    (*delegate)->GetI18nData(renderOption);
    std::string language = AceApplicationInfo::GetInstance().GetLanguage();
    std::string region = AceApplicationInfo::GetInstance().GetCountryOrRegion();
    std::string local = language + "_" + region;
    renderOption->Put("language", local.c_str());

    if (isMainPage) {
        std::string commonsJsContent;
        if ((*delegate)->GetAssetContent("commons.js", commonsJsContent)) {
            bool commonsJsResult = CallEvalBuf(isolate, commonsJsContent.c_str(), instanceId_, "commons.js");
            if (!commonsJsResult) {
                LOGE("fail to excute load commonsjs script commonsJsResult");
            }
        }
        std::string vendorsJsContent;
        if ((*delegate)->GetAssetContent("vendors.js", vendorsJsContent)) {
            bool vendorsJsResult = CallEvalBuf(isolate, vendorsJsContent.c_str(), instanceId_, "vendors.js");
            if (!vendorsJsResult) {
                LOGE("fail to excute load vendorsjs script");
            }
        }
        std::string code;
        if ((*delegate)->GetAssetContent("app.js", code)) {
            renderOption->Put("appCreate", "true");
            renderOption->Put("appCode", code.c_str());
        } else {
            LOGE("app.js is missing!");
        }
        std::string appMap;
        if ((*delegate)->GetAssetContent("app.js.map", appMap)) {
            page->SetAppMap(appMap);
        } else {
            LOGI("app map is missing!");
        }
    } else {
        renderOption->Put("appCreate", "false");
    }
    optionStr = renderOption->ToString();
}

void V8Engine::LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage)
{
    CHECK_RUN_ON(JS);
    LOGI("Enter LoadJs");
    ACE_SCOPED_TRACE("V8Engine::LoadJs");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
    if (isMainPage) {
        ACE_DCHECK(!engineInstance_->GetRunningPage());
        engineInstance_->SetRunningPage(page);
    }

    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();
    v8::Context::Scope contextScope(context);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, "createInstance").ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGD("no property named \"createInstance\" in global object");
        return;
    }
    if (!jsFuncVal->IsFunction()) {
        LOGD("property \"createInstance\" is not a function");
        return;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);

    std::string jsContent;
    if (!(*delegate)->GetAssetContent(url, jsContent)) {
        LOGE("js file load failed!");
        return;
    }

    std::string jsSourceMap;
    if ((*delegate)->GetAssetContent(url + ".map", jsSourceMap)) {
        page->SetPageMap(jsSourceMap);
    } else {
        LOGI("js source map load failed!");
    }

    std::string jsonData = page->GetPageParams();
    if (jsonData.empty()) {
        jsonData = "{}";
    }
    std::string optionStr;
    GetLoadOptions(optionStr, isMainPage, page);

    v8::Local<v8::String> instanceId =
        v8::String::NewFromUtf8(isolate, std::to_string(page->GetPageId()).c_str()).ToLocalChecked();
    v8::Local<v8::String> jsSrc = v8::String::NewFromUtf8(isolate, jsContent.c_str()).ToLocalChecked();
    v8::Local<v8::String> optionJsStr = v8::String::NewFromUtf8(isolate, optionStr.c_str()).ToLocalChecked();
    v8::Local<v8::Value> renderOptions;
    if (!v8::JSON::Parse(context, optionJsStr).ToLocal(&renderOptions)) {
        LOGE("Cannot parse renderOption to json format correctly, createInstance stop!");
        return;
    }
    v8::Local<v8::String> dataJsStr = v8::String::NewFromUtf8(isolate, jsonData.c_str()).ToLocalChecked();
    v8::Local<v8::Value> data;
    if (!v8::JSON::Parse(context, dataJsStr).ToLocal(&data)) {
        LOGE("Cannot parse data to json format correctly, createInstance stop!");
        return;
    }
    v8::Local<v8::Value> info = v8::Object::New(isolate);

    v8::Local<v8::Value> argv[] = { instanceId, jsSrc, renderOptions, data, info };
    int32_t len = sizeof(argv) / sizeof(argv[0]);

    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (!succ) {
        LOGE("JS framework load js bundle failed! instanceId: %{public}d", instanceId_);
        V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::LOAD_JS_BUNDLE_ERROR, instanceId_, page);
        return;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8Engine::UpdateRunningPage(const RefPtr<JsAcePage>& page)
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter UpdateRunningPage");
    engineInstance_->SetRunningPage(page);
}

void V8Engine::UpdateStagingPage(const RefPtr<JsAcePage>& page)
{
    LOGD("Enter UpdateStagingPage");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
}

void V8Engine::ResetStagingPage()
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter ResetStagingPage");
    auto runningPage = engineInstance_->GetRunningPage();
    engineInstance_->ResetStagingPage(runningPage);
}

void V8Engine::DestroyPageInstance(int32_t pageId)
{
    CHECK_RUN_ON(JS);
    LOGI("Enter DestroyPageInstance");
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, "destroyInstance").ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGD("no property named \"destroyInstance\" in global object");
        return;
    }
    if (!jsFuncVal->IsFunction()) {
        LOGD("property \"destroyInstance\" is not a function");
        return;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);
    v8::Local<v8::String> instanceId =
        v8::String::NewFromUtf8(isolate, std::to_string(pageId).c_str()).ToLocalChecked();
    v8::Local<v8::Value> argv[] = { instanceId };
    int32_t len = sizeof(argv) / sizeof(argv[0]);

    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (!succ) {
        LOGE("JS Engine DestroyPageInstance FAILED!");

        auto page = engineInstance_->GetDelegate()->GetPage(pageId);
        if (page) {
            V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::DESTROY_PAGE_ERROR, instanceId_, page);
        } else {
            V8Utils::JsStdDumpErrorAce(
                isolate, &tryCatch, JsErrorType::DESTROY_PAGE_ERROR, instanceId_, engineInstance_->GetStagingPage());
        }
        return;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8Engine::UpdateApplicationState(const std::string& packageName, Frontend::State state)
{
    CHECK_RUN_ON(JS);
    LOGI("Enter UpdateApplicationState: update app instance state from jsfwk, packageName %{public}s",
        packageName.c_str());
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    auto context = engineInstance_->GetV8Context();
    if (context.IsEmpty()) {
        LOGE("Update Application State, context Is Empty");
        return;
    }
    v8::Context::Scope contextScope(context);

    v8::TryCatch tryCatch(isolate);

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

    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::String> funcName = v8::String::NewFromUtf8(isolate, stateType.c_str()).ToLocalChecked();
    v8::Local<v8::Value> jsFuncVal;
    bool succ = global->Get(context, funcName).ToLocal(&jsFuncVal);
    if (!succ) {
        LOGD("no property named \"destroyInstance\" in global object");
        return;
    }
    if (!jsFuncVal->IsFunction()) {
        LOGD("property \"destroyInstance\" is not a function");
        return;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(jsFuncVal);
    v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, packageName.c_str()).ToLocalChecked();
    v8::Local<v8::Value> argv[] = { name };
    int32_t len = sizeof(argv) / sizeof(argv[0]);

    v8::Local<v8::Value> res;
    succ = jsFunc->Call(context, global, len, argv).ToLocal(&res);
    if (!succ) {
        LOGE("JS Engine appDestroyFunc FAILED!");
        V8Utils::JsStdDumpErrorAce(
            isolate, &tryCatch, JsErrorType::DESTROY_APP_ERROR, instanceId_, engineInstance_->GetStagingPage());
        return;
    }

    while (v8::platform::PumpMessageLoop(GetPlatform().get(), isolate)) {
        continue;
    }
}

void V8Engine::TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter TimerCallback");
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();

    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (isInterval) {
        V8EngineInstance::CallJs(isolate, pContext, callbackId.c_str(), std::string("{}").c_str(), true, true);
        (*delegate)->WaitTimer(callbackId, delay, isInterval, false);
    } else {
        V8EngineInstance::CallJs(isolate, pContext, callbackId.c_str(), std::string("{}").c_str(), false, true);
        (*delegate)->ClearTimer(callbackId);
    }
    pContext.Reset();
}

void V8Engine::MediaQueryCallback(const std::string& callbackId, const std::string& args)
{
    JsEngine::MediaQueryCallback(callbackId, args);
    JsCallback(callbackId, args);
}

void V8Engine::JsCallback(const std::string& callbackId, const std::string& args)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter JsCallback args: %{private}s", args.c_str());
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();

    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);

    V8EngineInstance::CallJs(isolate, pContext, callbackId.c_str(), args.c_str(), true, false);
    pContext.Reset();
}

void V8Engine::RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp)
{
    CHECK_RUN_ON(JS);
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = engineInstance_->GetV8Context();

    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pContext;
    pContext.Reset(isolate, context);
    V8EngineInstance::CallJs(isolate, pContext, callbackId.c_str(), std::to_string(timeStamp).c_str(), false, true);
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->CancelAnimationFrame(callbackId);
    pContext.Reset();
}

void V8Engine::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter FireAsyncEvent");
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEvent\"}]");
    LOGD("FireAsyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);

    bool succ = engineInstance_->FireJsEvent(callBuf.c_str());
    if (!succ) {
        LOGE("JS Engine FireAsyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
        return;
    }
}

void V8Engine::FireSyncEvent(const std::string& eventId, const std::string& param)
{
    CHECK_RUN_ON(JS);
    LOGD("Enter FireSyncEvent");
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEventSync\"}]");
    LOGD("FireSyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);

    bool succ = engineInstance_->FireJsEvent(callBuf.c_str());
    if (!succ) {
        LOGE("JS Engine FireSyncEvent FAILED !! jsCall: %{private}s", callBuf.c_str());
        return;
    }
}

void V8Engine::FireExternalEvent(const std::string& componentId, const uint32_t nodeId)
{
    CHECK_RUN_ON(JS);
    ACE_DCHECK(engineInstance_);
    v8::Isolate* isolate = engineInstance_->GetV8Isolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("FireExternalEvent context is empty");
        return;
    }
    auto page = GetRunningPage();
    if (page == nullptr) {
        LOGE("FireExternalEvent GetRunningPage is nullptr");
        return;
    }
    std::string arguments;
    auto engine = AceType::RawPtr(engineInstance_);
    auto bridge = AceType::DynamicCast<V8XComponentBridge>(page->GetXComponentBridgeById(nodeId));
    if (bridge) {
        bridge->HandleContext(context, nodeId, arguments, engine);
        return;
    }
}

void V8Engine::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
{
    ACE_DCHECK(engineInstance_);
    LOGD("Enter SetJsMessageDispatcher");
    engineInstance_->SetJsMessageDispatcher(dispatcher);
}

void V8Engine::RunGarbageCollection()
{
    CHECK_RUN_ON(JS);
    if (engineInstance_) {
        v8::Isolate* isolate = engineInstance_->GetV8Isolate();
        if (isolate != nullptr) {
            isolate->LowMemoryNotification();
        }
    }
}

RefPtr<GroupJsBridge> V8Engine::GetGroupJsBridge()
{
    return AceType::MakeRefPtr<V8GroupJsBridge>(instanceId_);
}

} // namespace OHOS::Ace::Framework
