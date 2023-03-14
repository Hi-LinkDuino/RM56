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

#ifndef DISPLAY_TEST_UTILS_H
#define DISPLAY_TEST_UTILS_H
#include <vector>
#include "display_test.h"
namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
void SaveFile(const char *fileName, uint8_t *data, int size);
void SetUint32(uint32_t &dst, uint32_t value);
void SetPixel(const BufferHandle &handle, int x, int y, uint32_t color);
void ClearColor(const BufferHandle &handle, uint32_t color);
uint32_t GetPixelValue(const BufferHandle &handle, int x, int y);
uint32_t CheckPixel(const BufferHandle &handle, int x, int y, uint32_t color);
std::vector<IRect> SplitBuffer(const BufferHandle &handle, std::vector<uint32_t> &colors);
} // OHOS
} // HDI
} // DISPLAY
} // TEST


#endif // HDI_TEST_RENDER_UTILS_H
