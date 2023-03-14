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

#ifndef XTS_ACTS_MEDIA_TEST_UTILS_H
#define XTS_ACTS_MEDIA_TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

const int32_t RET_OK = 0;
const int32_t RET_ERR = -1;
const int32_t FrameErrCode = -1;         // frame error code
const int32_t RecoderSourceMaxCount = 4; // max recorder source setting
const int32_t FdNull = -1;               // fd is null

/* *
 * get current dir
 * @return  string - current file path of the test suits
 */
std::string GetCurDir();

/* *
 * check if file exist
 * @param filename filename
 * @return  check result RET_OK or RET_ERR
 */
int32_t FileCheck(const char *filename);

/* *
 * Save Capture picture
 * @param p char ptr
 * @param size char size
 * @return pic saved success or failed retur RET_OK or RET_ERR
 */
int32_t SampleSaveCapture(std::string g_testPath, const char *p, uint32_t size);

/* *
 * get recorder fd
 * @return fd
 */
int32_t SampleGetRecordFd(std::string &recordFilePath);

#endif // XTS_ACTS_MEDIA_TEST_UTILS_H