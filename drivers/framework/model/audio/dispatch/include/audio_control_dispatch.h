/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CONTROL_DISPATCH_H
#define AUDIO_CONTROL_DISPATCH_H

#include "audio_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum ControlDispMethodCmd {
    AUDIODRV_CTRL_IOCTRL_ELEM_INFO,
    AUDIODRV_CTRL_IOCTRL_ELEM_READ,
    AUDIODRV_CTRL_IOCTRL_ELEM_WRITE,
    AUDIODRV_CTRL_IOCTRL_ELEM_BUTT,
};

struct ControlDispCmdHandleList {
    enum ControlDispMethodCmd cmd;
    int32_t (*func)(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply);
};

struct ControlHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    void *priv;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_CONTROL_H */
