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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_DELEGATE_H

#include "frameworks/base/memory/ace_type.h"
#include "frameworks/base/utils/noncopyable.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
// This is the primary class by which the CardFrontend delegates
class CardFrontendDelegate : public AceType {
    DECLARE_ACE_TYPE(CardFrontendDelegate, AceType);

public:
    CardFrontendDelegate();
    ~CardFrontendDelegate() override;
    void FireCardEvent(const EventMarker& eventMarker, const std::string& params = "");
    RefPtr<JsAcePage>& CreatePage(int32_t pageId, const std::string& url);
    RefPtr<JsAcePage>& CreatePage(int32_t pageId, const std::string& url, const WeakPtr<StageElement>& container);
    RefPtr<JsAcePage>& GetPage();

    RefPtr<AccessibilityNodeManager> GetJsAccessibilityManager() const
    {
        return jsAccessibilityManager_;
    }

private:
    RefPtr<JsAcePage> page_;
    RefPtr<AccessibilityNodeManager> jsAccessibilityManager_;

    ACE_DISALLOW_COPY_AND_MOVE(CardFrontendDelegate);
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_DELEGATE_H
