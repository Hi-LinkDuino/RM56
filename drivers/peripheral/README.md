# peripheral<a name="EN-US_TOPIC_0000001101652164"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the hardware driver interfaces \(HDIs\), hardware abstraction layer \(HAL\) implementation, driver models, and test cases related to drivers of peripherals. The directory is structured based on the driver modules. For details about each driver module, see the readme file in the sub-directory of each module.

## Directory Structure<a name="section161941989596"></a>

The code directory of this repository is  **/drivers/peripheral**, which contains the following sub-directories:

-   **audio**: This sub-directory stores definitions of audio HDIs that can be used to manage loading and unloading of sound card drivers, create audio renderers and capturers, select an audio scene, set audio properties, set the audio volume, and start or stop audio playback and recording. For details, see the [readme](audio/README.md) file in the  **audio**  sub-directory.
-   **codec**: This sub-directory stores definitions of codec HDIs, which allow upper-layer services to perform media codec-related operations. For details, see the [readme](codec/README.md) file in the  **codec**  sub-directory.
-   **display**: This sub-directory stores definitions of display HDIs and their default implementations. These HDIs allow upper-layer graphics services to manage display layers, manage memory of the display module, and implement hardware acceleration. For details, see the [readme](display/README.md) file in the  **display**  sub-directory.

-   **format**: This sub-directory stores definitions of format HDIs, allowing upper-layer services to multiplex and demultiplex media files. For details, see the [readme](format/README.md) file in the  **format**  sub-directory.
-   **input**: This sub-directory stores definitions and implementation code of input HDIs, allowing upper-layer input services to perform operations for input devices, including managing input devices, controlling service flows, and reporting input events. For details, see the [readme](input/README.md) file in the  **input**  sub-directory.
-   **sensor**: This sub-directory stores definitions and implementation code of sensor HDIs, including obtaining sensor information, enabling or disabling a sensor, subscribing to or unsubscribing from sensor data, and setting sensor options. For details, see the [readme](sensor/README.md) file in the  **sensor**  sub-directory.
-   **wlan**: This sub-directory stores definitions and implementation code of WLAN HDIs, including creating and stopping a channel between the HAL and the WLAN driver and obtaining the WLAN features supported by the device. For details, see the [readme](wlan/README.md) file in the  **wlan**  sub-directory.

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)


