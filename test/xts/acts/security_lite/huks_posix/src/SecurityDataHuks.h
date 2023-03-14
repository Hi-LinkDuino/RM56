/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TEST_XTS_ACTS_SECURITY_LITE_DATAHUKS_POSIX_SRC_SECURITYDATAHUKS_H
#define TEST_XTS_ACTS_SECURITY_LITE_DATAHUKS_POSIX_SRC_SECURITYDATAHUKS_H
#include "hks_client.h"
#include "hks_types.h"
#include <securec.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hks_file_api.h>
#include "gtest/gtest.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hks_errno.h"
#include <sys/time.h>
#include <hks_hardware_api.h>

#define DOUBLE_INT(x) ((x) * 2)
#define DOUBLE_STR_LEN(x) ((x) * 2 + 1)
#define FILE_ERROR -1
#define HICHAIN_FILEPATH "/storage/"
#define HKS_FREE_PTR1(p) { if ((p) != NULL) { free(p); (p) = NULL; }}

constexpr int NUM1 = 1;
constexpr int NUM2 = 2;
constexpr int NUM3 = 3;
constexpr int NUM4 = 4;
constexpr int NUM5 = 5;
constexpr int NUM6 = 6;
constexpr int NUM8 = 8;
constexpr int NUM9 = 9;
constexpr int NUM10 = 10;
constexpr int NUM11 = 11;
constexpr int NUM12 = 12;
constexpr int NUM15 = 15;
constexpr int NUM16 = 16;
constexpr int NUM20 = 20;
constexpr int NUM30 = 30;
constexpr int NUM21 = 21;
constexpr int NUM24 = 24;
constexpr int NUM32 = 32;
constexpr int NUM60 = 60;
constexpr int NUM62 = 62;
constexpr int NUM64 = 64;
constexpr int NUM65 = 65;
constexpr int NUM88 = 88;
constexpr int NUM100 = 100;
constexpr int NUM128 = 128;
constexpr int NUM192 = 192;
constexpr int NUM256 = 256;
constexpr int NUM1025 = 1025;
constexpr unsigned NUM2048 = 2048;
constexpr int NUM1234567 = 1234567;
constexpr int NUM1000000 = 1000000;
constexpr int NUM134 = -134;
constexpr int NUM135 = -135;
constexpr int NUM142 = -142;
constexpr int NUM1000 = -1000;
constexpr int NUM1006 = -1006;
constexpr int NUM1007 = -1007;
constexpr int NUM1010 = -1010;
constexpr int G_COUNT_MEM = 10000;
constexpr int MAX_INT = 2147483647;
constexpr int FILE_PATH_LENGTH = 256;

using namespace std;
using namespace testing::ext;
using errno_t = int32_t;

errno_t FopenS(FILE **fp, const char *filename, const char *modes);

int32_t FileSize(const char *filename);

int32_t FileWrite(const char *filename, uint32_t offset, const uint8_t *buf, uint32_t len);

int32_t FileRead(const char *filename, uint32_t offset, uint8_t *buf, uint32_t len);

uint64_t GetTimeMs();

void AddLog(const char* logType, const char *tag, const char *func, const char *format, const va_list* ap);

void Logi(const char *tag, const char *func, const char *format, ...);

void Logw(const char *tag, const char *func, const char *format, ...);

void Loge(const char *tag, const char *func, const char *format, ...);

void Logd(const char *tag, const char *func, const char *format, ...);

int32_t HksTestGetHardwareUdid(uint8_t* udid, uint32_t udidLen);

void HksStBlobInit1(struct hks_blob *blob, size_t nmemb, size_t size, uint8_t type);

void HksBlobDestroyT1(struct hks_blob *blob);

void HexStringToByte(const char *str, int nLen, unsigned char *pHex);

void BuildBlobData(struct hks_blob *param, const char *str, uint8_t type, uint32_t size, uint8_t isDataNull);

#endif
