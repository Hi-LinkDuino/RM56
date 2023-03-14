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

#ifndef FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_CANVAS_RENDERER_H
#define FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_CANVAS_RENDERER_H

#include "base/memory/referenced.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas_gradient.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas_image_data.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas_path.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_matrix2d.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_path2d.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_render_image.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"
#include "frameworks/core/components/custom_paint/offscreen_canvas.h"

namespace OHOS::Ace::Framework {

class JSCanvasRenderer : public Referenced {
public:
    JSCanvasRenderer();
    ~JSCanvasRenderer() override = default;

    enum class FunctionCode {
        FILL_RECT = 0,
        STROKE_RECT,
        CLEAR_RECT,
        FILL_TEXT,
        STROKE_TEXT,
        FILL_STYLE_SETTER,
        STROKE_STYLE_SETTER,
    };

    static RefPtr<CanvasPath2D> JsMakePath2D(const JSCallbackInfo& info);
    void SetAntiAlias();

    void ParseImageData(const JSCallbackInfo& info, ImageData& imageData, std::vector<uint32_t>& array);

    void JsFillRect(const JSCallbackInfo& info);
    void JsStrokeRect(const JSCallbackInfo& info);
    void JsClearRect(const JSCallbackInfo& info);
    void JsFillText(const JSCallbackInfo& info);
    void JsStrokeText(const JSCallbackInfo& info);
    void JsMeasureText(const JSCallbackInfo& info);
    void JsMoveTo(const JSCallbackInfo& info);
    void JsLineTo(const JSCallbackInfo& info);
    void JsBezierCurveTo(const JSCallbackInfo& info);
    void JsQuadraticCurveTo(const JSCallbackInfo& info);
    void JsArcTo(const JSCallbackInfo& info);
    void JsArc(const JSCallbackInfo& info);
    void JsEllipse(const JSCallbackInfo& info);
    void JsFill(const JSCallbackInfo& info);
    void JsStroke(const JSCallbackInfo& info);
    void JsClip(const JSCallbackInfo& info);
    void JsRect(const JSCallbackInfo& info);
    void JsBeginPath(const JSCallbackInfo& info);
    void JsClosePath(const JSCallbackInfo& info);
    void JsRestore(const JSCallbackInfo& info);
    void JsSave(const JSCallbackInfo& info);
    void JsRotate(const JSCallbackInfo& info);
    void JsScale(const JSCallbackInfo& info);
    void JsSetTransform(const JSCallbackInfo& info);
    void JsTransform(const JSCallbackInfo& info);
    void JsTranslate(const JSCallbackInfo& info);
    void JsSetLineDash(const JSCallbackInfo& info);
    void JsToDataUrl(const JSCallbackInfo& info);
    void JsCreateLinearGradient(const JSCallbackInfo& info);
    void JsCreateRadialGradient(const JSCallbackInfo& info);

    void JsGetFillStyle(const JSCallbackInfo& info);
    void JsGetStrokeStyle(const JSCallbackInfo& info);
    void JsGetLineCap(const JSCallbackInfo& info);
    void JsGetLineJoin(const JSCallbackInfo& info);
    void JsGetMiterLimit(const JSCallbackInfo& info);
    void JsGetLineWidth(const JSCallbackInfo& info);
    void JsGetTextAlign(const JSCallbackInfo& info);
    void JsGetTextBaseline(const JSCallbackInfo& info);
    void JsGetGlobalAlpha(const JSCallbackInfo& info);
    void JsGetGlobalCompositeOperation(const JSCallbackInfo& info);
    void JsGetLineDashOffset(const JSCallbackInfo& info);
    void JsGetShadowBlur(const JSCallbackInfo& info);
    void JsGetShadowColor(const JSCallbackInfo& info);
    void JsGetShadowOffsetX(const JSCallbackInfo& info);
    void JsGetShadowOffsetY(const JSCallbackInfo& info);
    void JsGetImageSmoothingEnabled(const JSCallbackInfo& info);
    void JsGetImageSmoothingQuality(const JSCallbackInfo& info);
    void JsGetFont(const JSCallbackInfo& info);
    void JsSetFont(const JSCallbackInfo& info);
    void JsSetFillStyle(const JSCallbackInfo& info);
    void JsSetStrokeStyle(const JSCallbackInfo& info);
    void JsSetLineCap(const JSCallbackInfo& info);
    void JsSetLineJoin(const JSCallbackInfo& info);
    void JsSetMiterLimit(const JSCallbackInfo& info);
    void JsSetLineWidth(const JSCallbackInfo& info);
    void JsSetGlobalAlpha(const JSCallbackInfo& info);
    void JsSetGlobalCompositeOperation(const JSCallbackInfo& info);
    void JsSetLineDashOffset(const JSCallbackInfo& info);
    void JsSetShadowBlur(const JSCallbackInfo& info);
    void JsSetShadowColor(const JSCallbackInfo& info);
    void JsSetShadowOffsetX(const JSCallbackInfo& info);
    void JsSetShadowOffsetY(const JSCallbackInfo& info);
    void JsSetImageSmoothingEnabled(const JSCallbackInfo& info);
    void JsDrawImage(const JSCallbackInfo& info);
    void JsCreatePattern(const JSCallbackInfo& info);
    void JsCreateImageData(const JSCallbackInfo& info);
    void JsPutImageData(const JSCallbackInfo& info);
    void JsGetImageData(const JSCallbackInfo& info);
    void JsGetJsonData(const JSCallbackInfo& info);
    void JsSetTextAlign(const JSCallbackInfo& info);
    void JsSetTextBaseline(const JSCallbackInfo& info);
    void JsSetImageSmoothingQuality(const JSCallbackInfo& info);
    void JsGetPixelMap(const JSCallbackInfo& info);

    void SetComponent(const RefPtr<CanvasTaskPool>& controller)
    {
        pool_ = controller;
        isOffscreen_ = false;
    }

    void SetOffscreenCanvas(const RefPtr<OffscreenCanvas>& offscreenCanvas)
    {
        offscreenCanvas_ = offscreenCanvas;
        isOffscreen_ = true;
    }

    std::vector<uint32_t> GetLineDash() const
    {
        return lineDash_;
    }

    void SetLineDash(const std::vector<uint32_t> lineDash)
    {
        lineDash_ = lineDash;
    }

    void SetAnti(bool anti)
    {
        anti_ = anti;
    }

    bool GetAnti()
    {
        return anti_;
    }

    ACE_DISALLOW_COPY_AND_MOVE(JSCanvasRenderer);

protected:
    RefPtr<CanvasTaskPool> pool_;
    RefPtr<OffscreenCanvas> offscreenCanvas_;
    bool anti_ = false;

private:
    PaintState paintState_;
    TextStyle style_;
    static std::unordered_map<int32_t, Pattern> pattern_;
    static int32_t patternCount_;
    Pattern GetPattern(int32_t id);
    std::vector<uint32_t> lineDash_;
    ImageData imageData_;
    bool isOffscreen_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_BRIDGE_JS_VIEW_JS_CANVAS_RENDERER_H
