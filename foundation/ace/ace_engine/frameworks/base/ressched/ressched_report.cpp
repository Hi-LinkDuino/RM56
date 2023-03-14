/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "base/ressched/ressched_report.h"

namespace OHOS::Ace {
namespace {
    const uint32_t RES_TYPE_CLICK_RECOGNIZE = 9;
    const uint32_t RES_TYPE_PUSH_PAGE       = 10;
    const uint32_t RES_TYPE_SLIDE           = 11;
}

ResSchedReport& ResSchedReport::GetInstance()
{
    static ResSchedReport instance;
    return instance;
}

void ResSchedReport::ResSchedDataReport(const char* name)
{
    std::unordered_map<std::string, std::string> payload;
    payload["name"] = name;
    if (reportDataFunc_ == nullptr) {
        reportDataFunc_ = LoadReportDataFunc();
    }
    if (reportDataFunc_ != nullptr) {
        if (strcmp(name, "click") == 0) {
            reportDataFunc_(RES_TYPE_CLICK_RECOGNIZE, 0, payload);
        } else if (strcmp(name, "push_page") == 0) {
            reportDataFunc_(RES_TYPE_PUSH_PAGE, 0, payload);
        } else if (strcmp(name, "slide_on") == 0) {
            reportDataFunc_(RES_TYPE_SLIDE, 1, payload);
        } else if (strcmp(name, "slide_off") == 0) {
            reportDataFunc_(RES_TYPE_SLIDE, 0, payload);
        }
    }
}
} // namespace OHOS::Ace