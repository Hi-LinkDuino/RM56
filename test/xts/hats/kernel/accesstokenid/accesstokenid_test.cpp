/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "accesstokenid_test.h"
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <ctime>
#include <climits>
#include <pthread.h>
#include <sys/syscall.h>
#include <grp.h>

constexpr unsigned char ACCESS_TOKEN_ID_IOCTL_BASE = 'A';
constexpr unsigned int GET_TOKEN_ID = 1;
constexpr unsigned int SET_TOKEN_ID = 2;
constexpr unsigned int GET_FTOKEN_ID = 3;
constexpr unsigned int SET_FTOKEN_ID = 4;
constexpr unsigned int ACCESS_TOKENID_MAX_NR = 5;
#define	ACCESS_TOKENID_GET_TOKENID \
    _IOR(ACCESS_TOKEN_ID_IOCTL_BASE, GET_TOKEN_ID, unsigned long long)
#define	ACCESS_TOKENID_SET_TOKENID \
    _IOW(ACCESS_TOKEN_ID_IOCTL_BASE, SET_TOKEN_ID, unsigned long long)
#define	ACCESS_TOKENID_GET_FTOKENID \
    _IOR(ACCESS_TOKEN_ID_IOCTL_BASE, GET_FTOKEN_ID, unsigned long long)
#define	ACCESS_TOKENID_SET_FTOKENID \
    _IOW(ACCESS_TOKEN_ID_IOCTL_BASE, SET_FTOKEN_ID, unsigned long long)
#define	ACCESS_TOKENID_ILLEGAL1 \
    _IOW(ACCESS_TOKEN_ID_IOCTL_BASE, 0, unsigned long long)
#define	ACCESS_TOKENID_ILLEGAL2 \
    _IOW(ACCESS_TOKEN_ID_IOCTL_BASE, ACCESS_TOKENID_MAX_NR, unsigned long long)

constexpr unsigned long long INVAL_TOKEN = 0xffffffffffffffff;

#define CHILDREN_NUM			    3
#define WAIT_FOR_SHELL_OP_TIME		1
#define FATHER_WAIT_TIME		    (WAIT_FOR_SHELL_OP_TIME * (CHILDREN_NUM + 1))

constexpr unsigned int ACCESS_TOKEN_GRPID = 3020;
constexpr unsigned int ACCESS_TOKEN_OTHER_UID = 1234;
constexpr unsigned int ACCESS_TOKEN_OTHER_GRPID = 1234;

const char dev_accesstokenid[] = "/dev/access_token_id";

struct tokeninfo {
    pid_t               pid;
    pid_t               tid;
    unsigned long long  tokenid;
    unsigned long long  ftokenid;
};

