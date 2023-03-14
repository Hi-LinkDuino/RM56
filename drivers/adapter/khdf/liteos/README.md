# lite\_khdf<a name="EN-US_TOPIC_0000001078489610"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the code and compilation scripts for the OpenHarmony driver subsystem to adapt to the LiteOS kernel and to deploy the hardware driver foundation \(HDF\).

## Directory Structure<a name="section161941989596"></a>

```
/drivers/adapter/khdf/liteos
├── model                # Code for adapting to the LiteOS
│   ├── bus              # Bus driver model
│   ├── display          # Display driver model
│   ├── input            # Input driver model
│   ├── network          # WLAN driver model
│   ├── sensor           # Sensor driver model
├── ndk                  # Compilation scripts for adapting to the NDK
├── network              # Code for adapting to the LiteOS kernel network
├── osal                 # System APIs for adapting to the LiteOS kernel
├── test                 # Test code for the LiteOS kernel driver framework
└── tools                # Development tools for the LiteOS kernel
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter\_uhdf](https://gitee.com/openharmony/drivers_adapter/blob/master/uhdf/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_uhdf/blob/master/README.md)

[drivers\_adapter\_khdf\_liteos](https://gitee.com/openharmony/drivers_adapter/blob/master/khdf/liteos/README.md)
