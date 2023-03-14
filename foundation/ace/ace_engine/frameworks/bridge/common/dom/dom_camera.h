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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CAMERA_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CAMERA_H

#include "core/components/camera/camera_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMCamera final : public DOMNode {
    DECLARE_ACE_TYPE(DOMCamera, DOMNode);

public:
    DOMCamera(NodeId nodeId, const std::string& nodeName);
    ~DOMCamera() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return cameraComponent_;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

    TakePhotoParams GetParamFromJson(const std::string& args) const;
    std::string GetRecorderParam(const std::string& args) const;

    static FlashType GetFlashType(const std::string& val);
    static DevicePosition GetDevicePosition(const std::string& val);

private:
    RefPtr<CameraComponent> cameraComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CAMERA_H