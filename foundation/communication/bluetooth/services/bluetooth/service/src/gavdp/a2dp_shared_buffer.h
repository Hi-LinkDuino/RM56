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
#ifndef A2DP_SHARED_BUFFER_H
#define A2DP_SHARED_BUFFER_H
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include "a2dp_encoder_sbc.h"

namespace OHOS {
namespace bluetooth {
class A2dpSharedBuffer {
public:
    A2dpSharedBuffer();
    uint32_t Read(uint8_t *buf, uint32_t len);
    uint32_t Write(const uint8_t *buf, uint32_t len);
    void Reset();
    void SetValid(bool isValid);
    ~A2dpSharedBuffer();
private:
    uint32_t cap_;
    uint8_t buf_[A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE] = {};
    uint8_t shiftBuf_[A2DP_SBC_MAX_PACKET_SIZE * FRAME_THREE] = {};
    uint32_t size_;
    std::mutex mutex_;
    bool isValid_ = false;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_SHARED_BUFFER_H