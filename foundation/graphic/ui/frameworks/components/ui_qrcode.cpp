/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_qrcode.h"
#include "qrcodegen.hpp"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

using qrcodegen::QrCode;
namespace OHOS {
UIQrcode::UIQrcode()
    : width_(0), needDraw_(false), backgroundColor_(Color::White()), qrColor_(Color::Black()), qrcodeVal_(nullptr)
{
    style_ = &(StyleDefault::GetBackgroundTransparentStyle());
    imageInfo_ = {{0}};
}

UIQrcode::~UIQrcode()
{
    if (qrcodeVal_ != nullptr) {
        UIFree(qrcodeVal_);
        qrcodeVal_ = nullptr;
    }

    if (imageInfo_.data != nullptr) {
        ImageCacheFree(imageInfo_);
        imageInfo_.data = nullptr;
    }
}

void UIQrcode::SetQrcodeInfo(const char* val, ColorType backgroundColor, ColorType qrColor)
{
    if (val == nullptr) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo val is null!\n");
        return;
    }
    uint32_t length = static_cast<uint32_t>(strlen(val));
    if ((length > QRCODE_VAL_MAX) || (length == 0)) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo val length is equal 0 or greater than QRCODE_VAL_MAX!\n");
        return;
    }
    backgroundColor_ = backgroundColor;
    qrColor_ = qrColor;
    SetQrcodeVal(val, length);
    RefreshQrcode();
}

void UIQrcode::RefreshQrcode()
{
    Invalidate();
    if (!needDraw_) {
        needDraw_ = true;
    }
}

void UIQrcode::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        UIView::SetWidth(width);
        RefreshQrcode();
    }
}

void UIQrcode::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        UIView::SetHeight(height);
        RefreshQrcode();
    }
}

void UIQrcode::ReMeasure()
{
    if (!needDraw_) {
        return;
    }
    needDraw_ = false;
    if (qrcodeVal_ == nullptr) {
        GRAPHIC_LOGE("UIQrcode::ReMeasure qrcodeVal_ is null!\n");
        return;
    }
    QrCode qr = QrCode::encodeText(qrcodeVal_, QrCode::Ecc::LOW);
    SetImageInfo(qr);
    SetSrc(&imageInfo_);
}

void UIQrcode::SetQrcodeVal(const char* qrcodeVal, uint32_t length)
{
    if (qrcodeVal_ != nullptr) {
        UIFree(qrcodeVal_);
        qrcodeVal_ = nullptr;
    }

    uint32_t len = static_cast<uint32_t>(length + 1);
    qrcodeVal_ = static_cast<char*>(UIMalloc(len));
    if (qrcodeVal_ != nullptr) {
        if (memcpy_s(qrcodeVal_, len, qrcodeVal, len) != EOK) {
            UIFree(reinterpret_cast<void*>(qrcodeVal_));
            qrcodeVal_ = nullptr;
        }
    }
}

void UIQrcode::SetImageInfo(qrcodegen::QrCode& qrcode)
{
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    width_ = (width >= height) ? height : width;
    if (width_ < qrcode.getSize()) {
        GRAPHIC_LOGE("UIQrcode::SetImageInfo width is less than the minimum qrcode width!\n");
        return;
    }
    imageInfo_.header.width = width;
    imageInfo_.header.height = height;
    imageInfo_.header.colorMode = ARGB8888;
    imageInfo_.dataSize = imageInfo_.header.width * imageInfo_.header.height * QRCODE_FACTOR_NUM;
    if (imageInfo_.data != nullptr) {
        ImageCacheFree(imageInfo_);
        imageInfo_.data = nullptr;
    }
    imageInfo_.data = reinterpret_cast<uint8_t*>(ImageCacheMalloc(imageInfo_));
    if (imageInfo_.data == nullptr) {
        GRAPHIC_LOGE("UIQrcode::SetImageInfo imageInfo_.data is null!\n");
        return;
    }
    GenerateQrCode(qrcode);
}

