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

#include "core/components/xcomponent/xcomponent_element.h"

#include "base/json/json_util.h"
#include "core/components/xcomponent/render_xcomponent.h"
#include "core/components/xcomponent/rosen_render_xcomponent.h"
#include "core/components/xcomponent/xcomponent_component.h"

namespace OHOS::Ace {
#ifdef OHOS_STANDARD_SYSTEM
namespace {
const char* SURFACE_STRIDE_ALIGNMENT = "8";
constexpr int32_t SURFACE_QUEUE_SIZE = 5;
} // namespace

std::unordered_map<std::string, uint64_t> XComponentElement::surfaceIdMap_;
#endif

XComponentElement::~XComponentElement()
{
    ReleasePlatformResource();
}

void XComponentElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr) {
        Element::SetNewComponent(newComponent);
        return;
    }
    auto xcomponent = AceType::DynamicCast<XComponentComponent>(newComponent);
    if (xcomponent) {
        idStr_ = xcomponent->GetId();
        if (texture_) {
            xcomponent->SetTextureId(texture_->GetId());
            xcomponent->SetTexture(texture_);
            isExternalResource_ = true;
        }
        name_ = xcomponent->GetName();
        Element::SetNewComponent(xcomponent);
    }
}

void XComponentElement::Prepare(const WeakPtr<Element>& parent)
{
    xcomponent_ = AceType::DynamicCast<XComponentComponent>(component_);
    InitEvent();
    RegisterSurfaceDestroyEvent();
    RegisterDispatchTouchEventCallback();
    if (xcomponent_) {
        if (!isExternalResource_) {
            CreatePlatformResource();
#ifdef OHOS_STANDARD_SYSTEM
            SetMethodCall();
#endif
        }
    }
    RenderElement::Prepare(parent);
    if (renderNode_) {
        auto renderXComponent = AceType::DynamicCast<RenderXComponent>(renderNode_);
        if (renderXComponent) {
            renderXComponent->SetXComponentSizeChange(
                [weak = WeakClaim(this)](int64_t textureId, int32_t width, int32_t height) {
                    auto xcomponentElement = weak.Upgrade();
                    if (xcomponentElement) {
                        xcomponentElement->OnXComponentSize(textureId, width, height);
                    }
            });
#ifdef OHOS_STANDARD_SYSTEM
            CreateSurface();
#endif
        }
    }
}

void XComponentElement::InitEvent()
{
    if (!xcomponent_) {
        LOGE("XComponentElement::InitEvent xcomponent_ is null");
        return;
    }
    if (!xcomponent_->GetXComponentInitEventId().IsEmpty()) {
        onSurfaceInit_ = AceSyncEvent<void(const std::string&, const uint32_t)>::Create(
            xcomponent_->GetXComponentInitEventId(), context_);
        onXComponentInit_ =
            AceAsyncEvent<void(const std::string&)>::Create(xcomponent_->GetXComponentInitEventId(), context_);
    }
    if (!xcomponent_->GetXComponentDestroyEventId().IsEmpty()) {
        onXComponentDestroy_ =
            AceAsyncEvent<void(const std::string&)>::Create(xcomponent_->GetXComponentDestroyEventId(), context_);
    }
}

void XComponentElement::RegisterSurfaceDestroyEvent()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetDestroyHandler([weak = WeakClaim(this)] {
            auto element = weak.Upgrade();
            if (element) {
                element->OnSurfaceDestroyEvent();
            }
        });
    }
}

void XComponentElement::OnSurfaceDestroyEvent()
{
    std::string param;
    if (IsDeclarativePara()) {
        auto json = JsonUtil::Create(true);
        json->Put("destroy", "");
        param = json->ToString();
    } else {
        param = std::string("\"destroy\",{").append("}");
    }
    if (!hasSendDestroyEvent_) {
        if (onXComponentDestroy_) {
            onXComponentDestroy_(param);
        }
        auto renderXComponent = AceType::DynamicCast<RenderXComponent>(renderNode_);
        if (renderXComponent) {
            renderXComponent->NativeXComponentDestroy();
        }
        hasSendDestroyEvent_ = true;
    }
}

bool XComponentElement::IsDeclarativePara()
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    return context->GetIsDeclarative();
}

void XComponentElement::RegisterDispatchTouchEventCallback()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("RegisterDispatchTouchEventCallback pipelineContext is null");
        return;
    }
    pipelineContext->SetDispatchTouchEventHandler([weak = WeakClaim(this)](const TouchEvent& event) {
        auto element = weak.Upgrade();
        if (element) {
            element->DispatchTouchEvent(event);
        }
    });
}

