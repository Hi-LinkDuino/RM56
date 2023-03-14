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
#include "faultlog_query_result.h"

#include <memory>

#include "faultlog_info.h"
#include "faultlog_query_result_impl.h"

namespace OHOS {
namespace HiviewDFX {
FaultLogQueryResult::~FaultLogQueryResult()
{
    if (impl_ != nullptr) {
        delete impl_;
        impl_ = nullptr;
    }
}

std::unique_ptr<FaultLogInfo> FaultLogQueryResult::Next()
{
    return impl_->GetNext();
}

bool FaultLogQueryResult::HasNext()
{
    return impl_->HasNext();
}
}  // namespace HiviewDFX
}  // namespace OHOS