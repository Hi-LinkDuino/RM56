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

#ifndef OHOS_ACELITE_NUMBER_FORMAT_MODEL_H
#define OHOS_ACELITE_NUMBER_FORMAT_MODEL_H

#include "acelite_config.h"
#if (FEATURE_NUMBER_FORMAT == 1)
#include "gfx_utils/list.h"
#include "js_fwk_common.h"
#include "number_format.h"
#include "types.h"
namespace OHOS {
namespace ACELite {
class NumberFormatModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY(NumberFormatModule);
    /**
     * @brief register number format method and numberFormat object
     */
    static void Init(jerry_value_t intlHandle);

    /**
     * @brief delete the date format object
     * @param the pointer need to be delete
     */
    static void DeleteNumberFormat(void *nativePointer);

    // the handle to release the native value when the js value number format object is not needed.
    static constexpr jerry_object_native_info_t gcCallback = {.free_cb = DeleteNumberFormat};

private:
    NumberFormatModule() : isUsingGroup_(true),
                           formatType_(I18N::NumberFormatType::DECIMAL),
                           formatter_(nullptr),
                           localeInfo_(nullptr) {
    }

    ~NumberFormatModule()
    {
        if (formatter_ != nullptr) {
            delete formatter_;
            formatter_ = nullptr;
        }
        if (localeInfo_ != nullptr) {
            delete localeInfo_;
            localeInfo_ = nullptr;
        }
    }
    static jerry_value_t CreateNumberFormat(const jerry_value_t func,
                                            const jerry_value_t context,
                                            const jerry_value_t args[],
                                            const jerry_length_t argsNum);
    static jerry_value_t Format(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static const char *SetStyle(NumberFormatModule &numModel,
                         const jerry_value_t key,
                         const jerry_value_t style);

    void SetUsingGroup(jerry_value_t styleValue);

    bool SetMaximumFractionDigits(jerry_value_t styleValue);

    bool SetNumberStyle(jerry_value_t styleValue);

    bool isUsingGroup_;
    I18N::NumberFormatType formatType_;
    I18N::NumberFormat *formatter_;
    I18N::LocaleInfo *localeInfo_;
};
}
}
#endif // FEATURE_NUMBER_FORAMT

#endif // NUMBER_FORMAT_MODEL_H
