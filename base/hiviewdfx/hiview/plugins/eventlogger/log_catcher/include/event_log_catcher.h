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
#ifndef EVENT_LOGGER_EVENT_LOG_CATCHER_H
#define EVENT_LOGGER_EVENT_LOG_CATCHER_H
#include <ctime>
#include <string>
namespace OHOS {
namespace HiviewDFX {
// do not dynamic alloc any memory in sub class, may cause leakage
class EventLogCatcher {
public:

    EventLogCatcher() {};
    int GetLogSize() const;
    int AppendFile(int fd, const std::string &fileName) const;
    void SetLogSize(int size);
    void Stop();
    virtual ~EventLogCatcher() {};
    // Initialize a logcater and check whether all parameters are matching the log type
    virtual bool Initialize(const std::string &strParam1, int intParam1, int intParam2);
    // start catch the log
    // return the bytes that caught
    virtual int Catch(int fd);
    // retun a string that describe the log catcher
    // should not be longer than 256 bytes
    virtual std::string GetDescription() const;
    int GetFdSize(int32_t fd);

    std::string GetName() const
    {
        return name_;
    }
protected:
    int logSize_ = -1;
    time_t catcherStartTime_ = -1;
    bool useStreamFilter_ = false;
    volatile bool needStop_ = false;
    std::string description_ = "";
    std::string name_ = "";
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENT_LOGGER_LOG_CATCHER_H
