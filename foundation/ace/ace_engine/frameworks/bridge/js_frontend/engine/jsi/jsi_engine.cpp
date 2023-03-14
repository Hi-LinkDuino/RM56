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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

#ifndef WINDOWS_PLATFORM
#include <dlfcn.h>
#endif
#include <regex>
#include <unistd.h>

#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/utils/time_util.h"
#include "bridge/js_frontend/engine/jsi/ark_js_runtime.h"
#include "bridge/js_frontend/engine/jsi/ark_js_value.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/common/js_api_perf.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/common/runtime_constants.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_animation_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_animator_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_badge_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_chart_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_clock_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_component_api_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_image_animator_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_input_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_list_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_stepper_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_xcomponent_bridge.h"

namespace OHOS::Ace::Framework {

#ifdef APP_USE_ARM
const std::string ARK_DEBUGGER_LIB_PATH = "/system/lib/libark_debugger.z.so";
#else
const std::string ARK_DEBUGGER_LIB_PATH = "/system/lib64/libark_debugger.z.so";
#endif
const int32_t MAX_READ_TEXT_LENGTH = 4096;
const std::regex URI_PARTTEN("^\\/([a-z0-9A-Z_]+\\/)*[a-z0-9A-Z_]+\\.?[a-z0-9A-Z_]*$");
using std::shared_ptr;
static int32_t globalNodeId = 100000;

RefPtr<JsAcePage> GetStagingPage(const shared_ptr<JsRuntime>& runtime)
{
    LOGD("GetStagingPage");
    if (!runtime) {
        LOGE("JsRuntime is null, cannot get staging page!");
        return nullptr;
    }
    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    return engineInstance->GetStagingPage();
}

RefPtr<JsAcePage> GetRunningPage(const shared_ptr<JsRuntime>& runtime)
{
    LOGD("GetRunningPage");
    if (!runtime) {
        LOGE("JsRuntime is null, cannot get running page!");
        return nullptr;
    }
    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    return engineInstance->GetRunningPage();
}

RefPtr<FrontendDelegate> GetFrontendDelegate(const shared_ptr<JsRuntime>& runtime)
{
    LOGD("GetFrontendDelegate");
    if (!runtime) {
        LOGE("JsRuntime is null, cannot get frontend delegate!");
        return nullptr;
    }
    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    return engineInstance->GetFrontendDelegate();
}

void GetValueAsString(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value, std::string& str)
{
    if (!value->IsObject(runtime)) {
        LOGE("JsValue is not an object!");
        return;
    }
    int32_t len = 0;
    shared_ptr<JsValue> propertyNames;
    value->GetEnumerablePropertyNames(runtime, propertyNames, len);
    for (int32_t i = 0; i < len; ++i) {
        if (i != 0) {
            str.append(1, DOM_PICKER_SPLIT_ARRAY);
        }

        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        shared_ptr<JsValue> item = value->GetProperty(runtime, key);

        if (item->IsString(runtime) || item->IsNumber(runtime) || item->IsBoolean(runtime)) {
            std::string valStr = item->ToString(runtime);
            str.append(valStr);
            continue;
        }
        if (item->IsArray(runtime)) {
            int32_t subLen = item->GetArrayLength(runtime);
            for (int32_t j = 0; j < subLen; ++j) {
                if (j != 0) {
                    str.append(1, DOM_PICKER_SPLIT_ITEM);
                }

                shared_ptr<JsValue> subItem = item->GetElement(runtime, j);
                std::string subItemStr = subItem->ToString(runtime);
                str.append(subItemStr);
            }
            continue;
        }
    }
}

void GetStyleFamilyValue(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value, std::string& familyStyle)
{
    std::string family;
    std::string src;
    int32_t len = value->GetArrayLength(runtime);
    for (int32_t i = 0; i < len; ++i) {
        // ValArray is one row of family array
        shared_ptr<JsValue> element = value->GetElement(runtime, i);
        if (element->IsObject(runtime)) {
            shared_ptr<JsValue> properties;
            int32_t objLen = 0;
            element->GetPropertyNames(runtime, properties, objLen);
            for (int32_t j = 0; j < objLen; ++j) {
                shared_ptr<JsValue> propKey = properties->GetElement(runtime, j);
                shared_ptr<JsValue> propValue = element->GetProperty(runtime, propKey);
                if (propValue->IsString(runtime)) {
                    std::string propValueStr = propValue->ToString(runtime);
                    std::string propKeyStr = propKey->ToString(runtime);
                    if (propKeyStr == "fontFamily") {
                        family = propValueStr;
                        if (!src.empty()) {
                            GetFrontendDelegate(runtime)->RegisterFont(family, src);
                            family.erase();
                            src.erase();
                        }
                        if (familyStyle.length() > 0) {
                            familyStyle += ",";
                        }
                        familyStyle += propValueStr;
                    } else if (propKeyStr == "src") {
                        // The format of font src is: url("src"), here get the src.
                        src = propValueStr.substr(URL_SOURCE_START_IDX, propValueStr.length() - URL_SOURCE_SUFFIX_LEN);
                        if (!family.empty()) {
                            GetFrontendDelegate(runtime)->RegisterFont(family, src);
                            family.erase();
                            src.erase();
                        }
                    }
                }
            }
        }
    }
}

void GetStyleAnimationName(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    std::vector<std::unordered_map<std::string, std::string>>& styleVec)
{
    int32_t len = value->GetArrayLength(runtime);
    for (int32_t i = 0; i < len; ++i) {
        std::unordered_map<std::string, std::string> animationNameKeyFrame;
        shared_ptr<JsValue> element = value->GetElement(runtime, i);
        if (element->IsObject(runtime)) {
            int32_t objLen = 0;
            shared_ptr<JsValue> properties;
            element->GetPropertyNames(runtime, properties, objLen);
            for (int32_t j = 0; j < objLen; ++j) {
                shared_ptr<JsValue> propKey = properties->GetElement(runtime, j);
                shared_ptr<JsValue> propValue = element->GetProperty(runtime, propKey);
                if (propValue->IsString(runtime) || propValue->IsNumber(runtime)) {
                    std::string propKeyStr = propKey->ToString(runtime);
                    std::string propValueStr = propValue->ToString(runtime);
                    animationNameKeyFrame.emplace(propKeyStr, propValueStr);
                } else {
                    LOGD("GetStyleAnimationName: type of value is either string nor object, unsupported.");
                }
            }
        }
        if (!animationNameKeyFrame.empty()) {
            styleVec.emplace_back(animationNameKeyFrame);
        }
    }
}

void GetAttrImage(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, ImageProperties& imageProperties)
{
    if (!runtime || !valObject) {
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> value = valObject->GetProperty(runtime, key);
        if (!value) {
            LOGW("value is null. Ignoring!");
            continue;
        }
        std::string valStr = value->ToString(runtime);
        if (value->IsString(runtime) || value->IsNumber(runtime) || value->IsBoolean(runtime)) {
            if (keyStr == DOM_SRC) {
                imageProperties.src = valStr;
            } else if (keyStr == DOM_WIDTH) {
                imageProperties.width = StringToDimension(valStr);
            } else if (keyStr == DOM_HEIGHT) {
                imageProperties.height = StringToDimension(valStr);
            } else if (keyStr == DOM_TOP) {
                imageProperties.top = StringToDimension(valStr);
            } else if (keyStr == DOM_LEFT) {
                imageProperties.left = StringToDimension(valStr);
            } else if (keyStr == DOM_DURATION) {
                imageProperties.duration = StringToInt(valStr);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetAttrImages(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arrayVal, std::vector<ImageProperties>& images)
{
    if (!arrayVal) {
        return;
    }
    int32_t length = arrayVal->GetArrayLength(runtime);
    for (int32_t i = 0; i < length; ++i) {
        shared_ptr<JsValue> valArray = arrayVal->GetProperty(runtime, i);
        ImageProperties imageProperties;
        if (valArray && valArray->IsObject(runtime)) {
            GetAttrImage(runtime, valArray, imageProperties);
            images.push_back(imageProperties);
        }
    }
}

void SetDomAttributesWithArray(const shared_ptr<JsRuntime>& runtime, const std::string& keyStr,
    const shared_ptr<JsValue>& value, std::vector<std::pair<std::string, std::string>>& attrs,
    JsCommandDomElementOperator& command)
{
    if (!value->IsArray(runtime)) {
        LOGE("Value is not array type.");
        return;
    }
    if (keyStr == "datasets") {
        auto chartBridge = AceType::MakeRefPtr<JsiChartBridge>();
        chartBridge->GetAttrDatasets(runtime, value);
        command.SetDatasets(chartBridge->GetDatasets());
    } else if (keyStr == "images") {
        std::vector<ImageProperties> images;
        GetAttrImages(runtime, value, images);
        command.SetImagesAttr(std::move(images));
    } else if (keyStr == "segments") {
        auto chartBridge = AceType::MakeRefPtr<JsiChartBridge>();
        chartBridge->ParseAttrSegmentArray(runtime, value);
        command.SetSegments(chartBridge->GetSegments());
    } else if (keyStr == "menuoptions") {
        auto inputBridge = AceType::MakeRefPtr<JsiInputBridge>();
        inputBridge->ParseInputOptions(runtime, value);
        command.SetInputOptions(inputBridge->GetInputOptions());
    } else {
        std::string valStr;
        GetValueAsString(runtime, value, valStr);
        LOGD("SetDomAttributes: key: %{private}s, attr: %{private}s", keyStr.c_str(), valStr.c_str());
        attrs.emplace_back(keyStr, valStr);
    }
}

void SetDomAttributesWithObject(const shared_ptr<JsRuntime>& runtime, const std::string& keyStr,
    const shared_ptr<JsValue>& value, JsCommandDomElementOperator& command)
{
    if (keyStr == "options") {
        auto chartBridge = AceType::MakeRefPtr<JsiChartBridge>();
        chartBridge->GetAttrOptionsObject(runtime, value);
        command.SetOptions(chartBridge->GetChartOptions());
    } else if (keyStr == "segments") {
        auto chartBridge = AceType::MakeRefPtr<JsiChartBridge>();
        chartBridge->ParseAttrSingleSegment(runtime, value);
        command.SetSegments(chartBridge->GetSegments());
    } else if (keyStr == DOM_CLOCK_CONFIG) {
        auto clockBridge = AceType::MakeRefPtr<JsiClockBridge>();
        clockBridge->ParseClockConfig(runtime, value);
        command.SetClockConfig(clockBridge->GetClockConfig());
    } else if (keyStr == DOM_NODE_TAG_LABEL) {
        auto stepperBridge = AceType::MakeRefPtr<JsiStepperBridge>();
        StepperLabels label;
        stepperBridge->GetAttrLabel(runtime, value, label);
        command.SetStepperLabel(label);
    } else if (keyStr == DOM_BADGE_CONFIG) {
        auto badgeBridge = AceType::MakeRefPtr<JsiBadgeBridge>();
        badgeBridge->ParseBadgeConfig(runtime, value);
        command.SetBadgeConfig(badgeBridge->GetBadgeConfig());
    } else if (keyStr == DOM_STEPPER_LABEL) {
        auto stepperBridge = AceType::MakeRefPtr<JsiStepperBridge>();
        StepperLabels label;
        stepperBridge->GetAttrLabel(runtime, value, label);
        command.SetStepperLabel(label);
    } else {
        LOGD("key %{public}s unsupported. Ignoring!", keyStr.c_str());
    }
}

bool SetDomAttributes(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& attrObj, JsCommandDomElementOperator& command)
{
    LOGD("SetDomAttributes");
    bool hasShowAttr = false;

    if (!attrObj || !attrObj->IsObject(runtime)) {
        return false;
    }

    int32_t len = 0;
    shared_ptr<JsValue> properties;
    attrObj->GetPropertyNames(runtime, properties, len);

    std::vector<std::pair<std::string, std::string>> attrs;
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        if (!key) {
            LOGE("key is null. Ignoring!");
            continue;
        }
        shared_ptr<JsValue> value = attrObj->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        if (value->IsString(runtime) || value->IsNumber(runtime) || value->IsBoolean(runtime)) {
            std::string valStr = value->ToString(runtime);
            LOGD("SetDomAttributes: key %{private}s, attr: %{private}s", keyStr.c_str(), valStr.c_str());
            if (keyStr == DOM_ID) {
                command.SetId(valStr);
            } else if (keyStr == DOM_TARGET) {
                command.SetTarget(valStr);
            } else if (keyStr == DOM_SHARE_ID) {
                command.SetShareId(valStr);
            }
            attrs.emplace_back(keyStr, valStr);
            if (keyStr == DOM_SHOW) {
                hasShowAttr = true;
            }
        } else if (value->IsArray(runtime)) {
            SetDomAttributesWithArray(runtime, keyStr, value, attrs, command);
        } else if (value->IsObject(runtime)) {
            SetDomAttributesWithObject(runtime, keyStr, value, command);
        } else if (value->IsUndefined(runtime)) {
            LOGE("value of key[%{private}s] is undefined. Ignoring!", keyStr.c_str());
        } else {
            LOGE("value of key[%{private}s] unsupported type. Ignoring!", keyStr.c_str());
        }
    }
    command.SetAttributes(std::move(attrs));
    return hasShowAttr;
}

void SetDomStyle(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& styleObj, JsCommandDomElementOperator& command)
{
    LOGD("SetDomStyle");
    if (!styleObj || !styleObj->IsObject(runtime)) {
        return;
    }

    int32_t len = 0;
    shared_ptr<JsValue> properties;
    styleObj->GetPropertyNames(runtime, properties, len);

    std::vector<std::pair<std::string, std::string>> styles;
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        if (!key) {
            LOGE("key is null. Ignoring!");
            continue;
        }
        shared_ptr<JsValue> value = styleObj->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        if (value->IsString(runtime) || value->IsNumber(runtime) || value->IsBoolean(runtime)) {
            std::string valStr = value->ToString(runtime);
            LOGD("SetDomStyle: key: %{private}s, style: %{private}s", keyStr.c_str(), valStr.c_str());
            styles.emplace_back(keyStr, valStr);
        } else if (value->IsArray(runtime)) {
            if (strcmp(keyStr.c_str(), DOM_TEXT_FONT_FAMILY) == 0) {
                // Deal with special case such as fontFamily, suppose all the keys in the array are the same.
                std::string familyStyle;
                GetStyleFamilyValue(runtime, value, familyStyle);
                styles.emplace_back(keyStr, familyStyle);
            } else if (strcmp(keyStr.c_str(), DOM_ANIMATION_NAME) == 0) {
                // Deal with special case animationName, it different with fontfamily,
                // the keys in the array are different.
                std::vector<std::unordered_map<std::string, std::string>> animationNameStyles;
                GetStyleAnimationName(runtime, value, animationNameStyles);
                command.SetAnimationStyles(std::move(animationNameStyles));
            } else if (strcmp(keyStr.c_str(), DOM_TRANSITION_ENTER) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionEnter;
                GetStyleAnimationName(runtime, value, transitionEnter);
                command.SetTransitionEnter(std::move(transitionEnter));
            } else if (strcmp(keyStr.c_str(), DOM_TRANSITION_EXIT) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> transitionExit;
                GetStyleAnimationName(runtime, value, transitionExit);
                command.SetTransitionExit(std::move(transitionExit));
            } else if (strcmp(keyStr.c_str(), DOM_SHARED_TRANSITION_NAME) == 0) {
                std::vector<std::unordered_map<std::string, std::string>> sharedTransitionName;
                GetStyleAnimationName(runtime, value, sharedTransitionName);
                command.SetSharedTransitionName(std::move(sharedTransitionName));
            } else {
                LOGD("value is array, key unsupported. Ignoring!");
            }
        } else if (value->IsUndefined(runtime)) {
            LOGD("value is undefined. Ignoring!");
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
    auto pipelineContext = GetFrontendDelegate(runtime)->GetPipelineContext();
    command.SetPipelineContext(pipelineContext);
    command.SetStyles(std::move(styles));
}

void AddDomEvent(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& eventObj, JsCommandDomElementOperator& command)
{
    LOGD("AddDomEvent");

    if (!eventObj || !eventObj->IsObject(runtime)) {
        return;
    }

    int32_t len = 0;
    shared_ptr<JsValue> properties;
    eventObj->GetPropertyNames(runtime, properties, len);

    std::vector<std::string> events;
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        if (!key) {
            LOGE("key is null. Ignoring!");
            continue;
        }
        shared_ptr<JsValue> value = eventObj->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        if (value->IsString(runtime)) {
            std::string valStr = value->ToString(runtime);
            events.emplace_back(valStr);
        } else {
            LOGW("value of unsupported type. Ignoring!");
        }
    }
    command.AddEvents(std::move(events));
}

shared_ptr<JsValue> GetAppInfo(const shared_ptr<JsRuntime>& runtime)
{
    LOGD("GetAppInfo");
    auto delegate = GetFrontendDelegate(runtime);
    shared_ptr<JsValue> appID = runtime->NewString(delegate->GetAppID());
    shared_ptr<JsValue> appName = runtime->NewString(delegate->GetAppName());
    shared_ptr<JsValue> versionName = runtime->NewString(delegate->GetVersionName());
    shared_ptr<JsValue> versionCode = runtime->NewNumber(delegate->GetVersionCode());

    // return the result as an object
    shared_ptr<JsValue> res = runtime->NewObject();
    res->SetProperty(runtime, "appID", appID);
    res->SetProperty(runtime, "appName", appName);
    res->SetProperty(runtime, "versionName", versionName);
    res->SetProperty(runtime, "versionCode", versionCode);

    return res;
}

void Terminate(const shared_ptr<JsRuntime>& runtime)
{
    auto delegate = GetFrontendDelegate(runtime);
    WeakPtr<PipelineContext> pipelineContextWeak = delegate->GetPipelineContext();
    auto uiTaskExecutor = delegate->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->Finish();
        }
    });
}

void GetPackageInfoCallback(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& message, const std::string& callbackId)
{
    std::string arguments;
    std::string methods;

    if (!message || !message->IsObject(runtime)) {
        LOGE("GetPackageInfoCallback: jsMessage is not Object");
        arguments.append("{\"arguments\":[\"jsMessage is not Object\",200],");
        methods.append("\"method\":\"fail\"}");
    } else {
        shared_ptr<JsValue> packageName = message->GetProperty(runtime, APP_PACKAGE_NAME);
        if (!packageName->IsString(runtime) || packageName->ToString(runtime) == "") {
            arguments.append("{\"arguments\":[\"packageName is not available string\",202],");
            methods.append("\"method\":\"fail\"}");
        } else {
            AceBundleInfo bundleInfo;
            if (!AceApplicationInfo::GetInstance().GetBundleInfo(packageName->ToString(runtime), bundleInfo)) {
                LOGE("can not get info by GetBundleInfo");
                arguments.append("{\"arguments\":[\"can not get info\",200],");
                methods.append("\"method\":\"fail\"}");
            } else {
                auto infoList = JsonUtil::Create(true);
                infoList->Put("versionName", bundleInfo.versionName.c_str());
                infoList->Put("versionCode", std::to_string(bundleInfo.versionCode).c_str());
                arguments.append("{\"arguments\":[").append(infoList->ToString()).append("],");
                methods.append("\"method\":\"success\"}");
            }
        }
    }

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    engineInstance->CallJs(callbackId, arguments + methods, false);
}

void GetPackageInfo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    LOGD("GetPackageInfo");
    if (!arg->IsObject(runtime) || !arg->IsArray(runtime)) {
        LOGE("GetPackageInfo: arg is not Object or Array");
        return;
    }

