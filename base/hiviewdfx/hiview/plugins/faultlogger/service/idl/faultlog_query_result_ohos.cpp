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
#include "faultlog_query_result_ohos.h"

#include <fcntl.h>
#include <sys/stat.h>

#include "file_util.h"

#include "faultlog_info_ohos.h"
#include "faultlog_query_result_inner.h"

namespace OHOS {
namespace HiviewDFX {
sptr<FaultLogInfoOhos> FaultLogQueryResultOhos::GetNext()
{
    if (result_ == nullptr) {
        return nullptr;
    }

    sptr<FaultLogInfoOhos> ret;
    auto info = result_->GetNext();
    if (info != nullptr) {
        ret = new FaultLogInfoOhos();
        ret->time = info->time;
        ret->uid = info->id;
        ret->pid = info->pid;
        ret->faultLogType = info->faultLogType;
        ret->module = info->module;
        ret->summary = info->summary;
        ret->reason = info->reason;
        ret->logPath = info->logPath;
        ret->sectionMaps = info->sectionMap;
        std::string realPath;
        if ((!info->logPath.empty()) && OHOS::HiviewDFX::FileUtil::PathToRealPath(info->logPath, realPath)) {
            ret->fd = open(realPath.c_str(), O_RDONLY);
        } else {
            ret->fd = -1;
        }
    }
    return ret;
}

bool FaultLogQueryResultOhos::HasNext()
{
    if (result_ == nullptr) {
        return false;
    }

    return result_->HasNext();
}
}  // namespace HiviewDFX
} // namespace OHOS