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

#ifndef USB_UTILS_H
#define USB_UTILS_H

#include <string>

struct ProcInfo {
    double ramPeak;
    double ramAvg;
    double cpuPeak;
    double cpuAvg;
    int32_t threadPeak;
};

struct ParseProcInfo {
    double ramTotal;
    double ramPeak;
    double ramCur;
    double cpuTotal;
    double cpuPeak;
    double cpuCur;

    int32_t ramCount;
    int32_t cpuCount;
    int32_t threadPeak;
    int32_t threadCur;
};

extern bool HasLog(const std::string &target, double startTs, const std::string &file = "/data/acm_write_xts");
extern double GetNowTs(void);
extern char *ParseSysCmdResult(FILE &result, int32_t line, int32_t word);
extern void CalcProcInfoFromFile(struct ProcInfo &info, const std::string &file);
extern double GetTsFromLog(const std::string &target, double startTs, const std::string &file);

#endif