    int32_t len = arg->GetArrayLength(runtime);
    if (len < static_cast<int32_t>(PAG_INFO_ARGS_LEN)) {
        LOGE("GetPackageInfo: invalid callback value");
        return;
    }
    shared_ptr<JsValue> message = arg->GetElement(runtime, PAG_INFO_ARGS_MSG_IDX);
    shared_ptr<JsValue> callBackId = arg->GetElement(runtime, 1);
    std::string callbackIdStr = callBackId->ToString(runtime);
    LOGD("system app getPackageInfo callBackID is %{private}s", callbackIdStr.c_str());

    if (!callbackIdStr.empty()) {
        GetPackageInfoCallback(runtime, message, callbackIdStr);
    }
}

void RequestFullWindow(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    int32_t duration = -1; // default scene
    const int32_t customFullWindowLength = 2;
    if (arg->IsArray(runtime)) {
        int32_t len = arg->GetArrayLength(runtime);
        if (len == customFullWindowLength) {
            shared_ptr<JsValue> param = arg->GetElement(runtime, 0);
            if (param && param->IsObject(runtime)) {
                shared_ptr<JsValue> durationValue = param->GetProperty(runtime, APP_REQUEST_FULL_WINDOW_DURATION);
                if (durationValue) {
                    std::string durationStr = durationValue->ToString(runtime);
                    if (!durationStr.empty()) {
                        duration = StringToInt(durationStr);
                        duration = duration >= 0 ? duration : -1;
                    }
                }
            }
        }
    }
    auto delegate = GetFrontendDelegate(runtime);
    WeakPtr<PipelineContext> pipelineContextWeak = delegate->GetPipelineContext();
    auto uiTaskExecutor = delegate->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak, duration]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->RequestFullWindow(duration);
        }
    });
}

void SetScreenOnVisible(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    std::map<std::string, std::string> params;
    std::string callbackId;

    static int32_t paramsLength = 2;
    if (!arg->IsArray(runtime) || arg->GetArrayLength(runtime) < paramsLength) {
        LOGE("SetScreenOnVisible, wrong args length!");
        return;
    }
    // get params
    shared_ptr<JsValue> paramObj = arg->GetElement(runtime, 0);
    if (!paramObj || !paramObj->IsObject(runtime)) {
        LOGE("SetScreenOnVisible, first argument is not an object!");
        return;
    }
    int32_t len = 0;
    shared_ptr<JsValue> properties;
    if (!paramObj->GetPropertyNames(runtime, properties, len)) {
        LOGE("SetScreenOnVisible, fail to get object property list!");
        return;
    }
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        shared_ptr<JsValue> val = paramObj->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        std::string valStr = val->ToString(runtime);
        params.try_emplace(keyStr, valStr);
    }
    // get callbackId
    callbackId = arg->GetElement(runtime, 1)->ToString(runtime);

    std::string flag = "fail";
    auto iter = params.find(APP_SCREEN_ON_VISIBLE_FLAG);
    if (iter != params.end()) {
        flag = iter->second;
    }

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!StringToBool(flag)) {
        engineInstance->CallJs(
            callbackId, R"({"arguments":["fail to set false flag in rich platform", 200],"method":"fail"})");
    } else {
        engineInstance->CallJs(callbackId, R"({"arguments":[],"method":"success"})");
    }
}

shared_ptr<JsValue> GetLocale(const shared_ptr<JsRuntime>& runtime)
{
    LOGD("GetLocale");

    shared_ptr<JsValue> language = runtime->NewString(AceApplicationInfo::GetInstance().GetLanguage());
    shared_ptr<JsValue> countryOrRegion = runtime->NewString(AceApplicationInfo::GetInstance().GetCountryOrRegion());
    shared_ptr<JsValue> dir = runtime->NewString(
        AceApplicationInfo::GetInstance().IsRightToLeft() ? LOCALE_TEXT_DIR_RTL : LOCALE_TEXT_DIR_LTR);

    shared_ptr<JsValue> res = runtime->NewObject();
    res->SetProperty(runtime, LOCALE_LANGUAGE, language);
    res->SetProperty(runtime, LOCALE_COUNTRY_OR_REGION, countryOrRegion);
    res->SetProperty(runtime, LOCALE_TEXT_DIR, dir);
    return res;
}

void SetLocale(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    LOGD("SetLocale");
    std::unique_ptr<JsonValue> localeJson = JsonUtil::ParseJsonString(arg->ToString(runtime));

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
            GetFrontendDelegate(runtime)->ChangeLocale(language, countryOrRegion);
        }
    }
}

std::string GetDeviceInfo()
{
    LOGD("GetDeviceInfo");
    auto infoList = JsonUtil::Create(true);
    infoList->Put("brand", SystemProperties::GetBrand().c_str());
    infoList->Put("manufacturer", SystemProperties::GetManufacturer().c_str());
    infoList->Put("model", SystemProperties::GetModel().c_str());
    infoList->Put("product", SystemProperties::GetProduct().c_str());

    if (AceApplicationInfo::GetInstance().GetLanguage().empty()) {
        infoList->Put("language", "N/A");
    } else {
        infoList->Put("language", AceApplicationInfo::GetInstance().GetLanguage().c_str());
    }
    if (AceApplicationInfo::GetInstance().GetCountryOrRegion().empty()) {
        infoList->Put("region", "N/A");
    } else {
        infoList->Put("region", AceApplicationInfo::GetInstance().GetCountryOrRegion().c_str());
    }

    auto container = Container::Current();
    int32_t width = container ? container->GetViewWidth() : 0;
    if (width != 0) {
        infoList->Put("windowWidth", std::to_string(width).c_str());
    } else {
        infoList->Put("windowWidth", "N/A");
    }

    int32_t height = container ? container->GetViewHeight() : 0;
    if (height != 0) {
        infoList->Put("windowHeight", std::to_string(height).c_str());
    } else {
        infoList->Put("windowHeight", "N/A");
    }

    infoList->Put("screenDensity", std::to_string(SystemProperties::GetResolution()).c_str());

    bool isRound = SystemProperties::GetIsScreenRound();
    if (isRound) {
        infoList->Put("screenShape", "circle");
    } else {
        infoList->Put("screenShape", "rect");
    }

    return infoList->ToString();
}

std::string ParseRouteUrl(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& key)
{
    LOGD("ParseRouteUrl");
    std::string argStr = arg->ToString(runtime);
    if (argStr.empty()) {
        return argStr;
    }

    std::string pageRoute;
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsString()) {
        pageRoute = argsPtr->GetValue(key)->GetString();
    }
    LOGD("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());

    return pageRoute;
}

std::string ParseRouteUrlSpecial(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    std::string argStr = arg->ToString(runtime);
    if (argStr.empty()) {
        return argStr;
    }

    std::string pageRoute;
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);

    if (argsPtr->Contains(ROUTE_KEY_URI)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    } else if (argsPtr->Contains(ROUTE_KEY_PATH)) {
        pageRoute = argsPtr->GetValue(ROUTE_KEY_PATH)->GetString();
    }
    LOGD("JsParseRouteUrl pageRoute = %{private}s", pageRoute.c_str());

    return pageRoute;
}

std::string ParseRouteParams(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& key)
{
    std::string argStr = arg->ToString(runtime);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
    std::string params;
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsObject()) {
        params = argsPtr->GetValue(key)->ToString();
    }
    return params;
}

int32_t ParseIntParams(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& key)
{
    std::string argStr = arg->ToString(runtime);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
    int32_t params = 0;
    if (argsPtr != nullptr && argsPtr->Contains(key) && argsPtr->GetValue(key)->IsNumber()) {
        params = argsPtr->GetValue(key)->GetInt();
    }
    return params;
}

bool ParseRouteOverwrite(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& key)
{
    std::string argStr = arg->ToString(runtime);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
    if (argsPtr != nullptr && argsPtr->Contains(key)) {
        return true;
    }
    return false;
}

void AddListener(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (arg->IsObject(runtime) && arg->IsArray(runtime)) {
        int32_t len = arg->GetArrayLength(runtime);
        if (len == 0) {
            return;
        }
        std::string listenerId = arg->GetElement(runtime, 0)->ToString(runtime);
        auto mediaQuery = GetFrontendDelegate(runtime)->GetMediaQueryInfoInstance();
        if (mediaQuery && !listenerId.empty()) {
            mediaQuery->SetListenerId(listenerId);
        }
    }
}

void ShowToast(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    LOGD("ShowToast");
    std::string argStr = arg->ToString(runtime);

    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
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
    LOGD("ShowToast message = %{private}s duration = %{public}d bottom = %{private}s", message.c_str(), duration,
        bottom.c_str());

    GetFrontendDelegate(runtime)->ShowToast(message, duration, bottom);
}

