/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HIVIEWDFX_HILOG_CPP_H
#define HIVIEWDFX_HILOG_CPP_H

#include "hiview_log.h"

#ifdef __cplusplus

namespace OHOS {
namespace HiviewDFX {
struct HiLogLabel {
    LogType type;
    unsigned int domain;
    const char *tag;
};

class HiLog final {
public:
    /**
    * @brief Outputs debug logs.
    *
    * @param label Indicates the log label.
    * @param fmt Indicates the format string, which is an enhancement of a printf format string and supports the
    * privacy identifier. Specifically, {public} or {private} is added between the % character and the format specifier
    * in each parameter. \n
    * @param... Indicates a list of parameters. The number and type of parameters must map onto the format specifiers
    * in the format string.
    * @return Returns <b>0</b> or a larger value if the operation is successful; returns a value smaller than <b>0</b>
    * otherwise.
    * @since 1.1
    * @version 1.0
    */
    static int Debug(const HiLogLabel &label, const char *fmt, ...) __attribute__((__format__(printf, 2, 3)));
    /**
    * @brief Outputs informational logs.
    *
    * @param label Indicates the log label.
    * @param fmt Indicates the format string, which is an enhancement of a printf format string and supports the
    * privacy identifier. Specifically, {public} or {private} is added between the % character and the format specifier
    * in each parameter. \n
    * @param... Indicates a list of parameters. The number and type of parameters must map onto the format specifiers
    * in the format string.
    * @return Returns <b>0</b> or a larger value if the operation is successful; returns a value smaller than <b>0</b>
    * otherwise.
    * @since 1.1
    * @version 1.0
    */
    static int Info(const HiLogLabel &label, const char *fmt, ...) __attribute__((__format__(printf, 2, 3)));
    /**
    * @brief Outputs warning logs.
    *
    * @param label Indicates the log label.
    * @param fmt Indicates the format string, which is an enhancement of a printf format string and supports the
    * privacy identifier. Specifically, {public} or {private} is added between the % character and the format specifier
    * in each parameter. \n
    * @param... Indicates a list of parameters. The number and type of parameters must map onto the format specifiers
    * in the format string.
    * @return Returns <b>0</b> or a larger value if the operation is successful; returns a value smaller than <b>0</b>
    * otherwise.
    * @since 1.1
    * @version 1.0
    */
    static int Warn(const HiLogLabel &label, const char *fmt, ...) __attribute__((__format__(printf, 2, 3)));
    /**
    * @brief Outputs error logs.
    *
    * @param label Indicates the log label.
    * @param fmt Indicates the format string, which is an enhancement of a printf format string and supports the
    * privacy identifier. Specifically, {public} or {private} is added between the % character and the format specifier
    * in each parameter. \n
    * @param... Indicates a list of parameters. The number and type of parameters must map onto the format specifiers
    * in the format string.
    * @return Returns <b>0</b> or a larger value if the operation is successful; returns a value smaller than <b>0</b>
    * otherwise.
    * @since 1.1
    * @version 1.0
    */
    static int Error(const HiLogLabel &label, const char *fmt, ...) __attribute__((__format__(printf, 2, 3)));
    /**
    * @brief Outputs fatal logs.
    *
    * @param label Indicates the log label.
    * @param fmt Indicates the format string, which is an enhancement of a printf format string and supports the
    * privacy identifier. Specifically, {public} or {private} is added between the % character and the format specifier
    * in each parameter. \n
    * @param... Indicates a list of parameters. The number and type of parameters must map onto the format specifiers
    * in the format string.
    * @return Returns <b>0</b> or a larger value if the operation is successful; returns a value smaller than <b>0</b>
    * otherwise.
    * @since 1.1
    * @version 1.0
    */
    static int Fatal(const HiLogLabel &label, const char *fmt, ...) __attribute__((__format__(printf, 2, 3)));
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // __cplusplus

#endif // HIVIEWDFX_HILOG_CPP_H
