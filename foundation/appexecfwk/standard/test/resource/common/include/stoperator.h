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
#ifndef _ABILITY_MANAGER_SERVICE_ST_OPERATOR_H_
#define _ABILITY_MANAGER_SERVICE_ST_OPERATOR_H_

#include <string>
#include <memory>
#include <vector>

namespace STtools {
using std::string;
class StOperator;
std::vector<string> SerializationStOperatorToVector(StOperator &ParentOperator);
void DeserializationStOperatorFromVector(StOperator &ParentOperator, std::vector<string> &vectorOperator);
class StOperator {
private:
    std::vector<std::shared_ptr<StOperator>> g_childOperator;
    std::shared_ptr<StOperator> g_parentOperator;
    string g_abilityType;
    string g_bundleName;
    string g_abilityName;
    string g_operatorName;  // data ability
    string g_message;
    /* data */
    static int countChild;

public:
    StOperator();
    StOperator(const string &type, const string &bundle, const string &ability, const string &operatorName = "",
        const string &message = "");
    ~StOperator();
    static int GetCountChild();
    string GetAbilityType();
    StOperator &SetAbilityType(const string &type);
    string GetBundleName();
    StOperator &SetBundleName(const string &bundleName);
    string GetAbilityName();
    StOperator &SetAbilityName(const string &abilityName);
    string GetOperatorName();
    StOperator &SetOperatorName(const string &operatorName);
    string GetMessage();
    StOperator &SetMessage(const string &message);
    StOperator &AddChildOperator(std::shared_ptr<StOperator> childOperator);
    std::vector<std::shared_ptr<StOperator>> GetChildOperator();
    std::vector<std::shared_ptr<StOperator>> PopChildOperator();
};
}  // namespace STtools
#endif  // _ABILITY_MANAGER_SERVICE_ST_OPERATOR_H_

