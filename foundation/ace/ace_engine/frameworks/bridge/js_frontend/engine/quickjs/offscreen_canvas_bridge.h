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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_OFFSCREEN_CANVAS_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_OFFSCREEN_CANVAS_BRIDGE_H

#include <cstdlib>
#include <map>
#include <string>

#include "frameworks/bridge/common/dom/dom_canvas.h"
#include "frameworks/bridge/js_frontend/engine/common/base_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "core/components/custom_paint/offscreen_canvas.h"

namespace OHOS::Ace::Framework {
int32_t GetCurrentBridgeId(JSContext* ctx, JSValueConst value);

class OffscreenCanvasBridge : public BaseCanvasBridge {
    DECLARE_ACE_TYPE(OffscreenCanvasBridge, BaseCanvasBridge)

public:
    OffscreenCanvasBridge() = default;
    OffscreenCanvasBridge(JSContext* ctx, const int32_t width, const int32_t height);
    ~OffscreenCanvasBridge() override = default;

    int32_t bridgeId_;
    RefPtr<OffscreenCanvas> offscreenCanvas_;

    JSValue GetBridge(JSContext* ctx);

    // method of OffscreenCanvas
    static JSValue JsGetContext(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsTransferToImageBitmap(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsToDataURL(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);

    // method of OffscreenCanvasContext
    static JSValue JsCreateLinearGradient(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsCreateRadialGradient(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsCreatePattern(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsAddColorStop(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsFillRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsClearRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsStrokeRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsFillText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsStrokeText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsMeasureText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsFill(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsClip(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsCreateImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPutImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsGetImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsDrawImage(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsSave(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsRestore(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsCreatePath2D(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsIsPointInStroke(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsIsPointInPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsResetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);

    // priority of OffscreenCanvasContext
    static JSValue JsFillStyleGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsFillStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsStrokeStyleGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsStrokeStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsLineCapGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsLineCapSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsLineJoinGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsLineJoinSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsMiterLimitGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsMiterLimitSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsLineWidthGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsLineWidthSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsTextAlignGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsTextAlignSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsTextBaselineGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsTextBaselineSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsFontGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsFontSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsAlphaGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsAlphaSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsCompositeOperationGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsCompositeOperationSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsLineDashOffsetGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsLineDashOffsetSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsShadowBlurGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsShadowBlurSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsShadowColorGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsShadowColorSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsShadowOffsetXGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsShadowOffsetXSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsShadowOffsetYGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsShadowOffsetYSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsSmoothingEnabledGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsSmoothingEnabledSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsSmoothingQualityGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsSmoothingQualitySetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsFilterParamGetter(JSContext* ctx, JSValueConst value);
    static JSValue JsFilterParamSetter(JSContext* ctx, JSValueConst value, JSValueConst proto);
    static JSValue JsBeginPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsStroke(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static RefPtr<CanvasPath2D> GetPath2D(JSContext* ctx, JSValueConst value);
    static std::unordered_map<int32_t, RefPtr<CanvasPath2D>> path2Ds_;
    static JSValue JsArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsRotate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsScale(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsBezierCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsQuadraticCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsSetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsTranslate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsGetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsSetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);

    static JSValue JsPath2DAddPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DSetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DQuadraticCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DBezierCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static JSValue JsPath2DClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
    static RefPtr<CanvasPath2D> JsMakePath2D(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv);
private:
    enum class FunctionCode {
        FILL_RECT = 0,
        STROKE_RECT,
        CLEAR_RECT,
        FILL_TEXT,
        STROKE_TEXT,
        FILL_STYLE_SETTER,
        STROKE_STYLE_SETTER,
    };
    static Gradient GetGradient(JSContext* ctx, JSValueConst value);
    static Pattern GetPattern(JSContext* ctx, JSValueConst value);
    static void ParseDomImage(JSContext* ctx, JSValueConst value, double& width, double& height, std::string& src);
    static void ParseImageData(
        JSContext* ctx, JSValueConst* argv, int32_t argc, std::vector<std::string>& array, ImageData& imageData);
    int32_t width_ = 0;
    int32_t height_ = 0;
    static int32_t path2dCount_;
    static int32_t gradientCount_;
    static int32_t patternCount_;
    static int32_t globalBridgeId_;
    static std::unordered_map<int32_t, Gradient> gradientColors_;
    static std::unordered_map<int32_t, Pattern> pattern_;
    static JSValue JsHandleStyleSetter(
        JSContext* ctx, JSValueConst value, JSValueConst proto, FunctionCode functionCode);
};
} // namespace OHOS::Ace::Framework

#endif  // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_OFFSCREEN_CANVAS_BRIDGE_H