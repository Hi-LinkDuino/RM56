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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_INNER_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_INNER_H
#include <list>
#include <memory>
#include <string>

#include "faultlog_info.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLogQueryResultInner {
public:
    explicit FaultLogQueryResultInner(const std::list<FaultLogInfo> &faultLogList) : logList_(faultLogList)
    {
        iter_ = logList_.begin();
    };

    ~FaultLogQueryResultInner(){};

    // Get the next available fault log info
    // if no log is available, nullptr will be returned
    std::unique_ptr<FaultLogInfo> GetNext();

    // check whether we have next available fault log info
    bool HasNext();

private:
    std::list<FaultLogInfo> logList_;
    std::list<FaultLogInfo>::iterator iter_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_INNER_H