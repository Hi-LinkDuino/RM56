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

#ifndef OHOS_ACELITE_MOCK_AMS_THREAD_H
#define OHOS_ACELITE_MOCK_AMS_THREAD_H

#include <QList>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

#include "js_thread.h"

// the mock thread of AMS service
namespace OHOS {
namespace ACELite {
constexpr uint16_t APP_RECORD_STRING_BUFFER_MAX = 256;
constexpr uint64_t INVALID_TOKEN = 0;

struct JSAppRecord {
    char appPath[APP_RECORD_STRING_BUFFER_MAX] = {0};
    char bundleName[APP_RECORD_STRING_BUFFER_MAX] = {0};
    JSThread *jsApp = nullptr;
    uint64_t token = INVALID_TOKEN;
    AbilityMsgId state = AbilityMsgId::UNKNOWN;
    ~JSAppRecord()
    {
        if (jsApp != nullptr) {
            const uint32_t waitTime = 20;
            while (jsApp->isRunning()) {
                QThread::msleep(waitTime);
            }
            delete jsApp;
            jsApp = nullptr;
        }
    }
};

enum AMSCommand {
    COMMNAD_UNKOWN = 0,
    COMMAND_START_ABILITY = 1,
    COMMAND_TERMINATE_ABILITY = 2,
};

struct AMSMessage {
    char path[APP_RECORD_STRING_BUFFER_MAX] = {0};
    char bundle[APP_RECORD_STRING_BUFFER_MAX] = {0};
    uint64_t token = INVALID_TOKEN;
    AMSCommand command = AMSCommand::COMMNAD_UNKOWN;
};

class AMSThread : public QThread {
public:
    AMSThread();
    virtual ~AMSThread() = default;
    void run() override;
    void Quit();
    int PostMessage(const AMSMessage *message);

private:
    const AMSMessage *GetMessage();
    int StartAbilityInner(const char *path, const char *bundleName);
    int TerminateAbilityInner(uint64_t token);
    JSAppRecord *CreateNewAppRecord(const char *path, const char *bundleName);
    JSAppRecord *GetAppRecord(uint64_t token);
    JSAppRecord *PopAppRecord(uint64_t token);
    JSAppRecord *GetAppRecordByInfo(const char *path, const char *bundleName);
    void DeleteAppRecord(uint64_t token);
    uint64_t GenerateToken();
    void MoveAppToTargetState(JSAppRecord &appRecord, AbilityMsgId state);
    volatile bool taskQuitFlag = false;
    uint64_t nextToken_ = 1; // begin with 1
    uint64_t currentTopToken_ = 0;
    QMutex tokenLock_;
    QList<JSAppRecord *> appRecords_;
    QMutex recordsLock_;
    QQueue<const AMSMessage *> msgQueue_;
    QWaitCondition queueCondition_;
    QMutex mutexlock_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_MOCK_AMS_THREAD_H
