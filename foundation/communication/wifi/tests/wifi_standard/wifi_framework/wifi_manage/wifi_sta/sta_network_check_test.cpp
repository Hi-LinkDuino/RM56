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
#include "sta_network_check.h"

#include <gtest/gtest.h>
#include <memory>

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class StaNetworkCheckTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pStaNetworkCheck = std::make_unique<StaNetworkCheck>(handle);
        pStaNetworkCheck->InitNetCheckThread();
    }
    virtual void TearDown()
    {
        pStaNetworkCheck->ExitNetCheckThread();
        pStaNetworkCheck.reset();
    }

public:
    void SignalNetCheckThreadSuccess();
    void StopNetCheckThreadSuccess();
    void HttpDetectionSuccess();

public:
    std::unique_ptr<StaNetworkCheck> pStaNetworkCheck;
    NetStateHandler handle;
};

void StaNetworkCheckTest::SignalNetCheckThreadSuccess()
{
    pStaNetworkCheck->SignalNetCheckThread();
}

void StaNetworkCheckTest::StopNetCheckThreadSuccess()
{
    pStaNetworkCheck->StopNetCheckThread();
}

void StaNetworkCheckTest::HttpDetectionSuccess()
{
    pStaNetworkCheck->HttpDetection();
}

HWTEST_F(StaNetworkCheckTest, SignalNetCheckThreadSuccess, TestSize.Level1)
{
    SignalNetCheckThreadSuccess();
}

HWTEST_F(StaNetworkCheckTest, StopNetCheckThreadSuccess, TestSize.Level1)
{
    StopNetCheckThreadSuccess();
}

HWTEST_F(StaNetworkCheckTest, HttpDetectionSuccess, TestSize.Level1)
{
    HttpDetectionSuccess();
}
} // Wifi
} // OHOS
