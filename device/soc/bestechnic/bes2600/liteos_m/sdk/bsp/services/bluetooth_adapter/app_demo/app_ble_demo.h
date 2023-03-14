/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __APP_BLE_DEMO_H__
#define __APP_BLE_DEMO_H__

typedef enum {
    DEVICE_ROLE_UNKNOW,
    DEVICE_ROLE_CENTER          = 1<<0,
    DEVICE_ROLE_PERIPHERAL      = 1<<1,
} APP_BLE_DEVICE_ROLE_E;

enum {
    APP_BLE_DATA_TX_WITHOUT_RSP,
    APP_BLE_DATA_TX_WITH_RSP,
    APP_BLE_DATA_TX_MODE_INVALID = 0xFF,
};

/// Bit field use to select the preferred TX or RX LE PHY. 0 means no preferences
/// syncsee@APP_GAP_PHY_E
typedef enum
{
    /// No preferred PHY
    BLE_GAP_PHY_ANY      = 0,
    /// LE 1M PHY preferred for an active link
    BLE_GAP_PHY_LE_1MBPS,
    /// LE 2M PHY preferred for an active link
    BLE_GAP_PHY_LE_2MBPS,
    /// LE Coded PHY preferred for an active link
    BLE_GAP_PHY_LE_CODED,
} BLE_GAP_PHY_E;

/// Option for PHY configuration
/// sync see@APP_GAP_PHY_OPTION_E
typedef enum
{
    /// No preference for rate used when transmitting on the LE Coded PHY
    BLE_GAP_PHY_OPT_LE_CODED_ALL_RATES = 0,
    /// 500kbps rate preferred when transmitting on the LE Coded PHY
    BLE_GAP_PHY_OPT_LE_CODED_500K_RATE,
    /// 125kbps  when transmitting on the LE Coded PHY
    BLE_GAP_PHY_OPT_LE_CODED_125K_RATE,
} BLE_GAP_PHY_OPTION_E;


void app_ble_adv_start(void);
void app_ble_adv_stop(void);
void app_ble_scan_start(void);
void app_ble_scan_stop(void);
void app_ble_data_loopback_test(uint8_t loopback_enable);
void app_ble_data_throughtput_test(uint8_t enable, uint16_t interval, uint16_t mtu);
void app_ble_data_txmode_set(uint8_t tx_mode);
void app_ble_set_phy_mode(uint8_t conidx, BLE_GAP_PHY_E phy_mode, BLE_GAP_PHY_OPTION_E phy_opt);
void app_ble_get_phy_mode(uint8_t conidx);
void app_ble_get_phy_tx_power(uint8_t conidx, BLE_GAP_PHY_E phy_mode, BLE_GAP_PHY_OPTION_E phy_opt);
void app_ble_get_tx_power_setting(void);

void app_ble_send_data(uint8_t mode, uint8_t* data, uint32_t data_len);
void app_ble_recv_data_done_callback(uint8_t* data, uint32_t data_len);

void app_ble_peripheral_init(int* app_id);
void app_ble_gatts_probe_value_config(uint8_t probe_enable);

void app_ble_center_init(int* app_id);
void app_ble_gattc_search_datapath_service(void);
void app_ble_gattc_tx_char_ccc_enable(uint8_t enable);
void app_ble_gattc_probe_char_ccc_enable(uint8_t enable);
void app_ble_gattc_probe_value_config(uint8_t probe_enable);
void app_ble_gattc_send_data_via_write_command(uint8_t* data, uint32_t data_len);
void app_ble_gattc_send_data_via_write_request(uint8_t* data, uint32_t data_len);

#endif //end of __APP_BLE_DEMO_H__