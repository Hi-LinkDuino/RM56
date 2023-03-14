# OTA<a name="EN-US_TOPIC_0000001078451366"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Constraints](#section1718733212019)
-   [Usage](#section18867101215181)
-   [Repositories Involved](#section68521336131912)

## Introduction<a name="section11660541593"></a>

Over the Air \(OTA\) provides the remote device update capability. Your devices will be able to support OTA update after secondary development based on the provided APIs. By providing unified update APIs externally, the update subsystem shields the differences of underlying chips.

## Directory Structure<a name="section1464106163817"></a>

```
/base/update/ota_lite
├── frameworks        # OTA update implementation, including update package parsing, verification, writing, and updating
│   └── test          # Self-test code
│   └── unittest      # Unit test code
│   └── source        # updater module source code
│       └── updater   # updater module code
│       └── verify    # Verification algorithm code
├── interfaces
│   └── kits          # External APIs for OTA update
├── hals              # Chip adaptation code, for example, HiSilicon chip adaptation code is located at device\hisilicon\hardware\update
```

## Constraints<a name="section1718733212019"></a>

The update subsystem is compiled using the C language. Currently, only the Hi3518EV300, Hi3516DV300, and Hi3861 development boards are supported. If you want to support devices that use other chips, you can implement the OpenHarmony integration APIs in the  **vendor**  directory. Currently, only the full-package update is supported.

## Usage<a name="section18867101215181"></a>

Add the dependency on the update subsystem. The following uses the Hi3516D V300 development board as an example.

-   Add  **update**  to the  **subsystem\_list field in the vendor\\hisilicon\\ipcamera\_hi3516dv300\_liteos\\config.json**  file, and add the following code under  **subsystem\_list**:

    ```
    {
            "subsystem": "update",
            "components": [
              { "component": "hota", "features": [] }
            ]
     },
    ```


-   Add the  **update.json**  file to the  **build\\lite\\components**  directory.

    ```
    "components": [
        {
          "component": "hota",
          "description": "",
          "optional": "false",
          "dirs": [
            "base/update/ota_lite/frameworks",
            "base/update/ota_lite/interfaces/kits"
          ],
          "targets": [
            "//base/update/ota_lite/frameworks:ota_lite"
          ],
    ...
    ```


-   Add test code. For example, add  **subsystem\_test**  to the  **base\\update\\ota\_lite\\frameworks\\BUILD.gn**  file.

-   Run the following commands to compile the system. You can experience the OTA update function after flashing the system to the Hi3516 chip.

    ```
    hb set
    hb build
    ```


## Repositories Involved<a name="section68521336131912"></a>

[Update subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/update.md)

**update\_ota\_lite**

[device\_hisilicon\_hardware](https://gitee.com/openharmony/device_hisilicon_hardware/blob/master/README.md)

