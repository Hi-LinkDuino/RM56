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

#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

#include "securec.h"
#include "utils/log/aie_log.h"

/**
 * return random number n: 0 <= n <= max
 */
int GetRandomInt(int max);

/**
 * Return true of false by random.
 */
bool GetRandomBool();

/**
 * Gets current system date and time in format of YYYYMMDDhhmmss. E.g. 20201109123000.
 */
std::string GetSystemTime();

/**
 * Generates a string with specified length by random.
 *
 * len  Length of string + 1, e.g. to generate a 20-bit string, set the input parameter len to 21.
 * str  The string to be generated.
 */
void GetRandomString(const int len, char *str);

/**
 * Save characters to txt Document.
 *
 * input a string to be saved in the file.
 * path  path and filename of result text
 */
void SaveCharTextAppend(char *input, char *path);

/**
 * Save strings to txt Document.
 *
 * input a string to be saved in the file.
 * path  path and filename of result text
 */
void SaveStringTextAppend(std::string input, std::string path);

#endif