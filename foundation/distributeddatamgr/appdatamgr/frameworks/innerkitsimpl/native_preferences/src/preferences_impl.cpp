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

#include "preferences_impl.h"

#include <cinttypes>
#include <climits>
#include <cstdlib>

#include <functional>

#include "logger.h"
#include "preferences_errno.h"
#include "preferences_xml_utils.h"
#include "securec.h"

namespace OHOS {
namespace NativePreferences {
static bool IsFileExist(const std::filesystem::path &inputPath)
{
    char path[PATH_MAX + 1] = { 0x00 };
    if (strlen(inputPath.c_str()) > PATH_MAX || realpath(inputPath.c_str(), path) == nullptr) {
        return false;
    }
    const char *pathString = path;
    FILE *file = std::fopen(pathString, "r");
    if (file != nullptr) {
        std::fclose(file);
        return true;
    }
    return false;
}

PreferencesImpl::PreferencesImpl(const std::filesystem::path &filePath)
    : loaded_(false), filePath_(filePath), backupPath_(MakeBackupPath(filePath_)),
      brokenPath_(MakeBrokenPath(filePath_)), taskPool_(TaskPool(MAX_TP_THREADS, MIN_TP_THREADS))
{
    currentMemoryStateGeneration_ = 0;
    diskStateGeneration_ = 0;
}

std::filesystem::path PreferencesImpl::MakeBackupPath(const std::filesystem::path &prefPath)
{
    std::filesystem::path backupPath = prefPath;
    backupPath += ".bak";
    return backupPath;
}

std::filesystem::path PreferencesImpl::MakeBrokenPath(const std::filesystem::path &prefPath)
{
    std::filesystem::path brokenPath = prefPath;
    brokenPath += ".bak";
    return brokenPath;
}

PreferencesImpl::~PreferencesImpl()
{
    taskPool_.Stop();
}

int PreferencesImpl::Init()
{
    int errCode = taskPool_.Start();
    if (errCode != E_OK) {
        return errCode;
    }
    StartLoadFromDisk();
    return E_OK;
}

void PreferencesImpl::StartLoadFromDisk()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loaded_ = false;
    }
    taskPool_.Schedule(std::string("PreferencesImpl"), std::bind(&PreferencesImpl::LoadFromDisk, std::ref(*this)));
}

int PreferencesImpl::CheckKey(const std::string &key)
{
    if (key.empty()) {
        LOG_ERROR("The key string is null or empty.");
        return E_KEY_EMPTY;
    }
    if (Preferences::MAX_KEY_LENGTH < key.length()) {
        LOG_ERROR("The key string length should shorter than 80.");
        return E_KEY_EXCEED_MAX_LENGTH;
    }
    return E_OK;
}

/* static */
void PreferencesImpl::LoadFromDisk(PreferencesImpl &pref)
{
    std::lock_guard<std::mutex> lock(pref.mutex_);
    if (pref.loaded_) {
        return;
    }

    if (IsFileExist(pref.backupPath_)) {
        if (std::remove(pref.filePath_.c_str())) {
            LOG_ERROR("Couldn't delete file %{private}s when LoadFromDisk and backup exist.", pref.filePath_.c_str());
        }
        if (std::rename(pref.backupPath_.c_str(), pref.filePath_.c_str())) {
            LOG_ERROR("Couldn't rename backup file %{private}s to file %{private}s,when LoadFromDisk and backup "
                      "exist.",
                pref.backupPath_.c_str(), pref.filePath_.c_str());
        } else {
            PreferencesXmlUtils::LimitXmlPermission(pref.filePath_);
        }
    }

    if (IsFileExist(pref.filePath_)) {
        pref.ReadSettingXml(pref.filePath_, pref.map_);
    }

    pref.loaded_ = true;
    pref.cond_.notify_all();
}

void PreferencesImpl::AwaitLoadFile()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!loaded_) {
        cond_.wait(lock, [this] { return loaded_; });
    }
}

