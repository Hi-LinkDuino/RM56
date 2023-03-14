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

/**
 * @addtogroup UI_Common
 * @{
 *
 * @brief Defines common UI capabilities, such as image and text processing.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file screen.h
 *
 * @brief Declares the screen information.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_SCREEN_H
#define GRAPHIC_LITE_SCREEN_H

#include "gfx_utils/heap_base.h"
#include "gfx_utils/image_info.h"

namespace OHOS {
/**
 * @brief Represents the screen info of the device.
 *
 * @since 1.0
 * @version 1.0
 */
class Screen : HeapBase {
public:
    /**
     * @brief Obtains a singleton <b>Screen</b> instance.
     * @return Returns the <b>Screen</b> instance.
     */
    static Screen& GetInstance();

    /**
     * @brief Obtains the width of this screen.
     *
     * @return Returns the screen width.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetWidth();

    /**
     * @brief Obtains the height of this screen.
     *
     * @return Returns the screen height.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetHeight();

    /**
     * @brief 获取当前屏幕的bitmap截图.请注意该接口会申请内存，请在需要释放时使用{@link ImageCacheFree()}接口.
     * @param info bitmap存储对象，获取的截图将被存到该引用中.
     * @return bitmap是否获取成功.
     * @since 5.0
     * @version 3.0
     */
    bool GetCurrentScreenBitmap(ImageInfo& info);

    /**
     * @brief 获取当前屏幕的形状.
     * @return 当前屏幕形状.
     * @since 6
     */
    ScreenShape GetScreenShape();

private:
    Screen() {}
    virtual ~Screen() {}
};
} // namespace OHOS
#endif // GRAPHIC_LITE_SCREEN_H