std::vector<ButtonInfo> ParseDialogButtons(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& key)
{
    std::vector<ButtonInfo> dialogButtons;
    std::string argStr = arg->ToString(runtime);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(argStr);
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsArray()) {
        for (int32_t i = 0; i < argsPtr->GetValue(key)->GetArraySize(); ++i) {
            auto button = argsPtr->GetValue(key)->GetArrayItem(i);
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

void ShowDialog(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    LOGD("ShowDialog");

    const std::string title = ParseRouteUrl(runtime, arg, PROMPT_KEY_TITLE);
    const std::string message = ParseRouteUrl(runtime, arg, PROMPT_KEY_MESSAGE);
    std::vector<ButtonInfo> buttons = ParseDialogButtons(runtime, arg, PROMPT_KEY_BUTTONS);
    const std::string success = ParseRouteUrl(runtime, arg, COMMON_SUCCESS);
    const std::string cancel = ParseRouteUrl(runtime, arg, COMMON_CANCEL);
    const std::string complete = ParseRouteUrl(runtime, arg, COMMON_COMPLETE);
    bool autoCancel = true;

    std::string argsStr = arg->ToString(runtime);
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

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    auto callback = [engineInstance, success, cancel, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                engineInstance->CallJs(
                    success, std::string("{\"index\":").append(std::to_string(successType)).append("}"), false);
                break;
            case 1:
                engineInstance->CallJs(cancel, std::string("\"cancel\",null"), false);
                break;
            case 2:
                engineInstance->CallJs(complete, std::string("\"complete\",null"), false);
                break;
            default:
                break;
        }
    };

    GetFrontendDelegate(runtime)->ShowDialog(title, message, buttons, autoCancel, std::move(callback), callbacks);
}

void SetTimer(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, bool isInterval)
{
    LOGD("SetTimer");
    if (arg->IsArray(runtime)) {
        int32_t len = arg->GetArrayLength(runtime);
        if (len < 2) {
            LOGW("SetTimer: invalid callback value");
            return;
        }
        std::string callBackId = arg->GetElement(runtime, 0)->ToString(runtime);
        std::string delay = arg->GetElement(runtime, 1)->ToString(runtime);
        if (!callBackId.empty() && !delay.empty()) {
            GetFrontendDelegate(runtime)->WaitTimer(callBackId, delay, isInterval, true);
        }
    }
}

void ClearTimeout(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    LOGD("ClearTimeout");

    if (arg->IsArray(runtime)) {
        int32_t len = arg->GetArrayLength(runtime);
        if (len < 1) {
            LOGW("ClearTimeout: invalid callback value");
            return;
        }
        std::string callBackId = arg->GetElement(runtime, 0)->ToString(runtime);
        if (!callBackId.empty()) {
            GetFrontendDelegate(runtime)->ClearTimer(callBackId);
        }
    }
}

shared_ptr<JsValue> JsHandleAnimationFrame(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    std::string callbackId = arg->ToString(runtime);
    if (callbackId.empty()) {
        LOGW("system animation callbackId is null");
        return runtime->NewNull();
    }

    if (methodName == ANIMATION_REQUEST_ANIMATION_FRAME) {
        GetFrontendDelegate(runtime)->RequestAnimationFrame(callbackId);
    } else if (methodName == ANIMATION_CANCEL_ANIMATION_FRAME) {
        GetFrontendDelegate(runtime)->CancelAnimationFrame(callbackId);
    } else {
        LOGW("animationFrame not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleCallback(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (methodName == CALLBACK_NATIVE) {
        if (arg->IsObject(runtime) && arg->IsArray(runtime)) {
            shared_ptr<JsValue> callbackId = arg->GetProperty(runtime, 0);
            shared_ptr<JsValue> result = arg->GetProperty(runtime, 1);
            std::string callbackIdStr = callbackId->ToString(runtime);
            std::string resultStr = result->ToString(runtime);

            std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(resultStr);
            if (argsPtr && argsPtr->GetValue(KEY_STEPPER_PENDING_INDEX) != nullptr) {
                auto jsonValue = argsPtr->GetValue(KEY_STEPPER_PENDING_INDEX);
                if (jsonValue->IsString()) {
                    resultStr = jsonValue->GetString();
                } else if (jsonValue->IsNumber()) {
                    resultStr = jsonValue->ToString();
                }
            }
            GetFrontendDelegate(runtime)->SetCallBackResult(callbackIdStr, resultStr);
        }
    } else if (methodName == APP_DESTROY_FINISH) {
        LOGD("JsHandleCallback: appDestroyFinish should release resource here");
    } else {
        LOGW("internal.jsResult not support method = %{private}s", methodName.c_str());
    }

    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleAppApi(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (methodName == APP_GET_INFO) {
        return GetAppInfo(runtime);
    } else if (methodName == APP_TERMINATE) {
        Terminate(runtime);
    } else if (methodName == APP_GET_PACKAGE_INFO) {
        GetPackageInfo(runtime, arg);
    } else if (methodName == APP_REQUEST_FULL_WINDOW) {
        RequestFullWindow(runtime, arg);
    } else if (methodName == APP_SCREEN_ON_VISIBLE) {
        SetScreenOnVisible(runtime, arg);
    } else {
        LOGW("system.app not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewNull();
}

std::string JsParseRouteUrl(const std::unique_ptr<JsonValue>& argsPtr, const std::string& key)
{
    std::string pageRoute;
    if (argsPtr != nullptr && argsPtr->GetValue(key) != nullptr && argsPtr->GetValue(key)->IsString()) {
        pageRoute = argsPtr->GetValue(key)->GetString();
    }
    return pageRoute;
}

bool GetParams(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, std::map<std::string, std::string>& params)
{
    if (!runtime) {
        LOGE("fail to get params due to runtime is illegal");
        return false;
    }
    int32_t len = 0;
    shared_ptr<JsValue> properties;
    if (!arg->GetPropertyNames(runtime, properties, len)) {
        LOGE("GetParams fail to get object property list!");
        return false;
    }
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> key = properties->GetElement(runtime, i);
        shared_ptr<JsValue> val = arg->GetProperty(runtime, key);
        std::string keyStr = key->ToString(runtime);
        std::string valStr = val->ToString(runtime);
        params[keyStr] = valStr;
    }
    return true;
}

bool GetParamsWithCallbackId(const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
    std::map<std::string, std::string>& params, std::string& callbackId)
{
    if (!runtime) {
        LOGE("fail to get params due to runtime is illegal");
        return false;
    }
    if (argv.size() < 2) {
        LOGE("fail to get params due to length is illegal");
        return false;
    }
    if (!GetParams(runtime, argv[0], params)) {
        return false;
    }
    if (!argv[1]->IsString(runtime)) {
        return false;
    }
    callbackId = argv[1]->ToString(runtime);
    return true;
}

bool ParseResourceStringParam(const char* paramName, const std::map<std::string, std::string>& params, std::string& str)
{
    std::string strName(paramName);
    auto iter = params.find(strName);
    if (iter != params.end()) {
        str = iter->second;
        return true;
    }
    return false;
}

bool ParseResourceNumberParam(const char* paramName, const std::map<std::string, std::string>& params, int32_t& num)
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
    const std::map<std::string, std::string>& params, std::string& uri, int32_t& position, int32_t& length)
{
    ParseResourceStringParam(READ_KEY_URI, params, uri);
    ParseResourceNumberParam(READ_KEY_POSITION, params, position);
    ParseResourceNumberParam(READ_KEY_LENGTH, params, length);
}

shared_ptr<JsValue> JsReadText(const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv)
{
    if (!runtime) {
        LOGE("JsReadText failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsReadText failed. engine is null.");
        return runtime->NewUndefined();
    }
    std::map<std::string, std::string> params;
    std::string callbackId;
    if (!GetParamsWithCallbackId(runtime, argv, params, callbackId)) {
        LOGE("JsReadText, get params or callbackId failed!");
        return runtime->NewUndefined();
    }
    std::string uri;
    ParseResourceStringParam(READ_KEY_URI, params, uri);
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        LOGE("JsReadText uri regex match failed");
        engine->CallJs(callbackId, R"({"arguments":["file uri pattern not correct", 202],"method":"fail"})");
        return runtime->NewUndefined();
    }

    std::string fileText;
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsReadText failed. delegate is null.");
        return runtime->NewUndefined();
    }
    if (!(delegate->GetResourceData(uri, fileText))) {
        LOGE("JsReadText get text data failed");
        engine->CallJs(callbackId, R"({"arguments":["get text data failed", 301],"method":"fail"})");
        return runtime->NewUndefined();
    }

    auto fileLength = ParseUtf8TextLength(fileText);
    int32_t position = 0;
    int32_t length = 0;
    if (!ParseResourceNumberParam(READ_KEY_POSITION, params, position)) {
        position = 1;
    }

    if (!ParseResourceNumberParam(READ_KEY_LENGTH, params, length) || (length > fileLength - position + 1)) {
        length = (fileLength - position + 1 <= 0) ? 0 : fileLength - position + 1;
    }

    if (fileLength == 0) {
        if ((position <= 0) || (length < 0)) {
            engine->CallJs(
                callbackId, R"({"arguments":["wrong start position or wrong read length", 202],"method":"fail"})");
            return runtime->NewUndefined();
        }
    } else {
        if ((position > fileLength) || (position <= 0) || (length < 0)) {
            engine->CallJs(
                callbackId, R"({"arguments":["wrong start position or wrong read length", 202],"method":"fail"})");
            return runtime->NewUndefined();
        }

        size_t substrPos = static_cast<size_t>(ParseUtf8TextSubstrStartPos(fileText, position));
        size_t substrEndPos = static_cast<size_t>(ParseUtf8TextSubstrEndPos(fileText, position + length - 1));
        fileText = fileText.substr(substrPos - 1, substrEndPos - substrPos + 1);
        HandleEscapeCharaterInUtf8TextForJson(fileText);
    }

    engine->CallJs(callbackId,
        std::string("{\"arguments\":[").append("{\"text\":\"").append(fileText).append("\"}],\"method\":\"success\"}"));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsReadArrayBuffer(
    const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv)
{
    if (!runtime) {
        LOGE("JsReadArrayBuffer failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsReadArrayBuffer failed. engine is null.");
        return runtime->NewUndefined();
    }
    std::map<std::string, std::string> params;
    std::string callbackId;
    if (!GetParamsWithCallbackId(runtime, argv, params, callbackId)) {
        LOGE("JsReadArrayBuffer, get params or callbackId failed!");
        return runtime->NewUndefined();
    }
    std::string uri;
    int32_t position = 1;
    int32_t length = MAX_READ_TEXT_LENGTH;
    ParseResourceParam(params, uri, position, length);
    std::smatch result;
    if (!std::regex_match(uri, result, URI_PARTTEN)) {
        LOGE("JsReadArrayBuffer uri regex match failed");
        engine->CallJs(callbackId, R"({"arguments":["file uri pattern not correct", 200],"method":"fail"})");
        return runtime->NewUndefined();
    }

    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsReadArrayBuffer failed. delegate is null.");
        return runtime->NewUndefined();
    }
    std::vector<uint8_t> binaryContent;
    if (!(delegate->GetResourceData(uri, binaryContent))) {
        LOGE("JsReadArrayBuffer get buffer data failed");
        engine->CallJs(callbackId, R"({"arguments":["get buffer data failed", 301],"method":"fail"})");
        return runtime->NewUndefined();
    }

    auto fileLength = static_cast<int32_t>(binaryContent.size());
    if (position > fileLength || position <= 0 || length <= 0) {
        LOGE("JsReadArrayBuffer position fileLength failed");
        engine->CallJs(
            callbackId, R"({"arguments":["wrong start position or wrong read length", 301],"method":"fail"})");
        return runtime->NewUndefined();
    }

    length = position + length - 1 > fileLength ? fileLength - position + 1 : length;
    shared_ptr<JsValue> binaryData = runtime->NewObject();
    shared_ptr<JsValue> binaryArray = runtime->NewArray();
    for (int32_t i = 0; i < length; ++i) {
        binaryArray->SetProperty(runtime, runtime->NewInt32(i), runtime->NewInt32(binaryContent[position - 1 + i]));
    }
    binaryData->SetProperty(runtime, "buffer", binaryArray);
    engine->CallJs(callbackId, R"({"arguments":["read array buffer success"],"method":"success"})");
    return binaryData;
}

shared_ptr<JsValue> JsHandleReadResource(
    const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv, const std::string& methodName)
{
    LOGD("JsHandleReadResource");
    if (methodName == READ_TEXT) {
        return JsReadText(runtime, argv);
    } else if (methodName == READ_ARRAY_BUFFER) {
        return JsReadArrayBuffer(runtime, argv);
    } else {
        LOGW("system.resource not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsHandleOffscreenCanvas(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    auto page = GetRunningPage(runtime);
    if (page == nullptr) {
        return runtime->NewUndefined();
    }

    if (methodName == OFFSCREEN_CANVAS_CREATE) {
        int32_t width = ParseIntParams(runtime, arg, "width");
        int32_t height = ParseIntParams(runtime, arg, "height");

        auto pipelineContext = GetFrontendDelegate(runtime)->GetPipelineContext();
        auto bridge = AceType::MakeRefPtr<JsiOffscreenCanvasBridge>(pipelineContext, width, height);
        page->PushOffscreenCanvasBridge(bridge->GetBridgeId(), bridge);
        return bridge->GetBridge(runtime);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsCallConfiguration(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (CONFIGURATION_GET_LOCALE == methodName) {
        return GetLocale(runtime);
    } else if (CONFIGURATION_SET_LOCALE == methodName) {
        SetLocale(runtime, arg);
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> JsGetDeviceInfo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    std::string callbackId = arg->ToString(runtime);
    if (callbackId.empty()) {
        LOGE("system device getInfo callBackID is null");
    } else {
        LOGD("system device getInfo callBackID = %{private}s", callbackId.c_str());
        std::string info = GetDeviceInfo();
        auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
        if (info.find("N/A") != std::string::npos) {
            engineInstance->CallJs(
                callbackId, std::string("{\"arguments\":[").append(info).append(",200],\"method\":\"fail\"}"), false);
        } else {
            engineInstance->CallJs(
                callbackId, std::string("{\"arguments\":[").append(info).append("],\"method\":\"success\"}"), false);
        }
    }

    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    auto src = ParseRouteUrl(runtime, arg, "src");
    auto success = ParseRouteUrl(runtime, arg, "success");
    auto fail = ParseRouteUrl(runtime, arg, "fail");

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engineInstance) {
        return runtime->NewNull();
    }

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
    engineInstance->GetFrontendDelegate()->HandleImage(src, callback);
    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleGridLayout(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (!runtime) {
        LOGE("JsHandleGridLayout failed. runtime is null.");
        return nullptr;
    }
    if (methodName == GRID_GET_SYSTEM_LAYOUT_INFO) {
        return runtime->NewString(GridSystemManager::GetInstance().GetCurrentGridInfo().ToString());
    } else {
        LOGW("grid not support method = %{public}s", methodName.c_str());
        return runtime->NewUndefined();
    }
}

shared_ptr<JsValue> JsHandleMediaQuery(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (methodName == ADD_LISTENER) {
        AddListener(runtime, arg);
    } else if (methodName == GET_DEVICE_TYPE) {
        auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
        if (engineInstance) {
            auto mediaQuery = engineInstance->GetFrontendDelegate()->GetMediaQueryInfoInstance();
            if (mediaQuery) {
                return runtime->NewString(mediaQuery->GetDeviceType());
            }
        }
    } else {
        LOGW("system.mediaquery not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewNull();
}

std::vector<ButtonInfo> JsParseDialogButtons(const std::unique_ptr<JsonValue>& argsPtr, const std::string& key)
{
    std::vector<ButtonInfo> dialogButtons;
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

void ShowActionMenu(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (!runtime) {
        LOGE("ShowActionMenu failed. runtime is null.");
        return;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("ShowActionMenu failed. engine is null.");
        return;
    }
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(arg->ToString(runtime));
    if (argsPtr == nullptr) {
        LOGE("ShowActionMenu failed. argsPtr is nullptr");
        return;
    }
    std::vector<ButtonInfo> buttons = JsParseDialogButtons(argsPtr, PROMPT_KEY_BUTTONS);
    if (buttons.empty() || buttons.size() > 6) { // The number of buttons cannot be zero or more than six
        LOGE("buttons is invalid");
        if (argsPtr->IsObject()) {
            const std::string fail = JsParseRouteUrl(argsPtr, COMMON_FAIL);
            const std::string complete = JsParseRouteUrl(argsPtr, COMMON_COMPLETE);
            engine->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})");
            engine->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:buttons is invalid"})");
        } else {
            std::string callBackStr = arg->ToString(runtime);
            // Get callbackId and clear redundant symbols, 2 is available min string length
            if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
                callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
                engine->CallJs(callBackStr,
                    R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:buttons is invalid"}],"method":"fail"})");
            }
        }
        return;
    }

    const std::string title = JsParseRouteUrl(argsPtr, PROMPT_KEY_TITLE);
    const std::string success = JsParseRouteUrl(argsPtr, COMMON_SUCCESS);
    const std::string fail = JsParseRouteUrl(argsPtr, COMMON_FAIL);
    const std::string complete = JsParseRouteUrl(argsPtr, COMMON_COMPLETE);
    auto callback = [engine, success, fail, complete](int32_t callbackType, int32_t successType) {
        switch (callbackType) {
            case 0:
                engine->CallJs(success, std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                                            .append(std::to_string(successType))
                                            .append("}")
                                            .c_str());
                engine->CallJs(complete, std::string(R"({"errMsg":"showActionMenu:ok","tapIndex":)")
                                             .append(std::to_string(successType))
                                             .append("}")
                                             .c_str());
                break;
            case 1:
                engine->CallJs(fail, R"({"errMsg":"showActionMenu:fail cancel"})");
                engine->CallJs(complete, R"({"errMsg":"showActionMenu:fail cancel"})");
                break;
            default:
                LOGE("callbackType is invalid");
                break;
        }
    };
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("ShowActionMenu failed. delegate is null.");
        return;
    }
    delegate->ShowActionMenu(title, buttons, std::move(callback));
}

shared_ptr<JsValue> JsHandlePrompt(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (methodName == PROMPT_SHOW_TOAST) {
        ShowToast(runtime, arg);
    } else if (methodName == PROMPT_SHOW_DIALOG) {
        ShowDialog(runtime, arg);
    } else if (methodName == PROMPT_SHOW_ACTION_MENU) {
        ShowActionMenu(runtime, arg);
    } else {
        LOGW("system.prompt not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> EnableAlertBeforeBackPage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (!runtime) {
        LOGE("EnableAlertBeforeBackPage failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("EnableAlertBeforeBackPage failed. engine is null.");
        return runtime->NewUndefined();
    }
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(arg->ToString(runtime));
    if (argsPtr != nullptr && argsPtr->IsObject()) {
        if (argsPtr->GetValue(PROMPT_KEY_MESSAGE) == nullptr || !argsPtr->GetValue(PROMPT_KEY_MESSAGE)->IsString()) {
            LOGE("enableAlertBeforeBackPage message is null");
            const std::string fail = JsParseRouteUrl(argsPtr, COMMON_FAIL);
            const std::string complete = JsParseRouteUrl(argsPtr, COMMON_COMPLETE);
            engine->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})");
            engine->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:massage is null"})");
            return runtime->NewUndefined();
        }
    } else {
        LOGE("enableAlertBeforeBackPage message is null");
        std::string callBackStr = arg->ToString(runtime);
        // Get callbackId and clear redundant symbols, 2 is available min string length
        if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
            callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
            engine->CallJs(callBackStr,
                R"({"arguments":[{"errMsg":"enableAlertBeforeBackPage:massage is null"}],"method":"fail"})");
        }
        return runtime->NewUndefined();
    }

    const std::string message = JsParseRouteUrl(argsPtr, PROMPT_KEY_MESSAGE);
    const std::string success = JsParseRouteUrl(argsPtr, COMMON_SUCCESS);
    const std::string fail = JsParseRouteUrl(argsPtr, COMMON_FAIL);
    const std::string complete = JsParseRouteUrl(argsPtr, COMMON_COMPLETE);
    auto callback = [engine, success, fail, complete](int32_t callbackType) {
        switch (callbackType) {
            case 1:
                engine->CallJs(success, R"({"errMsg":"enableAlertBeforeBackPage:ok"})");
                engine->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:ok"})");
                break;
            case 0:
                engine->CallJs(fail, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})");
                engine->CallJs(complete, R"({"errMsg":"enableAlertBeforeBackPage:fail cancel"})");
                break;
            default:
                LOGE("callbackType is invalid");
                break;
        }
    };
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("EnableAlertBeforeBackPage failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->EnableAlertBeforeBackPage(message, std::move(callback));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> DisableAlertBeforeBackPage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (!runtime) {
        LOGE("DisableAlertBeforeBackPage failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("DisableAlertBeforeBackPage failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("DisableAlertBeforeBackPage failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->DisableAlertBeforeBackPage();
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(arg->ToString(runtime));
    if (argsPtr->IsObject()) {
        const std::string success = JsParseRouteUrl(argsPtr, COMMON_SUCCESS);
        const std::string complete = JsParseRouteUrl(argsPtr, COMMON_COMPLETE);
        engine->CallJs(success, R"({"errMsg":"disableAlertBeforeBackPage:ok"})");
        engine->CallJs(complete, R"({"errMsg":"disableAlertBeforeBackPage:ok"})");
        return runtime->NewUndefined();
    }

    std::string callBackStr = arg->ToString(runtime);
    // Get callbackId and clear redundant symbols, 2 is available min string length
    if (callBackStr.size() > 2 && callBackStr.front() == '\"' && callBackStr.back() == '\"') {
        callBackStr = callBackStr.substr(1, callBackStr.size() - 2);
        engine->CallJs(callBackStr, R"({"arguments":[{"errMsg":"disableAlertBeforeBackPage:ok"}],"method":"success"})");
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> PostponePageTransition(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (!runtime) {
        LOGE("PostponePageTransition failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("PostponePageTransition failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("PostponePageTransition failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostponePageTransition();
    return runtime->NewUndefined();
}

shared_ptr<JsValue> LaunchPageTransition(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    if (!runtime) {
        LOGE("LaunchPageTransition failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("LaunchPageTransition failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("LaunchPageTransition failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->LaunchPageTransition();
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsHandlePageRoute(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    std::string uri = "";
    if (methodName == ROUTE_PAGE_BACK) {
        uri = ParseRouteUrlSpecial(runtime, arg);
    } else {
        uri = ParseRouteUrl(runtime, arg, ROUTE_KEY_URI);
    }
    std::string params = ParseRouteParams(runtime, arg, ROUTE_KEY_PARAMS);
    bool dontOverwrite = ParseRouteOverwrite(runtime, arg, ROUTE_KEY_DONT_OVERWRITE);

    std::unique_ptr<JsonValue> routerParamsData = JsonUtil::Create(true);
    routerParamsData->Put("paramsData", JsonUtil::ParseJsonString(params));
    routerParamsData->Put("dontOverwrite", dontOverwrite);
    params = routerParamsData->ToString();

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engineInstance) {
        LOGE("engineInstance is null");
        return runtime->NewNull();
    }
    // Operator map for page route.
    static const LinearMapNode<shared_ptr<JsValue> (*)(const std::string&, const std::string&, JsiEngineInstance&)>
        pageRouteOperators[] = {
            { ROUTE_PAGE_BACK,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    LOGD("JsBackRoute uri = %{private}s", uri.c_str());
                    instance.GetFrontendDelegate()->Back(uri);
                    return instance.GetJsRuntime()->NewNull();
                } },
            { ROUTE_PAGE_CLEAR,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    LOGD("Clear Page Route");
                    instance.GetFrontendDelegate()->Clear();
                    return instance.GetJsRuntime()->NewNull();
                } },
            { ROUTE_PAGE_GET_LENGTH,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    int32_t routeLength = instance.GetFrontendDelegate()->GetStackSize();
                    std::string indexLength = std::to_string(routeLength);
                    LOGD("JsGetLengthRoute routeLength=%{private}s", indexLength.c_str());
                    return instance.GetJsRuntime()->NewString(indexLength);
                } },
            { ROUTE_PAGE_GET_STATE,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    int32_t routeIndex = 0;
                    std::string routeName;
                    std::string routePath;
                    instance.GetFrontendDelegate()->GetState(routeIndex, routeName, routePath);
                    LOGD("JsGetStateRoute index=%{public}d name=%{private}s path=%{private}s", routeIndex,
                        routeName.c_str(), routePath.c_str());

                    shared_ptr<JsRuntime> runtime = instance.GetJsRuntime();
                    shared_ptr<JsValue> routeData = runtime->NewObject();
                    routeData->SetProperty(runtime, "index", runtime->NewNumber(routeIndex));
                    routeData->SetProperty(runtime, "name", runtime->NewString(routeName));
                    routeData->SetProperty(runtime, "path", runtime->NewString(routePath));
                    return routeData;
                } },
            { ROUTE_PAGE_PUSH,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    LOGD("JsPushRoute uri = %{private}s", uri.c_str());
                    instance.GetFrontendDelegate()->Push(uri, params);
                    return instance.GetJsRuntime()->NewNull();
                } },
            { ROUTE_PAGE_REPLACE,
                [](const std::string& uri, const std::string& params, JsiEngineInstance& instance) {
                    LOGD("JsReplaceRoute uri = %{private}s", uri.c_str());
                    instance.GetFrontendDelegate()->Replace(uri, params);
                    return instance.GetJsRuntime()->NewNull();
                } },
        };
    auto operatorIter = BinarySearchFindIndex(pageRouteOperators, ArraySize(pageRouteOperators), methodName.c_str());
    if (operatorIter != -1) {
        return pageRouteOperators[operatorIter].value(uri, params, *engineInstance);
    } else if (methodName == ROUTE_ENABLE_ALERT_BEFORE_BACK_PAGE) {
        return EnableAlertBeforeBackPage(runtime, arg);
    } else if (methodName == ROUTE_DISABLE_ALERT_BEFORE_BACK_PAGE) {
        return DisableAlertBeforeBackPage(runtime, arg);
    } else if (methodName == ROUTE_POSTPONE) {
        return PostponePageTransition(runtime, arg);
    } else if (methodName == ROUTE_LAUNCH) {
        return LaunchPageTransition(runtime, arg);
    } else {
        LOGW("system.router not support method = %{private}s", methodName.c_str());
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleSetTimeout(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (methodName == SET_TIMEOUT) {
        SetTimer(runtime, arg, false);
    } else if (methodName == CLEAR_TIMEOUT || methodName == CLEAR_INTERVAL) {
        ClearTimeout(runtime, arg);
    } else if (methodName == SET_INTERVAL) {
        SetTimer(runtime, arg, true);
    } else {
        LOGW("Unsupported method for timer module!");
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleAnimator(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg, const std::string& methodName)
{
    if (!runtime) {
        LOGE("runtime is nullptr");
        return nullptr;
    }
    if (!arg) {
        LOGE("arg is nullptr");
        return runtime->NewNull();
    }
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewNull();
    }
    std::string arguments = arg->ToString(runtime);
    if (methodName == ANIMATOR_CREATE_ANIMATOR) {
        int32_t bridgeId = JsiAnimatorBridgeUtils::JsCreateBridgeId();
        auto animatorContext = JsiAnimatorBridgeUtils::CreateAnimatorContext(runtime, page->GetPageId(), bridgeId);
        auto animatorBridge = AceType::MakeRefPtr<JsiAnimatorBridge>(runtime, animatorContext);
        auto task = AceType::MakeRefPtr<JsiAnimatorTaskCreate>(runtime, animatorBridge, arguments);
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimator>(bridgeId, task));
        return animatorContext;
    }
    return runtime->NewNull();
}

shared_ptr<JsValue> JsHandleModule(const std::string& moduleName, const std::string& methodName,
    const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv)
{
    static const LinearMapNode<shared_ptr<JsValue> (*)(
        const shared_ptr<JsRuntime>&, const std::vector<shared_ptr<JsValue>>&, const std::string&)>
        jsHandleMap[] = {
            { "animation",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleAnimationFrame(runtime, argv[1], methodName); } },
            { "internal.jsResult",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleCallback(runtime, argv[1], methodName); } },
            { "ohos.animator",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleAnimator(runtime, argv[1], methodName); } },
            { "system.app",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleAppApi(runtime, argv[1], methodName); } },
            { "system.configuration",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsCallConfiguration(runtime, argv[1], methodName); } },
            { "system.device",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) {
                    if (methodName == "getInfo") {
                        return JsGetDeviceInfo(runtime, argv[1]);
                    } else {
                        return runtime->NewNull();
                    }
                } },
            { "system.grid",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleGridLayout(runtime, argv[1], methodName); } },
            { "system.image", [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                                  const std::string& methodName) { return JsHandleImage(runtime, argv[1]); } },
            { "system.mediaquery",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleMediaQuery(runtime, argv[1], methodName); } },
            { "system.offscreenCanvas",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleOffscreenCanvas(runtime, argv[1], methodName); } },
            { "system.prompt",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandlePrompt(runtime, argv[1], methodName); } },
            { "system.resource",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleReadResource(runtime, argv, methodName); } },
            { "system.router",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandlePageRoute(runtime, argv[1], methodName); } },
            { "timer",
                [](const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
                    const std::string& methodName) { return JsHandleSetTimeout(runtime, argv[1], methodName); } },
        };

    auto jsHandleIter = BinarySearchFindIndex(jsHandleMap, ArraySize(jsHandleMap), moduleName.c_str());
    if (jsHandleIter != -1) {
        return jsHandleMap[jsHandleIter].value(runtime, std::move(argv), methodName);
    }
    return runtime->NewUndefined();
}

