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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_OFFSCREEN_CANVAS_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_OFFSCREEN_CANVAS_BRIDGE_H

#include <cstdlib>
#include <map>
#include <string>

#include "third_party/v8/include/v8.h"

#include "core/components/custom_paint/offscreen_canvas.h"
#include "frameworks/bridge/common/dom/dom_canvas.h"
#include "frameworks/bridge/js_frontend/engine/common/base_canvas_bridge.h"

namespace OHOS::Ace::Framework {

class V8OffscreenCanvasBridge : public BaseCanvasBridge {
    DECLARE_ACE_TYPE(V8OffscreenCanvasBridge, BaseCanvasBridge)

public:
    V8OffscreenCanvasBridge() = default;
    V8OffscreenCanvasBridge(const RefPtr<PipelineContext>& pipeContext, int32_t width, int32_t height);
    ~V8OffscreenCanvasBridge() override = default;

    void GetBridge(const v8::FunctionCallbackInfo<v8::Value>& args);

    int32_t GetBridgeId() const
    {
        return bridgeId_;
    }

    const RefPtr<OffscreenCanvas>& GetOffscreenCanvas() const
    {
        return offscreenCanvas_;
    }

    // method of OffscreenCanvas
    static void GetContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void TransferToImageBitmap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ToDataURL(const v8::FunctionCallbackInfo<v8::Value>& args);

    // method of OffscreenCanvasContext
    static void CreateLinearGradient(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreateRadialGradient(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreatePattern(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddColorStop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FillRect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ClearRect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void StrokeRect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FillText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void StrokeText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void MeasureText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void DrawRect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Fill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Clip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreateImageData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void PutImageData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetImageData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void DrawImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Save(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Restore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreatePath2D(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IsPointInStroke(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IsPointInPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ResetTransform(const v8::FunctionCallbackInfo<v8::Value>& args);

    // priority of OffscreenCanvasContext
    static void FillStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FillStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void StrokeStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void StrokeStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineCapGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineCapSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineJoinGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineJoinSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void MiterLimitGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void MiterLimitSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineWidthGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineWidthSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void TextAlignGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void TextAlignSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void TextBaselineGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void TextBaselineSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FontGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FontSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AlphaGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AlphaSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CompositeOperationGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CompositeOperationSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineDashOffsetGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineDashOffsetSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowBlurGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowBlurSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowColorGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowColorSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowOffsetXGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowOffsetXSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowOffsetYGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ShadowOffsetYSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SmoothingEnabledGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SmoothingEnabledSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SmoothingQualityGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SmoothingQualitySetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FilterParamGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void FilterParamSetter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void BeginPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Arc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Stroke(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ArcTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ClosePath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Scale(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LineTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Ellipse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetTransform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Transform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Translate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Path2DAddPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DSetTransform(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DMoveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DLineTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DArc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DArcTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DQuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DBezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DEllipse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DRect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Path2DClosePath(const v8::FunctionCallbackInfo<v8::Value>& args);
private:
    static Gradient GetGradient(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value);
    static Pattern GetPattern(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value);
    static void ParseDomImage(
        const v8::FunctionCallbackInfo<v8::Value>& args, double& width, double& height, std::string& src);
    static void ParseImageData(
        const v8::FunctionCallbackInfo<v8::Value>& args, std::vector<std::string>& array, ImageData& imageData);
    static RefPtr<CanvasPath2D> GetPath2D(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value);
    static RefPtr<CanvasPath2D> MakePath2D(const v8::FunctionCallbackInfo<v8::Value>& args);

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

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_OFFSCREEN_CANVAS_BRIDGE_H