void UIQrcode::GenerateQrCode(qrcodegen::QrCode& qrcode)
{
    FillQrCodeBackgroundColor();

    FillQrCodeColor(qrcode);
}

void UIQrcode::FillQrCodeColor(qrcodegen::QrCode& qrcode)
{
    int32_t qrWidth = qrcode.getSize();
    if (qrWidth <= 0) {
        GRAPHIC_LOGE("UIQrcode::FillQrCodeColor generated qrcode size is less or equal 0!\n");
        return;
    }
    int16_t width = imageInfo_.header.width;
    int16_t height = imageInfo_.header.height;
    uint16_t outFilePixelPrescaler = width_ / qrWidth;
    int32_t offsetX = (width - outFilePixelPrescaler * qrWidth) / 2;    // 2: half
    int32_t offsetY = (height - outFilePixelPrescaler * qrWidth) / 2;   // 2: half

    uint8_t* destData = nullptr;
    int64_t oneLinePixel = width * QRCODE_FACTOR_NUM * outFilePixelPrescaler;
    int64_t oneLineOffsetPixel = (offsetY * width * QRCODE_FACTOR_NUM) + (offsetX * QRCODE_FACTOR_NUM);
    for (int32_t y = 0; y < qrWidth; ++y) {
        destData = const_cast<uint8_t*>(imageInfo_.data) + (oneLinePixel * y) + oneLineOffsetPixel;
        for (int32_t x = 0; x < qrWidth; ++x) {
            if (qrcode.getModule(x, y)) {
                GetDestData(destData, outFilePixelPrescaler);
            }
            destData += QRCODE_FACTOR_NUM * outFilePixelPrescaler;
        }
    }
}

void UIQrcode::FillQrCodeBackgroundColor()
{
    uint8_t* initColorData = const_cast<uint8_t*>(imageInfo_.data);
    *(initColorData + 0) = backgroundColor_.blue;  // 0: B channel
    *(initColorData + 1) = backgroundColor_.green; // 1: G channel
    *(initColorData + 2) = backgroundColor_.red;   // 2: R channel
    *(initColorData + 3) = OPA_OPAQUE;             // 3: Alpha channel

    uint8_t* tempColorData = initColorData;
    for (int16_t col = 1; col < imageInfo_.header.width; ++col) {
        initColorData += QRCODE_FACTOR_NUM;
        if (memcpy_s(initColorData, QRCODE_FACTOR_NUM, tempColorData, QRCODE_FACTOR_NUM) != EOK) {
            GRAPHIC_LOGE("UIQrcode::FillQrCodeBackgroundColor memcpy_s failed!\n");
            return;
        }
    }
    initColorData = tempColorData;
    int32_t deltaWidth = QRCODE_FACTOR_NUM * imageInfo_.header.width;
    for (int16_t row = 1; row < imageInfo_.header.height; ++row) {
        initColorData += deltaWidth;
        if (memcpy_s(initColorData, deltaWidth, tempColorData, deltaWidth) != EOK) {
            GRAPHIC_LOGE("UIQrcode::FillQrCodeBackgroundColor memcpy_s failed!\n");
            return;
        }
    }
}

void UIQrcode::GetDestData(uint8_t* destData, int32_t outFilePixelPrescaler)
{
    for (int32_t x = 0; x < outFilePixelPrescaler; ++x) {
        uint8_t* tempData = destData + imageInfo_.header.width * QRCODE_FACTOR_NUM * x;
        for (int32_t y = 0; y < outFilePixelPrescaler; ++y) {
            *(tempData + 0) = qrColor_.blue;  // 0: B channel
            *(tempData + 1) = qrColor_.green; // 1: G channel
            *(tempData + 2) = qrColor_.red;   // 2: R channel
            *(tempData + 3) = OPA_OPAQUE;     // 3: Alpha channel
            tempData += QRCODE_FACTOR_NUM;
        }
    }
}
} // namespace OHOS
