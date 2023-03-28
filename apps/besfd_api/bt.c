#include "besfd_api.h"
#ifdef CHIP_HAS_RGB_LED
#include "app_led.h"
#endif
#include "factory_section.h"

// #include "bt_drv_interface.h"
#include "bt_if.h"
#include "hal_trace.h"
#include "besbt.h"

#ifdef BLE_ENABLE
#include "app_ble_include.h"
const static uint8_t adv_addr_set[6] = {0x66, 0x34, 0x33, 0x23, 0x22, 0x11};
int ble_adv_start(uint16_t advInterval)
{
    //FIXME: load ble name&mac from NV
    char *advname = (char *)factory_section_get_ble_name();
    uint8_t *addr = factory_section_get_ble_address();    
    uint8_t advdata[31] = {0};
    advdata[0] = 0x02;
    advdata[1] = 0x01;
    advdata[2] = 0x06;
    advdata[3] = 0x1b;
    advdata[4] = 0x09;
    memcpy(&advdata[5],advname,strlen(advname));

    TRACE(0, "%s: name=%s, addr=%x:%x:%x:%x:%x:%x", __func__, advname, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        app_ble_custom_init();
        app_ble_custom_adv_write_data(BLE_ADV_ACTIVITY_USER_0,
                                    true,
                                    (uint8_t *)addr,
                                    NULL,
                                    advInterval,
                                    ADV_TYPE_UNDIRECT,
                                    12,
                                    (uint8_t *)advdata, 31,
                                    NULL, 0);

        app_ble_custom_adv_start(BLE_ADV_ACTIVITY_USER_0);


    return 0;
}

void ble_adv_stop(void)
{
    app_ble_custom_adv_stop(BLE_ADV_ACTIVITY_USER_0);
}
#endif


extern void bes_bt_init(void);
extern void app_bt_accessmode_set_by_AT_command(btif_accessible_mode_t mode);


bool app_is_stack_ready(void);
void bt_enter_paring_mode(void)
{
    if(bt_thread_running()){
        //besbt thread running
        if(!app_is_stack_ready()){
            TRACE(0,"%s(): bt is running, wait ready\n",__func__);
            return;
        }
        //stop ADV

    } else {
        bes_bt_init();
        // BT ADV
        app_bt_accessmode_set_by_AT_command(BTIF_BAM_GENERAL_ACCESSIBLE);
        // BLE ADV
#ifdef BLE_ENABLE
        ble_adv_start(40);
#endif
#if defined(PRJ_SWIFT) && !defined(SWIFT_HW_KEY_TEST)
        swift_led_scene_update();
#endif
    }
}