// native implementation for js function: ace.domCreateBody()
shared_ptr<JsValue> JsDomCreateBody(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != CREATE_BODY_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", CREATE_BODY_ARGS_LEN);
        return runtime->NewUndefined();
    }

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    const int32_t pageId = page->GetPageId();
    int32_t nodeId = DOM_ROOT_NODE_ID_BASE + pageId;
    LOGD("JsDomCreateBody: pageId: %{public}d, nodeId: %{public}d:", pageId, nodeId);

    std::string tag = argv[CREATE_BODY_TAG_IDX]->ToString(runtime);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomBody>(tag.c_str(), nodeId);
    SetDomAttributes(runtime, argv[CREATE_BODY_ATTR_IDX], *command);
    SetDomStyle(runtime, argv[CREATE_BODY_STYLE_IDX], *command);
    AddDomEvent(runtime, argv[CREATE_BODY_EVENT_IDX], *command);
    page->PushCommand(command);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsDomAddElement(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != ADD_ELEMENT_ARGS_LEN) {
        LOGE("The argc is wrong, it is supposed to have %{public}d arguments", ADD_ELEMENT_ARGS_LEN);
        return runtime->NewUndefined();
    }
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    const int32_t instanceId = argv[ADD_ELEMENT_INSTANCE_ID]->ToInt32(runtime);
    if (page->GetPageId() != instanceId) {
        page = GetFrontendDelegate(runtime)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsDomAddElement fail to get page, pageId: %{public}d", instanceId);
            return runtime->NewUndefined();
        }
    }

    const int32_t pageId = page->GetPageId();
    int32_t parentNodeId = argv[ADD_ELEMENT_PARID_IDX]->ToInt32(runtime);
    parentNodeId = parentNodeId == 0 ? (DOM_ROOT_NODE_ID_BASE + pageId) : parentNodeId;

    int32_t nodeId = argv[ADD_ELEMENT_NODEID_IDX]->ToInt32(runtime);
    std::string tag = argv[ADD_ELEMENT_TAG_IDX]->ToString(runtime);
    LOGD("JsDomAddElement: pageId: %{private}d, parentNodeId: %{private}d, nodeId: %{private}d, tag: %{private}s",
        pageId, parentNodeId, nodeId, tag.c_str());

    auto command = Referenced::MakeRefPtr<JsCommandAddDomElement>(tag.c_str(), nodeId, parentNodeId);
    SetDomAttributes(runtime, argv[ADD_ELEMENT_ATTR_IDX], *command);
    SetDomStyle(runtime, argv[ADD_ELEMENT_STYLE_IDX], *command);
    AddDomEvent(runtime, argv[ADD_ELEMENT_EVENT_INDEX], *command);
    int32_t itemIndex = argv[ADD_ELEMENT_ITEM_INDEX]->ToInt32(runtime);
    if (argv[ADD_ELEMENT_CUSTOM_INDEX]->IsBoolean(runtime)) {
        bool isCustomComponent = argv[ADD_ELEMENT_CUSTOM_INDEX]->ToBoolean(runtime);
        command->SetIsCustomComponent(isCustomComponent);
    }
    command->SetForIndex(itemIndex);
    page->PushCommand(command);
    // Flush command as fragment immediately when pushed too many commands.
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
    return runtime->NewNull();
}

