# khdf\_liteosm<a name="EN-US_TOPIC_0000001096228646"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the code and compilation scripts for the OpenHarmony driver subsystem to adapt to the liteos\_m kernel and to deploy the hardware driver foundation \(HDF\).

## Directory Structure<a name="section161941989596"></a>

```
/drivers/adapter/khdf/liteos_m
├── core                 # Driver code for adapting to the LiteOS Cortex-M kernel
├── hcs                  # Script for compiling the HDF configuration source (HCS) file
├── osal                 # System APIs for adapting to the LiteOS Cortex-M kernel
└── test                 # Test code for the kernel driver framework
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

