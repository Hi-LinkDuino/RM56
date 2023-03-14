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

#include "input_host_callback_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "input_host_callback.h"
#include "securec.h"

#define HDF_LOG_TAG InputReportEventCallbackStub

namespace OHOS {
namespace Input {
int32_t InputReportEventCallbackStub::OnRemoteRequest(uint32_t code,
                                                      MessageParcel &data,
                                                      MessageParcel &reply,
                                                      MessageOption &option)
{
    switch (code) {
        case CMD_INPUT_CALLBACK_REMOTE_REPROT_EVENT: {
            if (data.ReadInterfaceToken() != InputReportEventCallbackStub::GetDescriptor()) {
                HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
                return INPUT_INVALID_PARAM;
            }
            uint32_t count = data.ReadUint32();
            uint32_t devIndex = data.ReadUint32();
            uint32_t readLen =  data.ReadUint32();
            if (readLen > sizeof(EventPackage)) {
                HDF_LOGE("rev EventPackage data readLen failed");
                return 0;
            }
            EventPackage* datatmp = (EventPackage*)OsalMemAlloc(readLen);
            if (datatmp == nullptr) {
                HDF_LOGE("%{public}s: OsalMemAlloc failed", __func__);
                return 0;
            }
            (void)memset_s(datatmp, readLen, 0, readLen);
            const EventPackage* pkgs = (const EventPackage*)(data.ReadRawData(readLen));
            (void)memcpy_s(datatmp, readLen, pkgs, readLen);
            HDF_LOGE("rev data readLen: %{public}d pkgs:%{public}p count:%{public}d", readLen, pkgs, count);
            pkgs = nullptr;
            ReportEventPkgCallback((const EventPackage*)datatmp, count, devIndex);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}

int32_t InputReportHostEventCallbackStub::OnRemoteRequest(uint32_t code,
                                                          MessageParcel &data,
                                                          MessageParcel &reply,
                                                          MessageOption &option)
{
    switch (code) {
        case CMD_INPUT_CALLBACK_REMOTE_REPROT_HOT_PLUG_EVENT: {
            if (data.ReadInterfaceToken() != InputReportHostEventCallbackStub::GetDescriptor()) {
                HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
                return INPUT_INVALID_PARAM;
            }
            uint32_t readLen =  data.ReadUint32();
            if (readLen > sizeof(HotPlugEvent)) {
                HDF_LOGE("rev hotplug data readLen failed");
                return 0;
            }
            HotPlugEvent* datatmp = (HotPlugEvent*)OsalMemAlloc(readLen);
            if (datatmp == nullptr) {
                HDF_LOGE("%{public}s: OsalMemAlloc failed", __func__);
                return 0;
            }
            int32_t ret = memset_s(datatmp, readLen, 0, readLen);
            if (ret != 0) {
                HDF_LOGE("memset_s faild : %{public}s", __func__);
                return 0;
            }
            HotPlugEvent* msg = (HotPlugEvent*)(data.ReadRawData(readLen));
            ret = memcpy_s(datatmp, readLen, msg, readLen);
            if (ret != 0) {
                HDF_LOGE("memset_s faild : %{public}s", __func__);
                return 0;
            }
            msg = nullptr;
            HDF_LOGE("rev hotplug data readLen: %{public}d pkgs:%{public}p", readLen, msg);
            ReportHotPlugEventCallback(datatmp);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}
}  // namespace Input
}  // namespace OHOS
