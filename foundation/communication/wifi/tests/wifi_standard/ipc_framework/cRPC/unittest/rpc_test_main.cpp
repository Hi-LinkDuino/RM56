/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>

static constexpr int MAX_SIZE = 10240;
extern "C" int __wrap_read(int fd, void *buf, unsigned int size)
{
    const int letterSize = 26;
    errno = 0;
    fd = 0;
    if (size == 0) {
        return 0;
    } else if (size == MAX_SIZE) {
        errno = EWOULDBLOCK;
        return -1;
    } else if (size > MAX_SIZE) {
        return -1;
    } else {
        for (unsigned int i = 0; i < size; ++i) {
            ((char *)buf)[i] = 'a' + (i % letterSize);
        }
        return size;
    }
}

extern "C" int __wrap_write(int fd, const void *buf, unsigned int size)
{
    errno = 0;
    fd = 0;
    if (buf == NULL) {
        return 0;
    }
    if (size == 0) {
        return 0;
    } else if (size == MAX_SIZE) {
        errno = EWOULDBLOCK;
        return -1;
    } else if (size > MAX_SIZE) {
        return -1;
    } else {
        return size;
    }
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
