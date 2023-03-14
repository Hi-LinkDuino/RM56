/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DM_RANDOM_H
#define OHOS_DM_RANDOM_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
int32_t GenRandInt(int32_t randMin, int32_t randMax);
int64_t GenRandLongLong(int64_t randMin, int64_t randMax);
int32_t GetRandomData(uint8_t *randStr, uint32_t len);
bool MbedtlsGenRandomStr(char *szOut, int32_t szOutLen, bool numberOnly);
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DM_RANDOM_H
