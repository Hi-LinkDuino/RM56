/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "raw_address.h"
#include <vector>
#include "securec.h"
#include <cstdlib>
#include "string"

namespace OHOS {
namespace bluetooth {
void RawAddress::ConvertToUint8(uint8_t *dst, const size_t size) const
{
    if (dst != nullptr && address_.length() == BT_ADDRESS_STR_LEN && size >= BT_ADDRESS_BYTE_LEN) {
        std::vector<std::string> token;
        std::size_t startPostion = 0;
        std::size_t colonPosition = address_.find(':', startPostion);
        while (colonPosition != std::string::npos) {
            token.push_back(address_.substr(startPostion, colonPosition - startPostion));
            startPostion = colonPosition + BT_COLON_BYTE_SIZE;
            colonPosition = address_.find(':', startPostion);
        }
        if (startPostion != BT_ADDRESS_STR_LEN) {
            token.push_back(address_.substr(startPostion));
        }
        if (token.size() != BT_ADDRESS_BYTE_LEN) {
            return;
        }
        for (int i = 0; i < BT_ADDRESS_BYTE_LEN; ++i) {
            char *tmp = nullptr;
            dst[i] = strtol(token[BT_ADDRESS_BYTE_LEN - 1 - i].c_str(), &tmp, BT_ADDRESS_STR_LEN - 1);
            if (tmp[0] != '\0') {
                return;
            }
        }
    }
}

RawAddress RawAddress::ConvertToString(const uint8_t *src, const size_t size)
{
    char token[BT_ADDRESS_STR_LEN + 1] = {0};
    if (size >= BT_ADDRESS_BYTE_LEN) {
        (void)sprintf_s(token,
            BT_ADDRESS_STR_LEN + 1,
            "%02X:%02X:%02X:%02X:%02X:%02X",
            src[BT_LAP_HIGH_BYTE],
            src[BT_LAP_MIDDLE_BYTE],
            src[BT_LAP_LOW_BYTE],
            src[BT_UAP_BYTE],
            src[BT_NAP_HIGH_BYTE],
            src[BT_NAP_LOW_BYTE]);
    }
    return RawAddress(token);
}

bool RawAddress::operator<(const RawAddress &rhs) const
{
    return (address_.compare(rhs.address_) < 0);
}

bool RawAddress::operator==(const RawAddress &rhs) const
{
    return (address_.compare(rhs.address_) == 0);
}
}  // namespace bluetooth
}  // namespace OHOS