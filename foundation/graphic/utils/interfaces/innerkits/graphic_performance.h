/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_GRAPHIC_PERFORMANCE_H
#define GRAPHIC_LITE_GRAPHIC_PERFORMANCE_H

#include "graphic_config.h"
#if ENABLE_DEBUG && ENABLE_DEBUG_PERFORMANCE_TRACE
#include <limits>
#include <map>
#include <signal.h>
#include <string>
#include <stdio.h>
#include <sys/time.h>

namespace OHOS {
static const char* perfTitle[5] = {"name", "times", "minCost", "maxCost", "avgCost"};
struct PerformanceItem {
    int32_t times;
    int32_t minCost;
    int32_t maxCost;
    double avgCost;
    int32_t warmup;
};

class Performance {
public:
    static Performance* GetInstance();

    static void SigUsr(int signo)
    {
        if (signo == SIGUSR1) {
            Performance::GetInstance()->SetPrintFlag();
        }
    }

    void RegisterSIGUSR1()
    {
        if (signal(SIGUSR1, SigUsr) == SIG_ERR) {
            printf("signal error!\n");
        }
    }

    void UpdateItem(std::string name, int32_t cost, int32_t warmup)
    {
        if (mapPerfItem.find(name) == mapPerfItem.end()) {
            PerformanceItem item = { 0 };
            item.times = 1;
            item.minCost = INT32_MAX;
            item.warmup = warmup;
            mapPerfItem[name] = item;
        } else {
            PerformanceItem& item = mapPerfItem[name];
            item.times++;
        }
        PerformanceItem& item = mapPerfItem[name];
        if (item.times > warmup) {
            item.minCost = (item.minCost > cost) ? cost : item.minCost;
            item.maxCost = (item.maxCost < cost) ? cost : item.maxCost;
            item.avgCost = item.avgCost + 1.0 * (cost - item.avgCost) / (item.times - warmup);
        }
    }

    void SetPrintFlag()
    {
        needPrint = true;
    }

    void PrintResult()
    {
        if (!needPrint) {
            return;
        }
        needPrint = false;
        /*
         * perfTitle[0]: "name"
         * perfTitle[1]: "times"
         * perfTitle[2]: "minCost"
         * perfTitle[3]: "maxCost"
         * perfTitle[4]: "avgCost"
         */
        printf("\n%-40s%-20s%-20s%-20s%-20s\n", perfTitle[0], perfTitle[1], perfTitle[2], perfTitle[3], perfTitle[4]);
        for (auto iter = mapPerfItem.begin(); iter != mapPerfItem.end(); iter++) {
            if (iter->second.times - iter->second.warmup > 0) {
                printf("%-40s%-20d%-20d%-20d%-20.2lf\n",
                       iter->first.c_str(),
                       iter->second.times - iter->second.warmup,
                       iter->second.minCost,
                       iter->second.maxCost,
                       iter->second.avgCost);
            }
        }
    }

private:
    Performance() {}
    ~Performance() {}

    std::map<std::string, PerformanceItem> mapPerfItem;
    bool needPrint = false;
};

class PerformanceTool {
public:
    explicit PerformanceTool(const char* name) : name_(name)
    {
        gettimeofday(&tm_, NULL);
    }

    PerformanceTool(const char* name, int32_t warmup) : name_(name), warmUp_(warmup)
    {
        gettimeofday(&tm_, NULL);
    }

    ~PerformanceTool()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        // 1000000: 1,000,000 microseconds per second
        int32_t cost = (now.tv_sec - tm_.tv_sec) * 1000000 + (now.tv_usec - tm_.tv_usec);
        Performance::GetInstance()->UpdateItem(name_, cost, warmUp_);
    }
private:
    std::string name_;
    int32_t warmUp_ = 0;
    struct timeval tm_;
};
} // namespace OHOS
#define DEBUG_PERFORMANCE_TRACE_AUTO()             OHOS::PerformanceTool __tmp__(__FUNCTION__)
#define DEBUG_PERFORMANCE_TRACE_WARMUP(x, warmup)  OHOS::PerformanceTool __tmp__(x, warmup)
#define DEBUG_PERFORMANCE_TRACE(x)                 OHOS::PerformanceTool __tmp__(x)
#define DEBUG_PERFORMANCE_REGISTER_SIG()           OHOS::Performance::GetInstance()->RegisterSIGUSR1()
#define DEBUG_PERFORMANCE_PRINT_RESULT()           OHOS::Performance::GetInstance()->PrintResult()
#else
#define DEBUG_PERFORMANCE_TRACE_AUTO()
#define DEBUG_PERFORMANCE_TRACE_WARMUP(x, warmup)
#define DEBUG_PERFORMANCE_TRACE(x)
#define DEBUG_PERFORMANCE_REGISTER_SIG()
#define DEBUG_PERFORMANCE_PRINT_RESULT()
#endif
#endif
