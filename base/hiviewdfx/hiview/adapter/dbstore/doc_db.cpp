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

#include "doc_db.h"

#include "doc_util.h"
#include "ejdb2.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002D10, "HiView-DOCDB"};
int DocDB::OpenDB(const std::string &dbFile, iwkv_openflags flag)
{
    EJDB_OPTS opts;
    opts.kv.path = dbFile.c_str();
    opts.kv.oflags = flag;
    opts.kv.random_seed = 0;
    opts.kv.fmt_version = 0;
    opts.kv.file_lock_fail_fast = 0;

    opts.kv.wal.enabled = 0;
    opts.kv.wal.check_crc_on_checkpoint = 0;
    opts.kv.wal.savepoint_timeout_sec = 0;
    opts.kv.wal.checkpoint_timeout_sec = 0;
    opts.kv.wal.wal_buffer_sz = 131072; // 131072 : 128KB
    opts.kv.wal.checkpoint_buffer_sz = 0;
    opts.kv.wal.wal_lock_interceptor = nullptr;
    opts.kv.wal.wal_lock_interceptor_opaque = nullptr;

    opts.http.enabled = 0;
    opts.http.port = 0;
    opts.http.bind = nullptr;
    opts.http.access_token = nullptr;
    opts.http.access_token_len = 0;
    opts.http.blocking = 0;
    opts.http.read_anon = 0;
    opts.http.max_body_size = 0;

    opts.no_wal = 0;
    opts.sort_buffer_sz = 0;
    opts.document_buffer_sz = 0;

    iwrc rc = ejdb_init();
    if (rc) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "ejdb init failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    }

    rc = ejdb_open(&opts, &db_);
    if (rc) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "open ejdb failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    } else {
        HiLog::Info(LABEL, "open ejdb success");
    }
    return 0;
}

int DocDB::OnlineBackupDB(const std::string &bakFile)
{
    uint64_t ts = 0;
    iwrc rc = ejdb_online_backup(db_, &ts, bakFile.c_str());
    if (rc) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "ejdb online backup failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    } else {
        HiLog::Info(LABEL, "ejdb online backup success");
    }
    return 0;
}

int DocDB::CloseDB()
{
    iwrc rc = ejdb_close(&db_);
    if (rc) {
        iwlog_ecode_error3(rc);
        HiLog::Error(LABEL, "close ejdb failed, reason:%{public}s", iwlog_ecode_explained(rc));
        return MapErrorCode(rc);
    } else {
        HiLog::Info(LABEL, "close ejdb success");
    }
    return 0;
}
} // HiviewDFX
} // OHOS