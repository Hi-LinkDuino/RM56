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

#include "UidGidTest.h"
#include <unistd.h>
#include <sys/types.h>
#include "log.h"
#include "utils.h"

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_DAC_SetGetUID_0100
 * @tc.name   setuid and getuid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGetuid, Function | MediumTest | Level1)
{
    AssertAllUid(SHELL_UID);
    uid_t uid = getuid();
    ASSERT_EQ(uid, SHELL_UID);

    uid_t newUid = GetRandom(100);
    LOG("new uid = %d", newUid);
    int rt = setuid(newUid);
    ASSERT_EQ(rt, 0);
    uid = getuid();
    ASSERT_EQ(uid, newUid);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    } else { // child
        uid = getuid();
        if (uid != newUid) {
            LOG("child uid check fail, child uid=%d", uid);
            exit(1);
        }
        LOG("child uid check OK");
        exit(0);
    }
    rt = setuid(0);
    ASSERT_EQ(rt, 0);
    uid = getuid();
    ASSERT_EQ(uid, 0);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetUID_0200
 * @tc.name   test setuid fail if input uid is negative
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetuidFail, Function | MediumTest | Level3)
{
    uid_t uid = getuid();
    ASSERT_EQ(uid, SHELL_UID);

    int newUid = -GetRandID();
    LOG("new uid = %d", newUid);
    int rt = setuid(newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    uid = getuid();
    ASSERT_EQ(uid, SHELL_UID);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetGetGID_0100
 * @tc.name   setgid and getgid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGetgid, Function | MediumTest | Level1)
{
    AssertAllGid(SHELL_GID);
    gid_t gid = getgid();
    ASSERT_EQ(gid, SHELL_GID);

    uid_t newgid = GetRandom(100);
    LOG("new gid = %d", newgid);
    int rt = setgid(newgid);
    ASSERT_EQ(rt, 0);
    gid = getgid();
    ASSERT_EQ(gid, newgid);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    } else { // child
        gid = getgid();
        if (gid != newgid) {
            LOG("child gid check fail, child gid=%d", gid);
            exit(1);
        }
        LOG("child guid check OK");
        exit(0);
    }
    rt = setgid(0);
    ASSERT_EQ(rt, 0);
    gid = getgid();
    ASSERT_EQ(gid, 0);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetGID_0200
 * @tc.name   test setgid fail if input gid is negative
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetgidFail, Function | MediumTest | Level3)
{
    gid_t gid = getgid();
    ASSERT_EQ(gid, SHELL_GID);

    int newgid = -GetRandID();
    LOG("new gid = %d", newgid);
    int rt = setgid(newgid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    gid = getgid();
    ASSERT_EQ(gid, SHELL_GID);
}


/**
 * @tc.number SUB_KERNEL_DAC_SetGetEUID_0100
 * @tc.name   seteuid and geteuid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGeteuid, Function | MediumTest | Level1)
{
    AssertAllUid(SHELL_UID);

    uid_t newEuid = GetRandID();
    LOG("new euid = %d", newEuid);
    int rt = seteuid(newEuid);
    ASSERT_EQ(rt, 0);
    uid_t euid = geteuid();
    ASSERT_EQ(euid, newEuid);
    AssertAllUid(newEuid);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    } else { // child
        uid_t uid, euid, suid;
        getresuid(&uid, &euid, &suid);
        if (uid != newEuid || euid != newEuid || suid != newEuid) {
            LOG("child resuid check fail, actual uid=%d,euid=%d,suid=%d,", uid, euid, suid);
            exit(1);
        }
        LOG("child uid check OK");
        exit(0);
    }
    rt = seteuid(SHELL_UID);
    ASSERT_EQ(rt, 0);
    euid = geteuid();
    ASSERT_EQ(euid, SHELL_UID);
    AssertAllUid(SHELL_UID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetEUID_0200
 * @tc.name   test seteuid fail if input uid is negative
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSeteuidFail, Function | MediumTest | Level3)
{
    uid_t uid = getuid();
    ASSERT_EQ(uid, SHELL_UID);

    int newEuid = -GetRandID();
    LOG("new euid = %d", newEuid);
    int rt = seteuid(newEuid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    uid = getuid();
    ASSERT_EQ(uid, SHELL_UID);
    uid_t euid = geteuid();
    ASSERT_EQ(euid, SHELL_UID);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetGetEGID_0100
 * @tc.name   setegid and getegid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGetegid, Function | MediumTest | Level1)
{
    AssertAllGid(SHELL_GID);

    uid_t newEgid = GetRandID();
    LOG("new egid = %d", newEgid);
    int rt = setegid(newEgid);
    ASSERT_EQ(rt, 0);
    gid_t egid = getegid();
    ASSERT_EQ(egid, newEgid);
    AssertAllGid(newEgid);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid > 0) { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    } else { // child
        gid_t gid, egid, sgid;
        getresgid(&gid, &egid, &sgid);
        if (gid != newEgid || egid != newEgid || sgid != newEgid) {
            LOG("child resgid check fail, actual gid=%d,egid=%d,sgid=%d,", gid, egid, sgid);
            exit(1);
        }
        LOG("child gid check OK");
        exit(0);
    }
    rt = setegid(SHELL_GID);
    ASSERT_EQ(rt, 0);
    egid = getegid();
    ASSERT_EQ(egid, SHELL_GID);
    AssertAllGid(SHELL_GID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetEGID_0200
 * @tc.name   test setegid fail if input gid is negative
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetegidFail, Function | MediumTest | Level3)
{
    gid_t gid = getgid();
    ASSERT_EQ(gid, SHELL_GID);

    int newEgid = -GetRandID();
    LOG("new egid = %d", newEgid);
    int rt = setegid(newEgid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    gid = getgid();
    ASSERT_EQ(gid, SHELL_GID);
    gid_t egid = getegid();
    ASSERT_EQ(egid, SHELL_GID);
}


/**
 * @tc.number SUB_KERNEL_DAC_SetREUID_0100
 * @tc.name   setreuid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetreuid, Function | MediumTest | Level1)
{
    AssertAllUid(SHELL_UID);

    int newUid = GetRandID();
    LOG("new uid1 = %d", newUid);
    int rt = setreuid(newUid, newUid);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid2 = %d", newUid);
    rt = setreuid(newUid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid3 = %d", newUid);
    rt = setreuid(-1, newUid);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    LOG("use all -1");
    rt = setreuid(-1, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    LOG("restore to 0");
    rt = setreuid(SHELL_UID, SHELL_UID);
    ASSERT_EQ(rt, 0);
    AssertAllUid(SHELL_UID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetREUID_0200
 * @tc.name   test setreuid fail if input uid is negative, or ruid and euid is not equal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetreuidFail, Function | MediumTest | Level2)
{
    AssertAllUid(SHELL_UID);

    int newUid = GetRandID();
    LOG("new uid = %d", -newUid);
    int rt = setreuid(-newUid, -newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    rt = setreuid(-newUid, newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    rt = setreuid(-newUid, -1);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    int newUid2 = GetRandID();
    LOG("new uid2 = %d", -newUid2);
    rt = setreuid(newUid, -newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setreuid(newUid2, -newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    AssertAllUid(SHELL_UID);

    LOG("check uids of setreuid should equal");
    rt = setreuid(newUid, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);

    AssertAllUid(SHELL_UID);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetREGID_0100
 * @tc.name   setregid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetregid, Function | MediumTest | Level1)
{
    AssertAllGid(SHELL_GID);

    int newGid = GetRandID();
    LOG("new gid1 = %d", newGid);
    int rt = setregid(newGid, newGid);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid2 = %d", newGid);
    rt = setregid(newGid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid3 = %d", newGid);
    rt = setregid(-1, newGid);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    LOG("use all -1");
    rt = setregid(-1, -1);
    EXPECT_EQ(rt, 0);
    AssertAllGid(newGid);

    LOG("restore to 0");
    rt = setregid(SHELL_GID, SHELL_GID);
    ASSERT_EQ(rt, 0);
    AssertAllGid(SHELL_GID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetREGID_0200
 * @tc.name   test setregid fail if input gid is negative, or rgid and egid is not equal
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetregidFail, Function | MediumTest | Level2)
{
    AssertAllGid(SHELL_GID);

    int newGid = GetRandID();
    LOG("new gid = %d", -newGid);
    int rt = setregid(-newGid, -newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    rt = setregid(-newGid, newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    rt = setregid(-newGid, -1);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    int newGid2 = GetRandID();
    LOG("new gid2 = %d", -newGid2);
    rt = setregid(newGid, -newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setregid(newGid2, -newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    AssertAllGid(SHELL_GID);

    LOG("check gids of setregid should equal");
    rt = setregid(newGid, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);

    AssertAllGid(SHELL_GID);
}

// ================================== res ===========================================
/**
 * @tc.number SUB_KERNEL_DAC_SetGetRESUID_0100
 * @tc.name   setresuid and getresuid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGetresuid, Function | MediumTest | Level1)
{
    AssertAllUid(SHELL_UID);

    int newUid = GetRandID();
    LOG("new uid1 = %d", newUid);
    int rt = setresuid(newUid, newUid, newUid);
    ASSERT_EQ(rt, 0);
    uid_t uid = getuid();
    ASSERT_EQ(uid, newUid);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid2 = %d", newUid);
    rt = setresuid(newUid, -1, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid3 = %d", newUid);
    rt = setresuid(-1, newUid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid4 = %d", newUid);
    rt = setresuid(-1, -1, newUid);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid5 = %d", newUid);
    rt = setresuid(-1, newUid, newUid);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid6 = %d", newUid);
    rt = setresuid(newUid, -1, newUid);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    newUid = GetRandID();
    LOG("new uid7 = %d", newUid);
    rt = setresuid(newUid, newUid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    LOG("use all -1");
    rt = setresuid(-1, -1, -1);
    ASSERT_EQ(rt, 0);
    AssertAllUid(newUid);

    LOG("restore to %d", SHELL_UID);
    rt = setresuid(SHELL_UID, SHELL_UID, SHELL_UID);
    ASSERT_EQ(rt, 0);
    AssertAllUid(SHELL_UID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetRESUID_0200
 * @tc.name   test setresuid fail if input uid is negative, or ruid,euid,suid is not equal to each other.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetresuidFail, Function | MediumTest | Level2)
{
    AssertAllUid(SHELL_UID);

    int newUid = GetRandID();
    int newUid2 = GetRandID();
    LOG("new uid = %d, new uid2 = %d", newUid, newUid2);
    int rt = setresuid(-newUid, newUid2, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresuid(newUid2, -newUid, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresuid(newUid2, -1, -newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresuid(-1, -1, -newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    AssertAllUid(SHELL_UID);

    LOG("check uids of setresuid should all equal to each other");
    rt = setresuid(newUid, newUid2, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresuid(newUid2, newUid, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresuid(newUid2, -1, newUid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresuid(-1, newUid, newUid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);

    AssertAllUid(SHELL_UID);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetGetRESGID_0100
 * @tc.name   setresgid and getresgid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetGetresgid, Function | MediumTest | Level1)
{
    AssertAllGid(SHELL_GID);

    int newGid = GetRandID();
    LOG("new gid1 = %d", newGid);
    int rt = setresgid(newGid, newGid, newGid);
    ASSERT_EQ(rt, 0);
    gid_t gid = getgid();
    ASSERT_EQ(gid, newGid);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid2 = %d", newGid);
    rt = setresgid(newGid, -1, -1);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid3 = %d", newGid);
    rt = setresgid(-1, newGid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid4 = %d", newGid);
    rt = setresgid(-1, -1, newGid);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid5 = %d", newGid);
    rt = setresgid(-1, newGid, newGid);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid6 = %d", newGid);
    rt = setresgid(newGid, -1, newGid);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    newGid = GetRandID();
    LOG("new gid7 = %d", newGid);
    rt = setresgid(newGid, newGid, -1);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    LOG("use all -1");
    rt = setresgid(-1, -1, -1);
    ASSERT_EQ(rt, 0);
    AssertAllGid(newGid);

    LOG("restore to %d", SHELL_GID);
    rt = setresgid(SHELL_GID, SHELL_GID, SHELL_GID);
    ASSERT_EQ(rt, 0);
    AssertAllGid(SHELL_GID);
}
/**
 * @tc.number SUB_KERNEL_DAC_SetGetRESGID_0200
 * @tc.name   test setresgid fail if input ids is negative, or rgid,egid,sgid is not equal to each other.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetresgidFail, Function | MediumTest | Level2)
{
    AssertAllGid(SHELL_GID);

    int newGid = GetRandID();
    int newGid2 = GetRandID();
    LOG("new gid = %d, new gid2 = %d", newGid, newGid2);
    int rt = setresgid(-newGid, newGid2, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresgid(newGid2, -newGid, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresgid(newGid2, -1, -newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);
    rt = setresgid(-1, -1, -newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EINVAL);

    AssertAllGid(SHELL_GID);

    LOG("check gids of setresgid should all equal to each other");
    rt = setresgid(newGid, newGid2, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresgid(newGid2, newGid, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresgid(newGid2, -1, newGid);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);
    rt = setresgid(-1, newGid, newGid2);
    ASSERT_EQ(rt, -1);
    ASSERT_EQ(errno, EPERM);

    AssertAllGid(SHELL_GID);
}

/**
 * @tc.number SUB_KERNEL_DAC_GetGroups_0100
 * @tc.name   getgroups basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testGetgroups, Function | MediumTest | Level1)
{
    const int testSize = 10;
    gid_t gidList[testSize] = {0};
    int n = getgroups(0, gidList);
    ASSERT_EQ(n, 1);
    int rt = getgroups(n, gidList);
    ASSERT_EQ(gidList[0], SHELL_GID);
    ASSERT_EQ(rt, n);
    n = getgroups(testSize, gidList);
    ASSERT_EQ(n, 1);
    ASSERT_EQ(gidList[0], SHELL_GID);

}

/**
 * @tc.number SUB_KERNEL_DAC_SetGroups_0100
 * @tc.name   setgroups function test. also test invalid size of getroups, and clear groups function.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetgroups1, Function | MediumTest | Level1)
{
    int groupSize = GetRandom(MAX_PROCESS_GROUPS - 3) + 3;
    gid_t *gidListIn  = (gid_t*)malloc(groupSize * sizeof(gid_t));
    if (!gidListIn) {
        LOG("gidListIn malloc fail!");
        FAIL();
    }
    gid_t *gidListOut = (gid_t*)malloc(groupSize * sizeof(gid_t));
    if (!gidListOut) {
        LOG("gidListOut malloc fail!");
        free(gidListIn);
        FAIL();
    }
    gidListIn[0]  = SHELL_GID;
    for (int i=1; i<groupSize; i++) {
        gidListIn[i]  = GetRandID();
        gidListOut[i] = 0;
    }

    int rt = setgroups(groupSize, gidListIn);
    EXPECT_EQ(rt, 0);

    int n = getgroups(1, gidListOut);
    EXPECT_EQ(n, -1) << "input size less than the number of group IDs should fail";
    EXPECT_EQ(errno, EINVAL);

    n = getgroups(groupSize, gidListOut);
    EXPECT_EQ(n, groupSize);
    for (int i=0; i<groupSize; i++) {
        EXPECT_EQ(gidListIn[i], gidListOut[i]) << " groups not equal, i=" << i << std::endl;
    }

    LOG("clear groups");
    rt = setgroups(0, NULL);
    EXPECT_EQ(rt, 0);
    n = getgroups(2, gidListOut);
    EXPECT_EQ(n, 1);
    EXPECT_EQ(gidListOut[0], SHELL_GID);
    free(gidListIn);
    free(gidListOut);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetGroups_0200
 * @tc.name   setgroups function test: gid not in groups will automaticlly add by kernel
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetgroups2, Function | MediumTest | Level1)
{
    gid_t gidListIn[2] = {GetRandID(), GetRandID()};
    gid_t gidListOut[4] = {0};

    LOG("Init: make sure groups not changed by other test.");
    int n = getgroups(0, gidListOut);
    EXPECT_EQ(n, 1);
    int rt = getgroups(n, gidListOut);
    EXPECT_EQ(gidListOut[0], SHELL_GID);
    EXPECT_EQ(rt, n);

    LOG("add 2 groups");
    rt = setgroups(2, gidListIn);
    EXPECT_EQ(rt, 0);
    n = getgroups(4, gidListOut);
    EXPECT_EQ(n, 3);
    EXPECT_EQ(gidListOut[0], gidListIn[0]);
    EXPECT_EQ(gidListOut[1], gidListIn[1]);
    EXPECT_EQ(gidListOut[2], SHELL_GID);

    LOG("clear groups");
    rt = setgroups(0, NULL);
    EXPECT_EQ(rt, 0);
    n = getgroups(0, gidListOut);
    EXPECT_EQ(n, 1);
    rt = getgroups(n, gidListOut);
    EXPECT_EQ(rt, n);
    EXPECT_EQ(gidListOut[0], SHELL_GID);
}

/**
 * @tc.number SUB_KERNEL_DAC_SetGroups_0300
 * @tc.name   setgroups function test: input size is grater than 'MAX_PROCESS_GROUPS'
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(UidGidTest, testSetgroupsFail, Function | MediumTest | Level3)
{
    LOG("add 2 groups");
    gid_t gidListIn[2] = {GetRandID(), GetRandID()};
    gid_t gidListOut[4] = {0};
    
    int rt = setgroups(2, gidListIn);
    EXPECT_EQ(rt, 0);

    LOG("setgroups fail");
    rt = setgroups(MAX_PROCESS_GROUPS+1, gidListIn);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    LOG("check groups not changed");
    int n = getgroups(4, gidListOut);
    EXPECT_EQ(n, 3);
    EXPECT_EQ(gidListOut[0], gidListIn[0]);
    EXPECT_EQ(gidListOut[1], gidListIn[1]);
    EXPECT_EQ(gidListOut[2], SHELL_GID);

    LOG("clear groups");
    rt = setgroups(0, NULL);
    EXPECT_EQ(rt, 0);
    n = getgroups(2, gidListOut);
    EXPECT_EQ(n, 1);
    EXPECT_EQ(gidListOut[0], SHELL_GID);
}
