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

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

#include "securec.h"
#include "utils.h"
#include "utils/log/aie_log.h"

using namespace std;

namespace {
    const int CHAR_TYPE = 4;
    const char MIN_UPPER_CASE_CHAR = 'A';
    const char MIN_LOWER_CASE_CHAR = 'a';
    const char MIN_NUMERIC_CHAR = '0';
    const char TRAILING_CHAR = '\0';
    const int NUMBER_OF_ALPHABETS = 26;
    const int NUMBER_OF_DIGITS = 10;
    const int CHAR_TYPE_UPPER_CASE = 1;
    const int CHAR_TYPE_LOWER_CASE = 2;
    const int CHAR_TYPE_WHITE_SPACE = 3;
    const char WHITE_SPACE = ' ';
    const int DIVISOR_TWO = 2;
    const int DEFAULT_RANDOM_INT = 1;
}

/**
 * Return n: 0 <= n <= max
 */
int GetRandomInt(int max)
{
    if (max >= 0) {
        int randomInt = rand() % (max + 1);
        return randomInt;
    }
    return DEFAULT_RANDOM_INT;
}

/**
 * Return true or false by random.
 */
bool GetRandomBool()
{
    return (rand() % DIVISOR_TWO == 1) ? true : false;
}

/**
 * Gets current system date and time in format of YYYYMMDDhhmmss. E.g. 20201109123000.
 */
std::string GetSystemTime()
{
    time_t currentSystemTime = time(nullptr);
    char formatTime[32] = {};
    strftime(formatTime, sizeof(formatTime), "%Y%m%d%H%M%S", localtime(&currentSystemTime));
    string formatTimeString = formatTime;
    return formatTimeString;
}

/**
 * Generates a string with specified length by random, which ends up with
 * a null character '\0'. As a result, to generate a string with 20 characters,
 * the len must be 21, and the str must point to an array of which the length is 21.
 *
 * len  Length of string.
 * str  The string to be generated.
 */
void GetRandomString(const int len, char *str)
{
    srand(time(nullptr));
    for (int i = 0; i < len - 1; ++i) {
        switch (rand() % CHAR_TYPE) {
            case CHAR_TYPE_UPPER_CASE:
                str[i] = MIN_UPPER_CASE_CHAR + rand() % NUMBER_OF_ALPHABETS;
                break;
            case CHAR_TYPE_LOWER_CASE:
                str[i] = MIN_LOWER_CASE_CHAR + rand() % NUMBER_OF_ALPHABETS;
                break;
            case CHAR_TYPE_WHITE_SPACE:
                str[i] = WHITE_SPACE;
                break;
            default:
                str[i] = MIN_NUMERIC_CHAR + rand() % NUMBER_OF_DIGITS;
                break;
        }
    }

    str[len - 1] = TRAILING_CHAR;
}

/**
 * Save characters to txt Document.
 *
 * input a string to be saved in the file.
 * path  path and filename of result text
 */
void SaveCharTextAppend(char *input, char *path)
{
    HILOGI("[Test][SaveCharTextAppend]input is %s\n", input);
    if (path == nullptr) {
        return;
    }
    FILE *pFile = fopen(path, "a+");

    if (pFile == nullptr) {
        return;
    }
    fwrite(input, strlen(input), 1, pFile);
    fwrite("\n", 1, 1, pFile);
    fclose(pFile);
}

/**
 * Save strings to txt Document.
 *
 * input a string to be saved in the file.
 * path  path and filename of result text
 */
void SaveStringTextAppend(std::string input, std::string path)
{
    SaveCharTextAppend((char*)input.c_str(), (char*)path.c_str());
}
