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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DOM_DOM_QRCODE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DOM_DOM_QRCODE_H

#include "core/components/qrcode/qrcode_component.h"
#include "core/components/qrcode/qrcode_theme.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMQrcode final : public DOMNode {
    DECLARE_ACE_TYPE(DOMQrcode, DOMNode);

public:
    DOMQrcode(NodeId nodeId, const std::string& nodeName);
    ~DOMQrcode() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return qrcodeChild_;
    }

protected:
    void ResetInitializedStyle() override;
    void PrepareSpecializedComponent() override;

private:
    RefPtr<QrcodeComponent> qrcodeChild_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DOM_DOM_QRCODE_H
