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

#include <ohos_errno.h>
#include <iproxy_server.h>

#include <pthread.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "screen_saver_feature.h"

static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply);

static ScreenSaverFeature g_feature = {
    SCREEN_SAVER_FEATURE_INTERFACE_IMPL,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = FeatureInvoke,
    SCREEN_SAVER_INTERFACE_IMPL,
    IPROXY_END,
    .identity = { -1, -1, NULL },
};

ScreenSaverFeature *GetScreenSaverFeatureImpl(void)
{
    return &g_feature;
}

static int32_t SetStateInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply)
{
    if (reply == NULL) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }
    bool enable = IpcIoPopBool(req);
    int32_t ret = OnSetScreenSaverState((IUnknown *)iProxy, enable ? TRUE : FALSE);
    IpcIoPushInt32(reply, ret);
    return EC_SUCCESS;
}

static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    if ((iProxy == NULL) || (origin == NULL) || (req == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }
    POWER_HILOGD("Screen saver feature invoke function id: %d", funcId);
    int32_t ret = EC_SUCCESS;
    switch (funcId) {
        case SCREENSAVER_FUNCID_SETSTATE:
            ret = SetStateInvoke(iProxy, origin, req, reply);
            break;
        default:
            POWER_HILOGE("Invalid function id: %d", funcId);
            break;
    }
    return ret;
}
