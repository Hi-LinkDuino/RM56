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

#ifndef PBAP_PSE_GAP_H
#define PBAP_PSE_GAP_H

#include <cstdint>

namespace OHOS {
namespace bluetooth {
class PbapPseGap {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param rfcommScn rfcomm channel
     * @param l2capPsm l2cat psm
     * @return
     */
    PbapPseGap(uint8_t rfcommScn, uint16_t l2capPsm);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~PbapPseGap() = default;

    /**
     * @brief register gap
     * @details register gap service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Register() const;

    /**
     * @brief deregister gap
     * @details deregister gap service
     */
    void Deregister() const;

private:
    uint8_t rfcommScn_ = 0;  // rfcomm channel
    uint16_t l2capPsm_ = 0;  // l2cap psm
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_GAP_H
