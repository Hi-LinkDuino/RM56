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

#ifndef HDI_TEST_RENDER_UTILS_H
#define HDI_TEST_RENDER_UTILS_H
#include "display_test.h"
#include "hdi_test_device_common.h"
namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
void SetUint32(uint32_t &dst, uint32_t value);
void SetPixel(const BufferHandle &handle, int x, int y, uint32_t color);
void ClearColor(const BufferHandle &handle, uint32_t color);
} // OHOS
} // HDI
} // DISPLAY
} // TEST


#endif // HDI_TEST_RENDER_UTILS_H
