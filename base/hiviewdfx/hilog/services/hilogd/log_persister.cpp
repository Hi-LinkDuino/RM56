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

#include "log_persister.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/prctl.h>

#include <fcntl.h>
#include <securec.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

#include "hilog_common.h"
#include "log_buffer.h"
#include "log_compress.h"
#include "format.h"

namespace OHOS {
namespace HiviewDFX {

static constexpr int DEFAULT_LOG_LEVEL = (1 << LOG_DEBUG) | (1 << LOG_INFO)
    | (1 << LOG_WARN) | (1 << LOG_ERROR) | (1 << LOG_FATAL);
static constexpr int SLEEP_TIME = 5;

static bool isEmptyThread(const std::thread& th)
{
    static const std::thread EMPTY_THREAD;
    return th.get_id() == EMPTY_THREAD.get_id();
}

std::recursive_mutex LogPersister::s_logPersistersMtx;
std::list<std::shared_ptr<LogPersister>> LogPersister::s_logPersisters;

std::shared_ptr<LogPersister> LogPersister::CreateLogPersister(HilogBuffer &buffer)
{
    // Because of:
    // -  static_assert(is_constructible<_Tp, _Args...>::value, "Can't construct object in make_shared");
    // make shared can't be used!
    return std::shared_ptr<LogPersister>(new LogPersister(buffer));
}

LogPersister::LogPersister(HilogBuffer &buffer) : m_hilogBuffer(buffer)
{
    m_mappedPlainLogFile = nullptr;
    m_bufReader = m_hilogBuffer.CreateBufReader([this]() { NotifyNewLogAvailable(); });
}

LogPersister::~LogPersister()
{
    m_hilogBuffer.RemoveBufReader(m_bufReader);
    Deinit();
}

int LogPersister::InitCompression()
{
    m_compressBuffer = std::make_unique<LogPersisterBuffer>();
    if (!m_compressBuffer) {
        return RET_FAIL;
    }
    switch (m_baseData.compressAlg) {
        case COMPRESS_TYPE_NONE:
            m_compressor = std::make_unique<NoneCompress>();
            break;
        case COMPRESS_TYPE_ZLIB:
            m_compressor = std::make_unique<ZlibCompress>();
            break;
        case COMPRESS_TYPE_ZSTD:
            m_compressor = std::make_unique<ZstdCompress>();
            break;
        default:
            break;
    }
    if (!m_compressor) {
        return RET_FAIL;
    }
    return RET_SUCCESS;
}

int LogPersister::InitFileRotator(const InitData& initData)
{
    std::string fileSuffix = "";
    switch (m_baseData.compressAlg) {
        case CompressAlg::COMPRESS_TYPE_ZSTD:
            fileSuffix = ".zst";
            break;
        case CompressAlg::COMPRESS_TYPE_ZLIB:
            fileSuffix = ".gz";
            break;
        default:
            break;
    }
    m_fileRotator = std::make_unique<LogPersisterRotator>(m_baseData.logPath, m_baseData.id,
        m_baseData.maxLogFileNum, fileSuffix);
    if (!m_fileRotator) {
        std::cerr << "Not enough memory!\n";
        return RET_FAIL;
    }

    PersistRecoveryInfo info = {0};
    bool restore = false;
    if (std::holds_alternative<LogPersistStartMsg>(initData)) {
        info.msg = std::get<LogPersistStartMsg>(initData);
        info.types = m_filters.inclusions.types;
        info.levels = m_filters.inclusions.levels;
    } else if (std::holds_alternative<PersistRecoveryInfo>(initData)) {
        info = std::get<PersistRecoveryInfo>(initData);
        restore = true;
    }
    return m_fileRotator->Init(info, restore);
}

int LogPersister::Init(const InitData& initData)
{
    std::cout << __PRETTY_FUNCTION__  << " Begin\n";
    std::lock_guard<decltype(m_initMtx)> lock(m_initMtx);
    if (m_inited) {
        return 0;
    }

    auto initByMsg = [this](const LogPersistStartMsg& msg) {
        m_baseData.id = msg.jobId;
        m_baseData.logPath = msg.filePath;
        m_baseData.logFileSizeLimit = msg.fileSize;
        m_baseData.maxLogFileNum = msg.fileNum;
        m_baseData.compressAlg = msg.compressAlg;
        m_baseData.newLogTimeout = std::chrono::seconds(SLEEP_TIME);

        m_filters.inclusions.types = msg.logType;
        m_filters.inclusions.levels = DEFAULT_LOG_LEVEL;
    };

    bool restore = false;
    if (std::holds_alternative<LogPersistStartMsg>(initData)) {
        const LogPersistStartMsg& msg = std::get<LogPersistStartMsg>(initData);
        initByMsg(msg);
    } else if (std::holds_alternative<PersistRecoveryInfo>(initData)) {
        const LogPersistStartMsg& msg = std::get<PersistRecoveryInfo>(initData).msg;
        initByMsg(msg);
        restore = true;
    } else {
        std::cerr << __PRETTY_FUNCTION__  << "Init data not provided\n";
        return RET_FAIL;
    }

    size_t separatorPos = m_baseData.logPath.find_last_of('/');
    if (separatorPos == std::string::npos) {
        return ERR_LOG_PERSIST_FILE_PATH_INVALID;
    }

    std::string parentPath = m_baseData.logPath.substr(0, separatorPos);
    if (access(parentPath.c_str(), F_OK) != 0) {
        perror("persister directory does not exist.");
        return ERR_LOG_PERSIST_FILE_PATH_INVALID;
    }

    // below guard is needed to have sure only one Path and Id is reqistered till end of init!
    std::lock_guard<decltype(s_logPersistersMtx)> guard(s_logPersistersMtx);
    if (CheckRegistered(m_baseData.id, m_baseData.logPath)) {
        std::cerr << __PRETTY_FUNCTION__ << "Log persister already registered. Path:" << m_baseData.logPath
            << " id:" << m_baseData.id << "\n";
        return ERR_LOG_PERSIST_TASK_FAIL;
    }
    if (InitCompression() !=  RET_SUCCESS) {
        return ERR_LOG_PERSIST_COMPRESS_INIT_FAIL;
    }
    if (int result = InitFileRotator(initData); result !=  RET_SUCCESS) {
        return result;
    }

    if (int result = PrepareUncompressedFile(parentPath, restore)) {
        return result;
    }

    RegisterLogPersister(shared_from_this());
    m_inited = true;
    std::cout << __PRETTY_FUNCTION__  << " Done\n";
    return 0;
}

int LogPersister::Deinit()
{
    std::cout << __PRETTY_FUNCTION__  << " Begin\n";
    std::lock_guard<decltype(m_initMtx)> lock(m_initMtx);
    if (!m_inited) {
        return 0;
    }

    Stop();

    munmap(m_mappedPlainLogFile, MAX_PERSISTER_BUFFER_SIZE);
    std::cout << "Removing unmapped plain log file: " << m_plainLogFilePath << "\n";
    if (remove(m_plainLogFilePath.c_str())) {
        std::cerr << "File: " << m_plainLogFilePath << " can't be removed. ";
        HilogPrintError(errno);
    }

    DeregisterLogPersister(shared_from_this());
    m_inited = false;
    std::cout << __PRETTY_FUNCTION__  << " Done\n";
    return 0;
}

int LogPersister::PrepareUncompressedFile(const std::string& parentPath, bool restore)
{
    std::string fileName = std::string(".") + AUXILLARY_PERSISTER_PREFIX + std::to_string(m_baseData.id);
    m_plainLogFilePath = parentPath + "/" + fileName;
    FILE* plainTextFile = fopen(m_plainLogFilePath.c_str(), restore ? "r+" : "w+");

    if (!plainTextFile) {
        std::cerr << __PRETTY_FUNCTION__ << " Open uncompressed log file(" << m_plainLogFilePath << ") failed: ";
        HilogPrintError(errno);
        return ERR_LOG_PERSIST_FILE_OPEN_FAIL;
    }

    if (!restore) {
        ftruncate(fileno(plainTextFile), sizeof(LogPersisterBuffer));
        fflush(plainTextFile);
        fsync(fileno(plainTextFile));
    }
    m_mappedPlainLogFile = reinterpret_cast<LogPersisterBuffer*>(mmap(nullptr, sizeof(LogPersisterBuffer),
        PROT_READ | PROT_WRITE, MAP_SHARED, fileno(plainTextFile), 0));
    if (fclose(plainTextFile)) {
        std::cerr << "File: " << plainTextFile << " can't be closed. ";
        HilogPrintError(errno);
    }
    if (m_mappedPlainLogFile == MAP_FAILED) {
        std::cerr << __PRETTY_FUNCTION__ << " mmap file failed: ";
        HilogPrintError(errno);
        return RET_FAIL;
    }
    if (restore) {
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << " Recovered persister, Offset=" << m_mappedPlainLogFile->offset << "\n";
#endif
        // try to store previous uncompressed logs
        auto compressionResult = m_compressor->Compress(*m_mappedPlainLogFile, *m_compressBuffer);
        if (compressionResult != 0) {
            std::cerr << __PRETTY_FUNCTION__ << " Compression error. Result:" << compressionResult << "\n";
            return RET_FAIL;
        }
        WriteCompressedLogs();
    } else {
        m_mappedPlainLogFile->offset = 0;
    }
    return 0;
}

void LogPersister::NotifyNewLogAvailable()
{
    m_receiveLogCv.notify_one();
}

std::list<std::string> LogDataToFormatedStrings(const HilogData& logData)
{
    std::list<std::string> resultLogLines;
    std::array<char, MAX_LOG_LEN*2> tempBuffer = {0};
    HilogShowFormatBuffer showBuffer;
    showBuffer.level = logData.level;
    showBuffer.pid = logData.pid;
    showBuffer.tid = logData.tid;
    showBuffer.domain = logData.domain;
    showBuffer.tv_sec = logData.tv_sec;
    showBuffer.tv_nsec = logData.tv_nsec;

    std::vector<char> dataCopy(logData.len, 0);
    if (dataCopy.data() == nullptr) {
        return resultLogLines;
    }
    if (memcpy_s(dataCopy.data(), logData.len, logData.tag, logData.len)) {
        return resultLogLines;
    }
    showBuffer.data = dataCopy.data();
    // Below code replace 'new line' character with 'zero' to simulate
    // continuation of very long log message with tag prefix at the begining of every line.
    // e.g.  "This is very \n long line \n for sure!!!"
    // This will be changed into:
    //   <formated meta data> <tag info> This is very
    //   <formated meta data> <tag info> long line
    //   <formated meta data> <tag info> for sure!!!
    size_t newLineOffset = logData.tag_len;
    char *msgBegin = dataCopy.data() + newLineOffset;
    char *currenMsgPos = msgBegin;
    while (*currenMsgPos != 0) {
        if (*currenMsgPos == '\n') {
            if (currenMsgPos != msgBegin) {
                *currenMsgPos = 0;
                showBuffer.tag_len = newLineOffset;
                HilogShowBuffer(tempBuffer.data(), tempBuffer.size(), showBuffer, OFF_SHOWFORMAT);
                resultLogLines.push_back(tempBuffer.data());
                newLineOffset += currenMsgPos - msgBegin + 1;
            } else {
                newLineOffset++;
            }
            msgBegin = currenMsgPos + 1;
        }
        currenMsgPos++;
    }
    if (currenMsgPos != msgBegin) {
        showBuffer.tag_len = newLineOffset;
        HilogShowBuffer(tempBuffer.data(), tempBuffer.size(), showBuffer, OFF_SHOWFORMAT);
        resultLogLines.push_back(tempBuffer.data());
    }
    return resultLogLines;
}

bool LogPersister::WriteUncompressedLogs(std::list<std::string>& formatedTextLogs)
{
    while (!formatedTextLogs.empty()) {
        std::string logLine = formatedTextLogs.front();
        uint16_t size = logLine.length() + 1; // we want to add new line character
        uint32_t origOffset = m_mappedPlainLogFile->offset;
        if (m_mappedPlainLogFile->offset + size > MAX_PERSISTER_BUFFER_SIZE)
            return false;

        char* currentContentPos = m_mappedPlainLogFile->content + m_mappedPlainLogFile->offset;
        uint32_t remainingSpace = MAX_PERSISTER_BUFFER_SIZE - m_mappedPlainLogFile->offset;
        int r = memcpy_s(currentContentPos, remainingSpace, logLine.c_str(), logLine.length());
        if (r != 0) {
            std::cerr << __PRETTY_FUNCTION__ << " Can't copy part of memory!\n";
            m_mappedPlainLogFile->offset = origOffset;
            return true;
        }
        formatedTextLogs.pop_front();
        m_mappedPlainLogFile->offset += logLine.length();
        m_mappedPlainLogFile->content[m_mappedPlainLogFile->offset] = '\n';
        m_mappedPlainLogFile->offset += 1;
    }
    return true;
}

int LogPersister::WriteLogData(const HilogData& logData)
{
    std::list<std::string> formatedTextLogs = LogDataToFormatedStrings(logData);

    // Firstly gather uncompressed logs in auxiliary file
    if (WriteUncompressedLogs(formatedTextLogs))
        return 0;
    // Try to compress auxiliary file
    auto compressionResult = m_compressor->Compress(*m_mappedPlainLogFile, *m_compressBuffer);
    if (compressionResult != 0) {
        std::cerr <<  __PRETTY_FUNCTION__ << " Compression error. Result:" << compressionResult << "\n";
        return RET_FAIL;
    }
    // Write compressed buffor and clear counters
    WriteCompressedLogs();
    // Try again write data that wasn't written at the beginning
    // If again fail then these logs are skipped
    return WriteUncompressedLogs(formatedTextLogs) ? 0 : RET_FAIL;
}

inline void LogPersister::WriteCompressedLogs()
{
    if (m_mappedPlainLogFile->offset == 0)
        return;
    m_fileRotator->Input(m_compressBuffer->content, m_compressBuffer->offset);
    m_plainLogSize += m_mappedPlainLogFile->offset;
    std::cout << __PRETTY_FUNCTION__ <<  " Stored plain log bytes: " << m_plainLogSize
        << " from: " << m_baseData.logFileSizeLimit << "\n";
    if (m_plainLogSize >= m_baseData.logFileSizeLimit) {
        m_plainLogSize = 0;
        m_fileRotator->FinishInput();
    }
    m_compressBuffer->offset = 0;
    m_mappedPlainLogFile->offset = 0;
}

void LogPersister::Start()
{
    {
        std::lock_guard<decltype(m_initMtx)> lock(m_initMtx);
        if (!m_inited) {
            std::cerr << __PRETTY_FUNCTION__ << " Log persister wasn't inited!\n";
            return;
        }
    }

    if (isEmptyThread(m_persisterThread)) {
        m_persisterThread = std::thread(&LogPersister::ReceiveLogLoop, shared_from_this());
    } else {
        std::cout << __PRETTY_FUNCTION__ << " Persister thread already started!\n";
    }
}

int LogPersister::ReceiveLogLoop()
{
    prctl(PR_SET_NAME, "hilogd.pst");
    std::cout << __PRETTY_FUNCTION__ << " " << std::this_thread::get_id() << "\n";
    for (;;) {
        if (m_stopThread) {
            break;
        }

        auto result = m_hilogBuffer.Query(m_filters, m_bufReader, [this](const HilogData& logData) {
            if (WriteLogData(logData)) {
                std::cerr << __PRETTY_FUNCTION__ << " Can't write new log data!\n";
            }
        });

        if (!result) {
            std::unique_lock<decltype(m_receiveLogCvMtx)> lk(m_receiveLogCvMtx);
            m_receiveLogCv.wait_for(lk, m_baseData.newLogTimeout);
        }
    }
    WriteCompressedLogs();
    m_fileRotator->FinishInput();
    return 0;
}

int LogPersister::Query(uint16_t logType, std::list<LogPersistQueryResult> &results)
{
    std::lock_guard<decltype(s_logPersistersMtx)> guard(s_logPersistersMtx);
    std::cout << __PRETTY_FUNCTION__ << " Persister.Query: logType " << logType << "\n";
    for (auto& logPersister : s_logPersisters) {
        uint16_t currentType = logPersister->m_filters.inclusions.types;
        std::cout << __PRETTY_FUNCTION__ << " Persister.Query: (*it)->queryCondition.types " << currentType << "\n";
        if (currentType & logType) {
            LogPersistQueryResult response;
            response.logType = currentType;
            logPersister->FillInfo(response);
            results.push_back(response);
        }
    }
    return 0;
}

void LogPersister::FillInfo(LogPersistQueryResult &response)
{
    response.jobId = m_baseData.id;
    if (strcpy_s(response.filePath, FILE_PATH_MAX_LEN, m_baseData.logPath.c_str())) {
        return;
    }
    response.compressAlg = m_baseData.compressAlg;
    response.fileSize = m_baseData.logFileSizeLimit;
    response.fileNum = m_baseData.maxLogFileNum;
}

int LogPersister::Kill(uint32_t id)
{
    auto logPersisterPtr = GetLogPersisterById(id);
    if (logPersisterPtr) {
        return logPersisterPtr->Deinit();
    }
    std::cerr << __PRETTY_FUNCTION__ << " Log persister with id: " << id << " does not exist.\n";
    return ERR_LOG_PERSIST_JOBID_FAIL;
}

void LogPersister::Stop()
{
    std::cout << __PRETTY_FUNCTION__ << " Exiting LogPersister!\n";
    if (isEmptyThread(m_persisterThread)) {
        std::cout << __PRETTY_FUNCTION__ << " Thread was exited or not started!\n";
        return;
    }

    m_stopThread = true;
    m_receiveLogCv.notify_all();

    if (m_persisterThread.joinable()) {
        m_persisterThread.join();
    }
}

bool LogPersister::CheckRegistered(uint32_t id, const std::string& logPath)
{
    std::lock_guard<decltype(s_logPersistersMtx)> lock(s_logPersistersMtx);
    auto it = std::find_if(s_logPersisters.begin(), s_logPersisters.end(),
        [&](const std::shared_ptr<LogPersister>& logPersister) {
            if (logPersister->m_baseData.logPath == logPath || logPersister->m_baseData.id == id) {
                return true;
            }
            return false;
        });
    return it != s_logPersisters.end();
}

std::shared_ptr<LogPersister> LogPersister::GetLogPersisterById(uint32_t id)
{
    std::lock_guard<decltype(s_logPersistersMtx)> guard(s_logPersistersMtx);

    auto it = std::find_if(s_logPersisters.begin(), s_logPersisters.end(),
        [&](const std::shared_ptr<LogPersister>& logPersister) {
            if (logPersister->m_baseData.id == id) {
                return true;
            }
            return false;
        });
    if (it == s_logPersisters.end()) {
        return std::shared_ptr<LogPersister>();
    }
    return *it;
}

void LogPersister::RegisterLogPersister(const std::shared_ptr<LogPersister>& obj)
{
    std::lock_guard<decltype(s_logPersistersMtx)> lock(s_logPersistersMtx);
    s_logPersisters.push_back(obj);
}

void LogPersister::DeregisterLogPersister(const std::shared_ptr<LogPersister>& obj)
{
    if (!obj) {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid invoke - this should never happened!\n";
        return;
    }
    std::lock_guard<decltype(s_logPersistersMtx)> lock(s_logPersistersMtx);
    auto it = std::find_if(s_logPersisters.begin(), s_logPersisters.end(),
        [&](const std::shared_ptr<LogPersister>& logPersister) {
            if (logPersister->m_baseData.id == obj->m_baseData.id) {
                return true;
            }
            return false;
        });
    if (it == s_logPersisters.end()) {
        std::cerr << __PRETTY_FUNCTION__ << " Inconsistent data - this should never happended!\n";
        return;
    }
    s_logPersisters.erase(it);
}
} // namespace HiviewDFX
} // namespace OHOS
