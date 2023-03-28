/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bt_mesh.h
 * @brief:   vendor bt mesh apis defination
 * @author:
 * @date:    2019/12/15
 * @version: 1.0
 */

#include "tg_bt_mesh.h"
#include "bt_adaptor_dbg.h"
#include "ble_adv_adp.h"
#include "gapm_msg.h"
#include "stdlib.h"
#include "co_bt_defines.h"
#include "tg_bluetooth.h"


struct tg_bt_mesh_adv_misc g_tg_bt_mesh_adv_misc = {false};

struct tg_bt_mesh_scan_misc g_tg_bt_mesh_scan_misc = {
       .scan_cb = NULL };


void HfpHfUserRportAudioDisconnected(unsigned char *addr, unsigned int addr_len)
{
    LOG_E("%s stub TBD!!!", __FUNCTION__);
}

void tg_bt_mesh_scan_result(ble_bdaddr_t *bleAddr, int8_t rssi, uint8_t evt_type, uint8_t *adv_buf, uint8_t len)
{
    tg_bt_mesh_addr_le_t le_Addr;
    struct tg_bt_mesh_net_buf_simple *simple_buf = BT_MESH_NET_BUF_SIMPLE(0x1f);

    if (g_tg_bt_mesh_scan_misc.open_flag) {
        simple_buf->data = simple_buf->__buf;
        simple_buf->len = len;
        le_Addr.type = bleAddr->addr_type;
        memcpy(simple_buf->data, adv_buf, len);
        memcpy(le_Addr.a.val, bleAddr->addr, sizeof(le_Addr.a.val));
        LOG_I("rssi:%d addr_type:%d, evt_type:%d", rssi, le_Addr.type, evt_type);
        DUMP8("%02x", le_Addr.a.val, sizeof(le_Addr.a.val));
        if (g_tg_bt_mesh_scan_misc.scan_cb) {
            LOG_I("%s mesh scan_cb", __FUNCTION__);
            g_tg_bt_mesh_scan_misc.scan_cb(&le_Addr, rssi, evt_type, simple_buf);
        }
    }
}

