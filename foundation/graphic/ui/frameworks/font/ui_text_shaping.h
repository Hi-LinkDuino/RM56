/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_LITE_TEXT_SHAPING_H
#define GRAPHIC_LITE_TEXT_SHAPING_H

#include "graphic_config.h"
#if ENABLE_SHAPING
#include "font/ui_font_header.h"
#include <cstdint>

enum ShapingScript {
    SHAPING_SCRIPT_ARABIC,
    SHAPING_SCRIPT_THAI,
    SHAPING_SCRIPT_DEVANAGARI,
    SHAPING_SCRIPT_HEBREW,
    SHAPING_SCRIPT_MYANMAR,
    SHAPING_SCRIPT_BENGALI,
    SHAPING_SCRIPT_INVALID
};

namespace OHOS {
class UITextShaping {
public:
    /**
     * @brief Construct a new UITextShaping object
     */
    UITextShaping();

    /**
     * @brief Destroy the UITextShaping object
     */
    ~UITextShaping();

    /**
     * @brief Get the Instance object
     * @return UITextShaping* the static instance
     */
    static UITextShaping* GetInstance();

    static bool IsSameTTFId(uint8_t fontId, uint32_t unicode);
}; // class UITextShaping
} // namespace OHOS
#endif // ENABLE_SHAPING
#endif // GRAPHIC_LITE_TEXT_SHAPING_H
