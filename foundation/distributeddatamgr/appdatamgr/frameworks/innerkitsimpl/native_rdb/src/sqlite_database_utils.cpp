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

#include "sqlite_database_utils.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <fstream>

#include "logger.h"
#include "rdb_errno.h"

namespace OHOS {
namespace NativeRdb {
std::map<std::string, int> SqliteDatabaseUtils::g_statementType = SqliteDatabaseUtils::MapInit();
std::mutex SqliteDatabaseUtils::g_locker;
// Set the file access permissions is 777
int SqliteDatabaseUtils::g_mkdirMode = 0771;
std::map<std::string, int> SqliteDatabaseUtils::MapInit()
{
    std::map<std::string, int> temp;
    temp["SEL"] = STATEMENT_SELECT;
    temp["INS"] = STATEMENT_UPDATE;
    temp["UPD"] = STATEMENT_UPDATE;
    temp["REP"] = STATEMENT_UPDATE;
    temp["DEL"] = STATEMENT_UPDATE;
    temp["ATT"] = STATEMENT_ATTACH;
    temp["DET"] = STATEMENT_DETACH;
    temp["COM"] = STATEMENT_COMMIT;
    temp["END"] = STATEMENT_COMMIT;
    temp["ROL"] = STATEMENT_ROLLBACK;
    temp["BEG"] = STATEMENT_BEGIN;
    temp["PRA"] = STATEMENT_PRAGMA;
    temp["CRE"] = STATEMENT_DDL;
    temp["DRO"] = STATEMENT_DDL;
    temp["ALT"] = STATEMENT_DDL;

    return temp;
}

/**
 * Obtains the type of SQL statement.
 */
int SqliteDatabaseUtils::GetSqlStatementType(std::string sql)
{
    if (sql.empty()) {
        return STATEMENT_OTHER;
    }
    sql.erase(0, sql.find_first_not_of(" "));
    sql.erase(sql.find_last_not_of(" ") + 1);

    if (sql.length() < SQL_FIRST_CHARACTER) {
        return STATEMENT_OTHER;
    }
    sql = sql.substr(0, SQL_FIRST_CHARACTER);
    std::string prefixSql;
    transform(sql.begin(), sql.end(), prefixSql.begin(), ::toupper);
    prefixSql = prefixSql.c_str();
    auto iter = g_statementType.find(prefixSql);
    if (iter != g_statementType.end()) {
        return iter->second;
    }
    return STATEMENT_OTHER;
}

/**
 * Delete the specified file.
 */
void SqliteDatabaseUtils::DeleteFile(std::string &fileName)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        LOG_ERROR("File %{private}s does not exist", fileName.c_str());
        return;
    }
    if (!remove(fileName.c_str())) {
        LOG_ERROR("FileName= %{private}s has been deleted", fileName.c_str());
        return;
    }
    LOG_INFO("Failed to delete File %{private}s", fileName.c_str());
}

/**
 * Rename file.
 */
bool SqliteDatabaseUtils::RenameFile(std::string &oldFileName, std::string &newFileName)
{
    if (access(oldFileName.c_str(), F_OK) != 0) {
        LOG_ERROR("File %{private}s does not exist", oldFileName.c_str());
        return false;
    }
    if (rename(oldFileName.c_str(), newFileName.c_str())) {
        LOG_ERROR("Rename oldFileName = %{private}s to newFileName  %{private}s", oldFileName.c_str(),
            newFileName.c_str());
        return true;
    }
    return false;
}

/**
 * Get and Check default path.
 */
std::string SqliteDatabaseUtils::GetDefaultDatabasePath(std::string &context, std::string &name, int &errorCode)
{
    std::unique_lock<std::mutex> lock(g_locker);
    if (access(context.c_str(), F_OK) != 0) {
        if (mkdir(context.c_str(), g_mkdirMode)) {
            errorCode = E_CREATE_FOLDER_FAIL;
        }
    }
    std::string databasePath = context.append("/").append("db");
    if (access(databasePath.c_str(), F_OK) != 0) {
        if (mkdir(databasePath.c_str(), g_mkdirMode)) {
            errorCode = E_CREATE_FOLDER_FAIL;
        }
    }
    char canonicalPath[PATH_MAX + 1] = { 0 };
    if (realpath(databasePath.c_str(), canonicalPath) == nullptr) {
        LOG_ERROR("Failed to obtain real path, errno:%{public}d", errno);
        errorCode = E_INVALID_FILE_PATH;
        return "";
    }
    std::string realFilePath(canonicalPath);
    realFilePath = realFilePath.append("/").append(name);
    return realFilePath;
}

/**
 * Get corrupt path from database path.
 */
std::string SqliteDatabaseUtils::GetCorruptPath(std::string &path, int &errorCode)
{
    std::string databaseFile = path;
    std::string name = databaseFile.substr(databaseFile.find_last_of("/") + 1);
    std::string parentFile = databaseFile.substr(0, databaseFile.find_last_of("/"));
    std::string databaseTypeDir = parentFile.substr(parentFile.find_last_of("/") + 1);
    size_t posDatabaseType = databaseTypeDir.find("_encrypt");

    bool isEncrypt = false;
    if (posDatabaseType != databaseTypeDir.npos) {
        std::string databaseTypeDirStr = databaseTypeDir.substr(posDatabaseType);
        std::string end = "_encrypt";
        if (databaseTypeDirStr.compare(end) == 0) {
            isEncrypt = true;
        }
    }

    std::string databaseDir = parentFile.substr(0, parentFile.find_last_of("/"));
    std::string encrypt = isEncrypt ? "_encrypt" : "";
    std::string corruptTypeDir = "corrupt" + encrypt;
    std::string corruptPath = databaseDir + "/" + corruptTypeDir;

    if (access(corruptPath.c_str(), F_OK) != 0) {
        if (mkdir(corruptPath.c_str(), g_mkdirMode)) {
            errorCode = E_CREATE_FOLDER_FAIL;
        }
    }
    corruptPath = corruptPath + "/" + name;
    return corruptPath;
}

/**
 * Get and Check no dbname path.
 */
std::string SqliteDatabaseUtils::GetDatabasePathNoName(std::string &context, RdbStoreConfig &fileConfig, int &errorCode)
{
    std::string securityLevel = fileConfig.GetDatabaseFileSecurityLevel();
    std::string databaseDir = GetDatabaseDir(fileConfig, securityLevel);
    std::string databasePath = context + "/" + databaseDir;
    std::unique_lock<std::mutex> lock(g_locker);
    if (access(databasePath.c_str(), F_OK) != 0) {
        if (mkdir(databasePath.c_str(), g_mkdirMode)) {
            errorCode = E_CREATE_FOLDER_FAIL;
        }
    }
    return databasePath;
}

std::string SqliteDatabaseUtils::GetDatabaseDir(RdbStoreConfig &fileConfig, std::string &securityLevel)
{
    std::string databaseTypeDir = fileConfig.GetDatabaseFileType();
    std::string suffix = fileConfig.GetEncryptKey().empty() ? "" : "_encrypt";
    std::string prefix;
    if (!securityLevel.compare("S3") || !securityLevel.compare("S4")) {
        prefix = "sece_";
    } else {
        prefix = "";
    }
    return prefix + databaseTypeDir + suffix;
}
} // namespace NativeRdb
} // namespace OHOS