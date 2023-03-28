/***************************************************************************
*
*Copyright 2015-2019 BES.
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

#ifndef __APP_BLE_PARAM_CONFIG_H__
#define __APP_BLE_PARAM_CONFIG_H__

/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/
/**
 * @brief The user of the adv activity
 *
 */
typedef enum
{
    BLE_ADV_ACTIVITY_USER_0 = 0,
    BLE_ADV_ACTIVITY_USER_1,
    BLE_ADV_ACTIVITY_USER_2,
    BLE_ADV_ACTIVITY_USER_3,
    BLE_ADV_ACTIVITY_USER_4,
    BLE_ADV_ACTIVITY_USER_5,

    BLE_ADV_ACTIVITY_USER_NUM,
} BLE_ADV_ACTIVITY_USER_E;

/**
 * @brief Advertise TX power level
 *
 */
typedef enum
{
    BLE_ADV_TX_POWER_LEVEL_0 = 3,
    BLE_ADV_TX_POWER_LEVEL_1 = 4,
    BLE_ADV_TX_POWER_LEVEL_2 = 5,
} BLE_ADV_TX_POWER_LEVEL_E;

/**
 * @brief The user of the ble adv
 *
 */
typedef enum
{
    USER_INUSE = 0,
    USER_ALL = USER_INUSE,
    USER_STUB,
    USER_GFPS,
    USER_GSOUND,
    USER_AI,
    USER_INTERCONNECTION,
    USER_TILE,
    USER_OTA,
    USER_BLE_AUDIO,
    USER_BLE_CUSTOMER_0,
    USER_BLE_CUSTOMER_1,
    USER_BLE_CUSTOMER_2,
    USER_BLE_CUSTOMER_3,
    USER_BLE_CUSTOMER_4,
    USER_BLE_CUSTOMER_5,
    USER_BLE_CUSTOMER_MAX = USER_BLE_CUSTOMER_5,
    USER_BLE_DEMO0,
    USER_BLE_DEMO1,
    BLE_ADV_USER_NUM,
} BLE_ADV_USER_E;

/**
 * @brief The user of the ble adv
 *
 */
typedef enum
{
    BLE_ADV_INTERVALREQ_USER_A2DP,
    BLE_ADV_INTERVALREQ_USER_SCO,
    BLE_ADV_INTERVALREQ_USER_TWS_STM,
    BLE_ADV_INTERVALREQ_USER_NUM,
} BLE_ADV_INTERVALREQ_USER_E;

typedef struct {
    uint16_t adv_interval[BLE_ADV_INTERVALREQ_USER_NUM];
    BLE_ADV_USER_E adv_user[BLE_ADV_USER_NUM];
    BLE_ADV_TX_POWER_LEVEL_E adv_tx_power_level;
} BLE_ADV_FILL_PARAM_T;


/****************************function declearation**************************/
#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------------------
 *            ble_adv_user2str
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    change BLE_ADV_USER_E to string
 *
 * Parameters:
 *    BLE_ADV_USER_E user
 *
 * Return:
 *    char *
 */
char *ble_adv_user2str(BLE_ADV_USER_E user);

/*---------------------------------------------------------------------------
 *            ble_adv_intv_user2str
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    change BLE_ADV_INTERVALREQ_USER_E to string
 *
 * Parameters:
 *    BLE_ADV_INTERVALREQ_USER_E user
 *
 * Return:
 *    char *
 */
char *ble_adv_intv_user2str(BLE_ADV_INTERVALREQ_USER_E user);

/*---------------------------------------------------------------------------
 *            app_ble_param_get_ctx
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get the context of ble_adv_fill_param
 *
 * Parameters:
 *    void
 *
 * Return:
 *    BLE_ADV_FILL_PARAM_T *
 */
BLE_ADV_FILL_PARAM_T *app_ble_param_get_ctx(void);

/*---------------------------------------------------------------------------
 *            app_ble_param_get_adv_interval
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get the activity user from adv user
 *
 * Parameters:
 *    user: adv user
 *
 * Return:
 *    BLE_ADV_ACTIVITY_USER_E: activity user
 */
BLE_ADV_ACTIVITY_USER_E app_ble_param_get_actv_user_from_adv_user(BLE_ADV_USER_E user);

/*---------------------------------------------------------------------------
 *            app_ble_param_get_adv_interval
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get the adv interval of app_ble_param_get_ctx
 *
 * Parameters:
 *    BLE_ADV_ACTIVITY_USER_E actv_user
 *
 * Return:
 *    uint16_t
 */
uint16_t app_ble_param_get_adv_interval(BLE_ADV_ACTIVITY_USER_E actv_user);

/*---------------------------------------------------------------------------
 *            app_ble_param_set_adv_interval
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    set the adv interval of activity
 *
 * Parameters:
 *    adv_intv_user: the adv interval request user
 *    adv_user: the adv user
 *    interval: the interval you want to set
 *
 * Return:
 *    void
 */
void app_ble_param_set_adv_interval(BLE_ADV_INTERVALREQ_USER_E adv_intv_user,
                                        BLE_ADV_USER_E adv_user,
                                        uint16_t interval);

/*---------------------------------------------------------------------------
 *            app_ble_param_get_adv_tx_power_level
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get the adv tx power level of activity
 *
 * Parameters:
 *    adv_intv_user: the adv activity request user
 *
 * Return:
 *    BLE_ADV_TX_POWER_LEVEL_E: the adv tx power level of activity
 */
BLE_ADV_TX_POWER_LEVEL_E app_ble_param_get_adv_tx_power_level(BLE_ADV_ACTIVITY_USER_E actv_user);

void app_ble_demo0_user_init(void);
void app_ble_demo1_user_init(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_BLE_PARAM_CONFIG_H__ */

