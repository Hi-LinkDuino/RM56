/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "write_file.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mbedtls/base64.h"
#include "securec.h"

namespace {
    const int MAX_FILE_LEN = 1000000;
    const int ONCE_WRITE = 2000;
}

int CopyFile(const char *org, const char *dest)
{
    int ret = 0;
    if (org == nullptr || dest == nullptr) {
        return -1;
    }
    int in = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (in < 0) {
        return -1;
    }

    int wholeLen = strlen(org);
    if (wholeLen == 0 || wholeLen > MAX_FILE_LEN) {
        close(in);
        return -1;
    }
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(wholeLen);
    if (buffer == nullptr) {
        close(in);
        return -1;
    }
    (void)memset_s(buffer.get(), wholeLen, 0, wholeLen);
    int len = 0;
    mbedtls_base64_decode(reinterpret_cast<unsigned char *>(buffer.get()), (size_t)wholeLen, (size_t *)&len,
        (unsigned char *)org, (size_t)wholeLen);
    int num = 0;
    while (num < len) {
        int trueLen = ((len - num) >= ONCE_WRITE) ? ONCE_WRITE : (len - num);
        char *temp = buffer.get() + num;
        num += trueLen;
        ret = write(in, temp, trueLen);
        if (ret < 0) {
            goto EXIT;
        }
    }
    ret = 0;
EXIT:
    close(in);
    return ret;
}

void DeleteFile(const char *path)
{
    if (path == nullptr) {
        return;
    }
    remove(path);
    return;
}

