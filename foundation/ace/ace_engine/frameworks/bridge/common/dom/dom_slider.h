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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SLIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SLIDER_H

#include "core/components/padding/padding_component.h"
#include "core/components/slider/slider_component.h"
#include "core/components/track/track_component.h"
#include "core/components/watch_slider/watch_slider_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMSlider final : public DOMNode {
    DECLARE_ACE_TYPE(DOMSlider, DOMNode);

public:
    DOMSlider(NodeId nodeId, const std::string& nodeName);
    ~DOMSlider() override = default;

    void InitializeStyle() override;

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return paddingChild_;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:

    RefPtr<TrackComponent> trackChild_;
    RefPtr<BlockComponent> blockChild_;
    RefPtr<SliderComponent> sliderChild_;
    RefPtr<WatchSliderComponent> watchSliderChild_;
    RefPtr<PaddingComponent> paddingChild_;

    double min_ = 0.0;
    double max_ = 100.0;
    double step_ = 1.0;
    double val_ = 0.0;
    Dimension paddingLeft_ = 32.0_vp;
    Dimension paddingRight_ = 32.0_vp;
    Color color_;
    Color selectColor_;
    Color blockColor_;
    EventMarker onMoveEndEventId_;

    std::string minIconUrl_;
    std::string maxIconUrl_;
    SliderMode mode_ = SliderMode::OUTSET;
    bool isContinuous_ = true;
    bool showSteps_ = false;
    bool showTips_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SLIDER_H
