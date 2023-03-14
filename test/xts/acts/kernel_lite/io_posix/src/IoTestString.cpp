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

#include "IoTest.h"
#include <string.h>

using namespace testing::ext;

/**
 * @tc.number SUB_KERNEL_IO_STRING_0100
 * @tc.name   strverscmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrverscmp, Function | MediumTest | Level1)
{
    int ret = strverscmp("hello world !", "HELLO WORLD");
    EXPECT_GT(ret, 0);

    ret = strverscmp("HELLO WORLD", "hello world !");
    EXPECT_LT(ret, 0);

    ret = strverscmp("hello world !", "hello world !");
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0200
 * @tc.name   strcoll basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcoll, Function | MediumTest | Level1)
{
    int ret = strcoll("AbC", "ABC");
    EXPECT_GT(ret, 0);

    ret = strcoll("ABC", "abc");
    EXPECT_LT(ret, 0);

    ret = strcoll("ABC", "ABC");
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0300
 * @tc.name   strcmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcmp, Function | MediumTest | Level1)
{
    int ret = strcmp("abcdef", "ABCDEF");
    EXPECT_GT(ret, 0);

    ret = strcmp("123456", "654321");
    EXPECT_LT(ret, 0);

    EXPECT_EQ(strcmp("~!@#$%^&*()_+", "~!@#$%^&*()_+"), 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0400
 * @tc.name   strncmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrncmp, Function | MediumTest | Level1)
{
    const char dest[] = "abcdef";
    const char src[] = "ABCDEF";
    int ret = strncmp(dest, src, 1);
    EXPECT_GT(ret, 0);

    ret = strncmp("123456", "654321", 6);
    EXPECT_LT(ret, 0);

    ret = strncmp("~!@#$%^&*()_+", "~!@#$%^&*()_+", 8);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0500
 * @tc.name   strxfrm basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrxfrm, Function | MediumTest | Level1)
{
    char dest[50] = {0};
    size_t ret = strxfrm(dest, "hello world !", sizeof(dest));
    EXPECT_EQ(ret, 13U);
    EXPECT_STREQ(dest, "hello world !");

    char destT[15] = {0};
    ret = strxfrm(destT, "this is string", sizeof(destT));
    EXPECT_EQ(ret, 14UL);
    EXPECT_STREQ(destT, "this is string");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0600
 * @tc.name   strsignal basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrsignal, Function | MediumTest | Level1)
{
    char *ret = strsignal(14);
    EXPECT_STREQ(ret, "Alarm clock");

    ret = strsignal(-1);
    EXPECT_STREQ(ret, "Unknown signal");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0700
 * @tc.name   stpcpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStpcpy, Function | MediumTest | Level1)
{
    const char *src = "hello world !";
    char dest[50] = {0};
    char *ret = stpcpy(dest, src);
    ASSERT_NE(ret, nullptr);
    EXPECT_STREQ(dest, src);

    const char *srcT = "~!@#$%%^&*()_+";
    char destT[50] = {0};
    ret = stpcpy(destT, srcT);
    ASSERT_NE(ret, nullptr);
    EXPECT_STREQ(destT, "~!@#$%%^&*()_+");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0800
 * @tc.name   stpncpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStpncpy, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    char dest[50] = {0};
    char *ret = stpncpy(dest, src, 5);
    EXPECT_STREQ(ret, "");
    EXPECT_STREQ(dest, "hello");

    char srcT[] = "~!@#$%%^&*()_+";
    char destT[50] = {0};
    ret = stpncpy(destT, srcT, 15);
    EXPECT_STREQ(ret, "");
    EXPECT_STREQ(destT, "~!@#$%%^&*()_+");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_0900
 * @tc.name   strlcpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrlcpy, Function | MediumTest | Level1)
{
    char dest[50] = {0};
    const char *src = "123456789abc";
    size_t ret = strlcpy(dest, src, sizeof(dest));
    EXPECT_EQ(ret, 12U);
    EXPECT_STREQ(dest, src);

    char destT[20] = {0};
    const char *srcT = " ~!@#$%^&*()_+";
    ret = strlcpy(destT, srcT, 15);
    EXPECT_EQ(ret, 14U);
    EXPECT_STREQ(destT, srcT);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1000
 * @tc.name   strcpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcpy, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    char dest[20] = {0};
    char *ret = strcpy(dest, src);
    EXPECT_STREQ(ret, dest);

    char srcT[] = "This is String1";
    char destT[20] = {0};
    ret = strcpy(destT, srcT);
    EXPECT_STREQ(ret, destT);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1100
 * @tc.name   strncpy basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrncpy, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    char dest[] = {0};
    char *ret = strncpy(dest, src, 6);
    EXPECT_STREQ(ret, dest);

    char destT[] = "~!@#$%^&*()_+";
    ret = strncpy(destT, "hello world", 0);
    EXPECT_STREQ(ret, destT);
}
/**
 * @tc.number SUB_KERNEL_IO_STRING_1200
 * @tc.name   strcasestr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcasestr, Function | MediumTest | Level1)
{
    char *ret = strcasestr("hello world !", " ");
    EXPECT_STREQ(ret, " world !");

    ret = strcasestr("This is String1", "Str");
    EXPECT_STREQ(ret, "String1");

    ret = strcasestr("hello world !", "ABC");
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1300
 * @tc.name   strlcat basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrlcat, Function | MediumTest | Level1)
{
    char src[] = "ABCDE";
    char dest[] = "abcdefgh";
    size_t ret = strlcat(dest, src, 8);
    EXPECT_EQ(ret, 13U);
    EXPECT_STREQ(dest, "abcdefgh");

    char srcT[] = "|/*";
    char destT[20] = "~!@#$%^&*()_+";
    ret = strlcat(destT, srcT, 11);
    EXPECT_EQ(ret, 14U);
    EXPECT_STREQ(destT, "~!@#$%^&*()_+");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1400
 * @tc.name   strcat basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcat, Function | MediumTest | Level1)
{
    char dest[50] = {0};
    char *ret = strcat(dest, "hello world !");
    EXPECT_STREQ(ret, dest);

    char destT[50] = "hello world ! || ";
    ret = strcat(destT, "This is String1");
    EXPECT_STREQ(ret, destT);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1500
 * @tc.name   strncat basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrncat, Function | MediumTest | Level1)
{
    char dest[50] = "hello world || ";
    char *ret = strncat(dest, "This is String1", 7);
    EXPECT_STREQ(ret, "hello world || This is");

    char destT[20] = "|/*";
    ret = strncat(destT, "~!@#$%^&*()_+", 13);
    EXPECT_STREQ(ret, destT);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1600
 * @tc.name   strchr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrchr, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, '!');
    EXPECT_STREQ(ret, "!! world");

    ret = strchr(src, '?');
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1700
 * @tc.name   strrchr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrrchr, Function | MediumTest | Level1)
{
    char src[] = "hello world & HEELO & WORLD";
    char *ret = strrchr(src, '&');
    EXPECT_STREQ(ret, "& WORLD");

    ret = strrchr(src, '?');
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1800
 * @tc.name   strnlen basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrnlen, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    unsigned int ltSrcSize = strlen(src) - 2;
    unsigned int gtSrcSize = strlen(src) + 2;

    size_t ret = strnlen(src, ltSrcSize);
    EXPECT_EQ(ret, ltSrcSize);

    ret = strnlen(src, gtSrcSize);
    EXPECT_EQ(ret, strlen(src));
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_1900
 * @tc.name   strtok basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtok, Function | MediumTest | Level1)
{
    char src[] = "hello world & This is-String1";
    char *ret = strtok(src, "&");
    EXPECT_STREQ(ret, src);

    ret = strtok(nullptr, "-");
    EXPECT_STREQ(ret, " This is");

    char srcS[] = "hello world !";
    ret = strtok(srcS, "?");
    EXPECT_STREQ(ret, srcS);

    char srcT[50] = {0};
    ret = strtok(srcT, "~");
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2000
 * @tc.name   strtok_r basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrtok_r, Function | MediumTest | Level1)
{
    char str[] = "hello world & This is-String1";
    char *endPtr = nullptr;
    char *ret = strtok_r(str, "&", &endPtr);
    EXPECT_STREQ(ret, str);
    EXPECT_STREQ(endPtr, " This is-String1");

    ret = strtok_r(nullptr, "-", &endPtr);
    EXPECT_STREQ(ret, " This is");
    EXPECT_STREQ(endPtr, "String1");

    char strS[] = "hello world !";
    ret = strtok_r(strS, "?", &endPtr);
    EXPECT_STREQ(ret, strS);
    EXPECT_STREQ(endPtr, nullptr);

    char strT[] = {0};
    ret = strtok_r(strT, "!", &endPtr);
    EXPECT_STREQ(ret, nullptr);
    EXPECT_STREQ(endPtr, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2100
 * @tc.name   strsep basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrsep, Function | MediumTest | Level1)
{
    char src[] = "123===ABC";
    char *str = src;
    char *ret = strsep(&str, "=");
    EXPECT_STREQ(ret, "123");

    char srcS[] = "hello world !";
    char *strS = srcS;
    ret = strsep(&strS, "?");
    EXPECT_STREQ(ret, "hello world !");
    EXPECT_EQ(strS, nullptr);

    char srcT[] = {0};
    char *strT = srcT;
    ret = strsep(&strT, "?");
    EXPECT_STREQ(ret, "");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2200
 * @tc.name   strdup basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrdup, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    char *ret = strdup(src);
    EXPECT_STREQ(ret, src) << "> strdup fail, errno = " << errno;

    char srcS[] = "This is String1";
    ret = strdup(srcS);
    EXPECT_STREQ(ret, "This is String1") << "> strdup fail, errno = " << errno;
    free(ret);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2300
 * @tc.name   strndup basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrndup, Function | MediumTest | Level1)
{
    char src[] = "hello world !";
    char *ret = strndup(src, 5);
    EXPECT_STREQ(ret, "hello") << "> strndup fail, errno = " << errno;

    char srcS[] = "This is String1";
    ret = strndup(srcS, 20);
    EXPECT_STREQ(ret, "This is String1") << "> strndup fail, errno = " << errno;
    free(ret);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2400
 * @tc.name   strpbrk basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrpbrk, Function | MediumTest | Level1)
{
    const char dest[] = "hello !! world";
    const char src[] = "!!";
    const char *ret = strpbrk(dest, src);
    EXPECT_STREQ(ret, "!! world");

    const char srcS[] = "??";
    ret = strpbrk(dest, srcS);
    EXPECT_STREQ(ret, nullptr);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2500
 * @tc.name   strcspn basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcspn, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char src[] = "!";
    size_t ret = strcspn(dest, src);
    EXPECT_EQ(ret, 12U);

    const char srcS[] = "a";
    ret = strcspn(dest, srcS);
    EXPECT_EQ(ret, 13U);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2600
 * @tc.name   strspn basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrspn, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char src[] = "heAlo";
    size_t ret = strspn(dest, src);
    EXPECT_EQ(ret, 5U);
    EXPECT_EQ(ret, 5U);

    const char destS[] = "this is string";
    const char srcS[] = "abc";
    ret = strspn(destS, srcS);
    EXPECT_EQ(ret, 0U);

    const char srcT[] = "helo\0 wal";
    ret = strspn(dest, srcT);
    EXPECT_EQ(ret, 5U);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2700
 * @tc.name   strstr basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrstr, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char src[] = "heAlo";
    const char *ret = strstr(dest, src);
    EXPECT_STREQ(ret, nullptr);

    const char destS[] = "string this is string";
    const char srcS[] = "string";
    ret = strstr(destS, srcS);
    EXPECT_STREQ(ret, "string this is string");

    const char srcT[] = "\0hello";
    ret = strstr(dest, srcT);
    EXPECT_STREQ(ret, "hello world !");
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2800
 * @tc.name   strcoll_l basic function testj
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrcoll_l, Function | MediumTest | Level1)
{
    locale_t locale = nullptr;
    int ret = strcoll_l("AbC", "ABC", locale);
    EXPECT_GT(ret, 0);

    ret = strcoll_l("ABC", "abc", locale);
    EXPECT_LT(ret, 0);

    ret = strcoll_l("ABC", "ABC", locale);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number SUB_KERNEL_IO_STRING_2900
 * @tc.name   strxfrm_l basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(IoTest, testStrxfrm_l, Function | MediumTest | Level1)
{
    char dest[50] = {0};
    locale_t locale = nullptr;
    size_t ret = strxfrm_l(dest, "hello world !", sizeof(dest), locale);
    EXPECT_EQ(ret, 13UL);
    EXPECT_STREQ(dest, "hello world !");

    char destT[15] = {0};
    ret = strxfrm_l(destT, "this is string", sizeof(destT), locale);
    EXPECT_EQ(ret, 14UL);
    EXPECT_STREQ(destT, "this is string");
}