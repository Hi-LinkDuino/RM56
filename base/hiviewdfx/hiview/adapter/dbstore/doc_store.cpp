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

#include "doc_store.h"

#include <cinttypes>
#include <memory>

#include "doc_db.h"
#include "doc_util.h"
#include "ejdb2.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002D10, "HiView-DOCDB"};
const char* COLLECTION = "collection";
class CallBackInfo {
public:
    CallBackInfo(IWXSTR *xstr, std::function<int (int, const Entry&)> callback)
        : finish_(false), xstr_(xstr), callback_(callback)
    {
    }

    ~CallBackInfo()
    {
    }

    void Clear()
    {
        if (xstr_ != nullptr) {
            iwxstr_clear(xstr_);
        }
    }

    void DoCallBack(int id, const Entry& entry)
    {
        if (callback_(id, entry) != 0) {
            finish_ = true;
        }
    }

    IWXSTR* GetIWXSTR()
    {
        return xstr_;
    }

    bool IsFinish() const
    {
        return finish_;
    }
private:
    bool finish_;
    IWXSTR *xstr_;
    std::function<int (int, const Entry&)> callback_;
};

int DocStore::Put(const Entry &entry)
{
    if (dbPtr == nullptr) {
        return -1;
    }

    JBL jbl = 0;
    iwrc rc = 0;
    rc = jbl_from_json(&jbl, entry.value.c_str());
    RCGO(rc, FINISH);

    rc = ejdb_put_new(dbPtr->db_, COLLECTION, jbl, const_cast<int64_t*>(&entry.id));
    RCGO(rc, FINISH);
    HiLog::Debug(LABEL, "put data to doc store success");
FINISH:
    if (jbl != 0) {
        jbl_destroy(&jbl);
    }
    if (rc != 0) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "put data to doc store failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }
    return 0;
}

int DocStore::PutBatch(const std::vector<Entry> &entries)
{
    if (dbPtr == nullptr) {
        return -1;
    }
    for (auto entry = entries.begin(); entry != entries.end(); entry++) {
        Put(*entry);
    }
    return 0;
}

int DocStore::Merge(const Entry &entry)
{
    if (entry.id <= 0) {
        HiLog::Error(LABEL, "id less than 0");
        return -1;
    }

    if (entry.value.empty()) {
        HiLog::Error(LABEL, "value is empty");
        return -1;
    }

    iwrc rc = ejdb_patch(dbPtr->db_, COLLECTION, entry.value.c_str(), entry.id);
    if (rc != 0) {
        HiLog::Error(LABEL, "merge data to doc store failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }
    return 0;
}

int DocStore::Delete(const DataQuery &query)
{
    if (dbPtr == nullptr) {
        return -1;
    }
    std::string delSql = query.ToDelString(query.limit_);
    HiLog::Debug(LABEL, "delete=%{public}s", delSql.c_str());
    JQL q = 0;
    iwrc rc = jql_create(&q, COLLECTION, delSql.c_str());
    RCGO(rc, FINISH);

    EJDB_EXEC ux;
    ux.cnt = 0;
    ux.db = dbPtr->db_;
    ux.limit = 0;
    ux.log = 0;
    ux.opaque = 0;
    ux.pool = 0;
    ux.q = q;
    ux.skip = 0;
    ux.visitor = 0;
    rc = ejdb_exec(&ux);
    RCGO(rc, FINISH);
FINISH:
    if (q) {
        jql_destroy(&q);
    }
    if (rc != 0) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "delete data from doc store failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }
    return 0;
}

int DocStore::GetEntriesWithQuery(const DataQuery &query, std::vector<Entry> &entries) const
{
    if (dbPtr == nullptr) {
        return -1;
    }

    unsigned int count = 0;
    IWXSTR *xstr = iwxstr_new();
    EJDB_DOC doc = nullptr;
    EJDB_LIST listp;
    HiLog::Debug(LABEL, "query=%{public}s, limit=%{public}d", query.ToString().c_str(), query.limit_);
    iwrc rc = ejdb_list2(dbPtr->db_, COLLECTION, query.ToString().c_str(), query.limit_, &listp);
    RCGO(rc, FINISH);
    doc = listp->first;
    while (doc != nullptr) {
        count++;
        iwxstr_clear(xstr);
        iwrc resRc = jbl_as_json(doc->raw, jbl_xstr_json_printer, xstr, 0);
        if (resRc != 0) {
            HiLog::Error(LABEL, "query result=%{public}" PRId64 " value is invalid, reason:%{public}s",
                doc->id, iwlog_ecode_explained(resRc));
            doc = doc->next;
            continue;
        }

        Entry entry;
        entry.id = doc->id;
        entry.value = iwxstr_ptr(xstr);
        if (entry.value.empty()) {
            HiLog::Debug(LABEL, "query result=%{public}" PRId64 " value is empty", doc->id);
            doc = doc->next;
            continue;
        }
        entries.emplace_back(entry);
        doc = doc->next;
    }
    HiLog::Debug(LABEL, "query count=%{public}u", count);
FINISH:
    if (xstr) {
        iwxstr_destroy(xstr);
    }

    if (rc == 0) {
        ejdb_list_destroy(&listp);
    } else {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "query data from doc store failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }
    return 0;
}

static iwrc DocumentsVisitor(EJDB_EXEC *ctx, const EJDB_DOC doc, int64_t *step)
{
    CallBackInfo* callBackInfoPtr = (CallBackInfo*)ctx->opaque;
    if (callBackInfoPtr == nullptr) {
        return 0;
    }
    if (callBackInfoPtr->IsFinish()) {
        return 0;
    }
    callBackInfoPtr->Clear();
    Entry entry;
    entry.id = doc->id;
    iwrc rc = jbl_as_json(doc->raw, jbl_xstr_json_printer, callBackInfoPtr->GetIWXSTR(), 0);
    entry.value = iwxstr_ptr(callBackInfoPtr->GetIWXSTR());
    callBackInfoPtr->DoCallBack(ctx->cnt, entry);
    return rc;
}

int DocStore::GetEntryDuringQuery(const DataQuery &query, std::function<int (int, const Entry&)> callback) const
{
    if (dbPtr == nullptr) {
        return -1;
    }
    HiLog::Debug(LABEL, "query=%{public}s, limit=%{public}d", query.ToString().c_str(), query.limit_);
    IWXSTR *xstr = iwxstr_new();
    CallBackInfo *callBackInfoPtr = new CallBackInfo(xstr, callback);
    JQL q = 0;
    iwrc rc = jql_create(&q, COLLECTION, query.ToString().c_str());
    RCGO(rc, FINISH);

    EJDB_EXEC ux;
    ux.cnt = 1;
    ux.db = dbPtr->db_;
    ux.limit = 0;
    ux.log = 0;
    ux.opaque = (CallBackInfo*)callBackInfoPtr;
    ux.pool = 0;
    ux.q = q;
    ux.skip = 0;
    ux.visitor = DocumentsVisitor;
    rc = ejdb_exec(&ux);
    RCGO(rc, FINISH);
FINISH:
    if (q) {
        jql_destroy(&q);
    }

    if (xstr) {
        iwxstr_clear(xstr);
    }

    if (callBackInfoPtr) {
        delete callBackInfoPtr;
    }

    if (rc != 0) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "query data from doc store failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }
    return 0;
}
} // HiviewDFX
} // OHOS