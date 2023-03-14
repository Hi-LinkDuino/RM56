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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_curves_module.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

std::string GetJsStringVal(JSContext* ctx, JSValueConst value)
{
    std::string val;
    if (JS_IsString(value)) {
        ScopedString curveJsonStr(ctx, value);
        val = curveJsonStr.get();
    }
    return val;
}

JSValue CurvesInterpolate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    QJSHandleScope handleScope(ctx);
    int32_t pageId;
    JS_ToInt32(ctx, &pageId, QJSUtils::GetPropertyStr(ctx, value, "__pageId"));
    std::string curveString = GetJsStringVal(ctx, QJSUtils::GetPropertyStr(ctx, value, "__curveString"));
    double time;
    JS_ToFloat64(ctx, &time, argv[0]);
    auto instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto delegate = instance->GetDelegate();
    if (delegate == nullptr) {
        LOGE("delegate is null.");
        return JS_NULL;
    }
    auto page = delegate->GetPage(pageId);
    if (page == nullptr) {
        LOGE("page is null.");
        return JS_NULL;
    }
    auto animationCurve = page->GetCurve(curveString);
    if (!animationCurve) {
        LOGE("animationCurve is null.");
        return JS_NULL;
    }
    double curveValue = animationCurve->Move(time);
    JSValue curveNum = JS_NewFloat64(ctx, curveValue);
    return curveNum;
}

JSValue CurvesInit(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    JS_SetPropertyStr(ctx, value, CURVE_INTERPOLATE, JS_NewCFunction(ctx, CurvesInterpolate, CURVE_INTERPOLATE, 1));
    if ((!argv) || ((argc != 1) && (argc != 0))) {
        LOGE("CurvesInit args count is invalid");
        return JS_NULL;
    }
    RefPtr<Curve> curve;
    std::string curveString;
    if (argc == 1) {
        ScopedString curveJsonStr(ctx, argv[0]);
        curveString = curveJsonStr.get();
    } else {
        curveString = "linear";
    }
    curve = CreateCurve(curveString);
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto page = instance->GetRunningPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    page->AddCurve(curveString, curve);
    int32_t pageId = page->GetPageId();
    JS_SetPropertyStr(ctx, value, "__pageId", JS_NewInt32(ctx, pageId));
    JS_SetPropertyStr(ctx, value, "__curveString", JS_NewString(ctx, curveString.c_str()));
    return value;
}

bool CreateSpringCurve(JSContext* ctx, JSValueConst& value, int32_t argc,
    JSValueConst* argv, RefPtr<Curve>& curve)
{
    if (argc != 4) {
        LOGE("Spring curve: the number of parameters is illegal");
        return false;
    }
    double x0;
    JS_ToFloat64(ctx, &x0, argv[0]);
    double y0;
    JS_ToFloat64(ctx, &y0, argv[1]);
    double x1;
    JS_ToFloat64(ctx, &x1, argv[2]);
    double y1;
    JS_ToFloat64(ctx, &y1, argv[3]);
    curve = AceType::MakeRefPtr<SpringCurve>(x0, y0, x1, y1);
    return true;
}

bool CreateCubicCurve(JSContext* ctx, JSValueConst& value, int32_t argc,
    JSValueConst* argv, RefPtr<Curve>& curve)
{
    if (argc != 4) {
        LOGE("Cubic curve: the number of parameters is illegal");
        return false;
    }
    double x0;
    JS_ToFloat64(ctx, &x0, argv[0]);
    double y0;
    JS_ToFloat64(ctx, &y0, argv[1]);
    double x1;
    JS_ToFloat64(ctx, &x1, argv[2]);
    double y1;
    JS_ToFloat64(ctx, &y1, argv[3]);
    curve = AceType::MakeRefPtr<CubicCurve>(x0, y0, x1, y1);
    return true;
}

bool CreateStepsCurve(JSContext* ctx, JSValueConst& value, int32_t argc,
    JSValueConst* argv, RefPtr<Curve>& curve)
{
    if (!argv || (argc != 1 && argc != 2)) {
        LOGE("Steps curve: the number of parameters is illegal");
        return false;
    }
    int32_t stepSize;
    if (argc == 2) {
        JS_ToInt32(ctx, &stepSize, argv[0]);
        bool isEnd = JS_ToBool(ctx, argv[1]);
        if (isEnd) {
            curve = AceType::MakeRefPtr<StepsCurve>(stepSize, StepsCurvePosition::END);
        } else {
            curve = AceType::MakeRefPtr<StepsCurve>(stepSize, StepsCurvePosition::START);
        }
    } else {
        JS_ToInt32(ctx, &stepSize, argv[0]);
        curve = AceType::MakeRefPtr<StepsCurve>(stepSize);
    }
    return true;
}

JSValue ParseCurves(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv, std::string& curveString)
{
    JS_SetPropertyStr(ctx, value, CURVE_INTERPOLATE, JS_NewCFunction(ctx, CurvesInterpolate, CURVE_INTERPOLATE, 1));
    RefPtr<Curve> curve;
    bool curveCreated;
    if (curveString == "spring") {
        curveCreated = CreateSpringCurve(ctx, value, argc, argv, curve);
    } else if (curveString == "cubic-bezier") {
        curveCreated = CreateCubicCurve(ctx, value, argc, argv, curve);
    } else if (curveString == "steps") {
        curveCreated = CreateStepsCurve(ctx, value, argc, argv, curve);
    } else {
        return JS_NULL;
    }
    if (!curveCreated) {
        return JS_NULL;
    }
    auto customCurve = curve->ToString();
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    auto page = instance->GetRunningPage(ctx);
    if (page == nullptr) {
        LOGE("page is nullptr");
        return JS_NULL;
    }
    page->AddCurve(customCurve, curve);
    int32_t pageId = page->GetPageId();
    JS_SetPropertyStr(ctx, value, "__pageId", JS_NewInt32(ctx, pageId));
    JS_SetPropertyStr(ctx, value, "__curveString", JS_NewString(ctx, customCurve.c_str()));
    JS_DupValue(ctx, value);
    return value;
}

JSValue CurvesBezier(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    std::string curveString("cubic-bezier");
    return ParseCurves(ctx, value, argc, argv, curveString);
}

JSValue CurvesSpring(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    std::string curveString("spring");
    return ParseCurves(ctx, value, argc, argv, curveString);
}

JSValue CurvesSteps(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    std::string curveString("steps");
    return ParseCurves(ctx, value, argc, argv, curveString);
}

void InitCurvesModule(JSContext* ctx, JSValue& moduleObj)
{
    JS_SetPropertyStr(ctx, moduleObj, CURVES_INIT, JS_NewCFunction(ctx, CurvesInit, CURVES_INIT, 1));
    JS_SetPropertyStr(ctx, moduleObj, CURVES_CUBIC_BEZIER, JS_NewCFunction(ctx, CurvesBezier, CURVES_CUBIC_BEZIER, 4));
    JS_SetPropertyStr(ctx, moduleObj, CURVES_SPRING, JS_NewCFunction(ctx, CurvesSpring, CURVES_SPRING, 4));
    JS_SetPropertyStr(ctx, moduleObj, CURVES_STEPS, JS_NewCFunction(ctx, CurvesSteps, CURVES_STEPS, 2));
}
} // namespace OHOS::Ace::Framework