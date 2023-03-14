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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_COMPONENT_H

#include <string>

#include "base/geometry/size.h"
#include "base/utils/utils.h"
#include "core/components/declaration/xcomponent/xcomponent_declaration.h"
#include "core/components/xcomponent/native_interface_xcomponent_impl.h"
#include "core/components/xcomponent/resource/native_texture.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {
class XComponentDelegate;
class RenderXComponent;
class XComponentComponent;

using TaskFunction = std::function<void(RenderXComponent&, const Offset&)>;
using PushTaskFunction = std::function<void(const TaskFunction&)>;

class ACE_EXPORT XComponentTaskPool : virtual public AceType {
    DECLARE_ACE_TYPE(XComponentTaskPool, AceType);

public:
    void PushTask(const TaskFunction& task)
    {
        if (pushToRenderNodeFunc_) {
            pushToRenderNodeFunc_(task);
        } else {
            tasks_.emplace_back(task);
        }
    }

    void SetPushToRenderNodeFunc(const PushTaskFunction& pushTaskFunc)
    {
        pushToRenderNodeFunc_ = pushTaskFunc;
    }

    const std::list<TaskFunction>& GetTasks() const
    {
        return tasks_;
    }

    void ClearTasks()
    {
        tasks_.clear();
    }

    void SetRenderNode(const WeakPtr<RenderXComponent>& renderNode);
    void NativeXComponentInit(
        OH_NativeXComponent* nativeXComponent,
        WeakPtr<NativeXComponentImpl> nativeXComponentImpl);

    WeakPtr<XComponentComponent> component_;

private:
    PushTaskFunction pushToRenderNodeFunc_;
    std::list<TaskFunction> tasks_;
    WeakPtr<RenderXComponent> renderNode_;
};

class XComponentController : public virtual AceType {
    DECLARE_ACE_TYPE(XComponentController, AceType);

public:
    using ConfigSurfaceImpl = std::function<void(uint32_t, uint32_t)>;

    uint64_t GetSurfaceId()
    {
        return surfaceId_;
    }

    void ConfigSurface(uint32_t surfaceWidth, uint32_t surfaceHeight)
    {
        if (ConfigSurfaceImpl_) {
            ConfigSurfaceImpl_(surfaceWidth, surfaceHeight);
        }
    }

    void SetConfigSurfaceImpl(ConfigSurfaceImpl&& ConfigSurfaceImpl)
    {
        ConfigSurfaceImpl_ = std::move(ConfigSurfaceImpl);
    }

public:
    void AddXComponentController(const RefPtr<XComponentController>& xcomponentController)
    {
        auto it = std::find(controllers_.begin(), controllers_.end(), xcomponentController);
        if (it != controllers_.end()) {
            LOGW("Controller is already existed");
            return;
        }
        controllers_.emplace_back(xcomponentController);
    }

    uint64_t surfaceId_ = 0;

    void RemoveXComponentController(const RefPtr<XComponentController>& xcomponentController)
    {
        if (xcomponentController) {
            controllers_.remove(xcomponentController);
        }
    }

    void Clear()
    {
        controllers_.clear();
    }

private:
    std::list<RefPtr<XComponentController>> controllers_;
    ConfigSurfaceImpl ConfigSurfaceImpl_;
};

// A component can show different native view.
class ACE_EXPORT XComponentComponent : public RenderComponent {
    DECLARE_ACE_TYPE(XComponentComponent, RenderComponent);

public:
    using CreatedCallback = std::function<void()>;
    using ReleasedCallback = std::function<void(bool)>;
    using ErrorCallback = std::function<void(const std::string&, const std::string&)>;
    using MethodCall = std::function<void(const std::string&)>;
    using Method = std::string;

    XComponentComponent() = default;
    explicit XComponentComponent(const std::string& type);
    ~XComponentComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetType(const std::string& type)
    {
        type_ = type;
    }

    const std::string& GetType() const
    {
        return type_;
    }

    void SetName(const std::string& name)
    {
        declaration_->SetXComponentName(name);
    }

    const std::string& GetName() const
    {
        return declaration_->GetXComponentName();
    }

    void SetId(const std::string& id)
    {
        declaration_->SetXComponentId(id);
    }

    const std::string& GetId() const
    {
        return declaration_->GetXComponentId();
    }

    void SetXComponentType(const std::string& type)
    {
        declaration_->SetXComponentType(type);
    }

    const std::string& GetXComponentType() const
    {
        return declaration_->GetXComponentType();
    }

    void SetLibraryName(const std::string& libraryName)
    {
        declaration_->SetXComponentLibraryName(libraryName);
    }

    const std::string& GetLibraryName() const
    {
        return declaration_->GetXComponentLibraryName();
    }

    void SetXComponentInitEventId(const EventMarker& xComponentInitEventId)
    {
        declaration_->SetXComponentInitEventId(xComponentInitEventId);
    }

    const EventMarker& GetXComponentInitEventId() const
    {
        return declaration_->GetXComponentInitEventId();
    }

    void SetXComponentDestroyEventId(const EventMarker& xComponentDestroyEventId)
    {
        declaration_->SetXComponentDestroyEventId(xComponentDestroyEventId);
    }

    const EventMarker& GetXComponentDestroyEventId() const
    {
        return declaration_->GetXComponentDestroyEventId();
    }

    void SetDeclaration(const RefPtr<XComponentDeclaration>& declaration)
    {
        if (declaration) {
            declaration_ = declaration;
        }
    }

    const RefPtr<XComponentTaskPool>& GetTaskPool() const
    {
        return pool_;
    }

    int64_t GetTextureId() const
    {
        return textureId_;
    }

    void SetTextureId(int64_t textureId)
    {
        textureId_ = textureId;
    }

    int32_t GetNodeId() const
    {
        return nodeId_;
    }

    void SetNodeId(int32_t nodeId)
    {
        nodeId_ = nodeId;
    }

    RefPtr<NativeTexture> GetTexture() const
    {
        return texture_;
    }

    void SetTexture(RefPtr<NativeTexture> texture)
    {
        texture_ = texture;
    }

    RefPtr<XComponentController> GetXComponentController() const
    {
        return xcomponentController_;
    }

    void SetXComponentController(const RefPtr<XComponentController>& xcomponentController)
    {
        xcomponentController_ = xcomponentController;
    }

#ifdef OHOS_STANDARD_SYSTEM
    void SetNativeWindow(void* nativeWindow)
    {
        nativeWindow_ = nativeWindow;
    }

    void* GetNativeWindow()
    {
        return nativeWindow_;
    }
#endif

private:
    RefPtr<XComponentDeclaration> declaration_;
    CreatedCallback createdCallback_ = nullptr;
    ReleasedCallback releasedCallback_ = nullptr;
    ErrorCallback errorCallback_ = nullptr;
    RefPtr<XComponentDelegate> delegate_;
    RefPtr<XComponentTaskPool> pool_;
    RefPtr<XComponentController> xcomponentController_;
    std::string type_;
    int64_t textureId_ = -1;
    int32_t nodeId_ = -1;
    RefPtr<NativeTexture> texture_;
#ifdef OHOS_STANDARD_SYSTEM
    void* nativeWindow_ = nullptr;
#endif
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_XCOMPONENT_COMPONENT_H
