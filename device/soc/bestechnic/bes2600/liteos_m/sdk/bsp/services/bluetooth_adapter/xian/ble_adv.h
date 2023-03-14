#ifndef __BLE_ADV_H__
#define __BLE_ADV_H__

typedef struct {
    int advType;
    int txPower;
    int minInterval;
    int maxInterval;
    int channelMap;
    int timeout;
} EnableMultiAdvParam;

typedef struct {
    bool setScanRsp;
    bool includeName;
    bool includeTxPower;
    int appearance;
    int manufacturerLen;
    char *manufacturerData;
    int srvcDataLen;
    char *srvcData;
    int srvcUuidLen;
    char *srvcUuid;
} SetDataMultiAdvParam;

#endif