/**
 * @brief mesh start adv broadcast
 * @param[in] param: advertise paramters set
 * @param[in] ad: advertise data
 * @param[in] ad_len: advertise data length
 * @param[in] sd: scan response data, reserved in mesh
 * @param[in] sd_len: advertise data length, reserved in mesh, set to 0
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_adv_start(const struct tg_bt_mesh_le_adv_param *param,
                          const struct tg_bt_mesh_data *ad, int32_t ad_len,
                          const struct tg_bt_mesh_data *sd, int32_t sd_len)
{
    uint32_t state;
    LOG_I("%s", __FUNCTION__);
    g_tg_bt_mesh_adv_misc.isInMeshAdv = true;
    g_tg_bt_mesh_adv_misc.options = param->options;
    g_tg_bt_mesh_adv_misc.interval_min = param->interval_min;
    g_tg_bt_mesh_adv_misc.interval_max = param->interval_max;
    if (0x01 & g_tg_bt_mesh_adv_misc.options){
        g_tg_bt_mesh_adv_misc.adv_type = GAPM_ADV_UNDIRECT;
    } else {
        g_tg_bt_mesh_adv_misc.adv_type = GAPM_ADV_NON_CONN;
    }
    g_tg_bt_mesh_adv_misc.adv_data_len = 0;

    for (uint32_t i = 0; i < ad_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (g_tg_bt_mesh_adv_misc.adv_data_len + ad[i].data_len + 2 > 31) {
            return -1;
        }

        g_tg_bt_mesh_adv_misc.adv_data[g_tg_bt_mesh_adv_misc.adv_data_len++] = ad[i].data_len + 1;
        g_tg_bt_mesh_adv_misc.adv_data[g_tg_bt_mesh_adv_misc.adv_data_len++] = ad[i].type;
        LOG_I("%s len %d", __func__, ad[i].data_len);
        //DUMP8("%02x ", ad[i].data, ad[i].data_len);
        memcpy(&g_tg_bt_mesh_adv_misc.adv_data[g_tg_bt_mesh_adv_misc.adv_data_len], ad[i].data, ad[i].data_len);

        g_tg_bt_mesh_adv_misc.adv_data_len += ad[i].data_len;

        LOG_I("fill adv len %d", g_tg_bt_mesh_adv_misc.adv_data_len);
    }

    g_tg_bt_mesh_adv_misc.srd_data_len = 0;

    for (uint32_t i = 0; i < sd_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (g_tg_bt_mesh_adv_misc.srd_data_len + sd[i].data_len + 2 > 31) {
            return -1;
        }

        g_tg_bt_mesh_adv_misc.srd_data[g_tg_bt_mesh_adv_misc.srd_data_len++] = sd[i].data_len + 1;
        g_tg_bt_mesh_adv_misc.srd_data[g_tg_bt_mesh_adv_misc.srd_data_len++] = sd[i].type;

        memcpy(&g_tg_bt_mesh_adv_misc.srd_data[g_tg_bt_mesh_adv_misc.srd_data_len], sd[i].data, sd[i].data_len);
        g_tg_bt_mesh_adv_misc.srd_data_len += sd[i].data_len;
    }
    state = tg_ble_adv_mesh_set_adv_data(g_tg_bt_mesh_adv_misc.adv_data, g_tg_bt_mesh_adv_misc.adv_data_len,
                                    g_tg_bt_mesh_adv_misc.srd_data, g_tg_bt_mesh_adv_misc.srd_data_len);
    state = tg_ble_adv_mesh_start_adv(g_tg_bt_mesh_adv_misc.interval_min, g_tg_bt_mesh_adv_misc.adv_type);
    return state;
}

/**
 * @brief mesh stop adv broadcast
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_adv_stop(void)
{
    LOG_I("%s", __FUNCTION__);
    g_tg_bt_mesh_adv_misc.isInMeshAdv = false;
    tg_ble_adv_mesh_stop_adv();
    return 0;
}

/**
 * @brief mesh start scan
 * @param[in] param: scan paramters set
 *            cb: callback function when scaned advertising packet
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_scan_start(const struct tg_bt_mesh_le_scan_param *param,
                           tg_bt_mesh_le_scan_cb_t cb)
{
    static bool init_flag = false;
    int32_t state;
    BLE_SCAN_PARAM_T scan_param;

    if (!init_flag) {
        init_flag = true;
        tg_ble_adv_mesh_init(tg_bt_mesh_scan_result);
    }

    LOG_I("%s type:%d filter_dup:%d interval:%d window:%d\n", __func__, param->type,
        param->filter_dup, param->interval, param->window);
    g_tg_bt_mesh_scan_misc.open_flag = 1;
    g_tg_bt_mesh_scan_misc.type = param->type;
    g_tg_bt_mesh_scan_misc.filter_dup = param->filter_dup;
    g_tg_bt_mesh_scan_misc.interval = param->interval;
    g_tg_bt_mesh_scan_misc.window = param->window;
    g_tg_bt_mesh_scan_misc.scan_cb = cb;

    scan_param.scanType = g_tg_bt_mesh_scan_misc.type;
    scan_param.scanWindow = g_tg_bt_mesh_scan_misc.window;
    scan_param.scanInterval = g_tg_bt_mesh_scan_misc.interval;
    state = tg_ble_adv_mesh_start_scan(BLE_SCAN_ALLOW_ADV_ALL_AND_INIT_RPA, &scan_param);
    return state;
}


/**
 * @brief mesh stop scan
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_scan_stop(void)
{
    int32_t state;

    LOG_I("%s", __FUNCTION__);
    g_tg_bt_mesh_scan_misc.open_flag = 0;
    state = tg_ble_adv_mesh_stop_scan();
    return state;
}

/**
 * @brief mesh set priority
 * @param[in] enable: 1 means increase mesh priority; 0 means set mesh priority to normal
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_enable_aggressive_setting(bool enable)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}

/**
 * @brief set oneshot adv broadcast data
 * @param[in] ad: advertise data
 * @param[in] ad_len: advertise data length
 * @param[in] sd: scan response data, reserved in mesh
 * @param[in] sd_len: advertise data length, reserved in mesh, set to 0
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_set_oneshot_data(const struct tg_bt_mesh_data *ad, int32_t ad_len,
                                 const struct tg_bt_mesh_data *sd, int32_t sd_len)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}

/**
 * @brief send oneshot adv packet
 * @param[in] None
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_send_adv_oneshot(void)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}


/**
 * @brief get ble mesh scan state.
 * @param[in] None
 * @return: on/off
 */
uint8_t tg_bt_mesh_get_scan_enable(void)
{
    LOG_I("%s", __FUNCTION__);
    return 0;
}

/**
 * @brief Generate random data
 * @param[out] buf:  buffer to store random value.
 * @param[in] len:  random length
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_mesh_rand(void *buf, int32_t len)
{
    uint32_t i;
    uint8_t* dst = (uint8_t *)buf;

    LOG_I("%s", __FUNCTION__);
    for (i = 0; i < len; i++)
    {
        dst[i] = (uint8_t)rand();
    }
    return 0;
}