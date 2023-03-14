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

#include "ipc_camera_host_service_fuzzer.h"
#include "fuzz_base.h"

#include <cstddef>
#include <cstdint>

static uint32_t U32_AT(const uint8_t *ptr)
{
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

static int32_t onRemoteRequest(int cmdId, MessageParcel &data)
{
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<CameraHostStub> IPCHost = std::make_shared<CameraHostStub>();
    sleep(1);
    auto ret = IPCHost->CameraHostServiceStubOnRemoteRequest(cmdId, data, reply, option);
    return ret;
}

static void IpcFuzzService(const uint8_t *data, size_t size)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel dataMessageParcel;
    uint32_t code = U32_AT(data);
    uint8_t *number = data;
    number = number + sizeof(uint32_t);
    if (size > sizeof(uint32_t)) {
        if ((code == 0) || (code == 3)) { // 0:code size 3:code size
            return;
        }
        size_t length = size;
        length = length - sizeof(uint32_t);
        dataMessageParcel.WriteBuffer(number, length);
        dataMessageParcel.RewindRead(0);
        onRemoteRequest((int)code, dataMessageParcel);
    }
}

static void OnRemoteRequestFunc(const uint8_t *data, size_t size)
{
    IpcFuzzService(data, size);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OnRemoteRequestFunc(data, size);
    return 0;
}