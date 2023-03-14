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
 * @addtogroup UI_DFX
 * @{
 *
 * @brief Provides test and analysis capabilities, such as stimulating input events and viewing information about a
 *        Document Object Model (DOM) tree.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_screenshot.h
 *
 * @brief Declares the screenshot function.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GRAPHIC_LITE_UI_SCREENSHOT_H
#define GRAPHIC_LITE_UI_SCREENSHOT_H

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "gfx_utils/heap_base.h"

namespace OHOS {
/**
 * @brief Provides callback functions for screenshot events.
 *
 * @since 1.0
 * @version 1.0
 */
class UIScreenshotListener;

/**
 * @brief Provides external screenshot functions.
 *
 * @since 1.0
 * @version 1.0
 */
class UIScreenshot : public HeapBase {
public:
    /**
     * @brief Obtains an instance in singleton pattern.
     *
     * @return Returns the screenshot function instance.
     * @since 1.0
     * @version 1.0
     */
    static UIScreenshot* GetInstance();

    /**
     * @brief Takes a screenshot and saves it to a file.
     *
     * @param path Indicates the pointer to the path for storing the screenshot file.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool ScreenshotToFile(const char* path);

private:
    UIScreenshotListener* screenshotListener_;

    UIScreenshot() : screenshotListener_(nullptr) {}
    virtual ~UIScreenshot();

    UIScreenshot(const UIScreenshot&) = delete;
    UIScreenshot& operator=(const UIScreenshot&) = delete;
    UIScreenshot(UIScreenshot&&) = delete;
    UIScreenshot& operator=(UIScreenshot&&) = delete;
};
} // namespace OHOS
#endif // ENABLE_DEBUG
#endif // GRAPHIC_LITE_UI_SCREENSHOT_H
