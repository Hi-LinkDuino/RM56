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
#include <gtest/gtest.h>
#include "directory_ex.h"
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsDirectoryTest : public testing::Test {
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsDirectoryTest::SetUpTestCase(void)
{
}

void UtilsDirectoryTest::TearDownTestCase(void)
{
}

void UtilsDirectoryTest::SetUp(void)
{
}

void UtilsDirectoryTest::TearDown(void)
{
}

/*
 * @tc.name: testGetCurrentProcFullFileName001
 * @tc.desc: get the directory of directorytest
 */
HWTEST_F(UtilsDirectoryTest, testGetCurrentProcFullFileName001, TestSize.Level0)
{
    string strBaseName = "/data/test/UtilsDirectoryTest";
    string strFilename = GetCurrentProcFullFileName();
    EXPECT_EQ(strFilename, strBaseName);
}

/*
 * @tc.name: testGetCurrentProcPath001
 * @tc.desc: get the path of directorytest
 */
HWTEST_F(UtilsDirectoryTest, testGetCurrentProcPath001, TestSize.Level0)
{
    string strPathName = "/data/test/";
    string strCurPathName = GetCurrentProcPath();
    EXPECT_EQ(strCurPathName, strPathName);
}

/*
 * @tc.name: testExtractFilePath001
 * @tc.desc: get the filename of the path
 */
HWTEST_F(UtilsDirectoryTest, testExtractFilePath001, TestSize.Level0)
{
    string strFilePath = "/data/test/";
    string strPath = ExtractFilePath(GetCurrentProcFullFileName());
    EXPECT_EQ(strFilePath, strPath);
}

/*
 * @tc.name: testExtractFileName001
 * @tc.desc: get the filename of the path
 */
HWTEST_F(UtilsDirectoryTest, testExtractFileName001, TestSize.Level0)
{
    string strBaseName = "UtilsDirectoryTest";
    string strName = ExtractFileName(GetCurrentProcFullFileName());
    EXPECT_EQ(strBaseName, strName);
}

/*
 * @tc.name: testExtractFileExt001
 * @tc.desc: get the filename of the path
 */
HWTEST_F(UtilsDirectoryTest, testExtractFileExt001, TestSize.Level0)
{
    string strBaseName = "test/test.txt";
    string strTypeName = ExtractFileExt(strBaseName);
    EXPECT_EQ(strTypeName, "txt");
}

/*
 * @tc.name: testExcludeTrailingPathDelimiter001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testExcludeTrailingPathDelimiter001, TestSize.Level0)
{
    string strResult = "data/test/UtilsDirectoryTest";
    string strName = ExcludeTrailingPathDelimiter("data/test/UtilsDirectoryTest/");
    EXPECT_EQ(strResult, strName);
}

/*
 * @tc.name: testIncludeTrailingPathDelimiter001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testIncludeTrailingPathDelimiter001, TestSize.Level0)
{
    string strResult = "data/test/UtilsDirectoryTest/";
    string strName = IncludeTrailingPathDelimiter("data/test/UtilsDirectoryTest");
    EXPECT_EQ(strResult, strName);
}

/*
 * @tc.name: testGetDirFiles001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testGetDirFiles001, TestSize.Level0)
{
    string resultfile[2] = { "/data/test/TestFile.txt", "/data/test/UtilsDirectoryTest" };
    // prepare test data
    ofstream file(resultfile[0], fstream::out);

    string dirpath = "/data/";
    vector<string> filenames;
    GetDirFiles(dirpath, filenames);
    auto pos = find(filenames.begin(), filenames.end(), resultfile[0]);
    EXPECT_NE(pos, filenames.end());

    pos = find(filenames.begin(), filenames.end(), resultfile[1]);
    EXPECT_NE(pos, filenames.end());

    // delete test data
    RemoveFile(resultfile[0]);
}

/*
 * @tc.name: testForceCreateDirectory001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testForceCreateDirectory001, TestSize.Level0)
{
    string dirpath = "/data/test_dir/test2/test3";
    bool ret = ForceCreateDirectory(dirpath);
    EXPECT_EQ(ret, true);
    ret = IsEmptyFolder(dirpath);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: testForceRemoveDirectory001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testForceRemoveDirectory001, TestSize.Level0)
{
    string dirpath = "/data/test_dir";
    bool ret = ForceRemoveDirectory(dirpath);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: testRemoveFile001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testRemoveFile001, TestSize.Level0)
{
    string dirpath = "/data/test_dir";
    bool ret = ForceCreateDirectory(dirpath);
    EXPECT_EQ(ret, true);
    string filename = dirpath + "/test.txt";
    FILE *fp = fopen(filename.c_str(), "w");
    if (NULL != fp) {
        fclose(fp);
        ret = RemoveFile(filename);
        EXPECT_EQ(ret, true);
    }
    ret = ForceRemoveDirectory(dirpath);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: testGetFolderSize001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testGetFolderSize001, TestSize.Level0)
{
    string dirpath = "/data/test_folder/";
    bool ret = ForceCreateDirectory(dirpath);
    EXPECT_EQ(ret, true);
    ofstream out(dirpath + "test.txt");
    if (out.is_open()) {
        out << "This is a line.\n";
        out << "This is another line.\n";
        out.close();
    }
    uint64_t resultsize = GetFolderSize(dirpath);
    uint64_t resultcomp = 38;
    EXPECT_EQ(resultsize, resultcomp);

    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
    ret = ChangeModeFile(dirpath + "test.txt", mode);
    EXPECT_EQ(ret, true);

    mode = S_IRUSR  | S_IRGRP | S_IROTH;
    ret = ChangeModeDirectory(dirpath, mode);
    EXPECT_EQ(ret, true);

    ret = ForceRemoveDirectory(dirpath);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: testPathToRealPath001
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testPathToRealPath001, TestSize.Level0)
{
    string path = "/data/test";
    string realpath;
    bool ret = PathToRealPath(path, realpath);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(path, realpath);
}

/*
 * @tc.name: testPathToRealPath002
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testPathToRealPath002, TestSize.Level0)
{
    string path = "/data/../data/test";
    string realpath;
    bool ret = PathToRealPath(path, realpath);
    EXPECT_EQ(ret, true);
    EXPECT_EQ("/data/test", realpath);
}

/*
 * @tc.name: testPathToRealPath003
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testPathToRealPath003, TestSize.Level0)
{
    string path = "./";
    string realpath;
    bool ret = PathToRealPath(path, realpath);
    EXPECT_EQ(ret, true);
    EXPECT_EQ("/data/test", realpath);
}

/*
 * @tc.name: testPathToRealPath004
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testPathToRealPath004, TestSize.Level0)
{
    string path = "";
    string realpath;
    bool ret = PathToRealPath(path, realpath);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: testPathToRealPath005
 * @tc.desc: directory unit test
 */
HWTEST_F(UtilsDirectoryTest, testPathToRealPath005, TestSize.Level0)
{
    string path = "/data/test/data/test/data/test/data/test/data/test/data/ \
        test/data/test/data/test/data/test/data/test/data/test/data/test/data/ \
        test/data/test/data/test/data/test/data/test/data/test/data/test/data/ \
        test/data/test/data/test/data/test/data/test/data/test/data/test/data/ \
        test/data/test/data/test/data/test";
    string realpath;
    bool ret = PathToRealPath(path, realpath);
    EXPECT_EQ(ret, false);
}

