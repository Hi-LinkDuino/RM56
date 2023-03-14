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
#include "ActsCapabilityTest.h"
#include <dirent.h>
#include <fcntl.h>
#include <securec.h>
#include <sys/capability.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gtest/gtest.h"
#include "CapabilityFileSystemTest.h"

using namespace std;
using namespace testing::ext;

#if defined(LITE_FS_JFFS2)
static int TestDacOverrideSuccess()
{
    int fd = 0;
    int ret = 0;
    char cap[] = "CapabilityTestSuite!\n";
    // Create a directory 'mkdir' in the directory 'TOP_DIR/CAPDIR0'
    ret = mkdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1, NORWX);
    if (ret != 0) {
        LOG("ErrInfo: Failed to mkdir 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Change the current working directory to 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1'
    ret = chdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1);
    if (ret != 0) {
        LOG("ErrInfo: Failed to chdir 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    chdir(TOP_DIR "/" CAPDIR0);
    // Delete the directory 'mkdir' in the directory 'TOP_DIR/CAPDIR0'
    ret = rmdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1);
    if (ret != 0) {
        LOG("ErrInfo: Failed to rmdir 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Rename the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' to 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1'
    ret = rename(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1);
    if (ret != 0) {
        LOG("ErrInfo: Failed to rename 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Check whether the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' exists
    ret = access(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1, F_OK);
    if (ret != 0) {
        LOG("ErrInfo: Failed to access 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Delete the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1'
    ret = unlink(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1);
    if (ret != 0) {
        LOG("ErrInfo: Failed to unlink 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' with CAP_DAC_OVERRIDE");
        return FALSE;
    }
    //  Create a file 'CAPDIR0_CAPFILE0' in the directory 'CAPDIR0'
    fd = open(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, O_WRONLY | O_CREAT | O_TRUNC, RWX);
    if (fd >= 0) {
        // File created successfully
        write(fd, cap, sizeof(cap));
        close(fd);
    } else {
        // Failed to create the file
        LOG("ErrInfo: Failed to create 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0'");
        return FALSE;
    }
    return 0;
}

static int TestDacOverrideFail()
{
    int ret = 0;
    // Failed to create a directory 'mkdir' in the directory 'TOP_DIR/CAPDIR0'
    ret = mkdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1, NORWX);
    if (ret != FALSE) {
        LOG("ErrInfo: mkdir 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1' without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Failed to change the current working directory to 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1'
    ret = chdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1);
    if (ret != FALSE) {
        LOG("ErrInfo: Change the current working directory without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Failed to delete the directory 'mkdir' in the directory 'TOP_DIR/CAPDIR0'
    ret = rmdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR1);
    if (ret != FALSE) {
        LOG("ErrInfo: Delete 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR1' without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Failed to rename the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' to 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1'
    ret = rename(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1);
    if (ret != FALSE) {
        LOG("ErrInfo: Rename 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Failed to check whether the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' exists
    ret = access(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1, F_OK);
    if (ret != FALSE) {
        LOG("ErrInfo: Check whether the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' exists without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    // Failed to delete the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1'
    ret = unlink(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE1);
    if (ret != FALSE) {
        LOG("ErrInfo: Delete 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE1' without CAP_DAC_OVERRIDE");
        return FALSE;
    }
    return 0;
}

static int TestDacReadSearchSuccess(int num)
{
    int fd = 0;
    int ret = 0;
    DIR *dir = nullptr;
    struct stat buf = { 0 };
    // Open a file 'CAPDIR0_CAPFILE0' in the directory 'TOP_DIR/CAPDIR0'
    fd = open(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, O_WRONLY);
    if (fd >= 0) {
        close(fd);
    } else {
        LOG("ErrInfo: Failed to open file with CAP_DAC_READ_SEARCH || CAP_DAC_OVERRIDE during the %d time", num);
        return FALSE;
    }
    // Open a directory 'CAPDIR0_CAPDIR0' in the directory 'TOP_DIR/CAPDIR0'
    dir = opendir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR0);
    if (dir == nullptr) {
        LOG("ErrInfo: Failed to open dir with CAP_DAC_READ_SEARCH || CAP_DAC_OVERRIDE during the %d time", num);
        return FALSE;
    }
    closedir(dir);
    // Obtain the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' status
    ret = stat(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, &buf);
    if (ret != 0) {
        LOG("ErrInfo: Failed to stat with CAP_DAC_READ_SEARCH || CAP_DAC_OVERRIDE during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int TestDacReadSearchFail()
{
    int fd = 0;
    int ret = 0;
    DIR *dir = nullptr;
    struct stat buf = { 0 };
    // Failed to open a file 'CAPDIR0_CAPFILE0' in the directory 'TOP_DIR/CAPDIR0'
    fd = open(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, O_WRONLY);
    if (fd >= 0) {
        LOG("ErrInfo: Open file without CAP_DAC_READ_SEARCH && CAP_DAC_OVERRIDE");
        close(fd);
        return FALSE;
    }
    // Failed to directory a file 'CAPDIR0_CAPDIR0' in the directory 'TOP_DIR/CAPDIR0'
    dir = opendir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR0);
    if (dir != nullptr) {
        LOG("ErrInfo: Open dir without CAP_DAC_READ_SEARCH && CAP_DAC_OVERRIDE");
        closedir(dir);
        return FALSE;
    }
    // Failed to obtain the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0' status
    ret = stat(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, &buf);
    if (ret != FALSE) {
        LOG("ErrInfo: Stat file without CAP_DAC_READ_SEARCH && CAP_DAC_OVERRIDE");
        return FALSE;
    }
    return 0;
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
static void CreateTxt()
{
    int ret;
    int fd = 0;
    char cap[] = "CapabilityTestSuite!\n";
    // Initialize the process and set the uid and gid of the process to zero
    SetUidGid(UID0, GID0);
    // Create a directory 'CAPDIR0' in the directory 'TOP_DIR'
    ret = mkdir(TOP_DIR "/" CAPDIR0, NORWX);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/CAPDIR0'";
    // Create a directory 'CAPDIR0_CAPDIR0' in the directory 'TOP_DIR/CAPDIR0'
    ret = mkdir(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPDIR0, RWX);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to create the directory 'TOP_DIR/CAPDIR0/CAPDIR0_CAPDIR0'";
    //  Create a file 'CAPDIR0_CAPFILE0' in the directory 'CAPDIR0'
    fd = open(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, O_WRONLY | O_CREAT | O_TRUNC, RWX);
    if (fd >= 0) {
        // File created successfully
        write(fd, cap, sizeof(cap));
        close(fd);
    } else {
        // Failed to create the file
        ASSERT_GE(fd, 0) << "ErrInfo: Failed to create the file 'TOP_DIR/CAPDIR0/CAPDIR0_CAPFILE0'";
    }
}

static int CapsetOnlySETPCAP(int num)
{
    struct __user_cap_header_struct capheader;
    (void)memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM];
    (void)memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].permitted |= CAP_TO_MASK(CAP_SETPCAP);
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].effective |= CAP_TO_MASK(CAP_SETPCAP);
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].inheritable |= CAP_TO_MASK(CAP_SETPCAP);
    // Set capabilities
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        LOG("ErrInfo: Failed to drop all caps except CAP_SETPCAP during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int AddCapUnauthorized(int num)
{
    struct __user_cap_header_struct capheader;
    (void)memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM];
    (void)memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    capdata[0].permitted = LINUX_FULL_CAP;
    capdata[0].effective = LINUX_FULL_CAP;
    capdata[0].inheritable = LINUX_FULL_CAP;
    // Set capabilities
    int ret = capset(&capheader, &capdata[0]);
    if (ret != FALSE) {
        LOG("ErrInfo: Add unauthorized capability during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int CapgetWithAllCap(int num)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    int ret = capget(&capheader, &capdataget[0]);
    if (ret != 0) {
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to get CAPs";
        LOG("ErrInfo: Failed to get CAPs during the %d time", num);
        return FALSE;
    }
    // The process has all capabilities
    if (capdataget[0].effective != OHOS_FULL_CAP) {
        EXPECT_EQ(capdataget[0].effective, OHOS_FULL_CAP) << "ErrInfo: Get wrong capabilities";
        LOG("ErrInfo: Get wrong capabilities during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int CapgetWithNoCap(int num)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    int ret = capget(&capheader, &capdataget[0]);
    if (ret != 0) {
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to get CAPs";
        LOG("ErrInfo: Failed to get CAPs during the %d time", num);
        return FALSE;
    }
    // The process does not have any capabilities
    if (capdataget[0].effective != NO_CAP) {
        EXPECT_EQ(capdataget[0].effective, NO_CAP) << "ErrInfo: Get wrong capabilities";
        LOG("ErrInfo: Get wrong capabilities during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int CapgetOnlySETPCAP(int num)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    int ret = capget(&capheader, &capdataget[0]);
    if (ret != 0) {
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to get CAPs";
        LOG("ErrInfo: Failed to get CAPs during the %d time", num);
        return FALSE;
    }
    // The process only has CAP_SETPCAP
    if (capdataget[0].effective != ONLY_SETPCAP_CAP) {
        EXPECT_EQ(capdataget[0].effective, ONLY_SETPCAP_CAP) << "ErrInfo: Get wrong capabilities";
        LOG("ErrInfo: Get wrong capabilities during the %d time", num);
        return FALSE;
    }
    return 0;
}

static int CapsetWithoutSETPCAP()
{
    // Drop the capabilities of CAP_SETPCAP 8
    int retsetpcap = DropCAPSETPCAP();
    EXPECT_EQ(retsetpcap, 0) << "ErrInfo: Failed to drop CAP_SETPCAP";
    // Drop the capabilities of CAP_CHOWN 0
    int retchown = DropCAPCHOWN();
    EXPECT_EQ(retchown, FALSE) << "ErrInfo: Drop CAP_CHOWN without CAP_SETPCAP";
    // Drop the capabilities of CAP_DAC_OVERRIDE 1
    int retdacoverride = DropCAPDACOVERRIDE();
    EXPECT_EQ(retdacoverride, FALSE) << "ErrInfo: Drop CAP_DAC_OVERRIDE without CAP_SETPCAP";
    // Drop the capabilities of CAP_DAC_READ_SEARCH 2
    int retdacreadsearch = DropCAPDACREADSEARCH();
    EXPECT_EQ(retdacreadsearch, FALSE) << "ErrInfo: Drop CAP_DAC_READ_SEARCH without CAP_SETPCAP";
    // Drop the capabilities of CAP_FOWNER 3
    int retfowner = DropCAPFOWNER();
    EXPECT_EQ(retfowner, FALSE) << "ErrInfo: Drop CAP_FOWNER without CAP_SETPCAP";
    // Drop the capabilities of CAP_KILL 5
    int retkill = DropCAPKILL();
    EXPECT_EQ(retkill, FALSE) << "ErrInfo: Drop CAP_KILL without CAP_SETPCAP";
    // Drop the capabilities of CAP_SETGID 6
    int retsetgid = DropCAPSETGID();
    EXPECT_EQ(retsetgid, FALSE) << "ErrInfo: Drop CAP_SETGID without CAP_SETPCAP";
    // Drop the capabilities of CAP_SETUID 7
    int retsetuid = DropCAPSETUID();
    EXPECT_EQ(retsetuid, FALSE) << "ErrInfo: Drop CAP_SETUID without CAP_SETPCAP";
    // Drop the capabilities of CAP_SETPCAP 8
    int retsetpcapfailed = DropCAPSETPCAP();
    EXPECT_EQ(retsetpcapfailed, FALSE) << "ErrInfo: Drop CAP_SETPCAP without CAP_SETPCAP";
    // Drop the capabilities of CAP_SYS_NICE 23
    int retsysnice = DropCAPSYSNICE();
    EXPECT_EQ(retsysnice, FALSE) << "ErrInfo: Drop CAP_SYS_NICE without CAP_SETPCAP";
    // Drop the capabilities of CAP_SYS_TIME 25
    int retsystime = DropCAPSYSTIME();
    EXPECT_EQ(retsystime, FALSE) << "ErrInfo: Drop CAP_SYS_TIME without CAP_SETPCAP";
    if ((retchown != FALSE) || (retdacoverride != FALSE) || (retdacreadsearch != FALSE) || (retfowner != FALSE) ||
        (retkill != FALSE) || (retsetgid != FALSE) || (retsetuid != FALSE) || (retsetpcapfailed != FALSE) ||
        (retsysnice != FALSE) || (retsystime != FALSE) || (retsetpcap == FALSE)) {
        LOG("ErrInfo: Drop the capabilities without CAP_SETPCAP");
        return FALSE;
    }
    return 0;
}

static int CapsetWithVersion(pid_t pid, unsigned int version)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.pid = pid;
    capheader.version = version;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    // Capget based on input parameters
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        // Capset with abnormal parameter
        return FALSE;
    }
    return 0;
}

static int CapgetWithVersion(pid_t pid, unsigned int version)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.pid = pid;
    capheader.version = version;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    // Capget based on input parameters
    int ret = capget(&capheader, &capdataget[0]);
    if (ret != 0) {
        // Capget with abnormal parameter
        return FALSE;
    }
    return 0;
}
#endif

#if (defined(_BOARD_HI3516_) && defined(LITE_FS_VFAT)) || \
    (!defined(_BOARD_HI3516_) && (defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)))
static int CapgetWithCaps(pid_t pid, unsigned int caps)
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    capheader.pid = pid;
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("CapgetWithAllCap memset_s failed");
        return FALSE;
    };
    // Capget based on input parameters and check whether the capability is the same as the input parameter
    int ret = capget(&capheader, &capdataget[0]);
    if (ret != 0 || capdataget[0].effective != caps) {
        EXPECT_EQ(capdataget[0].effective, caps) << "ErrInfo: Pid = " << pid << ", process has wrong capability";
        return FALSE;
    }
    return 0;
}
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0100
 * @tc.name       : Processes with the CAP_CHOWN capability can invoke their management
                    and control interfaces to change the file owner
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0100, Function | MediumTest | Level2)
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
        // Step 1: Change the file owner with interface 'chown'
        ret = chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, UID10000, GID10000);
        if (ret != 0) {
            LOG("ErrInfo: Failed to change the file owner with CAP_CHOWN");
            exitCode = 1;
        }
        // Step 2: Drop the capabilities of CAP_CHOWN
        ret = DropCAPCHOWN();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_CHOWN");
            exitCode = 1;
        }
        // Step 3: Failed to change the file owner with interface 'chown'
        ret = chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, UID0, GID0);
        if (ret != FALSE) {
            LOG("ErrInfo: Change the file owner without CAP_CHOWN");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0200
 * @tc.name       : Processes with the single CAP_DAC_OVERRIDE capability can invoke their management
                    and control interfaces to ignore read and write execution verification
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0200, Function | MediumTest | Level2)
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
        // Step 1: Drop the capabilities of CAP_DAC_READ_SEARCH
        ret = DropCAPDACREADSEARCH();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_DAC_READ_SEARCH");
            exitCode = 1;
        }
        // Step 2.1: Invoke CAP_DAC_READ_SEARCH related interfaces successfully
        ret = TestDacReadSearchSuccess(1);
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchSuccess error");
            exitCode = 1;
        }
        // Step 2.2: Invoke CAP_DAC_OVERRIDE related interfaces successfully
        ret = TestDacOverrideSuccess();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideSuccess error");
            exitCode = 1;
        }
        // Step 3: Continue to drop the capabilities of CAP_DAC_OVERRIDE
        ret = DropCAPDACOVERRIDEAndREADSEARCH();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_DAC_OVERRIDE after CAP_DAC_READ_SEARCH revoked");
            exitCode = 1;
        }
        // Step 4.1: Failed to invoke CAP_DAC_READ_SEARCH related interfaces
        ret = TestDacReadSearchFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchFail error");
            exitCode = 1;
        }
        // Step 4.2: Failed to invoke CAP_DAC_OVERRIDE related interfaces
        ret = TestDacOverrideFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideFail error");
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
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0300
 * @tc.name       : Processes with the single CAP_DAC_READ_SEARCH capability can invoke their management
                    and control interfaces to ignore read execution verification
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0300, Function | MediumTest | Level3)
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
        // Step 1: Drop the capabilities of CAP_DAC_OVERRIDE
        ret = DropCAPDACOVERRIDE();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_DAC_OVERRIDE");
            exitCode = 1;
        }
        // Step 2.1: Invoke CAP_DAC_READ_SEARCH related interfaces successfully
        ret = TestDacReadSearchSuccess(1);
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchSuccess error");
            exitCode = 1;
        }
        // Step 2.2: Invoke CAP_DAC_OVERRIDE related interfaces successfully
        ret = TestDacOverrideFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideFail error");
            exitCode = 1;
        }
        // Step 3: Continue to drop the capabilities of CAP_DAC_READ_SEARCH
        ret = DropCAPDACOVERRIDEAndREADSEARCH();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_DAC_READ_SEARCH after CAP_DAC_OVERRIDE revoked");
            exitCode = 1;
        }
        // Step 4.1: Failed to invoke CAP_DAC_READ_SEARCH related interfaces
        ret = TestDacReadSearchFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchFail error");
            exitCode = 1;
        }
        // Step 4.2: Failed to invoke CAP_DAC_OVERRIDE related interfaces
        ret = TestDacOverrideFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideFail error");
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
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0400
 * @tc.name       : Processes that have the CAP_DAC_OVERRIDE
                    and CAP_DAC_READ_SEARCH capabilities can invoke their management
                    and control interfaces to ignore read and write execution verification
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0400, Function | MediumTest | Level3)
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
        // Step 1.1: Invoke CAP_DAC_READ_SEARCH related interfaces successfully
        ret = TestDacReadSearchSuccess(1);
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchSuccess error");
            exitCode = 1;
        }
        // Step 1.2: Invoke CAP_DAC_OVERRIDE related interfaces successfully
        ret = TestDacOverrideSuccess();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideSuccess error");
            exitCode = 1;
        }
        // Step 2: Drop the capabilities of CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH
        ret = DropCAPDACOVERRIDEAndREADSEARCH();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH");
            exitCode = 1;
        }
        // Step 3.1: Failed to invoke CAP_DAC_READ_SEARCH related interfaces
        ret = TestDacReadSearchFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacReadSearchFail error");
            exitCode = 1;
        }
        // Step 3.2: Failed to invoke CAP_DAC_OVERRIDE related interfaces
        ret = TestDacOverrideFail();
        if (ret != 0) {
            LOG("ErrInfo: TestDacOverrideFail error");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0500
 * @tc.name       : Processes with the CAP_FOWNER capability can invoke their management
                    and control interfaces to modify file permissions
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0500, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Change the file owner with interface 'chown'
    ret = chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, UID10000, GID10000);
    ASSERT_EQ(ret, 0) << "ErrInfo: Failed to change the file owner with CAP_CHOWN";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Change the file permission with CAP_FOWNER'
        ret = chmod(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, RWX);
        if (ret != 0) {
            LOG("ErrInfo: Failed to chmod with CAP_FOWNER");
            exitCode = 1;
        }
        // Step 2: Drop the capabilities of CAP_FOWNER
        ret = DropCAPFOWNER();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_FOWNER");
            exitCode = 1;
        }
        // Step 2: Failed to change the file permission with interface 'chmod'
        ret = chmod(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, NORWX);
        if (ret != FALSE) {
            LOG("ErrInfo: Change the file permission without CAP_FOWNER");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
        // Cleanup action: Restore the initial status of the file
        ret = chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, UID0, GID0);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the file owner with CAP_FOWNER";
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0600
 * @tc.name       : Processes with the CAP_KILL capability can invoke their management
                    and control interfaces to kill other processes
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0600, Function | MediumTest | Level2)
{
    int ret;
    int status1 = 0;
    int status2 = 0;
    // Preset action: Fork a sub process pid1
    pid_t pid1 = fork();
    ASSERT_TRUE(pid1 >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    // Preset action: Change the UID&GID of the sub process pid1 and keep it hibernated
    if (pid1 == 0) {
        SetUidGid(UID555, GID555);
        ChildSleep();
    } else {
        // Preset action: Fork a sub process pid2
        pid_t pid2 = fork();
        ASSERT_TRUE(pid2 >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid2 == 0) {
            int exitCode = 0;
            // Step 1: Drop the capabilities of CAP_KILL
            ret = DropCAPKILL();
            if (ret != 0) {
                LOG("ErrInfo: Failed to drop CAP_KILL");
                exitCode = 1;
            }
            // Step 2: Failed to kill the sub process
            ret = kill(pid1, SIGXFSZ);
            if (ret != FALSE) {
                LOG("ErrInfo: Kill process without CAP_KILL");
                exitCode = 1;
            }
            // Step 3: Change the UID&GID of the sub process pid1 and keep it hibernated
            ret = SetUidGid(UID555, GID555);
            if (ret != 0) {
                LOG("ErrInfo: Failed to set uid and gid");
                exitCode = 1;
            }
            // Step 4: The sub process exit with the exitCode
            exit(exitCode);
        } else {
            // Step 4: Kill the sub process pid1 successfully
            ret = kill(pid1, SIGXFSZ);
            EXPECT_EQ(ret, 0) << "ErrInfo: Failed to Kill pid1 with CAP_KILL";
            // Cleanup action: Wait for the sub process pid1 and pid2 to be killed
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);
            EXPECT_NE(WIFEXITED(status2), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid2;
            EXPECT_EQ(WEXITSTATUS(status2), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = "
            << pid2;
        }
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0700
 * @tc.name       : Processes with the CAP_SETGID capability can invoke their management
                    and control interfaces to change the group IDs of other processes
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0700, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    pid_t pid = fork(); // Preset action: Fork a sub process
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        ret = setgid(NUM500); // Step 1.1: Set GID with interface 'setgid'
        if (ret != 0) {LOG("ErrInfo: Failed to set GID with interface 'setgid', now process gid=%d", getpid());
            exitCode = 1; }
        ret = setregid(NUM600, NUM600); // Step 1.2: Set reGID with interface 'setregid'
        if (ret != 0) {LOG("ErrInfo: Failed to set reGID with interface 'setregid', now process gid=%d", getpid());
            exitCode = 1; }
        ret = setresgid(NUM700, NUM700, NUM700); // Step 1.3: Set resGID with interface 'setresgid'
        if (ret != 0) {LOG("ErrInfo: Failed to set resGID with interface 'setresgid', now process gid=%d", getpid());
            exitCode = 1; }
        ret = setgroups(NUM3, GROUPLIST); // Step 1.4: Set groups with interface 'setgroups'
        if (ret != 0) {LOG("ErrInfo: Failed to set groups with interface 'setgroups'");
            exitCode = 1; }
        ret = DropCAPSETGID(); // Step 2: Drop the capabilities of CAP_SETGID
        if (ret != 0) {LOG("ErrInfo: Failed to drop CAP_SETGID");
            exitCode = 1; }
        ret = setgid(NUM500); // Step 3.1: Failed to set GID with interface 'setgid'
        if (ret != FALSE) {LOG("ErrInfo: Set GID without CAP_SETGID, now process gid=%d", getpid());
            exitCode = 1; }
        ret = setregid(NUM500, NUM500); // Step 3.2: Failed to set reGID with interface 'setregid'
        if (ret != FALSE) {LOG("ErrInfo: Set reGID without CAP_SETGID, now process gid=%d", getpid());
            exitCode = 1; }
        ret = setresgid(NUM500, NUM500, NUM500); // Step 3.3: Failed to set resGID with interface 'setregid'
        if (ret != FALSE) {LOG("ErrInfo: Set resGID without CAP_SETGID, now process gid=%d", getpid());
            exitCode = 1; }
        ret = setgroups(NUM3, GROUPLIST); // Step 3.4: Failed to set groups with interface 'setgroups'
        if (ret != FALSE) {LOG("ErrInfo: Set groups without CAP_SETGID");
            exitCode = 1; } // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        waitpid(pid, &status, 0); // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0800
 * @tc.name       : Processes with the CAP_SETUID capability can invoke their management
                    and control interfaces to change user IDs of other processes
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0800, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1.1: Set UID with interface 'setuid'
        ret = setuid(NUM500);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set UID with interface 'setuid', now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 1.2: Set reUID with interface 'setreuid'
        ret = setreuid(NUM600, NUM600);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set reUID with interface 'setreuid', now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 1.3: Set resUID with interface 'setresuid'
        ret = setresuid(NUM700, NUM700, NUM700);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set resUID with interface 'setresuid', now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 2: Drop the capabilities of CAP_SETUID
        ret = DropCAPSETUID();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_SETUID");
            exitCode = 1;
        }
        // Step 3.1: Failed to set UID with interface 'setuid'
        ret = setuid(NUM500);
        if (ret != FALSE) {
            LOG("ErrInfo: Set UID without CAP_SETUID, now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 3.2: Failed to set reUID with interface 'setreuid'
        ret = setreuid(NUM500, NUM500);
        if (ret != FALSE) {
            LOG("ErrInfo: Set reUID without CAP_SETUID, now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 3.3: Failed to set resUID with interface 'setreuid'
        ret = setresuid(NUM500, NUM500, NUM500);
        if (ret != FALSE) {
            LOG("ErrInfo: Set resUID without CAP_SETUID, now process uid=%d", getpid());
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_0900
 * @tc.name       : Processes with the CAP_SETPCCAP capability can invoke their management
                    and control interfaces to add and delete capabilities for other processes
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest0900, Security | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Drop the capabilities of CAP_SETPCAP
        ret = DropCAPSETPCAP();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_SETPCAP");
            exitCode = 1;
        }
        // Step 2: Failed to add the capabilities of CAP_SETPCAP
        exitCode = AddCapUnauthorized(1);
        if (exitCode != 0) {
            LOG("ErrInfo: Add capabilities without CAP_SETPCAP");
        }
        // Step 3: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 4: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1000
 * @tc.name       : Processes with the CAP_SYS_NICE capability can invoke their management
                    and control interfaces to set the process priority
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1000, Function | MediumTest | Level2)
{
    int ret;
    int status1 = 0;
    int status2 = 0;
    // Preset action: Fork a sub process pid1
    pid_t pid1 = fork();
    ASSERT_TRUE(pid1 >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    // Preset action: keep the sub process pid1 hibernated
    if (pid1 == 0) {
        ChildSleep();
    } else {
        // Preset action: Fork a sub process pid2
        pid_t pid2 = fork();
        ASSERT_TRUE(pid2 >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid2 == 0) {
            int exitCode = 0;
            // Preset action: Obtains the test priority
            struct sched_param param = { 0 };
            ret = sched_getparam(pid1, &param);
            param.sched_priority--;
            // Step 1: Set the priority of the sub process successfully
            ret = sched_setparam(pid1, &param);
            if (ret != 0) {
                LOG("ErrInfo: Failed to set priority with CAP_SYS_NICE");
                exitCode = 1;
            }
            // Step 2: Drop the capabilities of CAP_SYS_NICE
            ret = DropCAPSYSNICE();
            if (ret != 0) {
                LOG("ErrInfo: Failed to drop CAP_SYS_NICE");
                exitCode = 1;
            }
            // Step 3: Failed to set the priority of the sub process
            param.sched_priority++;
            ret = sched_setparam(pid1, &param);
            if (ret != FALSE) {
                LOG("ErrInfo: Set priority without CAP_SYS_NICE");
                exitCode = 1;
            }
            // Step 4: The sub process exit with the exitCode
            exit(exitCode);
        } else {
            // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
            waitpid(pid2, &status2, 0);
            EXPECT_NE(WIFEXITED(status2), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid2;
            EXPECT_EQ(WEXITSTATUS(status2), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = "
            << pid2;
            // Cleanup action: Kill the sub process and wait for the sub process to be killed
            ret = kill(pid1, SIGXFSZ);
            EXPECT_EQ(ret, 0) << "ErrInfo: Failed to kill the sub process pid1";
            waitpid(pid1, &status1, 0);
        }
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1100
 * @tc.name       : Processes with the CAP_SYS_TIME capability can call their management
                    and control interfaces and change the system clock
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1100, Function | MediumTest | Level2)
{
    int ret;
    int status = 0;
    struct timespec tp = { 0 };
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Preset action: Obtains the system time and the test time
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_sec += 1;
        // Step 1: Set the System Time with the test time successfully
        ret = clock_settime(CLOCK_REALTIME, &tp);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set clocktime with CAP_SYS_TIME");
            exitCode = 1;
        }
        // Step 2: Drop the capabilities of CAP_SYS_TIME
        ret = DropCAPSYSTIME();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop CAP_SYS_TIME");
            exitCode = 1;
        }
        // Step 3: Failed to set the system time with the test time
        tp.tv_sec += 1;
        ret = clock_settime(CLOCK_REALTIME, &tp);
        if (ret != FALSE) {
            LOG("ErrInfo: Set clocktime without CAP_SYS_TIME");
            exitCode = 1;
        }
        // Step 4: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1200
 * @tc.name       : Processes without the CAP_SETPCAP capability cannot drop any capability
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1200, Function | MediumTest | Level3)
{
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Failed to add capabilities without CAP_SETPCAP
        exitCode = CapsetWithoutSETPCAP();
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1300
 * @tc.name       : Inheritance of process capabilities
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1300, Function | MediumTest | Level1)
{
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Query the sub process capabilities for 10000 times
        for (int number = 0; number < NUM10000; number++) {
            exitCode = CapgetWithAllCap(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapgetWithAllCap error during the %d time", number);
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
        // Step 4: Query the parent process capabilities
        int ret = CapgetWithAllCap(1);
        EXPECT_EQ(ret, 0) << "ErrInfo: CapgetWithAllCap error";
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1400
 * @tc.name       : Invoke the capset interface to add and drop the process capabilities for 10000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1400, Reliability | MediumTest | Level2)
{
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Drop and add the sub process capabilities for 10000 times
        for (int number = 0; number < NUM10000; number++) {
            exitCode = CapsetOnlySETPCAP(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapsetOnlySETPCAP error during the %d time", number);
                break;
            }
            exitCode = AddCapUnauthorized(number);
            if (exitCode != 0) {
                LOG("ErrInfo: AddCapUnauthorized error during the %d time", number);
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1500
 * @tc.name       : Invoke the capset interface to revoke the process capabilities which not exist for 10000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1500, Reliability | MediumTest | Level2)
{
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        struct __user_cap_header_struct capheader = { 0 };
        (void)memset_s(&capheader, sizeof(struct __user_cap_header_struct),
            0, sizeof(struct __user_cap_header_struct));
        capheader.version = _LINUX_CAPABILITY_VERSION_3;
        capheader.pid = 0;
        struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
        (void)memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
            LINUX_FULL_CAP, CAP_NUM * sizeof(struct __user_cap_data_struct));
        capdata[CAP_TO_INDEX(INVALID_CAP_TO_INDEX)].permitted &= ~CAP_TO_MASK(INVALID_CAP_TO_INDEX);
        capdata[CAP_TO_INDEX(INVALID_CAP_TO_INDEX)].effective &= ~CAP_TO_MASK(INVALID_CAP_TO_INDEX);
        capdata[CAP_TO_INDEX(INVALID_CAP_TO_INDEX)].inheritable &= ~CAP_TO_MASK(INVALID_CAP_TO_INDEX);
        for (int number = 0; number < NUM10000; number++) {
            // Step 1: Drop an abnormal capability for 10000 times
            int ret = capset(&capheader, &capdata[0]);
            if (ret != 0) {
                LOG("ErrInfo: Drop an abnormal capability during the %d time", number);
                exitCode = 1;
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1600
 * @tc.name       : Enter the exception parameter for 10000 times when invoke the capset interface
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1600, Reliability | MediumTest | Level3)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM10000; number++) {
            // Step 1.1: Capset the abnormal capheader.pid '-1' for 10000 times
            ret = CapsetWithVersion(FALSE, _LINUX_CAPABILITY_VERSION_3);
            if (ret != FALSE) {
                LOG("ErrInfo: Capset with the abnormal capheader.pid '-1' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.2: Capset the abnormal capheader.pid '65536' for 10000 times
            ret = CapsetWithVersion(INVAILD_PID, _LINUX_CAPABILITY_VERSION_3);
            if (ret != FALSE) {
                LOG("ErrInfo: Capset with the abnormal capheader.pid '65536' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.3: Capset the abnormal capheader.version '1' for 10000 times
            ret = CapsetWithVersion(0, 1);
            if (ret != FALSE) {
                LOG("ErrInfo: Capset with the abnormal normal capheader.version '1' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.4: Capset the abnormal capheader.version '_LINUX_CAPABILITY_VERSION_1' for 10000 times
            ret = CapsetWithVersion(0, _LINUX_CAPABILITY_VERSION_1);
            if (ret != 0) {
                LOG("ErrInfo: Capset with the abnormal capheader.version '_LINUX_CAPABILITY_VERSION_1'"
                "during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.5: Add normal capheader.version '_LINUX_CAPABILITY_VERSION_3' for 10000 times
            ret = CapsetWithVersion(0, _LINUX_CAPABILITY_VERSION_3);
            if (ret != 0) {
                LOG("ErrInfo: Failed to capset with normal capheader.version during the %d time", number);
                exitCode = 1;
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1700
 * @tc.name       : Invoke the capget interface to query the process capabilities for 10000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1700, Reliability | MediumTest | Level2)
{
    // Step 1: Query the process capabilities for 10000 times
    for (int number = 0; number < NUM10000; number++) {
        int ret = CapgetWithAllCap(number);
        if (ret != 0) {
            EXPECT_EQ(ret, 0) << "ErrInfo: CapgetWithAllCap error during the " << number << " time";
            break;
        }
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1800
 * @tc.name       : Invoke the capget interface to query the process capabilities which not exist for 10000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1800, Reliability | MediumTest | Level3)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Drop all the sub process capabilities
        DropAllCAP();
        // Step 2: Obtain the sub process capabilities
        for (int number = 0; number < NUM10000; number++) {
            exitCode = CapgetWithNoCap(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapgetWithNoCap error during the %d time", number);
                break;
            }
        }
        // Step 3: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 4: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
        // Step 5: Query the parent process capabilities
        ret = CapgetWithAllCap(1);
        EXPECT_EQ(ret, 0) << "ErrInfo: CapgetWithAllCap error";
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_1900
 * @tc.name       : Enter the exception parameter for 10000 times when invoke the capget interface
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest1900, Reliability | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM10000; number++) {
            // Step 1.1: Capget the abnormal capheader.pid '-1' for 10000 times
            ret = CapgetWithVersion(FALSE, _LINUX_CAPABILITY_VERSION_3);
            if (ret != FALSE) {
                LOG("ErrInfo: Capget with the abnormal capheader.pid '-1' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.2: Capget the abnormal capheader.pid '65536' for 10000 times
            ret = CapgetWithVersion(INVAILD_PID, _LINUX_CAPABILITY_VERSION_3);
            if (ret != FALSE) {
                LOG("ErrInfo: Capget with the abnormal capheader.pid '65536' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.3: Capget the abnormal capheader.version '1' for 10000 times
            ret = CapgetWithVersion(0, 1);
            if (ret != FALSE) {
                LOG("ErrInfo: Capget with the abnormal capheader.version '1' during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.4: Capget with the abnormal capheader.version '_LINUX_CAPABILITY_VERSION_1' for 10000 times
            ret = CapgetWithVersion(0, _LINUX_CAPABILITY_VERSION_1);
            if (ret != 0) {
                LOG("ErrInfo: Capget with the abnormal capheader.version '_LINUX_CAPABILITY_VERSION_1'"
                "during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 1.5: Capget the normal capheader.version '_LINUX_CAPABILITY_VERSION_3' for 10000 times
            ret = CapgetWithVersion(0, _LINUX_CAPABILITY_VERSION_3);
            if (ret != 0) {
                LOG("ErrInfo: Failed to capget with the normal capheader.version '_LINUX_CAPABILITY_VERSION_3'"
                "during the %d time", number);
                exitCode = 1;
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2000
 * @tc.name       : The process repeatedly invokes the interfaces controlled by its capability for 10000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2000, Reliability | MediumTest | Level2)
{
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Invoke CAP_DAC_READ_SEARCH related interfaces for 10000 times
        for (int number = 0; number < NUM10000; number++) {
            exitCode = TestDacReadSearchSuccess(number);
            if (exitCode != 0) {
                LOG("ErrInfo: TestDacReadSearchSuccess error during the %d time", number);
                break;
            }
        }
        // Step 2: The sub process exit with the exitCode
        exit(exitCode);
    } else {
        // Step 3: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2100
 * @tc.name       : Five processes concurrently invoke APIs managed by the capability for 5000 times
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2100, Reliability | MediumTest | Level2)
{
    int status = 0;
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
            // Step 1: Five sub processes simultaneously drop capabilities for 5000 times
            exitCode = CapsetOnlySETPCAP(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapsetOnlySETPCAP Error during the %d time", number);
                break;
            }
            // Step 2: Five sub processes simultaneously add capabilities for 5000 times
            exitCode = AddCapUnauthorized(number);
            if (exitCode != 0) {
                LOG("ErrInfo: AddCapUnauthorized Error during the %d time", number);
                break;
            }
            // Step 3: Five sub processes simultaneously capget for 5000 times
            exitCode = CapgetOnlySETPCAP(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapgetOnlySETPCAP Error during the %d time", number);
                break;
            }
        }
        // Step 4: Five sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for five sub processes to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM5; num2++) {
            wait(&status);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: Pid = "<< pid
            << ", its exitCode is wrong and test case failed, please query logs";
        }
    }
}
#endif

#ifndef _BOARD_HI3516_
#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2200
 * @tc.name       : Check whether the default configuration of the system process capabilities
                    is the same as that described in the design document
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2200, Security | MediumTest | Level1)
{
    int ret;
    // Step 1: Check the capability of process 'init', pid = 1
    ret = CapgetWithCaps(INIT_PID, INIT_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 1, process init has wrong capability";
    // Step 2: Check the capability of process 'KProcess', pid = 2
    ret = CapgetWithCaps(KPROCESS_PID, KPROCESS_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 2, process KProcess has wrong capability";
    // Step 3: Check the capability of process 'shell', pid = 3
    ret = CapgetWithCaps(SHELL_PID, SHELL_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 3, process shell has wrong capability";
    // Step 4: Check the capability of process 'apphilogcat', pid = 4
    ret = CapgetWithCaps(HILOGCAT_PID, HILOGCAT_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 4, process apphilogcat has wrong capability";
    // Step 5: Check the capability of process 'foundation', pid = 5
    ret = CapgetWithCaps(FOUNDATION_PID, FOUNDATION_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 5, process foundation has wrong capability";
    // Step 6: Check the capability of process 'bundle_daemon', pid = 6
    ret = CapgetWithCaps(BUNDLE_DAEMON_PID, BUNDLE_DAEMON_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 6, process bundle_daemon has wrong capability";
    // Step 7: Check the capability of process 'appspawn', pid = 7
    ret = CapgetWithCaps(APPSPAWN_PID, APPSPAWN_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 7, process appspawn has wrong capability";
    // Step 8: Check the capability of process 'media_server', pid = 8
    ret = CapgetWithCaps(MEDIA_SERVER_PID, MEDIA_SERVER_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 8, process media_server has wrong capability";
    // Step 9: Check the capability of process 'wms_server' or 'ai_server', pid = 9
    ret = CapgetWithCaps(WMS_SERVER_OR_AI_SERVER_PID, WMS_SERVER_OR_AI_SERVER_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 9, process wms_server or ai_server has wrong capability";
}
#endif
#endif

#if defined(LITE_FS_VFAT) && defined(_BOARD_HI3516_)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2200
 * @tc.name       : Check whether the default configuration of the system process capabilities
                    is the same as that described in the design document
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2200, Security | MediumTest | Level1)
{
    int ret;
    // Step 1: Check the capability of process 'init', pid = 1
    ret = CapgetWithCaps(INIT_PID_2, INIT_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 1, process init has wrong capability";
    // Step 2: Check the capability of process 'KProcess', pid = 2
    ret = CapgetWithCaps(KPROCESS_PID_2, KPROCESS_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 2, process KProcess has wrong capability";
    // Step 3: Check the capability of process 'shell', pid = 8
    ret = CapgetWithCaps(SHELL_PID_2, SHELL_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 8, process shell has wrong capability";
    // Step 4: Check the capability of process 'apphilogcat', pid = 10
    ret = CapgetWithCaps(HILOGCAT_PID_2, HILOGCAT_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 10, process apphilogcat has wrong capability";
    // Step 5: Check the capability of process 'foundation', pid = 3
    ret = CapgetWithCaps(FOUNDATION_PID_2, FOUNDATION_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 3, process foundation has wrong capability";
    // Step 6: Check the capability of process 'bundle_daemon', pid = 4
    ret = CapgetWithCaps(BUNDLE_DAEMON_PID_2, BUNDLE_DAEMON_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 4, process bundle_daemon has wrong capability";
    // Step 7: Check the capability of process 'appspawn', pid = 5
    ret = CapgetWithCaps(APPSPAWN_PID_2, APPSPAWN_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 5, process appspawn has wrong capability";
    // Step 8: Check the capability of process 'media_server', pid = 6
    ret = CapgetWithCaps(MEDIA_SERVER_PID_2, MEDIA_SERVER_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 6, process media_server has wrong capability";
    // Step 9: Check the capability of process 'wms_server' or 'ai_server', pid = 7
    ret = CapgetWithCaps(WMS_SERVER_OR_AI_SERVER_PID_2, WMS_SERVER_OR_AI_SERVER_CAP);
    EXPECT_EQ(ret, 0) << "ErrInfo: Pid = 7, process wms_server or ai_server has wrong capability";
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2300
 * @tc.name       : Check whether the default configuration of the capability of the third-party application process
                    is the same as that described in the design document
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2300, Security | MediumTest | Level1)
{
    int ret;
    struct __user_cap_header_struct capheader = { 0 };
    (void)memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    struct __user_cap_data_struct capdataget[CAP_NUM] = { { 0 }, { 0 } };
    (void)memset_s(capdataget, CAP_NUM * sizeof(struct __user_cap_data_struct),
    0, CAP_NUM * sizeof(struct __user_cap_data_struct));
    pid_t pid = getpid();
    for (int num = OTHER_PID; num <= pid; num++) {
        // Step 1: The current test process has all capabilities
        if (num == pid) {
            capheader.pid = pid;
            ret = capget(&capheader, &capdataget[0]);
            EXPECT_EQ(capdataget[0].effective, OHOS_FULL_CAP) <<"ErrInfo: Pid = " << num
            << ", test_process has wrong capability";
        } else {
            // Step 2: Check the capability of process from pid = 9
            capheader.pid = num;
            ret = capget(&capheader, &capdataget[0]);
            if (ret == 0) {
                // Step 2.1: Check the capability of process which exists now
                EXPECT_EQ(capdataget[0].effective, NO_CAP) << "ErrInfo: Pid = " << num
                << ", thirdPartyApp has wrong capability";
            } else {
                // Step 2.2: Check the capability of process which not exist now
                EXPECT_EQ(ret, FALSE) << "ErrInfo: Capget return error, now process uid=" << getuid();
            }
        }
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2400
 * @tc.name       : The process continuously invokes the capset and capget interfaces,
 which does not affect the use of other processes
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2400, Function | MediumTest | Level1)
{
    int status = 0;
    // Preset action: Create a txt
    CreateTxt();
    // Preset action: Fork two sub processes
    pid_t pid;
    for (int num = 0; num < NUM2; num++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid == 0) {
            break;
        }
    }
    // get one parent & two children
    if (pid == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM5000; number++) {
            // Step 1: Two sub processes simultaneously drop capabilities for 5000 times
            exitCode = CapsetOnlySETPCAP(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapsetOnlySETPCAP Error during the %d time", number);
                break;
            }
            // Step 2: Two sub processes simultaneously add capabilities for 5000 times
            exitCode = AddCapUnauthorized(number);
            if (exitCode != 0) {
                LOG("ErrInfo: AddCapUnauthorized Error during the %d time", number);
                break;
            }
            // Step 3: Two sub processes simultaneously capget for 5000 times
            exitCode = CapgetOnlySETPCAP(number);
            if (exitCode != 0) {
                LOG("ErrInfo: CapgetOnlySETPCAP Error during the %d time", number);
                break;
            }
        }
        // Step 4: Two sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process simultaneously invoke chown interfaces for 1000 times
        for (int number2 = 0; number2 < NUM1000; number2++) {
            int ret = chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, number2, number2);
            if (ret != 0) {
                EXPECT_EQ(ret, 0) << "ErrInfo: Failed to chown during the " << number2 << " time";
                break;
            }
        }
        // Step 6: The parent process wait for two sub processes to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM2; num2++) {
            wait(&status);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: Pid = "<< pid
            << ", its exitCode is wrong and test case failed, please query logs";
        }
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2500
 * @tc.name       : Performance test of capset and capget interface
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2500, Performance | MediumTest | Level2)
{
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        struct timespec tp = { 0 };
        struct timespec starttime = { 0 };
        struct timespec endtime = { 0 };
        tp.tv_sec = 0;
        tp.tv_nsec = 0;
        // Preset action: Obtains the system time -> starttime
        clock_gettime(CLOCK_REALTIME, &starttime);
        // Step 1: Capset and capget for 200000 times
        for (int number = 0; number < NUM10000; number++) {
            CapsetOnlySETPCAP(number);
            AddCapUnauthorized(number);
            CapgetOnlySETPCAP(number);
        }
        // Step 2: Obtains the system time again -> endtime
        clock_gettime(CLOCK_REALTIME, &endtime);
        // Step 3: Compare the starttime and the endtime -> tp
        tp = CompareTime(starttime, endtime);
        if (tp.tv_sec > NUM20) {
            LOG("ErrInfo: Capset and capget for 200000 times used %d.%d s", tp.tv_sec, tp.tv_nsec);
            exitCode = 1;
        }
        // Step 4: Two sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // Step 5: The parent process wait for the sub process to exit and obtain the exitCode
        waitpid(pid, &status, 0);
        EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
        EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: The exitCode is wrong, please query logs, child_pid = " << pid;
    }
}
#endif

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_Capability_2600
 * @tc.name       : Performance test of the interface managed by Capability
 * @tc.desc       : [C-SECURITY-0100]
 */
HWTEST_F(CapabilityTestSuite, CapabilityTest2600, Performance | MediumTest | Level2)
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
    // Step 1: Chown for 10000 times
    for (int number = 0; number < NUM1000; number++) {
        chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, number, number);
    }
    // Step 2: Obtains the system time again -> endtime
    clock_gettime(CLOCK_REALTIME, &endtime);
    // Step 3: Compare the starttime and the endtime -> tp
    tp = CompareTime(starttime, endtime);
    EXPECT_LE(tp.tv_sec, NUM80) << "ErrInfo: Chown for 1000 times used " << tp.tv_sec << "." << tp.tv_nsec << "s";
    // Cleanup action: Restore the initial status of the file
    chown(TOP_DIR "/" CAPDIR0 "/" CAPDIR0_CAPFILE0, UID0, GID0);
}
#endif
