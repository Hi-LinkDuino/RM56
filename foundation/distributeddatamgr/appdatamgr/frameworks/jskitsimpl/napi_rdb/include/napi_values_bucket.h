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

#ifndef APPDATAMGR_NAPI_VALUES_BUCKET_H
#define APPDATAMGR_NAPI_VALUES_BUCKET_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "values_bucket.h"

EXTERN_C_START
__attribute__((visibility("default"))) napi_value NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_NewInstance(
    napi_env env, OHOS::NativeRdb::ValuesBucket &valuesBucket);
__attribute__((visibility("default"))) OHOS::NativeRdb::ValuesBucket *
NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_GetNativeObject(napi_env env, napi_value &value);
EXTERN_C_END
#endif // APPDATAMGR_NAPI_VALUES_BUCKET_H
