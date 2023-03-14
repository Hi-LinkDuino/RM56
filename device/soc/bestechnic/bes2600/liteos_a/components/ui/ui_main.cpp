/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
// #include "hal_trace.h"
#include "ui_main.h"
#include "cmsis_os.h"
#include "pthread.h"
#include "core/render_manager.h"
#include "common/graphic_startup.h"
#include "common/image_decode_ability.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "display_device.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#include "font/ui_font_vector.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "hal_tick.h"
#include "touch_input.h"

#define ENABLE_FPS
#ifdef ENABLE_ACE
#include "product_adapter.h"
#endif

#define FONT_MEM_LEN (512 * 1024)
static uint8_t g_fontMemBaseAddr[FONT_MEM_LEN];
#if ENABLE_ICU
static uint8_t g_icuMemBaseAddr[SHAPING_WORD_DICT_LENGTH];
#endif

using namespace OHOS;

static void InitFontEngine()
{
#if ENABLE_VECTOR_FONT
    GRAPHIC_LOGD("InitFontEngine fontMemSize: 0x%x", FONT_MEM_LEN);
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontMemBaseAddr),
                                   FONT_MEM_LEN, VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
#endif

#if ENABLE_ICU
    GraphicStartUp::InitLineBreakEngine(reinterpret_cast<uintptr_t>(g_icuMemBaseAddr), SHAPING_WORD_DICT_LENGTH,
                                        VECTOR_FONT_DIR, DEFAULT_LINE_BREAK_RULE_FILENAME);
#endif
}

static void InitImageDecodeAbility()
{
    uint32_t imageType = IMG_SUPPORT_BITMAP | OHOS::IMG_SUPPORT_JPEG | OHOS::IMG_SUPPORT_PNG;
    ImageDecodeAbility::GetInstance().SetImageDecodeAbility(imageType);
}

static void InitHal()
{
    DisplayDevice *display = DisplayDevice::GetInstance();
    BaseGfxEngine::InitGfxEngine(display);

    TouchInput *touch = TouchInput::GetInstance();
    InputDeviceManager::GetInstance()->Add(touch);
}

void InitUiKit(void)
{
    GraphicStartUp::Init();
    // init display/input device
    InitHal();
    // init font engine
    InitFontEngine();
    // init suppot image format
    InitImageDecodeAbility();
}

__attribute__((weak)) void RunApp(void)
{
    GRAPHIC_LOGI("RunApp default");
}

#ifdef ENABLE_ACE
static void RenderTEHandler()
{
}
#endif

static constexpr uint32_t UI_MAIN_TASK_DELAY = 5000;
static constexpr uint32_t ONE_SECOND = 1000;
static void *UiMainTask(void *arg)
{
    (void)arg;

    (void)pthread_setname_np(pthread_self(), "UiMain");
    osDelay(UI_MAIN_TASK_DELAY);

    InitUiKit();

    RunApp();

#ifdef ENABLE_ACE
    const ACELite::TEHandlingHooks hooks = {RenderTEHandler, nullptr};
    ACELite::ProductAdapter::RegTEHandlers(hooks);
#endif
#if ENABLE_FPS_SUPPORT
    uint32_t start = HALTick::GetInstance().GetTime();
#endif
    while (1) {
#ifdef ENABLE_ACE
        // Here render all js app in the same task.
        ACELite::ProductAdapter::DispatchTEMessage();
#endif
#if FULLY_RENDER
        DisplayDevice::GetInstance()->UpdateFBBuffer();
#endif
        uint32_t temp = HALTick::GetInstance().GetTime();
        TaskManager::GetInstance()->TaskHandler();
        uint32_t time = HALTick::GetInstance().GetElapseTime(temp);
        if (time < DEFAULT_TASK_PERIOD) {
            osDelay(DEFAULT_TASK_PERIOD - time);
        }
#if ENABLE_FPS_SUPPORT
        if (HALTick::GetInstance().GetElapseTime(start) >= ONE_SECOND) {
            GRAPHIC_LOGD("%u fps", (uint32_t)RenderManager::GetInstance().GetFPS());
            start = HALTick::GetInstance().GetTime();
        }
#endif
    }

    return nullptr;
}
#include "los_task.h"
#define UI_THREAD_STACK_SIZE (1024 * 32)
void UiMain(void)
{
    struct sched_param param = {0};
    printf(" to create UiMainTask\r\n");

    pthread_t thread;
    pthread_attr_t attr;
    (void)pthread_attr_init(&attr);
    (void)pthread_attr_setstacksize(&attr, UI_THREAD_STACK_SIZE);
    param.sched_priority = 15; // 15: UiMainTask priority
    (void)pthread_attr_setschedparam(&attr, &param);
    (void)pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (pthread_create(&thread, &attr, UiMainTask, nullptr) != 0) {
        printf("Failed to create UiMainTask");
    }
    LOS_TaskCpuAffiSet(thread, CPUID_TO_AFFI_MASK(0));
}
