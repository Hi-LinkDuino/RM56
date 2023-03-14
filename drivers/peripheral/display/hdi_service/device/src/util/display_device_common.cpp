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

#include "display_device_common.h"
#undef HDF_LOG_TAG
#define HDF_LOG_TAG DisplayCommon

using namespace OHOS;
using OHOS::MessageParcel;

DispPowerStatus Convert2PowerStatus(int32_t x)
{
    return ((x < POWER_STATUS_ON) || (x > POWER_STATUS_BUTT)) ? POWER_STATUS_BUTT : static_cast<DispPowerStatus>(x);
}
CompositionType Convert2CompositionType(int32_t x)
{
    return ((x < COMPOSITION_CLIENT) || (x > COMPOSITION_BUTT)) ? COMPOSITION_BUTT : static_cast<CompositionType>(x);
}
TransformType Convert2TransformType(int32_t x)
{
    return ((x < ROTATE_NONE) || (x > ROTATE_BUTT)) ? ROTATE_BUTT : static_cast<TransformType>(x);
}

BlendType Convert2BlendTypeType(int32_t x)
{
    return ((x < BLEND_NONE) || (x > BLEND_BUTT)) ? BLEND_BUTT : static_cast<BlendType>(x);
}

bool DisplayDeviceWriteUint32(MessageParcel *parcel, uint32_t data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(sizeof data)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }
    if (!parcel->WriteUint32(data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }

    return true;
}
bool DisplayDeviceReadUint32(uint32_t *data, OHOS::MessageParcel *parcel)
{
    uint32_t retValueLen = 0;
    if (sizeof(uint32_t) != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG("error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen,
            sizeof(uint32_t));
        return false;
    }
    *data = parcel->ReadUint32();

    return true;
}

bool DisplayDeviceWriteInt32(OHOS::MessageParcel *parcel, int32_t data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(sizeof data)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }
    if (!parcel->WriteInt32(data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }

    return true;
}
bool DisplayDeviceReadInt32(int32_t *data, OHOS::MessageParcel *parcel)
{
    uint32_t retValueLen = 0;
    if (sizeof(int32_t) != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG("error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen,
            sizeof(int32_t));
        return false;
    }
    *data = parcel->ReadInt32();
    return true;
}

bool DisplayDeviceWriteBool(OHOS::MessageParcel *parcel, bool data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(sizeof data)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }
    if (!parcel->WriteInt32(data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }
    return true;
}
bool DisplayDeviceReadBool(bool *data, OHOS::MessageParcel *parcel)
{
    uint32_t retValueLen = 0;
    if (sizeof(bool) != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG(
            "error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen, sizeof(bool));
        return false;
    }
    *data = parcel->ReadBool();

    return true;
}

bool DisplayDeviceWriteUint64(OHOS::MessageParcel *parcel, uint64_t data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(sizeof data)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }
    if (!parcel->WriteUint64(data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }

    return true;
}

bool DisplayDeviceReadUint64(uint64_t *data, OHOS::MessageParcel *parcel)
{
    uint32_t retValueLen = 0;
    if (sizeof(uint64_t) != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG("error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen,
            sizeof(uint64_t));
        return false;
    }
    *data = parcel->ReadUint64();

    return true;
}

bool DisplayDeviceWritePtr(OHOS::MessageParcel *parcel, uintptr_t data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(sizeof data)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }
    if (!parcel->WritePointer(data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }

    return true;
}
bool DisplayDeviceReadPtr(uintptr_t *&data, OHOS::MessageParcel *parcel)
{
    uint32_t retValueLen = 0;
    if (sizeof(uintptr_t) != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG("error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen,
            sizeof(uintptr_t));
        return false;
    }
    *data = parcel->ReadPointer();

    return true;
}

bool DisplayDeviceWriteCmdId(MessageParcel *parcel, DisplayDeviceCommandId cmdId)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel is nullptr");
        return false;
    }
    if (!parcel->WriteUint32(cmdId)) {
        DISPLAY_LOG("error: write cmdId into parcel failed");
        return false;
    }

    return true;
}

DisplayDeviceCommandId DisplayDeviceReadCmdId(MessageParcel *parcel)
{
    uint32_t enumTmp = parcel->ReadUint32();
    enumTmp &= ~BATCH_CMD_FLAG;

    return ((enumTmp > DSP_CMD_EXECUTECMD && enumTmp < DSP_CMD_RESERVED_1001)
               || (enumTmp > DSP_CMD_SET_PROXY_REMOTE_CALLBACK && enumTmp < DSP_CMD_RESERVED_2001)
               || (enumTmp > DSP_CMD_SETLAYERBLENDTYPE))
        ? DSP_CMD_INVALID
        : static_cast<DisplayDeviceCommandId>(enumTmp);
}

bool DisplayDeviceWriteBufHdl(MessageParcel *parcel, const BufferHandle *data)
{
    if (parcel == nullptr) {
        DISPLAY_LOG("error: MessageParcel iss nullptr");
        return false;
    }
    if (!WriteBufferHandle(*parcel, *data)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }

    return true;
}
bool DisplayDeviceReadBufHdl(BufferHandle *&data, OHOS::MessageParcel *parcel)
{
    data = ReadBufferHandle(*parcel);
    if (data == nullptr) {
        DISPLAY_LOG("read reply data failed");
        return false;
    }

    return true;
}

bool DisplayDeviceWriteFileDescriptor(MessageParcel *parcel, const int32_t fd)
{
    bool validFd = (fd >= 0);
    if (!parcel->WriteBool(validFd)) {
        DISPLAY_LOG("error: %{public}s parcel->WriteBool failed", __func__);
        return false;
    }
    if (validFd && !parcel->WriteFileDescriptor(fd)) {
        DISPLAY_LOG("error: %{public}s parcel->WriteFileDescriptor fd failed", __func__);
        return false;
    } else if (!validFd) {
        DISPLAY_LOG("%{public}s Write invalid FileDescriptor", __func__);
        return true;
    }
    return true;
}

bool DisplayDeviceReadFileDescriptor(int32_t *fd, MessageParcel *parcel)
{
    bool validFd = false;
    if (!parcel->ReadBool(validFd)) {
        DISPLAY_LOG("error: %{public}s ReadBool validFd failed", __func__);
        return false;
    }
    if (validFd) {
        *fd = parcel->ReadFileDescriptor();
        if (*fd == -1) {
            DISPLAY_LOG("error: %{public}s ReadFileDescriptor fd failed", __func__);
            return false;
        }
    } else {
        *fd = -1;
        DISPLAY_LOG("%{public}s Read invalid FileDescriptor", __func__);
    }
    return true;
}

bool DisplayDeviceWriteFileDescriptorArray(MessageParcel *parcel, const int *fd, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++) {
        DISPLAY_LOG("%{public}s(%{public}d), fence fd:%{public}d", __func__, __LINE__, fd[i]);
        if (!DisplayDeviceWriteFileDescriptor(parcel, fd[i])) {
            DISPLAY_LOG(
                "error: %{public}s parcel->WriteFileDescriptor fence failed, fence fd:%{public}d", __func__, fd[i]);
            return false;
        }
    }
    return true;
}

bool DisplayDeviceReadFileDescriptorArray(int *fd, MessageParcel *parcel, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++) {
        if (!DisplayDeviceReadFileDescriptor(&fd[i], parcel)) {
            DISPLAY_LOG("error: %{public}s parcel->ReadFileDescriptor fence failed", __func__);
            return false;
        }
    }
    return true;
}
