# full\_uhdf<a name="EN-US_TOPIC_0000001138458641"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the code and compilation scripts for the user-mode Hardware Driver Foundation \(HDF\) of the OpenHarmony driver subsystem and provides basic capabilities for the HDF.

## Directory Structure<a name="section161941989596"></a>

```
/drivers/adapter/uhdf2
├── config        # Configuration parsing module
├── hdi           # Driver HDIs
├── host          # Host environment module
├── include       # Header files
├── ipc           # IPC module
├── manager       # Management module
├── osal          # Compilation scripts for adapting to the POSIX APIs
├── security      # Security module
└── shared        # Code shared by the host and manager modules
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

