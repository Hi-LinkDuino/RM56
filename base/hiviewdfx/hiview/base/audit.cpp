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
#include "audit.h"

#include <fcntl.h>
#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

#include "file_util.h"
#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
static const char AUDIT_LOG_PATH[] = "/data/log/faultlog/hiview_audit.log.0";
static const char AUDIT_LOG_PATH_BAK[] = "/data/log/faultlog/hiview_audit.log.1";
}

Audit::~Audit()
{
    enabled_ = false;
}

bool Audit::WriteAuditEvent(StatsEvent eventType, uint64_t eventId, const std::string& digest)
{
    static auto& instance = Audit::GetInstance();
    if (!instance.IsEnabled()) {
        return false;
    }

    if (digest.empty() || (digest.length() >= MAX_AUDIT_LOG_SIZE)) {
        return false;
    }

    std::string out;
    out.append(std::to_string(TimeUtil::GenerateTimestamp()));
    out.push_back(DOMAIN_DELIMITER);
    out.append(std::to_string(eventId));
    out.push_back(DOMAIN_DELIMITER);
    out.append(std::to_string(eventType));
    out.push_back(DOMAIN_DELIMITER);
    out.append(digest);
    out.append("\n");
    instance.WriteEvent(out);
    return true;
}

bool Audit::GetAuditLog(bool loadFromFile, std::list<std::string>& ret)
{
    static auto& instance = Audit::GetInstance();
    if (!instance.IsEnabled()) {
        return false;
    }

    instance.GetAuditLogInner(loadFromFile, ret);
    return true;
}

bool Audit::IsEnabled()
{
    static auto& instance = Audit::GetInstance();
    return instance.enabled_;
}

void Audit::GetAuditLogInner(bool loadFromFile, std::list<std::string>& ret)
{
    if (!loadFromFile) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::string activeFile = GetLogFilePath(true);
    ReadLogFromFile(activeFile, ret);
    std::string inactiveFile = GetLogFilePath(false);
    ReadLogFromFile(inactiveFile, ret);
}

// call once in platform initialization
void Audit::Init(bool isBeta)
{
    if (init_) {
        return;
    }

    std::call_once(initFlag_, [&]() { enabled_ = isBeta; });
    std::lock_guard<std::mutex> lock(mutex_);
    useBak_ = IsBackupFileActive();
    std::string path = useBak_ ? AUDIT_LOG_PATH_BAK : AUDIT_LOG_PATH;
    writeFd_ = UniqueFd(FileUtil::Open(path, O_CREAT | O_APPEND | O_RDWR,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH));
    if (writeFd_.Get() < 0) {
        enabled_ = false;
    }
    writeLogCount_ = 0;
    init_ = true;
}

void Audit::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::fstream logFile(AUDIT_LOG_PATH, std::ios::out);
    std::fstream bakLogFile(AUDIT_LOG_PATH_BAK, std::ios::out);
}

void Audit::WriteEvent(const std::string& content)
{
    std::lock_guard<std::mutex> lock(mutex_);
    SaveToFileLocked(content);
}

bool Audit::IsActiveLogFileSizeReachTheashold()
{
    writeLogCount_ = 0;
    const auto& path = GetLogFilePath(true);
    struct stat sb;
    if (stat(path.c_str(), &sb) == -1) {
        return false;
    }

    return sb.st_size >= MAX_DISK_LOG_SIZE;
}

bool Audit::IsBackupFileActive()
{
    struct stat originLogFileStat;
    if (stat(AUDIT_LOG_PATH, &originLogFileStat) == -1) {
        return false;
    }

    struct stat bakLogFileStat;
    if (stat(AUDIT_LOG_PATH_BAK, &bakLogFileStat) == -1) {
        return false;
    }

    if (originLogFileStat.st_size == 0) {
        return false;
    }

    return bakLogFileStat.st_mtime >= originLogFileStat.st_mtime;
}

void Audit::SaveToFileLocked(const std::string& content)
{
    if ((writeLogCount_ > MAX_MEMORY_LOG_COUNT) && IsActiveLogFileSizeReachTheashold()) {
        SwitchActiveFile();
    }

    int fd = writeFd_.Get();
    if (fd > 0 && content.length() < MAX_AUDIT_LOG_SIZE) {
        // no need to check the result.
        write(fd, content.c_str(), content.length());
    }
    writeLogCount_ = writeLogCount_ + 1;
}

bool Audit::ReadLogFromFile(const std::string& path, std::list<std::string>& ret)
{
    std::list<std::string> fileLog;
    std::ifstream logFile(path);
    std::string line;
    if (logFile.good()) {
        while (getline(logFile, line)) {
            fileLog.push_back(line);
        }
    } else {
        logFile.close();
        return false;
    }
    logFile.close();
    ret.insert(ret.begin(), fileLog.begin(), fileLog.end());
    return true;
}

const std::string Audit::GetLogFilePath(bool active)
{
    if ((useBak_ && active) || (!useBak_ && !active)) {
        return AUDIT_LOG_PATH_BAK;
    }
    return AUDIT_LOG_PATH;
}

void Audit::SwitchActiveFile()
{
    useBak_ = !useBak_;
    const auto& path = GetLogFilePath(true);
    writeFd_ = UniqueFd(FileUtil::Open(path, O_CREAT | O_TRUNC | O_RDWR,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH));
}
} // namespace HiviewDFX
} // namespace OHOS
