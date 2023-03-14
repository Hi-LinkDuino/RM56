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

#ifndef GRAPHIC_LITE_MONITOR_H
#define GRAPHIC_LITE_MONITOR_H

#include <QtCore/qobject.h>

#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font_header.h"

namespace OHOS {
class Monitor : public QObject, public BaseGfxEngine {
    Q_OBJECT

public:
    Monitor() : defaultColor_ (0x44) {}
    virtual ~Monitor() {}
    static Monitor* GetInstance();
    void InitHal();
    void InitFontEngine();
    void InitImageDecodeAbility();
    void InitGUI();
    void GUILoopStart() const;
    void GUIRefresh();
    void GUILoopQuit() const;
    void Flush() override;
    BufferInfo* GetFBBufferInfo() override;
signals:
    void UpdatePaintSignal(uint32_t* tftFb, uint32_t imgWidth, uint32_t imgHeight);

protected:
    void UpdatePaint(uint32_t* tftFb, uint32_t imgWidth, uint32_t imgHeight)
    {
        emit UpdatePaintSignal(tftFb, imgWidth, imgHeight);
    }

private:
    static constexpr uint32_t GUI_REFR_PERIOD  = 50; /* ms */
    uint8_t fontPsramBaseAddr_[MIN_FONT_PSRAM_LENGTH];
    uint32_t tftFb_[HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION];
    uint32_t animaterBuffer_[HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION];
    uint32_t defaultColor_;
    static bool isRegister_;
};
} // namespace OHOS

#endif // GRAPHIC_LITE_MONITOR_H
