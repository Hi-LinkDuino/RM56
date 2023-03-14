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

#ifndef HFP_HF_CALL_MANAGER_H
#define HFP_HF_CALL_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include "hands_free_unit_calls.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods that is used to process the current calls.
 */
class HfpHfCallManager {
public:
    /**
     * @brief Construct a new HfpHfCallManager object.
     *
     * @param address Remote device address.
     */
    explicit HfpHfCallManager(const std::string &address);

    /**
     * @brief Destroy the HfpHfCallManager object.
     */
    ~HfpHfCallManager();

    /**
     * @brief Get the call by the id.
     *
     * @param id The call id.
     * @return Returns the pointer of the call.
     */
    HandsFreeUnitCalls *GetCallById(int id);

    /**
     * @brief Get the call by the state.
     *
     * @param state : The call state.
     * @return Returns the pointer of the call.
     */
    HandsFreeUnitCalls *GetCallByState(int state);

    /**
     * @brief Get the current calls.
     *
     * @return Returns the current calls.
     */
    std::vector<HandsFreeUnitCalls> GetCurrentCalls();

    /**
     * @brief Update the call.
     *
     * @param call The call.
     */
    void UpdateCall(HandsFreeUnitCalls &call);

    /**
     * @brief Complete call update.
     */
    void UpdateCallDone();

    /**
     * @brief Get the Call Number By the state.
     *
     * @param state The call state.
     * @return Returns the call number.
     */
    int GetCallNumberByState(int state);

    /**
     * @brief Notify the call change.
     *
     * @param call The call.
     */
    void NotifyCallChanged(const HandsFreeUnitCalls &call);

private:
    // The device address.
    std::string address_;
    // The current calls.
    std::map<int, HandsFreeUnitCalls> calls_ {};
    // The update calls.
    std::map<int, HandsFreeUnitCalls> updateCalls_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_CALL_MANAGER_H