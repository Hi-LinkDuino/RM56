/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <ohos_errno.h>
#include <ohos_init.h>
#include "audio_capturer_server.h"
#include "feature.h"
#include "iproxy_server.h"
#include "media_log.h"
#include "samgr_lite.h"
#include "service.h"

namespace OHOS {
namespace Audio {
struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

struct AudioCapturerService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
};

static const char *GetName(Service *service)
{
    (void)service;
    return AUDIO_CAPTURER_SERVICE_NAME;
}

static BOOL Initialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        MEDIA_ERR_LOG("Initialize: service is nullptr");
        return FALSE;
    }

    AudioCapturerService *capturerSvc = reinterpret_cast<AudioCapturerService*>(service);
    capturerSvc->identity = identity;
    MEDIA_DEBUG_LOG("Initialize(%s)! Identity<%d, %d, %d>",
                    AUDIO_CAPTURER_SERVICE_NAME, identity.serviceId,
                    identity.featureId, reinterpret_cast<void*>(&identity.queueId));
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *msg)
{
    if (service == nullptr || msg == nullptr) {
        MEDIA_ERR_LOG("MessageHandle: service or msg is nullptr");
        return FALSE;
    }

    MEDIA_DEBUG_LOG("MessageHandle(%s)! Request<%d, %d, %d>", service->GetName(service),
                    msg->msgId, msg->msgValue,
                    reinterpret_cast<void*>(&msg->data));
    return FALSE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, 0x800, 20, SHARED_TASK};
    return config;
}

static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    if (origin == nullptr) {
        MEDIA_ERR_LOG("Invoke: origin is nullptr");
        return FALSE;
    }

    pid_t pid = GetCallingPid(origin);
    AudioCapturerServer *mng = AudioCapturerServer::GetInstance();
    if (mng == nullptr) {
        MEDIA_ERR_LOG("Invoke failed, mng is nunnptr");
        return FALSE;
    }

    mng->Dispatch(funcId, pid, req, reply);
    return EC_SUCCESS;
}

void AudioCapturerServiceReg()
{
    static AudioCapturerService audioCapturerSvc = {
        .GetName = GetName,
        .Initialize = Initialize,
        .MessageHandle = MessageHandle,
        .GetTaskConfig = GetTaskConfig,
        SERVER_IPROXY_IMPL_BEGIN,
        .Invoke = Invoke,
        IPROXY_END,
    };
    MEDIA_INFO_LOG("Input AudioCapturerServiceReg");
    bool ret = SAMGR_GetInstance()->RegisterService(reinterpret_cast<Service*>(&audioCapturerSvc));
    if (!ret) {
        MEDIA_ERR_LOG("AudioCapturer regist service failed.");
        return;
    }
    ret = SAMGR_GetInstance()->RegisterDefaultFeatureApi(AUDIO_CAPTURER_SERVICE_NAME, GET_IUNKNOWN(audioCapturerSvc));
    if (!ret) {
        MEDIA_ERR_LOG("AudioCapturer regist feature failed.");
        return;
    }
}
}  // namespace Audio
}  // namespace OHOS
