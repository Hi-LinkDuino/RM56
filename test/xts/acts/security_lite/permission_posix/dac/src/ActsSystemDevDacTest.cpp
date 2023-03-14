/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "ActsDacTest.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gtest/gtest.h"
#include "ActsCapabilityTest.h"
#include "DACFileSystemTest.h"

using namespace std;
using namespace testing::ext;

#if defined(LITE_FS_JFFS2)
static void CreateDevDir()
{
    int ret;
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR0' in the directory '/storage'
    ret = mkdir("/storage/" DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory '/storage/DACDIR0'";
    // Create a directory 'DACDIR0_DACDIR0' in the directory '/storage/DACDIR0'
    ret = mkdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory '/storage/DACDIR0/DACDIR0_DACDIR0'";
}

static void CreateDevTxt()
{
    int ret;
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR0' in the directory '/storage'
    ret = mkdir("/storage/" DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory '/storage/DACDIR0'";
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0290
 * @tc.name       : DAC control mechanism-File system storage-System call rmdir-UID0-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 333
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0290, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD333);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 333";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0300
 * @tc.name       : DAC control mechanism-File system storage-System call rmdir-UID1-GID0-Capability
                    CAPDACOVERRIDE-Permission 611
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0300, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD611);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 611";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0310
 * @tc.name       : DAC control mechanism-File system storage-System call rmdir-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 105
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0310, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD105);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 105";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0320
 * @tc.name       : DAC control mechanism-File system storage-System call rmdir-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 555
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0320, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD555);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 555";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0330
 * @tc.name       : DAC control mechanism-File system storage-System call rmdir-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH-Permission 666
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0330, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD666);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 666";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0340
 * @tc.name       : DAC control mechanism-File system storage-System call stat-UID0-GID1-Capability NULL-Permission 777
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0340, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD777);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 777";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0350
 * @tc.name       : DAC control mechanism-File system storage-System call stat-UID1-GID0-Capability
                    CAPDACOVERRIDE-Permission 166
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0350, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD166);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 166";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = stat("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, &buf);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0360
 * @tc.name       : DAC control mechanism-File system storage-System call stat-UID0-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 111
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0360, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD111);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 111";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = stat("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, &buf);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0370
 * @tc.name       : DAC control mechanism-File system storage-System call stat-UID0-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 210
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0370, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD210);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 210";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = stat("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, &buf);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0380
 * @tc.name       : DAC control mechanism-File system storage-System call stat-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 655
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0380, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD655);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 655";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = stat("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, &buf);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0390
 * @tc.name       : DAC control mechanism-File system storage-System call rename-UID1-GID0-Capability
                    CAPDACREADSEARCH-Permission 570
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0390, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD570);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 570";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rename("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, "/storage/" DACDIR0 "/" DACDIR0_DACDIR1);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0400
 * @tc.name       : DAC control mechanism-File system storage-System call rename-UID1-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 306
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0400, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD306);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 306";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rename("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, "/storage/" DACDIR0 "/" DACDIR0_DACDIR1);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0410
 * @tc.name       : DAC control mechanism-File system storage-System call rename-UID1-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 027
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0410, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD027);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 027";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = rename("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, "/storage/" DACDIR0 "/" DACDIR0_DACDIR1);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0420
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID0-GID1-Capability
                    CAPDACREADSEARCH-Permission 401
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0420, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD401);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 401";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0430
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID0-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 507
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0430, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD507);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 507";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0440
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID0-GID0-Capability
                    CAPDACOVERRIDE-Permission 347
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0440, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD347);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 347";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0450
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 063
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0450, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD063);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 063";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0460
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID1-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 230
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0460, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD230);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 230";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0470
 * @tc.name       : DAC control mechanism-File system storage-System call chdir-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 724
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0470, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD724);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 724";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0480
 * @tc.name       : DAC control mechanism-File system storage-System call execute-UID1-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 702
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0480, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD702);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = execve("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
        // the file is not elf , can not exec
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0490
 * @tc.name       : DAC control mechanism-File system storage-System call execute-UID0-GID0-Capability
                    CAPDACOVERRIDE-Permission 473
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0490, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD473);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = execve("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
        // the file is not elf , can not exec
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0500
 * @tc.name       : DAC control mechanism-File system storage-System call execute-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 261
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0500, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD261);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = execve("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
        // the file is not elf , can not exec
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0510
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID0-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 460
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0510, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD460);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0520
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID1-GID0-Capability
                    CAPDACOVERRIDE-Permission 562
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0520, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD562);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}


