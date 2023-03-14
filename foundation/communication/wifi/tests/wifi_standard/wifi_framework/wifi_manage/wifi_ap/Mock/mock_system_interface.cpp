/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "mock_system_interface.h"
using namespace OHOS;
using namespace Wifi;

static bool mockTag = false;

MockSystemInterface &MockSystemInterface::GetInstance()
{
    static MockSystemInterface gMockSystemInterface;
    return gMockSystemInterface;
};

MockSystemInterface::MockSystemInterface()
{}

void MockSystemInterface::SetMockFlag(bool flag)
{
    mockTag = flag;
}

bool MockSystemInterface::GetMockFlag(void)
{
    return mockTag;
}

#ifdef __cplusplus
extern "C" {
#endif

int __real_system(const char *__command);
int __wrap_system(const char *__command)
{
    if (mockTag) {
        return MockSystemInterface::GetInstance().system(__command);
    } else {
        return __real_system(__command);
    }
}
}
