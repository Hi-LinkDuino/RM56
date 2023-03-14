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

#ifndef HILOG_MSG_WRAPPER_H
#define HILOG_MSG_WRAPPER_H

#include <cstring>
#include <iostream>
#include <securec.h>

#include <hilog/log.h>
#include "hilogtool_msg.h"

namespace OHOS {
namespace HiviewDFX {

class HilogMsgWrapper {
public:
    explicit HilogMsgWrapper(const std::vector<char> & _msgBuffer) : msgBuffer(_msgBuffer)
    {}
    explicit HilogMsgWrapper(std::vector<char> && _msgBuffer)
    {
        std::swap(msgBuffer, _msgBuffer);
    }
    HilogMsg& GetHilogMsg()
    {
        return *reinterpret_cast<HilogMsg*>(msgBuffer.data());
    }

    const std::vector<char> & GetRawData() const
    {
        return msgBuffer;
    }

private:
    std::vector<char> msgBuffer;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* HILOG_MSG_WRAPPER_H */
