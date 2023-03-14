/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HFP_HF_GAP_CLIENT_H
#define HFP_HF_GAP_CLIENT_H

#include <cstdint>

#include "base_def.h"
#include "btstack.h"
#include "gap_if.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for HFP HF GAP client.
 */
class HfpHfGapClient {
public:
    /**
     * @brief Construct a new HfpHfGapClient object.
     */
    HfpHfGapClient() = default;

    /**
     * @brief Destroy the HfpHfGapClient object.
     */
    ~HfpHfGapClient() = default;

    /**
     * @brief Register Service Security.
     *
     * @param address Remote device address.
     * @param scn Server channel number.
     * @return Returns the error code of register service security.
     */
    int RegisterServiceSecurity(const BtAddr &address, uint8_t scn);

    /**
     * @brief Deregister Service Security
     *
     * @param address Remote device address.
     * @return Returns the error code of deregister service security.
     */
    int DeregisterServiceSecurity(const BtAddr &address) const;

private:
    GapServiceSecurityInfo securityInfo_;
    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfGapClient);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_GAP_CLIENT_H
