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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROSEN_RENDER_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROSEN_RENDER_CONTEXT_H

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkRefCnt.h"

#include "base/geometry/rect.h"
#include "core/pipeline/base/render_context.h"
#include "core/pipeline/base/render_node.h"

class SkImage;
class SkPicture;
class SkPictureRecorder;

namespace OHOS::Ace {

class RosenRenderContext : public RenderContext {
    DECLARE_ACE_TYPE(RosenRenderContext, RenderContext)

public:
    RosenRenderContext() = default;
    ~RosenRenderContext() override;

    void Repaint(const RefPtr<RenderNode>& node) override;
    void PaintChild(const RefPtr<RenderNode>& child, const Offset& offset) override;
    bool IsIntersectWith(const RefPtr<RenderNode>& child, Offset& offset) override;
    void Restore() override;

    void InitContext(
        const std::shared_ptr<RSNode>& rsNode, const Rect& rect, const Offset& initialOffset = Offset::Zero());
    SkCanvas* GetCanvas();
    const std::shared_ptr<RSNode>& GetRSNode();

    void StartRecording();
    void StopRecordingIfNeeded();

    bool IsRecording()
    {
        return !!recordingCanvas_;
    }

    sk_sp<SkPicture> FinishRecordingAsPicture();
    sk_sp<SkImage> FinishRecordingAsImage();

private:
    std::shared_ptr<RSNode> rsNode_ = nullptr;
    SkPictureRecorder* recorder_ = nullptr;
    SkCanvas* recordingCanvas_ = nullptr;
    SkCanvas* rosenCanvas_ = nullptr;
    Rect estimatedRect_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROSEN_RENDER_CONTEXT_H
