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
#include "cmsis.h"
#include "cmsis_os2.h"
#include "plat_addr_map.h"
#include "hal_cache.h"
#include "hal_dsi.h"
#include "hdf_log.h"
#include "mipi_dsi_core.h"

#define HDF_LOG_TAG mipi_dsi

#ifndef PSRAMUHS_SIZE
#define PSRAMUHS_SIZE 0x800000
#endif

#define WIDTH 454
#define HEIGHT 454
#define BUFSIZE_MAX 0xE2000
#define BUF_NUM 3
#if (BUF_NUM < 2 || BUF_NUM > 3)
#error "BUF_NUM must be 2 or 3"
#endif

enum BUF_STATE {
    IDLE,
    READY,
    BUSY,
};

struct MipiDsiDevice {
    uint32_t buffers[BUF_NUM];
    uint32_t buf_size;
    volatile uint8_t buf_idx;
    volatile uint8_t free_chan;
    volatile enum BUF_STATE buf_state;
    enum DsiMode mode;
    struct HAL_DSI_CFG_T cfg;
};

static struct MipiDsiDevice priv = {
    .buffers = {
        PSRAMUHS_BASE + PSRAMUHS_SIZE - BUFSIZE_MAX,
        PSRAMUHS_BASE + PSRAMUHS_SIZE - BUFSIZE_MAX * 2,
#if (BUF_NUM == 3)
        PSRAMUHS_BASE + PSRAMUHS_SIZE - BUFSIZE_MAX * 3,
#endif
    },
    .buf_size = BUFSIZE_MAX,
    .buf_idx = 0,
    .free_chan = 0,
    .buf_state = IDLE,
    .mode = DSI_CMD_MODE,
    .cfg = {
        .active_width = WIDTH + 1,
        .active_height = HEIGHT + 1,
        .h_front_porch = 13,
        .h_back_porch = 13,
        .v_front_porch = 13,
        .v_back_porch = 13,
        .total_width = WIDTH + 32,
        .total_height = HEIGHT + 32,

        .g_mem_pitch = WIDTH * 4,
        .graphic_h_sa = 1,
        .graphic_v_sa = 1,
        .graphic_width = WIDTH,
        .graphic_height = HEIGHT,
        .zm_graphic_width = WIDTH,
        .zm_graphic_height = HEIGHT,

        .cursor_h_sa = 90,
        .cursor_v_sa = 90,
        .cursor_width = 28,
        .cursor_height = 28,
        .hwc_color1 = 0x0000ff,
        .hwc_color2 = 0x00ff00,

        .blankcolor = 0x104060,

        .cos0 = 0x3fc2,
        .sin0 = 0x05a4,
        .c_mult_s = 0x3000,
        .saturation = 0x5000,
        .brightness = 0xfff8,
        .contrast = 0x5000,

        .cfg_alpha_y = 0x00,
        .cfg_ckey_y = 0x16,
        .cfg_ckey_y1 = 0x0,
        .cfg_ckey_y2 = 0x60,
        .cfg_alpha_u = 0xff,
        .cfg_ckey_u = 0x80,
        .cfg_ckey_u1 = 0x0,
        .cfg_ckey_u2 = 0x80,
        .cfg_alpha_v = 0xff,
        .cfg_ckey_v = 0x80,
        .cfg_ckey_v1 = 0x0,
        .cfg_ckey_v2 = 0x80,
    },
};

static int32_t MipiDsiDevInit()
{
    for (int i = 0; i < HDF_ARRAY_SIZE(priv.buffers); i++) {
        memset((void *)priv.buffers[i], 0, BUFSIZE_MAX);
        hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)priv.buffers[i], BUFSIZE_MAX);
        HDF_LOGD("buffer[%d] 0x%x", i, priv.buffers[i]);
    }
    return HDF_SUCCESS;
}

