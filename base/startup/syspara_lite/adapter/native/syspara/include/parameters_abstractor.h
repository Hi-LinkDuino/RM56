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

#ifndef PARAMETERS_ABSTRACTOR_H
#define PARAMETERS_ABSTRACTOR_H

#include <string>

namespace OHOS {
namespace system {
class ParametersAbstractor {
public:
    virtual std::string GetParameter(const std::string& key, const std::string& defValue) = 0;
    virtual bool SetParameter(const std::string& key, const std::string& value) = 0;
    virtual int WaitParameter(const std::string& key, const std::string& value, int timeout) = 0;
    virtual unsigned int FindParameter(const std::string& key) = 0;
    virtual unsigned int GetParameterCommitId(unsigned int handle) = 0;
    virtual std::string GetParameterName(unsigned int handle) = 0;
    virtual std::string GetParameterValue(unsigned int handle) = 0;
    virtual ~ParametersAbstractor() {};
};
} // namespace system
} // namespace OHOS

#endif // PARAMETERS_ABSTRACTOR_H