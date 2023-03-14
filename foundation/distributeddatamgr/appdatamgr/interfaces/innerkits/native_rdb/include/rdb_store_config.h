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

#ifndef NATIVE_RDB_RDB_STORE_CONFIG_H
#define NATIVE_RDB_RDB_STORE_CONFIG_H

#include <string>
#include <vector>
#include <rdb_types.h>

namespace OHOS::NativeRdb {
// indicates the type of the storage
enum class StorageMode {
    MODE_MEMORY = 101,
    MODE_DISK,
};

enum class JournalMode {
    MODE_DELETE,
    MODE_TRUNCATE,
    MODE_PERSIST,
    MODE_MEMORY,
    MODE_WAL,
    MODE_OFF,
};

enum class SyncMode {
    MODE_OFF,
    MODE_NORMAL,
    MODE_FULL,
    MODE_EXTRA,
};

enum class DatabaseFileType {
    NORMAL,
    BACKUP,
    CORRUPT,
};

enum class DatabaseFileSecurityLevel {
    S4,
    S3,
    S2,
    S1,
    S0,
    NO_LEVEL,
};

using DistributedType = OHOS::DistributedRdb::RdbDistributedType;

class RdbStoreConfig {
public:
    RdbStoreConfig(const RdbStoreConfig &config);
    RdbStoreConfig(const std::string &path, StorageMode storageMode = StorageMode::MODE_DISK, bool readOnly = false,
        const std::vector<uint8_t> &encryptKey = std::vector<uint8_t>(), const std::string &journalMode = "",
        const std::string &syncMode = "", const std::string &databaseFileType = "",
        const std::string &databaseFileSecurityLevel = "");
    ~RdbStoreConfig();
    std::string GetName() const;
    std::string GetPath() const;
    StorageMode GetStorageMode() const;
    std::string GetJournalMode() const;
    std::string GetSyncMode() const;
    std::vector<uint8_t> GetEncryptKey() const;
    bool IsReadOnly() const;
    bool IsMemoryRdb() const;
    std::string GetDatabaseFileType() const;
    std::string GetDatabaseFileSecurityLevel() const;

    // set the journal mode, if not set, the default mode is WAL
    void SetName(const std::string &name);
    void SetJournalMode(JournalMode journalMode);
    void SetPath(std::string path);
    void SetReadOnly(bool readOnly);
    void SetStorageMode(StorageMode storageMode);
    void SetDatabaseFileType(DatabaseFileType type);
    void SetEncryptKey(const std::vector<uint8_t> &encryptKey);
    void ClearEncryptKey();

    // distributed rdb
    int SetBundleName(const std::string &bundleName);
    std::string GetBundleName() const;
    int SetDistributedType(DistributedType type);
    DistributedType GetDistributedType() const;

    static std::string GetJournalModeValue(JournalMode journalMode);
    static std::string GetSyncModeValue(SyncMode syncMode);
    static std::string GetDatabaseFileTypeValue(DatabaseFileType databaseFileType);
    static std::string GetDatabaseFileSecurityLevelValue(DatabaseFileSecurityLevel databaseFileSecurityLevel);

private:
    std::string name;
    std::string path;
    StorageMode storageMode;
    std::string journalMode;
    std::string syncMode;
    std::vector<uint8_t> encryptKey;
    bool readOnly;
    std::string databaseFileType;
    std::string databaseFileSecurityLevel;

    // distributed rdb
    DistributedType distributedType_ = DistributedRdb::RdbDistributedType::RDB_DEVICE_COLLABORATION;
    std::string bundleName_;
    //
};
} // namespace OHOS::NativeRdb

#endif
