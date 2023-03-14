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
#ifndef OHOS_WIFI_AUTH_CENTER_TEST_H
#define OHOS_WIFI_AUTH_CENTER_TEST_H

#include <gtest/gtest.h>
#include "wifi_auth_center.h"

namespace OHOS {
namespace Wifi {
const int ARRAY_PERMISSION = 9;
const int PID_NUM_1001 = 1001;
const int UID_NUM_1001 = 1001;
class WifiAuthCenterTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        WifiAuthCenter::GetInstance().Init();
    }
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pid = PID_NUM_1001;
        uid = UID_NUM_1001;
    }
    virtual void TearDown()
    {}

public:
    int pid;
    int uid;
};
}  // namespace Wifi
}  // namespace OHOS
#endif