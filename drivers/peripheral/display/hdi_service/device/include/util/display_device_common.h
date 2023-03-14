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

#ifndef _DISPLAY_DEVICE_COMMON_H_
#define _DISPLAY_DEVICE_COMMON_H_

#include <buffer_handle_parcel.h>
#include <display_type.h>
#include <fcntl.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <iservmgr_hdi.h>
#include <map>
#include <message_option.h>
#include <message_parcel.h>
#include <osal_mem.h>
#include <securec.h>
#include <unistd.h>
#include "display_device.h"
#include "display_layer.h"

#define DISPLAY_LOG(fmt, args...) HDF_LOGD("DispDevDbg %{public}s / %{public}d :" fmt, __func__, __LINE__, ##args)
#define DISPLAY_START DISPLAY_LOG("START")
#define DISPLAY_END DISPLAY_LOG("END")

const int32_t HDF_DISPLAY_DRIVER_FUNC_NUM_MAX = 50;
/*****cmdId bit status****
** 0000 0000 0000 1111 0000 0000 0000 0000 functionType
** 0000 0000 0000 0000 0000 0000 1111 1111 functionNum
** 0000 0000 0000 0000 0001 0000 0000 0000 isBatchCommand
** 0000 0000 0000 0000 0010 0000 0000 0000 BatchCommandEndFlag
** 1111 1111 1111 0000 1100 1111 0000 0000 reserved  */

/* single command data : ************    * mean parameter, E mean batch command end flag
** batch  command data : CL*CL*CL*N|E    N mean command number with batch flag
** common receive reply: CL*CL*CL*N|E    C mean command, L mean parameter length **/
const uint32_t BATCH_CMD_FLAG = 0x1000;
const int32_t COMPOSER_CMD_MAX_NUM = 128;
const int32_t STRMAXLEN = 100;
const int32_t COMPOSER_SERVER_ARRAY_NUMBER_MAX = 256;

