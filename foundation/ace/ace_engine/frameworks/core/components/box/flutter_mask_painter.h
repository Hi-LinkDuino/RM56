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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FlUTTER_MASK_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FlUTTER_MASK_PAINTER_H

#include <string>

#include "base/memory/ace_type.h"
#include "core/components/box/mask.h"
#include "core/components/common/properties/decoration.h"
#include "core/image/image_provider.h"
#include "core/pipeline/layers/clip_layer.h"
#include "core/pipeline/layers/container_layer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

enum class MaskLoadingStatus {
    UNLOADED = 0,
    LOADING,
    UPDATING,
    LOAD_SUCCESS,
    LOAD_FAIL,
};

class FlutterMaskPainter : public Mask {
    DECLARE_ACE_TYPE(FlutterMaskPainter, Mask);

public:
    FlutterMaskPainter() = default;
    ~FlutterMaskPainter() = default;

    void LoadMask(const WeakPtr<PipelineContext>& context, const RefPtr<RenderNode>& parent) override;

    bool GetSVGImageDom(RefPtr<Flutter::ClipLayer>& clipLayer);
    bool GetGradientPaint(const Rect& paintRect, RefPtr<Flutter::ClipLayer>& clipLayer);
    bool GetPathPaint(const Rect& paintRect, const SkPath& path, RefPtr<Flutter::ClipLayer>& clipLayer);

    bool HasReady() const;
    bool IsPaintSvgMask() const;

    void SetFetchImageObjBackgroundTask(CancelableTask task)
    {
        if (fetchSvgImageTask_) {
            fetchSvgImageTask_.Cancel(false);
        }
        fetchSvgImageTask_ = task;
    }

protected:
    void LoadSVGImage(const WeakPtr<PipelineContext>& context);
    void LoadGradient(const WeakPtr<PipelineContext>& context);
    void LoadPath(const WeakPtr<PipelineContext>& context);
    void UpdateSvgScale(const Size& boxSize, Size& svgSize);

    RefPtr<RenderNode> parent_;
    sk_sp<SkSVGDOM> skiaDom_;

    // svg image draw object
    MaskLoadingStatus maskLoadStatus_ = MaskLoadingStatus::UNLOADED;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;

    // draw gradient
    RefPtr<Decoration> decoration_ = nullptr;
    double dipScale_ = 1.0f;
    CancelableTask fetchSvgImageTask_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FlUTTER_MASK_PAINTER_H
