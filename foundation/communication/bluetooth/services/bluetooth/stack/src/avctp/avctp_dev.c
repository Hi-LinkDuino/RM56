/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "avctp_dev.h"
#include "avctp_int.h"
#include "avctp_st.h"
#include "log.h"
#include "securec.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*****************************************************************************
 * Function  Define
 ****************************************************************************/
/*
 * Function     AvctGetCbDevByAddress
 * Description  This function is called to get the device block by peer device address.
 * Param[in]    peerAddr  point of the peer address
 * Return       AvctCbDev the point of control channel block.
 */
AvctCbDev *AvctGetCbDevByAddress(const BtAddr *peerAddr)
{
    LOG_INFO("[AVCT] %{public}s: ", __func__);
    AvctCbDev *cbDev = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_DEVICES; i++) {
        if ((g_avctMng.cbDev[i].alloced) &&
            (!memcmp((char *)g_avctMng.cbDev[i].peerAddr.addr, (char *)&(peerAddr->addr), BT_ADDR_LENGTH))) {
            cbDev = &g_avctMng.cbDev[i];
            LOG_DEBUG("[AVCT] %{public}s: Device id is %hhu", __func__, i);
            break;
        }
    }
    return cbDev;
}

/*
 * Function     AvctGetCbDevByChId
 * Description  This function is called to get the device block by channel link id.
 * Param[in]    chId  channel link id
 * Return       AvctCbDev  the point of device block.
 */
AvctCbDev *AvctGetCbDevByChId(uint16_t chId)
{
    LOG_INFO("[AVCT] %{public}s: channel id is 0x%x", __func__, chId);
    AvctCbDev *cbDev = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_DEVICES; i++) {
        LOG_DEBUG("[AVCT] %{public}s: g_avctMng.cbDev[%hhu].chLink is %hhu", __func__, i, g_avctMng.cbDev[i].chLink);
        if (((g_avctMng.cbDev[i].chLink & AVCT_ALLOC_CTRL) && (g_avctMng.cbDev[i].cbCtrl->chId == chId)) ||
            ((g_avctMng.cbDev[i].chLink & AVCT_ALLOC_BR) && (g_avctMng.cbDev[i].cbBr->chId == chId))) {
            cbDev = &g_avctMng.cbDev[i];
            LOG_DEBUG("[AVCT] %{public}s: Device id is %hhu", __func__, i);
            break;
        }
    }
    return cbDev;
}

/*
 * Function     AvctCbDevAlloc
 * Description  This function is called to alloce memory for the device block.
 * Param[in]    peerAddr  device address.
 * Return       AvctCbDev  the point of device block.
 */
AvctCbDev *AvctCbDevAlloc(const BtAddr *peerAddr)
{
    LOG_INFO("[AVCT] %{public}s:", __func__);
    AvctCbDev *cbDev = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_DEVICES; i++) {
        if (!g_avctMng.cbDev[i].alloced) {
            cbDev = &g_avctMng.cbDev[i];
            (void)memset_s(cbDev, sizeof(AvctCbDev), 0, sizeof(AvctCbDev));
            cbDev->alloced = true;
            (void)memcpy_s(&(cbDev->peerAddr), sizeof(BtAddr), peerAddr, sizeof(BtAddr));
            LOG_DEBUG("[AVCT] %{public}s: Device id is %hhu", __func__, i);
            break;
        }
    }
    return cbDev;
}

/*
 * Function     AvctCbDevDealloc
 * Description  This function is called to dealloce memory for the device block.
 * Param[in]    cbDev  The point to device block.
 * Return       void
 */
void AvctCbDevDealloc(AvctCbDev *cbDev)
{
    LOG_INFO("[AVCT] %{public}s: ", __func__);
    if (cbDev->cbCtrl != NULL) {
        AvctCbChDealloc(&(cbDev->cbCtrl));
    }
    if (cbDev->cbBr != NULL) {
        AvctCbChDealloc(&(cbDev->cbBr));
    }
    (void)memset_s(cbDev, sizeof(AvctCbDev), 0, sizeof(AvctCbDev));
    return;
}

/*
 * Function     AvctCbChAlloc
 * Description  This function is called to alloc memory for CbCtrl.
 * Param[in]   *connParam  oint to the info of the connection request.
 *              Such as the Role、Profile ID、Callback func point.
 * Param[in]    void
 * Return      AvctCbCh* channel link point.
 */
AvctCbCh *AvctCbChAlloc()
{
    LOG_INFO("[AVCT] %{public}s:", __func__);
    AvctCbCh *cbCh = malloc(sizeof(AvctCbCh));
    if (cbCh != NULL) {
        (void)memset_s(cbCh, sizeof(AvctCbCh), 0, sizeof(AvctCbCh));
    }
    return cbCh;
}

/*
 * Function     AvctCbChDealloc
 * Description  This function is called to dealloc memory for CbCtrl.
 * Param[in]   *cbCh  The point of the chanel block.
 * Return      void.
 */
void AvctCbChDealloc(AvctCbCh **cbCh)
{
    LOG_INFO("[AVCT] %{public}s:", __func__);
    if (*cbCh != NULL) {
        free(*cbCh);
        *cbCh = NULL;
    }
    return;
}