namespace {
static unsigned long long GenRand64(void)
{
    struct timespec time = {0, 0};
    unsigned long long randvalue = 0;
    int fd;

    fd = open("/dev/random", O_RDONLY);
    if (fd > 0) {
        read(fd, &randvalue, sizeof(unsigned long long));
    }
    close(fd);

    sleep(1);
    clock_gettime(CLOCK_REALTIME, &time);

    return randvalue + time.tv_nsec;
}

static int GetTokenid(unsigned long long *token)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return -1;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_GET_TOKENID, token);
    if (ret) {
        printf("ioctl ACCESS_TOKENID_GET_TOKENID failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int SetTokenid(unsigned long long *token)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return -1;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_SET_TOKENID, token);
    if (ret) {
        printf("ioctl ACCESS_TOKENID_SET_TOKENID failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int GetfTokenid(unsigned long long *ftoken)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return -1;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_GET_FTOKENID, ftoken);
    if (ret) {
        printf("ioctl ACCESS_TOKENID_GET_FTOKENID failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int SetfTokenid(unsigned long long *ftoken)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return -1;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_SET_FTOKENID, ftoken);
    if (ret) {
        printf("ioctl ACCESS_TOKENID_SET_FTOKENID failed\r\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static void GetCurToken(unsigned long long *token, unsigned long long *ftoken)
{
    GetTokenid(token);
    GetfTokenid(ftoken);

    return;
}

static void *CheckChildThreadInheritance(void *args)
{
    struct tokeninfo *tinfo = (struct tokeninfo *) args;

    tinfo->pid = getpid();
    tinfo->tid = gettid();
    GetTokenid(&(tinfo->tokenid));
    GetfTokenid(&(tinfo->ftokenid));
    
    pthread_exit(nullptr);
    return nullptr;
}

static void *CheckChildThreadSetIndepent(void *args)
{
    struct tokeninfo *tinfo = (struct tokeninfo *) args;
    unsigned long long tokenSet = GenRand64();
    unsigned long long ftokenSet = GenRand64();
    unsigned long long tokenidGet = INVAL_TOKEN;
    unsigned long long ftokenidGet = INVAL_TOKEN;

    tinfo->pid = getpid();
    tinfo->tid = gettid();

    GetTokenid(&tokenidGet);
    GetfTokenid(&ftokenidGet);
    SetTokenid(&tokenSet);
    SetfTokenid(&ftokenSet);
    GetTokenid(&(tinfo->tokenid));
    GetfTokenid(&(tinfo->ftokenid));

    /* Indicate that the tokenid setting of each child thread does not met requirements. */
    if (ftokenidGet == 0 && tinfo->tokenid == tokenSet && tinfo->ftokenid == ftokenSet && tinfo->ftokenid != 0) {
        tinfo->ftokenid = INVAL_TOKEN;
    }

    pthread_exit(nullptr);
    return nullptr;
}
}

using namespace testing::ext;
using namespace std;

void AccesstokenidTest::SetUp() {}

void AccesstokenidTest::TearDown() {}

void AccesstokenidTest::SetUpTestCase() {}

void AccesstokenidTest::TearDownTestCase() {}

/**
 * @tc.name: CheckInitToken
 * @tc.desc: Test init value of tokenid and ftokenid
 * @tc.desc: tokenid equals to the father(hdcd) and ftokenid equals to 0
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, CheckInitToken, Function | MediumTest | Level1)
{
    unsigned long long token = INVAL_TOKEN;
    unsigned long long ftoken = INVAL_TOKEN;

    GetCurToken(&token, &ftoken);

    /* /data/service/el0/access_token/nativetoken.json
       {"processName":"hdcd","APL":3,"version":1,"tokenId":680034571,"tokenAttr":0,"dcaps":[]}
    */
    ASSERT_NE(0, token);
    ASSERT_EQ(0, ftoken);
}

/**
 * @tc.name: CheckSetTokenid
 * @tc.desc: Test setting of tokenid
 * @tc.desc: tokenid equals to the setting value
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, CheckSetTokenid, Function | MediumTest | Level1)
{
    unsigned long long token = INVAL_TOKEN;
    unsigned long long tokenSet = GenRand64();

    SetTokenid(&tokenSet);
    GetTokenid(&token);

    ASSERT_EQ(tokenSet, token);
}

/**
 * @tc.name: CheckSetfTokenid
 * @tc.desc: Test setting of ftokenid
 * @tc.desc: ftokenid equals to the setting value
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, CheckSetfTokenid, Function | MediumTest | Level1)
{
    unsigned long long ftoken = INVAL_TOKEN;
    unsigned long long ftokenSet = GenRand64();

    SetfTokenid(&ftokenSet);
    GetfTokenid(&ftoken);

    ASSERT_EQ(ftokenSet, ftoken);
}

/**
 * @tc.name: CheckChildThreadInheritance
 * @tc.desc: Test each child thread tokenid equals to father process while ftokenid not equals
 * @tc.desc: The ftokenid of each child thread equals to 0
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, CheckChildThreadInheritance, Function | MediumTest | Level1)
{
    pthread_t cid[10];

    unsigned long long token = INVAL_TOKEN;
    unsigned long long ftoken = INVAL_TOKEN;
    unsigned long long tokenSet = GenRand64();
    unsigned long long ftokenSet = GenRand64();

    struct tokeninfo tinfo;
    tinfo.pid = getpid();
    tinfo.tid = gettid();
    tinfo.tokenid = INVAL_TOKEN;
    tinfo.ftokenid = INVAL_TOKEN;

    GetTokenid(&token);
    GetfTokenid(&ftoken);

    SetTokenid(&tokenSet);
    SetfTokenid(&ftokenSet);

    for (int i = 0; i < 10; i++) {
        if (pthread_create(&cid[i], nullptr, CheckChildThreadInheritance, &tinfo) != 0) {
            printf("thread %d (ID %ld) pthread_create error\n", i, cid[i]);
        }

        if (pthread_join(cid[i], nullptr) != 0) {
            printf("thread %d (ID %ld) pthread_join error\n", i, cid[i]);
        }

        ASSERT_EQ(tinfo.tokenid, tokenSet);
        ASSERT_NE(tinfo.ftokenid, ftokenSet);
        ASSERT_EQ(0, tinfo.ftokenid);
    }
}

/**
 * @tc.name: CheckChildThreadSetIndepent
 * @tc.desc: Test each child thread tokenid and ftokenid is indepent
 * @tc.desc: The tokenid and ftokenid of each child thread not equal to father process
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, CheckChildThreadSetIndepent, Function | MediumTest | Level1)
{
    pthread_t cid[10];

    unsigned long long token = INVAL_TOKEN;
    unsigned long long ftoken = INVAL_TOKEN;
    unsigned long long tokenSet = GenRand64();
    unsigned long long ftokenSet = GenRand64();

    struct tokeninfo tinfo;
    tinfo.pid = getpid();
    tinfo.tid = gettid();
    tinfo.tokenid = INVAL_TOKEN;
    tinfo.ftokenid = INVAL_TOKEN;

    GetTokenid(&token);
    GetfTokenid(&ftoken);

    SetTokenid(&tokenSet);
    SetfTokenid(&ftokenSet);

    for (int i = 0; i < 10; i++) {
        if (pthread_create(&cid[i], nullptr, CheckChildThreadSetIndepent, &tinfo) != 0) {
            printf("thread %d (ID %ld) pthread_create error\n", i, cid[i]);
        }

        if (pthread_join(cid[i], nullptr) != 0) {
            printf("thread %d (ID %ld) pthread_join error\n", i, cid[i]);
        }

        ASSERT_NE(tinfo.tokenid, tokenSet);
        ASSERT_NE(tinfo.ftokenid, ftokenSet);
        ASSERT_NE(0, tinfo.ftokenid);
    }
}

/**
 * @tc.name: AbnormalGetTokenid
 * @tc.desc: Test abnormal ioctl cmd of ACCESS_TOKENID_GET_TOKENID
 * @tc.desc: using nullptr instead of the address of tokenid to ioctl
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, AbnormalGetTokenid, Function | MediumTest | Level1)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_GET_TOKENID, nullptr);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: AbnormalSetTokenid
 * @tc.desc: Test abnormal ioctl cmd of ACCESS_TOKENID_SET_TOKENID
 * @tc.desc: using nullptr instead of the address of tokenid to ioctl
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, AbnormalSetTokenid, Function | MediumTest | Level1)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_SET_TOKENID, nullptr);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: AbnormalGetfTokenid
 * @tc.desc: Test abnormal ioctl cmd of ACCESS_TOKENID_GET_FTOKENID
 * @tc.desc: using nullptr instead of the address of ftokenid to ioctl
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, AbnormalGetfTokenid, Function | MediumTest | Level1)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_GET_FTOKENID, nullptr);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: AbnormalSetfTokenid
 * @tc.desc: Test abnormal ioctl cmd of ACCESS_TOKENID_SET_FTOKENID
 * @tc.desc: using nullptr instead of the address of ftokenid to ioctl
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, AbnormalSetfTokenid, Function | MediumTest | Level1)
{
    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    int ret = ioctl(fd, ACCESS_TOKENID_SET_FTOKENID, nullptr);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: AbnormalIoctlCmd
 * @tc.desc: Test abnormal ioctl cmd of ACCESS_TOKENID_ILLEGAL1 and ACCESS_TOKENID_ILLEGAL1
 * @tc.desc: using illegal cmd instead of accesstokenid to ioctl
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, AbnormalIoctlCmd, Function | MediumTest | Level1)
{
    unsigned long long token;

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    int ret1 = ioctl(fd, ACCESS_TOKENID_ILLEGAL1, &token);
    int ret2 = ioctl(fd, ACCESS_TOKENID_ILLEGAL2, &token);
    close(fd);

    ASSERT_NE(0, ret1);
    ASSERT_NE(0, ret2);
}

/**
 * @tc.name: OtherUidSetTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_UID can not set tokenid
 * @tc.desc: tokenid can be only set by uid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, OtherUidSetTokenid, Function | MediumTest | Level1)
{
    unsigned long long tokenSet = GenRand64();
    int ret;

    ret = setuid(ACCESS_TOKEN_OTHER_UID);
    if (ret != 0) {
        printf("setuid error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_SET_TOKENID, &tokenSet);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: OtherUidGetTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_UID can get tokenid
 * @tc.desc: tokenid can get not only by uid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, OtherUidGetTokenid, Function | MediumTest | Level1)
{
    unsigned long long token = INVAL_TOKEN;
    int ret;

    ret = setuid(ACCESS_TOKEN_OTHER_UID);
    if (ret != 0) {
        printf("setuid error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_GET_TOKENID, &token);
    close(fd);

    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: WithoutGrpSetfTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_GRPID can not set ftokenid
 * @tc.desc: ftokenid can not set by groups without grpid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, WithoutGrpSetfTokenid, Function | MediumTest | Level1)
{
    int ret;
    size_t size = 1;
    gid_t list[1] = {ACCESS_TOKEN_OTHER_GRPID};
    unsigned long long ftokenSet = GenRand64();

    ret = setgroups(size, list);
    if (ret != 0) {
        printf("setgroups error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_SET_FTOKENID, &ftokenSet);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: WithoutGrpGetfTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_GRPID can not get ftokenid
 * @tc.desc: ftokenid can not get by groups without grpid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, WithoutGrpGetfTokenid, Function | MediumTest | Level1)
{
    int ret;
    size_t size = 1;
    gid_t list[1] = {ACCESS_TOKEN_OTHER_GRPID};
    unsigned long long ftoken = INVAL_TOKEN;

    ret = setgroups(size, list);
    if (ret != 0) {
        printf("setgroups error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_GET_FTOKENID, &ftoken);
    close(fd);

    ASSERT_NE(0, ret);
}

/**
 * @tc.name: WithGrpSetfTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_GRPID and ACCESS_TOKEN_GRPID can set ftokenid
 * @tc.desc: ftokenid can set by groups with grpid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, WithGrpSetfTokenid, Function | MediumTest | Level1)
{
    int ret;
    size_t size = 2;
    gid_t list[2] = {ACCESS_TOKEN_OTHER_GRPID, ACCESS_TOKEN_GRPID};
    unsigned long long ftokenSet = GenRand64();

    ret = setgroups(size, list);
    if (ret != 0) {
        printf("setgroups error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_SET_FTOKENID, &ftokenSet);
    close(fd);

    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: WithGrpGetfTokenid
 * @tc.desc: Test ACCESS_TOKEN_OTHER_GRPID and ACCESS_TOKEN_GRPID can set ftokenid
 * @tc.desc: ftokenid can set by groups with grpid 3020
 * @tc.type: FUNC
 */
HWTEST_F(AccesstokenidTest, WithGrpGetfTokenid, Function | MediumTest | Level1)
{
    int ret;
    size_t size = 2;
    gid_t list[2] = {ACCESS_TOKEN_OTHER_GRPID, ACCESS_TOKEN_GRPID};
    unsigned long long ftoken = INVAL_TOKEN;

    ret = setgroups(size, list);
    if (ret != 0) {
        printf("setgroups error %d \r\n", ret);
    }

    int fd = open(dev_accesstokenid, O_RDWR);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_accesstokenid);
        return;
    }

    ret = ioctl(fd, ACCESS_TOKENID_GET_FTOKENID, &ftoken);
    close(fd);

    ASSERT_EQ(0, ret);
}