void XComponentElement::DispatchTouchEvent(const TouchEvent& event)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("DispatchTouchEvent pipelineContext is null");
        return;
    }
    auto renderXComponent = AceType::DynamicCast<RenderXComponent>(renderNode_);
    if (renderXComponent) {
        touchEventPoint_.id = event.id;
        touchEventPoint_.screenX = 0;
        touchEventPoint_.screenY = 0;
        touchEventPoint_.x = event.x;
        touchEventPoint_.y = event.y;
        touchEventPoint_.size = event.size;
        touchEventPoint_.force = event.force;
        touchEventPoint_.deviceId = event.deviceId;
        touchEventPoint_.timeStamp = event.time.time_since_epoch().count();
        SetTouchEventType(event);
        SetTouchPoint(event);
        renderXComponent->NativeXComponentDispatchTouchEvent(touchEventPoint_);
    }
}

void XComponentElement::SetTouchEventType(const TouchEvent& event)
{
    switch (event.type) {
        case TouchType::DOWN:
            touchEventPoint_.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_DOWN;
            break;
        case TouchType::UP:
            touchEventPoint_.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UP;
            break;
        case TouchType::MOVE:
            touchEventPoint_.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_MOVE;
            break;
        case TouchType::CANCEL:
            touchEventPoint_.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_CANCEL;
            break;
        default:
            touchEventPoint_.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UNKNOWN;
            break;
    }
}

void XComponentElement::SetTouchPoint(const TouchEvent& event)
{
    for (uint32_t i = 0; i < OH_MAX_TOUCH_POINTS_NUMBER; ++i) {
        OH_NativeXComponent_TouchPoint ohTouchPoint;
        ohTouchPoint.id = 0;
        ohTouchPoint.screenX = 0;
        ohTouchPoint.screenY = 0;
        ohTouchPoint.x = 0;
        ohTouchPoint.y = 0;
        ohTouchPoint.type = OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UNKNOWN;
        ohTouchPoint.size = 0;
        ohTouchPoint.force = 0;
        ohTouchPoint.timeStamp = 0;
        ohTouchPoint.isPressed = false;
        touchEventPoint_.touchPoints[i] = ohTouchPoint;
    }

    if (event.pointers.size() > OH_MAX_TOUCH_POINTS_NUMBER) {
        touchEventPoint_.numPoints = OH_MAX_TOUCH_POINTS_NUMBER;
    } else {
        touchEventPoint_.numPoints = event.pointers.size();
    }

    for (uint32_t i = 0; i < touchEventPoint_.numPoints; ++i) {
        OH_NativeXComponent_TouchPoint ohTouchPoint;
        TouchPoint point = event.pointers[i];
        ohTouchPoint.id = point.id;
        ohTouchPoint.x = point.x;
        ohTouchPoint.y = point.y;
        ohTouchPoint.size = point.size;
        ohTouchPoint.force = point.force;
        ohTouchPoint.timeStamp = point.downTime.time_since_epoch().count();
        ohTouchPoint.isPressed = point.isPressed;
        touchEventPoint_.touchPoints[i] = ohTouchPoint;
    }
}

void XComponentElement::CreatePlatformResource()
{
    ReleasePlatformResource();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("XComponentElement CreatePlatformResource context = null");
        return;
    }
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);

    auto errorCallback = [weak = WeakClaim(this), uiTaskExecutor](
            const std::string& errorId, const std::string& param) {
        uiTaskExecutor.PostTask([weak, errorId, param] {
            auto XComponentElement = weak.Upgrade();
            if (XComponentElement) {
                LOGE("XComponentElement errorCallback");
            }
        });
    };
    texture_ = AceType::MakeRefPtr<NativeTexture>(context_, errorCallback);
    texture_->Create(
        [weak = WeakClaim(this), errorCallback](int64_t id) mutable {
            auto XComponentElement = weak.Upgrade();
            if (XComponentElement) {
                auto component = XComponentElement->xcomponent_;
                if (component) {
                    XComponentElement->isExternalResource_ = true;
                    component->SetTextureId(id);
                    component->SetTexture(XComponentElement->texture_);
                    if (XComponentElement->renderNode_ != nullptr) {
                        XComponentElement->renderNode_->Update(component);
                    }
                }
            }
        },
        idStr_);
}

#ifdef OHOS_STANDARD_SYSTEM
void XComponentElement::CreateSurface()
{
    if (renderNode_) {
        auto rosenTexture = AceType::DynamicCast<RosenRenderXComponent>(renderNode_);
        if (rosenTexture) {
            producerSurface_ = rosenTexture->GetSurface();
        }
    }

    if (producerSurface_ == nullptr) {
        LOGE("producerSurface is nullptr");
        return;
    }

    auto surfaceUtils = SurfaceUtils::GetInstance();
    auto ret = surfaceUtils->Add(producerSurface_->GetUniqueId(), producerSurface_);
    if (ret != SurfaceError::SURFACE_ERROR_OK) {
        LOGE("xcomponent add surface error: %{public}d", ret);
    }

    producerSurface_->SetQueueSize(SURFACE_QUEUE_SIZE);
    producerSurface_->SetUserData("SURFACE_STRIDE_ALIGNMENT", SURFACE_STRIDE_ALIGNMENT);
    producerSurface_->SetUserData("SURFACE_FORMAT", std::to_string(PIXEL_FMT_RGBA_8888));

    XComponentElement::surfaceIdMap_.emplace(xcomponent_->GetId(), producerSurface_->GetUniqueId());

    if (!xcomponentController_) {
        auto controller = xcomponent_->GetXComponentController();
        if (!controller) {
            LOGE("There is no controller in xcomponent.");
            return;
        }
        xcomponentController_ = controller;
    }
    xcomponentController_->surfaceId_ = producerSurface_->GetUniqueId();
}

