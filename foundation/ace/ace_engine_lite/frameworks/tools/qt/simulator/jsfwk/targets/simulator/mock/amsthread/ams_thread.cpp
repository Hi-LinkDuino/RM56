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

#include "ams_thread.h"

#include "ability_manager_interface.h"
#include "ace_log.h"
#include "q_auto_lock_guard.h"

using namespace OHOS::ACELite;
static AMSThread *g_amsInstance = nullptr;

static bool IsAbilityInfoValid(const char *path, const char *bundleName)
{
    if (path == nullptr || bundleName == nullptr) {
        return false;
    }

    if (strlen(path) == 0 || strlen(bundleName) == 0) {
        return false;
    }

    if (strlen(path) >= APP_RECORD_STRING_BUFFER_MAX || strlen(bundleName) >= APP_RECORD_STRING_BUFFER_MAX) {
        return false;
    }

    return true;
}

int StartAbility(const char *path, const char *bundleName)
{
    if (g_amsInstance == nullptr) {
        return -1;
    }

    if (!IsAbilityInfoValid(path, bundleName)) {
        return -1;
    }

    auto message = new AMSMessage();
    if ((strcpy_s(message->path, APP_RECORD_STRING_BUFFER_MAX, path) != 0) ||
        (strcpy_s(message->bundle, APP_RECORD_STRING_BUFFER_MAX, bundleName) != 0)) {
        delete message;
        message = nullptr;
        return -1;
    }
    message->command = COMMAND_START_ABILITY;
    return g_amsInstance->PostMessage(message);
}

int TerminateAbility(uint64_t token)
{
    if (g_amsInstance == nullptr) {
        return -1;
    }

    auto message = new AMSMessage();
    message->command = COMMAND_TERMINATE_ABILITY;
    message->token = token;
    return g_amsInstance->PostMessage(message);
}

