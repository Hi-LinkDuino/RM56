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

#include "core/image/animated_image_player.h"

#include "third_party/skia/include/codec/SkCodecAnimation.h"
#include "third_party/skia/include/core/SkPixelRef.h"

#include "base/log/log.h"
#include "core/components/image/flutter_render_image.h"
#include "core/image/image_provider.h"

namespace OHOS::Ace {

void AnimatedImagePlayer::Pause()
{
    animator_->Pause();
}

void AnimatedImagePlayer::Resume()
{
    animator_->Resume();
}

void AnimatedImagePlayer::RenderFrame(const int32_t& index)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("Context may be destroyed!");
        return;
    }
    auto taskExecutor = context->GetTaskExecutor();
    taskExecutor->PostTask(
        [weak = AceType::WeakClaim(this), index, dstWidth = dstWidth_, dstHeight = dstHeight_, taskExecutor] {
            auto player = weak.Upgrade();
            if (!player) {
                return;
            }
            auto canvasImage = flutter::CanvasImage::Create();
            sk_sp<SkImage> skImage = player->DecodeFrameImage(index);
            if (dstWidth > 0 && dstHeight > 0) {
                skImage = ImageProvider::ApplySizeToSkImage(skImage, dstWidth, dstHeight);
            }
            if (skImage) {
                canvasImage->set_image({ skImage, player->unrefQueue_ });
            } else {
                LOGW("animated player cannot get the %{public}d skImage!", index);
                return;
            }
            taskExecutor->PostTask([callback = player->successCallback_, canvasImage,
                                       source = player->imageSource_] { callback(source, canvasImage); },
                TaskExecutor::TaskType::UI);
        },
        TaskExecutor::TaskType::IO);
}

sk_sp<SkImage> AnimatedImagePlayer::DecodeFrameImage(const int32_t& index)
{
    // first seek in cache
    auto iterator = cachedFrame_.find(index);
    if (iterator != cachedFrame_.end() && iterator->second != nullptr) {
        LOGD("index %{private}d found in cache.", index);
        return SkImage::MakeFromBitmap(*iterator->second);
    }

    SkBitmap bitmap;
    SkImageInfo info = codec_->getInfo().makeColorType(kN32_SkColorType);
    bitmap.allocPixels(info);
    SkCodec::Options options;
    options.fFrameIndex = index;
    const int32_t requiredFrame = frameInfos_[index].fRequiredFrame;
    if (requiredFrame != SkCodec::kNoFrame) {
        if (requiredFrame == lastRequiredFrameIndex_ && lastRequiredBitmap_ && lastRequiredBitmap_->getPixels() &&
            CopyTo(&bitmap, lastRequiredBitmap_->colorType(), *lastRequiredBitmap_)) {
            options.fPriorFrame = requiredFrame;
        } else if (requiredFrame != lastRequiredFrameIndex_) {
            // find requiredFrame in cached frame.
            auto iter = cachedFrame_.find(requiredFrame);
            if (iter != cachedFrame_.end() && iter->second != nullptr &&
                CopyTo(&bitmap, iter->second->colorType(), *iter->second)) {
                options.fPriorFrame = requiredFrame;
            }
        }
    }

    if (SkCodec::kSuccess != codec_->getPixels(info, bitmap.getPixels(), bitmap.rowBytes(), &options)) {
        LOGW("Could not getPixels for frame %{public}d:", index);
        return nullptr;
    }

    if (frameInfos_[index].fDisposalMethod != SkCodecAnimation::DisposalMethod::kRestorePrevious) {
        lastRequiredBitmap_ = std::make_unique<SkBitmap>(bitmap);
        lastRequiredFrameIndex_ = index;
    }

    if (iterator != cachedFrame_.end() && iterator->second == nullptr) {
        LOGD("index %{private}d cached.", index);
        iterator->second = std::make_unique<SkBitmap>(bitmap);
    }
#ifndef GPU_DISABLED
    // weak reference of io manager must be check and used on io thread, because io manager is created on io thread.
    if (ioManager_) {
        auto resourceContext = ioManager_->GetResourceContext();
        if (resourceContext) {
            SkPixmap pixmap(bitmap.info(), bitmap.pixelRef()->pixels(), bitmap.pixelRef()->rowBytes());
            return SkImage::MakeCrossContextFromPixmap(resourceContext.get(), pixmap, true, pixmap.colorSpace());
        }
    }
#endif
    return SkImage::MakeFromBitmap(bitmap);
}

bool AnimatedImagePlayer::CopyTo(SkBitmap* dst, SkColorType dstColorType, const SkBitmap& src)
{
    SkPixmap srcPixmap;
    if (!src.peekPixels(&srcPixmap)) {
        return false;
    }
    SkBitmap tempDstBitmap;
    SkImageInfo dstInfo = srcPixmap.info().makeColorType(dstColorType);
    if (!tempDstBitmap.setInfo(dstInfo)) {
        return false;
    }
    if (!tempDstBitmap.tryAllocPixels()) {
        return false;
    }
    SkPixmap dstPixmap;
    if (!tempDstBitmap.peekPixels(&dstPixmap)) {
        return false;
    }
    if (!srcPixmap.readPixels(dstPixmap)) {
        return false;
    }
    dst->swap(tempDstBitmap);
    return true;
}

} // namespace OHOS::Ace
