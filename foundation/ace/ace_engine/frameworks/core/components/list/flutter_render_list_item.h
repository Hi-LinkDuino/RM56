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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_ITEM_H

#include "core/components/box/render_box.h"
#include "core/components/list/render_list_item.h"
#include "core/pipeline/layers/opacity_layer.h"
#include "core/pipeline/layers/transform_layer.h"

namespace flutter {
class Canvas;
class Paint;
class PaintData;
}

namespace OHOS::Ace {

const uint8_t DEFAULT_OPACITY = 255;

class FlutterRenderListItem : public RenderListItem {
    DECLARE_ACE_TYPE(FlutterRenderListItem, RenderListItem);

public:
    FlutterRenderListItem() = default;
    ~FlutterRenderListItem() override = default;

    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void OnGlobalPositionChanged() override;
    void OnPaintFinish() override;
    bool IsRepaintBoundary() const override
    {
        // Sticky item is specially paint in render list.
        return !GetSticky();
    }

    bool HasEffectiveTransform() const override;

private:
    void PaintWithFocusEffect(RenderContext& context, const Offset& offset);
    void PaintStickyEffect(RenderContext& context, const Offset& offset);
    void PaintStickyEffectNoTransparent(RenderContext& context, const Offset& offset);
    void PaintCurrentSticky(const RefPtr<Decoration>& background, const Offset& offset);
    void PaintNextSticky(const RefPtr<Decoration>& background, const Offset& offset);
    void PaintFadeOutEffect(const RefPtr<RenderNode>& node, const Offset& offset);
    void UpdateTransformLayer();
    void PaintItemDivider(RenderContext& context);
    void PaintActiveBackground(RenderContext& context, const Offset& offset);
    void PaintUniversalBackground(RenderContext& context, const Offset& offset, uint32_t colorVal);

    RefPtr<Flutter::TransformLayer> transformLayer_;
    double scale_ = 1.0;
    uint8_t opacity_ = DEFAULT_OPACITY;
    double decorationAlpha_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_ITEM_H
