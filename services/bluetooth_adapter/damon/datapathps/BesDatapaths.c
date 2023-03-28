#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include <string.h>
#include "hal_trace.h"
#include "bes_gatts_adapter.h"

static int32_t g_datapathps_if = -1;

#define datapath_service_uuid_128_content           {0xfb ,0x34 ,0x9b ,0x5f ,0x80 ,0x00 ,0x00 ,0x80 ,0x00 ,0x10 ,0x00 ,0x00 ,0x20 ,0x8e ,0x00 ,0x00 }
#define datapath_tx_char_val_uuid_128_content   {0xfb ,0x34 ,0x9b ,0x5f ,0x80 ,0x00 ,0x00 ,0x80 ,0x00 ,0x10 ,0x00 ,0x00 ,0x32 ,0x8e ,0x00 ,0x00}
#define datapath_rx_char_val_uuid_128_content   {0xfb ,0x34 ,0x9b ,0x5f ,0x80 ,0x00 ,0x00 ,0x80 ,0x00 ,0x10 ,0x00 ,0x00 ,0x40 ,0x8e ,0x00 ,0x00 }

#define DATAPATHPS_MAX_LEN            (128)  
#define DATA_PATH_GATT_DESC_CLIENT_CHAR_CFG_UUID {0xfb ,0x34 ,0x9b ,0x5f ,0x80 ,0x00 ,0x00 ,0x80 ,0x00 ,0x10 ,0x00 ,0x00 ,0x02 ,0x29 ,0x00 ,0x00 }

#define GATT_DECL_PRIMARY_SERVICE       { 0x00, 0x28 }
#define GATT_DECL_INCLUDED_SERVICE       { 0x02, 0x28 }
#define GATT_DECL_CHARACTERISTIC_UUID        { 0x03, 0x28 }
#define GATT_DESC_CLIENT_CHAR_CFG_UUID       { 0x02, 0x29 }
#define GATT_DESC_CHAR_USER_DESCRIPTION_UUID { 0x01, 0x29 }


///Attributes State Machine
enum
{
    DATAPATHPS_IDX_SVC,

    DATAPATHPS_IDX_TX_CHAR,
    DATAPATHPS_IDX_TX_VAL,
    DATAPATHPS_IDX_TX_NTF_CFG,
    DATAPATHPS_IDX_TX_DESC,

    DATAPATHPS_IDX_RX_CHAR,
    DATAPATHPS_IDX_RX_VAL,

    DATAPATHPS_IDX_RX_DESC,

    DATAPATHPS_IDX_NB,
};

static const uint8_t DATAPATH_SERVICE_UUID_128[16] = datapath_service_uuid_128_content;

static const bes_adapter_attm_item_t datapath_att_db[DATAPATHPS_IDX_NB] =
{
    // Service Declaration
    [DATAPATHPS_IDX_SVC]        =   {GATT_DECL_PRIMARY_SERVICE, 2, ADAPTER_BLE_PROP_READ, ADAPTER_BLE_PERM_READ, 0},

    // TX Characteristic Declaration
    [DATAPATHPS_IDX_TX_CHAR]    =   {GATT_DECL_CHARACTERISTIC_UUID, 2, ADAPTER_BLE_PROP_READ, 0 , 0},
    // TX Characteristic Value
    [DATAPATHPS_IDX_TX_VAL]     =   {datapath_tx_char_val_uuid_128_content,16,  ADAPTER_BLE_PROP_NOTIFY | ADAPTER_BLE_PROP_READ, 0, DATAPATHPS_MAX_LEN},

    // TX Characteristic - Client Characteristic Configuration Descriptor
    [DATAPATHPS_IDX_TX_NTF_CFG] =   {DATA_PATH_GATT_DESC_CLIENT_CHAR_CFG_UUID, 16, ADAPTER_BLE_PROP_READ | ADAPTER_BLE_PROP_WRITE, 0, 2},

    // TX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_TX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION_UUID, 2, ADAPTER_BLE_PROP_READ, 0,32},

    // RX Characteristic Declaration
    [DATAPATHPS_IDX_RX_CHAR]    =   {GATT_DECL_CHARACTERISTIC_UUID, 2,ADAPTER_BLE_PROP_READ, 0,0},
    // RX Characteristic Value
    [DATAPATHPS_IDX_RX_VAL]     =   {datapath_rx_char_val_uuid_128_content, 16,ADAPTER_BLE_PROP_WRITE, 0,DATAPATHPS_MAX_LEN},
    // RX Characteristic - Characteristic User Description Descriptor
    [DATAPATHPS_IDX_RX_DESC]    =   {GATT_DESC_CHAR_USER_DESCRIPTION_UUID,2, ADAPTER_BLE_PROP_READ, 0, 32},
};

extern void BesDatapathpsRegisterCallback(void);
void bes_datapathps_init(void)
{
    POSSIBLY_UNUSED int32_t status = 0;
    bool use_bes_db = false;
    
    BesDatapathpsRegisterCallback();
    g_datapathps_if = bes_gatt_server_add_service(-1, 
                                                                                (uint8_t *)DATAPATH_SERVICE_UUID_128,
                                                                                16,
                                                                                true,
                                                                                DATAPATHPS_IDX_NB, 
                                                                                use_bes_db);
    if(g_datapathps_if >= 0){
        for(int i =0; i < DATAPATHPS_IDX_NB;  i ++){
            status = bes_gatt_server_add_att_item(g_datapathps_if,
                                                                        i,
                                                                        (bes_adapter_attm_item_t*)&datapath_att_db[i]);
        }

        status = bes_gatt_server_start_service(g_datapathps_if);
    }
}

