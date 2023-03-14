/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTEDDATAMGR_APPDATAMGR_BASE_TRANSACTION_H
#define DISTRIBUTEDDATAMGR_APPDATAMGR_BASE_TRANSACTION_H

#include <iostream>

namespace OHOS::NativeRdb {
enum TransType {
    ROLLBACK_SELF = 1,
    ROLLBACK_PARENT,
};

class BaseTransaction {
public:
    explicit BaseTransaction(int id);
    ~BaseTransaction();
    bool IsAllBeforeSuccessful() const;
    void SetAllBeforeSuccessful(bool allBeforeSuccessful);
    bool IsMarkedSuccessful() const;
    void SetMarkedSuccessful(bool markedSuccessful);
    int getType() const;
    bool IsChildFailure() const;
    void setChildFailure(bool failureFlag);
    std::string getTransactionStr();
    std::string getCommitStr();
    std::string getRollbackStr();

private:
    bool allBeforeSuccessful;
    bool markedSuccessful;
    bool childFailure;
    int type;
    int id;

    const std::string BEGIN_IMMEDIATE = "BEGIN EXCLUSIVE";
    const std::string TRANS_STR = "TRANS_STR";
    const std::string SAVE_POINT = "SAVEPOINT";
    const std::string COMMIT = "COMMIT";
    const std::string ROLLBACK = "ROLLBACK";
    const std::string ROLLBACK_TO = "ROLLBACK TO";
};
} // namespace OHOS::NativeRdb
#endif
