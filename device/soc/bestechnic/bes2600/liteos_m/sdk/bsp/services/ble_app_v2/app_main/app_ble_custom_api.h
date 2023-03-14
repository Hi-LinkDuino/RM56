/***************************************************************************
*
*Copyright 2015-2021 BES.
*All rights reserved. All unpublished rights reserved.
*
*No part of this work may be used or reproduced in any form or by any
*means, or stored in a database or retrieval system, without prior written
*permission of BES.
*
*Use of this work is governed by a license granted by BES.
*This work contains confidential and proprietary information of
*BES. which is protected by copyright, trade secret,
*trademark and other intellectual property rights.
*
****************************************************************************/

#ifndef __APP_BLE_CUSTOM_API_H__
#define __APP_BLE_CUSTOM_API_H__

/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 *            app_ble_custom_adv_write_data
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    write adv data of customer
 *
 * Parameters:
 *    actv_user: The user of the adv activity
 *    is_custom_adv_flags: If this flag was set, custom can set adv flag by himself
 *    local_addr: The local address of this adv. If local_addr is NULL, then use the ble address from factory section
 *    peer_addr: If adv_type is direct adv, this param is the address of peer ble
 *    adv_interval: Adv interval
 *    adv_type: Adv type
 *    tx_power_dbm: Adv tx power in dbm, range: -3~16
 *    adv_data: Adv data
 *    adv_data_size: Adv data size
 *    scan_rsp_data: Scan response data
 *    scan_rsp_data_size: Scan response data size
 *
 * Return:
 *    None
 */
void app_ble_custom_adv_write_data(BLE_ADV_ACTIVITY_USER_E actv_user,
                    bool is_custom_adv_flags,
                    uint8_t *local_addr,
                    ble_bdaddr_t *peer_addr,
                    uint16_t adv_interval,
                    BLE_ADV_TYPE_E adv_type,
                    int8_t tx_power_dbm,
                    uint8_t *adv_data, uint8_t adv_data_size,
                    uint8_t *scan_rsp_data, uint8_t scan_rsp_data_size);

/*---------------------------------------------------------------------------
 *            app_ble_custom_adv_start
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for customer to start a adv
 *
 * Parameters:
 *    actv_user: The user of the adv activity
 *
 * Return:
 *    None
 */
void app_ble_custom_adv_start(BLE_ADV_ACTIVITY_USER_E actv_user);

/*---------------------------------------------------------------------------
 *            app_ble_custom_adv_start
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for customer to stop a adv
 *
 * Parameters:
 *    actv_user: The user of the adv activity
 *
 * Return:
 *    None
 */
void app_ble_custom_adv_stop(BLE_ADV_ACTIVITY_USER_E actv_user);

/*---------------------------------------------------------------------------
 *            app_ble_custom_init
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    initialize app ble custom
 *
 * Parameters:
 *    void
 *
 * Return:
 *    None
 */
void app_ble_custom_init(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_BLE_CUSTOM_API_H__ */

