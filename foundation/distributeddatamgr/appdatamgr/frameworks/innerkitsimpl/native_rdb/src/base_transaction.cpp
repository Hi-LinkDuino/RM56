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

#include "base_transaction.h"

namespace OHOS ::NativeRdb {
BaseTransaction::BaseTransaction(int id)
    : allBeforeSuccessful(true), markedSuccessful(false), childFailure(false), type(ROLLBACK_SELF), id(id)
{
}

BaseTransaction::~BaseTransaction()
{
}

bool BaseTransaction::IsAllBeforeSuccessful() const
{
    return allBeforeSuccessful;
}

void BaseTransaction::SetAllBeforeSuccessful(bool allBeforeSuccessful)
{
    this->allBeforeSuccessful = allBeforeSuccessful;
}

bool BaseTransaction::IsMarkedSuccessful() const
{
    return markedSuccessful;
}

void BaseTransaction::SetMarkedSuccessful(bool markedSuccessful)
{
    this->markedSuccessful = markedSuccessful;
}

int BaseTransaction::getType() const
{
    return type;
}

bool BaseTransaction::IsChildFailure() const
{
    return childFailure;
}

void BaseTransaction::setChildFailure(bool failureFlag)
{
    this->childFailure = failureFlag;
}

std::string BaseTransaction::getTransactionStr()
{
    std::string retStr = this->id == 0 ? BEGIN_IMMEDIATE : SAVE_POINT + " " + TRANS_STR + std::to_string(this->id);
    return retStr + ";";
}

std::string BaseTransaction::getCommitStr()
{
    std::string retStr = this->id == 0 ? COMMIT : "";
    return retStr + ";";
}

std::string BaseTransaction::getRollbackStr()
{
    std::string retStr = this->id == 0 ? ROLLBACK : ROLLBACK_TO + " " + TRANS_STR + std::to_string(this->id);
    return retStr + ";";
}
} // namespace OHOS::NativeRdb
