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

#include "doc_error.h"
#include "doc_util.h"

namespace OHOS {
namespace HiviewDFX {
int MapErrorCode(iwrc rc)
{
    switch (rc) {
        case 0:
            return DOC_STORE_SUCCESS;
        case IWKV_ERROR_CORRUPTED:
            return DOC_STORE_ERROR_CORRUPTED;
        default:
            return DOC_STORE_ERROR_FAIL;
    }
}
} // HiviewDFX
} // OHOS