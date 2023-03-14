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

#include "monitor.h"

#include "common/graphic_startup.h"
#include "common/image_decode_ability.h"
#include "common/input_device_manager.h"
#include "draw/draw_utils.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#if ENABLE_VECTOR_FONT
#include "font/ui_font_vector.h"
#else
#include "common/ui_text_language.h"
#include "font/ui_font_bitmap.h"
#endif
#include "key_input.h"
#include "mouse_input.h"
#include "mousewheel_input.h"
#include "windows.h"

namespace OHOS {
bool Monitor::isRegister_ = false;

Monitor* Monitor::GetInstance()
{
    static Monitor instance;
    if (!isRegister_) {
        BaseGfxEngine::InitGfxEngine(&instance);
        isRegister_ = true;
    }
    return &instance;
}

void Monitor::InitHal()
{
#if USE_MOUSE
    MouseInput* mouse = MouseInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(mouse);
#endif

#if USE_MOUSEWHEEL && ENABLE_ROTATE_INPUT
    MousewheelInput* mousewheel = MousewheelInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(mousewheel);
#endif

#if USE_KEY
    KeyInput* key = KeyInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(key);
#endif
}

BufferInfo* Monitor::GetFBBufferInfo()
{
    static BufferInfo* bufferInfo = nullptr;
    if (bufferInfo == nullptr) {
        bufferInfo = new BufferInfo;
        bufferInfo->rect = {0, 0, HORIZONTAL_RESOLUTION - 1, VERTICAL_RESOLUTION - 1};
        bufferInfo->mode = ARGB8888;
        bufferInfo->color = 0x44;
        bufferInfo->phyAddr = bufferInfo->virAddr = tftFb_;
        // 3: Shift right 3 bits
        bufferInfo->stride = HORIZONTAL_RESOLUTION * (DrawUtils::GetPxSizeByColorMode(bufferInfo->mode) >> 3);
        bufferInfo->width = HORIZONTAL_RESOLUTION;
        bufferInfo->height = VERTICAL_RESOLUTION;
    }
    return bufferInfo;
}

void Monitor::Flush()
{
    UpdatePaint(tftFb_, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

// assuming below are the memory pool
static uint8_t g_fontMemBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH];
#if ENABLE_ICU
static uint8_t g_icuMemBaseAddr[OHOS::SHAPING_WORD_DICT_LENGTH];
#endif

void Monitor::InitFontEngine()
{
#if ENABLE_VECTOR_FONT
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontMemBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
#else
    BitmapFontInit();
    std::string dPath(_pgmptr);
    size_t len = dPath.size();
    size_t pos = dPath.find_last_of('\\');
    dPath.replace((pos + 1), (len - pos), "..\\..\\simulator\\font\\font.bin");
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontMemBaseAddr), MIN_FONT_PSRAM_LENGTH,
                                   dPath.c_str(), nullptr);
#endif

#if ENABLE_ICU
    GraphicStartUp::InitLineBreakEngine(reinterpret_cast<uintptr_t>(g_icuMemBaseAddr), SHAPING_WORD_DICT_LENGTH,
                                        VECTOR_FONT_DIR, DEFAULT_LINE_BREAK_RULE_FILENAME);
#endif
}

void Monitor::InitImageDecodeAbility()
{
    uint32_t imageType = IMG_SUPPORT_BITMAP | OHOS::IMG_SUPPORT_JPEG | OHOS::IMG_SUPPORT_PNG;
    ImageDecodeAbility::GetInstance().SetImageDecodeAbility(imageType);
}

void Monitor::GUILoopStart() const
{
    Sleep(GUI_REFR_PERIOD);
}

void Monitor::InitGUI()
{
    for (uint32_t i = 0; i < HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION; i++) {
        tftFb_[i] = defaultColor_;
    }
    UpdatePaint(tftFb_, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

void Monitor::GUILoopQuit() const {}
} // namespace OHOS
