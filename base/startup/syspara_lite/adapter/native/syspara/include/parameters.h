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

#ifndef SYSTEM_PARAMETERS_H
#define SYSTEM_PARAMETERS_H

#include <limits>
#include <string>

namespace OHOS {
namespace system {
/*
 * Returns the current value of the system parameter `key`.
 * If the parameter is empty or doesn't exist, `def` will be returned.
 */
std::string GetParameter(const std::string& key, const std::string& def);

/*
 * Returns true if the system parameter `key` has the value "1", "y", "yes", "on", or "true",
 * false for "0", "n", "no", "off", or "false", or `def` otherwise.
 */
bool GetBoolParameter(const std::string& key, bool def);

/*
 * Returns the signed integer corresponding to the system parameter `key`.
 * If the parameter is empty, doesn't exist, doesn't have an integer value, or is outside
 * the optional bounds, returns `def`.
 */
template<typename T>
T GetIntParameter(const std::string& key, T def, T min = std::numeric_limits<T>::min(),
    T max = std::numeric_limits<T>::max());

/*
 * Returns the unsigned integer corresponding to the system parameter `key`.
 * If the parameter is empty, doesn't exist, doesn't have an integer value, or is outside
 * the optional bound, returns `def`.
 */
template<typename T>
T GetUintParameter(const std::string& key, T def, T max = std::numeric_limits<T>::max());

/*
 * Sets the system parameter `key` to `value`.
 * Note that system parameter setting is inherently asynchronous so a return value of `true`
 * isn't particularly meaningful, and immediately reading back the value won't necessarily
 * tell you whether or not your call succeeded. A `false` return value definitely means failure.
 */
bool SetParameter(const std::string& key, const std::string& value);

int WaitParameter(const std::string& key, const std::string& value, int timeout);

unsigned int FindParameter(const std::string& key);

unsigned int GetParameterCommitId(unsigned int handle);

std::string GetParameterName(unsigned int handle);

std::string GetParameterValue(unsigned int handle);

/*
 * Obtains the device type of your product represented by a string.
 */
std::string GetDeviceType(void);
} // namespace system
} // namespace OHOS

#endif // SYSTEM_PARAMETERS_H