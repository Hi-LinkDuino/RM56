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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_ANIMATOR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_ANIMATOR_H

#include "bridge/declarative_frontend/jsview/js_container_base.h"

#include "core/components/image/image_animator_component.h"

namespace OHOS::Ace::Framework {

class JSImageAnimator : public JSContainerBase {
public:
    static void Create();
    static void JSBind(BindingTarget globalObj);

    static void SetImages(const JSCallbackInfo& info);
    static void SetState(int32_t state);
    static void SetDuration(int32_t duration);
    static void SetIteration(int32_t iteration);
    static void SetFillMode(int32_t fillMode);
    static void SetPreDecode(int32_t preDecode);
    static void SetIsReverse(bool isReverse);
    static void SetFixedSize(bool fixedSize);

    static void OnStart(const JSCallbackInfo& info);
    static void OnPause(const JSCallbackInfo& info);
    static void OnRepeat(const JSCallbackInfo& info);
    static void OnCancel(const JSCallbackInfo& info);
    static void OnFinish(const JSCallbackInfo& info);

private:
    static void ParseImages(const std::unique_ptr<JsonValue>& image, ImageProperties& imageProperties);
    static EventMarker GetEventMarker(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_IMAGE_ANIMATOR_H
