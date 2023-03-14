# HDF Adapter<a name="EN-US_TOPIC_0000001138458641"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the code and compilation scripts for the kerner or user-mode Hardware Driver Foundation \(HDF\) of the OpenHarmony driver subsystem and provides basic capabilities for the HDF.

## Directory Structure<a name="section161941989596"></a>

```
/drivers/adapter
├── khdf/liteos         # HDF dependent adapter for LiteOS-A kernel
├── khdf/liteos_m       # HDF dependent adapter for LiteOS-M kernel
├── uhdf                # HDF user mode interface dependent adapter
└── uhdf2               # HDF user mode dependent adapter
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

