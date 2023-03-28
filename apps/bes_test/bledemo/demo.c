#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include <string.h>
#include "hal_trace.h"
#include "bes_gatts_adapter.h"

#define service_uuid_16_content       {0xff, 0xff}
#define tx_char_val_uuid_16_content   {0x01, 0xff}
#define rx_char_val_uuid_16_content   {0x02, 0xff}

#define MAX_LEN            (128)  

#define GATT_DECL_PRIMARY_SERVICE       { 0x00, 0x28 }
#define GATT_DECL_INCLUDED_SERVICE       { 0x02, 0x28 }
#define GATT_DECL_CHARACTERISTIC_UUID        { 0x03, 0x28 }
#define GATT_DESC_CLIENT_CHAR_CFG_UUID       { 0x02, 0x29 }
#define GATT_DESC_CHAR_USER_DESCRIPTION_UUID { 0x01, 0x29 }


static const uint8_t DATAPATH_SERVICE_UUID[2] = service_uuid_16_content;

#define PROP(x) ADAPTER_BLE_PROP_##x
#define DECLARE_PRIMARY_SERVICE()   {GATT_DECL_PRIMARY_SERVICE, 2, PROP(READ), ADAPTER_BLE_PERM_READ, 0}
#define DECLARE_TX_CHAR(uuid,uuid_len, ext) {GATT_DECL_CHARACTERISTIC_UUID, 2, PROP(READ), 0 , 0}, \
                {uuid, uuid_len,  PROP(READ), 0, ext}

#define DECLARE_TX_CHAR_WITH_NOTIFY(uuid,uuid_len, ext) {GATT_DECL_CHARACTERISTIC_UUID, 2, PROP(READ), 0 , 0}, \
                {uuid, uuid_len,  PROP(NOTIFY) | PROP(READ), 0, ext}, \
                {GATT_DESC_CLIENT_CHAR_CFG_UUID, 2, PROP(READ) | PROP(WRITE), 0, 2}
                

#define DECLARE_DESC()      {GATT_DESC_CHAR_USER_DESCRIPTION_UUID, 2, PROP(READ), 0,32}

#define DECLARE_RX_CHAR(uuid,uuid_len, ext) {GATT_DECL_CHARACTERISTIC_UUID, 2, PROP(READ), 0 , 0}, \
                {uuid, uuid_len,  PROP(WRITE), 0, ext}


static const bes_adapter_attm_item_t att_db[] =
{
    DECLARE_PRIMARY_SERVICE(),

    DECLARE_TX_CHAR_WITH_NOTIFY(tx_char_val_uuid_16_content, 2, MAX_LEN),
    DECLARE_DESC(),

    DECLARE_RX_CHAR(rx_char_val_uuid_16_content, 2, MAX_LEN),
    DECLARE_DESC()
};

extern void BleDemoCallback(void);
void ble_demo_init(void)
{
    int32_t status = 0;
    int32_t itf = -1;
    int32_t num_att = ARRAY_SIZE(att_db);

    BleDemoCallback();
    itf = bes_gatt_server_add_service(-1,(uint8_t *)DATAPATH_SERVICE_UUID,sizeof(DATAPATH_SERVICE_UUID),
                                        true, num_att, false);
    TRACE(2, "%s: itf=%d", __func__, itf);
    if(itf >= 0){
        for(int i =0; i < num_att;  i ++){
            status = bes_gatt_server_add_att_item(itf, i, (bes_adapter_attm_item_t*)&att_db[i]);
        }

        status = bes_gatt_server_start_service(itf);
        TRACE(2, "%s: start service, status=%d", __func__, status);
    }
}

