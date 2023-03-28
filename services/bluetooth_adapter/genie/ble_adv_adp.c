
#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"
#include "bleadapter_utils.h"
#include "bes_gatts_adapter.h"
#include "me_api.h"
#include "stdlib.h"
#include "hal_timer.h"
#include "ble_adv_adp.h"
#include "app_ble_param_config.h"
#include "bt_adaptor_dbg.h"

#define TG_ADV_MAX_SIZE (6)

#define TG_ADV_MESH_ID  (0)
#define TG_ADV_GATTC_DEFAULT_ID (1)

typedef enum {
    TG_SCAN_MESH = 0,
    TG_SCAN_GATTC,
}tg_scan_type_e;


typedef struct __tg_ble_adv_record {
    bool occupied;
    uint8_t adv_index;
    int advId;
    ble_bdaddr_t local_addr;
}tg_ble_adv_record_t;

typedef struct __tg_ble_scan_param {
    bool scan_flag;
    bool gattc_scan_flag;
    bool mesh_scan_flag;
    tg_scan_type_e scan_type;
    enum BLE_SCAN_FILTER_POLICY scanFilterPolicy;
    BLE_SCAN_PARAM_T scan_param;
}tg_ble_scan_param;


static tg_ble_scan_param gScanParam;
static tg_ble_adv_record_t gAdvRecord[TG_ADV_MAX_SIZE];

static ble_adv_data_report_cb_t ble_adv_mesh_report_cb = NULL;
static ble_adv_data_report_cb_t ble_adv_gattc_report_cb = NULL;

extern void app_ble_custom_init(void);
extern void app_ble_custom_adv_start_3p(int actv_user);
extern void app_ble_custom_adv_stop_3p(int actv_user);
extern void app_ble_custom_set_adv_data(int actv_user,
                                                                            uint8_t *adv_data,
                                                                            uint8_t adv_data_size,
                                                                            uint8_t *scan_rsp_data,
                                                                            uint8_t scan_rsp_data_size);
extern void app_ble_custome_set_adv_para(int actv_user,
                                                                                uint8_t *local_addr,
                                                                                uint8_t *peer_addr,
                                                                                uint16_t adv_interval,
                                                                                uint8_t adv_type,
                                                                                int8_t tx_power_dbm);


static tg_ble_adv_record_t *ble_adv_get_free_adv_record(void)
{
    for(uint8_t i = 0; i < TG_ADV_MAX_SIZE; i++) {
        if(gAdvRecord[i].occupied == false) {
            memset((uint8_t*)&gAdvRecord[i], 0, sizeof(tg_ble_adv_record_t));
            gAdvRecord[i].occupied = true;
            gAdvRecord[i].advId = 0;
            gAdvRecord[i].adv_index = i;

            return &gAdvRecord[i];
        }
    }
    return NULL;
}

static tg_ble_adv_record_t *ble_adv_get_adv_record_by_advId(int advId)
{
    for(uint8_t i = 0; i < TG_ADV_MAX_SIZE; i++) {
        if((gAdvRecord[i].occupied == true) && (gAdvRecord[i].advId == advId)) {
            return &gAdvRecord[i];
        }
    }
    return NULL;
}

static void tg_ble_adv_free_record(int advId)
{
     for(uint8_t i = 0; i < TG_ADV_MAX_SIZE; i++) {
        if((gAdvRecord[i].occupied == true) && (gAdvRecord[i].advId == advId)) {
            gAdvRecord[i].occupied = false;
        }
     }
}

int tg_ble_adv_set_adv_data(int advId, uint8_t *advData, uint8_t advLen, uint8_t *rspData, uint8_t rspLen)
{
    tg_ble_adv_record_t * adv_record = NULL;

    if(!advData && !rspData) {
        LOG_I("%s param error", __FUNCTION__);
        return -1;
    }
    LOG_I("%s advId:%d", __FUNCTION__, advId);
    adv_record = ble_adv_get_adv_record_by_advId(advId);
    if(!adv_record) {
        adv_record = ble_adv_get_free_adv_record();
        adv_record->advId = advId;
    }
    if(!adv_record) {
        LOG_W("%s there is no adv record!", __FUNCTION__);
        return -1;
    }

    app_ble_custom_set_adv_data(adv_record->adv_index, advData, advLen, rspData, rspLen);
    return 0;
}

int tg_ble_adv_start_adv(int advId, uint16_t adv_interval, uint8_t adv_type, int8_t tx_power_dbm)
{
    tg_ble_adv_record_t * adv_record = NULL;
    LOG_I("%s advId:%d", __FUNCTION__, advId);
    adv_record = ble_adv_get_adv_record_by_advId(advId);
    if(!adv_record) {
        adv_record = ble_adv_get_free_adv_record();
        adv_record->advId = advId;
    }
    if(!adv_record) {
        LOG_W("%s there is no adv record!", __FUNCTION__);
        return -1;
    }
    app_ble_custome_set_adv_para(adv_record->adv_index, NULL, NULL, adv_interval, adv_type, tx_power_dbm);
    app_ble_custom_adv_start_3p(adv_record->adv_index);
    return 0;
}

int tg_ble_adv_stop_adv(int advId)
{
    tg_ble_adv_record_t * adv_record = NULL;
    LOG_I("%s advId:%d", __FUNCTION__, advId);
    adv_record = ble_adv_get_adv_record_by_advId(advId);
    if(!adv_record) {
        LOG_W("%s advId:%d error", __FUNCTION__, advId);
        return -1;
    }
    app_ble_custom_adv_stop_3p(adv_record->adv_index);
    tg_ble_adv_free_record(advId);
    return 0;
}

