# LiteIPC<a name="EN-US_TOPIC_0000001084861846"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Usage](#section1312121216216)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

LiteIPC is a lite module for inter-process communication \(IPC\). It provides IPC capabilities for the system service framework and is implemented in both the kernel space and user space. In the kernel space, LiteIPC provides functionalities such as inter-process message sending and receiving, IPC memory management, timeout notification, and death notification. In the user space, LiteIPC provides marshalling and unmarshalling capabilities and distributes IPC callback messages and death messages.

## Directory Structure<a name="section161941989596"></a>

```
/foundation/communication/ipc_lite
├── frameworks         # Framework code
│   └── liteipc
│       ├── include   # Header files
│       ├── src       # Source code
├── interfaces         # APIs
│   └── kits          # IPC, marshalling, and unmarshalling APIs
```

## Usage<a name="section1312121216216"></a>

LiteIPC provides the IPC, marshalling, and unmarshalling capabilities. The IPC capability is encapsulated by the samgr\_lite module. You can use the service-related APIs provided by samgr\_lite and the marshalling and unmarshalling APIs provided by LiteIPC to invoke services across processes. For details about how to develop, register, obtain, and invoke services, see the  [samgr\_lite repository](en-us_topic_0000001081604584.md).

## Repositories Involved<a name="section1371113476307"></a>

[Intelligent Soft Bus subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/intelligent-soft-bus.md)

**communication\_ipc\_lite**

[communication\_softbus\_lite](https://gitee.com/openharmony/communication_softbus_lite/blob/master/README.md)

[communication\_wifi\_aware](https://gitee.com/openharmony/communication_wifi_aware/blob/master/README.md)

