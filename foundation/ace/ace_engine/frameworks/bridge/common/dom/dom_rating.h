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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_RATING_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_RATING_H

#include "core/components/rating/rating_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMRating final : public DOMNode {
    DECLARE_ACE_TYPE(DOMRating, DOMNode);

public:
    DOMRating(NodeId nodeId, const std::string& nodeName);
    ~DOMRating() override = default;

    void SetThemeAttrs();

    RefPtr<Component> GetSpecializedComponent() override
    {
        return ratingChild_;
    }

    void ResetInitializedStyle() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;

private:
    RefPtr<RatingComponent> ratingChild_;
    EventMarker changeEventId_;

    std::pair<int32_t, bool> starNum_ = { DEFAULT_RATING_STAR_NUM, false };
    std::pair<double, bool> ratingScore_ = { DEFAULT_RATING_SCORE, false };
    std::pair<double, bool> stepSize_ = { DEFAULT_RATING_STEP_SIZE, false };
    std::pair<std::string, bool> foregroundSrc_ { "", false };
    std::pair<std::string, bool> secondarySrc_ { "", false };
    std::pair<std::string, bool> backgroundSrc_ { "", false };
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_RATING_H
