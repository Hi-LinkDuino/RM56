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
#ifndef OHOS_MOCK_MAC_ADDRESS_H
#define OHOS_MOCK_MAC_ADDRESS_H

#include <gmock/gmock.h>

namespace OHOS {
namespace Wifi {
class MockMacAddress {
public:
    virtual ~MockMacAddress() = default;

    virtual bool IsValidMac(const char *mac) = 0;

};

class MacAddress : public MockMacAddress {
public:
    static MacAddress &GetInstance(void);

    MOCK_METHOD1(IsValidMac, bool(const char *mac));
};
}  // namespace Wifi
}  // namespace OHOS
#endif