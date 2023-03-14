/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HKS_CRYPTO_HAL_COMMON_H
#define HKS_CRYPTO_HAL_COMMON_H

#include "hks_type_inner.h"

class HksCryptoHalCommon {
public:
    HksCryptoHalCommon()
    {}
    virtual ~HksCryptoHalCommon()
    {}

protected:
    uint8_t HexChar(char hex) const;
    uint8_t ReadHex(const uint8_t hex[2]) const;

    const int HKS_COUNT_OF_HALF = 2;
    const uint32_t HKS_PADDING_SUPPLENMENT = 16;
    const uint32_t HKS_UPDATE_DATA_MAX = 64;
    const uint32_t HKS_CONTEXT_DATA_MAX = (1024 * 2);
};
#endif  // HKS_CRYPTO_HAL_COMMON_H
