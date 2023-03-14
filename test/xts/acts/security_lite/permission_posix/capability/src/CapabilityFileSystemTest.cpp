/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "CapabilityFileSystemTest.h"
#include <unistd.h>
#include "ActsCapabilityTest.h"

using namespace testing::ext;

// Preset action of the test suite, which is executed before the first test case
void CapabilityTestSuite::SetUp()
{
    // Permission mask preset when creating a file
    umask(ZERO);
    // Init capabilities
    CapInit();
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Delete the 'TOP_DIR/CAPDIR0' if the directory exists
    RemoveDir(TOP_DIR "/" CAPDIR0);
    // Obtain the current working directory of the test code
    mCurPath = GetCurrentPath();
    // Modify the current working directory of the test code
    int ret = chdir(TOP_DIR);
    if (ret != 0) {
        LOG("ErrInfo: Failed to chdir to %s, ret=%d, errno=%d", TOP_DIR, ret, errno);
    }
}

// Test suite cleanup action, which is executed after the last test case
void CapabilityTestSuite::TearDown()
{
    // Delete the 'TOP_DIR/CAPDIR0' if the directory exists
    RemoveDir(TOP_DIR "/" CAPDIR0);
    // Restore the working directory of the test code
    int ret = chdir(mCurPath);
    if (ret != 0) {
        LOG("ErrInfo: Failed to chdir to %s, ret=%d, errno=%d", mCurPath, ret, errno);
    }
}

int main(int argc, char *argv[])
{
    testing::GTEST_FLAG(output) = "xml:";
    testing::InitGoogleTest(&argc, argv);
    if (CheckFsMount(TOP_DIR, TOP_DIR_MOUNT_INFO) != 0) {
        return 1;
    }
    return RUN_ALL_TESTS();
}