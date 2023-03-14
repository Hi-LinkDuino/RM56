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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PARCEL_MACRO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PARCEL_MACRO_H

#include "parcel.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
#define READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)         \
    do {                                                                 \
        if (!(parcel).Read##type(data)) {                                \
            APP_LOGE("fail to read %{public}s type from parcel", #type); \
            return false;                                                \
        }                                                                \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)         \
    do {                                                                  \
        if (!(parcel).Write##type(data)) {                                \
            APP_LOGE("fail to write %{public}s type into parcel", #type); \
            return false;                                                 \
        }                                                                 \
    } while (0)
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PARCEL_MACRO_H