enum {
    HDF_DISPLAY_DRIVER_FUNC_TYPE_INVALID,
    HDF_DISPLAY_DRIVER_FUNC_TYPE_DEVICE,
    HDF_DISPLAY_DRIVER_FUNC_TYPE_LAYER,
    HDF_DISPLAY_DRIVER_FUNC_TYPE_MAX
};
using DisplayDeviceCommandId = enum {
    DSP_CMD_INVALID = 0x00000,
    DSP_CMD_EXECUTECMD = 0x00001,
    // device
    DSP_CMD_RESERVED_1001 = 0x10000 | 1,
    DSP_CMD_RESERVED_1002 = 0x10000 | 2,
    DSP_CMD_REGHOTPLUGCALLBACK = 0x10000 | 3,
    DSP_CMD_GETDISPLAYCAPABILITY = 0x10000 | 4,
    DSP_CMD_GETDISPLAYSUPPORTEDMODES = 0x10000 | 5,
    DSP_CMD_GETDISPLAYMODE = 0x10000 | 6,
    DSP_CMD_SETDISPLAYMODE = 0x10000 | 7,
    DSP_CMD_GETDISPLAYPOWERSTATUS = 0x10000 | 8,
    DSP_CMD_SETDISPLAYPOWERSTATUS = 0x10000 | 9,
    DSP_CMD_GETDISPLAYBACKLIGHT = 0x10000 | 10,
    DSP_CMD_SETDISPLAYBACKLIGHT = 0x10000 | 11,
    DSP_CMD_GETDISPLAYPROPERTY = 0x10000 | 12,
    DSP_CMD_SETDISPLAYPROPERTY = 0x10000 | 13,
    DSP_CMD_PREPAREDISPLAYLAYERS = 0x10000 | 14 | 0x1000,
    DSP_CMD_RESERVED_1015 = 0x10000 | 15,
    DSP_CMD_GETDISPLAYCOMPCHANGE = 0x10000 | 16,
    DSP_CMD_RESERVED_1017 = 0x10000 | 17,
    DSP_CMD_SETDISPLAYCLIENTCROP = 0x10000 | 18,
    DSP_CMD_SETDISPLAYCLIENTDESTRECT = 0x10000 | 19,
    DSP_CMD_SETDISPLAYCLIENTBUFFER = 0x10000 | 20 | 0x1000,
    DSP_CMD_SETDISPLAYCLIENTDAMAGE = 0x10000 | 21 | 0x1000,
    DSP_CMD_SETDISPLAYVSYNCENABLED = 0x10000 | 22,
    DSP_CMD_RESERVED_1023 = 0x10000 | 23,
    DSP_CMD_RESERVED_1024 = 0x10000 | 24,
    DSP_CMD_REGDISPLAYVBLANKCALLBACK = 0x10000 | 25,
    DSP_CMD_RESERVED_1026 = 0x10000 | 26,
    DSP_CMD_GETDISPLAYRELEASEFENCE = 0x10000 | 27,
    DSP_CMD_COMMIT = 0x10000 | 28 | 0x1000,
    DSP_CMD_INVOKEDISPLAYCMD = 0x10000 | 29 | 0x1000,
    DSP_CMD_CREATEVIRTUALDISPLAY = 0x10000 | 30,
    DSP_CMD_DESTROYVIRTUALDISPLAY = 0x10000 | 31,
    DSP_CMD_SETVIRTUALDISPLAYBUFFER = 0x10000 | 32,
    DSP_CMD_REGDISPLAYREFRESHCALLBACK = 0x10000 | 33,
    DSP_CMD_GETWRITEBACKFRAME = 0x10000 | 34,
    DSP_CMD_CREATEWRITEBACK = 0x10000 | 35,
    DSP_CMD_DESTROYWRITEBACK = 0x10000 | 36,
    DSP_CMD_SET_PROXY_REMOTE_CALLBACK = 0x10000 | 37,
    // layer
    DSP_CMD_RESERVED_2001 = 0x20000 | 1,
    DSP_CMD_RESERVED_2002 = 0x20000 | 2,
    DSP_CMD_CREATELAYER = 0x20000 | 3,
    DSP_CMD_RESERVED_2004 = 0x20000 | 4,
    DSP_CMD_SETLAYERVISIBLE = 0x20000 | 5 | 0x1000,
    DSP_CMD_GETLAYERVISIBLESTATE = 0x20000 | 6,
    DSP_CMD_RESERVED_2007 = 0x20000 | 7,
    DSP_CMD_RESERVED_2008 = 0x20000 | 8,
    DSP_CMD_SETLAYERCROP = 0x20000 | 9 | 0x1000,
    DSP_CMD_SETLAYERZORDER = 0x20000 | 10 | 0x1000,
    DSP_CMD_GETLAYERZORDER = 0x20000 | 11,
    DSP_CMD_SETLAYERPREMULTI = 0x20000 | 12 | 0x1000,
    DSP_CMD_GETLAYERPREMULTI = 0x20000 | 13,
    DSP_CMD_SETLAYERALPHA = 0x20000 | 14 | 0x1000,
    DSP_CMD_GETLAYERALPHA = 0x20000 | 15,
    DSP_CMD_SETLAYERCOLORKEY = 0x20000 | 16 | 0x1000,
    DSP_CMD_GETLAYERCOLORKEY = 0x20000 | 17,
    DSP_CMD_SETLAYERPALETTE = 0x20000 | 18 | 0x1000,
    DSP_CMD_GETLAYERPALETTE = 0x20000 | 19,
    DSP_CMD_RESERVED_2020 = 0x20000 | 20,
    DSP_CMD_SETLAYERCOMPRESSION = 0x20000 | 21 | 0x1000,
    DSP_CMD_GETLAYERCOMPRESSION = 0x20000 | 22,
    DSP_CMD_RESERVED_2023 = 0x20000 | 23,
    DSP_CMD_FLUSH = 0x20000 | 24 | 0x1000,
    DSP_CMD_SETLAYERVISIBLEREGION = 0x20000 | 25 | 0x1000,
    DSP_CMD_SETLAYERDIRTYREGION = 0x20000 | 26 | 0x1000,
    DSP_CMD_GETLAYERBUFFER = 0x20000 | 27,
    DSP_CMD_SETLAYERBUFFER = 0x20000 | 28 | 0x1000,
    DSP_CMD_INVOKELAYERCMD = 0x20000 | 29 | 0x1000,
    DSP_CMD_SETLAYERCOMPOSITIONTYPE = 0x20000 | 30 | 0x1000,
    DSP_CMD_RESERVED_2031 = 0x20000 | 31,
    DSP_CMD_INITDISPLAY = 0x20000 | 32,
    DSP_CMD_DEINITDISPLAY = 0x20000 | 33,
    DSP_CMD_GETDISPLAYINFO = 0x20000 | 34,
    DSP_CMD_CLOSELAYER = 0x20000 | 35,
    DSP_CMD_SETLAYERSIZE = 0x20000 | 36 | 0x1000,
    DSP_CMD_GETLAYERSIZE = 0x20000 | 37,
    DSP_CMD_SETTRANSFORMMODE = 0x20000 | 38 | 0x1000,
    DSP_CMD_WAITFORVBLANK = 0x20000 | 39,
    DSP_CMD_SNAPSHOT = 0x20000 | 40,
    DSP_CMD_SETLAYERBLENDTYPE = 0x20000 | 41 | 0x1000
};
using HotPlugCallback = void (*)(uint32_t outputId, bool connected, void *data);
using VBlankCallback = void (*)(unsigned int sequence, uint64_t ns, void *data);
using PageFlipCallback = void (*)(unsigned int sequence, uint64_t ns, void *data);
using RefreshCallback = void (*)(uint32_t devId, void *data);

