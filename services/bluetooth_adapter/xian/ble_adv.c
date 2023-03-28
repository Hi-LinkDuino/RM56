#include "plat_types.h"
#include "bt_gatt_types.h"
#include "ble_adv.h"

extern void app_ble_custom_adv_write_data_3p(uint8_t user_id,
                    bool is_custom_adv_flags,
                    uint8_t *local_addr,
                    uint8_t *peer_addr,
                    uint8_t   addr_type, // Address type of the device 0=public/1=private random
                    uint16_t adv_interval,
                    uint8_t  adv_type,
                    int8_t tx_power_dbm,
                    uint8_t *adv_data, uint8_t adv_data_size,
                    uint8_t *scan_rsp_data, uint8_t scan_rsp_data_size);

static wh_bt_status_t multi_adv_set_data(int client_if, SetDataMultiAdvParam *param)
{

}

static wh_bt_status_t multi_adv_enable(int client_if, EnableMultiAdvParam *param)
{
    
}

static wh_bt_status_t multi_adv_disable(int client_if)
{

}