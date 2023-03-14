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

#include <time.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "ClockID.h"

using namespace testing::ext;

const char* DATEMSK_FILE = "/storage/getdate_mask";

// Resolution of: CLOCK_MONOTONIC, CLOCK_REALTIME, CLOCK_MONOTONIC_RAW: 1 us
const int CLOCK_RESOLUTION_HIGH = 1000;
// Resolution of: CLOCK_MONOTONIC_COARSE, CLOCK_REALTIME_COARSE: 1 tick = 10 ms
const int CLOCK_RESOLUTION_LOW = 1000 * 1000;

class ClockTimeTest : public testing::Test {
protected:
    static struct timespec mTestStatTime;
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        clock_gettime(CLOCK_REALTIME, &mTestStatTime);
        LOG("test start at %ld\n", (long)mTestStatTime.tv_sec);
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
        struct timespec time1 = {0, 0};
        clock_gettime(CLOCK_REALTIME, &time1);
        LOG("test end at %ld", (long)time1.tv_sec);

        mTestStatTime.tv_sec += 10; // approximate total test time
        if (!clock_settime(CLOCK_REALTIME, &mTestStatTime)) {
            LOG("restore time ok\n");
        } else {
            LOG("restore time failed\n");
        }
    }
};
struct timespec ClockTimeTest::mTestStatTime;

/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_GETRES_0100
 * @tc.name   test all supported clockid of clock_getres
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(SupportedClockIDTest, testClockGetresAll, Function | MediumTest | Level1)
{
    clockid_t cid = GetParam();
    const char* cname = ALL_CLOCKS_NAME[cid];

    struct timespec time1 = {0, 0};
    int rt = clock_getres(cid, &time1);
    LOG("%s Resolution: %ld nanosecond\n", cname, time1.tv_nsec);
    EXPECT_EQ(rt, 0) << "clock_getres of " << cname << "failed, errno =" <<errno;

    EXPECT_EQ(time1.tv_sec, 0);
    if (cid == CLOCK_MONOTONIC || cid == CLOCK_REALTIME || cid == CLOCK_MONOTONIC_RAW) {
        EXPECT_EQ(time1.tv_nsec, CLOCK_RESOLUTION_HIGH) << "Resolution check failed";
    } else {
        EXPECT_EQ(time1.tv_nsec, CLOCK_RESOLUTION_LOW) << "Resolution check failed";
    }
}

/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_GETTIME_0100
 * @tc.name   test all supported clockid of clock_gettime
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_P(SupportedClockIDTest, testClockGettimeAll, Function | MediumTest | Level1)
{
    clockid_t cid = GetParam();
    const char* cname = ALL_CLOCKS_NAME[cid];

    struct timespec time1 = {0, 0};
    int rt = clock_gettime(cid, &time1);
    if (rt == 0) {
        LOG("clock_gettime(%s) : tv_sec=%ld, tv_nsec=%ld\n", cname, time1.tv_sec, time1.tv_nsec);
    } else {
        LOG("%s return error, rt=%d, errno=%d:%s\n", cname, rt, errno, strerror(errno));
    }
    EXPECT_EQ(rt, 0);
}

INSTANTIATE_TEST_CASE_P(ClockTimeTest, SupportedClockIDTest, ALL_SUPPORTED_IDS);