void XComponentElement::SetMethodCall()
{
    if (!xcomponentController_) {
        auto controller = xcomponent_->GetXComponentController();
        if (!controller) {
            LOGE("There is no controller in xcomponent.");
            return;
        }
        xcomponentController_ = controller;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("There is no context in xcomponent.");
        return;
    }
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    xcomponentController_->SetConfigSurfaceImpl([weak = WeakClaim(this), uiTaskExecutor]
                                                (uint32_t surfaceWidth, uint32_t surfaceHeight) {
        uiTaskExecutor.PostTask([weak, surfaceWidth, surfaceHeight]() {
            auto xComponentElement = weak.Upgrade();
            if (xComponentElement) {
                xComponentElement->ConfigSurface(surfaceWidth, surfaceHeight);
            }
        });
    });
}

void XComponentElement::ConfigSurface(uint32_t surfaceWidth, uint32_t surfaceHeight)
{
    if (producerSurface_) {
        producerSurface_->SetUserData("SURFACE_WIDTH", std::to_string(surfaceWidth));
        producerSurface_->SetUserData("SURFACE_HEIGHT", std::to_string(surfaceHeight));
    }
}
#endif

void XComponentElement::ReleasePlatformResource()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("XComponentElement ReleasePlatformResource context null");
        return;
    }

    // Reusing texture will cause a problem that last frame of last video will be display.
    if (texture_) {
        auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
                                                             TaskExecutor::TaskType::PLATFORM);
        if (platformTaskExecutor.IsRunOnCurrentThread()) {
            if (!isExternalResource_) {
                texture_->Release();
            }
            texture_.Reset();
        }
    }

#ifdef OHOS_STANDARD_SYSTEM
    if (producerSurface_) {
        auto surfaceUtils = SurfaceUtils::GetInstance();
        auto ret = surfaceUtils->Remove(producerSurface_->GetUniqueId());
        if (ret != SurfaceError::SURFACE_ERROR_OK) {
            LOGE("xcomponent remove surface error: %{public}d", ret);
        }
    }
#endif
}

void XComponentElement::OnXComponentSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight)
{
#ifdef OHOS_STANDARD_SYSTEM
    if (producerSurface_ != nullptr) {
        if (renderNode_ != nullptr) {
            auto context = context_.Upgrade();
            if (context == nullptr) {
                LOGE("context is nullptr");
                return;
            }

            float viewScale = context->GetViewScale();
            auto nativeWindow = CreateNativeWindowFromSurface(&producerSurface_);
            if (nativeWindow) {
                NativeWindowHandleOpt(nativeWindow, SET_BUFFER_GEOMETRY,
                                      (int)(textureWidth * viewScale), (int)(textureHeight * viewScale));
                xcomponent_->SetNativeWindow(nativeWindow);
            } else {
                LOGE("can not create NativeWindow from surface");
            }
        }
    }
    std::string str = "";
    if (!onLoadDone_) {
        onLoadDone_ = true;
        this->OnTextureSize(X_INVALID_ID, str);
    }
#endif

    if (texture_) {
        texture_->OnSize(textureId, textureWidth, textureHeight,
                         [weak = WeakClaim(this), textureId](std::string& result) {
                            auto xcomponentElement = weak.Upgrade();
                            if (xcomponentElement) {
                                xcomponentElement->OnTextureSize(textureId, result);
                            }
        });
    }
}

void XComponentElement::OnTextureSize(int64_t textureId, std::string& result)
{
    if (xcomponent_) {
        OnSurfaceInit(xcomponent_->GetId(), xcomponent_->GetNodeId());
        OnXComponentInit("");
    }
}

void XComponentElement::OnXComponentInit(const std::string& param)
{
    std::string loadStr;
    if (IsDeclarativePara()) {
        auto json = JsonUtil::Create(true);
        json->Put("load", "");
        loadStr = json->ToString();
    } else {
        loadStr = std::string("\"load\",{").append("}");
    }

    if (onXComponentInit_) {
        onXComponentInit_(loadStr);
    }
}

void XComponentElement::OnSurfaceInit(const std::string& componentId, const uint32_t nodeId)
{
    if (onSurfaceInit_) {
        onSurfaceInit_(componentId, nodeId);
    }
}
} // namespace OHOS::Ace
