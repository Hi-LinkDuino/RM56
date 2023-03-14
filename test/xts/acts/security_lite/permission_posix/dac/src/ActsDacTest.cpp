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
static void CreateTxt()
{
    int ret;
    int fd = 0;
    char dac[] = "DacTestSuite!\n";
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR0' in the directory 'TOP_DIR'
    ret = mkdir(TOP_DIR "/" DACDIR0, CHMOD700);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR0'";
    // Create a directory 'DACDIR0_DACDIR0' in the directory 'TOP_DIR/DACDIR0'
    ret = mkdir(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACDIR0, RWX);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR0/DACDIR0_DACDIR0'";
    //  Create a file 'DACDIR0_DACFILE0' in the directory 'DacTest'
    fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY | O_CREAT | O_TRUNC, CHMOD700);
    if (fd >= 0) {
        // File created successfully
        write(fd, dac, sizeof(dac));
        close(fd);
    } else {
        // Failed to create the file
        ASSERT_GE(fd, 0) << "ErrInfo: Failed to create the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0'";
    }
}

static void CreateTxt1()
{
    int ret;
    int fd = 0;
    char dac[] = "DacTestSuite!\n";
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'DACDIR1' in the directory 'TOP_DIR'
    ret = mkdir(TOP_DIR "/" DACDIR1, CHMOD700);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR1'";
    // Create a directory 'DACDIR1_DACDIR0' in the directory 'TOP_DIR/DACDIR1'
    ret = mkdir(TOP_DIR "/" DACDIR1 "/" DACDIR1_DACDIR0, RWX);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/DACDIR1/DACDIR1_DACDIR0'";
    //  Create a file 'DACDIR1_DACFILE0' in the directory 'DACDIR1'
    fd = open(TOP_DIR "/" DACDIR1 "/" DACDIR1_DACFILE0, O_WRONLY | O_CREAT | O_TRUNC, CHMOD700);
    if (fd >= 0) {
        // File created successfully
        write(fd, dac, sizeof(dac));
        close(fd);
    } else {
        // Failed to create the file
        ASSERT_GE(fd, 0) << "ErrInfo: Failed to create the file 'TOP_DIR/DACDIR1/DACDIR1_DACFILE0'";
    }
    // Change the file 'DACDIR1_DACFILE0' owner with interface 'chown'
    ret = chown(TOP_DIR "/" DACDIR1 "/" DACDIR1_DACFILE0, UID1, GID1);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the file 'DACDIR1_DACFILE0' owner with interface 'chown'";
    // Change the directory 'TOP_DIR/DACDIR1' owner with interface 'chown'
    ret = chown(TOP_DIR "/" DACDIR1, UID1, GID1);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the directory 'TOP_DIR/DACDIR1' owner with interface 'chown'";
}

static int ThreeProcessReadOneTxt()
{
    int fd = 0;
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork three sub processes
    pid_t pid;
    for (int num = 0; num < NUM3; num++) {
        pid = fork();
        if (pid < 0) {
            LOG("======== Fork Error! =========");
            return -1;
        }
        usleep(SLEEP_NUM);
        if (pid == 0) {
            break;
        }
    }
    // get one parent & three children
    if (pid == 0) {
        int exitCode = 0;
        // Initialize the process and set the uid and gid of the process to zero
        SetUidGid(UID0, GID0);
        // Drop the capabilities of CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH
        DropCAPDACOVERRIDEAndREADSEARCH();
        // Three sub processes read a file at the same time
        for (int number = 0; number < NUM1000; number++) {
            fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
            if (fd >= 0) {
                close(fd);
            } else {
                LOG("ErrInfo: failed to open the file during the %d time", number);
                exitCode = 1;
                break;
            }
        }
        // Three sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // The parent process wait for three sub processes to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM3; num2++) {
            wait(&status);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: Pid = "<< pid
            << ", its exitCode is wrong and test case failed, please query logs";
        }
        // Delete the directory 'TOP_DIR/DACDIR0'
        RemoveDir(TOP_DIR "/" DACDIR0);
    }
    return 0;
}

static int TwoProcessReadTwoTxt()
{
    int status = 0;
    // Preset action: Create a file whose owner is uid0, gid0
    CreateTxt();
    // Preset action: Create a file whose owner is uid1, gid1
    CreateTxt1();
    // Preset action: Fork two sub processes
    pid_t pid[NUM2];
    for (int num = 0; num < NUM2; num++) {
        pid[num] = fork();
        EXPECT_TRUE(pid[num] >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid[num] == 0) {
            // Set UID and GID of process pid[0] to 0 and set UID and GID of process pid[1] to 1
            SetUidGid(num, num);
            // Drop both process capabilities of CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH
            DropCAPDACOVERRIDEAndREADSEARCH();
            break;
        }
    }
    // get one parent & two children
    if (pid[0] == 0 || pid[1] == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM1000; number++) {
            // Two processes with different UIDs and GIDs read two files with different owners at the same time
            if (pid[0] == 0) {
                // The process pid[0] can open the file Dac.txt whose owners are uid0 and gid0
                int fd0dac = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
                if (fd0dac >= 0) {
                    close(fd0dac);
                } else {
                    LOG("ErrInfo: PID[0] failed to open the file during the %d time", number);
                    exitCode = 1;
                    break;
                }
                // The process pid[0] can not open the file Dac.txt whose owners are uid1 and gid1
                int fd0dac1 = open(TOP_DIR "/" DACDIR1 "/" DACDIR1_DACFILE0, O_WRONLY);
                if (fd0dac1 >= 0) {
                    LOG("ErrInfo: PID[0] open the file with wrong uid&gid during the %d time", number);
                    close(fd0dac1);
                    exitCode = 1;
                    break;
                }
            } else {
                // The process pid[1] can not open the file Dac.txt whose owners are uid0 and gid0
                int fd1dac = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
                if (fd1dac >= 0) {
                    LOG("ErrInfo: PID[1] open the file with wrong uid&gid during the %d time", number);
                    close(fd1dac);
                    exitCode = 1;
                    break;
                }
                // The process pid[1] can open the file Dac.txt whose owners are uid1 and gid1
                int fd1dac1 = open(TOP_DIR "/" DACDIR1 "/" DACDIR1_DACFILE0, O_WRONLY);
                if (fd1dac1 >= 0) {
                    close(fd1dac1);
                } else {
                    LOG("ErrInfo: PID[1] failed to open the file during the %d time", number);
                    exitCode = 1;
                    break;
                }
            }
        }
        // Two sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // The parent process wait for the sub process pid[0] to exit and obtain the exitCode
        waitpid(pid[0], &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid[0];
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = "
        << pid[0];
        // The parent process wait for the sub process pid[1] to exit and obtain the exitCode
        waitpid(pid[1], &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid[1];
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = "
        << pid[1];
    }
    return 0;
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0140
 * @tc.name       : Invoke the chmod interface to set the file permission
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0140, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Preset action: Drop the capabilities of CAP_FOWNER
        DropCAPFOWNER();
        // Step 1: Set the uid and gid of the process to 0
        SetUidGid(UID0, GID0);
        // Step 2.1: Change the file permission 700 with interface 'chmod'
        ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD700);
        if (ret != 0) {
            LOG("ErrInfo: Failed in chmod 700");
            exitCode = 1;
        }
        // Step 2.2: Change the file permission 111 with interface 'chmod'
        ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD111);
        if (ret != 0) {
            LOG("ErrInfo: Failed in chmod 111");
            exitCode = 1;
        }
        // Step 2.3: Change the file permission -1 with interface 'chmod'
        ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, FALSE);
        if (ret != 0) {
            LOG("ErrInfo: Failed in chmod FALSE");
            exitCode = 1;
        }
        // Step 2.4: Change the file permission -777 with interface 'chmod'
        ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, -777);
        if (ret != 0) {
            LOG("ErrInfo: Failed in chmod -777");
            exitCode = 1;
        }
        // Step 3: Set the uid and gid of the process to 10000
        SetUidGid(UID10000, GID10000);
        // Step 4: Failed to change the file permission for the process that is not the file owner
        ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD777);
        EXPECT_EQ(ret, FALSE) << "ErrInfo: Chmod 777 with wrong uid";
        // Step 5: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0150
 * @tc.name       : Invoke chown interface to set the file owner
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0150, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Set the uid and gid of the process to 0
        SetUidGid(UID0, GID0);
        // Step 2.1: Set the directory 'TOP_DIR/DACDIR0' owner UID10000 and GID10000
        ret = chown(TOP_DIR "/" DACDIR0, UID10000, GID10000);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the directory 'TOP_DIR/DACDIR0' owner UID10000 and GID10000");
            exitCode = 1;
        }
        // Step 2.2: Set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner UID10000 and GID10000
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID10000, GID10000);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner UID10000 and GID10000");
            exitCode = 1;
        }
        // Step 2.3: Set the directory 'TOP_DIR/DACDIR0' owner UID2147483647 and GID2147483647
        ret = chown(TOP_DIR "/" DACDIR0, MAX_INT, MAX_INT);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the directory 'TOP_DIR/DACDIR0' owner UID2147483647 and GID2147483647");
            exitCode = 1;
        }
        // Step 2.4: Set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner UID2147483647 and GID2147483647
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, MAX_INT, MAX_INT);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the file owner UID2147483647 and GID2147483647");
            exitCode = 1;
        }
        // Step 2.5: Set the directory 'TOP_DIR/DACDIR0' owner UID0 and GID0
        ret = chown(TOP_DIR "/" DACDIR0, UID0, GID0);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the directory 'TOP_DIR/DACDIR0' owner UID0 and GID0");
            exitCode = 1;
        }
        // Step 2.4: Set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner UID0 and GID0
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID0, GID0);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner UID0 and GID0");
            exitCode = 1;
        }
        // Step 3: Drop the capabilities of CAP_CHOWN
        DropCAPCHOWN();
        // Step 4.1: Failed to set the directory 'TOP_DIR/DACDIR0' owner without CAP_CHOWN");
        ret = chown(TOP_DIR "/" DACDIR0, UID10000, GID10000);
        if (ret != FALSE) {
            LOG("ErrInfo: Set the directory 'TOP_DIR/DACDIR0' owner without CAP_CHOWN");
            exitCode = 1;
        }
        // Step 4.2: Failed to set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner without CAP_CHOWN");
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID10000, GID10000);
        if (ret != FALSE) {
            LOG("ErrInfo: Set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner without CAP_CHOWN");
            exitCode = 1;
        }
        // Step 4.3: Set the uid and gid of the process to 555
        SetUidGid(UID555, GID555);
        // Step 4.4: Failed to set the directory 'TOP_DIR/DACDIR0' owner without CAP_CHOWN");
        ret = chown(TOP_DIR "/" DACDIR0, UID10000, GID10000);
        if (ret != FALSE) {
            LOG("ErrInfo: Set the directory 'TOP_DIR/DACDIR0' owner without CAP_CHOWN");
            exitCode = 1;
        }
        // Step 4.5: Failed to set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner without CAP_CHOWN");
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID10000, GID10000);
        if (ret != FALSE) {
            LOG("ErrInfo: Set the file 'TOP_DIR/DACDIR0/DACDIR0_DACFILE0' owner without CAP_CHOWN");
            exitCode = 1;
        }
        // Step 5: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 6: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0160
 * @tc.name       : Concurrent file reading by multiple processes
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0160, Function | MediumTest | Level2)
{
    int ret;
    // Step 1: Multiple processes read the same file
    ret = ThreeProcessReadOneTxt();
    EXPECT_EQ(ret, 0) << "ErrInfo: ThreeProcessReadOneTxt() exit error";
    // Step 2: Multiple processes read the different files with different owners
    ret = TwoProcessReadTwoTxt();
    EXPECT_EQ(ret, 0) << "ErrInfo: TwoProcessReadTwoTxt() exit error";
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0170
 * @tc.name       : The owner or permission of a file fail to be modified when the file is operated by another process
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0170, Function | MediumTest | Level2)
{
    int fd = 0;
    int status = 0;
    int exitCode0 = 0;
    int exitCode1 = 0;
    // Preset action: Create a file whose owner is uid0, gid0
    CreateTxt();
    // Preset action: Fork two sub processes
    pid_t pid[NUM2];
    for (int num = 0; num < NUM2; num++) {
        pid[num] = fork();
        ASSERT_TRUE(pid[num] >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid[num] == 0) {
            // Drop both process capabilities of CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH
            DropCAPDACOVERRIDEAndREADSEARCH();
            break;
        }
    }
    // Step 1: Open a file by Process pid[0]
    if (pid[0] == 0) {
        fd = open(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, O_WRONLY);
    }
    if (pid[1] == 0) {
        // Step 2: Change the file owner with interface 'chown' by Process pid[1]
        int retchown = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID10000, GID10000);
        if (retchown != 0) {
            LOG("ErrInfo: Failed to change the file owner when the file is operated by another process");
            exitCode1 = 1;
        }
        // Step 3: Change the file permission 000 with interface 'chmod' by Process pid[1]
        int retchmod = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD000);
        if (retchmod != 0) {
            LOG("ErrInfo: Failed to change the file permission when the file is operated by another process");
            exitCode1 = 1;
        }
        // Step 4: The sub process pid[1] exit with the exitCode1
        exit(exitCode1);
    }
    // Step 5: Close the file by Process pid[0]
    if (pid[0] == 0) {
        if (fd >= 0) {
            close(fd);
        } else {
            LOG("ErrInfo: PID[0] failed to open the file");
            exitCode0 = 1;
        }
        // Step 6: The sub process pid[0] exit with the exitCode0
        exit(exitCode0);
    }
    if ((pid[0] != 0) && (pid[1] != 0)) {
        // Step 7: The parent process wait for the sub process pid[0] and pid[1] to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM2; num2++) {
            waitpid(pid[num2], &status, 0);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid[num2];
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = "
            << pid[num2];
        }
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0180
 * @tc.name       : Change the file permission for 10000 times
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0180, Reliability | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a txt
    CreateTxt();
    // Step 1: Change the file permission for 10000 times
    for (int number = 0; number < NUM10000; number++) {
        // Step 1: Change the file permission
        if (number & 1) {
            ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD700);
            if (ret != 0) {
                EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the file permission during the " << number << " time";
                break;
            }
        } else {
            ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD777);
            if (ret != 0) {
                EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the file permission during the " << number << " time";
                break;
            }
        }
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0190
 * @tc.name       : Change the file owner for 10000 times
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0190, Reliability | MediumTest | Level2)
{
    int ret;
    // Preset action: Create a txt
    CreateTxt();
    // Step 1: Change the file owner for 10000 times
    for (int number = 0; number < NUM10000; number++) {
        ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, number, number);
        if (ret != 0) {
            EXPECT_EQ(ret, 0) << "ErrInfo: Failed to Change the file owner during the " << number << " time";
            break;
        }
    }
    // Cleanup action: Restore the initial status of the file
    ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID0, GID0);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to Change the file owner";
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0200
 * @tc.name       : Five processes concurrently invoke chmod and chown interface for 5000 times
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0200, Reliability | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork five sub processes
    pid_t pid;
    for (int num = 0; num < NUM5; num++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid == 0) {
            break;
        }
    }
    // get one parent & five children
    if (pid == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM5000; number++) {
            // Step 1: Change the file owner for 5000 times
            ret = chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, number, number);
            if (ret != 0) {
                LOG("ErrInfo: Failed to Change the file owner during the %d time", number);
                break;
            }
            // Step 2: Change the file permission for 5000 times
            if (number & 1) {
                ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD700);
                if (ret != 0) {
                    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the file permission 700 during the " << number
                    << " time";
                    break;
                }
            } else {
                ret = chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD777);
                if (ret != 0) {
                    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to change the file permission 777 during the " << number
                    << " time";
                    break;
                }
            }
        }
        // Step 3: Two sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // Step 4: The parent process wait for two sub processes to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM5; num2++) {
            wait(&status);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: Pid = "<< pid
            << ", its exitCode is wrong and test case failed, please query logs";
        }
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0210
 * @tc.name       : Performance test of the chomd and chown interface
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0210, Performance | MediumTest | Level2)
{
    struct timespec tp = { 0 };
    struct timespec starttime = { 0 };
    struct timespec endtime = { 0 };
    tp.tv_sec = 0;
    tp.tv_nsec = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Obtains the system time -> starttime
    clock_gettime(CLOCK_REALTIME, &starttime);
    for (int number = 0; number < NUM5000; number++) {
        // Step 1: Change the file owner for 5000 times
        chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, number, number);
        // Step 2: Change the file permission for 5000 times
        if (number & 1) {
            chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD700);
        } else {
            chmod(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, CHMOD777);
        }
    }
    // Step 3: Obtains the system time again -> endtime
    clock_gettime(CLOCK_REALTIME, &endtime);
    // Step 4: Compare the starttime and the endtime -> tp
    tp = CompareTime(starttime, endtime);
    EXPECT_LE(tp.tv_sec, NUM20) << "ErrInfo: Chown for 10000 times used " << tp.tv_sec << "." << tp.tv_nsec << "s";
    // Cleanup action: Restore the initial status of the file
    chown(TOP_DIR "/" DACDIR0 "/" DACDIR0_DACFILE0, UID0, GID0);
}
#endif