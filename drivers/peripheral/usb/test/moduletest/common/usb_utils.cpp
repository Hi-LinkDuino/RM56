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

#include "usb_utils.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sys/time.h>

using namespace std;

bool HasLog(const string &target, double startTs, const string &file)
{
    bool ret = false;
    ifstream logFile(file);
    string::size_type pos;
    string lineStr;
    const string flagStr = "[XTSCHECK]";
    const int32_t tsStartPos = 11;
    const int32_t tsLength = 17;
    while (getline(logFile, lineStr)) {
        double logTs;
        pos = lineStr.find(flagStr);
        if (pos != string::npos) {
            logTs = stod(lineStr.substr(pos + tsStartPos, tsLength));
            if ((logTs - startTs) >= 0) {
                if (lineStr.find(target) != string::npos) {
                    ret = true;
                }
            }
        }
        lineStr.clear();
    }
    logFile.close();
    return ret;
}

double GetNowTs(void)
{
    const double transUsecNum = 1000000.0;
    timeval tv = {0};
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec + tv.tv_usec / transUsecNum);
}

char *ParseSysCmdResult(FILE &result, int32_t line, int32_t word)
{
    char s[1024];
    char *pch = nullptr;
    int32_t lineCnt = 1;
    int32_t wordCnt = 1;
    while (1) {
        if (fgets(s, sizeof(s), &result) == nullptr) {
            break;
        }
        pch = strtok(s, " ");
        while (pch != nullptr) {
            if (lineCnt == line && wordCnt == word) {
                return pch;
            }
            pch = strtok(nullptr, " ");
            wordCnt++;
        }
        lineCnt++;
        wordCnt = 1;
    }
    return pch;
}

static void ParseFile(char *pch, struct ParseProcInfo &Pinfo)
{
    while (pch != nullptr) {
        if (strstr(pch, "VmRSS")) {
            pch = strtok(nullptr, " ");
            Pinfo.ramCur = atof(pch);
            Pinfo.ramCount += 1;
            Pinfo.ramTotal += Pinfo.ramCur;
            if (Pinfo.ramCur > Pinfo.ramPeak) {
                Pinfo.ramPeak = Pinfo.ramCur;
            }
            break;
        }
        if (strstr(pch, "Cpu")) {
            pch = strtok(nullptr, " ");
            Pinfo.cpuCur = atof(pch);
            Pinfo.cpuCount += 1;
            Pinfo.cpuTotal += Pinfo.cpuCur;
            if (Pinfo.cpuCur > Pinfo.cpuPeak) {
                Pinfo.cpuPeak = Pinfo.cpuCur;
            }
            break;
        }
        if (strstr(pch, "Threads")) {
            pch = strtok(nullptr, " ");
            Pinfo.threadCur = atoi(pch);
            if (Pinfo.threadCur > Pinfo.threadPeak) {
                Pinfo.threadPeak = Pinfo.threadCur;
            }
            break;
        }
        pch = strtok(nullptr, " ");
    }
}

void CalcProcInfoFromFile(struct ProcInfo &info, const string &file)
{
    char s[100];
    char *pch;
    struct ParseProcInfo Pinfo = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0};
    FILE *f = fopen(file.c_str(), "r");
    while (1) {
        if (fgets(s, sizeof(s), f) == nullptr) {
            break;
        }
        pch = strtok(s, " \t");
        ParseFile(pch, Pinfo);
    }

    if (Pinfo.ramCount == 0 || Pinfo.cpuCount == 0) {
        fclose(f);
        return;
    }
    info.ramPeak = Pinfo.ramPeak;
    info.ramAvg = Pinfo.ramTotal / Pinfo.ramCount;
    info.cpuPeak = Pinfo.cpuPeak;
    info.cpuAvg = Pinfo.cpuTotal / Pinfo.cpuCount;
    info.threadPeak = Pinfo.threadPeak;

    if (fclose(f)) {
        return;
    }
}

double GetTsFromLog(const string &target, double startTs, const string &file)
{
    double logTs;
    ifstream logFile(file);
    string lineStr;
    const int32_t tsStartPos = 11;
    const int32_t tsLength = 17;
    while (getline(logFile, lineStr)) {
        if (lineStr.find(target) != string::npos) {
            logTs = stod(lineStr.substr(tsStartPos, tsLength));
            if ((logTs - startTs) >= 0) {
                return logTs;
            }
        }
        lineStr.clear();
    }
    logFile.close();
    return 0;
}
