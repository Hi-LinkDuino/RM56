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

#ifndef GRAPHIC_LITE_GRAPHIC_STARTUP_H
#define GRAPHIC_LITE_GRAPHIC_STARTUP_H
#include "gfx_utils/heap_base.h"

namespace OHOS {
class BaseFont;

/** @brief Entry of graphic, mainly used to start graphic. */
class GraphicStartUp : public HeapBase {
public:
    static void Init();

    static void InitFontEngine(uintptr_t cacheMemAddr, uint32_t cacheMemLen, const char* dPath, const char* ttfName);
    static void InitLineBreakEngine(uintptr_t cacheMemAddr, uint32_t cacheMemLen, const char* path,
                                    const char* fileName);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_STARTUP_H
