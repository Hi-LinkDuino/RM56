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

#include "drag_window_ohos.h"

#include "base/log/log_wrapper.h"

namespace OHOS::Ace {
namespace {
sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(const RefPtr<PixelMap>& pixmap)
{
    return SkColorSpace::MakeSRGB(); // Media::PixelMap has not support wide gamut yet.
}

SkAlphaType AlphaTypeToSkAlphaType(const RefPtr<PixelMap>& pixmap)
{
    switch (pixmap->GetAlphaType()) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}

SkColorType PixelFormatToSkColorType(const RefPtr<PixelMap>& pixmap)
{
    switch (pixmap->GetPixelFormat()) {
        case PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}

SkImageInfo MakeSkImageInfoFromPixelMap(const RefPtr<PixelMap>& pixmap)
{
    SkColorType colorType = PixelFormatToSkColorType(pixmap);
    SkAlphaType alphaType = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> colorSpace = ColorSpaceToSkColorSpace(pixmap);
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), colorType, alphaType, colorSpace);
}

void DrawSkImage(SkCanvas* canvas, const RefPtr<PixelMap>& pixelmap, int32_t width, int32_t height)
{
    // Step1: Create SkPixmap
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixelmap);
    SkPixmap imagePixmap(imageInfo, reinterpret_cast<const void*>(pixelmap->GetPixels()), pixelmap->GetRowBytes());

    // Step2: Create SkImage and draw it
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    if (!skImage) {
        LOGE("sk image is nullptr");
        return;
    }
    SkPaint paint;
    sk_sp<SkColorSpace> colorSpace = skImage->refColorSpace();
#ifdef USE_SYSTEM_SKIA
    paint.setColor4f(paint.getColor4f(), colorSpace.get());
#else
    paint.setColor(paint.getColor4f(), colorSpace.get());
#endif
    auto skSrcRect = SkRect::MakeXYWH(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight());
    auto skDstRect = SkRect::MakeXYWH(0, 0, width, height);
    canvas->drawImageRect(skImage, skSrcRect, skDstRect, &paint);
}
} // namespace

RefPtr<DragWindow> DragWindow::CreateDragWindow(
    const std::string& windowName, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    OHOS::sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowRect({ x - static_cast<int32_t>(width) / 2, y - static_cast<int32_t>(height) / 2, width, height });
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_DRAGGING_EFFECT);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    OHOS::sptr<OHOS::Rosen::Window> dragWindow = OHOS::Rosen::Window::Create(windowName, option);
    if (!dragWindow) {
        LOGE("create drag window failed.");
        return nullptr;
    }

    OHOS::Rosen::WMError ret = dragWindow->Show();
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("DragWindow::CreateDragWindow, drag window Show() failed, ret: %d", ret);
    }

    auto window = AceType::MakeRefPtr<DragWindowOhos>(dragWindow);
    window->SetSize(width, height);
    return window;
}

void DragWindowOhos::MoveTo(int32_t x, int32_t y) const
{
    if (!dragWindow_) {
        LOGE("DragWindowOhos::MoveTo, the drag window is null.");
        return;
    }

    OHOS::Rosen::WMError ret = dragWindow_->MoveTo(x + offsetX_ - width_ / 2, y + offsetY_ - height_ / 2);
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("DragWindow::MoveTo, drag window move failed, ret: %d", ret);
        return;
    }

    ret = dragWindow_->Show();
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("DragWindow::CreateDragWindow, drag window Show() failed, ret: %d", ret);
    }
}

void DragWindowOhos::Destory() const
{
    if (!dragWindow_) {
        LOGE("DragWindowOhos::Destroy, the drag window is null.");
        return;
    }

    OHOS::Rosen::WMError ret = dragWindow_->Destroy();
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("DragWindow::Destroy, drag window destroy failed, ret: %d", ret);
    }
}

void DragWindowOhos::DrawPixelMap(const RefPtr<PixelMap>& pixelmap)
{
    if (!pixelmap) {
        LOGE("the pixmap is nullptr");
        return;
    }
    auto rect = dragWindow_->GetRect();
    auto surfaceNode = dragWindow_->GetSurfaceNode();
    rsUiDirector_ = Rosen::RSUIDirector::Create();
    rsUiDirector_->Init();
    auto transactionProxy = Rosen::RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    rsUiDirector_->SetRSSurfaceNode(surfaceNode);
    rootNode_ = Rosen::RSRootNode::Create();
    rootNode_->SetBounds(0, 0, rect.width_, rect.height_);
    rootNode_->SetFrame(0, 0, rect.width_, rect.height_);
    rsUiDirector_->SetRoot(rootNode_->GetId());
    auto canvasNode = std::static_pointer_cast<Rosen::RSCanvasNode>(rootNode_);
    auto skia = canvasNode->BeginRecording(rect.width_, rect.height_);
    DrawSkImage(skia, pixelmap, rect.width_, rect.height_);
    canvasNode->FinishRecording();
    rsUiDirector_->SendMessages();
}
} // namespace OHOS::Ace