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

#include "core/pipeline/base/flutter_render_context.h"

#include "core/components/plugin/render_plugin.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/base/render_sub_container.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t OVERFLOW_PLATFORM_VERSION = 7;

inline bool ShouldPaint(const RefPtr<RenderNode>& node)
{
    return node != nullptr && node->GetVisible() && !node->GetHidden();
}

} // namespace

using namespace Flutter;

FlutterRenderContext::~FlutterRenderContext()
{
    StopRecordingIfNeeded();
}

void FlutterRenderContext::Repaint(const RefPtr<RenderNode>& node)
{
    if (!ShouldPaint(node) || !node->NeedRender() || !node->GetRenderLayer()) {
        return;
    }
    InitContext(node->GetRenderLayer(), node->GetRectWithShadow());
    node->RenderWithContext(*this, Offset::Zero());
    StopRecordingIfNeeded();
}

void FlutterRenderContext::PaintChild(const RefPtr<RenderNode>& child, const Offset& offset)
{
    if (!ShouldPaint(child)) {
        return;
    }

    bool canChildOverflow = false;
    auto pipeline = child->GetContext().Upgrade();
    if (pipeline) {
        canChildOverflow = pipeline->GetMinPlatformVersion() >= OVERFLOW_PLATFORM_VERSION;
    }

    Rect rect = child->GetTransitionPaintRect() + offset;
    if (!(child->IsPaintOutOfParent() || canChildOverflow) && !estimatedRect_.IsIntersectWith(rect)) {
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        child->ClearAccessibilityRect();
#endif
        return;
    }

    if (child->GetRenderLayer()) {
        StopRecordingIfNeeded();
        std::string name = AceType::TypeName(child);
        if (name != "FlutterRenderForm" && name != "FlutterRenderPlugin") {
            if (child->NeedRender()) {
                FlutterRenderContext context;
                auto pipelineContext = child->GetContext().Upgrade();
                auto transparentHole = pipelineContext->GetTransparentHole();
                if (transparentHole.IsValid() && child->GetNeedClip()) {
                    Offset childOffset = rect.GetOffset();
                    Rect hole = transparentHole - childOffset;
                    context.SetClipHole(hole);
                }
                context.Repaint(child);
            } else {
                // No need to repaint, notify to update AccessibilityNode info.
                child->NotifyPaintFinish();
            }
        }
        // add child layer to parent layer
        OffsetLayer* layer = CastLayerAs<OffsetLayer>(child->GetRenderLayer());
        if (!layer) {
            LOGE("layer is null");
            return;
        }
        Offset pos = rect.GetOffset();
        if (name != "FlutterRenderForm" && name != "FlutterRenderPlugin") {
            layer->SetOffset(pos.GetX(), pos.GetY());
        } else {
            SetOffSet(child, layer, pos, name);
        }
        containerLayer_->AddChildren(AceType::Claim(layer));
    } else {
        child->RenderWithContext(*this, rect.GetOffset());
    }
}

void FlutterRenderContext::SetOffSet(
    const RefPtr<RenderNode>& child, OffsetLayer* layer, const Offset& pos, const std::string& name)
{
    if (!child || !layer) {
        LOGE("child is nullptr, or layer is nullptr");
        return;
    }

    auto renderPost = child->GetGlobalOffset();
    auto context = child->GetContext().Upgrade();
    if (context) {
        auto density = context->GetDensity();
        auto parent = child->GetParent();
        Offset pluginOffset = { 0, 0 };
        if (!NearZero(density)) {
            if (parent.Upgrade() && parent.Upgrade()->GetRenderLayer()) {
                layer->SetOffset(
                    renderPost.GetX() / density - renderPost.GetX(), renderPost.GetY() / density - renderPost.GetY());
                pluginOffset = { renderPost.GetX() / density - renderPost.GetX(),
                    renderPost.GetY() / density - renderPost.GetY() };
            } else {
                layer->SetOffset(pos.GetX() / density, pos.GetY() / density);
                pluginOffset = { pos.GetX() / density, pos.GetY() / density };
            }
        }
        // plugin offset
        if (name == "FlutterRenderPlugin" || name == "FlutterRenderForm") {
            auto renderPlugin = AceType::DynamicCast<RenderSubContainer>(child);
            if (!renderPlugin) {
                return;
            }
            auto pluginContext = renderPlugin->GetSubPipelineContext();
            if (!pluginContext) {
                return;
            }
            pluginContext->SetPluginEventOffset(renderPost);
            pluginContext->SetPluginOffset(pluginOffset);
        }
    }
}

void FlutterRenderContext::StartRecording()
{
    currentLayer_ = AceType::MakeRefPtr<PictureLayer>();
    recorder_ = flutter::PictureRecorder::Create();
    canvas_ = flutter::Canvas::Create(
        recorder_.get(), estimatedRect_.Left(), estimatedRect_.Top(), estimatedRect_.Right(), estimatedRect_.Bottom());
    if (clipHole_.IsValid()) {
        canvas_->save();
        needRestoreHole_ = true;
        canvas_->clipRect(
            clipHole_.Left(), clipHole_.Top(), clipHole_.Right(), clipHole_.Bottom(), SkClipOp::kDifference);
    }
    containerLayer_->AddChildren(currentLayer_);
}

void FlutterRenderContext::StopRecordingIfNeeded()
{
    if (!IsRecording()) {
        return;
    }

    if (needRestoreHole_) {
        canvas_->restore();
        needRestoreHole_ = false;
    }
    currentLayer_->SetPicture(recorder_->endRecording());
    currentLayer_ = nullptr;
    recorder_ = nullptr;
    canvas_ = nullptr;
}

bool FlutterRenderContext::IsIntersectWith(const RefPtr<RenderNode>& child, Offset& offset)
{
    if (!ShouldPaint(child)) {
        return false;
    }

    Rect rect = child->GetTransitionPaintRect() + offset;
    if (!estimatedRect_.IsIntersectWith(rect)) {
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        child->ClearAccessibilityRect();
#endif
        return false;
    }

    offset = rect.GetOffset();
    return true;
}

void FlutterRenderContext::InitContext(RenderLayer layer, const Rect& rect)
{
    estimatedRect_ = rect;
    containerLayer_ = CastLayerAs<ContainerLayer>(layer);
    containerLayer_->RemoveChildren();
}

flutter::Canvas* FlutterRenderContext::GetCanvas()
{
    if (!IsRecording()) {
        StartRecording();
    }
    return canvas_.get();
}

void FlutterRenderContext::Restore()
{
    auto canvas = GetCanvas();
    if (canvas != nullptr) {
        canvas->restore();
    }
}

} // namespace OHOS::Ace
