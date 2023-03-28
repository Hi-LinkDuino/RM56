#include "ohos_bt_a2dp_sink.h"
#include "btapp.h"
#include "app_bt_func.h"
#include "app_a2dp.h"

typedef struct {
    int ConnState;
    int StrategyType;
    int PlayingState;
} bt_a2dp_sink_db_t;

static bt_a2dp_sink_db_t gA2dpSinkDb = {OHOS_PROFILE_STATE_DISCONNECTED,OHOS_CONNECTION_UNKNOWN,OHOS_A2DP_NOT_PLAYING};
static BtA2dpSinkCallbacks *A2dpConnStateChangeCallback = NULL;

void A2dpConnectStateChange(app_a2dp_event_e type, const unsigned char* addr, uint32_t state)
{
    BdAddr remot_addr;
    memcpy(remot_addr.addr,addr,OHOS_BD_ADDR_LEN);
    switch(type)
    {
        case APP_A2DP_EVENT_CONNECTION_STATE:
            gA2dpSinkDb.ConnState = state;
            if(A2dpConnStateChangeCallback && A2dpConnStateChangeCallback->connectstateChangedCb)
                A2dpConnStateChangeCallback->connectstateChangedCb(&remot_addr,state);
        break;
        case APP_A2DP_EVENT_PLAY_STATUS:
            gA2dpSinkDb.PlayingState = state;
            if(A2dpConnStateChangeCallback && A2dpConnStateChangeCallback->playingStateChangedCb)
                A2dpConnStateChangeCallback->playingStateChangedCb(&remot_addr,state);
        break;
    }

}

int A2dpSinkGetDeviceConnectState(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if(curr_device && memcmp(curr_device->remote.address,bdAddr->addr,OHOS_BD_ADDR_LEN) == 0){
        return gA2dpSinkDb.ConnState;
    }
    return -1;
}

int A2dpSinkGetPlayingState(const BdAddr *bdAddr)
{
    return gA2dpSinkDb.PlayingState;
}

int A2dpSinkConnect(const BdAddr *bdAddr)
{
    BdAddr remot_addr;
    if (bdAddr == NULL){
        return -1;
    }
    memcpy(remot_addr.addr,bdAddr->addr,OHOS_BD_ADDR_LEN);
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if(curr_device && memcmp(curr_device->remote.address,bdAddr->addr,OHOS_BD_ADDR_LEN) == 0 && 
        gA2dpSinkDb.StrategyType != OHOS_CONNECTION_FORBIDDEN){
        A2dpConnectStateChange(APP_A2DP_EVENT_CONNECTION_STATE,(uint8_t*)remot_addr.addr,OHOS_PROFILE_STATE_CONNECTING);
        app_bt_A2DP_OpenStream(curr_device->profile_mgr.stream,(bt_bdaddr_t*)remot_addr.addr);
        return 0;
    }
    else
        return -1;
}

int A2dpSinkDisconnect(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if(curr_device && memcmp(curr_device->remote.address,bdAddr->addr,OHOS_BD_ADDR_LEN) == 0){
        A2dpConnectStateChange(APP_A2DP_EVENT_CONNECTION_STATE,bdAddr->addr,OHOS_PROFILE_STATE_DISCONNECTING);
        app_bt_A2DP_CloseStream(curr_device->profile_mgr.stream);
        return 0;
    }
    else
        return -1;
}

int A2dpSinkSetConnectStrategy(const BdAddr *bdAddr, int strategy)
{
    if (bdAddr == NULL){
        return -1;
    }
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if(curr_device && memcmp(curr_device->remote.address,bdAddr->addr,OHOS_BD_ADDR_LEN) == 0){
        gA2dpSinkDb.StrategyType = strategy;
        return 0;
    }
    else
        return -1;
}

int A2dpSinkGetConnectStrategy(const BdAddr *bdAddr)
{
    if (bdAddr == NULL){
        return -1;
    }
    struct BT_DEVICE_T *curr_device = app_bt_get_device(BT_DEVICE_ID_1);
    if(curr_device && memcmp(curr_device->remote.address,bdAddr->addr,OHOS_BD_ADDR_LEN) == 0){
        return gA2dpSinkDb.StrategyType;
    }
    return -1;
}

int A2dpSinkRegisterCallbacks(BtA2dpSinkCallbacks *func)
{
    A2dpConnStateChangeCallback = func;
    app_a2dp_state_change_callback_register((void*)A2dpConnectStateChange);
    return 0;
}

int A2dpSinkDeregisterCallbacks(BtA2dpSinkCallbacks *func)
{
    A2dpConnStateChangeCallback = NULL;
    app_a2dp_state_change_callback_deregister();
    return 0;
}
