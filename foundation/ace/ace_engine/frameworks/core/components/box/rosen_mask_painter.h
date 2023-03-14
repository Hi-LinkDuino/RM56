/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROSEN_MASK_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROSEN_MASK_PAINTER_H

#include "core/components/box/mask.h"
#include "core/image/image_provider.h"
#include "core/pipeline/pipeline_context.h"
#include "experimental/svg/model/SkSVGDOM.h"

namespace OHOS::Rosen {
    class RSMask;
}

namespace OHOS::Ace {
enum class LoadStatus {
    UNLOADED = 0,
    LOADING,
    UPDATING,
    LOADSUCCESS,
    LOADFAIL,
};

class RosenMaskPainter : public Mask {
    DECLARE_ACE_TYPE(RosenMaskPainter, Mask);

public:
    RosenMaskPainter() = default;
    ~RosenMaskPainter() = default;

    void LoadMask(const WeakPtr<PipelineContext>& context, const RefPtr<RenderNode>& parent) override;
    bool HasReady() const;
    std::shared_ptr<Rosen::RSMask> GetRSMask(const Rect& paintRect, const SkPath& path);
    void SetFetchImageObjBackgroundTask(CancelableTask task);

protected:
    void LoadSVGImage(const WeakPtr<PipelineContext>& context);
    void LoadGradient(const WeakPtr<PipelineContext>& context);
    void LoadPath(const WeakPtr<PipelineContext>& context);
    void UpdateSVGScale(const Size& boxSize, Size& svgSize);
    bool UpadteSVGImageDom(double& x, double& y);
    bool GetPathPaint(SkPaint& paint);
    bool GetGradientPaint(const Rect& paintRect, SkPaint& paint);

    RefPtr<RenderNode> parent_;
    LoadStatus loadStatus_ = LoadStatus::UNLOADED;
    CancelableTask fetchSvgImageTask_;

    // draw gradient
    RefPtr<Decoration> decoration_ = nullptr;
    double dipScale_ = 1.0f;

    // svg image draw object
    sk_sp<SkSVGDOM> skiaDom_;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROSEN_MASK_PAINTER_H