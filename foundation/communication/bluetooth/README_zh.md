# 蓝牙组件<a name="ZH-CN_TOPIC_0000001148577119"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [约束](#section119744591305)
-   [说明](#section1312121216216)
    -   [标准系统使用说明](#section1699952017198)
    -   [轻量或小型系统使用说明](#section223312597203)
    -   [C接口使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

蓝牙服务组件为设备提供接入与使用Bluetooth的相关接口，包括BLE设备gatt相关的操作，以及BLE广播、扫描等功能。

## 目录<a name="section161941989596"></a>

```
/foundation/communication/bluetooth
├── interfaces                               # 接口代码
│    └── innerkits                           # 系统服务接口存放目录
│        ├── native_c                        # C接口存放目录
│        │   └── include                     # C接口定义目录
│        └── native_cpp                      # C++接口存放目录
│── sa_profile                               # 蓝牙服务定义目录
│── services                                 # 蓝牙服务代码目录
└── LICENSE                                  # 版权声明文件
```

## 约束<a name="section119744591305"></a>

蓝牙服务组件使用C语言编写。

## 说明<a name="section1312121216216"></a>

现在提供的是BLE相关接口及GAP基本开关蓝牙的接口，其它接口暂不提供。

### 标准系统使用说明<a name="section1699952017198"></a>

标准系统提供C接口定义及服务和协议栈的代码，目前提供的只有BLE相关的接口，包括BLE设备gatt相关的操作，以及BLE广播、扫描等功能，其它A2DP，AVRCP，HFP等相关接口在后续增量发布。

标准系统使用目录如下：

[interfaces/](https://gitee.com/openharmony/communication_bluetooth/tree/master/interfaces)

[sa\_profile/](https://gitee.com/openharmony/communication_bluetooth/tree/master/sa_profile)

[services/](https://gitee.com/openharmony/communication_bluetooth/tree/master/services)

### 轻量或小型系统使用说明<a name="section223312597203"></a>

轻量或小型系统只提供C接口定义，目前只有BLE相关接口，包括BLE设备gatt相关的操作，以及BLE广播、扫描等功能，其它A2DP，AVRCP，HFP等相关接口在后续增量发布。

轻量或小型系统使用目录如下：

interfaces/innerkits/native\_c/include

### C接口使用说明<a name="section129654513264"></a>

-   打开/关闭蓝牙：

```
/* 打开传统蓝牙 */
bool EnableBt(void);
/* 关闭传统蓝牙 */
bool DisableBt(void);
/* 打开低功耗蓝牙 */
bool EnableBle(void);
/* 关闭低功耗蓝牙 */
bool DisableBle(void);
```

-   获取蓝牙状态

```
/* 获取传统蓝牙状态 */
int GetBtState();
/* 判断低功耗蓝牙状态 */
bool IsBleEnabled();
```

-   获取本端Mac地址

```
/* 获取mac地址 */
bool GetLocalAddr(unsigned char *mac, unsigned int len);
```

-   设置本端设备名称

```
/* 设置本端设备名称 */
bool SetLocalName(unsigned char *localName, unsigned char length);
```

-   设备需要完成以下接口的开发步骤，以使能Gatt server功能并开启服务：

```
/* 初始化蓝牙协议栈 */
int InitBtStack(void);
int EnableBtStack(void);
/* 注册应用，appUuid由应用提供  */
int BleGattsRegister(BtUuid appUuid);
/* 添加服务 */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);
/* 添加特征值 */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid, int properties, int permissions);
/* 添加描述符 */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);
/* 开始服务 */
int BleGattsStartService(int serverId, int srvcHandle);
```

-   设备需要完成以下接口的开发步骤，以使能BLE广播的发送：

```
/* 设置广播数据 */
int BleSetAdvData(int advId, const BleConfigAdvData *data);
/* 开始发送广播 */
int BleStartAdv(int advId, const BleAdvParams *param);
```

-   若需要扫描功能，设备需要完成以下接口的开发步骤，以使能BLE扫描功能：

```
/* 设置扫描参数 */
int BleSetScanParameters(int clientId, BleScanParams *param);
/* 开始扫描 */
int BleStartScan(void);
```

## 相关仓<a name="section1371113476307"></a>

communication\_bluetooth

