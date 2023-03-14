/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "mipi_dsi.h"
#include "cmsis_os2.h"
#include "plat_addr_map.h"
#include "hal_cache.h"
#include "hal_dsi.h"
#include "hal_lcdc.h"
#include "lcdc.h"
#define A7_DISPLAY 
#ifdef A7_DISPLAY
#include "best2003.h"
#endif
#include "hdf_log.h"
#include "mipi_dsi_core.h"

#define HDF_LOG_TAG mipi_dsi

#ifndef PSRAMUHS_SIZE
#define PSRAMUHS_SIZE 0x800000
#endif

#define BUFSIZE_MAX 0xE2000
#define BUF_NUM 2

enum BUF_STATE {
    IDLE,
    READY,
    BUSY,
};

struct MipiDsiDevice {
    uint32_t buffers[BUF_NUM];
    uint32_t buf_size;
    volatile uint8_t buf_idx;
    volatile enum BUF_STATE buf_state;
};

#ifdef A7_DISPLAY
static uint32_t g_buffer0[BUFSIZE_MAX / 4];
static uint32_t g_buffer1[BUFSIZE_MAX / 4];
#endif
static struct MipiDsiDevice priv = {
    .buffers = {
#ifdef A7_DISPLAY
        g_buffer0,
        g_buffer1,
#else
        PSRAMUHS_BASE + PSRAMUHS_SIZE - BUFSIZE_MAX,
        PSRAMUHS_BASE + PSRAMUHS_SIZE - BUFSIZE_MAX * 2,
#endif
    },
    .buf_size = BUFSIZE_MAX,
    .buf_idx = 0,
};

__attribute__((weak)) struct lcd_dev_s *board_lcd_getdev(int lcddev)
{
    return NULL;
}

static int32_t MipiDsiDevInit()
{
    for (int i = 0; i < HDF_ARRAY_SIZE(priv.buffers); i++) {
        memset((void *)priv.buffers[i], 0, BUFSIZE_MAX);
        HDF_LOGD("buffer[%d] 0x%x", i, priv.buffers[i]);
    }
    return HDF_SUCCESS;
}

static int32_t MipiDsiDevSetCntlrCfg(struct MipiDsiCntlr *cntlr)
{
    HDF_LOGD("%s: width %u, height %u, lane %u", __func__,
             cntlr->cfg.timing.xPixels, cntlr->cfg.timing.ylines, cntlr->cfg.lane);
    hal_dsi_init(cntlr->cfg.timing.xPixels, cntlr->cfg.lane);
    osDelay(100);
    return HDF_SUCCESS;
}

static int32_t MipiDsiDevSetCmd(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd)
{
    (void)cntlr;
    if (!cmd || !cmd->payload || cmd->dataLen < 1) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cmd->dataType == 0x05) {
        hal_dsi_send_cmd(cmd->payload[0]);
    } else {
        hal_dsi_send_cmd_list(cmd->payload[0], cmd->dataLen - 1, &cmd->payload[1]);
    }
    return HDF_SUCCESS;
}

int32_t MipiDsiDevGetCmd(struct MipiDsiCntlr *cntlr, struct DsiCmdDesc *cmd, uint32_t readLen, uint8_t *out)
{
    (void)cntlr;
    if (!cmd || !cmd->payload || !out || readLen < 1) {
        return HDF_ERR_INVALID_PARAM;
    }
    int ret = hal_dsi_read_cmd(cmd->payload[0], out, readLen);
    return (ret == readLen) ? HDF_SUCCESS : HDF_FAILURE;
}

static void MipiDsiDevCallback(int error)
{
    priv.buf_state = IDLE;
}

static void MipiDsiDevToHs(struct MipiDsiCntlr *cntlr)
{
    (void)cntlr;
    static bool init = false;
    if (!init) {
        lcdc_initialize(priv.buffers[priv.buf_idx], 0);
        lcdc_irq_callback_register(MipiDsiDevCallback);
        init = true;
    }
}

static void MipiDsiDevToLp(struct MipiDsiCntlr *cntlr)
{
    (void)cntlr;
}

void *MipiDsiDevMmap(uint32_t size)
{
    priv.buf_size = size;
    return (void *)(priv.buffers[priv.buf_idx]);
}

void MipiDsiDevFlush(void)
{
#ifdef A7_DISPLAY
    L1C_CleanInvalidateCacheRange(1, (size_t)priv.buffers[priv.buf_idx], (size_t)priv.buffers[priv.buf_idx] + priv.buf_size);
#else
    hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)priv.buffers[priv.buf_idx], priv.buf_size);
#endif
    uint32_t cnt = 0;
    while (priv.buf_state != IDLE) {
        if (cnt++ > 100) {
            HDF_LOGW("!IDLE, stat %d, cur_buf %d", priv.buf_state, priv.buf_idx);
            break;
        }
        osDelay(1);
    }

    hal_lcdc_lstartaddr(0, (uint32_t)priv.buffers[priv.buf_idx]);
    priv.buf_state = READY;
    priv.buf_idx = (priv.buf_idx + 1) % BUF_NUM;
    hal_lcdcl_enable(0, true);
    lcdc_update();
}

static int32_t MipiDsiDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t MipiDsiDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    int ret = MipiDsiDevInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiInit failed", __func__);
        return ret;
    }
    static struct MipiDsiCntlrMethod mipiDsiMethod = {
        .setCntlrCfg = MipiDsiDevSetCntlrCfg,
        .setCmd = MipiDsiDevSetCmd,
        .getCmd = MipiDsiDevGetCmd,
        .toHs = MipiDsiDevToHs,
        .toLp = MipiDsiDevToLp,
    };
    static struct MipiDsiService mipiDsiService = {
        .flush = MipiDsiDevFlush,
        .mmap = MipiDsiDevMmap,
    };
    static struct MipiDsiCntlr mipiDsiCntlr = {
        .devNo = 0,
        .ops = &mipiDsiMethod,
    };
    ret = MipiDsiRegisterCntlr(&mipiDsiCntlr, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MipiDsiRegisterCntlr failed", __func__);
        return ret;
    }
    mipiDsiCntlr.priv = &mipiDsiService; // must after MipiDsiRegisterCntlr
    return ret;
}

static void MipiDsiDriverRelease(struct HdfDeviceObject *device)
{
    struct MipiDsiCntlr *mipiDsiCntlr = MipiDsiCntlrFromDevice(device);
    if (mipiDsiCntlr) {
        MipiDsiUnregisterCntlr(mipiDsiCntlr);
        mipiDsiCntlr = NULL;
    }
}

static struct HdfDriverEntry g_MipiDsiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_MIPI_DSI",
    .Bind = MipiDsiDriverBind,
    .Init = MipiDsiDriverInit,
    .Release = MipiDsiDriverRelease,
};
HDF_INIT(g_MipiDsiDriverEntry);
