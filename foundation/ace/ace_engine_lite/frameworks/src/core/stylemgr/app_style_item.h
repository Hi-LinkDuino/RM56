/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_APP_STYLE_ITEM_H
#define OHOS_ACELITE_APP_STYLE_ITEM_H

#include <stdlib.h>
#include <string.h>

#include "js_fwk_common.h"
#include "key_parser.h"
#include "non_copyable.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
constexpr char PSEUDO_CLASS_TYPE_ACTIVE[] = ":active";
constexpr char PSEUDO_CLASS_TYPE_CHECKED[] = ":checked";

enum {
    STYLE_PROP_VALUE_TYPE_UNKNOWN = 0x00,
    STYLE_PROP_VALUE_TYPE_STRING = 0x01,
    STYLE_PROP_VALUE_TYPE_NUMBER = 0x02,
    STYLE_PROP_VALUE_TYPE_BOOL = 0x03,
    STYLE_PROP_VALUE_TYPE_FLOATING = 0x04,
    STYLE_PROP_VALUE_TYPE_PERCENT = 0x05,
};

union StyleValue {
    double floating;
    int32_t number;
    float percent;
    char *string;
    bool boolean;
};

/**
 * pseudo class support
 */
enum {
    PSEUDO_CLASS_UNKNOWN = 0x00, // please add new type above
    PSEUDO_CLASS_ACTIVE = 0x01,  // :active
    PSEUDO_CLASS_CHECKED = 0x02  // :checked
};

/**
 * style flag
 * former part for odd, latter part for even
 */
typedef struct U8Bits : public MemoryHeap {
    bool standardOdd_ : 1;
    bool pseudoActiveOdd_ : 1;
    bool pseudoCheckedOdd_ : 1;
    bool pseudoUnknownOdd_ : 1;
    bool standardEven_ : 1;
    bool pseudoActiveEven_ : 1;
    bool pseudoCheckedEven_ : 1;
    bool pseudoUnknownEven_ : 1;
} U8BitsType;

typedef union StyleFlag {
    U8BitsType u8Bits;
    uint8_t u8Byte;
} StyleFlagType;

class AppStyleItem final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(AppStyleItem);
    AppStyleItem()
        : pre_(nullptr),
          next_(nullptr),
          styleValue_({ 0.0 }),
          propNameId_(0),
          valueType_(STYLE_PROP_VALUE_TYPE_UNKNOWN),
          pseudoClassType_(PSEUDO_CLASS_UNKNOWN)
    {
    }

    ~AppStyleItem()
    {
        if (valueType_ == STYLE_PROP_VALUE_TYPE_STRING) {
            ACE_FREE(styleValue_.string);
        }
    }

    uint16_t GetPropNameId() const
    {
        return propNameId_;
    }

    uint8_t GetValueType() const
    {
        return valueType_;
    }

    const char *GetStrValue() const
    {
        return (valueType_ != STYLE_PROP_VALUE_TYPE_STRING || GetStrValueLen() == 0) ? nullptr : styleValue_.string;
    }

    size_t GetStrValueLen() const
    {
        return (styleValue_.string != nullptr) ? strlen(styleValue_.string) : 0;
    }

    int32_t GetNumValue() const
    {
        return (valueType_ != STYLE_PROP_VALUE_TYPE_NUMBER) ? 0 : styleValue_.number;
    }

    bool GetBoolValue() const
    {
        return (valueType_ != STYLE_PROP_VALUE_TYPE_BOOL) ? false : styleValue_.boolean;
    }

    double GetFloatingValue() const
    {
        return (valueType_ != STYLE_PROP_VALUE_TYPE_FLOATING) ? false : styleValue_.floating;
    }

    float GetPercentValue() const
    {
        return (valueType_ != STYLE_PROP_VALUE_TYPE_PERCENT) ? 0 : styleValue_.percent;
    }

    const AppStyleItem *GetNext() const
    {
        return next_;
    }

    void SetNext(AppStyleItem *nextStyle)
    {
        next_ = nextStyle;
    }

    void SetPre(AppStyleItem *preStyle)
    {
        pre_ = preStyle;
    }

    const AppStyleItem *GetPre() const
    {
        return pre_;
    }

    bool IsPseudoClassItem() const
    {
        return (pseudoClassType_ != PSEUDO_CLASS_UNKNOWN);
    }

    uint8_t GetPseudoClassType() const
    {
        return pseudoClassType_;
    }

    bool UpdateNumValToStr();
    void UpdateValueFrom(const AppStyleItem &from);
    static AppStyleItem *GenerateFromJSValue(jerry_value_t stylePropName, jerry_value_t stylePropValue);
    static AppStyleItem *CreateStyleItem(uint16_t keyId,
                                         const jerry_value_t stylePropValue,
                                         uint8_t pseudoClassType = PSEUDO_CLASS_UNKNOWN);
    // copy constructor can not return null, so define a static method instead
    static AppStyleItem *CopyFrom(const AppStyleItem *from);
    /**
     * @brief Check if the given style is one pseudo class type, if it is, the corrensponding pseudo class enum
     *        will be calculated and returned, the style name also will be cutted to the key without the pseudo
     *        class name, the new key length also will be given out.
     *
     * @param[in] styleKey: the target style name to check
     * @param[in] keyLength: the target style name length
     * @param[out] keyLength: the new style name length
     *
     * @return unknown or the corrensponding pseudo class type
     */
    static uint8_t EstimatePseudoClassType(const char * const styleKey, uint16_t *keyLength);

private:
    /**
     * @author yongxianglai@bestechnic.com
     * @brief 解析自定义 keyId
     * 
     * @param keyId 
     * @param stylePropValue 
     * @param pseudoClassType 
     * @return true 已处理
     * @return false 未处理
     */
    static bool ProcessPrivateKeyId(uint16_t keyId, const jerry_value_t stylePropValue, AppStyleItem *newStyleItem);

    void SetNumValue(int32_t value)
    {
        valueType_ = STYLE_PROP_VALUE_TYPE_NUMBER;
        styleValue_.number = value;
    }

    void SetBoolValue(bool value)
    {
        valueType_ = STYLE_PROP_VALUE_TYPE_BOOL;
        styleValue_.boolean = value;
    }

    void SetFloatingValue(double value)
    {
        valueType_ = STYLE_PROP_VALUE_TYPE_FLOATING;
        styleValue_.floating = value;
    }

    void SetPercentValue(float value)
    {
        valueType_ = STYLE_PROP_VALUE_TYPE_PERCENT;
        styleValue_.percent = value;
    }

    void SetStringValue(const char * const value);

    AppStyleItem *pre_;
    AppStyleItem *next_;

    StyleValue styleValue_;
    uint16_t propNameId_;
    uint8_t valueType_;
    uint8_t pseudoClassType_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_APP_STYLE_ITEM_H
