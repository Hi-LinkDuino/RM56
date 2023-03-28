#ifndef BESGATTCLIENTAPPTEST_H
#define BESGATTCLIENTAPPTEST_H

#include "ohos_bt_def.h"

void RegisterClientFunCb(int status, int clientId, const BtUuid *appUuid);
void  ConnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr);
void  DisconnectClientFunCb(int connId, int status, int clientId, const BdAddr *bdAddr);
void  ConnectParaUpdateFunCb(int connId, int interval, int latency, int timeout, int status);
void  SearchServiceCompleteFunCb(int connId, int status);
void  ReadCharacteristicFunCb(int connId, int status, BtGattReadData *readData);
void  WriteCharacteristicFunCb(int connId, int status, unsigned short handle);
void  ReadDescriptorFunCb(int connId, int status, BtGattReadData *readData);
void  WriteDescriptorFunCb(int connId, int status, unsigned short handle);
void  ExecuteWriteFunCb(int connId, int status);
void  ConfigureMtuSizeFunCb(int connId, int status, int mtuSize);
void  ReadRemoteRssiFunCb(int clientId, const BdAddr *bdAddr, int rssi, int status);
void  RegisterNotificationFunCb(int connId, int registered, int status, unsigned short handle);
void  NotificationFunCb(int connId, BtGattNotifyData notifyData);
#endif