static int32_t MipiDsiDevSetCntlrCfg(struct MipiDsiCntlr *cntlr)
{
    enum DSI_MODE_T dsi_mode = DSI_MODE_CMD;
    uint8_t dsi_lane = (uint8_t)cntlr->cfg.lane;
    uint32_t dsi_bitclk = cntlr->cfg.phyDataRate;
    uint32_t dsi_pclk = cntlr->cfg.pixelClk;
    priv.cfg.active_width = cntlr->cfg.timing.xPixels;
    priv.cfg.active_height = cntlr->cfg.timing.ylines;
    // FIXME
    if (cntlr->cfg.mode == DSI_CMD_MODE) {
        priv.cfg.active_width += 1;
        priv.cfg.active_height += 1;
    } else if (cntlr->cfg.mode == DSI_VIDEO_MODE) {
        dsi_mode = DSI_MODE_VIDEO;
    }
    priv.mode = cntlr->cfg.mode;
    priv.cfg.h_back_porch = cntlr->cfg.timing.hbpPixels;
    priv.cfg.v_front_porch = cntlr->cfg.timing.vfpLines;
    priv.cfg.v_back_porch = cntlr->cfg.timing.vbpLines;
    priv.cfg.total_width = cntlr->cfg.timing.hlinePixels;
    priv.cfg.total_height = cntlr->cfg.timing.ylines + cntlr->cfg.timing.vfpLines +
                            cntlr->cfg.timing.vbpLines + cntlr->cfg.timing.vsaLines;

    priv.cfg.g_mem_pitch = cntlr->cfg.timing.xPixels * 4;
    priv.cfg.graphic_width = cntlr->cfg.timing.xPixels;
    priv.cfg.graphic_height = cntlr->cfg.timing.ylines;
    priv.cfg.zm_graphic_width = cntlr->cfg.timing.xPixels;
    priv.cfg.zm_graphic_height = cntlr->cfg.timing.ylines;

    HDF_LOGD("%s: width %u, height %u, dsi_bitclk %u, dsi_pclk %u", __func__,
             priv.cfg.active_width, priv.cfg.active_height, dsi_bitclk, dsi_pclk);
    /* Init the hardware and clear the display */
    hal_dsi_init_v2(cntlr->cfg.timing.xPixels, dsi_mode, dsi_lane, dsi_bitclk, dsi_pclk);
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

static void MipiDsiDevCallback(uint8_t layerId, uint8_t channel, uint32_t addr)
{
    uint8_t ready_buf_idx;
    priv.free_chan = channel;
    switch (priv.buf_state) {
    case READY:
        // free channel = next channel
        if (priv.buf_idx >= 1) {
            ready_buf_idx = priv.buf_idx - 1;
        } else {
            ready_buf_idx = BUF_NUM - 1;
        }
        hal_lcdc_update_addr(1, channel, priv.buffers[ready_buf_idx]);
        priv.buf_state = (priv.mode == DSI_VIDEO_MODE) ? BUSY : IDLE;
        break;
    case BUSY:
        priv.buf_state = IDLE;
        break;
    default:
        break;
    }
}

static void MipiDsiDevToHs(struct MipiDsiCntlr *cntlr)
{
    (void)cntlr;
    static bool init = false;
    if (!init) {
        hal_lcdc_init(&priv.cfg, NULL, (const uint8_t *)priv.buffers[priv.buf_idx], NULL);
        hal_dsi_start();
        hal_lcdc_set_callback(MipiDsiDevCallback);
        init = true;
    }
}

static void MipiDsiDevToLp(struct MipiDsiCntlr *cntlr)
{
    (void)cntlr;
}

void *MipiDsiDevMmap(uint32_t size)
{
    static uint8_t last_buf = 0;
    if (size > BUFSIZE_MAX) {
        HDF_LOGE("%s: invalid size 0x%x", __func__, size);
        return NULL;
    }
    priv.buf_size = size;
#if (BUF_NUM < 3)
    // ensure the buffers are exchanged
    uint32_t cnt = 0;
    while (priv.buf_state == READY) {
        if (cnt++ > 100) {
            HDF_LOGW("READY -> BUSY error, chan %d, cur_buf %d", priv.free_chan, priv.buf_idx);
            break;
        }
        osDelay(1);
    }
#endif
    if (last_buf != priv.buf_idx) {
        memset((void *)(priv.buffers[priv.buf_idx]), 0, priv.buf_size);
        last_buf = priv.buf_idx;
    }
    return (void *)(priv.buffers[priv.buf_idx]);
}

void MipiDsiDevFlush(void)
{
    uint32_t cnt = 0;
    hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)priv.buffers[priv.buf_idx], priv.buf_size);
    while (priv.buf_state != IDLE) {
        if (cnt++ > 100) {
            HDF_LOGW("!IDLE, stat %d, cur_buf %d", priv.buf_state, priv.buf_idx);
            break;
        }
        osDelay(1);
    }
    uint32_t irqflags = int_lock();
    if (priv.mode == DSI_VIDEO_MODE) {
        hal_lcdc_update_addr(1, priv.free_chan, priv.buffers[priv.buf_idx]);
    } else {
        hal_lcdc_update_addr(1, priv.buf_idx, priv.buffers[priv.buf_idx]);
        hal_lcdc_start(); // trigger transmission, used in DSI_CMD_MODE
    }
    priv.buf_state = READY;
    priv.buf_idx = (priv.buf_idx + 1) % BUF_NUM;
    int_unlock(irqflags);
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
