/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <memory>
#include <thread>

#include "zip.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
using namespace testing::ext;

namespace {
const std::string BASE_PATH = "/data/app/el2/100/base/";
const std::string APP_PATH = "com.example.zlib/com.example.zlib/com.example.zlib.MainAbility/files/";
}  // namespac
class ZipTest : public testing::Test {
public:
    ZipTest()
    {}
    ~ZipTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ZipTest::SetUpTestCase(void)
{}

void ZipTest::TearDownTestCase(void)
{}

void ZipTest::SetUp()
{}

void ZipTest::TearDown()
{}

void ZipCallBack(int result)
{
    printf("--Zip--callback--result=%d--\n", result);
}
void UnzipCallBack(int result)
{
    printf("--UnZip--callback--result=%d--\n", result);
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_zip_0100_8file
 * @tc.name: zip_0100_8file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_zip_0100_8file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test";
    std::string dest = BASE_PATH + APP_PATH + "result/8file.zip";

    OPTIONS options;
    Zip(FilePath(src), FilePath(dest), options, ZipCallBack, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_zip_0200_1file
 * @tc.name: zip_0200_1file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_zip_0200_1file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test/01";
    std::string dest = BASE_PATH + APP_PATH + "result/1file.zip";

    OPTIONS options;
    Zip(FilePath(src), FilePath(dest), options, ZipCallBack, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_zip_0100_zip1file
 * @tc.name: zip_0100_zip1file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_zip_0100_zip1file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test/01/zip1.txt";
    std::string dest = BASE_PATH + APP_PATH + "result/zip1file.zip";

    OPTIONS options;
    Zip(FilePath(src), FilePath(dest), options, ZipCallBack, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_unzip_0100_8file
 * @tc.name: unzip_0100_8file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_unzip_0100_8file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "result/8file.zip";
    std::string dest = BASE_PATH + APP_PATH + "unzip/01";

    OPTIONS options;
    Unzip(FilePath(src), FilePath(dest), options, UnzipCallBack);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
/**
 * @tc.number: APPEXECFWK_LIBZIP_unzip_single_0200_1file
 * @tc.name: unzip_single_0200_1file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_unzip_single_0200_1file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "result/1file.zip";
    std::string dest = BASE_PATH + APP_PATH + "unzip/02";

    OPTIONS options;
    Unzip(FilePath(src), FilePath(dest), options, UnzipCallBack);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
/**
 * @tc.number: APPEXECFWK_LIBZIP_zip_0100_zip1file
 * @tc.name: zip_0100_zip1file
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_unzip_0100_zip1file, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "result/zip1file.zip";
    std::string dest = BASE_PATH + APP_PATH + "unzip/zip1file";

    OPTIONS options;
    Unzip(FilePath(src), FilePath(dest), options, UnzipCallBack);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0100
 * @tc.name: Checkzip_0100
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0100, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test";
    std::string dest = BASE_PATH + APP_PATH + "check";
    FilePath srcFile(src);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile, destFile, options, ZipCallBack, false);
    destFile.CheckDestDirTail();
    FilePath newDestFile(destFile.CheckDestDirTail());
    std::cout << newDestFile.Value() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0200
 * @tc.name: Checkzip_0200
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0200, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test";
    std::string dest = BASE_PATH + APP_PATH + "error/check.zip";
    FilePath srcFile(src);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile, destFile, options, ZipCallBack, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0300
 * @tc.name: Checkzip_0300
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0300, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "error";
    std::string src1 = BASE_PATH + APP_PATH + "#%#@$%";
    std::string src2 = BASE_PATH + APP_PATH + "error/error1";
    std::string dest = BASE_PATH + APP_PATH + "/check.zip";
    FilePath srcFile(src);
    FilePath srcFile1(src1);
    FilePath srcFile2(src2);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile, destFile, options, ZipCallBack, false);
    Zip(srcFile1, destFile, options, ZipCallBack, false);
    Zip(srcFile2, destFile, options, ZipCallBack, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0400
 * @tc.name: Checkzip_0400
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0400, Function | MediumTest | Level1)
{
    std::string src1 = BASE_PATH + APP_PATH + "error.txt";
    std::string dest = BASE_PATH + APP_PATH + "check.zip";
    FilePath srcFile1(src1);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile1, destFile, options, ZipCallBack, false);
    std::cout << "srcFile1  DirName: " << srcFile1.DirName().Value() << std::endl;
    std::cout << "srcFile1  Value:   " << srcFile1.Value() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0500
 * @tc.name: Checkzip_0500
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0500, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "error";
    std::string dest = BASE_PATH + APP_PATH + "error1";
    FilePath srcFile(src);
    FilePath destFile(dest);
    
    OPTIONS options;
    Unzip(srcFile, destFile, options, UnzipCallBack);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0600
 * @tc.name: Checkzip_0600
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0600, Function | MediumTest | Level1)
{
    std::string src = BASE_PATH + APP_PATH + "test";
    std::string dest = "";
    FilePath srcFile(src);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile, destFile, options, ZipCallBack, false);
    std::cout << destFile.Value() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0700
 * @tc.name: Checkzip_0700
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0700, Function | MediumTest | Level1)
{
    std::string src = "";
    std::string dest = BASE_PATH + APP_PATH;
    FilePath srcFile(src);
    FilePath destFile(dest);

    OPTIONS options;
    Zip(srcFile, destFile, options, ZipCallBack, false);
    destFile.CheckDestDirTail();
    FilePath newDestFile(destFile.CheckDestDirTail());
    std::cout << newDestFile.Value() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

/**
 * @tc.number: APPEXECFWK_LIBZIP_Checkzip_0800
 * @tc.name: Checkzip_0800
 * @tc.desc:
 */
HWTEST_F(ZipTest, APPEXECFWK_LIBZIP_Checkzip_0800, Function | MediumTest | Level1)
{
    std::string src = "";
    std::string dest = BASE_PATH + APP_PATH;
    FilePath srcFile(src);
    FilePath destFile(dest);
    
    OPTIONS options;
    Unzip(srcFile, destFile, options, UnzipCallBack);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS