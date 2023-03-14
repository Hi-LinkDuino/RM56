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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_H

#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSVideo : public JSViewAbstract, public JSInteractableView {
public:
    using EventCallback = std::function<void(const std::string&)>;
    static void Create(const JSCallbackInfo& info);
    static void JsMuted(bool isMuted);
    static void JsAutoPlay(bool autoPlay);
    static void JsControls(bool controls);
    static void JsLoop(bool loop);
    static void JsObjectFit(int32_t objectFit);

    static void JsOnStart(const JSCallbackInfo& args);
    static void JsOnPause(const JSCallbackInfo& args);
    static void JsOnFinish(const JSCallbackInfo& args);
    static void JsOnFullscreenChange(const JSCallbackInfo& args);
    static void JsOnPrepared(const JSCallbackInfo& args);
    static void JsOnSeeking(const JSCallbackInfo& args);
    static void JsOnSeeked(const JSCallbackInfo& args);
    static void JsOnUpdate(const JSCallbackInfo& args);
    static void JsOnError(const JSCallbackInfo& args);

    static void JSBind(BindingTarget globalObj);

private:
    static EventMarker GetEventMarker(const JSCallbackInfo& info, const std::vector<std::string>& keys);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_H
