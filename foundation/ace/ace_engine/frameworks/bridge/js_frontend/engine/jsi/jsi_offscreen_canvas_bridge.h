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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_OFFSCREEN_CANVAS_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_OFFSCREEN_CANVAS_BRIDGE_H

#include <map>
#include <string>

#include "core/components/custom_paint/offscreen_canvas.h"
#include "frameworks/bridge/common/dom/dom_canvas.h"
#include "frameworks/bridge/js_frontend/engine/common/base_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

namespace OHOS::Ace::Framework {

class JsiOffscreenCanvasBridge : public BaseCanvasBridge {
    DECLARE_ACE_TYPE(JsiOffscreenCanvasBridge, BaseCanvasBridge)

public:
    JsiOffscreenCanvasBridge() = default;
    JsiOffscreenCanvasBridge(const RefPtr<PipelineContext>& pipeContext, int32_t width, int32_t height);
    ~JsiOffscreenCanvasBridge() override = default;

    shared_ptr<JsValue> GetBridge(const shared_ptr<JsRuntime>& runtime);

    int32_t GetBridgeId() const
    {
        return bridgeId_;
    }

    const RefPtr<OffscreenCanvas>& GetOffscreenCanvas() const
    {
        return offscreenCanvas_;
    }

    // method of OffscreenCanvas
    static shared_ptr<JsValue> JsGetContext(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTransferToImageBitmap(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsToDataURL(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    // method of OffscreenCanvasContext
    static shared_ptr<JsValue> JsCreateLinearGradient(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCreateRadialGradient(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCreatePattern(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAddColorStop(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFillRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsClearRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsStrokeRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFillText(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsStrokeText(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsMeasureText(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFill(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsClip(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCreateImageData(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPutImageData(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsGetImageData(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsDrawImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSave(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsRestore(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCreatePath2D(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsIsPointInStroke(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsIsPointInPath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsResetTransform(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    // priority of OffscreenCanvasContext
    static shared_ptr<JsValue> JsFillStyleGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFillStyleSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsStrokeStyleGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsStrokeStyleSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineCapGetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineCapSetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineJoinGetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineJoinSetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsMiterLimitGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsMiterLimitSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineWidthGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineWidthSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTextAlignGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTextAlignSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTextBaselineGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTextBaselineSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFontGetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFontSetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAlphaGetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsAlphaSetter(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCompositeOperationGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsCompositeOperationSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineDashOffsetGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineDashOffsetSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowBlurGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowBlurSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowColorGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowColorSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowOffsetXGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowOffsetXSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowOffsetYGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsShadowOffsetYSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSmoothingEnabledGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSmoothingEnabledSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSmoothingQualityGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSmoothingQualitySetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFilterParamGetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsFilterParamSetter(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsBeginPath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsArc(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsStroke(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsArcTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsMoveTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsClosePath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsRotate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsScale(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsLineTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsBezierCurveTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsEllipse(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSetTransform(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTransform(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsTranslate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsGetLineDash(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsSetLineDash(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    static shared_ptr<JsValue> JsPath2DAddPath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DSetTransform(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DMoveTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DLineTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DArc(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DArcTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DBezierCurveTo(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DEllipse(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);
    static shared_ptr<JsValue> JsPath2DClosePath(const shared_ptr<JsRuntime>& runtime,
        const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

private:
    static Gradient GetGradient(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value);
    static Pattern GetPattern(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value);
    static RefPtr<CanvasPath2D> GetPath2D(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value);
    static void ParseDomImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value, double& width,
        double& height, std::string& src);
    static void ParseImageData(const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
        int32_t argc, std::vector<std::string>& array, ImageData& imageData);
    static RefPtr<CanvasPath2D> JsMakePath2D(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
        const std::vector<shared_ptr<JsValue>>& argv, int32_t argc);

    static int32_t path2dCount_;
    static int32_t gradientCount_;
    static int32_t patternCount_;
    static int32_t globalBridgeId_;
    static std::unordered_map<int32_t, Gradient> gradientColors_;
    static std::unordered_map<int32_t, Pattern> pattern_;
    static std::unordered_map<int32_t, RefPtr<CanvasPath2D>> path2Ds_;

    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t bridgeId_ = 0;
    RefPtr<OffscreenCanvas> offscreenCanvas_ = nullptr;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_JSI_JSI_OFFSCREEN_CANVAS_BRIDGE_H