void AppBleStopAdvReport(int advId)
{
    if(advId >= TG_ADV_MAX_SIZE){
        LOG_W("%s advId:%d error", __func__, advId);
        return;
    }

    tg_ble_adv_free_record(advId);
}

int tg_ble_adv_mesh_set_adv_data(uint8_t *advData, uint8_t advLen, uint8_t *rspData, uint8_t rspLen)
{
    return tg_ble_adv_set_adv_data(TG_ADV_MESH_ID, advData, advLen, rspData, rspLen);
}

int tg_ble_adv_gattc_set_adv_data(uint8_t *advData, uint8_t advLen, uint8_t *rspData, uint8_t rspLen)
{
    return tg_ble_adv_set_adv_data(TG_ADV_GATTC_DEFAULT_ID, advData, advLen, rspData, rspLen);
}

int tg_ble_adv_mesh_start_adv(uint16_t adv_interval, uint8_t adv_type)
{
    return tg_ble_adv_start_adv(TG_ADV_MESH_ID, adv_interval, adv_type, BLE_ADV_TX_POWER_LEVEL_2);
}

int tg_ble_adv_gattc_start_adv(uint16_t adv_interval, uint8_t adv_type)
{
    return tg_ble_adv_start_adv(TG_ADV_GATTC_DEFAULT_ID, adv_interval, adv_type, BLE_ADV_TX_POWER_LEVEL_2);
}

int tg_ble_adv_mesh_stop_adv(void)
{
    return tg_ble_adv_stop_adv(TG_ADV_MESH_ID);
}

int tg_ble_adv_gattc_stop_adv(void)
{
    return tg_ble_adv_stop_adv(TG_ADV_GATTC_DEFAULT_ID);
}


int tg_ble_adv_start_scan(tg_scan_type_e type, enum BLE_SCAN_FILTER_POLICY scanFilterPolicy,
                            BLE_SCAN_PARAM_T *param)
{
    LOG_I("%s start scan, type:%d", __FUNCTION__, type);
    gScanParam.scan_type = type;
    gScanParam.scan_flag = true;
    if(type == TG_SCAN_MESH) {
        gScanParam.mesh_scan_flag = true;
    } else if(type == TG_SCAN_GATTC) {
        gScanParam.gattc_scan_flag = true;
    }
    gScanParam.scanFilterPolicy = scanFilterPolicy;
    memcpy((uint8_t*)&gScanParam.scan_param, (uint8_t*)param, sizeof(BLE_SCAN_PARAM_T));

    app_ble_start_scan(scanFilterPolicy, param->scanWindow, param->scanInterval);
    return 0;
}

int tg_ble_adv_stop_scan(tg_scan_type_e type)
{
    LOG_I("%s stop scan, type:%d", __FUNCTION__, type);
    if(!gScanParam.scan_flag) {
        return 0;
    }
    if(type == TG_SCAN_MESH) {
        gScanParam.mesh_scan_flag = false;
    } else if(type == TG_SCAN_GATTC) {
        gScanParam.gattc_scan_flag = false;
    }

    if(gScanParam.mesh_scan_flag || gScanParam.gattc_scan_flag) {
        return 0;
    }
    //stop scan
    app_ble_stop_scan();
    gScanParam.scan_flag = false;
    return 0;
}

void tg_ble_adv_scan_result_callback(ble_bdaddr_t *bleAddr,int8_t rssi, uint8_t evt_type, uint8_t *adv_buf, uint8_t len)
{
    if(ble_adv_mesh_report_cb) {
        ble_adv_mesh_report_cb(bleAddr, rssi, evt_type, adv_buf, len);
    }
    if(ble_adv_gattc_report_cb) {
        ble_adv_gattc_report_cb(bleAddr, rssi, evt_type, adv_buf, len);
    }
}

int tg_ble_adv_init(tg_scan_type_e type, ble_adv_data_report_cb_t cb)
{
    static bool init_flag = false;

    if(!init_flag) {
        LOG_I("%s init adv", __FUNCTION__);
        init_flag = true;
        app_ble_custom_init();
        app_ble_adv_report_callback_register(tg_ble_adv_scan_result_callback);
    }
    if(type == TG_SCAN_MESH) {
        ble_adv_mesh_report_cb = cb;
    } else if(type == TG_SCAN_GATTC) {
        ble_adv_gattc_report_cb = cb;
    }
    return 0;
}

int tg_ble_adv_mesh_init(ble_adv_data_report_cb_t cb)
{
    return tg_ble_adv_init(TG_SCAN_MESH, cb);
}

int tg_ble_adv_gattc_init(ble_adv_data_report_cb_t cb)
{
    return tg_ble_adv_init(TG_SCAN_GATTC, cb);
}

int tg_ble_adv_mesh_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy,
                                    BLE_SCAN_PARAM_T *param)
{
    return tg_ble_adv_start_scan(TG_SCAN_MESH, scanFilterPolicy, param);
}

int tg_ble_adv_gattc_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy,
                                    BLE_SCAN_PARAM_T *param)
{
    return tg_ble_adv_start_scan(TG_SCAN_GATTC, scanFilterPolicy, param);
}

int tg_ble_adv_mesh_stop_scan(void)
{
    return tg_ble_adv_stop_scan(TG_SCAN_MESH);
}

int tg_ble_adv_gattc_stop_scan(void)
{
    return tg_ble_adv_stop_scan(TG_SCAN_GATTC);
}