namespace OHOS {
namespace ACELite {
AMSThread::AMSThread()
{
    g_amsInstance = this;
}

void AMSThread::run()
{
    taskQuitFlag = false;
    while (!taskQuitFlag) {
        const AMSMessage *innerMsg = GetMessage();
        if (innerMsg == nullptr) {
            // error case
            return;
        }
        // the innerMsg will be freed out of the scop
        std::unique_ptr<const AMSMessage> msgGuard(innerMsg);
        switch (innerMsg->command) {
            case COMMAND_START_ABILITY: {
                StartAbilityInner(innerMsg->path, innerMsg->bundle);
                break;
            }
            case COMMAND_TERMINATE_ABILITY: {
                TerminateAbilityInner(innerMsg->token);
                break;
            }
            default: {
                break;
            }
        }
    }
    g_amsInstance = nullptr;
}

void AMSThread::Quit()
{
    taskQuitFlag = true;
    g_amsInstance = nullptr;
}

int AMSThread::PostMessage(const AMSMessage *message)
{
    if (message == nullptr) {
        return -1;
    }

    QAutoLockGuard lockGuard(mutexlock_);
    msgQueue_.enqueue(message);
    // notify there is new message
    queueCondition_.wakeAll();
    return 0;
}

const AMSMessage *AMSThread::GetMessage()
{
    QAutoLockGuard lockGuard(mutexlock_);
    if (msgQueue_.isEmpty()) {
        // wait until any new message is putted into
        queueCondition_.wait(&mutexlock_);
    }
    const AMSMessage *msg = msgQueue_.dequeue();
    return msg;
}

int AMSThread::StartAbilityInner(const char *path, const char *bundleName)
{
    if (!IsAbilityInfoValid(path, bundleName)) {
        return -1;
    }

    JSAppRecord *exsitingApp = GetAppRecordByInfo(path, bundleName);
    if (exsitingApp != nullptr) {
        // already started and top
        if (exsitingApp->token == currentTopToken_) {
            return 0;
        }
        JSAppRecord *currentTopApp = GetAppRecord(currentTopToken_);
        if (currentTopApp != nullptr) {
            MoveAppToTargetState(*currentTopApp, AbilityMsgId::BACKGROUND);
        }
        MoveAppToTargetState(*exsitingApp, AbilityMsgId::ACTIVE);
        currentTopToken_ = exsitingApp->token;
        return 0;
    }

    JSAppRecord *newAppRecord = CreateNewAppRecord(path, bundleName);
    if (newAppRecord == nullptr) {
        return -1;
    }
    // no activing app
    if (currentTopToken_ == INVALID_TOKEN) {
        MoveAppToTargetState(*newAppRecord, AbilityMsgId::START);
        MoveAppToTargetState(*newAppRecord, AbilityMsgId::ACTIVE);
        currentTopToken_ = newAppRecord->token;
        return 0;
    }

    // already there is one activated app, move it to background and active the new one
    JSAppRecord *currentTop = GetAppRecord(currentTopToken_);
    if (currentTop == nullptr) {
        currentTopToken_ = INVALID_TOKEN;
        return -1;
    }
    if (currentTop->jsApp == nullptr) {
        DeleteAppRecord(currentTopToken_);
        currentTopToken_ = INVALID_TOKEN;
        return -1;
    }
    // background current one
    MoveAppToTargetState(*currentTop, AbilityMsgId::BACKGROUND);
    // change new to the active one
    MoveAppToTargetState(*newAppRecord, AbilityMsgId::START);
    MoveAppToTargetState(*newAppRecord, AbilityMsgId::ACTIVE);
    currentTopToken_ = newAppRecord->token;
    return 0;
}

int AMSThread::TerminateAbilityInner(uint64_t token)
{
    JSAppRecord *appRecord = PopAppRecord(token);
    if (appRecord == nullptr) {
        return -1;
    }
    MoveAppToTargetState(*appRecord, AbilityMsgId::BACKGROUND);
    if (appRecord->token == currentTopToken_) {
        // try get another one
        JSAppRecord *next = (appRecords_.isEmpty()) ? nullptr : appRecords_.at(0);
        if (next) {
            MoveAppToTargetState(*next, AbilityMsgId::ACTIVE);
        }
    }
    MoveAppToTargetState(*appRecord, AbilityMsgId::DESTORY);
    delete appRecord;
    appRecord = nullptr;
    return 0;
}

void AMSThread::MoveAppToTargetState(JSAppRecord &appRecord, AbilityMsgId state)
{
    if (appRecord.jsApp == nullptr) {
        return;
    }

    appRecord.jsApp->PostCommand(state);
    appRecord.state = state;
    if (state == AbilityMsgId::ACTIVE) {
        currentTopToken_ = appRecord.token;
    }
    const uint32_t waitTime = 10;
    QThread::msleep(waitTime); // sleep a little while for giving enough time to handle the command
}

uint64_t AMSThread::GenerateToken()
{
    QAutoLockGuard lockGuard(tokenLock_);
    if (nextToken_ >= UINT16_MAX) {
        nextToken_ = 0;
    }
    nextToken_++;
    uint64_t token = nextToken_;
    return token;
}

JSAppRecord *AMSThread::CreateNewAppRecord(const char *path, const char *bundleName)
{
    uint64_t token = GenerateToken();
    // create and start JS thread loop
    JSThread *newApp = new JSThread();
    newApp->ConfigAbilityInfo(path, bundleName, token);
    newApp->start();
    // create new record
    auto newAppRecord = new JSAppRecord();
    newAppRecord->token = token;
    newAppRecord->jsApp = newApp;
    if ((strcpy_s(newAppRecord->appPath, APP_RECORD_STRING_BUFFER_MAX, path) != 0) ||
        (strcpy_s(newAppRecord->bundleName, APP_RECORD_STRING_BUFFER_MAX, bundleName) != 0)) {
        newApp->quit();
        delete newApp;
        newApp = nullptr;
        delete newAppRecord;
        newAppRecord = nullptr;
        return nullptr;
    }
    // add to list
    QAutoLockGuard lockGuard(recordsLock_);
    appRecords_.append(newAppRecord);
    return newAppRecord;
}

JSAppRecord *AMSThread::GetAppRecord(uint64_t token)
{
    recordsLock_.lock();
    JSAppRecord *existing = nullptr;
    for (auto record : appRecords_) {
        if (record->token == token) {
            existing = record;
            break;
        }
    }
    recordsLock_.unlock();
    return existing;
}

JSAppRecord *AMSThread::PopAppRecord(uint64_t token)
{
    recordsLock_.lock();
    JSAppRecord *existing = nullptr;
    for (auto record : appRecords_) {
        if (record->token == token) {
            existing = record;
            break;
        }
    }
    if (existing != nullptr) {
        appRecords_.removeOne(existing);
    }
    recordsLock_.unlock();
    return existing;
}

void AMSThread::DeleteAppRecord(uint64_t token)
{
    recordsLock_.lock();
    JSAppRecord *existing = nullptr;
    for (auto record : appRecords_) {
        if (record->token == token) {
            existing = record;
            break;
        }
    }
    if (existing != nullptr) {
        appRecords_.removeOne(existing);
        delete existing;
        existing = nullptr;
    }
    recordsLock_.unlock();
}

JSAppRecord *AMSThread::GetAppRecordByInfo(const char *path, const char *bundleName)
{
    if (path == nullptr || bundleName == nullptr) {
        return nullptr;
    }

    if (strlen(path) >= APP_RECORD_STRING_BUFFER_MAX || strlen(bundleName) >= APP_RECORD_STRING_BUFFER_MAX) {
        return nullptr;
    }
    recordsLock_.lock();
    JSAppRecord *existing = nullptr;
    for (auto record : appRecords_) {
        if (strcmp(path, record->appPath) == 0 && strcmp(bundleName, record->bundleName) == 0) {
            existing = record;
            break;
        }
    }
    recordsLock_.unlock();
    return existing;
}
} // namespace ACELite
} // namespace OHOS
