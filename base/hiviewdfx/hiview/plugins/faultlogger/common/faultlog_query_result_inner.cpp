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
#include "faultlog_query_result_inner.h"

#include <memory>
#include <string>

#include "faultlog_info.h"
#include "faultlogger_plugin.h"
#include "file_util.h"

namespace OHOS {
namespace HiviewDFX {
std::unique_ptr<FaultLogInfo> FaultLogQueryResultInner::GetNext()
{
    if (iter_ != logList_.end()) {
        auto ret = std::make_unique<FaultLogInfo>(*iter_);
        ++iter_;
        return ret;
    }
    return nullptr;
}

bool FaultLogQueryResultInner::HasNext()
{
    return iter_ != logList_.end();
}
}  // namespace HiviewDFX
}  // namespace OHOS
