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

#ifndef KERNEL_LITE_IO_TEST_H
#define KERNEL_LITE_IO_TEST_H

#include <stdio.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"

#define IOTEST_TEMPFILE "/storage/io_posix_test.txt"

class IoTest : public::testing::Test {
protected:
    static void TearDownTestCase()
    {
        if (remove(IOTEST_TEMPFILE) == -1) {
            LOG("TearDownTestCase: remove test file(/storage/io_posix_test.txt) failed, errno=%d", errno);
        }
    }
    void TearDown()
    {
        Msleep(50); // don't test too fast
    }
};

#define INIT_TEST_FILE(fp) do {                                                 \
    FOPEN_WRITE(fp);                                                            \
    ASSERT_NE(fputs("hello world", fp), -1) << "fputs fail, errno = " << errno; \
    ASSERT_NE(fclose(fp), -1) << "fclose fail, errno = " << errno;              \
} while (0)

#define FOPEN_WRITE(fp) do {                                                    \
    fp = fopen(IOTEST_TEMPFILE, "w");                                           \
    ASSERT_NE(fp, nullptr) << "fopen fail, errno = " << errno;                  \
} while (0)

#define FOPEN_READ(fp) do {                                                     \
    fp = fopen(IOTEST_TEMPFILE, "r");                                           \
    ASSERT_NE(fp, nullptr) << "fopen fail, errno = " << errno;                  \
} while (0)

#define FILENO(fp) do {                                                         \
    fd = fileno(fp);                                                            \
    ASSERT_NE(fd, -1) << "> fileno fail, errno = " << errno;                    \
} while (0)

#endif
