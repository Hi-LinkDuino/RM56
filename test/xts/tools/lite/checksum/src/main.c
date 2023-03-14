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

/*
 * Description: 计算单个文件或目录下多文件进行计算SHA256校验和
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/checksum_file.h"

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int count = 2;
    char *filePath = argv[1];
    struct stat sBuf;

    if (argc != count) {
        printf("error: the argc is 2, but argc is %d.\n", argc);
    }

    int result = stat(filePath, &sBuf); // 获取文件信息，把信息放到sBuf中
    if (result == -1) {
        printf("error: fail to get the stat of %s: %s.\n", filePath, strerror(errno));
        return RESULT_FAILED;
    }

    if (S_ISDIR(sBuf.st_mode)) {
        CalcMultiFilesSha256(filePath); // 输入的文件路径为目录
    } else {
        CalcSingleFileSha256(filePath); // 输入的文件路径是普通文件
    }

    int fileNum = GetFileTotalNum();
    printf("the checksum test success,the total number of files is %d.\n", fileNum);

    return RESULT_SUCCESS;
}
