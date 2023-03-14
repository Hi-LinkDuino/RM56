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

#ifndef PREFERENCES_IMPL_H
#define PREFERENCES_IMPL_H

#include <any>
#include <condition_variable>
#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "preferences.h"
#include "preferences_observer.h"
#include "preferences_value.h"
#include "task_pool.h"

namespace OHOS {
namespace NativePreferences {
class PreferencesImpl : public Preferences {
public:
    explicit PreferencesImpl(const std::filesystem::path &path);
    virtual ~PreferencesImpl();

    int Init();

    int GetInt(const std::string &key, int defValue) override;

    std::string GetString(const std::string &key, const std::string &defValue) override;

    bool GetBool(const std::string &key, bool defValue) override;

    float GetFloat(const std::string &key, float defValue) override;

    double GetDouble(const std::string &key, double defValue) override;

    int64_t GetLong(const std::string &key, int64_t defValue) override;

    std::set<std::string> GetStringSet(const std::string &key, std::set<std::string> &defValue) override;

    bool HasKey(const std::string &key) override;

    int PutInt(const std::string &key, int value) override;

    int PutString(const std::string &key, const std::string &value) override;

    int PutBool(const std::string &key, bool value) override;

    int PutLong(const std::string &key, int64_t value) override;

    int PutFloat(const std::string &key, float value) override;

    int PutDouble(const std::string &key, double value) override;

    int PutStringSet(const std::string &key, const std::set<std::string> &value) override;

    std::map<std::string, PreferencesValue> GetAll() override;

    int Delete(const std::string &key) override;

    int Clear() override;

    void Flush() override;

    int FlushSync() override;

    void RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;

    void UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;

    static std::filesystem::path MakeBackupPath(const std::filesystem::path &prefPath);
    static std::filesystem::path MakeBrokenPath(const std::filesystem::path &prefPath);

private:
    class MemoryToDiskRequest {
    public:
        MemoryToDiskRequest(const std::map<std::string, PreferencesValue> &writeToDiskMap,
            const std::list<std::string> &keysModified,
            const std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers, int64_t memStataGeneration);
        ~MemoryToDiskRequest() {}
        void SetDiskWriteResult(bool wasWritten, int result);

        bool isSyncRequest_;
        int64_t memoryStateGeneration_;
        std::map<std::string, PreferencesValue> writeToDiskMap_;
        std::mutex reqMutex_;
        std::condition_variable reqCond_;
        std::list<std::string> keysModified_;
        std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers_;

        int writeToDiskResult_;
        bool wasWritten_;
        bool handled_;
    };

    std::shared_ptr<MemoryToDiskRequest> commitToMemory();
    void notifyPreferencesObserver(const MemoryToDiskRequest &request);
    void PutPreferencesValue(const std::string &key, const PreferencesValue &value);
    void StartLoadFromDisk();
    int CheckKey(const std::string &key);
    int CheckStringValue(const std::string &value);

    /* thread function */
    static void LoadFromDisk(PreferencesImpl &pref);
    void AwaitLoadFile();
    void WriteToDiskFile(std::shared_ptr<MemoryToDiskRequest> mcr);
    bool CheckRequestValidForStateGeneration(const MemoryToDiskRequest &mcr);

    bool ReadSettingXml(const std::filesystem::path &prefPath, std::map<std::string, PreferencesValue> &prefMap);
    bool WriteSettingXml(const std::filesystem::path &prefPath, const std::map<std::string, PreferencesValue> &prefMap);

    bool loaded_;

    /* Current memory state (always increasing) */
    int64_t currentMemoryStateGeneration_;
    /* Latest memory state that was committed to disk */
    int64_t diskStateGeneration_;

    std::mutex mutex_;
    std::condition_variable cond_;

    std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers_;
    std::map<std::string, PreferencesValue> map_;
    std::list<std::string> modifiedKeys_;

    const std::filesystem::path filePath_;
    const std::filesystem::path backupPath_;
    const std::filesystem::path brokenPath_;
    // Task pool
    /* max threads of the task pool. */
    static constexpr int MAX_TP_THREADS = 10;
    /* min threads of the task pool. */
    static constexpr int MIN_TP_THREADS = 1;
    TaskPool taskPool_;
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_IMPL_H
