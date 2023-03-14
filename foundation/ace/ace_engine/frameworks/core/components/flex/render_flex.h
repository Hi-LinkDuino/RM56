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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_RENDER_FLEX_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_RENDER_FLEX_H

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/flex/render_flex_item.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class FlexComponent;

struct FlexItemProperties {
    double totalShrink = 0.0;
    double totalGrow = 0.0;
    RefPtr<RenderFlexItem> lastShrinkChild;
    RefPtr<RenderFlexItem> lastGrowChild;
};

struct BaselineProperties {
    double maxBaselineDistance = 0.0;
    double maxDistanceAboveBaseline = 0.0;
    double maxDistanceBelowBaseline = 0.0;
};

struct MagicLayoutNode {
    LayoutParam innerLayout = LayoutParam(Size(), Size());
    RefPtr<RenderNode> node;
};

class ACE_EXPORT RenderFlex : public RenderNode {
    DECLARE_ACE_TYPE(RenderFlex, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    FlexAlign GetJustifyContent() const
    {
        return mainAxisAlign_;
    }

    FlexAlign GetAlignItems() const
    {
        return crossAxisAlign_;
    }

    double GetSpace() const
    {
        return space_;
    }

    Dimension GetInspectorSpace() const
    {
        return inspectorSpace_;
    }

    TextBaseline GetBaseline() const
    {
        return textBaseline_;
    }

    bool IsStretchToParent() const
    {
        return stretchToParent_;
    }

    FlexAlign GetAlignItem() const
    {
        return crossAxisAlign_;
    }

    AlignDeclarationPtr GetAlignPtr() const
    {
        return alignPtr_;
    }
    double GetBaselineDistance(TextBaseline baseline) override;

    Size GetChildViewPort() override;

    void OnChildRemoved(const RefPtr<RenderNode>& child) override;

    void Dump() override;

    bool IsChildOverflow() const
    {
        return isChildOverflow_;
    }

    bool GetAlignDeclarationOffset(AlignDeclarationPtr alignDeclarationPtr, Offset& offset) const override;

    bool CheckIfNeedLayoutAgain() override;

protected:
    void ClearRenderObject() override;
    bool MaybeRelease() override;

    Overflow overflow_ = Overflow::CLIP;
    bool isChildOverflow_ = false;

private:
    void PerformLayoutInWeightMode();
    void PerformLayoutInIndexMode();
    void PerformLayoutInItemMode();

    void LayoutMagicNodes(BaselineProperties& baselineProperties);
    void RelayoutForStretchMagicNode();
    void RelayoutForStretchFlexNode(const FlexItemProperties& flexItemProperties);
    void LayoutHiddenNodes();

    /**
     * This function is used to travel a single flex item in the first time layout to determine the flexItemProperties.
     *
     * @param flexItem The item under operation.
     * @param flexItemProperties Store all the flex item properties.
     */
    void LayoutFlexItem(RefPtr<RenderFlexItem>& flexItem, FlexItemProperties& flexItemProperties);

    void RelayoutFlexItem(const RefPtr<RenderFlexItem>& flexItem, double flexSize, BaselineProperties& baselineProps,
        double& allocatedFlexSpace);

    void LayoutInfinityChild(const RefPtr<RenderNode>& item, double mainSize, BaselineProperties& baselineProperties);

    /**
     * This function is used to determine all the items' sizes in the second time layout according to the
     * flexItemProperties. The size of Flex(Row/Column) will be resized by the flex item.
     *
     * @param flexItemProps FlexItemProperties.
     * @param baselineProps The distance properties of baseline.
     */
    void ResizeItems(const FlexItemProperties& flexItemProps, BaselineProperties& baselineProps);

    void DetermineSelfSize(MainAxisSize mainAxisSize, bool useViewPort);

    void DetermineItemsPosition(const BaselineProperties& baselineProperties);

    /**
     * This function is used to calculate the frontSpace and the betweenSpace according to mainAxisAlign and
     * remainSpace. Only when the allocated size is smaller than the maxMainSize, it can be effective. In other word, it
     * requires all the items are not flex items.
     *
     * @param remainSpace Remain space.
     * @param frontSpace The space before the first item.
     * @param betweenSpace The space between two items.
     */
    void CalculateSpace(double remainSpace, double& frontSpace, double& betweenSpace) const;

    /**
     * This function is used to place all the items according to the size and the space.
     *
     * @param frontSpace The space before the first item.
     * @param betweenSpace The space between two items.
     * @param baselineProperties The distance properties of baseline.
     */
    void PlaceChildren(double frontSpace, double betweenSpace, const BaselineProperties& baselineProperties);

    void LayoutAbsoluteChildren();

    void CheckBaselineProperties(const RefPtr<RenderNode>& item, BaselineProperties& baselineProperties);

    LayoutParam MakeStretchInnerLayoutParam(const RefPtr<RenderNode>& item) const;
    LayoutParam MakeLayoutParamWithLimit(double minMainLimit, double maxMainLimit, bool isStretch) const;
    LayoutParam MakeConstrainedLayoutParam(double mainFlexExtent, const LayoutParam& constraint, bool isStretch,
        bool supportZerolayout = false) const;

    void ResizeByItem(const RefPtr<RenderNode>& item, double &allocatedSize);
    void CheckSizeValidity(const RefPtr<RenderNode>& item);
    Size GetConstrainedSize(double mainSize);
    double GetAvailableMainSize();
    double GetMainSize(const RefPtr<RenderNode>& item) const;
    double GetCrossSize(const RefPtr<RenderNode>& item) const;
    double GetStretchCrossLimit() const;
    void InitFlexProperties();
    void TravelChildrenFlexProps();
    void ClearChildrenLists();
    FlexAlign GetSelfAlign(const RefPtr<RenderNode>& item) const;
    TextDirection AdjustTextDirectionByDir();
    void UpdateAccessibilityAttr();
    void OnPaintFinish() override;

    bool IsStartTopLeft(FlexDirection direction, TextDirection textDir) const;

    void PerformItemAlign(std::list<RefPtr<RenderNode>>& nodelist);

    FlexDirection direction_ = FlexDirection::ROW;
    FlexAlign mainAxisAlign_ = FlexAlign::FLEX_START;
    FlexAlign crossAxisAlign_ = FlexAlign::FLEX_START;
    MainAxisSize mainAxisSize_ = MainAxisSize::MAX;
    CrossAxisSize crossAxisSize_ = CrossAxisSize::MIN;
    TextBaseline textBaseline_ = TextBaseline::ALPHABETIC;
    FlexLayoutMode layoutMode_ = FlexLayoutMode::FLEX_ITEM_MODE;
    bool stretchToParent_ = false;

    double mainSize_ = 0.0;
    double crossSize_ = 0.0;
    double allocatedSize_ = 0.0;

    double space_ = 0.0;
    Dimension inspectorSpace_;

    std::set<RefPtr<RenderNode>> infinityLayoutNodes_;
    std::set<RefPtr<RenderNode>> absoluteNodes_;
    std::list<RefPtr<RenderNode>> relativeNodes_;
    std::list<RefPtr<RenderFlexItem>> stretchNodes_;
    // use map to order the magic Nodes
    std::map<int32_t, std::list<MagicLayoutNode>> magicNodes_;
    std::map<int32_t, double> magicWeightMaps_;
    std::set<RefPtr<RenderNode>> displayNodes_; // displayNodes_ used to record all display nodes in magic layout

    RefPtr<RenderNode> scrollNode;
    bool isMainInfinite_ = false;
    bool isCrossInfinite_ = false;
    bool useViewPort_ = false;
    bool containsNavigation_ = false;
    double navigationMainSize_ = 0.0;
    int32_t validSizeCount_ = 0;
    double totalFlexWeight_ = 0.0;
    int32_t maxDisplayIndex_ = 0;
    bool useOldLayoutVersion_ = false;
    bool isDeclarative_ = false;

    AlignDeclarationPtr alignPtr_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_RENDER_FLEX_H
