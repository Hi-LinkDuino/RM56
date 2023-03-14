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

#ifndef UTILS_BUNDLE_LITE_AAFWK_EVENT_ERROR_CODE_H
#define UTILS_BUNDLE_LITE_AAFWK_EVENT_ERROR_CODE_H

#include "memory_heap.h"
#include "product_adapter.h"
#include "aafwk_event_error_id.h"

namespace OHOS {
namespace ACELite {
#define APP_EVENT(code1) \
    AafwkEventCodePrint::GetInstance()->AafwkEventPrint(code1, 0)
#define APP_ERRCODE_EXTRA(code1, code2) \
    AafwkEventCodePrint::GetInstance()->AafwkErrorPrint(code1, code2)
 
class AafwkEventCodePrint final : public MemoryHeap {
public:
    AafwkEventCodePrint() = default;
    ~AafwkEventCodePrint() = default;

    static AafwkEventCodePrint *GetInstance()
    {
        static AafwkEventCodePrint printInstance;
        return &printInstance;
    }

    void AafwkEventPrint(uint8_t info2, uint8_t info3)
    {
        ProductAdapter::PrintEventTrace(0, info2, info3);
    }

    void AafwkEventPrint(uint8_t info1, uint8_t info2, uint8_t info3)
    {
        ProductAdapter::PrintEventTrace(info1, info2, info3);
    }

    void AafwkErrorPrint(uint8_t info1, uint16_t info2)
    {
        ProductAdapter::PrintErrCode(info1, info2);
    }
};
}
}
#endif