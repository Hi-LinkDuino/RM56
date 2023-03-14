/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_SERVICE_H
#define RECORDER_SERVICE_H

#include "liteipc_adapter.h"
#include "recorder_common.h"
#include "recorder_impl.h"
#include "serializer.h"

namespace OHOS {
namespace Media {
/* Since IPC is serialized, there is no concurrency problem */
class RecorderClientMng {
public:
    ~RecorderClientMng() = default;

    static RecorderClientMng *GetInstance();
    RecorderImpl *GetRecorder(pid_t pid);
    bool AcceptClient(pid_t pid);
    void DropClient(pid_t pid);
    void Dispatch(int32_t funcId, pid_t pid, IpcIo *req, IpcIo *reply);

private:
    RecorderClientMng() = default;
    SvcIdentity svc_;
    pid_t client_ = -1;
    RecorderImpl *rec_ = nullptr;
};

class RecorderCallbackClient : public RecorderCallback {
public:
    RecorderCallbackClient() = delete;
    ~RecorderCallbackClient() = default;
    RecorderCallbackClient(SvcIdentity *sid) : sid_(sid) {}

    void OnError(int32_t errorType, int32_t errorCode) override;
    void OnInfo(int32_t type, int32_t extra) override;

private:
    SvcIdentity *sid_ = nullptr;
};

void RecorderServiceReg();
}  // namespace Media
}  // namespace OHOS
#endif  // RECORDER_SERVICE_H
