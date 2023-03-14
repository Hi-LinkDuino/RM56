/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HC_FILE_COMMON_H
#define HC_FILE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_FILE_NAME "test"
#define TEST_FILE_DIR "/testDir"
#define DEFAULT_FILE_PERMISSION 0644
#define ASCII_START 32
#define ASCII_END 126
#define ONE_BYTE 1
#define HALF_KILOBYTE 512
#define ONE_KILOBYTE 1024
#define TEN_KILOBYTE 10240
#define TEST_FILE_SIZE_LIST_LEN 4

static const int testFileSizeList[TEST_FILE_SIZE_LIST_LEN] = {
    ONE_BYTE,
    HALF_KILOBYTE,
    ONE_KILOBYTE,
    TEN_KILOBYTE
};

char *GenerateTestingText(int size);

#ifdef __cplusplus
}
#endif
#endif // HC_FILE_COMMON_H