void PreferencesImpl::WriteToDiskFile(std::shared_ptr<MemoryToDiskRequest> mcr)
{
    bool fileExists = false;
    if (IsFileExist(filePath_)) {
        fileExists = true;
    }

    if (fileExists) {
        bool needWrite = CheckRequestValidForStateGeneration(*mcr);
        if (!needWrite) {
            mcr->SetDiskWriteResult(false, E_OK);
            return;
        }

        if (IsFileExist(backupPath_)) {
            if (std::remove(filePath_.c_str())) {
                LOG_ERROR("Couldn't delete file %{private}s when writeToFile and backup exist.", filePath_.c_str());
            }
        } else {
            if (std::rename(filePath_.c_str(), backupPath_.c_str())) {
                LOG_ERROR("Couldn't rename file %{private}s to backup file %{private}s", filePath_.c_str(),
                    backupPath_.c_str());
                mcr->SetDiskWriteResult(false, E_ERROR);
                return;
            } else {
                PreferencesXmlUtils::LimitXmlPermission(backupPath_);
            }
        }
    }

    if (WriteSettingXml(filePath_, mcr->writeToDiskMap_)) {
        if (IsFileExist(backupPath_) && std::remove(backupPath_.c_str())) {
            LOG_ERROR("Couldn't delete backup file %{private}s when writeToFile finish.", backupPath_.c_str());
        }
        diskStateGeneration_ = mcr->memoryStateGeneration_;
        mcr->SetDiskWriteResult(true, E_OK);
    } else {
        /* Clean up an unsuccessfully written file */
        if (IsFileExist(filePath_)) {
            if (std::remove(filePath_.c_str())) {
                LOG_ERROR("Couldn't clean up partially-written file %{private}s", filePath_.c_str());
            }
        }
        mcr->SetDiskWriteResult(false, E_ERROR);
    }
}

bool PreferencesImpl::CheckRequestValidForStateGeneration(const MemoryToDiskRequest &mcr)
{
    bool valid = false;

    if (diskStateGeneration_ < mcr.memoryStateGeneration_) {
        if (mcr.isSyncRequest_) {
            valid = true;
        } else {
            if (currentMemoryStateGeneration_ == mcr.memoryStateGeneration_) {
                valid = true;
            }
        }
    }
    return valid;
}

int PreferencesImpl::GetInt(const std::string &key, int defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    int ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsInt()) {
            ret = val;
        }
    }
    return ret;
}

std::string PreferencesImpl::GetString(const std::string &key, const std::string &defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    std::string ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsString()) {
            ret = (std::string)val;
        }
    }
    return ret;
}

bool PreferencesImpl::GetBool(const std::string &key, bool defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    bool ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsBool()) {
            ret = val;
        }
    }
    return ret;
}

float PreferencesImpl::GetFloat(const std::string &key, float defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    float ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsFloat()) {
            ret = val;
        }
    }
    return ret;
}

double PreferencesImpl::GetDouble(const std::string &key, double defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    double ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsDouble()) {
            ret = val;
        }
    }
    return ret;
}

int64_t PreferencesImpl::GetLong(const std::string &key, int64_t defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    int64_t ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsLong()) {
            ret = val;
        }
    }
    return ret;
}

std::set<std::string> PreferencesImpl::GetStringSet(const std::string &key, std::set<std::string> &defValue)
{
    if (CheckKey(key) != E_OK) {
        return defValue;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    std::set<std::string> ret = defValue;

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue val = iter->second;
        if (val.IsSet()) {
            ret = val;
        }
    }
    return ret;
}

std::map<std::string, PreferencesValue> PreferencesImpl::GetAll()
{
    AwaitLoadFile();

    return map_;
}

bool PreferencesImpl::ReadSettingXml(
    const std::filesystem::path &prefPath, std::map<std::string, PreferencesValue> &prefMap)
{
    std::vector<Element> settings;
    if (!PreferencesXmlUtils::ReadSettingXml(prefPath.generic_string(), settings)) {
        LOG_ERROR("ReadSettingXml:%{private}s failed!", filePath_.c_str());
        return false;
    }

    for (auto it = settings.begin(); it != settings.end(); it++) {
        Element element = *it;
        if (element.tag_.compare("int") == 0) {
            int value = std::stoi(element.value_);
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(value)));
        } else if (element.tag_.compare("bool") == 0) {
            bool value = (element.value_.compare("true") == 0) ? true : false;
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(value)));
        } else if (element.tag_.compare("long") == 0) {
            int64_t value = static_cast<int64_t>(std::stoll(element.value_));
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(value)));
        } else if (element.tag_.compare("float") == 0) {
            float value = std::stof(element.value_);
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(value)));
        } else if (element.tag_.compare("double") == 0) {
            double value = std::stod(element.value_);
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(value)));
        } else if (element.tag_.compare("string") == 0) {
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(element.value_)));
        } else if (element.tag_.compare("set") == 0) {
            std::set<std::string> values;
            for (auto child : element.children_) {
                values.insert(child.value_);
            }
            prefMap.insert(std::make_pair(element.key_, PreferencesValue(values)));
        } else {
            LOG_WARN(
                "ReadSettingXml:%{private}s, unknown element tag:%{public}s.", prefPath.c_str(), element.tag_.c_str());
        }
    }
    return true;
}

