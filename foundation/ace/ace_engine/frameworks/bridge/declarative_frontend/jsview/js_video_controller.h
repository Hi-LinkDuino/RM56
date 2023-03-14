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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_CONTROLLER_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "core/components/video/video_component_v2.h"

namespace OHOS::Ace::Framework {

class JSVideoController : public Referenced {
public:
    JSVideoController() = default;
    ~JSVideoController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSVideoController* scroller);

    void Start(const JSCallbackInfo& args);
    void Pause(const JSCallbackInfo& args);
    void Stop(const JSCallbackInfo& args);
    void SetCurrentTime(const JSCallbackInfo& args);
    void RequestFullscreen(const JSCallbackInfo& args);
    void ExitFullscreen(const JSCallbackInfo& args);

    RefPtr<VideoControllerV2> GetController() const
    {
        return videoController_;
    }

    void SetController(const RefPtr<VideoControllerV2>& videoController)
    {
        videoController_ = videoController;
    }

private:
    RefPtr<VideoControllerV2> videoController_;
    ACE_DISALLOW_COPY_AND_MOVE(JSVideoController);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIDEO_CONTROLLER_H