// native implementation for js function: ace.removeElement()
shared_ptr<JsValue> JsRemoveElement(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsRemoveElement");
    if (argc != REMOVE_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", REMOVE_ELEMENT_ARGS_LEN);
        return runtime->NewUndefined();
    }

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    const int32_t instanceId = argv[REMOVE_ELEMENT_INSTANCE_ID]->ToInt32(runtime);
    if (page->GetPageId() != instanceId) {
        page = GetFrontendDelegate(runtime)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsRemoveElement fail to get page, pageId: %{public}d", instanceId);
            return runtime->NewUndefined();
        }
    }

    int32_t nodeId = argv[REMOVE_ELEMENT_ID_IDX]->ToInt32(runtime);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    page->PushCommand(Referenced::MakeRefPtr<JsCommandRemoveDomElement>(nodeId));
    return runtime->NewUndefined();
}

// native implementation for js function: ace.updateElementAttrs()
shared_ptr<JsValue> JsUpdateElementAttrs(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsUpdateElementAttrs");
    if (argc != UPLOAD_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", UPLOAD_ELEMENT_ARGS_LEN);
        return runtime->NewUndefined();
    }

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    const int32_t instanceId = argv[UPLOAD_ELEMENT_INSTANCE_ID]->ToInt32(runtime);
    if (page->GetPageId() != instanceId) {
        page = GetFrontendDelegate(runtime)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsRemoveElement fail to get page, pageId: %{public}d", instanceId);
            return runtime->NewUndefined();
        }
    }

    int32_t nodeId = argv[UPLOAD_ELEMENT_NID_IDX]->ToInt32(runtime);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(runtime, argv[UPLOAD_ELEMENT_DOM_IDX], *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
    return runtime->NewUndefined();
}

// native implementation for js function: ace.updateElementStyles()
shared_ptr<JsValue> JsUpdateElementStyles(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsUpdateElementStyles");
    if (argc != UPLOAD_ELEMENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", UPLOAD_ELEMENT_ARGS_LEN);
        return runtime->NewUndefined();
    }

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    const int32_t instanceId = argv[UPLOAD_ELEMENT_INSTANCE_ID]->ToInt32(runtime);
    if (page->GetPageId() != instanceId) {
        page = GetFrontendDelegate(runtime)->GetPage(instanceId);
        if (page == nullptr) {
            LOGE("JsRemoveElement fail to get page, pageId: %{public}d", instanceId);
            return runtime->NewUndefined();
        }
    }

    int32_t nodeId = argv[UPLOAD_ELEMENT_NID_IDX]->ToInt32(runtime);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(runtime, argv[UPLOAD_ELEMENT_DOM_IDX], *command);
    page->PushCommand(command);
    return runtime->NewUndefined();
}

// native implementation for js function: ace.onCreateFinish()
shared_ptr<JsValue> JsOnCreateFinish(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsOnCreateFinish");

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    page->SetPageCreated();
    return runtime->NewUndefined();
}

// native implementation for js function: ace.onUpdateFinish()
shared_ptr<JsValue> JsOnUpdateFinish(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsOnUpdateFinish");

    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }

    if (page->CheckPageCreated()) {
        GetFrontendDelegate(runtime)->TriggerPageUpdate(page->GetPageId());
    }
    return runtime->NewUndefined();
}

// native implementation for js function: ace.callNative()
shared_ptr<JsValue> JsCallNative(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsCallNative");
    if (argc != NATIVE_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", NATIVE_ARGS_LEN);
        return runtime->NewUndefined();
    }

    std::string moduleAndMethod = argv[NATIVE_ARGS_METHOD_IDX]->ToString(runtime);
    LOGD("JsCallNative moduleAndMethod = %{public}s", moduleAndMethod.c_str());

    std::unique_ptr<JsonValue> moduleAndMethodPtr = JsonUtil::ParseJsonString(moduleAndMethod);
    if (!moduleAndMethodPtr) {
        LOGE("Get moduleAndMethod from argv failed");
        return runtime->NewUndefined();
    }

    std::unique_ptr<JsonValue> modulePtr = moduleAndMethodPtr->GetValue("module");
    if (!modulePtr) {
        LOGE("Get module from moduleAndMethodPtr failed");
        return runtime->NewUndefined();
    }

    std::unique_ptr<JsonValue> methodPtr = moduleAndMethodPtr->GetValue("method");
    if (!methodPtr) {
        LOGE("Get method from moduleAndMethodPtr failed");
        return runtime->NewUndefined();
    }

    const std::string moduleName = modulePtr->GetString();
    const std::string methodName = methodPtr->GetString();

    return JsHandleModule(moduleName, methodName, runtime, argv);
}

// native implementation for js function: ace.callComponent()
shared_ptr<JsValue> JsCallComponent(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsCallComponent");
    if (argc != COMPONENT_ARGS_LEN) {
        LOGE("The arg is wrong, it is supposed to have %{public}d arguments", COMPONENT_ARGS_LEN);
        return runtime->NewUndefined();
    }

    auto page = GetRunningPage(runtime);
    if (page == nullptr) {
        return runtime->NewUndefined();
    }
    int32_t nodeId = argv[COMPONENT_ARGS_ID_IDX]->ToInt32(runtime);
    nodeId = (nodeId == 0) ? DOM_ROOT_NODE_ID_BASE + page->GetPageId() : nodeId;
    std::string methodName = argv[COMPONENT_ARGS_METHOD_IDX]->ToString(runtime);
    std::string arguments = argv[COMPONENT_ARGS_IDX]->ToString(runtime);

    if (std::strcmp(methodName.c_str(), "getContext") == 0) {
        auto canvasBridge = AceType::DynamicCast<JsiCanvasBridge>(page->GetBridgeById(nodeId));
        if (canvasBridge) {
            canvasBridge->HandleJsContext(runtime, nodeId, arguments);
            return canvasBridge->GetRenderContext();
        }
        return runtime->NewUndefined();
    } else if (std::strcmp(methodName.c_str(), "toDataURL") == 0) {
        auto bridge = AceType::DynamicCast<JsiCanvasBridge>(page->GetBridgeById(nodeId));
        if (bridge) {
            bridge->HandleToDataURL(runtime, nodeId, arguments);
            return bridge->GetDataURL();
        }
    } else if (std::strcmp(methodName.c_str(), "getBoundingClientRect") == 0) {
        return JsiComponentApiBridge::JsGetBoundingRect(runtime, nodeId);
    } else if (std::strcmp(methodName.c_str(), "getInspector") == 0) {
        return JsiComponentApiBridge::JsGetInspector(runtime, nodeId);
    } else if (std::strcmp(methodName.c_str(), "getScrollOffset") == 0) {
        return JsiComponentApiBridge::JsGetScrollOffset(runtime, nodeId);
    } else if (std::strcmp(methodName.c_str(), "scrollTo") == 0) {
        JsiComponentApiBridge::JsScrollTo(runtime, arguments, nodeId);
        return runtime->NewUndefined();
    } else if (std::strcmp(methodName.c_str(), "getXComponentContext") == 0) {
        auto bridge = AceType::DynamicCast<JsiXComponentBridge>(page->GetXComponentBridgeById(nodeId));
        if (bridge) {
            bridge->HandleContext(runtime, nodeId, arguments);
            return bridge->GetRenderContext();
        }
        return runtime->NewUndefined();
#ifdef OHOS_STANDARD_SYSTEM
    } else if (std::strcmp(methodName.c_str(), "getXComponentSurfaceId") == 0) {
        return JsiXComponentBridge::JsGetXComponentSurfaceId(runtime, nodeId);
#endif
    }

    shared_ptr<JsValue> resultValue = runtime->NewUndefined();
    if (std::strcmp(methodName.c_str(), "animate") == 0) {
        LOGD("animate args = %{private}s", arguments.c_str());
        resultValue = JsiAnimationBridgeUtils::CreateAnimationContext(runtime, page->GetPageId(), nodeId);
        auto animationBridge = AceType::MakeRefPtr<JsiAnimationBridge>(runtime, resultValue, nodeId);
        auto task = AceType::MakeRefPtr<JsiAnimationBridgeTaskCreate>(runtime, animationBridge, arguments);
        page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    } else if (std::strcmp(methodName.c_str(), "currentOffset") == 0) {
        return JsiListBridge::JsGetCurrentOffset(runtime, nodeId);
    } else if (std::strcmp(methodName.c_str(), "getState") == 0) {
        return JsiImageAnimatorBridge::JsGetState(runtime, nodeId);
    }  else if (std::strcmp(methodName.c_str(), "addChild") == 0) {
        auto sPage = GetStagingPage(runtime);
        if (sPage == nullptr) {
            return runtime->NewUndefined();
        }
        int32_t childNodeId = 0;
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(arguments);
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
        return runtime->NewUndefined();
    } else {
        page->PushCommand(Referenced::MakeRefPtr<JsCommandCallDomElementMethod>(nodeId, methodName, arguments));
    }
    // focus method should delayed util show attribute update.
    if (page->CheckPageCreated() && strlen(DOM_FOCUS) >= strlen(methodName.c_str()) &&
        strcmp(DOM_FOCUS, methodName.c_str()) != 0) {
        GetFrontendDelegate(runtime)->TriggerPageUpdate(page->GetPageId(), true);
    }
    return resultValue;
}

int GetNodeId(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& arg)
{
    int32_t id = 0;
    auto nodeId = arg->GetProperty(runtime, "__nodeId");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    id = id < 0 ? 0 : id;
    return id;
}

