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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_ELEMENT_H

#include "core/components/xcomponent/native_interface_xcomponent_impl.h"
#include "core/components/xcomponent/resource/native_texture.h"
#include "core/components/xcomponent/xcomponent_component.h"
#include "core/pipeline/base/render_element.h"

#ifdef OHOS_STANDARD_SYSTEM
#include "display_type.h"
#include "foundation/graphic/standard/interfaces/innerkits/surface/window.h"
#include "foundation/windowmanager/interfaces/innerkits/wm/window.h"
#include "render_service_client/core/ui/rs_node.h"
#include "render_service_client/core/ui/rs_surface_node.h"
#include "surface_utils.h"
#endif

namespace OHOS::Ace {
class XComponentComponent;

class XComponentElement : public RenderElement {
    DECLARE_ACE_TYPE(XComponentElement, RenderElement);

public:
    using InitEventCallback = std::function<void(const std::string&)>;
    using DestroyEventCallback = std::function<void(const std::string&)>;
    ~XComponentElement();
    void Prepare(const WeakPtr<Element>& parent) override;
    void SetNewComponent(const RefPtr<Component>& newComponent) override;

    RefPtr<XComponentComponent> xcomponent_;

private:
    void InitEvent();
    void RegisterSurfaceDestroyEvent();
    void OnSurfaceDestroyEvent();
    void SetTouchEventType(const TouchEvent& event);
    void SetTouchPoint(const TouchEvent& event);
    void OnXComponentInit(const std::string& param);
    void OnSurfaceInit(const std::string& componentId, const uint32_t nodeId);
    void RegisterDispatchTouchEventCallback();
    void DispatchTouchEvent(const TouchEvent& event);
    void OnXComponentSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight);
    void OnTextureSize(int64_t textureId, std::string& result);

    void CreatePlatformResource();
    void ReleasePlatformResource();
    bool IsDeclarativePara();

    std::function<void(const std::string&, const uint32_t)> onSurfaceInit_;
    InitEventCallback onXComponentInit_;
    DestroyEventCallback onXComponentDestroy_;
    OH_NativeXComponent_TouchEvent touchEventPoint_;
    std::string name_;
    std::string idStr_;
    bool hasSendDestroyEvent_ = false;
    bool isExternalResource_ = false;
    RefPtr<NativeTexture> texture_;

#ifdef OHOS_STANDARD_SYSTEM
    void CreateSurface();
    void OnXComponentHiddenChange(bool hidden);
    void SetMethodCall();
    void ConfigSurface(uint32_t surfaceWidth, uint32_t surfaceHeight);

    bool onLoadDone_ = false;
    static std::unordered_map<std::string, uint64_t> surfaceIdMap_;
    RefPtr<XComponentController> xcomponentController_;
    sptr<OHOS::Surface> producerSurface_ = nullptr;
#endif
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_ELEMENT_H
