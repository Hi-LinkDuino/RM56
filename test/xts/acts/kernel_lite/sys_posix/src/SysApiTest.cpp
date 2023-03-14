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

#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <search.h>
#include <err.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include "gtest/gtest.h"
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"

#define SYS_TEST_FILE_01 "/storage/data/systest01.txt"
using namespace testing::ext;

class SysApiTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        LOG("setup ok\n");
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
        unsetenv("PATH");
        unsetenv("HOME");
        LOG("teardown ok\n");
    }
};

/**
* @tc.number     SUB_KERNEL_SYS_GETENV_0100
* @tc.name       test getenv
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testGetenv, Function | MediumTest | Level2)
{
    EXPECT_STREQ(getenv("HOME"), nullptr) << "Getenv HOME error!";
    EXPECT_STREQ(getenv("ROOT"), nullptr) << "Getenv HOME error!";

    ASSERT_EQ(setenv("PATH", "/bin", 1), 0) << "setenv PATH fail!";
    EXPECT_STREQ(getenv("PATH"), "/bin") << "secure_get PATH env error!";

    setenv("PATH", "/usr/bin", 0);
    EXPECT_STREQ(getenv("PATH"), "/bin") << "setenv PATH error!";
    unsetenv("PATH");
}

/**
* @tc.number     SUB_KERNEL_SYS_SECURE_GETENV_0100
* @tc.name       test secure_getenv
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testSecureGetenv, Function | MediumTest | Level2)
{
    EXPECT_STREQ(secure_getenv("HOME"), NULL) << "Secure_getenv HOME error!";
    EXPECT_STREQ(secure_getenv("ROOT"), NULL) << "Secure_getenv HOME error!";

    ASSERT_EQ(setenv("PATH", "", 1), 0) << "setenv PATH fail!";
    EXPECT_STREQ(secure_getenv("PATH"), "") << "secure_getenv PATH error!";

    setenv("PATH", "/usr/bin", 0);
    EXPECT_STREQ(secure_getenv("PATH"), "") << "setenv PATH error!";
    unsetenv("PATH");
}

/**
* @tc.number     SUB_KERNEL_SYS_SETENV_0100
* @tc.name       test setenv
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testSetenv, Function | MediumTest | Level2)
{
    ASSERT_EQ(setenv("", "/bin", 1), -1) << "envname error ";
    ASSERT_EQ(setenv("HOME", "/bin", 1), 0) << "setenv fail!";
    EXPECT_STREQ(secure_getenv("HOME"), "/bin") << "getenv fail !";
    int pid;
    pid = fork();
    if (pid == 0) {
        if (strcmp(secure_getenv("HOME"), "/bin") == 0) {
            exit(0);
        } else {
            LOG("child process home env is different from parent !");
            exit(1);
        }
    } else {
            WaitProcExitedOK(pid);
    }
}

/**
* @tc.number    SUB_KERNEL_SYS_SETENV_0200
* @tc.name      test setenv in child process
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testSetenvInChild, Function | MediumTest | Level3)
{
    setenv("HOME", "", 1);
    int pid;
    pid = fork();
    if (pid == 0) {
        setenv("HOME", "/usr/bin", 1);
        if (strcmp(secure_getenv("HOME"), "/usr/bin") == 0) {
            exit(0);
        } else {
            LOG("child process getenv error!");
            exit(1);
        }
    } else {
        WaitProcExitedOK(pid);
        EXPECT_STREQ(secure_getenv("HOME"), "") << "parent env has changed by child process !";
    }
    unsetenv("HOME");
}

/**
* @tc.number     SUB_KERNEL_SYS_PUTENV_0100
* @tc.name       test putenv
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testPutenv, Function | MediumTest | Level2)
{
    ASSERT_EQ(putenv((char *)"PATH=/bin"), 0) << "putenv PATH fail!";
    EXPECT_STREQ(secure_getenv("PATH"), "/bin") << "getenv fail !";
    unsetenv("PATH");
}

/**
* @tc.number     SUB_KERNEL_SYS_PUTENV_0200
* @tc.name       test putenv in parent and child process
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testPutenvInProcess, Function | MediumTest | Level3)
{
    ASSERT_EQ(putenv((char *)"HOME=/bin"), 0) << "putenv PATH fail!";
    int pid;
    pid = fork();
    if (pid == 0) {
        int rt = 0;
        if (strcmp(secure_getenv("HOME"), "/bin") != 0) {
            rt = 1;
            LOG("child process PATH env are different from parent process!");
        }
        if (putenv((char *)"PATH=/bin") != 0) {
            rt = 1;
            LOG("putenv PATH fail in child process!");
        }
        exit(rt);
    } else {
        WaitProcExitedOK(pid);
        EXPECT_STRNE(secure_getenv("PATH"), "/bin") << "parent env has changed by child process !";
    }
    unsetenv("HOME");
    unsetenv("PATH");
}

/**
* @tc.number    SUB_KERNEL_SYS_UNSETENV_0100
* @tc.name      test unsetenv
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testUnsetenv, Function | MediumTest | Level2)
{
    setenv("HOME", "/bin", 1);
    ASSERT_EQ(unsetenv("HOME"), 0) << "delete home env error !";
    LOG("after Unsetenv homePath=%s", secure_getenv("HOME"));
    EXPECT_STREQ(secure_getenv("HOME"), NULL) << "home env is not null !";

    ASSERT_EQ(unsetenv("ABC"), 0) << "unsetenv other env error !";
}

int CompareInt(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}
/**
* @tc.number    SUB_KERNEL_SYS_QSORT_0100
* @tc.name      test qsort int
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testQsort, Function | MediumTest | Level1)
{
    int iArray[] = { -99, 0, 65537, 100, 15, 6000 };
    int iArrayOk[] = { -99, 0, 15, 100, 6000, 65537 };
    qsort(iArray, 6, sizeof(int), CompareInt);
    for (int i = 0; i < 6; i++){
        EXPECT_EQ(iArray[i], iArrayOk[i]) << "int qsort error!";
    }
}

int CompareChar(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}
/**
* @tc.number    SUB_KERNEL_SYS_QSORT_0200
* @tc.name      test qsort char
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testQsortChar, Function | MediumTest | Level1)
{
    char cArray[] = { 'z', 's', 'a', 'j', 'p', 'o' };
    char cArrayOk[] = { 'a', 'j', 'o', 'p', 's', 'z' };
    qsort(cArray, 6, sizeof(char), CompareChar);
    EXPECT_EQ(strncmp(cArray, cArrayOk, sizeof(cArray)), 0) << "char qsort error!";
}

int CompareFloat(const void *a, const void *b)
{
    float fa = *(float *)a;
    float fb = *(float *)b;
    return (fa > fb) ? 1 : -1;
}
/**
* @tc.number    SUB_KERNEL_SYS_QSORT_0300
* @tc.name      test qsort float
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testQsortFloat, Function | MediumTest | Level2)
{
    float fArray[] = { 1.2f, 10.5f, 3.8f, 99.7f, 0.8f, 6000.0f };
    float fArrayOk[] = { 0.8f, 1.2f, 3.8f, 10.5f, 99.7f, 6000.0f };
    qsort(fArray, 6, sizeof(float), CompareFloat);
    for (int i = 0; i < 6; i++) {
        EXPECT_FLOAT_EQ(fArray[i], fArrayOk[i]) << "float qsort error!";
    }
}

int CompareDouble(const void *a, const void *b)
{
    double fa = *(double *)a;
    double fb = *(double *)b;
    return (fa > fb) ? 1 : -1;
}
/**
* @tc.number    SUB_KERNEL_SYS_QSORT_0400
* @tc.name      test qsort double
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testQsortDouble, Function | MediumTest | Level2)
{
    double fArray[] = { 1.78, 1.09, 1.63, -1.11, 1.99, 1.30 };
    double fArrayOk[] = { -1.11, 1.09, 1.30, 1.63, 1.78, 1.99 };
    qsort(fArray, 6, sizeof(double), CompareDouble);
    for (int i = 0; i < 6; i++) {
        EXPECT_DOUBLE_EQ(fArray[i], fArrayOk[i]) << "double qsort error!";
    }
}

void TestRegcomp(int flag, const char *pattern, const char *buf, const char *resOk)
{
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    char res[64];
    int j = 0;
    regcomp(&reg, pattern, flag);
    int status = regexec(&reg, buf, nmatch, pmatch, 0);
    EXPECT_EQ(status, 0) << "extended flag error !";
    if (status == REG_NOMATCH) {
        LOG("no match");
    } else if (status == 0) {
        LOG("Match:");
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
            putchar(buf[i]);
            res[j] = buf[i];
            j++;
        }
        res[j] = 0;
        LOG("\n");
    }
    EXPECT_STREQ(res, resOk) << "match result error !";
    regfree(&reg);
}

/**
* @tc.number    SUB_KERNEL_SYS_REGCOMP_0100
* @tc.name      test regcomp cflags = Extended
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegcompExtended, Function | MediumTest | Level2)
{
    TestRegcomp(REG_EXTENDED, "^\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*.\\w+([-.]\\w+)*$",
                "harmony20000925@abcdef.com", "harmony20000925@abcdef.com");
}

/**
* @tc.number    SUB_KERNEL_SYS_REGCOMP_0200
* @tc.name      test regcomp cflags = ICASE
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegcompIcase, Function | MediumTest | Level2)
{
    TestRegcomp(REG_ICASE, "HARMONY[1-9]",
            "harmony20000925@abcdef.com", "harmony2");
}

/**
* @tc.number    SUB_KERNEL_SYS_REGCOMP_0300
* @tc.name      test regcomp cflags = NEWLINE
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegcompNewline, Function | MediumTest | Level2)
{
    TestRegcomp(REG_EXTENDED|REG_NEWLINE, "^addr=([^&]*)",
            "testohos&sex=girl&age=18\r\naddr=bantian&hobby=movie", "addr=bantian");
}

/**
* @tc.number    SUB_KERNEL_SYS_REGCOMP_0400
* @tc.name      test regcomp cflags = NOSUB
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegcompNosub, Function | MediumTest | Level3)
{
    int cflags = REG_ICASE|REG_NOSUB;
    regex_t reg;
    const char* buf = "harmony20000925@abcdef.com";
    const char* pattern2 = "HARMONY[1-9]";
    regcomp(&reg, pattern2, cflags);
    int status = regexec(&reg, buf, (size_t)0, nullptr, 0);
    EXPECT_EQ(status, 0) << "nosub flag error !";
    regfree(&reg);
}

/**
* @tc.number    SUB_KERNEL_SYS_REGERROR_0100
* @tc.name      test regerror from regcomp
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegerrorComp, Function | MediumTest | Level2)
{
    regex_t reg;
    const char* pattern2 = "^[a-zA-Z0-9]*@[a-zA-Z0-9-_.]*[.]*?";
    int status = regcomp(&reg, pattern2, REG_EXTENDED);
    char emsg[1024] = {0};
    size_t len = regerror(status, nullptr, emsg, sizeof(emsg));
    EXPECT_TRUE(len == 24) << "len return error !";
    len = len < sizeof(emsg) ? len : sizeof(emsg) - 1;
    emsg[len] = '\0';
    LOG("status = %d; ErrMsg: %s", status, emsg);
    EXPECT_STREQ("Invalid character range", emsg) << "get regerror message error !";
}

/**
* @tc.number    SUB_KERNEL_SYS_REGERROR_0200
* @tc.name      test regerror from regexec
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRegerrorExec, Function | MediumTest | Level2)
{
    regex_t reg;
    const char* buf = "harmony20000925@abcdef.com";
    const char* pattern2 = "HARMONY[1-9]";
    regcomp(&reg, pattern2, REG_EXTENDED);
    int status = regexec(&reg, buf, (size_t) 0, nullptr, 0);
    char emsg[1024] = {0};
    size_t len = regerror(status, &reg, emsg, sizeof(emsg));
    EXPECT_TRUE(len == 9) << "len return error !";
    len = len < sizeof(emsg) ? len : sizeof(emsg) - 1;
    emsg[len] = '\0';
    LOG("ErrMsg: %s", emsg);
    EXPECT_STREQ("No match", emsg) << "get regerror message error !";
}

/**
* @tc.number    SUB_KERNEL_SYS_STRERROR_0100
* @tc.name      test strerror
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testStrerror, Function | MediumTest | Level1)
{
    for (int i = 0; i < 15; i++) {
        LOG("%d : %s", i, strerror(i));
    }
    EXPECT_STREQ("No error information", strerror(-1)) << "get strerror error!";
    EXPECT_STREQ("No error information", strerror(0)) << "get strerror error!";
    EXPECT_STREQ("No such file or directory", strerror(2)) << "get strerror error!";
    EXPECT_STREQ("No child process", strerror(10)) << "get strerror error!";
    EXPECT_STREQ("No error information", strerror(65536)) << "get strerror error!";
}

/**
* @tc.number    SUB_KERNEL_SYS_STRERROR_L_0100
* @tc.name      test strerror_l
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testStrerror_l, Function | MediumTest | Level2)
{
    int errnum = 2;
    locale_t locale = newlocale(LC_ALL, "", (locale_t)0);
    char *errorStr = strerror_l(errnum, locale);
    LOG("Error: %s", errorStr);
    EXPECT_STREQ("No such file or directory", errorStr) << "get strerror error!";
}

/**
* @tc.number    SUB_KERNEL_SYS_STRERROR_R_0100
* @tc.name      test strerror_r
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testStrerror_r, Function | MediumTest | Level2)
{
    int rt;
    int errnum = 2;
    char buf[8];
    rt = strerror_r(errnum, buf, 8);
    EXPECT_EQ(rt, ERANGE) << "strerror_r return error!";
    EXPECT_STREQ(buf, "No such") << "get strerror_r error!";
    char buf2[256];
    rt = strerror_r(errnum, buf2, 256);
    EXPECT_EQ(rt, 0) << "strerror_r return error!";
    EXPECT_STREQ(buf2, "No such file or directory") << "get strerror_r error!";
    for (int i = 0; i < 256; i++) {
        buf2[i] = 0;
    }
    rt = strerror_r(errnum, buf2, 0);
    EXPECT_EQ(rt, ERANGE) << "strerror_r return error!";
    EXPECT_STREQ(buf2, "") << "get strerror_r error!";
}

/**
* @tc.number    SUB_KERNEL_SYS_UNAME_0100
* @tc.name      test uname
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testUname, Function | MediumTest | Level1)
{
    struct utsname sysInfo = {0};
    int rtVal = uname(&sysInfo);
    ASSERT_NE(-1, rtVal) << "ErrInfo: get uname error !";
    EXPECT_STREQ(sysInfo.sysname, SYSINFO_SYSNAME) << "sysname error!";
    LOG("    system name  : %s", sysInfo.sysname);
    LOG("    node name    : %s", sysInfo.nodename);
    LOG("    release      : %s", sysInfo.release);
    LOG("    version      : %s", sysInfo.version);
}

/**
* @tc.number    SUB_KERNEL_SYS_UNAME_0200
* @tc.name      uname error test
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testUnameNull, Function | MediumTest | Level3)
{
    int i = uname(nullptr);
    ASSERT_EQ(i, -1) << "Null param return error";
}

struct Myque {
    struct Myque *next;
    struct Myque *prev;
    char *name;
};
int CountQue(Myque *head)
{
    Myque *first = head;
    int i = 0;
    do {
        LOG("name=%s", head->name);
        head = head->next;
        i++;
    } while (head != first);
    LOG("Myque num = %d", i);
    return i;
}
/**
* @tc.number    SUB_KERNEL_SYS_Remque_0100
* @tc.name      test remque
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testRemque, Function | MediumTest | Level2)
{
    struct Myque element1 = {0}, element2 = {0}, element3 = {0};
    element1.name = (char*)"n1";
    element2.name = (char*)"n2";
    element3.name = (char*)"n3";
    insque(&element1, &element1);
    insque(&element2, &element1);
    insque(&element3, &element2);
    EXPECT_EQ(CountQue(&element1), 3) << "queue num error !";
    EXPECT_TRUE(element1.next == &element2) << "queue num error !";

    remque(&element2);
    EXPECT_EQ(CountQue(&element1), 2) << "queue num error !";
    EXPECT_TRUE(element1.next == &element3) << "queue num error !";

    remque(&element3);
    EXPECT_EQ(CountQue(&element1), 1) << "queue num error !";
    EXPECT_TRUE(element1.next == &element1) << "queue num error !";

    remque(&element1);
    EXPECT_EQ(CountQue(&element1), 1) << "queue num error !";
    EXPECT_TRUE(element1.next == &element1) << "queue num error !";
}

/**
* @tc.number    SUB_KERNEL_SYS_SEED48_0100
* @tc.name      test seed48
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testSeed48, Function | MediumTest | Level2)
{
    unsigned short seed[3] = {1, 10, 100};
    unsigned short *res = seed48(seed);
    res = seed48(seed);
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(res[i], seed[i]) << "seed48 test error!";
    }
    long d;
    for (int i = 0; i < 10; i++) {
        seed48(seed);
        d = lrand48();
        EXPECT_EQ(d, 1542287255) << "seed48 test error!";
    }
}


void VerrLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    verr(0, format, args);
    va_end(args);
}
void VerrxLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    verrx(0, format, args);
    va_end(args);
}
void CheckStdPrint(const char *res)
{
    FILE *fp1 = fopen(SYS_TEST_FILE_01, "r");

    ASSERT_FALSE(fp1 == nullptr) << "fopen fail, errno = " << errno;
    char str[100] = {0};
    char *gStr = fgets(str, sizeof(str), fp1);
    printf("gStr = %s\n", gStr);
    EXPECT_STREQ(gStr, str);
    EXPECT_STREQ(str, res);
    fclose(fp1);
}
/**
* @tc.number    SUB_KERNEL_SYS_Verr_0100
* @tc.name      test verr
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testVerr, Function | MediumTest | Level2)
{
    int pid = fork();
    if (pid == 0) {
        FILE *fp = freopen(SYS_TEST_FILE_01, "w", stderr);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            exit(1);
        }
        errno = 2;
        VerrLog("argument for %s ", "verr");
        fclose(fp);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        CheckStdPrint("ActsSysApiTest.bin: argument for verr : No such file or directory\n");
    }
    remove(SYS_TEST_FILE_01);
}

/**
* @tc.number    SUB_KERNEL_SYS_Verr_0200
* @tc.name      test verr null
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testVerrNull, Function | MediumTest | Level2)
{
    int pid = fork();
    if (pid == 0) {
        FILE *fp = freopen(SYS_TEST_FILE_01, "w", stderr);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            exit(1);
        }
        errno = 2;
        VerrLog(nullptr, "verr");
        fclose(fp);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        CheckStdPrint("ActsSysApiTest.bin: No such file or directory\n");
    }
    remove(SYS_TEST_FILE_01);
}

/**
* @tc.number    SUB_KERNEL_SYS_Verrx_0100
* @tc.name      test verrx
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testVerrx, Function | MediumTest | Level2)
{
    int pid = fork();
    if (pid == 0) {
        FILE *fp = freopen(SYS_TEST_FILE_01, "w", stderr);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            exit(1);
        }
        errno = 2;
        VerrxLog("argument for %s ", "verrx");
        fclose(fp);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        CheckStdPrint("ActsSysApiTest.bin: argument for verrx \n");
    }
    remove(SYS_TEST_FILE_01);
}

/**
* @tc.number    SUB_KERNEL_SYS_Verrx_0200
* @tc.name      test verrx null
* @tc.desc      [C- SOFTWARE -0200]
*/
HWTEST_F(SysApiTest, testVerrxNull, Function | MediumTest | Level2)
{
    int pid = fork();
    if (pid == 0) {
        FILE *fp = freopen(SYS_TEST_FILE_01, "w", stderr);
        if (fp == nullptr) {
            LOG("freopen fail, errno = %d", errno);
            exit(1);
        }
        errno = 2;
        VerrxLog(nullptr, "verrx");
        fclose(fp);
        exit(0);
    } else {
        WaitProcExitedOK(pid);
        CheckStdPrint("ActsSysApiTest.bin: \n");
    }
    remove(SYS_TEST_FILE_01);
}