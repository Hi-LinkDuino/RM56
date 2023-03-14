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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_H

#include "core/components/image/image_component.h"
#include "core/components/image/image_event.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_utils.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "napi/native_api.h"
#include "native_engine/native_engine.h"
#endif

#ifdef USE_QUICKJS_ENGINE
#ifdef __cplusplus
extern "C" {
#endif
#include "third_party/quickjs/quickjs.h"
#ifdef __cplusplus
}
#endif
#endif

namespace OHOS::Ace::Framework {
class JSImage : public JSViewAbstract, public JSInteractableView {
public:
    static void Create(const JSCallbackInfo& info);
    static void HandleLoadImageSuccess(const BaseEventInfo& param);
    static void HandleLoadImageFail(const BaseEventInfo& param);
    static void SetAlt(const JSCallbackInfo& args);
    static void SetMatchTextDirection(bool value);
    static void SetFitOriginalSize(bool value);
    static void SetSourceSize(const JSCallbackInfo& info);
    static void SetObjectFit(int32_t value);
    static void SetImageFill(const JSCallbackInfo& info);
    static void SetImageInterpolation(int32_t imageInterpolation);
    static void SetImageRenderMode(int32_t imageRenderMode);
    static void SetImageRepeat(int32_t imageRepeat);
    static void JSBind(BindingTarget globalObj);
    static void OnComplete(const JSCallbackInfo& args);
    static void OnError(const JSCallbackInfo& args);
    static void OnFinish(const JSCallbackInfo& info);
    static void SetSyncLoad(const JSCallbackInfo& info);

    static void JsPadding(const JSCallbackInfo& info);
    static void JsMargin(const JSCallbackInfo& info);
    static void ParseMarginOrPadding(const JSCallbackInfo& info, bool isMargin);
    static void JsBorder(const JSCallbackInfo& info);
    static void JsBorderRadius(const JSCallbackInfo& info);
    static void SetLeftBorderWidth(const Dimension& value);
    static void SetTopBorderWidth(const Dimension& value);
    static void SetRightBorderWidth(const Dimension& value);
    static void SetBottomBorderWidth(const Dimension& value);
    static void SetBorderRadius(const Dimension& value);
    static void JsOpacity(const JSCallbackInfo& info);
    static void JsTransition(const JSCallbackInfo& info);

protected:
    /**
     * box properties setter
     */
    static RefPtr<Decoration> GetFrontDecoration();
    static const Border& GetBorder();
    static BorderEdge GetLeftBorderEdge();
    static BorderEdge GetTopBorderEdge();
    static BorderEdge GetRightBorderEdge();
    static BorderEdge GetBottomBorderEdge();
    static void SetBorderEdge(const BorderEdge& edge);
    static void SetLeftBorderEdge(const BorderEdge& edge);
    static void SetTopBorderEdge(const BorderEdge& edge);
    static void SetRightBorderEdge(const BorderEdge& edge);
    static void SetBottomBorderEdge(const BorderEdge& edge);
    static void SetBorder(const Border& border);
    static void SetWidth(const Dimension& width);
    static void SetHeight(const Dimension& height);
    static void SetMarginTop(const std::string& value);
    static void SetMarginBottom(const std::string& value);
    static void SetMarginLeft(const std::string& value);
    static void SetMarginRight(const std::string& value);
    static void SetMargin(const std::string& value);
    static void SetPaddingTop(const std::string& value);
    static void SetPaddingBottom(const std::string& value);
    static void SetPaddingLeft(const std::string& value);
    static void SetPaddingRight(const std::string& value);
    static void SetPadding(const std::string& value);
    static void SetBackgroundColor(const Color& color);
    static void SetLeftBorderColor(const Color& color);
    static void SetTopBorderColor(const Color& color);
    static void SetRightBorderColor(const Color& color);
    static void SetBottomBorderColor(const Color& color);
    static void SetAutoResize(bool autoResize);

};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_H
