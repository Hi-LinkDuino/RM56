/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_STRINGBUILDER_H
#define OHOS_HDI_STRINGBUILDER_H

#include "util/string.h"

namespace OHOS {
namespace HDI {
class StringBuilder {
public:
    ~StringBuilder();

    StringBuilder &Append(char c);

    StringBuilder &Append(const char *string);

    StringBuilder &Append(const String &string);

    StringBuilder &AppendFormat(const char *format, ...);

    String ToString() const;

private:
    bool Grow(size_t size);

    static const char *TAG;
    char *buffer_ = nullptr;
    size_t position_ = 0;
    size_t capacity_ = 0;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_STRINGBUILDER_H