using CommandId = int32_t;
using BlendOpt = int32_t;

DispPowerStatus Convert2PowerStatus(int32_t x);
CompositionType Convert2CompositionType(int32_t x);
TransformType Convert2TransformType(int32_t x);
BlendType Convert2BlendTypeType(int32_t x);

bool DisplayDeviceWriteCmdId(OHOS::MessageParcel *parcel, DisplayDeviceCommandId cmdId);
DisplayDeviceCommandId DisplayDeviceReadCmdId(OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteUint32(OHOS::MessageParcel *parcel, uint32_t data);
bool DisplayDeviceReadUint32(uint32_t *data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteInt32(OHOS::MessageParcel *parcel, int32_t data);
bool DisplayDeviceReadInt32(int32_t *data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteBool(OHOS::MessageParcel *parcel, bool data);
bool DisplayDeviceReadBool(bool *data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteUint64(OHOS::MessageParcel *parcel, uint64_t data);
bool DisplayDeviceReadUint64(uint64_t *data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWritePtr(OHOS::MessageParcel *parcel, uintptr_t data);
bool DisplayDeviceReadPtr(uintptr_t *&data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteBufHdl(OHOS::MessageParcel *parcel, const BufferHandle *data);
bool DisplayDeviceReadBufHdl(BufferHandle *&data, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteFileDescriptor(OHOS::MessageParcel *parcel, const int fd);
bool DisplayDeviceReadFileDescriptor(int *fd, OHOS::MessageParcel *parcel);
bool DisplayDeviceWriteFileDescriptorArray(OHOS::MessageParcel *parcel, const int *fd, uint32_t num = 1);
bool DisplayDeviceReadFileDescriptorArray(int *fd, OHOS::MessageParcel *parcel, uint32_t num = 1);

template <typename T>
bool DisplayDeviceWriteData(OHOS::MessageParcel *parcel, const T *data, uint32_t num = 1)
{
    if (nullptr == parcel || nullptr == data || num == 0) {
        DISPLAY_LOG("error: OHOS::MessageParcel %{public}s nullptr, data %{public}s nullptr, num %{public}s 0",
            nullptr == parcel ? "is" : "is not", nullptr == data ? "is" : "is not", 0 == num ? "is" : "is not");
        return false;
    }

    if (!parcel->WriteUint32(sizeof(T) * num)) {
        DISPLAY_LOG("error: write data length into parcel failed");
        return false;
    }

    if (!parcel->WriteBuffer(data, sizeof(T) * num)) {
        DISPLAY_LOG("error: write data into parcel failed");
        return false;
    }
    return true;
}
template <typename T>
bool DisplayDeviceReadData(T *data, OHOS::MessageParcel *parcel, uint32_t num = 1)
{
    if (nullptr == parcel || nullptr == data || num == 0) {
        DISPLAY_LOG("error: OHOS::MessageParcel %{public}s nullptr, data %{public}s nullptr, num %{public}s 0",
            nullptr == parcel ? "is" : "is not", nullptr == data ? "is" : "is not", 0 == num ? "is" : "is not");
        return false;
    }

    uint32_t retValueLen = 0;
    if (sizeof(T) * num != (retValueLen = parcel->ReadUint32())) {
        DISPLAY_LOG(
            "error: wrong data length of proxy received: %{public}u, expected: %{public}zu", retValueLen, sizeof(T));
        return false;
    }

    T *dataTmp = (T *)parcel->ReadUnpadBuffer(retValueLen);
    if (dataTmp == nullptr) {
        DISPLAY_LOG("read buffer data failed");
        return false;
    }
    if (memcpy_s(data, num * sizeof(T), dataTmp, retValueLen) != EOK) {
        return HDF_ERR_INVALID_PARAM;
    }
    return true;
}

#endif // _DISPLAY_DEVICE_COMMON_H_
