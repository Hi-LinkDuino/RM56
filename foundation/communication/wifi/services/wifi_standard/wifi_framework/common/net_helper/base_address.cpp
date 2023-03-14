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

#include "base_address.h"
#include "log_helper.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_UTIL_BaseAddress"

#define BUFFER_SIZE 64

namespace OHOS {
namespace Wifi {
BaseAddress::BaseAddress(const std::string &ip, size_t prefixLength, FamilyType family)
    : family_(family), prefixLength_(prefixLength), ipAddress_(ip)
{}

bool BaseAddress::operator==(const BaseAddress &address) const
{
    return (family_ == address.family_) && (ipAddress_ == address.ipAddress_);
}

BaseAddress::~BaseAddress()
{}

void BaseAddress::Dump() const
{
    std::string ipType;
    switch (family_) {
        case FamilyType::FAMILY_INET: {
            ipType = "IPv4";
            break;
        }
        case FamilyType::FAMILY_INET6: {
            ipType = "IPv6";
            break;
        }
        default:
            ipType = "NONE";
            break;
    }
    LOGI("TYPE: [%{public}s] address [%s/%zu]", ipType.c_str(), ipAddress_.c_str(), prefixLength_);
}
}  // namespace Wifi
}  // namespace OHOS