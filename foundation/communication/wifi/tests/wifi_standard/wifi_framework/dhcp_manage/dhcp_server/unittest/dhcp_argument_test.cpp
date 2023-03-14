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
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "dhcp_argument.h"

using namespace testing::ext;


HWTEST(DhcpArgumentTest, InitArgumentsTest, TestSize.Level1)
{
    EXPECT_TRUE(InitArguments() == RET_SUCCESS);
}

HWTEST(DhcpArgumentTest, ParseArgumentsTest, TestSize.Level1)
{
    PrintRequiredArguments();
    ShowHelp(2);
    char *argv[ARGUMENT_VALUE_SIZE] = {
                                        const_cast<char *>(""), 
                                        const_cast<char *>("--dns=192.168.1.1,192.168.1.2"),
                                        const_cast<char *>("--ifname=eth0"),
                                        const_cast<char *>("--gateway=192.168.1.1"),
                                        const_cast<char *>("--pool=192.168.1.100,192.168.1.150"),
                                        const_cast<char *>("--version"),
                                        const_cast<char *>("--help"),
                                        const_cast<char *>("--unknown"),
                                        };

    EXPECT_TRUE(ParseArguments(8, argv) == RET_SUCCESS);
    ArgumentInfo *arg = GetArgument("dns");
    EXPECT_TRUE(arg);
    EXPECT_EQ(strncmp(arg->name, "dns", strlen("dns")), 0);
    EXPECT_EQ(strncmp(arg->value, "192.168.1.1,192.168.1.2", strlen("192.168.1.1,192.168.1.2")), 0);
    arg = GetArgument("ifname");
    EXPECT_TRUE(arg);
    EXPECT_EQ(strncmp(arg->name, "ifname", strlen("ifname")), 0);
    EXPECT_EQ(strncmp(arg->value, "eth0", strlen("eth0")), 0);
}

HWTEST(DhcpArgumentTest, PutArgumentTest, TestSize.Level1)
{
    EXPECT_TRUE(PutArgument(NULL, NULL) == RET_FAILED);
    const char *argu = "lease";
    const char *val = "4000";
    EXPECT_TRUE(PutArgument(NULL, val) == RET_FAILED);
    EXPECT_TRUE(PutArgument(argu, NULL) == RET_FAILED);
    EXPECT_TRUE(PutArgument(argu, val) == RET_SUCCESS);
    argu = "xxxx";
    val = "nothing";
    EXPECT_TRUE(PutArgument(argu, val) == RET_SUCCESS);
    EXPECT_TRUE(PutArgument(argu, val) == RET_FAILED);
    argu = "longlongvalue";
    val = "verylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvaluevery"
    "longvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvalueverylongvaluev"
    "erylongvalueverylongvalueverylongvalueverylongvalue";
    EXPECT_TRUE(PutArgument(argu, val) == RET_ERROR);
}

HWTEST(DhcpArgumentTest, GetArgumentTest, TestSize.Level1)
{
    ArgumentInfo *arg = GetArgument("lease");
    EXPECT_TRUE(arg);
    EXPECT_EQ(0, strncmp(arg->name, "lease", strlen("lease")));
    EXPECT_EQ(0, strncmp(arg->value, "4000", strlen("4000")));

    arg = GetArgument("xxxx");
    EXPECT_TRUE(arg);
    EXPECT_EQ(0, strncmp(arg->name, "xxxx", strlen("xxxx")));
    EXPECT_EQ(0, strncmp(arg->value, "nothing", strlen("nothing")));
}

HWTEST(DhcpArgumentTest, HasArgumentTest, TestSize.Level1)
{
    const char *name = "xxx";
    EXPECT_TRUE(HasArgument(name) == 0);
    name = "lease";
    EXPECT_TRUE(HasArgument(name) == 1);
    FreeArguments();
}
