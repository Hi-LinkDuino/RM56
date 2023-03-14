# syspara\_lite<a name="EN-US_TOPIC_0000001081867232"></a>

-   [Introduction](#section469617221261)
-   [Directory Structure](#section692981610397)
-   [Constraints](#section741841418125)
-   [Usage](#section1464106163817)
-   [Repositories Involved](#section641143415335)

## Introduction<a name="section469617221261"></a>

The syspara\_lite module provides APIs for obtaining device information, such as the product name, brand name, and manufacturer name, based on the OpenHarmony Product Compatibility Specifications \(PCS\). It also provides APIs for setting and obtaining system attributes.

## Directory Structure<a name="section692981610397"></a>

```
base/startup/syspara_lite/    # syspara_lite module
├── frameworks             # Source files for the syspara_lite module
├── hals                   # Header files for the hardware abstraction layer of the syspara_lite module
└── interfaces             # External APIs for the syspara_lite module
```

## Constraints<a name="section741841418125"></a>

-   The syspara\_lite module is developed using the C language.
-   It supports mini-system devices \(reference memory ≥ 128 KB\), such as Hi3861 V100, and small-system devices \(reference memory ≥ 1 MB\), such as Hi3516D V300 and Hi3518E V300.
-   The fields of system attributes are defined by original equipment manufacturers \(OEMs\). Currently, only default values are provided. The specific values need to be adjusted as required.

## Usage<a name="section1464106163817"></a>

To obtain the system attributes, use the following code:

```
char* value1 = GetDeviceType();
printf("Device type =%s\n", value1);
free(value1);
char* value2 = GetManufacture();
printf("Manufacture =%s\n", value2);
free(value2);
char* value3 = GetBrand();
printf("GetBrand =%s\n", value3);
free(value3);
```

## Repositories Involved<a name="section641143415335"></a>

[Startup subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/startup.md)

**[startup\_syspara\_lite](https://gitee.com/openharmony/startup_syspara_lite/blob/master/README.md)**

[startup\_appspawn\_lite](https://gitee.com/openharmony/startup_appspawn_lite/blob/master/README.md)

[startup\_bootstrap\_lite](https://gitee.com/openharmony/startup_bootstrap_lite/blob/master/README.md)

[startup\_init\_lite](https://gitee.com/openharmony/startup_init_lite/blob/master/README.md)