shared_ptr<JsValue> JsSetAttribute(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 2) {
        LOGE("the argc is error");
        return runtime->NewUndefined();
    }
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewUndefined();
    }
    if (!argv[0]->IsString(runtime) || !argv[1]->IsString(runtime)) {
        LOGE("args is not string ");
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> attr = runtime->NewObject();
    attr->SetProperty(runtime, argv[0], argv[1]);

    int32_t nodeId = GetNodeId(runtime, thisObj);
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementAttrs>(nodeId);
    if (SetDomAttributes(runtime, attr, *command)) {
        page->ReserveShowCommand(command);
    }
    page->PushCommand(command);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsSetStyle(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 2) {
        LOGE("the argc is error");
        return runtime->NewBoolean(false);
    }
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return runtime->NewBoolean(false);
    }
    if (!argv[0]->IsString(runtime) || !argv[1]->IsString(runtime)) {
        LOGE("args is not string ");
        return runtime->NewBoolean(false);
    }
    shared_ptr<JsValue> style = runtime->NewObject();
    style->SetProperty(runtime, argv[0], argv[1]);

    int32_t nodeId = GetNodeId(runtime, thisObj);
    auto command = Referenced::MakeRefPtr<JsCommandUpdateDomElementStyles>(nodeId);
    SetDomStyle(runtime, style, *command);
    page->PushCommand(command);
    return runtime->NewBoolean(true);
}

shared_ptr<JsValue> JsAppendChild(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewUndefined();
    }
    int32_t id = GetNodeId(runtime, argv[0]);
    auto domDocument = page->GetDomDocument();
    if (domDocument) {
        RefPtr<DOMNode> node = domDocument->GetDOMNodeById(id);
        if (node == nullptr) {
            LOGE("node is nullptr");
        }
        int32_t parentNodeId = GetNodeId(runtime, thisObj);
        RefPtr<DOMNode> parentNode = domDocument->GetDOMNodeById(parentNodeId);
        if (parentNode == nullptr) {
            LOGE("parentNodeId is nullptr");
        }
        if (parentNode != nullptr && node != nullptr) {
            parentNode->RemoveNode(node);
        }
        auto command = Referenced::MakeRefPtr<JsCommandAppendElement>(node ? node->GetTag() : "", id, parentNodeId);
        page->PushCommand(command);
        if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
            page->FlushCommands();
        }
    }
    return runtime->NewUndefined();
}

int32_t CreateDomElement(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("the argc is error");
        return -1;
    }
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("GetStagingPage return nullptr");
        return -1;
    }
    int32_t nodeId = ++globalNodeId;
    std::string tag = argv[0]->ToString(runtime);
    auto command = Referenced::MakeRefPtr<JsCommandCreateDomElement>(tag.c_str(), nodeId);
    page->PushCommand(command);
    if (!page->CheckPageCreated() && page->GetCommandSize() > FRAGMENT_SIZE) {
        page->FlushCommands();
    }
    return  globalNodeId;
}

shared_ptr<JsValue> JsFocus(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewUndefined();
    }
    if (page->CheckPageCreated()) {
        GetFrontendDelegate(runtime)->TriggerPageUpdate(page->GetPageId(), true);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsAnimate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto page = GetStagingPage(runtime);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewUndefined();
    }
    int32_t nodeId = GetNodeId(runtime, thisObj);
    std::string arguments = argv[0]->ToString(runtime);
    shared_ptr<JsValue> resultValue = runtime->NewUndefined();
    resultValue = JsiAnimationBridgeUtils::CreateAnimationContext(runtime, page->GetPageId(), nodeId);
    auto animationBridge = AceType::MakeRefPtr<JsiAnimationBridge>(runtime, resultValue, nodeId);
    auto task = AceType::MakeRefPtr<JsiAnimationBridgeTaskCreate>(runtime, animationBridge, arguments);
    page->PushCommand(Referenced::MakeRefPtr<JsCommandAnimation>(nodeId, task));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsGetBoundingClientRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetNodeId(runtime, thisObj);
    return JsiComponentApiBridge::JsGetBoundingRect(runtime, nodeId);
}

shared_ptr<JsValue> JsGetInspector(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t nodeId = GetNodeId(runtime, thisObj);
    return JsiComponentApiBridge::JsGetInspector(runtime, nodeId);
}

shared_ptr<JsValue> JsCreateElement(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t newNodeId = CreateDomElement(runtime, thisObj, argv, argc);
    shared_ptr<JsValue> node = runtime->NewObject();
    node->SetProperty(runtime, "__nodeId", runtime->NewInt32(newNodeId));
    node->SetProperty(runtime, "setAttribute", runtime->NewFunction(JsSetAttribute));
    node->SetProperty(runtime, "setStyle", runtime->NewFunction(JsSetStyle));
    node->SetProperty(runtime, "addChild", runtime->NewFunction(JsAppendChild));
    node->SetProperty(runtime, "focus", runtime->NewFunction(JsFocus));
    node->SetProperty(runtime, "animate", runtime->NewFunction(JsAnimate));
    node->SetProperty(runtime, "getBoundingClientRect", runtime->NewFunction(JsGetBoundingClientRect));
    node->SetProperty(runtime, "getInspector", runtime->NewFunction(JsGetInspector));
    return node;
}

// native implementation for js function: perfutil.print()
shared_ptr<JsValue> JsPerfPrint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    std::string ret = JsApiPerf::GetInstance().PrintToLogs();
    return runtime->NewString(ret);
}

// native implementation for js function: perfutil.sleep()
shared_ptr<JsValue> JsPerfSleep(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t valInt = argv[0]->ToInt32(runtime);
    usleep(valInt);
    return runtime->NewNull();
}

// native implementation for js function: perfutil.begin()
shared_ptr<JsValue> JsPerfBegin(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsBeginLog(argv[0]->ToString(runtime), currentTime);
    return runtime->NewNull();
}

// native implementation for js function: perfutil.end()
shared_ptr<JsValue> JsPerfEnd(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int64_t currentTime = GetMicroTickCount();
    JsApiPerf::GetInstance().InsertJsEndLog(argv[0]->ToString(runtime), currentTime);
    return runtime->NewNull();
}

// native implementation for js function: hiView.report()
shared_ptr<JsValue> JsHiViewReport(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsHiViewReport");

    if (argc != HIVIEW_ARGS_LEN) {
        LOGE("argc error, argc = %{public}d", argc);
        return runtime->NewNull();
    }
    if (argv[HIVIEW_ARGS_ID_IDX]->IsNumber(runtime) && argv[HIVIEW_ARGS_JSON_IDX]->IsString(runtime)) {
        std::string eventId = argv[HIVIEW_ARGS_ID_IDX]->ToString(runtime);
        std::string eventJson = argv[HIVIEW_ARGS_JSON_IDX]->ToString(runtime);
        EventReport::JsEventReport(StringToInt(eventId), eventJson);
        LOGD("JsEventReport success");
    } else {
        LOGE("parameter type error");
    }
    return runtime->NewNull();
}

// native implementation for js function: i18nPluralRules.select()
shared_ptr<JsValue> JsPluralRulesFormat(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // Should have one parameters.
    if (argc != 1) {
        LOGE("argc error, argc = %{public}d", argc);
        return runtime->NewNull();
    }
    if (argv[0]->IsNumber(runtime)) {
        std::string choice = argv[0]->ToString(runtime);
        shared_ptr<JsValue> result =
            runtime->NewString(Localization::GetInstance()->PluralRulesFormat(StringToDouble(choice)));
        return result;
    } else {
        LOGE("parameter type error ");
        return runtime->NewNull();
    }
}

// -----------------------
// Start JsiEngineInstance
// -----------------------
JsiEngineInstance::~JsiEngineInstance()
{
    // Destroy group bridge
    auto groupJsBridge = frontendDelegate_->GetGroupJsBridge();
    if (groupJsBridge != nullptr) {
        groupJsBridge->Destroy();
    }

    if (runningPage_) {
        runningPage_->OnJsEngineDestroy();
    }

    if (stagingPage_) {
        stagingPage_->OnJsEngineDestroy();
    }

    if (runtime_) {
        runtime_->RegisterUncaughtExceptionHandler(nullptr);
        runtime_->Reset();
    }
    runtime_.reset();
    runtime_ = nullptr;

}

void JsiEngineInstance::FlushCommandBuffer(void* context, const std::string& command)
{
    // These js code is put into jsfwk, No need to do any thing here.
    return;
}

shared_ptr<JsRuntime> JsiEngineInstance::GetJsRuntime() const
{
    return runtime_;
}

RefPtr<JsAcePage> JsiEngineInstance::GetRunningPage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return runningPage_;
}

RefPtr<JsAcePage> JsiEngineInstance::GetStagingPage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return stagingPage_;
}

WeakPtr<JsMessageDispatcher> JsiEngineInstance::GetJsMessageDispatcher() const
{
    return dispatcher_;
}

RefPtr<FrontendDelegate> JsiEngineInstance::GetFrontendDelegate() const
{
    return frontendDelegate_;
}

void JsiEngineInstance::SetRunningPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    runningPage_ = page;
}

void JsiEngineInstance::SetStagingPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    stagingPage_ = page;
}

void JsiEngineInstance::ResetStagingPage(const RefPtr<JsAcePage>& page)
{
    std::lock_guard<std::mutex> lock(mutex_);
    stagingPage_ = page;
}

void JsiEngineInstance::SetJsMessageDispatcher(const WeakPtr<JsMessageDispatcher>& dispatcher)
{
    dispatcher_ = dispatcher;
}

void JsiEngineInstance::RegisterAceModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterAceModule");
    LOGD("JsiEngineInstance RegisterAceModule");

    shared_ptr<JsValue> aceObj = runtime_->NewObject();
    if (!aceObj) {
        LOGE("RegisterAceModule failed. aceObj is null");
        return;
    }
    if (!aceObj->SetProperty(runtime_, "domCreateBody", runtime_->NewFunction(JsDomCreateBody))) {
        LOGE("RegisterAceModule domCreateBody failed.");
    }
    if (!aceObj->SetProperty(runtime_, "domAddElement", runtime_->NewFunction(JsDomAddElement))) {
        LOGE("RegisterAceModule domAddElement failed.");
    }
    if (!aceObj->SetProperty(runtime_, "removeElement", runtime_->NewFunction(JsRemoveElement))) {
        LOGE("RegisterAceModule removeElement failed.");
    }
    if (!aceObj->SetProperty(runtime_, "updateElementAttrs", runtime_->NewFunction(JsUpdateElementAttrs))) {
        LOGE("RegisterAceModule updateElementAttrs failed.");
    }
    if (!aceObj->SetProperty(runtime_, "updateElementStyles", runtime_->NewFunction(JsUpdateElementStyles))) {
        LOGE("RegisterAceModule updateElementStyles failed.");
    }
    if (!aceObj->SetProperty(runtime_, "onCreateFinish", runtime_->NewFunction(JsOnCreateFinish))) {
        LOGE("RegisterAceModule onCreateFinish failed.");
    }
    if (!aceObj->SetProperty(runtime_, "onUpdateFinish", runtime_->NewFunction(JsOnUpdateFinish))) {
        LOGE("RegisterAceModule onUpdateFinish failed.");
    }
    if (!aceObj->SetProperty(runtime_, "callNative", runtime_->NewFunction(JsCallNative))) {
        LOGE("RegisterAceModule callNative failed.");
    }
    if (!aceObj->SetProperty(runtime_, "callComponent", runtime_->NewFunction(JsCallComponent))) {
        LOGE("RegisterAceModule callComponent failed.");
    }

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    if (!global->SetProperty(runtime_, "ace", aceObj)) {
        LOGE("RegisterAceModule ace failed.");
    }
}

shared_ptr<JsValue> JsCallNativeHandler(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 2 || !argv[0]->IsString(runtime) || !argv[1]->IsString(runtime)) {
        LOGE("JsCallNativeHandler: invalid parameters");
        return runtime->NewNull();
    }

    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (engine == nullptr) {
        return runtime->NewNull();
    }

    std::string event = argv[0]->ToString(runtime);
    std::string params = argv[1]->ToString(runtime);
    engine->GetDelegate()->CallNativeHandler(event, params);
    return runtime->NewNull();
}

void JsiEngineInstance::RegisterConsoleModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterConsoleModule");
    LOGD("JsiEngineInstance RegisterConsoleModule");
    shared_ptr<JsValue> global = runtime_->GetGlobal();

    // app log method
    shared_ptr<JsValue> consoleObj = runtime_->NewObject();
    consoleObj->SetProperty(runtime_, "log", runtime_->NewFunction(JsiBaseUtils::AppDebugLogPrint));
    consoleObj->SetProperty(runtime_, "debug", runtime_->NewFunction(JsiBaseUtils::AppDebugLogPrint));
    consoleObj->SetProperty(runtime_, "info", runtime_->NewFunction(JsiBaseUtils::AppInfoLogPrint));
    consoleObj->SetProperty(runtime_, "warn", runtime_->NewFunction(JsiBaseUtils::AppWarnLogPrint));
    consoleObj->SetProperty(runtime_, "error", runtime_->NewFunction(JsiBaseUtils::AppErrorLogPrint));
    global->SetProperty(runtime_, "console", consoleObj);

    // js framework log method
    shared_ptr<JsValue> aceConsoleObj = runtime_->NewObject();
    aceConsoleObj->SetProperty(runtime_, "log", runtime_->NewFunction(JsiBaseUtils::JsDebugLogPrint));
    aceConsoleObj->SetProperty(runtime_, "debug", runtime_->NewFunction(JsiBaseUtils::JsDebugLogPrint));
    aceConsoleObj->SetProperty(runtime_, "info", runtime_->NewFunction(JsiBaseUtils::JsInfoLogPrint));
    aceConsoleObj->SetProperty(runtime_, "warn", runtime_->NewFunction(JsiBaseUtils::JsWarnLogPrint));
    aceConsoleObj->SetProperty(runtime_, "error", runtime_->NewFunction(JsiBaseUtils::JsErrorLogPrint));
    global->SetProperty(runtime_, "aceConsole", aceConsoleObj);
    global->SetProperty(runtime_, "callNativeHandler", runtime_->NewFunction(JsCallNativeHandler));
}

