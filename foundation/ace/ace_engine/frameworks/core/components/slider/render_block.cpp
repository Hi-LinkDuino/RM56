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

#include "core/components/slider/render_block.h"

namespace OHOS::Ace {

void RenderBlock::Update(const RefPtr<Component>& component)
{
    const RefPtr<BlockComponent> block = AceType::DynamicCast<BlockComponent>(component);
    if (block == nullptr) {
        LOGE("Slider::RenderBlock update with nullptr");
        return;
    }
    blockColor_ = block->GetBlockColor();
    blockSize_ = block->GetBlockSize();
    hotRegionWidth_ = block->GetHotRegionWidth();
    hotRegionHeight_ = block->GetHotRegionHeight();
    MarkNeedRender();
}

void RenderBlock::PerformLayout()
{
    SetLayoutSize(Measure());
}

Size RenderBlock::Measure()
{
    double rawWidth = NormalizeToPx(hotRegionWidth_);
    double rawHeight = NormalizeToPx(hotRegionHeight_);
    LOGD("Slider::RenderBlock Measure width: %{public}lf, height: %{public}lf", rawWidth, rawHeight);
    return Size(rawWidth, rawHeight);
}

} // namespace OHOS::Ace