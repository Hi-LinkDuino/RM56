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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_ROSEN_RENDER_QRCODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_ROSEN_RENDER_QRCODE_H

#include "include/core/SkBitmap.h"
#include "qrcodegen.hpp"

#include "core/components/qrcode/render_qrcode.h"

namespace OHOS::Ace {

class RosenRenderQrcode : public RenderQrcode {
    DECLARE_ACE_TYPE(RosenRenderQrcode, RenderQrcode);

union BGRA {
    struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } argb;

    uint32_t value;
};

public:
    RosenRenderQrcode() = default;
    ~RosenRenderQrcode() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void DrawQRCode(RenderContext& context, const Offset& topLeft, int32_t size, const qrcodegen::QrCode& qrCode);
    uint32_t ConvertColorFromHighToLow(const Color& color);
    SkBitmap ProcessQrcodeData(int32_t width, const qrcodegen::QrCode& qrCode);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_ROSEN_RENDER_QRCODE_H