/**
 * @tc.number SUB_KERNEL_TIME_API_CLOCK_SETTIME_0100
 * @tc.name   test clock_settime basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(ClockTimeTest, testClockSettime, Function | MediumTest | Level1)
{
    struct timespec time1 = {0, 0};
    int rt = clock_gettime(CLOCK_REALTIME, &time1);
    ASSERT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    LOG("current time: sec=%llu, nsec=%ld", time1.tv_sec, time1.tv_nsec);
    time_t sec = time1.tv_sec;
    time1.tv_sec -= 1;
    time1.tv_nsec = 1;
    rt = clock_settime(CLOCK_REALTIME, &time1);
    ASSERT_EQ(rt, 0) << "clock_settime failed, errno=" << errno;
    sleep(1);
    rt = clock_gettime(CLOCK_REALTIME, &time1);
    ASSERT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    ASSERT_EQ(sec, time1.tv_sec);
}

/**
 * @tc.number SUB_KERNEL_TIME_API_FTIME_0100
 * @tc.name   test ftime basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(ClockTimeTest, testFtime, Function | MediumTest | Level1)
{
    Msleep(10); // hopefully to let the flowing code not scheduled-out
    struct timeb tb = {0};
    struct timespec ts = {0};
    int rt = clock_gettime(CLOCK_REALTIME, &ts);
    ASSERT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    ts.tv_sec -= 1;
    ts.tv_nsec = 1000000;
    rt = clock_settime(CLOCK_REALTIME, &ts);
    ASSERT_EQ(rt, 0) << "clock_settime failed, errno=" << errno;
    rt = clock_gettime(CLOCK_REALTIME, &ts);
    int rt2 = ftime(&tb);
    EXPECT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    EXPECT_EQ(rt2, 0) << "ftime failed, errno=" << errno;
    LOG("current time: sec=%llu, nsec=%ld", ts.tv_sec, (long)ts.tv_nsec);
    LOG("current time: sec=%llu, millitm=%d", tb.time, (int)tb.millitm);
    EXPECT_EQ(ts.tv_sec, tb.time);
    EXPECT_NEAR((int)ts.tv_nsec/1000000, (int)tb.millitm, 1);
}

/**
 * @tc.number SUB_KERNEL_TIME_API_STIME_0100
 * @tc.name   test stime basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(ClockTimeTest, testStime, Function | MediumTest | Level1)
{
    Msleep(10); // hopefully to let the flowing code not scheduled-out
    struct timespec ts = {0};
    int rt = clock_gettime(CLOCK_REALTIME, &ts);
    LOG("current time: sec=%llu, nsec=%ld", ts.tv_sec, (long)ts.tv_nsec);
    ASSERT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    time_t t = ts.tv_sec + 1;
    rt = stime(&t);
    ASSERT_EQ(rt, 0) << "stime failed, errno=" << errno;
    Msleep(10);
    rt = clock_gettime(CLOCK_REALTIME, &ts);
    EXPECT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    LOG("current time: sec=%llu, nsec=%ld", ts.tv_sec, (long)ts.tv_nsec);
    EXPECT_EQ(ts.tv_sec, t);
}

/**
 * @tc.number SUB_KERNEL_TIME_API_TIME_0100
 * @tc.name   test time basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(ClockTimeTest, testTime, Function | MediumTest | Level1)
{
    Msleep(10); // hopefully to let the flowing code not scheduled-out
    struct timespec ts = {0};
    int rt = clock_gettime(CLOCK_REALTIME, &ts);
    ASSERT_EQ(rt, 0) << "clock_gettime failed, errno=" << errno;
    rt = stime(&ts.tv_sec); // set ts.tv_nsec to 0
    ASSERT_EQ(rt, 0) << "stime failed, errno=" << errno;

    time_t t1, t2;
    t1 = time(&t2);
    EXPECT_EQ(t1, ts.tv_sec) << "time failed";
    EXPECT_EQ(t1, t2) << "time failed";

    t2 = 1;
    rt = stime(&t2);
    EXPECT_EQ(rt, 0) << "stime failed, errno=" << errno;

    t1 = time(&t2);
    EXPECT_EQ(t1, 1);
    EXPECT_EQ(t1, t2);

    t1 = time(nullptr);
    EXPECT_EQ(t1, 1);

    rt = stime(&ts.tv_sec); // restore time
    ASSERT_EQ(rt, 0) << "stime failed, errno=" << errno;
}

/**
 * @tc.number SUB_KERNEL_TIME_API_TIMES_0100
 * @tc.name   test times basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(ClockTimeTest, testTimes, Function | MediumTest | Level1)
{
    // this test should run in a child process
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int exitCode = 0;
        const int testClockt = 100;
        const int msPerClock = 10;
        struct tms start = {0};
        struct tms end = {0};
        clock_t stTime = times(&start);
        LOG("start_clock: stTime: %ld", stTime);
        LOG("start_clock: tms_utime: %ld, tms_stime: %ld, tms_cutime:%ld, tms_cstime:%ld",
            start.tms_utime, start.tms_stime, start.tms_cutime, start.tms_cstime);

        KeepRun(testClockt * msPerClock);

        clock_t endTime = times(&end);
        LOG("end_clock: endTime: %ld", endTime);
        LOG("end_clock: tms_utime: %ld, tms_stime: %ld, tms_cutime:%ld, tms_cstime:%ld",
            end.tms_utime, end.tms_stime, end.tms_cutime, end.tms_cstime);

        LOG("Real Time: %ld, User Time %ld, System Time %ld\n", (long)(endTime - stTime),
            (long)(end.tms_utime - start.tms_utime),
            (long)(end.tms_stime - start.tms_stime));

        if (start.tms_utime != 0 || start.tms_stime != 0 || start.tms_cutime != 0 || start.tms_cstime != 0) {
            LOG("init value check failed");
        }
        if (!CheckValueClose(end.tms_utime, testClockt, 0.02)) {
            LOG("tms_utime value check failed");
        }
        if (!CheckValueClose((endTime - stTime), testClockt, 0.02)) {
            LOG("Real Time value check failed");
        }
        exit(exitCode);
    } else { // parent
        WaitProcExitedOK(pid);
    }
}

/**
* @tc.number     SUB_KERNEL_TIME_API_GETTIMEOFDAY_0100
* @tc.name       test gettimeofday api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testGettimeofday, Function | MediumTest | Level1) {
    int sleepSec = 1;
    struct timeval tvalStart = {0};
    struct timeval tvalEnd = {0};
    struct timezone tzone;

    int ret1 = gettimeofday(&tvalStart, &tzone);
    sleep(sleepSec);
    int ret2 = gettimeofday(&tvalEnd, &tzone);
    EXPECT_EQ(0, ret1);
    EXPECT_EQ(0, ret2);
    EXPECT_TRUE((tvalEnd.tv_sec - tvalStart.tv_sec) >= sleepSec)
        << "check end-start>=1 fail, start[" << tvalStart.tv_sec << "],end[" << tvalEnd.tv_sec << "]";
    EXPECT_TRUE((tvalEnd.tv_sec - tvalStart.tv_sec) < sleepSec+1)
        << "check end-start<2 fail, start[" << tvalStart.tv_sec << "],end[" << tvalEnd.tv_sec << "]";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_SETTIMEOFDAY_0100
* @tc.name       test settimeofday api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testSettimeofday, Function | MediumTest | Level1) {
    int setSec = 100;
    int sleepSec = 2;
    struct timeval tvalStart = {0};
    struct timeval tvalEnd = {0};
    struct timeval set = {.tv_sec = setSec, .tv_usec = 0};

    int ret1 = settimeofday(&set, NULL);
    int ret2 = gettimeofday(&tvalStart, NULL);
    sleep(sleepSec);
    int ret3 = gettimeofday(&tvalEnd, NULL);
    EXPECT_EQ(0, ret1);
    EXPECT_EQ(0, ret2);
    EXPECT_EQ(0, ret3);
    EXPECT_EQ(setSec, tvalStart.tv_sec)
        << "settimeofday set[" << setSec << "],get[" << tvalStart.tv_sec << "]";
    EXPECT_TRUE((tvalEnd.tv_sec - tvalStart.tv_sec) >= sleepSec)
        << "check end-start>=2 fail, start[" << tvalStart.tv_sec << "],end[" << tvalEnd.tv_sec << "]";
    EXPECT_TRUE((tvalEnd.tv_sec - tvalStart.tv_sec) < sleepSec+1)
        << "check end-start<3 fail, start[" << tvalStart.tv_sec << "],end[" << tvalEnd.tv_sec << "]";
}

/**
* @tc.number     SUB_KERNEL_TIME_API_LOCALTIME_0100
* @tc.name       test localtime api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testLocaltime, Function | MediumTest | Level1) {
    char cTime[32];
    time_t tStart;
    time_t tEnd;
    struct timeval tSet = {.tv_sec = 86399, .tv_usec = 0};

    int ret = settimeofday(&tSet, NULL);
    time(&tStart);
    sleep(2);
    time(&tEnd);
    EXPECT_EQ(0, ret);

    struct tm *tmStart = localtime(&tStart);
    ASSERT_NE(nullptr, tmStart);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmStart);
    EXPECT_STREQ("23:59:59", cTime);
    struct tm *tmEnd = localtime(&tEnd);
    ASSERT_NE(nullptr, tmEnd);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmEnd);
    EXPECT_STREQ("00:00:01", cTime);
}

/**
* @tc.number     SUB_KERNEL_TIME_API_LOCALTIMER_0100
* @tc.name       test localtime_r api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testLocaltimer, Function | MediumTest | Level1) {
    char cTime[32];
    time_t tStart;
    time_t tEnd;
    struct tm tmrStart = {0};
    struct tm tmrEnd = {0};

    struct timeval tSet = {.tv_sec = 86399, .tv_usec = 0};
    int ret = settimeofday(&tSet, NULL);
    time(&tStart);
    sleep(1);
    time(&tEnd);
    struct tm *tmrStartPtr = localtime_r(&tStart, &tmrStart);
    ASSERT_NE(nullptr, tmrStartPtr);
    struct tm *tmrEndPtr = localtime_r(&tEnd, &tmrEnd);
    ASSERT_NE(nullptr, tmrEndPtr);

    EXPECT_EQ(0, ret);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", &tmrStart);
    EXPECT_STREQ("23:59:59", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmrStartPtr);
    EXPECT_STREQ("23:59:59", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", &tmrEnd);
    EXPECT_STREQ("00:00:00", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmrEndPtr);
    EXPECT_STREQ("00:00:00", cTime);
    strftime(cTime, sizeof(cTime), "%F %T", &tmrStart);
    LOG("   result[%s]", cTime);
}

/**
* @tc.number     SUB_KERNEL_TIME_API_GETDATE_0100
* @tc.name       test getdate api
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testGetdateBasic, Function | MediumTest | Level1) {
    // set DATEMSK env
    FILE *fp = nullptr;
    char mask[20] = "%Y-%m-%d %H:%M:%S";
    fp = fopen(DATEMSK_FILE, "w+");
    ASSERT_NE(nullptr, fp);
    int ret = fwrite(mask, sizeof(mask), 1, fp);
    EXPECT_TRUE(ret > 0);
    ret = setenv("DATEMSK", DATEMSK_FILE, 1);
    EXPECT_EQ(0, ret);
    ret = fclose(fp);
    EXPECT_NE(-1, ret);

    // test getdate
    char cTime[30];
    struct tm *retTm = nullptr;
    const char *cInput = "2020-10-26 00:01:01";
    retTm = getdate(cInput);
    ASSERT_NE(nullptr, retTm) << "   getdate fail errno:" << getdate_err;
    strftime(cTime, sizeof(cTime), mask, retTm);
    EXPECT_STREQ(cInput, cTime);
    strftime(cTime, sizeof(cTime), "%D %A %H:%M:%S", retTm);
    EXPECT_STREQ("10/26/20 Sunday 00:01:01", cTime);

    // restore
    ret = remove(DATEMSK_FILE);
    EXPECT_EQ(0, ret);
    ret = unsetenv("DATEMSK");
    EXPECT_EQ(0, ret);
}

/**
* @tc.number     SUB_KERNEL_TIME_API_GETDATE_0200
* @tc.name       getdate error test
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ClockTimeTest, testGetdateError, Function | MediumTest | Level2) {
    // no env
    struct tm *retTm = nullptr;
    const char *cInput = "2020-10-26 00:01:01";
    retTm = getdate(cInput);
    EXPECT_EQ(nullptr, retTm);
    EXPECT_EQ(1, getdate_err);

    // set env, but file not exist
    int ret = setenv("DATEMSK", DATEMSK_FILE, 1);
    EXPECT_EQ(0, ret);
    retTm = getdate(cInput);
    EXPECT_EQ(nullptr, retTm);
    EXPECT_EQ(2, getdate_err);

    // creat file and set env
    FILE *fp = nullptr;
    char mask[10] = "%H:%M:%S";
    fp = fopen(DATEMSK_FILE, "w+");
    ASSERT_NE(nullptr, fp);
    ret = fwrite(mask, sizeof(mask), 1, fp);
    EXPECT_TRUE(ret > 0);
    ret = fclose(fp);
    EXPECT_NE(-1, ret);

    // test getdate
    retTm = getdate("10/26/20 00:01:01");
    EXPECT_EQ(nullptr, retTm);
    EXPECT_EQ(7, getdate_err);

    // restore
    ret = remove(DATEMSK_FILE);
    EXPECT_EQ(0, ret);
    ret = unsetenv("DATEMSK");
    EXPECT_EQ(0, ret);
}
