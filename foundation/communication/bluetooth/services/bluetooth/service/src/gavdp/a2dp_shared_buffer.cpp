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
#include "a2dp_shared_buffer.h"
#include "securec.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
A2dpSharedBuffer::A2dpSharedBuffer()
{
    std::lock_guard<std::mutex> lock_(mutex_);
    cap_ = A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE;
    size_ = 0;
    isValid_ = false;
}

A2dpSharedBuffer::~A2dpSharedBuffer()
{
}

uint32_t A2dpSharedBuffer::Read(uint8_t *buf, uint32_t len)
{
    std::lock_guard<std::mutex> lock_(mutex_);
    if (!isValid_) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: buffer not ready!", __func__);
        return 0;
    }
    LOG_INFO("[A2dpSharedBuffer] %{public}s: start size_ [%{public}u]  len[%{public}u] cap_ [%{public}u] \n",
        __func__, size_,  len, cap_);
    if (size_ < len) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: no sufficient data\n", __func__);
        return 0;
    }
    if (memcpy_s(buf, len, buf_, len) != EOK) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: memcpy_s failed\n", __func__);
    }
    size_ -= len;
    if (memcpy_s(shiftBuf_, cap_, buf_ + len, size_) != EOK) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: memcpy_s failed\n", __func__);
    }
    if (memcpy_s(buf_, cap_, shiftBuf_, size_) != EOK) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: memcpy_s failed\n", __func__);
    }
    LOG_INFO("[A2dpSharedBuffer] %{public}s: end size_ [%{public}u] len[%{public}u] cap_[%{public}u]\n",
        __func__, size_, len, cap_);
    return len;
}

uint32_t A2dpSharedBuffer::Write(const uint8_t *buf, uint32_t len)
{
    std::lock_guard<std::mutex> lock_(mutex_);
    if (!isValid_) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: buffer not ready!", __func__);
        return 0;
    }
    LOG_INFO("[A2dpSharedBuffer] %{public}s: start size_ [%{public}u] len[%{public}u] cap_[%{public}u]\n",
        __func__, size_, len, cap_);
    if (size_ + len > cap_) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: no space size_ [%{public}u] len[%{public}u] cap_[%{public}u]\n",
            __func__, size_, len, cap_);
        return 0;
    }
    if (memcpy_s(buf_ + size_, cap_ - size_, buf, len) != EOK) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: memcpy_s failed\n", __func__);
    }
    size_ += len;
    LOG_INFO("[A2dpSharedBuffer] %{public}s: end size_ [%{public}u] len[%{public}u] cap_[%{public}u]\n",
        __func__, size_, len, cap_);
    return len;
}

void A2dpSharedBuffer::Reset()
{
    std::lock_guard<std::mutex> lock_(mutex_);
    if (memset_s(buf_, cap_, 0, cap_) != EOK) {
        LOG_ERROR("[A2dpSharedBuffer] %{public}s: memset_s failed\n", __func__);
    }
    size_ = 0;
    isValid_ = false;
}

void A2dpSharedBuffer::SetValid(bool isValid)
{
    std::lock_guard<std::mutex> lock_(mutex_);
    LOG_ERROR("[A2dpSharedBuffer] %{public}s: isValid %{public}d\n", __func__, isValid);
    isValid_ = isValid;
}
}  // namespace bluetooth
}  // namespace OHOS