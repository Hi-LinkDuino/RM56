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
#include <cstddef>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gtest/gtest.h"
#include "ActsCapabilityTest.h"
#include "DACFileSystemTest.h"

using namespace std;
using namespace testing::ext;

#if defined(LITE_FS_JFFS2) || defined(LITE_FS_VFAT)
static int TestSetUid()
{
    // Test the 'setuid' interface
    int ret = 0;
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    // Step 1: Verify that UID is set to 0
    ret = setuid(UID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=0, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=0
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID0) << "Failed to set ruid=0: ruid=" << ruid;
    EXPECT_EQ(euid, UID0) << "Failed to set euid=0: euid=" << euid;
    EXPECT_EQ(suid, UID0) << "Failed to set suid=0: suid=" << suid;
    // Step 2: Verify that UID is set to 1000
    ret = setuid(UID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=1000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=1000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID1000) << "Failed to set ruid=1000: ruid=" << ruid;
    EXPECT_EQ(euid, UID1000) << "Failed to set euid=1000: euid=" << euid;
    EXPECT_EQ(suid, UID1000) << "Failed to set suid=1000: suid=" << suid;
    // Step 3: Verify that UID is set to 10000
    ret = setuid(UID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=10000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=10000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID10000) << "Failed to set ruid=10000: ruid=" << ruid;
    EXPECT_EQ(euid, UID10000) << "Failed to set euid=10000: euid=" << euid;
    EXPECT_EQ(suid, UID10000) << "Failed to set suid=10000: suid=" << suid;
    // Step 4: Verify that UID is set to 2147483647
    ret = setuid(MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=2147483647, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=2147483647
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, MAX_INT) << "Failed to set ruid=2147483647: ruid=" << ruid;
    EXPECT_EQ(euid, MAX_INT) << "Failed to set euid=2147483647: euid=" << euid;
    EXPECT_EQ(suid, MAX_INT) << "Failed to set suid=2147483647: suid=" << suid;
    return 0;
}

static int TestSetREUid()
{
    // Test the 'setreuid' interface
    int ret = 0;
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    // Step 1: Verify that UID is set to 0
    ret = setreuid(UID0, UID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=0, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=0
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID0) << "Failed to set ruid=0: ruid=" << ruid;
    EXPECT_EQ(euid, UID0) << "Failed to set euid=0: euid=" << euid;
    EXPECT_EQ(suid, UID0) << "Failed to set suid=0: suid=" << suid;
    // Step 2: Verify that UID is set to 1000
    ret = setreuid(UID1000, UID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=1000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=1000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID1000) << "Failed to set ruid=1000: ruid=" << ruid;
    EXPECT_EQ(euid, UID1000) << "Failed to set euid=1000: euid=" << euid;
    EXPECT_EQ(suid, UID1000) << "Failed to set suid=1000: suid=" << suid;
    // Step 3: Verify that UID is set to 10000
    ret = setreuid(UID10000, UID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=10000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=10000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID10000) << "Failed to set ruid=10000: ruid=" << ruid;
    EXPECT_EQ(euid, UID10000) << "Failed to set euid=10000: euid=" << euid;
    EXPECT_EQ(suid, UID10000) << "Failed to set suid=10000: suid=" << suid;
    // Step 4: Verify that UID is set to 2147483647
    ret = setreuid(MAX_INT, MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=2147483647, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=2147483647
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, MAX_INT) << "Failed to set ruid=2147483647: ruid=" << ruid;
    EXPECT_EQ(euid, MAX_INT) << "Failed to set euid=2147483647: euid=" << euid;
    EXPECT_EQ(suid, MAX_INT) << "Failed to set suid=2147483647: suid=" << suid;
    return 0;
}