bool PreferencesImpl::WriteSettingXml(
    const std::filesystem::path &prefPath, const std::map<std::string, PreferencesValue> &prefMap)
{
    std::vector<Element> settings;
    for (auto it = prefMap.begin(); it != prefMap.end(); it++) {
        Element elem;
        elem.key_ = it->first;

        PreferencesValue value = it->second;
        if (value.IsInt()) {
            elem.tag_ = std::string("int");
            elem.value_ = std::to_string((int)value);
        } else if (value.IsBool()) {
            elem.tag_ = std::string("bool");
            elem.value_ = std::to_string((bool)value);
        } else if (value.IsLong()) {
            elem.tag_ = std::string("long");
            elem.value_ = std::to_string((int64_t)value);
        } else if (value.IsFloat()) {
            elem.tag_ = std::string("float");
            elem.value_ = std::to_string((float)value);
        } else if (value.IsDouble()) {
            elem.tag_ = std::string("double");
            elem.value_ = std::to_string((double)value);
        } else if (value.IsString()) {
            elem.tag_ = std::string("string");
            elem.value_ = (std::string)value;
        } else if (value.IsSet()) {
            elem.tag_ = std::string("set");
            auto values = (std::set<std::string>)value;
            for (std::string val : values) {
                Element element;
                element.tag_ = std::string("string");
                element.value_ = (std::string)val;
                elem.children_.push_back(element);
            }
        } else {
            LOG_WARN("WriteSettingXml:%{private}s, unknown element type.", filePath_.c_str());
            continue;
        }
        settings.push_back(elem);
    }

    return PreferencesXmlUtils::WriteSettingXml(prefPath.generic_string(), settings);
}

bool PreferencesImpl::HasKey(const std::string &key)
{
    if (CheckKey(key) != E_OK) {
        return false;
    }

    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);
    return (map_.find(key) != map_.end());
}

void PreferencesImpl::RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::weak_ptr<PreferencesObserver> weakPreferencesObserver = preferencesObserver;
    preferencesObservers_.push_back(weakPreferencesObserver);
}

void PreferencesImpl::UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = preferencesObservers_.begin(); it != preferencesObservers_.end(); ++it) {
        std::weak_ptr<PreferencesObserver> weakPreferencesObserver = *it;
        std::shared_ptr<PreferencesObserver> sharedPreferencesObserver = weakPreferencesObserver.lock();
        if (!sharedPreferencesObserver || sharedPreferencesObserver == preferencesObserver) {
            preferencesObservers_.erase(it);
            break;
        }
    }
}

void PreferencesImpl::PutPreferencesValue(const std::string &key, const PreferencesValue &value)
{
    AwaitLoadFile();

    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = map_.find(key);
    if (iter != map_.end()) {
        PreferencesValue &val = iter->second;
        if (val == value) {
            return;
        }
    }

    map_.insert_or_assign(key, value);
    modifiedKeys_.push_back(key);
}

