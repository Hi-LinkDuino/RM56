
#ifndef __BLE_ADV_ADP__
#define __BLE_ADV_ADP__

#include "app_ble_mode_switch.h"
#include "app_ble_customif.h"

#ifdef __cplusplus
extern "C" {
#endif


//gattc interface
int tg_ble_adv_gattc_set_adv_data(uint8_t *advData, uint8_t advLen, uint8_t *rspData, uint8_t rspLen);
int tg_ble_adv_gattc_start_adv(uint16_t adv_interval, uint8_t adv_type);
int tg_ble_adv_gattc_stop_adv(void);
int tg_ble_adv_gattc_init(ble_adv_data_report_cb_t cb);
int tg_ble_adv_gattc_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy, BLE_SCAN_PARAM_T *param);
int tg_ble_adv_gattc_stop_scan(void);

//mesh
int tg_ble_adv_mesh_set_adv_data(uint8_t *advData, uint8_t advLen, uint8_t *rspData, uint8_t rspLen);
int tg_ble_adv_mesh_start_adv(uint16_t adv_interval, uint8_t adv_type);
int tg_ble_adv_mesh_stop_adv(void);
int tg_ble_adv_mesh_init(ble_adv_data_report_cb_t cb);
int tg_ble_adv_mesh_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy, BLE_SCAN_PARAM_T *param);
int tg_ble_adv_mesh_stop_scan(void);

#ifdef __cplusplus
}
#endif

#endif  //__BLE_ADV_ADP__