/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#define _GNU_SOURCE
#include <string.h>
#include "utils_file.h"
#define LOG_TAG "UtilsFile"
#include "log.h"

#define UtilsFileAssert(cond, expr)                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            HILOG_ERROR(HILOG_MODULE_APP, "%d: '%s' failed", __LINE__, #cond); \
            expr;                                                              \
        }                                                                      \
    } while (0)

void utils_file_test()
{
    const char *path = "/data/test.txt";
    const char *data = "utils_file_test";
    const int whence[3] = {SEEK_SET_FS, SEEK_CUR_FS, SEEK_END_FS};
    char buf[64] = {0};
    unsigned int fileSize = 0;

    int ret;
    int fd = UtilsFileOpen(path, O_WRONLY_FS | O_CREAT_FS, 0664);
    UtilsFileAssert(fd >= 0, return );

    ret = UtilsFileWrite(fd, data, strlen(data));
    UtilsFileAssert(ret == strlen(data), goto ERR);
    ret = UtilsFileClose(fd);
    UtilsFileAssert(ret == 0, return );

    fd = UtilsFileOpen(path, O_RDONLY_FS, 0);
    UtilsFileAssert(fd >= 0, return );
    ret = UtilsFileStat(path, &fileSize);
    UtilsFileAssert(ret == 0, goto ERR);
    HILOG_DEBUG(HILOG_MODULE_APP, "fileSize %u", fileSize);

    for (int i = 0; i < 3; i++) {
        ret = UtilsFileSeek(fd, 0, whence[i]);
        UtilsFileAssert(ret >= 0, goto ERR);
    }

    UtilsFileSeek(fd, 0, SEEK_SET_FS);
    ret = UtilsFileRead(fd, buf, 4);
    UtilsFileAssert(ret == 4, goto ERR);
    buf[4] = '\0';
    HILOG_DEBUG(HILOG_MODULE_APP, "read: %s", buf);
    UtilsFileSeek(fd, 2, SEEK_SET_FS);
    ret = UtilsFileRead(fd, buf, 4);
    UtilsFileAssert(ret == 4, goto ERR);
    buf[4] = '\0';
    HILOG_DEBUG(HILOG_MODULE_APP, "read: %s", buf);

ERR:
    ret = UtilsFileClose(fd);
    UtilsFileAssert(ret == 0, return );
    ret = UtilsFileDelete(path);
    UtilsFileAssert(ret == 0, return );
}
