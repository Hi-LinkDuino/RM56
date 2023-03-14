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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_IMPL_OHOS_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_IMPL_OHOS_H

#include <memory>

#include "refbase.h"

#include "faultlog_info.h"
#include "faultlog_query_result_proxy.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLogQueryResultImpl {
public:
    explicit FaultLogQueryResultImpl(sptr<FaultLogQueryResultProxy> impl) : impl_(impl){};
    virtual ~FaultLogQueryResultImpl();

    std::unique_ptr<FaultLogInfo> GetNext();
    bool HasNext();

private:
    sptr<FaultLogQueryResultProxy> impl_ = nullptr;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULT_QUERY_RESULT_IMPL_OHOS_H