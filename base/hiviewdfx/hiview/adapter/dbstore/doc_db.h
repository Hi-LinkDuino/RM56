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

#ifndef HIVIEW_DOC_DB_H
#define HIVIEW_DOC_DB_H

#include <string>

#include "ejdb2.h"

namespace OHOS {
namespace HiviewDFX {
class DocDB {
public:
    DocDB(): db_(0) {};
    ~DocDB() {};
    int OpenDB(const std::string &dbFile, iwkv_openflags flag);
    int OnlineBackupDB(const std::string &bakFile);
    int CloseDB();
private:
    friend class DocStore;
    EJDB db_;
}; // DocDb
} // HiviewDFX
} // OHOS
#endif // HIVIEW_DOC_DB_H