void JsiEngineInstance::RegisterConsoleModule(ArkNativeEngine* engine)
{
    ACE_SCOPED_TRACE("JsiEngineInstance::RegisterConsoleModule");
    LOGD("JsiEngineInstance RegisterConsoleModule to nativeEngine");
    NativeValue* global = engine->GetGlobal();
    if (global->TypeOf() != NATIVE_OBJECT) {
        LOGE("global is not NativeObject");
        return;
    }
    auto nativeGlobal = reinterpret_cast<NativeObject*>(global->GetInterface(NativeObject::INTERFACE_ID));

    // app log method
    NativeValue* console = engine->CreateObject();
    auto consoleObj = reinterpret_cast<NativeObject*>(console->GetInterface(NativeObject::INTERFACE_ID));
    consoleObj->SetProperty("log", engine->CreateFunction("log", strlen("log"), AppDebugLogPrint, nullptr));
    consoleObj->SetProperty("debug", engine->CreateFunction("debug", strlen("debug"), AppDebugLogPrint, nullptr));
    consoleObj->SetProperty("info", engine->CreateFunction("info", strlen("info"), AppInfoLogPrint, nullptr));
    consoleObj->SetProperty("warn", engine->CreateFunction("warn", strlen("warn"), AppWarnLogPrint, nullptr));
    consoleObj->SetProperty("error", engine->CreateFunction("error", strlen("error"), AppErrorLogPrint, nullptr));
    nativeGlobal->SetProperty("console", console);
}

shared_ptr<JsValue> SyscapCanIUse(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("argc should be 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsString(runtime)) {
        LOGW("argv[0] is not IsString");
        return runtime->NewNull();
    }

    std::string syscapString = argv[0]->ToString(runtime);
    bool ret = Ace::SystemProperties::IsSyscapExist(syscapString.c_str());
    return runtime->NewBoolean(ret);
}

void JsiEngineInstance::RegisterSyscapModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterSyscapModule");
    LOGD("JsiEngineInstance RegisterSyscapModule");
    shared_ptr<JsValue> global = runtime_->GetGlobal();

    global->SetProperty(runtime_, CAN_IUSE, runtime_->NewFunction(SyscapCanIUse));
}

void JsiEngineInstance::RegisterDocumentModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterDocumentModule");
    LOGD("JsiEngineInstance RegisterDocumentModule");
    shared_ptr<JsValue> global = runtime_->GetGlobal();
    shared_ptr<JsValue> domObj = runtime_->NewObject();
    domObj->SetProperty(runtime_, "createElement", runtime_->NewFunction(JsCreateElement));
    global->SetProperty(runtime_, "dom", domObj);
}

void JsiEngineInstance::RegisterPerfUtilModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterPerfUtilModule");
    LOGD("JsiEngineInstance RegisterPerfUtilModule");
    shared_ptr<JsValue> perfObj = runtime_->NewObject();
    perfObj->SetProperty(runtime_, "printlog", runtime_->NewFunction(JsPerfPrint));
    perfObj->SetProperty(runtime_, "sleep", runtime_->NewFunction(JsPerfSleep));
    perfObj->SetProperty(runtime_, "begin", runtime_->NewFunction(JsPerfBegin));
    perfObj->SetProperty(runtime_, "end", runtime_->NewFunction(JsPerfEnd));

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    global->SetProperty(runtime_, "perfutil", perfObj);
}

void JsiEngineInstance::RegisterHiViewModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterHiViewModule");
    LOGD("JsiEngineInstance RegisterHiViewModule");
    shared_ptr<JsValue> hiViewObj = runtime_->NewObject();
    hiViewObj->SetProperty(runtime_, "report", runtime_->NewFunction(JsHiViewReport));

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    global->SetProperty(runtime_, "hiView", hiViewObj);
}

void JsiEngineInstance::RegisterI18nPluralRulesModule()
{
    ACE_SCOPED_TRACE("JsiEngine::RegisterI18nPluralRulesModule");
    LOGD("JsiEngineInstance RegisterI18nPluralRulesModule");
    shared_ptr<JsValue> i18nObj = runtime_->NewObject();
    i18nObj->SetProperty(runtime_, "select", runtime_->NewFunction(JsPluralRulesFormat));

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    global->SetProperty(runtime_, "i18nPluralRules", i18nObj);
}

bool JsiEngineInstance::InitJsEnv(bool debugger_mode, const std::unordered_map<std::string, void*>& extraNativeObject)
{
    ACE_SCOPED_TRACE("JsiEngine::InitJsEnv");
    runtime_.reset(new ArkJSRuntime());
    if (runtime_ == nullptr) {
        LOGE("Js Engine cannot allocate JSI JSRuntime");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
        return false;
    }

    runtime_->SetLogPrint(PrintLog);
    std::string library_path = "";
    if (debugger_mode) {
        library_path = ARK_DEBUGGER_LIB_PATH;
    }
    if (!runtime_->Initialize(library_path, isDebugMode_)) {
        LOGE("Js Engine initialize runtime failed");
        return false;
    }

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    for (const auto& [key, value] : extraNativeObject) {
        shared_ptr<JsValue> nativeValue = runtime_->NewNativePointer(value);
        runtime_->GetGlobal()->SetProperty(runtime_, key, nativeValue);
    }
#endif

    RegisterAceModule();
    RegisterConsoleModule();
    RegisterSyscapModule();
    RegisterDocumentModule();
    RegisterPerfUtilModule();
    RegisterHiViewModule();
    RegisterI18nPluralRulesModule();

    // load jsfwk
    if (!runtime_->ExecuteJsBin("/system/etc/strip.native.min.abc")) {
        LOGE("Failed to load js framework!");
        return false;
    }
    LOGD("Load js framework success");

    // Init groupJsBridge
    InitGroupJsBridge();

    runtime_->SetEmbedderData(this);
    runtime_->RegisterUncaughtExceptionHandler(JsiBaseUtils::ReportJsErrorEvent);
    return true;
}

void JsiEngineInstance::InitGroupJsBridge()
{
    auto groupJsBridge = DynamicCast<JsiGroupJsBridge>(frontendDelegate_->GetGroupJsBridge());
    if (groupJsBridge == nullptr || groupJsBridge->InitializeGroupJsBridge(runtime_) == JS_CALL_FAIL) {
        LOGE("Js Engine Initialize GroupJsBridge failed!");
        EventReport::SendJsException(JsExcepType::JS_ENGINE_INIT_ERR);
    }
}

bool JsiEngineInstance::FireJsEvent(const std::string& eventStr)
{
    LOGI("JsiEngineInstance FireJsEvent");
    if (!runningPage_) {
        LOGW("js engine instance running page is not valid.");
        return false;
    }
    std::vector<shared_ptr<JsValue>> argv;
    argv.push_back(runtime_->NewString(std::to_string(runningPage_->GetPageId())));
    shared_ptr<JsValue> var1 = runtime_->ParseJson(eventStr);
    argv.push_back(var1);

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    shared_ptr<JsValue> func = global->GetProperty(runtime_, "callJS");

    if (!func->IsFunction(runtime_)) {
        LOGE("\"callJs\" is not a function!");
        return false;
    }
    func->Call(runtime_, global, argv, argv.size());
    return true;
}

void JsiEngineInstance::CallJs(const std::string& callbackId, const std::string& args, bool keepAlive, bool isGlobal)
{
    LOGI("JsiEngineInstance CallJs");
    std::string keepAliveStr = keepAlive ? "true" : "false";
    std::string callBuff = std::string("[{\"args\": [\"")
                               .append(callbackId)
                               .append("\",")
                               .append(args)
                               .append(",")
                               .append(keepAliveStr)
                               .append("], \"method\":\"callback\"}]");
    LOGD("CallJs string: %{private}s", callBuff.c_str());
    int32_t instanceId = isGlobal ? DEFAULT_APP_ID : runningPage_->GetPageId();

    std::vector<shared_ptr<JsValue>> argv;
    argv.push_back(runtime_->NewString(std::to_string(instanceId)));
    argv.push_back(runtime_->ParseJson(callBuff));

    shared_ptr<JsValue> global = runtime_->GetGlobal();
    shared_ptr<JsValue> func = global->GetProperty(runtime_, "callJS");
    if (!func->IsFunction(runtime_)) {
        LOGE("\"callJs\" is not a function!");
        return;
    }
    func->Call(runtime_, global, argv, argv.size());
}

// -----------------------
// Start JsiEngine
// -----------------------
bool JsiEngine::Initialize(const RefPtr<FrontendDelegate>& delegate)
{
    ACE_SCOPED_TRACE("JsiEngine::Initialize");
    LOGD("JsiEngine initialize");
    engineInstance_ = AceType::MakeRefPtr<JsiEngineInstance>(delegate, instanceId_);
    engineInstance_->SetDebugMode(NeedDebugBreakPoint());
    bool result = engineInstance_->InitJsEnv(IsDebugVersion(), GetExtraNativeObject());
    if (!result) {
        LOGE("JsiEngine Initialize, init js env failed");
        return false;
    }

    auto runtime = engineInstance_->GetJsRuntime();
    auto vm = std::static_pointer_cast<ArkJSRuntime>(runtime)->GetEcmaVm();
    if (vm == nullptr) {
        LOGE("JsiEngine Initialize, vm is null");
        return false;
    }

    auto nativeEngine = new ArkNativeEngine(const_cast<EcmaVM*>(vm), static_cast<void*>(this));
    nativeEngine_ = nativeEngine;
    engineInstance_->SetNativeEngine(nativeEngine_);
    SetPostTask(nativeEngine_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    nativeEngine_->CheckUVLoop();
#endif

    ACE_DCHECK(delegate);
    if (delegate && delegate->GetAssetManager()) {
        std::string packagePath = delegate->GetAssetManager()->GetLibPath();
        if (!packagePath.empty()) {
            nativeEngine->SetPackagePath(packagePath);
        }
    }
    RegisterWorker();

    return true;
}

void JsiEngine::SetPostTask(NativeEngine* nativeEngine)
{
    LOGI("SetPostTask");
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& postTask = [weakDelegate, weakEngine = AceType::WeakClaim(this), id = instanceId_](bool needSync) {
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }

        delegate->PostJsTask([weakEngine, needSync, id]() {
            auto jsEngine = weakEngine.Upgrade();
            if (jsEngine == nullptr) {
                LOGW("jsEngine is nullptr");
                return;
            }
            auto nativeEngine = jsEngine->GetNativeEngine();
            if (nativeEngine == nullptr) {
                return;
            }
            ContainerScope scope(id);
            nativeEngine->Loop(LOOP_NOWAIT, needSync);
        });
    };
    nativeEngine_->SetPostTask(postTask);
}

void JsiEngine::RegisterInitWorkerFunc()
{
    auto weakInstance = AceType::WeakClaim(AceType::RawPtr(engineInstance_));
    auto&& initWorkerFunc = [weakInstance](NativeEngine* nativeEngine) {
        LOGI("WorkerCore RegisterInitWorkerFunc called");
        if (nativeEngine == nullptr) {
            LOGE("nativeEngine is nullptr");
            return;
        }
        auto arkNativeEngine = static_cast<ArkNativeEngine*>(nativeEngine);
        if (arkNativeEngine == nullptr) {
            LOGE("arkNativeEngine is nullptr");
            return;
        }
        auto instance = weakInstance.Upgrade();
        if (instance == nullptr) {
            LOGE("instance is nullptr");
            return;
        }
        instance->RegisterConsoleModule(arkNativeEngine);
        // load jsfwk
        if (!arkNativeEngine->ExecuteJsBin("/system/etc/strip.native.min.abc")) {
            LOGE("Failed to load js framework!");
        }
    };
    nativeEngine_->SetInitWorkerFunc(initWorkerFunc);
}

void JsiEngine::RegisterAssetFunc()
{
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(engineInstance_->GetDelegate()));
    auto&& assetFunc = [weakDelegate](const std::string& uri, std::vector<uint8_t>& content, std::string& ami) {
        LOGI("WorkerCore RegisterAssetFunc called");
        auto delegate = weakDelegate.Upgrade();
        if (delegate == nullptr) {
            LOGE("delegate is nullptr");
            return;
        }
        size_t index = uri.find_last_of(".");
        if (index == std::string::npos) {
            LOGE("invalid uri");
        } else {
            delegate->GetResourceData(uri.substr(0, index) + ".abc", content, ami);
        }
    };
    nativeEngine_->SetGetAssetFunc(assetFunc);
}

void JsiEngine::RegisterWorker()
{
    RegisterInitWorkerFunc();
    RegisterAssetFunc();
}

JsiEngine::~JsiEngine()
{
    LOG_DESTROY();
    if (nativeEngine_ != nullptr) {
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        nativeEngine_->CancelCheckUVLoop();
#endif
        delete nativeEngine_;
        nativeEngine_ = nullptr;
    }
    if (engineInstance_) {
        engineInstance_->SetNativeEngine(nullptr);
    }
}

void JsiEngine::GetLoadOptions(std::string& optionStr, bool isMainPage, bool hasAppCode)
{
    LOGD("JsiEngine GetLoadOptions");
    ACE_DCHECK(engineInstance_);
    auto delegate = engineInstance_->GetFrontendDelegate();
    if (!delegate) {
        LOGW("GetLoadOptions error: delegate is null");
        return;
    }
    auto mediaQuery = delegate->GetMediaQueryInfoInstance();
    auto renderOption = MediaQueryInfo::GetMediaQueryJsonInfo();
    if (mediaQuery) {
        renderOption->Put("isInit", mediaQuery->GetIsInit());
    }
    renderOption->Put("pcPreview", PC_PREVIEW);
    renderOption->Put("appInstanceId", "10002");
    renderOption->Put("packageName", delegate->GetAppID().c_str());

    // get resoureConfig
    delegate->GetResourceConfiguration(renderOption);

    // get i18n message
    delegate->GetI18nData(renderOption);
    std::string language = AceApplicationInfo::GetInstance().GetLanguage();
    std::string region = AceApplicationInfo::GetInstance().GetCountryOrRegion();
    std::string local = language + "_" + region;
    renderOption->Put("language", local.c_str());

    if (isMainPage && hasAppCode) {
        renderOption->Put("appCreate", "true");
    } else {
        renderOption->Put("appCreate", "false");
    }
    optionStr = renderOption->ToString();
}

void JsiEngine::LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage)
{
    ACE_SCOPED_TRACE("JsiEngine::LoadJs");
    LOGD("JsiEngine LoadJs");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
    if (isMainPage) {
        ACE_DCHECK(!engineInstance_->GetRunningPage());
        engineInstance_->SetRunningPage(page);
    }

    auto runtime = engineInstance_->GetJsRuntime();
    auto delegate = engineInstance_->GetFrontendDelegate();

    // get source map
    std::string jsSourceMap;
    if (delegate->GetAssetContent(url + ".map", jsSourceMap)) {
        page->SetPageMap(jsSourceMap);
    } else {
        LOGI("js source map load failed!");
    }
    // get js bundle content
    shared_ptr<JsValue> jsCode = runtime->NewUndefined();
    shared_ptr<JsValue> jsAppCode = runtime->NewUndefined();
    const char js_ext[] = ".js";
    const char bin_ext[] = ".abc";
    auto pos = url.rfind(js_ext);
    if (pos != std::string::npos && pos == url.length() - (sizeof(js_ext) - 1)) {
        std::string urlName = url.substr(0, pos) + bin_ext;
        std::string assetBasePath = delegate->GetAssetPath(urlName);
        std::string assetPath = assetBasePath.append(urlName);
        LOGD("assetPath is: %{private}s", assetPath.c_str());

        if (isMainPage) {
            std::string commonsBasePath = delegate->GetAssetPath("commons.abc");
            if (!commonsBasePath.empty()) {
                std::string commonsPath = commonsBasePath.append("commons.abc");
                LOGD("commonsPath is: %{private}s", commonsPath.c_str());
                if (!runtime->ExecuteJsBin(commonsPath)) {
                    LOGE("ExecuteJsBin \"commons.js\" failed.");
                    return;
                }
            }
            std::string vendorsBasePath = delegate->GetAssetPath("vendors.abc");
            if (!vendorsBasePath.empty()) {
                std::string vendorsPath = vendorsBasePath.append("vendors.abc");
                LOGD("vendorsPath is: %{private}s", vendorsPath.c_str());
                if (!runtime->ExecuteJsBin(vendorsPath)) {
                    LOGE("ExecuteJsBin \"vendors.js\" failed.");
                    return;
                }
            }
            std::string appMap;
            if (delegate->GetAssetContent("app.js.map", appMap)) {
                page->SetAppMap(appMap);
            } else {
                LOGI("app map load failed!");
            }
            std::string appBasePath = delegate->GetAssetPath("app.abc");
            std::string appPath = appBasePath.append("app.abc");
            LOGD("appPath is: %{private}s", appPath.c_str());
            if (!runtime->ExecuteJsBin(appPath)) {
                LOGE("ExecuteJsBin \"app.js\" failed.");
                return;
            }
            jsAppCode = runtime->GetGlobal()->GetProperty(runtime, "___mainEntry___");
            runtime->GetGlobal()->SetProperty(runtime, "___mainEntry___", runtime->NewUndefined());
            if (!jsAppCode->IsFunction(runtime)) {
                LOGE("appJsCode is not a function");
                return;
            }
        }
        if (!runtime->ExecuteJsBin(assetPath)) {
            LOGE("ExecuteJsBin %{private}s failed.", urlName.c_str());
            return;
        }

        jsCode = runtime->GetGlobal()->GetProperty(runtime, "___mainEntry___");
        runtime->GetGlobal()->SetProperty(runtime, "___mainEntry___", runtime->NewUndefined());
        if (!jsCode->IsFunction(runtime)) {
            LOGE("jsCode is not a function");
            return;
        }
    }

    // get page params
    std::string jsonData = page->GetPageParams();
    if (jsonData.empty()) {
        jsonData = "{}";
    }

    // get load options
    std::string optionStr;
    GetLoadOptions(optionStr, isMainPage, jsAppCode->IsFunction(runtime));
    shared_ptr<JsValue> instanceId = runtime->NewString(std::to_string(page->GetPageId()));
    shared_ptr<JsValue> renderOptions = runtime->ParseJson(optionStr);
    if (isMainPage && (!renderOptions || !renderOptions->SetProperty(runtime, "appCode", jsAppCode))) {
        LOGE("appCode property set failed.");
        return;
    }
    shared_ptr<JsValue> data = runtime->ParseJson(jsonData);
    shared_ptr<JsValue> info = runtime->NewObject();
    const std::vector<shared_ptr<JsValue>>& argv = { instanceId, jsCode, renderOptions, data, info };

    shared_ptr<JsValue> global = runtime->GetGlobal();
    shared_ptr<JsValue> func = global->GetProperty(runtime, "createInstance");

    if (!func) {
        LOGE("\"createInstance\" not found");
        return;
    }
    if (!func->IsFunction(runtime)) {
        LOGE("\"createInstance\" is not a function");
        return;
    }

    func->Call(runtime, global, argv, argv.size());
}

// Update running page
void JsiEngine::UpdateRunningPage(const RefPtr<JsAcePage>& page)
{
    LOGD("JsiEngine UpdateRunningPage");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetRunningPage(page);
}

// Update staging page
void JsiEngine::UpdateStagingPage(const RefPtr<JsAcePage>& page)
{
    LOGD("JsiEngine UpdateStagingPage");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetStagingPage(page);
}

// Reset loading page
void JsiEngine::ResetStagingPage()
{
    LOGD("JsiEngine ResetStagingPage");
    ACE_DCHECK(engineInstance_);
    // weird
    auto runningPage = engineInstance_->GetRunningPage();
    engineInstance_->ResetStagingPage(runningPage);
}

void JsiEngine::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
{
    LOGD("JsiEngine SetJsMessageDispatcher");
    ACE_DCHECK(engineInstance_);
    engineInstance_->SetJsMessageDispatcher(dispatcher);
}

void JsiEngine::FireAsyncEvent(const std::string& eventId, const std::string& param)
{
    LOGD("JsiEngine FireAsyncEvent");
    ACE_DCHECK(engineInstance_);

    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEvent\"}]");
    LOGD("FireASyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    if (!engineInstance_->FireJsEvent(callBuf.c_str())) {
        LOGE("Js Engine FireSyncEvent FAILED!");
    }
}

void JsiEngine::FireSyncEvent(const std::string& eventId, const std::string& param)
{
    LOGD("JsiEngine FireSyncEvent");
    std::string callBuf = std::string("[{\"args\": [\"")
                              .append(eventId)
                              .append("\",")
                              .append(param)
                              .append("], \"method\":\"fireEventSync\"}]");
    LOGD("FireSyncEvent string: %{private}s", callBuf.c_str());

    ACE_DCHECK(engineInstance_);
    if (!engineInstance_->FireJsEvent(callBuf.c_str())) {
        LOGE("Js Engine FireSyncEvent FAILED!");
    }
}

void JsiEngine::FireExternalEvent(const std::string& componentId, const uint32_t nodeId)
{
    ACE_DCHECK(engineInstance_);
    auto runtime = engineInstance_->GetJsRuntime();
    auto page = GetRunningPage(runtime);
    if (page == nullptr) {
        LOGE("FireExternalEvent GetRunningPage is nullptr");
        return;
    }
    std::string arguments;
    auto bridge = AceType::DynamicCast<JsiXComponentBridge>(page->GetXComponentBridgeById(nodeId));
    if (bridge) {
        bridge->HandleContext(runtime, nodeId, arguments);
        return;
    }
}

// Destroy page instance on Js
void JsiEngine::DestroyPageInstance(int32_t pageId)
{
    LOGI("JsiEngine DestroyPageInstance");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    const std::vector<shared_ptr<JsValue>>& argv = { runtime->NewString(std::to_string(pageId)) };

    shared_ptr<JsValue> global = runtime->GetGlobal();
    shared_ptr<JsValue> func = global->GetProperty(runtime, "destroyInstance");
    if (!func || !func->IsFunction(runtime)) {
        LOGE("\"destroyInstance\" not found or is not a function!");
        return;
    }
    func->Call(runtime, global, argv, argv.size());

    RunGarbageCollection();
}

// destroy application instance according to packageName
void JsiEngine::DestroyApplication(const std::string& packageName)
{
    LOGI("JsiEngine DestroyApplication");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    const std::vector<shared_ptr<JsValue>>& argv = { runtime->NewString(packageName) };

    shared_ptr<JsValue> global = runtime->GetGlobal();
    shared_ptr<JsValue> func = global->GetProperty(runtime, "appDestroy");
    if (!func || !func->IsFunction(runtime)) {
        LOGE("\"appDestroy\" not found or is not a function!");
        return;
    }
    func->Call(runtime, global, argv, argv.size());
}

void JsiEngine::TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval)
{
    if (isInterval) {
        engineInstance_->CallJs(callbackId, std::string("{}"), true, true);
        engineInstance_->GetFrontendDelegate()->WaitTimer(callbackId, delay, isInterval, false);
    } else {
        engineInstance_->CallJs(callbackId, std::string("{}"), false, true);
        engineInstance_->GetFrontendDelegate()->ClearTimer(callbackId);
    }
}

void JsiEngine::MediaQueryCallback(const std::string& callbackId, const std::string& args)
{
    if (!callbackId.empty() && engineInstance_) {
        engineInstance_->CallJs(callbackId, args, true, false);
    }
}

void JsiEngine::RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp)
{
    if (engineInstance_) {
        engineInstance_->CallJs(callbackId, std::to_string(timeStamp), false, true);
        engineInstance_->GetFrontendDelegate()->CancelAnimationFrame(callbackId);
    }
}

void JsiEngine::JsCallback(const std::string& callbackId, const std::string& args)
{
    if (engineInstance_) {
        engineInstance_->CallJs(callbackId, args, true, false);
    }
}

void JsiEngine::RunGarbageCollection()
{
    if (engineInstance_ && engineInstance_->GetJsRuntime()) {
        engineInstance_->GetJsRuntime()->RunGC();
    }
}

std::string JsiEngine::GetStacktraceMessage()
{
    auto arkNativeEngine = static_cast<ArkNativeEngine*>(nativeEngine_);
    if (!arkNativeEngine) {
        LOGE("GetStacktraceMessage arkNativeEngine is nullptr");
        return "";
    }
    std::string stack;
    arkNativeEngine->SuspendVM();
    bool getStackSuccess = arkNativeEngine->BuildNativeAndJsBackStackTrace(stack);
    arkNativeEngine->ResumeVM();
    if (!getStackSuccess) {
        LOGE("GetStacktraceMessage arkNativeEngine get stack failed");
        return "";
    }

    auto runningPage = engineInstance_ ? engineInstance_->GetRunningPage() : nullptr;
    return JsiBaseUtils::TransSourceStack(runningPage, stack);
}

RefPtr<GroupJsBridge> JsiEngine::GetGroupJsBridge()
{
    return AceType::MakeRefPtr<JsiGroupJsBridge>();
}

bool JsiEngine::OnStartContinuation()
{
    LOGI("JsiEngine OnStartContinuation");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    if (!runtime) {
        LOGE("OnStartContinuation failed, runtime is null.");
        return false;
    }

    return CallAppFunc("onStartContinuation");
}

void JsiEngine::OnCompleteContinuation(int32_t code)
{
    LOGI("JsiEngine OnCompleteContinuation");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    if (!runtime) {
        LOGE("OnCompleteContinuation failed, runtime is null.");
        return;
    }

    std::vector<shared_ptr<JsValue>> argv = { runtime->NewNumber(code) };
    CallAppFunc("onCompleteContinuation", argv);
}

void JsiEngine::OnRemoteTerminated()
{
    LOGI("JsiEngine OnRemoteTerminated");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    if (!runtime) {
        LOGE("OnRemoteTerminated failed, runtime is null.");
        return;
    }

    CallAppFunc("onRemoteTerminated");
}

void JsiEngine::OnSaveData(std::string& data)
{
    LOGI("JsiEngine OnSaveData");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    if (!runtime) {
        LOGE("OnSaveData failed, runtime is null.");
        return;
    }

    shared_ptr<JsValue> object = runtime->NewObject();
    std::vector<shared_ptr<JsValue>> argv = { object };
    if (CallAppFunc("onSaveData", argv)) {
        data = object->GetJsonString(runtime);
    }
}

bool JsiEngine::OnRestoreData(const std::string& data)
{
    LOGI("JsiEngine OnRestoreData");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    if (!runtime) {
        LOGE("OnRestoreData failed, runtime is null.");
        return false;
    }
    shared_ptr<JsValue> result;
    shared_ptr<JsValue> jsonObj = runtime->ParseJson(data);
    if (jsonObj->IsUndefined(runtime) || jsonObj->IsException(runtime)) {
        LOGE("JsiEngine: Parse json for restore data failed.");
        return false;
    }
    std::vector<shared_ptr<JsValue>> argv = { jsonObj };
    return CallAppFunc("onRestoreData", argv);
}

bool JsiEngine::CallAppFunc(const std::string& appFuncName)
{
    std::vector<shared_ptr<JsValue>> argv = {};
    return CallAppFunc(appFuncName, argv);
}

bool JsiEngine::CallAppFunc(const std::string& appFuncName, std::vector<shared_ptr<JsValue>>& argv)
{
    LOGD("JsiEngine CallAppFunc");
    shared_ptr<JsRuntime> runtime = engineInstance_->GetJsRuntime();
    ACE_DCHECK(runtime);
    shared_ptr<JsValue> global = runtime->GetGlobal();
    shared_ptr<JsValue> appObj = global->GetProperty(runtime, "aceapp");
    if (!appObj->IsObject(runtime)) {
        LOGE("property \"aceapp\" is not a object");
        return false;
    }
    shared_ptr<JsValue> defaultObject = appObj->GetProperty(runtime, "$def");
    if (!defaultObject->IsObject(runtime)) {
        LOGE("property \"$def\" is not a object");
        return false;
    }
    shared_ptr<JsValue> func = defaultObject->GetProperty(runtime, appFuncName);
    if (!func || !func->IsFunction(runtime)) {
        LOGE("%{public}s not found or is not a function!", appFuncName.c_str());
        return false;
    }
    shared_ptr<JsValue> result;
    result = func->Call(runtime, defaultObject, argv, argv.size());
    return (result->ToString(runtime) == "true");
}

} // namespace OHOS::Ace::Framework