static int TestSetRESUid()
{
    // Test the 'setresuid' interface
    int ret = 0;
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    // Step 1: Verify that UID is set to 0
    ret = setresuid(UID0, UID0, UID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=0, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=0
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID0) << "Failed to set ruid=0: ruid=" << ruid;
    EXPECT_EQ(euid, UID0) << "Failed to set euid=0: euid=" << euid;
    EXPECT_EQ(suid, UID0) << "Failed to set suid=0: suid=" << suid;
    // Step 2: Verify that UID is set to 1000
    ret = setresuid(UID1000, UID1000, UID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=1000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=1000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID1000) << "Failed to set ruid=1000: ruid=" << ruid;
    EXPECT_EQ(euid, UID1000) << "Failed to set euid=1000: euid=" << euid;
    EXPECT_EQ(suid, UID1000) << "Failed to set suid=1000: suid=" << suid;
    // Step 3: Verify that UID is set to 10000
    ret = setresuid(UID10000, UID10000, UID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=10000, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=10000
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, UID10000) << "Failed to set ruid=10000: ruid=" << ruid;
    EXPECT_EQ(euid, UID10000) << "Failed to set euid=10000: euid=" << euid;
    EXPECT_EQ(suid, UID10000) << "Failed to set suid=10000: suid=" << suid;
    // Step 4: Verify that UID is set to 2147483647
    ret = setresuid(MAX_INT, MAX_INT, MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set uid=2147483647, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the setuid interface to set the UID=2147483647
    getresuid(&ruid, &euid, &suid);
    EXPECT_EQ(ruid, MAX_INT) << "Failed to set ruid=2147483647: ruid=" << ruid;
    EXPECT_EQ(euid, MAX_INT) << "Failed to set euid=2147483647: euid=" << euid;
    EXPECT_EQ(suid, MAX_INT) << "Failed to set suid=2147483647: suid=" << suid;
    return 0;
}

static int TestSetUidAbnormal()
{
    // Enter the exception parameter when invoke the 'setuid','setreuid','setresuid' interface
    int ret = 0;
    uid_t newruid = 0;
    uid_t neweuid = 0;
    uid_t newsuid = 0;
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    // Obtain the ruid, euid, suid of the current process
    getresuid(&ruid, &euid, &suid);
    // Step 1: Verify that UID is set to -100 with the 'setuid' interface
    ret = setuid(ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set uid=-100, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the 'setuid' interface to set the UID=-100
    getresuid(&newruid, &neweuid, &newsuid);
    EXPECT_EQ(newruid, ruid) << "The value of ruid changes after an invalid parameter is entered: ruid=" << ruid;
    EXPECT_EQ(neweuid, euid) << "The value of euid changes after an invalid parameter is entered: euid=" << euid;
    EXPECT_EQ(newsuid, suid) << "The value of suid changes after an invalid parameter is entered: suid=" << suid;
    // Step 2: Verify that UID is set to -100 with the 'setreuid' interface
    ret = setreuid(ABNORMALINT, ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set uid=-100, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the 'setuid' interface to set the UID=-100
    getresuid(&newruid, &neweuid, &newsuid);
    EXPECT_EQ(newruid, ruid) << "The value of ruid changes after an invalid parameter is entered: ruid=" << ruid;
    EXPECT_EQ(neweuid, euid) << "The value of euid changes after an invalid parameter is entered: euid=" << euid;
    EXPECT_EQ(newsuid, suid) << "The value of suid changes after an invalid parameter is entered: suid=" << suid;
    // Step 3: Verify that UID is set to -100 with the 'setreuid' interface
    ret = setresuid(ABNORMALINT, ABNORMALINT, ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set uid=-100, now process uid=%d", getuid());
        return FALSE;
    }
    // To test the function of invoking the 'setuid' interface to set the UID=-100
    getresuid(&newruid, &neweuid, &newsuid);
    EXPECT_EQ(newruid, ruid) << "The value of ruid changes after an invalid parameter is entered: ruid=" << ruid;
    EXPECT_EQ(neweuid, euid) << "The value of euid changes after an invalid parameter is entered: euid=" << euid;
    EXPECT_EQ(newsuid, suid) << "The value of suid changes after an invalid parameter is entered: suid=" << suid;
    return 0;
}

static int TestSetGid()
{
    // Test the 'setgid' interface
    int ret = 0;
    gid_t rgid = 0;
    gid_t egid = 0;
    gid_t sgid = 0;
    // Step 1: Verify that GID is set to 0
    ret = setgid(GID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=0, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=0
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID0) << "Failed to set rgid=0: rgid=" << rgid;
    EXPECT_EQ(egid, GID0) << "Failed to set egid=0: egid=" << egid;
    EXPECT_EQ(sgid, GID0) << "Failed to set sgid=0: sgid=" << sgid;
    // Step 2: Verify that GID is set to 1000
    ret = setgid(GID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=1000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=1000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID1000) << "Failed to set rgid=1000: rgid=" << rgid;
    EXPECT_EQ(egid, GID1000) << "Failed to set egid=1000: egid=" << egid;
    EXPECT_EQ(sgid, GID1000) << "Failed to set sgid=1000: sgid=" << sgid;
    // Step 3: Verify that GID is set to 10000
    ret = setgid(GID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=10000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=10000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID10000) << "Failed to set rgid=10000: rgid=" << rgid;
    EXPECT_EQ(egid, GID10000) << "Failed to set egid=10000: egid=" << egid;
    EXPECT_EQ(sgid, GID10000) << "Failed to set sgid=10000: sgid=" << sgid;
    // Step 4: Verify that GID is set to 2147483647
    ret = setgid(MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=2147483647, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=2147483647
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, MAX_INT) << "Failed to set rgid=2147483647: rgid=" << rgid;
    EXPECT_EQ(egid, MAX_INT) << "Failed to set egid=2147483647: egid=" << egid;
    EXPECT_EQ(sgid, MAX_INT) << "Failed to set sgid=2147483647: sgid=" << sgid;
    return 0;
}

static int TestSetREGid()
{
    // Test the 'setregid' interface
    int ret = 0;
    gid_t rgid = 0;
    gid_t egid = 0;
    gid_t sgid = 0;
    // Step 1: Verify that GID is set to 0
    ret = setregid(GID0, GID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=0, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=0
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID0) << "Failed to set rgid=0: rgid=" << rgid;
    EXPECT_EQ(egid, GID0) << "Failed to set egid=0: egid=" << egid;
    EXPECT_EQ(sgid, GID0) << "Failed to set sgid=0: sgid=" << sgid;
    // Step 2: Verify that GID is set to 1000
    ret = setregid(GID1000, GID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=1000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=1000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID1000) << "Failed to set rgid=1000: rgid=" << rgid;
    EXPECT_EQ(egid, GID1000) << "Failed to set egid=1000: egid=" << egid;
    EXPECT_EQ(sgid, GID1000) << "Failed to set sgid=1000: sgid=" << sgid;
    // Step 3: Verify that GID is set to 10000
    ret = setregid(GID10000, GID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=10000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=10000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID10000) << "Failed to set rgid=10000: rgid=" << rgid;
    EXPECT_EQ(egid, GID10000) << "Failed to set egid=10000: egid=" << egid;
    EXPECT_EQ(sgid, GID10000) << "Failed to set sgid=10000: sgid=" << sgid;
    // Step 4: Verify that GID is set to 2147483647
    ret = setregid(MAX_INT, MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=2147483647, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=2147483647
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, MAX_INT) << "Failed to set rgid=2147483647: rgid=" << rgid;
    EXPECT_EQ(egid, MAX_INT) << "Failed to set egid=2147483647: egid=" << egid;
    EXPECT_EQ(sgid, MAX_INT) << "Failed to set sgid=2147483647: sgid=" << sgid;
    return 0;
}

static int TestSetRESGid()
{
    // Test the 'setresgid' interface
    int ret = 0;
    gid_t rgid = 0;
    gid_t egid = 0;
    gid_t sgid = 0;
    // Step 1: Verify that GID is set to 0
    ret = setresgid(GID0, GID0, GID0);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=0, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=0
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID0) << "Failed to set rgid=0: rgid=" << rgid;
    EXPECT_EQ(egid, GID0) << "Failed to set egid=0: egid=" << egid;
    EXPECT_EQ(sgid, GID0) << "Failed to set sgid=0: sgid=" << sgid;
    // Step 2: Verify that GID is set to 1000
    ret = setresgid(GID1000, GID1000, GID1000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=1000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=1000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID1000) << "Failed to set rgid=1000: rgid=" << rgid;
    EXPECT_EQ(egid, GID1000) << "Failed to set egid=1000: egid=" << egid;
    EXPECT_EQ(sgid, GID1000) << "Failed to set sgid=1000: sgid=" << sgid;
    // Step 3: Verify that GID is set to 10000
    ret = setresgid(GID10000, GID10000, GID10000);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=10000, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=10000
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, GID10000) << "Failed to set rgid=10000: rgid=" << rgid;
    EXPECT_EQ(egid, GID10000) << "Failed to set egid=10000: egid=" << egid;
    EXPECT_EQ(sgid, GID10000) << "Failed to set sgid=10000: sgid=" << sgid;
    // Step 4: Verify that GID is set to 2147483647
    ret = setresgid(MAX_INT, MAX_INT, MAX_INT);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set gid=2147483647, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the setgid interface to set the GID=2147483647
    getresgid(&rgid, &egid, &sgid);
    EXPECT_EQ(rgid, MAX_INT) << "Failed to set rgid=2147483647: rgid=" << rgid;
    EXPECT_EQ(egid, MAX_INT) << "Failed to set egid=2147483647: egid=" << egid;
    EXPECT_EQ(sgid, MAX_INT) << "Failed to set sgid=2147483647: sgid=" << sgid;
    return 0;
}

static int TestSetGidAbnormal()
{
    // Enter the exception parameter when invoke the 'setgid','setregid','setresgid' interface
    int ret = 0;
    gid_t newrgid = 0;
    gid_t newegid = 0;
    gid_t newsgid = 0;
    gid_t rgid = 0;
    gid_t egid = 0;
    gid_t sgid = 0;
    // Obtain the rgid, egid, sgid of the current process
    getresgid(&rgid, &egid, &sgid);
    // Step 1: Verify that GID is set to -100 with the 'setgid' interface
    ret = setgid(ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set gid=-100, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the 'setgid' interface to set the GID=-100
    getresgid(&newrgid, &newegid, &newsgid);
    EXPECT_EQ(newrgid, rgid) << "The value of rgid changes after an invalid parameter is entered: rgid=" << rgid;
    EXPECT_EQ(newegid, egid) << "The value of egid changes after an invalid parameter is entered: egid=" << egid;
    EXPECT_EQ(newsgid, sgid) << "The value of sgid changes after an invalid parameter is entered: sgid=" << sgid;
    // Step 2: Verify that GID is set to -100 with the 'setregid' interface
    ret = setregid(ABNORMALINT, ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set gid=-100, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the 'setgid' interface to set the GID=-100
    getresgid(&newrgid, &newegid, &newsgid);
    EXPECT_EQ(newrgid, rgid) << "The value of rgid changes after an invalid parameter is entered: rgid=" << rgid;
    EXPECT_EQ(newegid, egid) << "The value of egid changes after an invalid parameter is entered: egid=" << egid;
    EXPECT_EQ(newsgid, sgid) << "The value of sgid changes after an invalid parameter is entered: sgid=" << sgid;
    // Step 3: Verify that GID is set to -100 with the 'setregid' interface
    ret = setresgid(ABNORMALINT, ABNORMALINT, ABNORMALINT);
    if (ret != FALSE) {
        EXPECT_EQ(ret, FALSE);
        LOG("ErrInfo: Set gid=-100, now process gid=%d", getgid());
        return FALSE;
    }
    // To test the function of invoking the 'setgid' interface to set the GID=-100
    getresgid(&newrgid, &newegid, &newsgid);
    EXPECT_EQ(newrgid, rgid) << "The value of rgid changes after an invalid parameter is entered: rgid=" << rgid;
    EXPECT_EQ(newegid, egid) << "The value of egid changes after an invalid parameter is entered: egid=" << egid;
    EXPECT_EQ(newsgid, sgid) << "The value of sgid changes after an invalid parameter is entered: sgid=" << sgid;
    return 0;
}

static int TestSetGroups()
{
    // Test the 'setgroups' interface
    int ret;
    gid_t list[SIZE255];
    for (size_t num = 0; num < SIZE253; num++) {
        list[num] = num;
    }
    list[SIZE254] = MAX_INT;
    ret = setgroups(SIZE255, list);
    if (ret != 0) {
        EXPECT_EQ(ret, 0);
        LOG("ErrInfo: Failed to set groups");
        return FALSE;
    }
    return 0;
}

static void TsetFork(uid_t uid, gid_t gid, size_t size, const gid_t list[])
{
    // The sub process inherits the UID, GID, and groups of the parent process
    int ret;
    int status = 0;
    gid_t reallist[SIZE255];
    // Preset action: Adjust the UID, GID, and groups of the parent process
    ret = setuid(uid);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set uid, now process uid=" << getuid();
    ret = setgid(gid);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set gid, now process gid=" << getgid();
    setgroups(0, nullptr);
    ret = setgroups(size, list);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set groups";
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Inheritance test of sub process UID
        uid_t retuid = getuid();
        if (retuid != uid) {
            LOG("ErrInfo: The sub process UID changes when fork process, now process uid=%d", getuid());
            exitCode = 1;
        }
        // Step 2: Inheritance test of sub process GID
        gid_t retgid = getgid();
        if (retgid != gid) {
            LOG("ErrInfo: The sub process GID changes when fork process, now process gid=%d", getgid());
            exitCode = 1;
        }
        // Step 3: Inheritance test of sub process groups
        int retgroups = getgroups(0, reallist);
        if (retgroups == FALSE) {
            LOG("ErrInfo: Failed to get groups");
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

static int ChangeSensitiveInformation()
{
    int fd = 0;
    int ret = 0;
    int exitCode = 0;
    char dac[] = "DACPreTest!\n";
    // Failed to read sensitive information
    fd = open("/proc/process", O_WRONLY);
    if (fd >= 0) {
        ret = write(fd, dac, sizeof(dac));
        if (ret != FALSE) {
            LOG("ErrInfo: Change sensitive information, ret = %d", ret);
            exitCode = 1;
        }
        close(fd);
    }
    return exitCode;
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0010
 * @tc.name       : Invoke the interface to set the process UID
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0010, Function | MediumTest | Level1)
{
    int ret = 0;
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    uid_t newruid = 0;
    uid_t neweuid = 0;
    uid_t newsuid = 0;
    // Preset action: Obtain the ruid, euid, suid of the current process
    getresuid(&ruid, &euid, &suid);
    // Step 1: Test the 'setuid' interface
    ret = TestSetUid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetUid() exit error";
    // Step 2: Test the 'setreuid' interface
    ret = TestSetREUid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetREUid() exit error";
    // Step 3: Test the 'setreuid' interface
    ret = TestSetRESUid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetRESUid() exit error";
    // Step 4: Enter the exception parameter when invoke the 'setuid','setreuid','setresuid' interface
    ret = TestSetUidAbnormal();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetUidAbnormal() exit error";
    // Cleanup action: Restore the initial UID of the process
    ret = setresuid(ruid, euid, suid);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial UID of the process";
    getresuid(&newruid, &neweuid, &newsuid);
    EXPECT_EQ(newruid, ruid) << "The value of ruid changes after testcase: ruid=" << ruid;
    EXPECT_EQ(neweuid, euid) << "The value of euid changes after testcase: euid=" << euid;
    EXPECT_EQ(newsuid, suid) << "The value of suid changes after testcase: suid=" << suid;
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0020
 * @tc.name       : Invoke the interface to set the process GID
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0020, Function | MediumTest | Level1)
{
    int ret = 0;
    gid_t rgid = 0;
    gid_t egid = 0;
    gid_t sgid = 0;
    gid_t newrgid = 0;
    gid_t newegid = 0;
    gid_t newsgid = 0;
    // Preset action: Obtain the rgid, egid, sgid of the current process
    getresgid(&rgid, &egid, &sgid);
    // Step 1: Test the 'setgid' interface
    ret = TestSetGid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetGid() exit error";
    // Step 2: Test the 'setregid' interface
    ret = TestSetREGid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetREGid() exit error";
    // Step 3: Test the 'setregid' interface
    ret = TestSetRESGid();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetRESGid() exit error";
    // Step 4: Enter the exception parameter when invoke the 'setgid','setregid','setresgid' interface
    ret = TestSetGidAbnormal();
    EXPECT_EQ(ret, 0) << "ErrInfo: TestSetGidAbnormal() exit error";
    // Cleanup action: Restore the initial GID of the process
    ret = setresgid(rgid, egid, sgid);
    EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial GID of the process";
    getresgid(&newrgid, &newegid, &newsgid);
    EXPECT_EQ(newrgid, rgid) << "The value of rgid changes after testcase: rgid=" << rgid;
    EXPECT_EQ(newegid, egid) << "The value of egid changes after testcase: egid=" << egid;
    EXPECT_EQ(newsgid, sgid) << "The value of sgid changes after testcase: sgid=" << sgid;
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0030
 * @tc.name       : Invoke the setgroups interface to set the process groups that contain a single GID or an empty value
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0030, Function | MediumTest | Level1)
{
    int ret;
    gid_t grouplist[SIZE255];
    // Preset action: Obtain the groups of the current process
    unsigned int groupsize = getgroups(0, grouplist);
    if (groupsize >= 0) {
        getgroups(groupsize, grouplist);
        // Preset action: Obtain the group lists required for the testcase
        gid_t list1[SIZE1] = {1};
        gid_t list2[SIZE1] = {MAX_INT};
        gid_t list3[SIZE1] = {};
        // Step 1: Set the group list to {1}
        ret = setgroups(SIZE1, list1);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {1}";
        // Step 2: Set the group list to {2147483647}
        ret = setgroups(SIZE1, list2);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {2147483647}";
        // Step 3: Set the group list to {}
        ret = setgroups(SIZE1, list3);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {}";
        // Cleanup action: Restore the initial groups of the process
        setgroups(0, nullptr);
        ret = setgroups(groupsize, grouplist);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial groups of the process";
    } else {
        EXPECT_GE(groupsize, 0) << "ErrInfo: Failed to get groups";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0040
 * @tc.name       : Invoke the setgroups interface to set the process groups that contain the same GID
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0040, Function | MediumTest | Level2)
{
    int ret;
    gid_t grouplist[SIZE255];
    // Preset action: Obtain the groups of the current process
    unsigned int groupsize = getgroups(0, grouplist);
    if (groupsize >= 0) {
        getgroups(groupsize, grouplist);
        // Preset action: Obtain the group lists required for the testcase
        gid_t list1[SIZE2]={GID0, GID0};
        gid_t list2[SIZE2]={GID1, GID1};
        gid_t list3[SIZE2]={MAX_INT, MAX_INT};
        // Step 1: Set the group list to {0, 0}
        ret = setgroups(SIZE2, list1);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {0, 0}";
        // Step 2: Set the group list to {1, 1}
        ret = setgroups(SIZE2, list2);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {1, 1}";
        // Step 3: Set the group list to {2147483647, 2147483647}
        ret = setgroups(SIZE2, list3);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {2147483647, 2147483647}";
        // Cleanup action: Restore the initial groups of the process
        setgroups(0, nullptr);
        ret = setgroups(groupsize, grouplist);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial groups of the process";
    } else {
        EXPECT_GE(groupsize, 0) << "ErrInfo: Failed to get groups";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0050
 * @tc.name       : Invoke the setgroups interface to set the process groups that contain the duplicate GIDs
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0050, Function | MediumTest | Level3)
{
    int ret;
    gid_t grouplist[SIZE255];
    // Preset action: Obtain the groups of the current process
    unsigned int groupsize = getgroups(0, grouplist);
    if (groupsize >= 0) {
        getgroups(groupsize, grouplist);
        // Preset action: Obtain the group lists required for the testcase
        gid_t list1[SIZE3]={GID0, GID0, MAX_INT};
        gid_t list2[SIZE3]={GID10000, GID10000, MAX_INT};
        gid_t list3[SIZE3]={GID0, MAX_INT, MAX_INT};
        // Step 1: Set the group list to {0, 0, 2147483647}
        ret = setgroups(SIZE3, list1);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {0, 0, 2147483647}";
        // Step 2: Set the group list to {10000, 10000, 2147483647}
        ret = setgroups(SIZE3, list2);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {10000, 10000, 2147483647}";
        // Step 3: Set the group list to {0, 2147483647, 2147483647}
        ret = setgroups(SIZE3, list3);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {0, 2147483647, 2147483647}";
        // Cleanup action: Restore the initial groups of the process
        setgroups(0, nullptr);
        ret = setgroups(groupsize, grouplist);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial groups of the process";
    } else {
        EXPECT_GE(groupsize, 0) << "ErrInfo: Failed to get groups";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0060
 * @tc.name       : Invoke the setgroups interface to set the process groups that contain all different GIDs
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0060, Function | MediumTest | Level3)
{
    int ret;
    gid_t grouplist[SIZE255];
    // Preset action: Obtain the groups of the current process
    size_t groupsize = getgroups(0, grouplist);
    if (groupsize >= 0) {
        getgroups(groupsize, grouplist);
        // Preset action: Obtain the group lists required for the testcase
        gid_t list0[SIZE255];
        for (size_t num0 = 0; num0 < SIZE254; num0++) {
            list0[num0] = num0;
        }
        list0[SIZE254] = MAX_INT;
        gid_t list1[INVAILD_SIZE];
        for (size_t num1 = 0; num1 < MAX_SIZE; num1++) {
            list1[num1] = num1;
        }
        list1[MAX_SIZE] = MAX_INT;
        // Step 1: Set 255 different group lists
        ret = setgroups(SIZE255, list0);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to set the group list to {0, 1, 2, ..., 255}";
        // Step 2: Set the number of groups that exceed the upper limit
        ret = setgroups(INVAILD_SIZE, list1);
        EXPECT_EQ(ret, FALSE) << "ErrInfo: Set groups size over max, size=65537";
        // Cleanup action: Restore the initial groups of the process
        setgroups(0, nullptr);
        ret = setgroups(groupsize, grouplist);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial groups of the process";
    } else {
        EXPECT_GE(groupsize, 0) << "ErrInfo: Failed to get groups";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0070
 * @tc.name       : Invoke the setuid, gid, and groups interfaces to set the uid, gid,
                    and groups of processes concurrently
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0070, Security | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Fork three sub processes
    pid_t pid;
    for (int num = 0; num < NUM3; num++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
        usleep(SLEEP_NUM);
        if (pid == 0) {
            break;
        }
    }
    // get one parent & three children
    if (pid == 0) {
        int exitCode = 0;
        for (int number = 0; number < NUM3000; number++) {
            // Preset action: Initialize the subprocess UID, GID and groups
            setuid(0);
            setgid(0);
            setgroups(0, nullptr);
            // Step 1: Test the 'setuid' interface concurrently
            ret = TestSetUid();
            if (ret != 0) {
                LOG("ErrInfo: TestSetUid error during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 2: Test the 'setgid' interface concurrently
            ret = TestSetGid();
            if (ret != 0) {
                LOG("ErrInfo: TestSetGid error during the %d time", number);
                exitCode = 1;
                break;
            }
            // Step 2: Test the 'setgroups' interface concurrently
            ret = TestSetGroups();
            if (ret != 0) {
                LOG("ErrInfo: TestSetGroups error during the %d time", number);
                exitCode = 1;
                break;
            }
        }
        // Step 3: Three sub processes exit with the exitCode
        exit(exitCode);
    } else {
        // Step 4: The parent process wait for three sub processes to exit and obtain the exitCode
        for (int num2 = 0; num2 < NUM3; num2++) {
            wait(&status);
            EXPECT_NE(WIFEXITED(status), 0) << "ErrInfo: The sub process exit error, child_pid = " << pid;
            EXPECT_EQ(WEXITSTATUS(status), 0) << "ErrInfo: Pid = "<< pid
            << ", its exitCode is wrong and test case failed, please query logs";
        }
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0080
 * @tc.name       : Inheritance of process UID, GID and groups
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0080, Function | MediumTest | Level2)
{
    int ret;
    uid_t ruid = 0; uid_t euid = 0; uid_t suid = 0;
    uid_t newruid = 0; uid_t neweuid = 0; uid_t newsuid = 0;
    gid_t rgid = 0; gid_t egid = 0; gid_t sgid = 0;
    gid_t newrgid = 0; gid_t newegid = 0; gid_t newsgid = 0;
    gid_t grouplist[SIZE255];
    // Preset action: Obtain the ruid, euid, suid of the current process
    getresuid(&ruid, &euid, &suid);
    // Preset action: Obtain the rgid, egid, sgid of the current process
    getresgid(&rgid, &egid, &sgid);
    // Preset action: Obtain the groups of the current process
    int groupsize = getgroups(0, grouplist);
    if (groupsize >= 0) {
        getgroups(groupsize, grouplist);
        // Preset action: Obtain the group lists required for the testcase
        gid_t list1[SIZE1] = {GID10000};
        gid_t list2[SIZE1] = {};
        gid_t list3[SIZE255];
        for (size_t num = 0; num < SIZE254; num++) {
            list3[num] = num;
        }
        list3[SIZE254] = MAX_INT;
        // Step 1: Factor combination test of UID, GID, and groups
        TsetFork(UID0, GID10000, SIZE1, list1);
        TsetFork(UID10000, GID10000, SIZE1, list1);
        TsetFork(MAX_INT, GID10000, SIZE1, list1);
        TsetFork(UID10000, GID0, SIZE1, list1);
        TsetFork(UID10000, MAX_INT, SIZE1, list1);
        TsetFork(UID10000, GID10000, SIZE1, list2);
        TsetFork(UID10000, GID10000, SIZE255, list3);
        // Cleanup action: Restore the initial UID of the process
        ret = setresuid(ruid, euid, suid);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial UID of the process";
        getresuid(&newruid, &neweuid, &newsuid);
        EXPECT_EQ(newruid, ruid) << "The value of ruid changes after testcase: ruid=" << ruid;
        EXPECT_EQ(neweuid, euid) << "The value of euid changes after testcase: euid=" << euid;
        EXPECT_EQ(newsuid, suid) << "The value of suid changes after testcase: suid=" << suid;
        // Cleanup action: Restore the initial GID of the process
        ret = setresgid(rgid, egid, sgid);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial GID of the process";
        getresgid(&newrgid, &newegid, &newsgid);
        EXPECT_EQ(newrgid, rgid) << "The value of rgid changes after testcase: rgid=" << rgid;
        EXPECT_EQ(newegid, egid) << "The value of egid changes after testcase: egid=" << egid;
        EXPECT_EQ(newsgid, sgid) << "The value of sgid changes after testcase: sgid=" << sgid;
        // Cleanup action: Restore the initial groups of the process
        setgroups(0, nullptr);
        ret = setgroups(groupsize, grouplist);
        EXPECT_EQ(ret, 0) << "ErrInfo: Failed to restore the initial groups of the process";
    } else {
        EXPECT_GE(groupsize, 0) << "ErrInfo: Failed to get groups";
    }
}

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0120
 * @tc.name       : Failed to use the third-party app UID to change sensitive information
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0120, Security | MediumTest | Level2)
{
    int ret;
    int status = 0;
    // Preset action: Fork a sub process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    usleep(SLEEP_NUM);
    if (pid == 0) {
        int exitCode = 0;
        // Step 1: Set the process uid and gid to the third-party application uid and gid
        ret = SetUidGid(UID20000, GID20000);
        if (ret != 0) {
            LOG("ErrInfo: Failed to set the process uid and gid");
            exitCode = 1;
        }
        // Step 2: Drop all the sub process capabilities
        ret = DropAllCAP();
        if (ret != 0) {
            LOG("ErrInfo: Failed to drop all the sub process capabilities");
            exitCode = 1;
        }
        // Step 3: Failed to change sensitive information
        ret = ChangeSensitiveInformation();
        if (ret != 0) {
            LOG("ErrInfo: change sensitive information");
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

/*
 * @tc.number     : SUB_SEC_AppSEC_PermissionMgmt_DAC_0130
 * @tc.name       : Performance test of the setuid, setgid and setgroups interface
 * @tc.desc       : [C- SECURITY -0200]
 */
HWTEST_F(DacTestSuite, DACTest0130, Performance | MediumTest | Level2)
{
    struct timespec tp = { 0 };
    struct timespec starttime = { 0 };
    struct timespec endtime = { 0 };
    tp.tv_sec = 0;
    tp.tv_nsec = 0;
    // Preset action: Obtain the group lists required for the testcase
    gid_t list[SIZE255];
    for (size_t num = 0; num < SIZE253; num++) {
        list[num] = num;
    }
    // Preset action: Obtains the system time -> starttime
    clock_gettime(CLOCK_REALTIME, &starttime);
    for (int number = 0; number < NUM10000; number++) {
        list[SIZE254] = number;
        // Step 1.1: Setuid for 10000 times
        setuid(number);
        // Step 1.2: Setgid for 10000 times
        setgid(number);
        // Step 1.3: Setgroups for 10000 times
        setgroups(SIZE255, list);
    }
    // Step 2: Obtains the system time again -> endtime
    clock_gettime(CLOCK_REALTIME, &endtime);
    // Step 3: Compare the starttime and the endtime -> tp
    tp = CompareTime(starttime, endtime);
    EXPECT_LE(tp.tv_sec, NUM20) << "ErrInfo: Chown for 10000 times used " << tp.tv_sec << "." << tp.tv_nsec << "s";
    // Cleanup action: Restore the uid, gid and groups of the process to zero
    SetUidGid(UID0, GID0);
    setgroups(0, nullptr);
}
#endif
