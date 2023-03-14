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
#include "net_test.h"
#include "net.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(RpcNetTest, SetNonBlockTest, TestSize.Level1)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_TRUE(fd >= 0);
    EXPECT_TRUE(SetNonBlock(fd, 0) == 0);
    int flags = fcntl(fd, F_GETFL, 0);
    EXPECT_TRUE((flags & O_NONBLOCK) == 0);
    EXPECT_TRUE(SetNonBlock(fd, 1) == 0);
    flags = fcntl(fd, F_GETFL, 0);
    EXPECT_TRUE((flags & O_NONBLOCK) > 0);
    close(fd);
}

static constexpr int MAX_SIZE = 10240;
HWTEST_F(RpcNetTest, MyReadTest, TestSize.Level1)
{
    int fd = 0;
    char buff[MAX_SIZE] = {0};
    EXPECT_TRUE(MyRead(fd, buff, 0) == 0);
    EXPECT_TRUE(MyRead(fd, buff, MAX_SIZE) == 0);
    EXPECT_TRUE(MyRead(fd, buff, MAX_SIZE + 1) == SOCK_ERR);
    EXPECT_TRUE(MyRead(fd, buff, 10) == 10);
}

HWTEST_F(RpcNetTest, MyWriteTest, TestSize.Level1)
{
    int fd = 0;
    char buff[MAX_SIZE] = {0};
    EXPECT_TRUE(MyWrite(fd, buff, 0) == 0);
    EXPECT_TRUE(MyWrite(fd, buff, MAX_SIZE) == 0);
    EXPECT_TRUE(MyWrite(fd, buff, MAX_SIZE + 1) == SOCK_ERR);
    EXPECT_TRUE(MyWrite(fd, buff, 10) == 10);
}

HWTEST_F(RpcNetTest, CreateUnixServerTest, TestSize.Level1)
{
    int fd = CreateUnixServer("./rpc_test.sock", 1);
    EXPECT_TRUE(fd >= 0);
    close(fd);
    if (access("./rpc_test.sock", 0) == 0) {
        unlink("./rpc_test.sock");
    }
}

HWTEST_F(RpcNetTest, ConnectUnixServerTest, TestSize.Level1)
{
    EXPECT_TRUE(ConnectUnixServer("./rpc_test.sock") < 0);
    int servFd = CreateUnixServer("./rpc_test.sock", 1);
    EXPECT_TRUE(servFd >= 0);
    int cliFd = ConnectUnixServer("./rpc_test.sock");
    EXPECT_TRUE(cliFd >= 0);
    close(cliFd);
    close(servFd);
    if (access("./rpc_test.sock", 0) == 0) {
        unlink("./rpc_test.sock");
    }
}

HWTEST_F(RpcNetTest, WaitFdEventTest, TestSize.Level1)
{
    int servFd = CreateUnixServer("./rpc_test.sock", 1);
    EXPECT_TRUE(servFd >= 0);
    EXPECT_TRUE(WaitFdEvent(servFd, READ_EVENT, 100) == 0);
    int cliFd = ConnectUnixServer("./rpc_test.sock");
    EXPECT_TRUE(cliFd >= 0);
    EXPECT_TRUE(WaitFdEvent(servFd, READ_EVENT, 100) == 1);
    close(cliFd);
    close(servFd);
    if (access("./rpc_test.sock", 0) == 0) {
        unlink("./rpc_test.sock");
    }
}
}  // namespace Wifi
}  // namespace OHOS