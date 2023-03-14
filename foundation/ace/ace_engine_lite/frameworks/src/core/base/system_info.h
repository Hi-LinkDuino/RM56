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

#ifndef OHOS_ACELITE_BASE_SYSTEM_INFO_H
#define OHOS_ACELITE_BASE_SYSTEM_INFO_H

#include "acelite_config.h"
#include "memory_heap.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class SystemInfo final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SystemInfo);
    static SystemInfo &GetInstance();
    void Initialize();

    /**
     * @brief get the value of width/height
     * @return the width/height value
     */
    float GetAspectRatio() const;

    /**
     * @brief get the device type, support liteWearable and smartVision now
     * @return the device type
     */
    const char *GetDeviceType() const;

    /**
     * @brief charge the device is round screen or not
     * @return the device is round scrren or not
     */
    bool IsRoundScreen() const;

    uint16_t GetScreenHeight() const;

    uint16_t GetScreenWidth() const;

private:
    SystemInfo() : screenWidth_(0), screenHeight_(0), aspectRatio_(0.0), deviceType_(nullptr), isRoundScreen_(false) {}
    ~SystemInfo() {}
    static const uint8_t DEVICE_TYPE_STR_LEN = 32;
    uint16_t screenWidth_;
    uint16_t screenHeight_;
    float aspectRatio_;
    const char *deviceType_;
    bool isRoundScreen_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_BASE_SYSTEM_INFO_H
