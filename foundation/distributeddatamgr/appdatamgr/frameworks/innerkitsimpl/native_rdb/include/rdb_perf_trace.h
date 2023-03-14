/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NATIVE_RDB_PERF_TRACE_H
#define NATIVE_RDB_PERF_TRACE_H

#include "bytrace.h"

namespace OHOS::NativeRdb {
#define RDB_TRACE_BEGIN(name) StartTrace(BYTRACE_TAG_DISTRIBUTEDDATA, (name))
#define RDB_TRACE_END() FinishTrace(BYTRACE_TAG_DISTRIBUTEDDATA)
}
#endif
