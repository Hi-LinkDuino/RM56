/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
static void CreateDir()
{
    int ret;
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR0' in the directory 'TOP_DIR'
    ret = mkdir(TOP_DIR "/" DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR0'";
    // Create a directory 'DACDIR0_DACDIR0' in the directory 'TOP_DIR/DACDIR0'
    ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR0/DACDIR0_DACDIR0'";
}

static void CreateTxt()
{
    int ret;
    int fd = 0;
    char dac[] = "DacTestSuite!\n";
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR0' in the directory 'TOP_DIR'
    ret = mkdir(TOP_DIR "/" DACDIR0, CHMOD777);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR0'";
    //  Create a file 'DACDIR0_DACFILE0' in the directory 'TOP_DIR/DACDIR0'
    fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY | O_CREAT | O_TRUNC, CHMOD777);
    if (fd >= 0) {
        // File created successfully
        write(fd, dac, sizeof(dac));
        close(fd);
    } else {
        // Failed to create the file
        ASSERT_GE(fd, 0) << "ErrInfo: Failed to create the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0'";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0760
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID0-GID1-Capability NULL-Permission 000
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0760, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD000);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 000";
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
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0770
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID0-GID1-Capability NULL-Permission 052
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0770, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD052);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 052";
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
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0780
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID0-GID1-Capability NULL-Permission 077
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0780, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD077);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 077";
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
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0790
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID1-GID0-Capability NULL-Permission 444
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0790, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD444);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 444";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0800
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID1-GID0-Capability
                    CAPDACREADSEARCH-Permission 716
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0800, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD716);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 716";
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
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0810
 * @tc.name       : DAC control mechanism-File system jffs2-System call rmdir-UID0-GID0-Capability
                    CAPDACREADSEARCH-Permission 222
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0810, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD222);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 222";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rmdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0820
 * @tc.name       : DAC control mechanism-File system jffs2-System call stat-UID1-GID0-Capability
                    CAPDACOVERRIDE-Permission 000
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0820, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD000);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 000";
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
        ret = stat(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, &buf);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0830
 * @tc.name       : DAC control mechanism-File system jffs2-System call stat-UID1-GID1-Capability NULL-Permission 422
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0830, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD422);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 422";
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
        // Step 3: Invoke the interface to operate the file system
        ret = stat(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, &buf);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0840
 * @tc.name       : DAC control mechanism-File system jffs2-System call stat-UID1-GID1-Capability
                    CAPDACREADSEARCH-Permission 334
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0840, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD334);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 334";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = stat(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, &buf);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0850
 * @tc.name       : DAC control mechanism-File system jffs2-System call stat-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 543
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0850, Function | MediumTest | Level2)
{
    int ret;
    struct stat buf = { 0 };
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD334);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 334";
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
        ret = stat(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, &buf);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0860
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID0-GID1-Capability
                    CAPDACOVERRIDE-Permission 614
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0860, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD614);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 614";
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
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0870
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID0-GID1-Capability
                    CAPDACREADSEARCH-Permission 242
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0870, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD242);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 242";
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
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0880
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID0-GID0-Capability NULL-Permission 430
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0880, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD430);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 430";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0890
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID0-GID0-Capability NULL-Permission 765
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0890, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD765);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 765";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0900
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID0-GID0-Capability
                    CAPDACREADSEARCH-Permission 123
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0900, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD123);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 123";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0910
 * @tc.name       : DAC control mechanism-File system jffs2-System call rename-UID1-GID1-Groups contain-Capability
                    NULL-Permission 151
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0910, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD151);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 151";
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
        ret = rename(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE1);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0920
 * @tc.name       : DAC control mechanism-File system jffs2-System call chdir-UID1-GID0-Capability
                    CAPDACREADSEARCH_CAPDACOVERRIDE-Permission 256
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0920, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD256);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 256";
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
        ret = chdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0930
 * @tc.name       : DAC control mechanism-File system jffs2-System call chdir-UID0-GID0-Capability
                    CAPDACREADSEARCH-Permission 235
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0930, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD235);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 235";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0940
 * @tc.name       : DAC control mechanism-File system jffs2-System call chdir-UID1-GID1-Capability NULL-Permission 670
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0940, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD670);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 670";
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
        // Step 3: Invoke the interface to operate the file system
        ret = chdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0950
 * @tc.name       : DAC control mechanism-File system jffs2-System call chdir-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH-Permission 116
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0950, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD116);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 116";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = chdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0960
 * @tc.name       : DAC control mechanism-File system jffs2-System call execute-UID0-GID1-Capability
                    CAPDACREADSEARCH-Permission 045
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0960, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD045);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 045";
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
        ret = execve(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0970
 * @tc.name       : DAC control mechanism-File system jffs2-System call execute-UID1-GID0-Capability NULL-Permission 124
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0970, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD124);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 124";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = execve(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0980
 * @tc.name       : DAC control mechanism-File system jffs2-System call execute-UID0-GID0-Capability NULL-Permission 536
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0980, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD536);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 536";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = execve(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0990
 * @tc.name       : DAC control mechanism-File system jffs2-System call execute-UID1-GID1-Capability
                    CAPDACREADSEARCH-Permission 657
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0990, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD657);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 657";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = execve(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1000
 * @tc.name       : DAC control mechanism-File system jffs2-System call execute-UID1-GID1-Groups contain-Capability
                    NULL-Permission 310
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1000, Function | MediumTest | Level2)
{
    int ret;
    char *argv[] = {nullptr, nullptr};
    char *envp[] = {nullptr};
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD310);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 310";
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
        ret = execve(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, argv, envp);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1010
 * @tc.name       : DAC control mechanism-File system jffs2-System call access-UID0-GID1-Capability NULL-Permission 354
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1010, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD354);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 354";
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
        ret = access(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1020
 * @tc.name       : DAC control mechanism-File system jffs2-System call access-UID1-GID0-Capability NULL-Permission 623
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1020, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD623);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 623";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = access(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1030
 * @tc.name       : DAC control mechanism-File system jffs2-System call access-UID1-GID1-Capability
                    CAPDACREADSEARCH-Permission 717
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1030, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD717);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 717";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        ret = access(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1040
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID0-GID1-Capability NULL-Permission 203
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1040, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD203);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 203";
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
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1050
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID0-GID0-Capability
                    CAPDACREADSEARCH-Permission 325
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1050, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD325);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 325";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1060
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID0-GID0-Capability
                    CAPDACREADSEARCH-Permission 453
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1060, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD453);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 453";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1070
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID1-GID1-Groups contain-Capability
                    NULL-Permission 342
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1070, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD342);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 342";
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
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1080
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID1-GID1-Groups contain-Capability
                    NULL-Permission 731
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1080, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD731);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 731";
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
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1090
 * @tc.name       : DAC control mechanism-File system jffs2-System call mkdir-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH-Permission 074
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1090, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, CHMOD074);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 074";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0 "/" DACDIR0_DACDIR0_DACDIR0, NORWX);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1100
 * @tc.name       : DAC control mechanism-File system jffs2-System call unlink-UID1-GID0-Capability
                    CAPDACREADSEARCH-Permission 175
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1100, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD175);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 175";
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
        ret = unlink(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1110
 * @tc.name       : DAC control mechanism-File system jffs2-System call unlink-UID1-GID0-Capability
                    CAPDACREADSEARCH-Permission 446
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1110, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD446);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 446";
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
        ret = unlink(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1120
 * @tc.name       : DAC control mechanism-File system jffs2-System call unlink-UID1-GID0-Capability
                    CAPDACREADSEARCH-Permission 560
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1120, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD560);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 560";
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
        ret = unlink(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1130
 * @tc.name       : DAC control mechanism-File system jffs2-System call unlink-UID1-GID1-Capability NULL-Permission 013
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1130, Function | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD013);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 013";
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
        // Step 3: Invoke the interface to operate the file system
        ret = unlink(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1140
 * @tc.name       : DAC control mechanism-File system jffs2-System call unlink-UID1-GID1-Groups contain-Capability
                    NULL-Permission 457
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1140, Function | MediumTest | Level2)
{
    int ret;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD457);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 457";
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
        ret = unlink(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1150
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID0-GID1-Capability
                    CAPDACREADSEARCH-Permission 120
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1150, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD120);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 120";
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
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (fd >= 0) {
            close(fd);
        } else {
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1160
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID0-GID1-Capability
                    CAPDACREADSEARCH-Permission 564
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1160, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD564);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 564";
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
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (fd >= 0) {
            close(fd);
        } else {
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1170
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID0-GID0-Capability NULL-Permission 415
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1170, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD415);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 415";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1180
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID1-GID1-Capability NULL-Permission 044
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1180, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD044);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 044";
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
        // Step 3: Invoke the interface to operate the file system
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1190
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID1-GID1-Capability NULL-Permission 703
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1190, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD703);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 703";
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
        // Step 3: Invoke the interface to operate the file system
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (fd >= 0) {
            close(fd);
        } else {
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1200
 * @tc.name       : DAC control mechanism-File system jffs2-System call open-UID1-GID1-Groups contain-Capability
                    NULL-Permission 637
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1200, Function | MediumTest | Level2)
{
    int ret;
    int fd = 0;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a file
    CreateTxt();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD637);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 637";
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
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, F_OK);
        if (fd >= 0) {
            close(fd);
        } else {
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1210
 * @tc.name       : DAC control mechanism-File system jffs2-System call opendir-UID1-GID0-Capability NULL-Permission 031
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1210, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD031);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 031";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID0);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1220
 * @tc.name       : DAC control mechanism-File system jffs2-System call opendir-UID0-GID0-Capability NULL-Permission 712
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1220, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD712);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 712";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID0, GID0);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1230
 * @tc.name       : DAC control mechanism-File system jffs2-System call opendir-UID1-GID1-Capability
                    CAPDACREADSEARCH-Permission 274
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1230, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD274);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 274";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_1240
 * @tc.name       : DAC control mechanism-File system jffs2-System call opendir-UID1-GID1-Groups contain-Capability
                    CAPDACREADSEARCH-Permission 665
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest1240, Function | MediumTest | Level2)
{
    int ret;
    DIR* dir = nullptr;
    gid_t list[SINGLESIZE] = {0};
    // Preset action: Create a directory
    CreateDir();
    // Preset action: Change the file permission according to the test procedure
    ret = chmod(TOP_DIR "/" DACDIR0, CHMOD665);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chmod 665";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the sub process capabilities according to the test procedure
        DropCAPDACOVERRIDE();
        // Step 2: Change the sub process uid, gid and groups according to the test procedure
        SetUidGid(UID555, GID555);
        setgroups(SINGLESIZE, list);
        // Step 3: Invoke the interface to operate the file system
        dir = opendir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0);
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