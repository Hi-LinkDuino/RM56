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

#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_curves_module.h"

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/core/animation/curve.h"

namespace OHOS::Ace::Framework {

shared_ptr<JsValue> CurvesInterpolate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto jsPageId = thisObj->GetProperty(runtime, "__pageId");
    auto pageId = jsPageId->ToInt32(runtime);
    auto jsCurveString = thisObj->GetProperty(runtime, "__curveString");
    auto curveString = jsCurveString->ToString(runtime);

    double time = argv[0]->ToDouble(runtime);
    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        LOGE("get jsi engine instance failed");
        return runtime->NewNull();
    }
    auto delegate = instance->GetDelegate();
    if (delegate == nullptr) {
        LOGE("delegate is null.");
        return runtime->NewNull();
    }
    auto page = delegate->GetPage(pageId);
    if (page == nullptr) {
        LOGE("page is null.");
        return runtime->NewNull();
    }
    auto animationCurve = page->GetCurve(curveString);
    if (!animationCurve) {
        LOGE("animationCurve is null.");
        return runtime->NewNull();
    }
    double curveValue = animationCurve->Move(time);
    return runtime->NewNumber(curveValue);
}

shared_ptr<JsValue> CurvesInit(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto curveObj = runtime->NewObject();
    curveObj->SetProperty(runtime, CURVE_INTERPOLATE, runtime->NewFunction(CurvesInterpolate));
    if (argc != 1 && argc != 0) {
        LOGE("CurvesInit args count is invalid");
        return runtime->NewNull();
    }
    RefPtr<Curve> curve;
    std::string curveString;
    if (argc == 1) {
        curveString = argv[0]->ToString(runtime);
    } else {
        curveString = "linear";
    }
    curve = CreateCurve(curveString);
    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        LOGE("get jsi engine instance failed");
        return runtime->NewNull();
    }
    auto page = instance->GetStagingPage();
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewNull();
    }

    page->AddCurve(curveString, curve);
    int32_t pageId = page->GetPageId();
    curveObj->SetProperty(runtime, "__pageId", runtime->NewInt32(pageId));
    curveObj->SetProperty(runtime, "__curveString", runtime->NewString(curveString));
    return curveObj;
}

bool CreateSpringCurve(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, RefPtr<Curve>& curve)
{
    if (argc != 4) {
        LOGE("Spring curve: the number of parameters is illegal");
        return false;
    }
    double x0 = argv[0]->ToDouble(runtime);
    double y0 = argv[1]->ToDouble(runtime);
    double x1 = argv[2]->ToDouble(runtime);
    double y1 = argv[3]->ToDouble(runtime);
    
    curve = AceType::MakeRefPtr<SpringCurve>(x0, y0, x1, y1);
    return true;
}

bool CreateCubicCurve(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, RefPtr<Curve>& curve)
{
    if (argc != 4) {
        LOGE("Cubic curve: the number of parameters is illegal");
        return false;
    }
    double x0 = argv[0]->ToDouble(runtime);
    double y0 = argv[1]->ToDouble(runtime);
    double x1 = argv[2]->ToDouble(runtime);
    double y1 = argv[3]->ToDouble(runtime);

    curve = AceType::MakeRefPtr<CubicCurve>(x0, y0, x1, y1);
    return true;
}

bool CreateStepsCurve(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, RefPtr<Curve>& curve)
{
    if (argc != 1 && argc != 2) {
        LOGE("Steps curve: the number of parameters is illegal");
        return false;
    }
    int32_t stepSize;
    if (argc == 2) {
        stepSize = argv[0]->ToInt32(runtime);
        bool isEnd = argv[1]->ToBoolean(runtime);
        if (isEnd) {
            curve = AceType::MakeRefPtr<StepsCurve>(stepSize, StepsCurvePosition::START);
        } else {
            curve = AceType::MakeRefPtr<StepsCurve>(stepSize, StepsCurvePosition::END);
        }
    } else {
        stepSize = argv[0]->ToInt32(runtime);
        curve = AceType::MakeRefPtr<StepsCurve>(stepSize);
    }
    return true;
}

shared_ptr<JsValue> ParseCurves(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, std::string& curveString)
{
    auto curveObj = runtime->NewObject();
    curveObj->SetProperty(runtime, CURVE_INTERPOLATE, runtime->NewFunction(CurvesInterpolate));
    RefPtr<Curve> curve;
    bool curveCreated;
    if (curveString == "spring") {
        curveCreated = CreateSpringCurve(runtime, thisObj, argv, argc, curve);
    } else if (curveString == "cubic-bezier") {
        curveCreated = CreateCubicCurve(runtime, thisObj, argv, argc, curve);
    } else if (curveString == "steps") {
        curveCreated = CreateStepsCurve(runtime, thisObj, argv, argc, curve);
    } else {
        LOGE("curve params: %{public}s is illegal", curveString.c_str());
        return runtime->NewNull();
    }
    if (!curveCreated) {
        return runtime->NewNull();
    }
    auto customCurve = curve->ToString();
    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        LOGE("get jsi engine instance failed");
        return runtime->NewNull();
    }
    auto page = instance->GetStagingPage();
    if (page == nullptr) {
        LOGE("page is nullptr");
        return runtime->NewNull();
    }
    page->AddCurve(customCurve, curve);
    int32_t pageId = page->GetPageId();
    curveObj->SetProperty(runtime, "__pageId", runtime->NewInt32(pageId));
    curveObj->SetProperty(runtime, "__curveString", runtime->NewString(customCurve));
    return curveObj;
}

shared_ptr<JsValue> CurvesBezier(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    std::string curveString("cubic-bezier");
    return ParseCurves(runtime, thisObj, argv, argc, curveString);
}

shared_ptr<JsValue> CurvesSpring(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    std::string curveString("spring");
    return ParseCurves(runtime, thisObj, argv, argc, curveString);
}

shared_ptr<JsValue> CurvesSteps(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    std::string curveString("steps");
    return ParseCurves(runtime, thisObj, argv, argc, curveString);
}

void InitCurvesModule(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj)
{
    moduleObj->SetProperty(runtime, CURVES_INIT, runtime->NewFunction(CurvesInit));
    moduleObj->SetProperty(runtime, CURVES_CUBIC_BEZIER, runtime->NewFunction(CurvesBezier));
    moduleObj->SetProperty(runtime, CURVES_SPRING, runtime->NewFunction(CurvesSpring));
    moduleObj->SetProperty(runtime, CURVES_STEPS, runtime->NewFunction(CurvesSteps));
}

} // namespace OHOS::Ace::Framework