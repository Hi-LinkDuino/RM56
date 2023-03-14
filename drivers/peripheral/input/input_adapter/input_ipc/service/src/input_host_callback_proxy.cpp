/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "input_host_callback_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>

#define HDF_LOG_TAG InputCallbackProxy

namespace OHOS {
namespace Input {
void InputCallbackProxy::ReportEventPkgCallback(const EventPackage *pkgs, uint32_t count, uint32_t devIndex)
{
    MessageParcel data {};
    MessageParcel reply {};
    MessageOption option { MessageOption::TF_ASYNC };
    HDF_LOGE("%{public}s: count:%{public}d.", __func__, count);
    if (!pkgs) {
        HDF_LOGE("%{public}s: pkgs is null failed.", __func__);
        return;
    }
    if (!data.WriteInterfaceToken(InputCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }
    if (!data.WriteUint32(count)) {
        HDF_LOGE("%{public}s: write error count failed.", __func__);
        return;
    }
    if (!data.WriteUint32(devIndex)) {
        HDF_LOGE("%{public}s: write error devIndex failed.", __func__);
        return;
    }
    if (!data.WriteUint32((sizeof(EventPackage) * count))) {
        HDF_LOGE("%{public}s: write error length failed line %{public}d", __func__, __LINE__);
        return;
    }
    if (!data.WriteRawData((void*)(pkgs), (sizeof(EventPackage) * count))) {
        HDF_LOGE("%{public}s: write error pkgs failed.", __func__);
        return;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_CALLBACK_REMOTE_REPROT_EVENT_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void InputHotCallbackProxy::ReportHotPlugEventCallback(const HotPlugEvent *event)
{
    MessageParcel data {};
    MessageParcel reply {};
    MessageOption option {};
    if (!data.WriteInterfaceToken(InputHotCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }
    if (!data.WriteUint32(sizeof(HotPlugEvent))) {
        HDF_LOGE("%{public}s: write error length failed line %{public}d", __func__, __LINE__);
        return;
    }
    if (!data.WriteRawData((void*)event, sizeof(HotPlugEvent))) {
        HDF_LOGE("%{public}s: write event failed.", __func__);
        return;
    }
    int32_t ret = Remote()->SendRequest(CMD_INPUT_CALLBACK_REMOTE_REPROT_HOT_PLUG_EVENT_PROXY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}
}  // namespace Input
}  // namespace OHOS
