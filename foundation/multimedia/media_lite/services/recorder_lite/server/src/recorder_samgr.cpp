/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "feature.h"
#include "iproxy_server.h"
#include "media_log.h"
#include "recorder_service.h"
#include "samgr_lite.h"
#include "service.h"
#include <ohos_errno.h>
#include <ohos_init.h>

namespace OHOS {
namespace Media {
struct DefaultFeatureApi {
    INHERIT_SERVER_IPROXY;
};

typedef struct RecorderService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
} RecorderService;

static const char *GetName(Service *service)
{
    (void)service;
    return RECORDER_SERVICE_NAME;
}

static BOOL Initialize(Service *service, Identity identity)
{
    RecorderService *recordSvc = (RecorderService *)service;
    recordSvc->identity = identity;
    MEDIA_INFO_LOG("Initialize(%s)! Identity<%d, %d, %p>", RECORDER_SERVICE_NAME, identity.serviceId,
                   identity.featureId, identity.queueId);
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *msg)
{
    MEDIA_DEBUG_LOG("MessageHandle(%s)! Request<%d, %d, %p>", service->GetName(service), msg->msgId, msg->msgValue,
                    msg->data);
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
    MEDIA_DEBUG_LOG("Service Remote Invoke is called! <%p, %d, %p, %p, %p>", iProxy, funcId, origin, req, reply);
    pid_t pid = GetCallingPid(origin);
    RecorderClientMng *mng = RecorderClientMng::GetInstance();
    mng->Dispatch(funcId, pid, req, reply);
    return EC_SUCCESS;
}

void RecorderServiceReg()
{
    static RecorderService recorderSvc = {
        .GetName = GetName,
        .Initialize = Initialize,
        .MessageHandle = MessageHandle,
        .GetTaskConfig = GetTaskConfig,
        SERVER_IPROXY_IMPL_BEGIN,
        .Invoke = Invoke,
        IPROXY_END,
    };
    BOOL ret = SAMGR_GetInstance()->RegisterService((Service *)&recorderSvc);
    if (ret != TRUE) {
        MEDIA_ERR_LOG("Recorder regist service failed.");
        return;
    }
    ret = SAMGR_GetInstance()->RegisterDefaultFeatureApi(RECORDER_SERVICE_NAME, GET_IUNKNOWN(recorderSvc));
    if (ret != TRUE) {
        MEDIA_ERR_LOG("Recorder regist feature failed.");
        return;
    }
}
} // namespace Media
} // namespace OHOS