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

#define LOG_TAG "RdbStoreConfig"

#include "rdb_store_config.h"

#include "logger.h"
#include "rdb_errno.h"

namespace OHOS::NativeRdb {
RdbStoreConfig::RdbStoreConfig(const RdbStoreConfig &config)
{
    name = config.GetName();
    path = config.GetPath();
    storageMode = config.GetStorageMode();
    journalMode = config.GetJournalMode();
    syncMode = config.GetSyncMode();
    encryptKey = config.GetEncryptKey();
    readOnly = config.IsReadOnly();
    databaseFileType = config.GetDatabaseFileType();
    databaseFileSecurityLevel = config.GetDatabaseFileSecurityLevel();
}

RdbStoreConfig::RdbStoreConfig(const std::string &name, StorageMode storageMode, bool isReadOnly,
    const std::vector<uint8_t> &encryptKey, const std::string &journalMode, const std::string &syncMode,
    const std::string &databaseFileType, const std::string &databaseFileSecurityLevel)
    : name(name),
      path(name),
      storageMode(storageMode),
      journalMode(journalMode),
      syncMode(syncMode),
      encryptKey(encryptKey),
      readOnly(isReadOnly),
      databaseFileType(databaseFileType),
      databaseFileSecurityLevel(databaseFileSecurityLevel)
{}

RdbStoreConfig::~RdbStoreConfig()
{
    ClearEncryptKey();
}

/**
 * Obtains the database name.
 */
std::string RdbStoreConfig::GetName() const
{
    return name;
}

/**
 * Obtains the database path.
 */
std::string RdbStoreConfig::GetPath() const
{
    return path;
}

/**
 * Obtains the storage mode.
 */
StorageMode RdbStoreConfig::GetStorageMode() const
{
    return storageMode;
}

/**
 * Obtains the journal mode in this {@code StoreConfig} object.
 */
std::string RdbStoreConfig::GetJournalMode() const
{
    return journalMode;
}

/**
 * Obtains the synchronization mode in this {@code StoreConfig} object.
 */
std::string RdbStoreConfig::GetSyncMode() const
{
    return syncMode;
}

/**
 * Obtains the encrypt key in this {@code StoreConfig} object.
 */
std::vector<uint8_t> RdbStoreConfig::GetEncryptKey() const
{
    return encryptKey;
}

/**
 * Sets the encrypt key for the object.
 */
void RdbStoreConfig::SetEncryptKey(const std::vector<uint8_t> &encryptKey)
{
    this->encryptKey = encryptKey;
}

/**
 * Checks whether the database is read-only.
 */
bool RdbStoreConfig::IsReadOnly() const
{
    return readOnly;
}

/**
 * Checks whether the database is memory.
 */
bool RdbStoreConfig::IsMemoryRdb() const
{
    return GetStorageMode() == StorageMode::MODE_MEMORY;
}

/**
 * Obtains the database file type in this {@code StoreConfig} object.
 */
std::string RdbStoreConfig::GetDatabaseFileType() const
{
    return databaseFileType;
}

/**
 * Obtains the security level of the database file.
 */
std::string RdbStoreConfig::GetDatabaseFileSecurityLevel() const
{
    return databaseFileSecurityLevel;
}

void RdbStoreConfig::SetName(const std::string &name)
{
    this->name = name;
}

/**
 * Sets the journal mode  for the object.
 */
void RdbStoreConfig::SetJournalMode(JournalMode journalMode)
{
    this->journalMode = GetJournalModeValue(journalMode);
}

void RdbStoreConfig::SetDatabaseFileType(DatabaseFileType type)
{
    this->databaseFileType = GetDatabaseFileTypeValue(type);
}

/**
 * Sets the path  for the object.
 */
void RdbStoreConfig::SetPath(std::string path)
{
    this->path = path;
}

void RdbStoreConfig::SetStorageMode(StorageMode storageMode)
{
    this->storageMode = storageMode;
}

void RdbStoreConfig::SetReadOnly(bool readOnly)
{
    this->readOnly = readOnly;
}

void RdbStoreConfig::ClearEncryptKey()
{
    std::fill(encryptKey.begin(), encryptKey.end(), 0);
    encryptKey.clear();
}

int RdbStoreConfig::SetDistributedType(DistributedType type)
{
    if (type < DistributedType::RDB_DEVICE_COLLABORATION || type >= DistributedType::RDB_DISTRIBUTED_TYPE_MAX) {
        LOG_ERROR("type is invalid");
        return E_ERROR;
    }
    distributedType_ = type;
    return E_OK;
}

DistributedType RdbStoreConfig::GetDistributedType() const
{
    return distributedType_;
}

int RdbStoreConfig::SetBundleName(const std::string &bundleName)
{
    if (bundleName.empty()) {
        LOG_ERROR("bundleName is empty");
        return E_ERROR;
    }
    bundleName_ = bundleName;
    return E_OK;
}

std::string RdbStoreConfig::GetBundleName() const
{
    return bundleName_;
}

std::string RdbStoreConfig::GetJournalModeValue(JournalMode journalMode)
{
    std::string value = "";

    switch (journalMode) {
        case JournalMode::MODE_DELETE:
            return "DELETE";
        case JournalMode::MODE_TRUNCATE:
            return "TRUNCATE";
        case JournalMode::MODE_PERSIST:
            return  "PERSIST";
        case JournalMode::MODE_MEMORY:
            return "MEMORY";
        case JournalMode::MODE_WAL:
            return "WAL";
        case JournalMode::MODE_OFF:
            return "OFF";
        default:
            break;
    }
    return value;
}

std::string RdbStoreConfig::GetSyncModeValue(SyncMode syncMode)
{
    std::string value = "";
    switch (syncMode) {
        case SyncMode::MODE_OFF:
            return "MODE_OFF";
        case SyncMode::MODE_NORMAL:
            return "MODE_NORMAL";
        case SyncMode::MODE_FULL:
            return "MODE_FULL";
        case SyncMode::MODE_EXTRA:
            return "MODE_EXTRA";
        default:
            break;
    }

    return value;
}

std::string RdbStoreConfig::GetDatabaseFileTypeValue(DatabaseFileType databaseFileType)
{
    std::string value = "";
    switch (databaseFileType) {
        case DatabaseFileType::NORMAL:
            return "db";
        case DatabaseFileType::BACKUP:
            return "backup";
        case DatabaseFileType::CORRUPT:
            return "corrupt";
        default:
            break;
    }

    return value;
}

std::string RdbStoreConfig::GetDatabaseFileSecurityLevelValue(DatabaseFileSecurityLevel databaseFileSecurityLevel)
{
    std::string value = "";

    switch (databaseFileSecurityLevel) {
        case DatabaseFileSecurityLevel::S4:
            return "S4";
        case DatabaseFileSecurityLevel::S3:
            return "S3";
        case DatabaseFileSecurityLevel::S2:
            return "S2";
        case DatabaseFileSecurityLevel::S1:
            return "S1";
        case DatabaseFileSecurityLevel::S0:
            return "S0";
        case DatabaseFileSecurityLevel::NO_LEVEL:
            return "NO_LEVEL";
        default:
            break;
    }

    return value;
}
} // namespace OHOS::NativeRdb
