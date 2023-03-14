/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "FileSystemTest.h"
#include <string.h>
#include <unistd.h>
#include "utils.h"

using namespace testing::ext;

// delete test file and dir
void DeleteTestFiles()
{
    if (access(TOP_DIR "/DIR_NEW", F_OK) == 0) {
        RemoveDir(TOP_DIR "/DIR_NEW");
    }
    if (access(TOP_DIR "/" DIR0, F_OK) == 0) {
        RemoveDir(TOP_DIR "/" DIR0);
    }
    if (access(TOP_DIR "/" FILE0, F_OK) == 0) {
        remove(TOP_DIR "/" FILE0);
    }
}

// before testCase
void FileSystemTest::SetUp()
{
    DeleteTestFiles();
    errno = 0;
    LOG("------- case start");
    mCurPath = GetCurrentPath();
    int rt = chdir(TOP_DIR);
    if (rt == -1) {
        LOG("== chdir to %s failed! rt = %d, errno = %d", TOP_DIR, rt, errno);
    } else {
        LOG("== chdir to %s OK!", TOP_DIR);
    }
}

// after testCase
void FileSystemTest::TearDown()
{
    DeleteTestFiles();
    int rt = chdir(mCurPath);
    if (rt == -1) {
        LOG("== chdir to %s failed! rt = %d, errno = %d", mCurPath, rt, errno);
    } else {
        LOG("== chdir to %s OK!", mCurPath);
    }
    LOG("------- case end\n");
}

// check TOP_DIR file system, 0 is exist, -1 is non-exist
int CheckFsMount()
{
    const int lenMax = 100;
    int len;
    char buf[lenMax];
    const char mountInfoFile[] = "/proc/mounts";

    // check TOP_DIR exist
    if (access(TOP_DIR, F_OK) != 0) {
        LOG("'%s' not accessible, Test Stop!", TOP_DIR);
        return -1;
    }

    FILE *fp = fopen(mountInfoFile, "r");
    if (fp != nullptr) {
        while (fgets(buf, lenMax, fp) != nullptr) {
            len = strlen(buf);
            if (strlen(buf) != 0) {
                buf[len - 1] = '\0';
            }

            if (strstr(buf, TOP_DIR_MOUNT_INFO) != nullptr) {
                fclose(fp);
                return 0;
            }
        }
        fclose(fp);
    }
    LOG("'%s' not mount properly, Test Stop!", TOP_DIR);
    return -1;
}

int main(int argc, char *argv[])
{
    testing::GTEST_FLAG(output) = "xml:";
    testing::InitGoogleTest(&argc, argv);
    if (CheckFsMount() != 0) {
        return 1;
    }
    DeleteTestFiles();
    return RUN_ALL_TESTS();
}