/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0530
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID0-GID0-Capability
                    CAPDACOVERRIDE-Permission 076
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0530, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD076);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0540
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID1-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 305
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0540, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD076);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0550
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 132
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0550, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD132);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0560
 * @tc.name       : DAC control mechanism-File system storage-System call access-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 241
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0560, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD241);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = access("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0570
 * @tc.name       : DAC control mechanism-File system storage-System call mkdir-UID0-GID1-Capability
                    CAPDACOVERRIDE-Permission 456
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0570, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD456);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 456";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0580
 * @tc.name       : DAC control mechanism-File system storage-System call mkdir-UID1-GID0-Capability
                    CAPDACOVERRIDE-Permission 167
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0580, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD167);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 167";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0590
 * @tc.name       : DAC control mechanism-File system storage-System call mkdir-UID1-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 511
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0590, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD511);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 511";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0600
 * @tc.name       : DAC control mechanism-File system storage-System call mkdir-UID1-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 640
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0600, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD640);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 640";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
        if (ret != 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0610
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID0-GID1-Capability
                    CAPDACOVERRIDE-Permission 362
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0610, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD362);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0620
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID0-GID1-Capability
                    CAPDACOVERRIDE-Permission 526
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0620, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD526);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0630
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID0-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 604
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0630, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD604);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0640
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID0-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 671
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0640, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD671);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0650
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID0-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 743
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0650, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD743);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0660
 * @tc.name       : DAC control mechanism-File system storage-System call unlink-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 235
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0660, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD235);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = unlink("/storage/" DACDIR0 "/" DACDIR0_DACFILE0);
        if (ret != FALSE) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0670
 * @tc.name       : DAC control mechanism-File system storage-System call open-UID1-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 371
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0670, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD371);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        fd = open("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
        if (fd >= 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            close(fd);
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0680
 * @tc.name       : DAC control mechanism-File system storage-System call open-UID1-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 702
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0680, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD702);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        fd = open("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
        if (fd >= 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            close(fd);
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0690
 * @tc.name       : DAC control mechanism-File system storage-System call open-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 406
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0690, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD406);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        fd = open("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
        if (fd >= 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            close(fd);
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0700
 * @tc.name       : DAC control mechanism-File system storage-System call open-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 257
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0700, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateDevTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0, CHMOD257);
    // storage/shm can not change chmod
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod ";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        fd = open("/storage/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
        if (fd >= 0) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            close(fd);
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0710
 * @tc.name       : DAC control mechanism-File system storage-System call opendir-UID0-GID1-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 750
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0710, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD750);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 750";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        CapInit();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID555);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (dir == nullptr) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        } else {
            closedir(dir);
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0720
 * @tc.name       : DAC control mechanism-File system storage-System call opendir-UID0-GID0-Capability
                    CAPDACOVERRIDE-Permission 143
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0720, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD143);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 143";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (dir == nullptr) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        } else {
            closedir(dir);
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0730
 * @tc.name       : DAC control mechanism-File system storage-System call opendir-UID1-GID1-Capability
                    CAPDACOVERRIDE-Permission 521
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0730, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD521);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 521";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (dir == nullptr) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        } else {
            closedir(dir);
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0740
 * @tc.name       : DAC control mechanism-File system storage-System call opendir-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 016
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0740, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD016);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 016";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (dir == nullptr) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        } else {
            closedir(dir);
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0750
 * @tc.name       : DAC control mechanism-File system storage-System call opendir-UID1-GID1-Groups contain-Capability
                    CAPDACOVERRIDE-Permission 407
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0750, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDevDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod("/storage/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD407);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 407";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir("/storage/" DACDIR0 "/" DACDIR0_DACDIR0);
        if (dir == nullptr) {
            LOG("ErrInfo: VFS error with DAC or Capability");
            exitCode = 1;
        } else {
            closedir(dir);
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        int status = 0;
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif
