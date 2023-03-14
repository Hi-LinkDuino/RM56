/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_DSI_CORE_H
#define MIPI_DSI_CORE_H

#include "osal_mutex.h"
#include "hdf_base.h"
#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"
#include "mipi_dsi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define MAX_CNTLR_CNT 4

struct MipiDsiCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    unsigned int devNo; /* device number */
    struct MipiCfg cfg;
    struct MipiDsiCntlrMethod *ops;
    struct OsalMutex lock;
    void *priv; /* be struct mipi_dsi_device */
};

struct MipiDsiCntlrMethod {
    int32_t (*setCntlrCfg)(struct MipiDsiCntlr *cntlr);
    int32_t (*setCmd)(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd);
    int32_t (*getCmd)(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd, uint32_t readLen, uint8_t *out);
    void (*toHs)(struct MipiDsiCntlr *cntlr);
    void (*toLp)(struct MipiDsiCntlr *cntlr);
    void (*enterUlps)(struct MipiDsiCntlr *cntlr);
    void (*exitUlps)(struct MipiDsiCntlr *cntlr);
    int32_t (*powerControl)(struct MipiDsiCntlr *cntlr, uint8_t enable);
    int32_t (*attach)(struct MipiDsiCntlr *cntlr, uint8_t *name);
    int32_t (*setDrvData)(struct MipiDsiCntlr *cntlr, void *panelData);
};

int32_t MipiDsiRegisterCntlr(struct MipiDsiCntlr *cntlr, struct HdfDeviceObject *device);
void MipiDsiUnregisterCntlr(struct MipiDsiCntlr *cntlr);

/**
  * @brief Turn HdfDeviceObject to an MipiDsiCntlr.
 *
 * @param device Indicates a HdfDeviceObject.
 *
 * @return Retrns the pointer of the MipiDsiCntlr on success; returns NULL otherwise.
 * @since 1.0
 */
struct MipiDsiCntlr *MipiDsiCntlrFromDevice(struct HdfDeviceObject *device);

/**
 * @brief Obtains the MIPI DSI device handle with a specified channel ID.
 *
 * @param id Indicates the MIPI DSI channel ID.
 *
 * @return Returns the MIPI DSI device if the operation is successful; returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct MipiDsiCntlr *MipiDsiCntlrOpen(uint8_t id);

/**
 * @brief Releases the MIPI DSI device handle.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiCntlrClose(struct MipiDsiCntlr *cntlr);

/**
 * @brief Sets configuration parameters for a MIPI DSI device.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param cfg Indicates the pointer to the configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrSetCfg(struct MipiDsiCntlr *cntlr, struct MipiCfg *cfg);

/**
 * @brief Obtains the configuration parameters of a MIPI DSI device.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param cfg Indicates the pointer to the configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrGetCfg(struct MipiDsiCntlr *cntlr, struct MipiCfg *cfg);

/**
 * @brief Sets LP mode for a MIPI DSI device.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiCntlrSetLpMode(struct MipiDsiCntlr *cntlr);

/**
 * @brief Sets the high-speed (HS) mode for a MIPI DSI device.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiCntlrSetHsMode(struct MipiDsiCntlr *cntlr);

/**
 * @brief Sends a display command set (DCS) command used for sending the initial parameters of a peripheral.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param cmd Indicates the pointer to the command to be sent.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrTx(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd);

/**
* @brief Receives a DCS command used for reading data, such as the status and parameters of a peripheral
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param cmd Indicates the pointer to the command to be received.
 * @param readLen Indicates the length of the data to read, in bytes.
 * @param out Indicates the pointer to the read data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrRx(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd, int32_t readLen, uint8_t *out);

/**
* @brief attach a DSI device to its DSI host
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param name Indicates the name of a peripheral.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrAttach(struct MipiDsiCntlr *cntlr, uint8_t *name);

/**
 * @brief Sets additional parameters for a MIPI DSI device.
 *
 * @param cntlr Indicates the MIPI DSI device obtained via {@link MipiDsiOpen}.
 * @param panelData Indicates the pointer to the additional parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiCntlrSetDrvData(struct MipiDsiCntlr *cntlr, void *panelData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
