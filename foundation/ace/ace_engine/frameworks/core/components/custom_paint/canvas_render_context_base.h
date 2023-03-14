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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_CANVAS_RENDER_CONTEXT_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_CANVAS_RENDER_CONTEXT_BASE_H

#include <functional>

namespace OHOS::Ace {

const char WEBGL_RENDER_CONTEXT_NAME[] = "WebGLRenderingContext";

class CanvasRenderContextBase {
public:
    CanvasRenderContextBase() = default;
    virtual ~CanvasRenderContextBase() {}

    virtual void Create(void* context) = 0;

    virtual void Init() = 0;

    virtual void Attach(uint64_t textureId) = 0;

    virtual void Update() = 0;

    virtual void Detach() = 0;

    virtual void SetBitMapPtr(char* bitMapPtr, int bitMapWidth, int bitMapHeight) = 0;

    virtual uint64_t CreateTexture() = 0;

    virtual void SetUpdateCallback(std::function<void()>) = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_CANVAS_RENDER_CONTEXT_BASE_H
