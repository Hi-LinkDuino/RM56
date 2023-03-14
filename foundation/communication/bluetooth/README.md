# Bluetooth<a name="ZH-CN_TOPIC_0000001148577119"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Usage](#section1312121216216)
    -   [Standard System](#section1699952017198)
    -   [Mini or Small System](#section223312597203)
    -   [C APIs](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

The Bluetooth module provides APIs for accessing and using Bluetooth services, such as APIs for Generic Attribute Profile (GATT) operations, Bluetooth Low Energy (BLE) advertising, and scan.

## Directory Structure<a name="section161941989596"></a>

```
/foundation/communication/bluetooth
├── interfaces                              # API code
│   └── innerkits                           # System service APIs
│       ├── native_c                        # C APIs
│       │   └── include                     # Header files for C APIs
│       └── native_cpp                      # C++ APIs
├── sa_profile                              # Service ability profile
└── services                                # Bluetooth service code
└── LICENSE                                  # License declaration file
```

## Constraints<a name="section119744591305"></a>

The Bluetooth module must be compiled in C language.

## Usage<a name="section1312121216216"></a>

Only BLE-related APIs and basic Bluetooth Generic Access Profile (GAP) APIs are provided.

### Standard System<a name="section1699952017198"></a>

The C API definition as well as the service and protocol stack code are provided for the standard system. Currently, only the BLE-related APIs, including the APIs for GATT operations on BLE devices, BLE advertising, and scan, are provided. Other APIs, such as APIs related to A2DP, AVRCP, and HFP, will be gradually provided later.

The directories for the standard system are as follows:

[interfaces/](https://gitee.com/openharmony/communication_bluetooth/tree/master/interfaces)

[sa\_profile/](https://gitee.com/openharmony/communication_bluetooth/tree/master/sa_profile)

[services/](https://gitee.com/openharmony/communication_bluetooth/tree/master/services)

### Mini or Small System<a name="section223312597203"></a>

Only C APIs for BLE, such as APIs for GATT operations, BLE advertising, and scan, are provided for the mini and small systems. Other APIs, such as APIs related to A2DP, AVRCP, and HFP, will be gradually provided later.

The directory for the mini or standard system is as follows:

interfaces/innerkits/native\_c/include

### C APIs<a name="section129654513264"></a>

-   Enable or disable Bluetooth.

```
/* Enable classic Bluetooth.*/
bool EnableBt(void);
/* Disable classic Bluetooth. */
bool DisableBt(void);
/* Enable BLE. */
bool EnableBle(void);
/* Disable BLE. */
bool DisableBle(void);
```

-   Obtain Bluetooth status.

```
/* Obtain the classic Bluetooth status. */
int GetBtState();
/* Check whether BLE is enabled. */
bool IsBleEnabled();
```

-   Obtain the local MAC address.

```
/* Obtain the MAC address. */
bool GetLocalAddr(unsigned char *mac, unsigned int len);
```

-   Set the name of the local device.

```
/* Set the name of the local device.*/
bool SetLocalName(unsigned char *localName, unsigned char length);
```

-   Enable the GATT server feature and start the GATT service.

```
/* Initialize the Bluetooth protocol stack. */
int InitBtStack(void);
int EnableBtStack(void);
/* Register an application with a specified appUuid. */
int BleGattsRegister(BtUuid appUuid);
/* Add a service. */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);
/* Add a characteristic. */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid, int properties, int permissions);
/* Add a descriptor. */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);
/* Start the GATT service. */
int BleGattsStartService(int serverId, int srvcHandle);
```

-   Enable BLE advertising.

```
/* Set the data to advertise. */
int BleSetAdvData(int advId, const BleConfigAdvData *data);
/* Start advertising. */
int BleStartAdv(int advId, const BleAdvParams *param);
```

-   Enable BLE scan.

```
/* Set scan parameters. */
int BleSetScanParameters(int clientId, BleScanParams *param);
/* Start a scan. */
int BleStartScan(void);
```

## Repositories Involved<a name="section1371113476307"></a>

communication\_bluetooth
