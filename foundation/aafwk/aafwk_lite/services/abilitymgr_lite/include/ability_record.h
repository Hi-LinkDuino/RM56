/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RECORD_H
#define OHOS_ABILITY_RECORD_H

#ifdef __LITEOS_M__
#include "cmsis_os.h"
#endif
#include "ability_info.h"

namespace OHOS {
class JsAppHost;

constexpr int FAIL_CALLBACK_ERRORCODE = 200;

/* States-feedback from launcher to AMS */
typedef enum {
    SCHEDULE_INITED,
    SCHEDULE_INACTIVE,
    SCHEDULE_ACTIVE,
    SCHEDULE_BACKGROUND,
    SCHEDULE_STOP
} AbilityState;

typedef enum {
    STATE_JS_RUNNING,
    STATE_NATIVE_RUNNING,
    STATE_JS_JUMP_NATIVE,
    STATE_NATIVE_JUMP_JS,
    STATE_JS_JUMP_JS,
} AppState;

class AbilityRecord {
public:
    AbilityRecord();
    ~AbilityRecord();

    void SetAppName(const char *appName);
    const char *GetAppName() const
    {
        return appName_;
    }

    void SetAppPath(const char *appPath);
    const char *GetAppPath() const
    {
        return appPath_;
    }

    void SetAppData(const void *appData, uint16_t dataLength);
    const void *GetAppData() const
    {
        return appData_;
    }

    void SetAbilityInfo(const AbilityInfo abilityInfo)
    {
        abilityInfo_ = abilityInfo;
    }

    AbilityInfo GetAbilityInfo() const
    {
        return abilityInfo_;
    }

    uint16_t GetDataLength() const
    {
        return dataLength_;
    }

    void SetState(const AbilityState state)
    {
        state_ = state;
    }

    AbilityState GetState() const
    {
        return state_;
    }

    bool IsAttached() const
    {
        return state_ != SCHEDULE_STOP;
    }

    void SetToken(uint16_t token)
    {
        token_ = token;
    }
    uint16_t GetToken() const
    {
        return token_;
    }

    void SetTaskId(uint32_t taskId)
    {
        taskId_ = taskId;
    }

    uint32_t GetTaskId() const
    {
        return taskId_;
    }

    void SetTerminated(bool isTerminated)
    {
        isTerminated_ = isTerminated;
    }

    bool IsTerminated() const
    {
        return isTerminated_;
    }

    void SetMessageQueueId(const osMessageQueueId_t jsAppQueueId)
    {
        jsAppQueueId_ = jsAppQueueId;
    }

    const osMessageQueueId_t& GetMessageQueueId() const
    {
        return jsAppQueueId_;
    }

    void SetJsAppHost(const JsAppHost *jsAppHost)
    {
        jsAppHost_ = const_cast<JsAppHost *>(jsAppHost);
    }

    const JsAppHost *GetJsAppHost() const
    {
        return jsAppHost_;
    }

    static void CopyAbilityRecord(AbilityRecord &abilityRecord, AbilityRecord &newAbilityRecord);

private:
    char *appName_ { nullptr };
    char *appPath_ { nullptr };
    AbilityInfo abilityInfo_;
    void *appData_ { nullptr };
    uint16_t dataLength_ { 0 };
    AbilityState state_ { SCHEDULE_STOP };
    uint16_t token_ { 0 };
    uint32_t taskId_ { 0 };
    osMessageQueueId_t jsAppQueueId_ { nullptr };
    JsAppHost *jsAppHost_ { nullptr };
    bool isTerminated_ = false;
};
} // namespace OHOS

#endif // OHOS_ABILITY_RECORD_H
