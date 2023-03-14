/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef EVENT_LOGGER_PEER_BINDER_LOG_CATCHER
#define EVENT_LOGGER_PEER_BINDER_LOG_CATCHER
#include <string>
#include <memory>
#include <vector>

#include "sys_event.h"

#include "event_log_catcher.h"
namespace OHOS {
namespace HiviewDFX {
class PeerBinderCatcher : public EventLogCatcher {
public:
    explicit PeerBinderCatcher();
    ~PeerBinderCatcher() override{};
    bool Initialize(const std::string& strParam1, int pid, int layer) override;
    int Catch(int fd) override;
    void Init(std::shared_ptr<SysEvent> event, std::string filePath);
    void SetBinderProcPath(std::string filePath)
    {
        binderProcPath_ = filePath;
    };

    static const inline std::string LOGGER_EVENT_PEERBINDER = "PeerBinder";
    static const inline std::string LOGGER_BINDER_DEBUG_PROC_PATH = "/sys/kernel/debug/binder/proc/";
    static const inline std::string LOGGER_BINDER_TRACING_PROC_PATH = "/sys/kernel/tracing/binder/proc/";
private:
    struct BinderInfo {
        int client;
        int server;
        unsigned long wait;
    };

    enum {
        LOGGER_BINDER_STACK_HEAD_TAIL = -2,
        LOGGER_BINDER_STACK_ALL = -1,
        LOGGER_BINDER_STACK_NONE = 0,
        LOGGER_BINDER_STACK_ONE
    };

    static constexpr int TRACE_THRESHOLD = 6;
    static constexpr int PEER_TRACE_NUM = 3;
    static constexpr int BINDER_TIMEOUT = 1;

    int pid_ = 0;
    int layer_;
    std::shared_ptr<SysEvent> event_ = nullptr;
    std::string binderProcPath_ = "";

    void SaveBinderTransactionInfo(int fd) const;
    std::set<int> GetBinderPeerChainPids(int pid, int layer, int fd) const;
    std::set<int> GetBinderPeerPids(int pid, long layer) const;
    std::set<int> GetPeerBinderByLayer (int pid, int layer,
        std::map<int, std::list<BinderInfo>> info, int fd) const;
    std::map<int, std::list<BinderInfo>> BinderInfoParser(int pid) const;
    std::string GetBinderProcPath() const;
    void ParseBinderCallChain(int pid, std::map<int, std::list<BinderInfo>> info,
    std::vector<int> path, std::map<int, bool> &visit, std::vector<std::vector<int>> &paths) const;
    int GetMatchBinderPeerInfo(int &pid, int &tid, int &waitTime, std::string &matchLine) const;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENT_LOGGER_PEER_BINDER_LOG_CATCHER