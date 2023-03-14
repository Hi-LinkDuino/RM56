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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_IMAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_IMAGE_H

#include "core/components/clip/clip_component.h"
#include "core/components/image/image_component.h"
#include "core/components/image/image_theme.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMImage final : public DOMNode {
    DECLARE_ACE_TYPE(DOMImage, DOMNode);

public:
    DOMImage(NodeId nodeId, const std::string& nodeName);
    ~DOMImage() override = default;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return imageChild_;
    }

    bool IsLeafNode() const override
    {
        auto src = imageChild_->GetSrc();
        return !(src.size() > 4 && src.substr(src.size() - 4) == ".svg");
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    ImageObjectPosition ParseImageObjectPosition(const std::string& value);
    bool IsSubscriptEnable() const override
    {
        if (declaration_) {
            declaration_->SetIsSubscriptEnable(true);
        }
        return true;
    }

private:
    RefPtr<ImageComponent> imageChild_;
    EventMarker loadSuccessEvent_;
    EventMarker loadFailEvent_;
    RefPtr<ImageTheme> theme_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_IMAGE_H
