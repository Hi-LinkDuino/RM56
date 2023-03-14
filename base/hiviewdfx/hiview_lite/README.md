# hiview Module<a name="EN-US_TOPIC_0000001079122822"></a>

-   [Overview](#section469617221261)
-   [Architecture](#section15884114210197)
-   [Repositories Involved](#section767551120815)

## Overview<a name="section469617221261"></a>

The hiview module provides the initialization of the DFX subsystem and controls the startup of each component as required.

## Architecture<a name="section15884114210197"></a>

The DFX initialization consists of two phases: DEVICE\_INIT and CORE\_INIT.

DEVICE\_INIT:

1. The config component and core configuration parameters \(enabling or disabling the dump or event function\) of the DFX subsystem are initialized.

2. The log component is initialized, without involving the dynamic memory allocation and file operation capabilities.

3. The status information about the current DFX subsystem is written into the config component.

CORE\_INIT:

1. The log, dump, event, and output components are initialized as required based on the parameters of the config component.

2. Memory management and file system operations are started normally. Memory can be requested and files can be created as required.

## Repositories Involved<a name="section767551120815"></a>

[DFX subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/dfx.md)

[hiviewdfx\_hilog\_lite](https://gitee.com/openharmony/hiviewdfx_hilog_lite/blob/master/README.md)

**hiviewdfx\_hiview\_lite**

[hiviewdfx\_hievent\_lite](https://gitee.com/openharmony/hiviewdfx_hievent_lite/blob/master/README.md)