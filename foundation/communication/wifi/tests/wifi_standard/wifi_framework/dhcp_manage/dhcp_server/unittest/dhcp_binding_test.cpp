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

#include <gtest/gtest.h>
#include <string>
#include <securec.h>
#include "dhcp_binding.h"
#include "common_util.h"

using namespace testing::ext;

HWTEST(DhcpBindingTest, NextPendingIntervalTest, TestSize.Level1)
{
    uint64_t pendingInterval = 0;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 1);
    pendingInterval = 2;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 1);
    pendingInterval = 3;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 2);
    pendingInterval = 9;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 2);
    pendingInterval = 10;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 5);
    pendingInterval = 11;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 5);
    pendingInterval = 29;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == pendingInterval + 5);
    pendingInterval = 30;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == 1200);
    pendingInterval = 31;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == 1200);
    pendingInterval = (uint64_t)-1;
    EXPECT_TRUE(NextPendingInterval(pendingInterval) == 1200);
}

HWTEST(DhcpBindingTest, IsExpireTest, TestSize.Level1)
{
    EXPECT_EQ(IsExpire(nullptr), DHCP_FALSE);
    AddressBinding binding;
    ASSERT_TRUE(memset_s(&binding, sizeof(binding), 0, sizeof(binding)) == EOK);
    EXPECT_EQ(IsExpire(&binding), DHCP_TRUE);
    uint64_t curr = Tmspsec();
    binding.leaseTime = 60;
    binding.bindingStatus = BIND_PENDING;
    binding.pendingTime = curr - binding.leaseTime + 5;
    EXPECT_EQ(IsExpire(&binding), DHCP_FALSE);
    binding.bindingStatus = BIND_PENDING;
    binding.pendingTime = curr - binding.leaseTime - 1;
    EXPECT_EQ(IsExpire(&binding), DHCP_TRUE);
    binding.bindingStatus = BIND_ASSOCIATED;
    binding.bindingTime = curr - binding.leaseTime + 5;
    EXPECT_EQ(IsExpire(&binding), DHCP_FALSE);
    binding.bindingStatus = BIND_ASSOCIATED;
    binding.bindingTime = curr - binding.leaseTime - 1;
    EXPECT_EQ(IsExpire(&binding), DHCP_TRUE);
}

HWTEST(DhcpBindingTest, WriteAddressBindingTest, TestSize.Level1)
{
    EXPECT_TRUE(WriteAddressBinding(nullptr, nullptr, 0) == RET_FAILED);
    AddressBinding binding;
    ASSERT_TRUE(memset_s(&binding, sizeof(binding), 0, sizeof(binding)) == EOK);
    for (int i = 0; i <= NUM_FIVE; ++i) {
        binding.chaddr[i] = (uint8_t)i;
    }
    binding.leaseTime = 60;
    binding.bindingTime = 10;
    binding.pendingTime = 11;
    binding.pendingInterval = 2;
    binding.bindingMode = 1;
    binding.bindingStatus = 0;
    char outBinding[1024] = {0};
    EXPECT_TRUE(WriteAddressBinding(&binding, outBinding, sizeof(outBinding)) == RET_SUCCESS);
    std::string expectMsg = "00:01:02:03:04:05 0.0.0.0 60 10 11 2 1 0";
    EXPECT_TRUE(expectMsg == std::string(outBinding));
}

HWTEST(DhcpBindingTest, ParseAddressBindingTest, TestSize.Level1)
{
    std::string msg;
    AddressBinding binding;
    ASSERT_TRUE(memset_s(&binding, sizeof(binding), 0, sizeof(binding)) == EOK);
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == -1);
    msg = "00:01:02:03:04:05 127.0.0.1 60 10 11 2 1";
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == -1);
    msg = "00:01:02:03:04:05 127.0.0.1 60 10 11 2 1 0";
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == -1);
    msg = "00:01:02:03:04:05 127.0.0.1 60 10 5 2 1 0";
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == 0);
    msg = "00:01:02:03:04:05 127.0.0.1 60 10 5 2 1 2";
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == 0);
    uint64_t curr = Tmspsec();
    msg = "00:01:02:03:04:05 127.0.0.1 60 " + std::to_string(curr + 60) + " " + std::to_string(curr + 10) + " 5 2 1 2";
    EXPECT_TRUE(ParseAddressBinding(&binding, msg.c_str()) == 0);
}