int PreferencesImpl::PutInt(const std::string &key, int value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::CheckStringValue(const std::string &value)
{
    if (Preferences::MAX_VALUE_LENGTH < value.length()) {
        LOG_ERROR("The value string length should shorter than 8 * 1024.");
        return E_VALUE_EXCEED_MAX_LENGTH;
    }
    return E_OK;
}

int PreferencesImpl::PutString(const std::string &key, const std::string &value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    errCode = CheckStringValue(value);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::PutBool(const std::string &key, bool value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::PutLong(const std::string &key, int64_t value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::PutFloat(const std::string &key, float value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::PutDouble(const std::string &key, double value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::PutStringSet(const std::string &key, const std::set<std::string> &value)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }
    for (auto child : value) {
        if (CheckStringValue(child) != E_OK) {
            return errCode;
        }
    }
    PutPreferencesValue(key, PreferencesValue(value));
    return E_OK;
}

int PreferencesImpl::Delete(const std::string &key)
{
    int errCode = CheckKey(key);
    if (errCode != E_OK) {
        return errCode;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    auto pos = map_.find(key);
    if (pos != map_.end()) {
        map_.erase(pos);
        modifiedKeys_.push_back(key);
    }

    return E_OK;
}

int PreferencesImpl::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!map_.empty()) {
        for (auto &kv : map_) {
            modifiedKeys_.push_back(kv.first);
        }
        map_.clear();
    }
    return E_OK;
}

void PreferencesImpl::Flush()
{
    std::shared_ptr<PreferencesImpl::MemoryToDiskRequest> request = commitToMemory();
    request->isSyncRequest_ = false;

    taskPool_.Schedule(
        std::string("PreferencesImpl"), std::bind(&PreferencesImpl::WriteToDiskFile, std::ref(*this), request));
    notifyPreferencesObserver(*request);
}

int PreferencesImpl::FlushSync()
{
    std::shared_ptr<PreferencesImpl::MemoryToDiskRequest> request = commitToMemory();
    request->isSyncRequest_ = true;

    taskPool_.Schedule(
        std::string("PreferencesImpl"), std::bind(&PreferencesImpl::WriteToDiskFile, std::ref(*this), request));
    std::unique_lock<std::mutex> lock(request->reqMutex_);
    request->reqCond_.wait(lock, [request] { return request->handled_; });
    if (request->wasWritten_) {
        LOG_DEBUG("%{private}s:%{public}" PRId64 " written", filePath_.c_str(), request->memoryStateGeneration_);
    }

    notifyPreferencesObserver(*request);
    return request->writeToDiskResult_;
}

std::shared_ptr<PreferencesImpl::MemoryToDiskRequest> PreferencesImpl::commitToMemory()
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t memoryStateGeneration = -1;
    std::list<std::string> keysModified;
    std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers;
    std::map<std::string, PreferencesValue> writeToDiskMap;
    writeToDiskMap = map_;
    if (!modifiedKeys_.empty()) {
        currentMemoryStateGeneration_++;
        keysModified = modifiedKeys_;
        modifiedKeys_.clear();
    }
    memoryStateGeneration = currentMemoryStateGeneration_;
    preferencesObservers = preferencesObservers_;
    return std::make_shared<MemoryToDiskRequest>(
        writeToDiskMap, keysModified, preferencesObservers, memoryStateGeneration);
}

void PreferencesImpl::notifyPreferencesObserver(const PreferencesImpl::MemoryToDiskRequest &request)
{
    if ((request.preferencesObservers_.empty()) || (request.keysModified_.empty())) {
        return;
    }
    for (auto key = request.keysModified_.begin(); key != request.keysModified_.end(); ++key) {
        for (auto it = request.preferencesObservers_.begin(); it != request.preferencesObservers_.end(); ++it) {
            std::weak_ptr<PreferencesObserver> weakPreferencesObserver = *it;
            if (std::shared_ptr<PreferencesObserver> sharedPreferencesObserver = weakPreferencesObserver.lock()) {
                sharedPreferencesObserver->OnChange(*this, *key);
            }
        }
    }
}

PreferencesImpl::MemoryToDiskRequest::MemoryToDiskRequest(
    const std::map<std::string, PreferencesValue> &writeToDiskMap, const std::list<std::string> &keysModified,
    const std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers, int64_t memStataGeneration)
{
    writeToDiskMap_ = writeToDiskMap;
    keysModified_ = keysModified;
    preferencesObservers_ = preferencesObservers;
    memoryStateGeneration_ = memStataGeneration;
    isSyncRequest_ = false;
    handled_ = false;
    wasWritten_ = false;
    writeToDiskResult_ = E_ERROR;
}

void PreferencesImpl::MemoryToDiskRequest::SetDiskWriteResult(bool wasWritten, int result)
{
    writeToDiskResult_ = result;
    wasWritten_ = wasWritten;
    handled_ = true;
    reqCond_.notify_one();
}
} // End of namespace NativePreferences
} // End of namespace OHOS
