/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "unique_fd.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsUniqueFd : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    static char const* testfilename;
};

char const* UtilsUniqueFd::testfilename = "testfilename.test";

void UtilsUniqueFd::SetUpTestCase(void)
{

    ofstream outfile;
    outfile.open(testfilename, ios::out | ios::trunc);
    outfile << "testdata\n"
            << std::endl;
    outfile.close();
}

void UtilsUniqueFd::TearDownTestCase(void)
{

    ifstream inputfile;
    inputfile.open(testfilename, ios::in);
    std::string testStr;
    inputfile >> testStr;
    inputfile.close();
    if (remove(testfilename) == 0) {
        ;
    } else {
        FAIL();
    }
}

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFd, TestSize.Level0)
{
    int fd = open("NOTHISFILE", O_RDWR, 0666);

    UniqueFd ufd2(fd);
    if (ufd2 == -1) {
        SUCCEED();
    }
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueCtroFromInt, TestSize.Level0)
{

    UniqueFd ufd2(open(testfilename, O_RDWR, 0666));
    if (ufd2 == -1) {
        FAIL();
    }
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompare, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_EQ(fd, ufd2);
    ASSERT_EQ(ufd2, fd);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompareNl, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_TRUE(ufd2 >= 0);
    ASSERT_TRUE(0 <= ufd2);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompareBg, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_TRUE(ufd2 > 0);
    ASSERT_TRUE(0 < ufd2);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompareNb, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_TRUE(ufd2 <= 1000000);
    ASSERT_TRUE(1000000 >= ufd2);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompareLess, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_TRUE(ufd2 < 1000000);
    ASSERT_TRUE(1000000 > ufd2);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdeqcompareNeq, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR, 0666);
    UniqueFd ufd2(fd);
    ASSERT_TRUE(ufd2 != 1000000);
    ASSERT_TRUE(1000000 != ufd2);
};

class NewDeleter {
public:
    static int iflag;
    static void Close(int fd)
    {

        iflag = 10;
        close(fd);
    }
};

int NewDeleter::iflag = 0;

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdDefineDeletor, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR);

    {
        UniqueFdAddDeletor<NewDeleter> ufd2(fd);
        ASSERT_EQ(NewDeleter::iflag, 0);
        if (ufd2 == -1) {
            std::cout << "open test.h error" << std::endl;
        }
    }
    ASSERT_EQ(NewDeleter::iflag, 10);
};

HWTEST_F(UtilsUniqueFd, testUtilsUniqueFdDefineDeletorCloseStatus, TestSize.Level0)
{
    int fd = open(testfilename, O_RDWR);

    {
        UniqueFdAddDeletor<NewDeleter> ufd2(fd);
    }

    char buf[] = "test";
    int ret = write(fd, buf, sizeof(buf));
    ASSERT_EQ(ret, -1);
};
