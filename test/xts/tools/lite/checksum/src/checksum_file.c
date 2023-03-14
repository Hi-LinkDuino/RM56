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
 * Description: 对单个文件或目录下多文件进行计算SHA256校验和
 */

#include "checksum_file.h"

#include <dirent.h>
#include <errno.h>
#include <securec.h>
#include <stdio.h>
#include <string.h>

#include "checksum_sha256.h"


#define BUFFER_SIZE 4096
int g_fileNums = 0;

/**
 * 该方法实现将十六进制的字节转成字符串
 */
void HexArrayToString(const unsigned char *hexarray, int length, unsigned char *string)
{
    int byte = 4;
    unsigned char value = 0x0f;
    const unsigned char num2CharTable[16] = "0123456789ABCDEF";

    for (int i = 0; i < length; i++) {
        *(string++) = num2CharTable[(hexarray[i] >> byte) & value];
        *(string++) = num2CharTable[hexarray[i] & value];
    }

    *string = 0x0;
}

/**
 * 获取文件校验和的最终结果
 */
unsigned char* GetChecksumResult(MesgDigest* mesgd)
{
    unsigned int dataLen = mesgd->dataLen;
    unsigned int totalLen = 64;
    unsigned int len = 8;
    unsigned int groupNum = 4;
    int num = 24;
    unsigned char pad0 = 0x80;
    unsigned char pad1 = 0x00;
    static unsigned char hash[32];
    unsigned char bitNum = 0x000000ff;
    unsigned int i, j;
    errno_t err;

    // 0-dataLen之间元素保持不变，dataLen位填充1次1000 0000
    mesgd->data[dataLen++] = pad0;

    // 剩余部分填充0000 0000
    if (mesgd->dataLen < totalLen - len) {
        for (; dataLen < totalLen - len; dataLen++) {
            mesgd->data[dataLen] = pad1;
        }
    }

    if (mesgd->dataLen >= totalLen - len) {
        for (; dataLen < totalLen; dataLen++) {
            mesgd->data[dataLen] = pad1;
        }

        CalcSha256(mesgd, mesgd->data);
        err = memset_s(mesgd->data, sizeof(mesgd->data), 0, totalLen - len);
        if (err != EOK) {
            printf("memset_s failed, err = %d\n", err);
        }
    }

    mesgd->bitLen += mesgd->dataLen * len;

    // 填充长度的0-63位
    for (i = 1; i <= len; i++) {
        mesgd->data[totalLen - i] = mesgd->bitLen >> ((i - 1) * len);
    }

    // 计算填充后数据的sha256
    CalcSha256(mesgd, mesgd->data);

    for (i = 0; i < groupNum; ++i) {
        for (j = 0; j < len; j++) {
            hash[i + groupNum * j] = (mesgd->hash[j] >> (num - i * len)) & bitNum;
        }
    }

    return hash;
}


/**
 * 该方法计算文件数据块的SHA256校验和
 */
void CalcFileChunkSha256(MesgDigest* mesgd, unsigned char data[], size_t len)
{
    unsigned int dataLen = 64;
    unsigned int bitLen = 512;

    for (size_t i = 0; i < len; i++) {
        mesgd->data[mesgd->dataLen] = data[i];
        mesgd->dataLen++;
        if (mesgd->dataLen == dataLen) {
            CalcSha256(mesgd, mesgd->data);
            mesgd->bitLen += bitLen;
            mesgd->dataLen = 0;
        }
    }
}

/**
 * 该方法计算单个文件的SHA256校验和
 */
int CalcSingleFileSha256(char* fileName)
{
    unsigned int outputLen = 32;
    unsigned char *output = NULL;
    unsigned char outputStr[64];

    FILE *fp = NULL;
    if ((fp = fopen(fileName, "rb")) == NULL) {
        printf("error: fail to open file %s: %s.\n", fileName, strerror(errno));
    }

    MesgDigest mesgd;
    InitSha256(&mesgd);

    unsigned char buffer[BUFFER_SIZE];
    while (!feof(fp)) {
        size_t size = fread(buffer, 1, BUFFER_SIZE, fp);
        CalcFileChunkSha256(&mesgd, buffer, size);
    }
    fclose(fp);

    output = GetChecksumResult(&mesgd);
    HexArrayToString(output, outputLen, outputStr);

    g_fileNums++;
    printf("%s:%s\n", fileName, outputStr);
    return RESULT_SUCCESS;
}

char* CreatePathName(char *base, int len, char *name0, char *name1, char *name2)
{
    errno_t err;

    err = memset_s(base, len, '\0', len);
    if (err != EOK) {
        printf("memset_s failed, err = %d\n", err);
    }

    err = strcpy_s(base, len, name0);
    if (err != EOK) {
        printf("strcpy_s failed, err = %d\n", err);
    }

    err = strcat_s(base, len, name1);
    if (err != EOK) {
        printf("strcat_s failed, err = %d\n", err);
    }

    err = strcat_s(base, len, name2);
    if (err != EOK) {
        printf("strcat_s failed, err = %d\n", err);
    }
    
    return base;
}

/**
 * 该方法计算当前目录下所有文件的SHA256校验和
 */
int CalcMultiFilesSha256(char* dirPathName)
{
    DIR *dir;
    struct dirent *ptr = NULL;
    int maxLen = 1000;
    static char pathName[1000], fileName[1000];
    char *fileNewName = NULL;
    char *pathNewName = NULL;
    int typeFile = 8;
    int typeDir = 4;

    if ((dir = opendir(dirPathName)) == NULL) {
        printf("error:fail to open dir %s: %s.\n", dirPathName, strerror(errno));
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        } else if (ptr->d_type == typeFile) {
            // 获取的类型为文件，计算文件的校验和
            fileNewName = CreatePathName(fileName, maxLen, dirPathName, "/", ptr->d_name);
            CalcSingleFileSha256(fileNewName);
        } else if (ptr->d_type == typeDir) {
            // 获取的类型为目录，递归方式遍历所有子目录
            pathNewName = CreatePathName(pathName, maxLen, dirPathName, "/", ptr->d_name);
            CalcMultiFilesSha256(pathNewName);
        }
    }
    closedir(dir);
    return RESULT_SUCCESS;
}

/**
 * 该方法获取文件的个数
 */
int GetFileTotalNum(void)
{
    return g_fileNums;
}