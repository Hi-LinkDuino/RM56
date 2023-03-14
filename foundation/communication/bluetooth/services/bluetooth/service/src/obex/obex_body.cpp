/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "obex_body.h"
#include <cstring>
#include <memory>
#include "log.h"
#include "obex_types.h"

namespace OHOS {
namespace bluetooth {
ObexArrayBodyObject::ObexArrayBodyObject(const uint8_t *buf, size_t bufLen)
{
    auto ret = PrivateWrite(buf, bufLen);
    if (ret == 0) {
        OBEX_LOG_ERROR("%{public}s, ret=%zu", __PRETTY_FUNCTION__, ret);
    }
}

size_t ObexArrayBodyObject::Read(uint8_t *buf, size_t bufLen)
{
    std::lock_guard<std::mutex> lock(mutex_);
    size_t readSize = bufLen;
    size_t remainSize = body_.size() - index_;
    if (remainSize < readSize) {
        readSize = remainSize;
    }
    for (size_t i = 0; i < readSize; i++) {
        buf[i] = body_[index_++];
    }
    OBEX_LOG_DEBUG("ObexArrayBodyObject::Read: %zu / %zu", index_, body_.size());
    return readSize;
}

size_t ObexArrayBodyObject::Write(const uint8_t *buf, size_t bufLen)
{
    return PrivateWrite(buf, bufLen);
}

size_t ObexArrayBodyObject::PrivateWrite(const uint8_t *buf, size_t bufLen)
{
    std::lock_guard<std::mutex> lock(mutex_);
    body_.insert(body_.end(), buf, buf + bufLen);
    return bufLen;
}

int ObexArrayBodyObject::Close()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return 0;
}
}  // namespace bluetooth
}  // namespace OHOS