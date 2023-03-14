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

/**
 * @file avrcp_ct_gap.h
 *
 * @brief Declares the class of the Generic Access Profile, including attributes and methods.
 */

#ifndef AVRCP_CT_GAP_H
#define AVRCP_CT_GAP_H

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods for registering/unregistering the security requirements into the GAP.
 * @see Audio/Video Remote Control 1.6.2 Section 12 -> 12.2 Security aspects.
 */

class AvrcCtGapManager {
public:
    /**
     * @brief A constructor used to create an <b>AvrcCtGapManager</b> instance.
     */
    AvrcCtGapManager();

    /**
     * @brief A destructor used to delete the <b>AvrcCtGapManager</b> instance.
     */
    ~AvrcCtGapManager();

    /**
     * @brief Registers security requirements into the GAP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    static int RegisterSecurity(void);

    /**
     * @brief Unregisters security requirements from the GAP.
     *
     * @return The result of the method execution.
     * @retval RET_NO_ERROR   Execute success.
     * @retval RET_BAD_STATUS Execute failure.
     */
    static int UnregisterSecurity(void);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_CT_GAP_H
