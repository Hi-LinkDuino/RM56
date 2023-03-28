#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_def.h"
#include "me_api.h"
#include "harmony_utils.h"
#include "stdlib.h"
#include "hal_timer.h"
#include "app_ble_mode_switch.h"
#include "app_ble_customif.h"

#define HARMONY_ADV_OCCUPIED_FLAG (0xff)
#define HARMONY_ADV_FREE_FLAG (0x11)

#define HARMONY_ADV_MAX_SIZE (6)

typedef struct ble_addr
{
    /// BD Address of device
    uint8_t addr[BTIF_BD_ADDR_SIZE];
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
} ble_addr_t;

typedef struct __harmony_ble_adv_record{
    unsigned char flag;
    unsigned char adv_index;
    int advId;
    ble_addr_t local_addr;
}harmony_ble_adv_record;

static BleScanParams blescanparamter;

static harmony_ble_adv_record g_harmony_adv_record[HARMONY_ADV_MAX_SIZE];

extern void app_ble_custom_init(void);
static BtGattCallbacks * g_gatt_callback = NULL;
extern void bes_bt_init(void);

BLE_ADV_TYPE_E ble_adv_type_harmony_change_to_bes(BleAdvType type)
{
    BLE_ADV_TYPE_E adv_type = ADV_TYPE_NON_CONN_NON_SCAN;
    switch(type)
    {
        case OHOS_BLE_ADV_IND:
            adv_type = ADV_TYPE_UNDIRECT;
        break;
        case OHOS_BLE_ADV_DIRECT_IND_HIGH:
            adv_type = ADV_TYPE_DIRECT_HDC;
        break;
        case OHOS_BLE_ADV_SCAN_IND:
            adv_type = ADV_TYPE_NON_CONN_SCAN;
        break;
        case OHOS_BLE_ADV_NONCONN_IND:
            adv_type = ADV_TYPE_NON_CONN_NON_SCAN;
        break;
        case OHOS_BLE_ADV_DIRECT_IND_LOW:
            adv_type = ADV_TYPE_DIRECT_LDC;
        break;
        default:
            break;
    }
    return adv_type;
}

int general_random_ble_address(BdAddr *local_addr)
{
    if (local_addr == NULL){
        return -1;
    }

    srand(GET_CURRENT_MS());
    for(int i = 0;i< OHOS_BD_ADDR_LEN; i++)
        local_addr->addr[i] = rand()%0x100;
    local_addr->addr[5] &= 0x3F;
    return 0;
}

int InitBtStack(void)
{
    bes_bt_init();
    return 0;
}

int EnableBtStack(void)
{
    return BT_STS_SUCCESS;
}

int DisableBtStack(void)
{
    return BT_STS_SUCCESS;
}

static harmony_ble_adv_record* BleGetFreeAdvRecord(void)
{
    for(int i = 0; i < HARMONY_ADV_MAX_SIZE; i ++){
        if(g_harmony_adv_record[i].flag == HARMONY_ADV_FREE_FLAG){
            g_harmony_adv_record[i].flag = HARMONY_ADV_OCCUPIED_FLAG;
            return &g_harmony_adv_record[i];
        }
    }

    return NULL;
}

static harmony_ble_adv_record* BleGetAdvRecordByAdvId(int advId)
{
    for(int i = 0; i < HARMONY_ADV_MAX_SIZE; i ++){
        if((g_harmony_adv_record[i].flag == HARMONY_ADV_OCCUPIED_FLAG) &&
                (g_harmony_adv_record[i].advId == advId)){

            return &g_harmony_adv_record[i];
        }
    }

    return NULL;
}

static void BleFreeAdvRecord(int advId)
{
    for(int i = 0; i < HARMONY_ADV_MAX_SIZE; i ++){
        if((g_harmony_adv_record[i].flag == HARMONY_ADV_OCCUPIED_FLAG) &&
                (g_harmony_adv_record[i].advId == advId)){
            g_harmony_adv_record[i].flag = HARMONY_ADV_FREE_FLAG;
            break;
        }
    }
}

static void BleInitAdvRecord(void)
{
     for(int i = 0; i < HARMONY_ADV_MAX_SIZE; i ++){
        g_harmony_adv_record[i].advId = 0;
        g_harmony_adv_record[i].adv_index = i;
        g_harmony_adv_record[i].flag = HARMONY_ADV_FREE_FLAG;
     }
}

extern unsigned char *bt_get_local_address(void);
extern void app_ble_custom_adv_start_3p(int actv_user);
extern void app_ble_custom_adv_stop_3p(int actv_user);
extern void app_ble_custom_set_adv_data(int actv_user,
                                                                            uint8_t *adv_data,
                                                                            uint8_t adv_data_size,
                                                                            uint8_t *scan_rsp_data,
                                                                            uint8_t scan_rsp_data_size);
void app_ble_custome_set_adv_para(int actv_user,
                                                                                uint8_t *local_addr,
                                                                                uint8_t *peer_addr,
                                                                                uint16_t adv_interval,
                                                                                uint8_t adv_type,
                                                                                int8_t tx_power_dbm);


