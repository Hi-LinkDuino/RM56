#ifndef __BES_GATT_CLIENT_ADAPTER_H__
#define __BES_GATT_CLIENT_ADAPTER_H__
#include "plat_types.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"

#define datapath_service_uuid_128_content   {0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01 }

void BesGattClientAdapterTest(void);
static adapter_client_task_env_t g_client_task_env;

extern void app_ble_start_connect(uint8_t *addr);
extern void BesGattcRegisterCallback(void);

adapter_client_task_env_t * BesGattClientGetEnv(void)
{
    return &g_client_task_env;
}

void BesGattClientAdapterInit(void)
{
    BesGattcRegisterCallback();
    g_client_task_env.task_id = btadapter_ble_client_get_start_task_id();
    btadapter_ble_client_start_service();

    {
        //uint8_t dst_addr[6] = {0x12, 0x62, 0x24 ,0x33,0x33,0x50};
        uint8_t dst_addr[6] = {0x50, 0x33, 0x33 ,0x24,0x62,0x12};
        app_ble_start_connect(dst_addr);
        osDelay(1000);
    }
}

void BesGattClientAdapterTest(void)
{
    uint8_t test_uuid[16] = datapath_service_uuid_128_content;

    btadapter_ble_client_start_service_discovery(0,
                                                                                BES_GATT_DISCOVER_SVC_PRIMARY_BY_UUID,
                                                                                test_uuid,
                                                                                16,
                                                                                0x01,
                                                                                0xffff);
}

#endif