int BleSetAdvData(int advId, const BleConfigAdvData *data)
{
    if (data == NULL){
        return -1;
    }

    harmony_ble_adv_record * advRecord = NULL;

    advRecord = BleGetFreeAdvRecord();
    if(advRecord == NULL){
        return -1;
    }

    advRecord->advId = advId;
    app_ble_custom_set_adv_data(advRecord->adv_index,
                                                            (uint8_t*)data->advData,
                                                            data->advLength,
                                                            (uint8_t*)data->scanRspData,
                                                            data->scanRspLength);
    if(g_gatt_callback){
        g_gatt_callback->advDataCb(advId,0);
    }
    return 0;
}


int BleStartAdv(int advId, const BleAdvParams *param)
{
    if (param == NULL){
        return -1;
    }
    uint16_t adv_interval  = 0;
    harmony_ble_adv_record * advRecord = NULL;
    ble_addr_t peerAddr;
    BdAddr local_addr;
    BLE_ADV_TYPE_E adv_type;

    advRecord = BleGetAdvRecordByAdvId(advId);
    if(advRecord == NULL){
        return -1;
    }
    general_random_ble_address(&local_addr);

    memcpy(advRecord->local_addr.addr,local_addr.addr,OHOS_BD_ADDR_LEN);

    if(param->peerAddr.addr)
        memcpy(peerAddr.addr,param->peerAddr.addr,OHOS_BD_ADDR_LEN);
    peerAddr.addr_type = param->peerAddrType;
    adv_type = ble_adv_type_harmony_change_to_bes(param->advType);

    adv_interval = (param->minInterval + param->maxInterval)/2;
    app_ble_custome_set_adv_para(advRecord->adv_index,
                                                               local_addr.addr,
                                                               (uint8_t*)&peerAddr,
                                                               adv_interval,
                                                               adv_type,
                                                               param->txPower);
    app_ble_custom_adv_start_3p(advRecord->adv_index);

    if(g_gatt_callback){
        g_gatt_callback->advEnableCb(advId, 0);
    }
    return 0;
}

int BleUpdateAdv(int advId, const BleConfigAdvData *data, const BleAdvParams *param)
{
    if (data == NULL || param == NULL){
        return -1;
    }

    uint16_t adv_interval  = 0;
    harmony_ble_adv_record * advRecord = NULL;
    ble_addr_t peerAddr;
    BdAddr local_addr;
    BLE_ADV_TYPE_E adv_type;

    advRecord = BleGetAdvRecordByAdvId(advId);
    if(advRecord == NULL){
        return -1;
    }

    app_ble_custom_set_adv_data(advRecord->adv_index,
                                (uint8_t*)data->advData,
                                data->advLength,
                                (uint8_t*)data->scanRspData,
                                data->scanRspLength);

    if(param->peerAddr.addr)
        memcpy(peerAddr.addr,param->peerAddr.addr,OHOS_BD_ADDR_LEN);
    peerAddr.addr_type = param->peerAddrType;
    adv_type = ble_adv_type_harmony_change_to_bes(param->advType);

    adv_interval = (param->minInterval + param->maxInterval)/2;
    app_ble_custome_set_adv_para(advRecord->adv_index,
                                   advRecord->local_addr.addr,
                                   (uint8_t *)&peerAddr,
                                   adv_interval,
                                   adv_type,
                                   param->txPower);

    app_ble_custom_adv_start_3p(advRecord->adv_index);

    if(g_gatt_callback){
        g_gatt_callback->advUpdateCb(advId, 0);
    }
    return 0;
}

int BleStopAdv(int advId)
{
    harmony_ble_adv_record * advRecord = NULL;

    advRecord = BleGetAdvRecordByAdvId(advId);
    if(advRecord == NULL){
        return -1;
    }

    app_ble_custom_adv_stop_3p(advRecord->adv_index);
    BleFreeAdvRecord(advId);

    if(g_gatt_callback){
        g_gatt_callback->advDisableCb(advId, 0);
    }
    return 0;
}

void AppBleStopAdvReport(int advId)
{
    if(advId >= HARMONY_ADV_MAX_SIZE){
        TRACE(1, "%s advId:%d", __func__, advId);
        return;
    }

    BleFreeAdvRecord(advId);

    if(g_gatt_callback){
        g_gatt_callback->advDisableCb(advId, 0);
    }
}

int BleSetScanParameters(int clientId, BleScanParams *param)
{
    memcpy(&blescanparamter,param,sizeof(BleScanParams));
    if(g_gatt_callback){
        g_gatt_callback->scanParamSetCb(clientId,0);
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleStartScan(void)
{
    app_ble_start_scan(blescanparamter.scanFilterPolicy, blescanparamter.scanWindow, blescanparamter.scanInterval);
    return OHOS_BT_STATUS_SUCCESS;
}

int BleStopScan(void)
{
    app_ble_stop_scan();
    return OHOS_BT_STATUS_SUCCESS;
}

void ble_scan_result_callback(ble_bdaddr_t *bleAddr,int8_t rssi, uint8_t evt_type, uint8_t *adv_buf,uint8_t len)
{
    BtScanResultData scanResultdata;
    memcpy(scanResultdata.addr.addr,bleAddr->addr,OHOS_BD_ADDR_LEN);
    scanResultdata.addrType = bleAddr->addr_type;
    scanResultdata.rssi = rssi;
    scanResultdata.advData = adv_buf;
    scanResultdata.advLen = len;
    if(g_gatt_callback){
        g_gatt_callback->scanResultCb(&scanResultdata);
    }
}

int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    g_gatt_callback= func;
    BleInitAdvRecord();
    app_ble_custom_init();
    app_ble_adv_report_callback_register(ble_scan_result_callback);
    return 0;
}

