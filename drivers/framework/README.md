# HDF<a name="EN-US_TOPIC_0000001078041442"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Usage](#section1312121216216)
    -   [HDF](#section129654513264)
    -   [Sensor](#section188637474417)
    -   [Display](#section161502341317)
    -   [Input](#section12629164020115)
    -   [WLAN](#section11408103183114)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository stores the core source code information of the OpenHarmony driver subsystem, including the driver framework, configuration management, configuration parsing, universal framework model, and unified hardware driver interfaces. It is designed to provide a more precise and efficient development environment, where you can perform one-time development for multi-system deployment.

**Figure  1**  Architecture of the HDF<a name="fig19330181162816"></a>  
![](figures/architecture-of-the-hdf.png)

## Directory Structure<a name="section161941989596"></a>

```
/drivers/framework
├── ability        # Capabilities for the driver development, such as the message model libraries
│   ├── config     # Parsing code of the configuration
│   └── sbuf       # Data serialization code
├── core           # Core code for implementing the HDF
│   ├── adapter    # Kernel adaptation layer
│   ├── common     # Common basic code
│   ├── host       # Driver host environment module
│   ├── manager    # Management module
│   └── shared     # Code shared by the host and manager modules
├── include        # Header files for the HDF to provide capabilities externally
│   ├── config     # Header files declaring capabilities for parsing configuration
│   ├── core       # Header files exposed externally
│   ├── net        # Header files related to network operations
│   ├── osal       # Header files of the OS adaptation layer
│   ├── platform   # Header files declaring platform APIs
│   ├── utils      # Header files declaring common capabilities
│   └── wifi       # Header files for the WLAN module to provide capabilities externally
├── model          # Universal framework module for drivers
│   ├── display    # Display framework module
│   ├── input      # Input framework module
│   ├── network    # WLAN framework module
│   └── sensor     # Sensor driver module
├── support        # Basic capabilities
│   └── platform   # Platform driver framework and APIs, including GPIO, I2C, and SPI
├── tools          # Source code related to the tools of the HDF
│   └── hc-gen     # Source code of the configuration management tool
└── utils          # Basic data structures and algorithms
```

## Usage<a name="section1312121216216"></a>

### HDF<a name="section129654513264"></a>

To develop a driver based on the HDF, you only need to register and configure required APIs. The driver framework will load and initialize the driver based on the parsing content.

Driver development based on the HDF consists of the following three parts:

-   Driver: Develop the functions.

-   Information configuration: Present the loading information of the driver.

-   Resource configuration: Configure the hardware information of the driver.

You need to complete the logic code for the functions of a driver by the following APIs.

The first part that catches your eyes is the driver entry, which is described through  **DriverEntry**.

Three APIs are available, namely  **Bind**,  **Init**, and  **Release**.

```
struct HdfDriverEntry g_deviceSample = {
    .moduleVersion = 1,
    .moduleName = "sample_driver", 
    .Bind = SampleDriverBind,
    .Init = SampleDriverInit,
    .Release = SampleDriverRelease,
};
```

**Bind**: This API is used to bind driver devices and its functions.

```
int32_t SampleDriverBind(struct HdfDeviceObject *deviceObject)
{
    return HDF_SUCCESS;
}
```

**Init**: When devices are successfully bound, the HDF calls  **Init**  to initialize the driver. After initialization is complete, the HDF will determine whether to create external service interfaces based on the configuration file. If the driver fails to be initialized, the driver framework will automatically release the created device interface.

```
int32_t SampleDriverInit(struct HdfDeviceObject *deviceObject)
{
    return HDF_SUCCESS;
}
```

**Release**: When you need to uninstall a driver, the HDF calls this function to release the driver resources. Then, other internal resources will be released.

```
void SampleDriverRelease(struct HdfDeviceObject *deviceObject)
{
    // Release all resources.
    return;
}
```

For details, see  [HDF Overview](https://gitee.com/openharmony/docs/blob/master/en/device-dev/driver/driver-hdf.md).

### Sensor<a name="section188637474417"></a>

The sensor driver module is developed based on the HDF and supports functions such as cross-OS migration and differentiated device configuration.

-   APIs for implementing sensor driver module capabilities: Implement the capabilities of registering, loading, and deregistering sensor drivers as well as detecting sensor device depending on the HDF, normalize APIs for sensor devices of the same type, and offer APIs for parsing register configurations, abstract APIs for bus access, and abstract platform APIs.
-   APIs to be implemented by developers: Based on the HDF Configuration Source \(HCS\), implement differentiated configuration for sensors of the same type and serialized configuration of sensor device parameters, and offer APIs for some sensor device operations to simplify the sensor driver development.

For details, see  [Sensor Driver Overview](https://gitee.com/openharmony/docs/blob/master/en/device-dev/driver/driver-peripherals-sensor-des.md).

### Display<a name="section161502341317"></a>

The display driver model that is developed based on the HDF shields the differences among chip platforms, achieving cross-platform migration of the OS. It also abstracts the common service logic of peripherals and configures differentiated adaptation APIs so that a driver model can be compatible with different peripheral. In this way, third-party vendors can efficiently access the OpenHarmony driver ecosystem.

-   APIs for implementing display driver module capabilities: Implement the Hardware Driver Interfaces \(HDIs\) and their adaptation with the chip platform. In addition, the kernel-mode driver abstracts the common services of the panel driver and provides capabilities of initializing the panel, obtaining the panel configuration, powering on/off the panel, and implementing the backlight control.
-   APIs to be implemented by developers: Complete the board-level HCS configuration and private data configuration of the panel, or offer differentiated APIs for some components to ensure efficient development of the display driver.

For details, see  [LCD Overview](https://gitee.com/openharmony/docs/blob/master/en/device-dev/driver/driver-peripherals-lcd-des.md).

### Input<a name="section12629164020115"></a>

The input driver model is developed based on the HDF, provides unified driver APIs for upper-layer input services, and is decoupled from the chip platform. In addition, it abstracts several types of common platform drivers based on different input devices and is compatible with those input devices through configuration and differentiated peripheral APIs.

-   APIs for implementing input driver module capabilities: Implement the HDIs and provide capabilities of managing devices, controlling services, and reporting data. Besides, the input driver model provides a unified driver for different input devices and the capabilities of registering/unregistering an input device, reporting event data, parsing configuration, and loading a common driver.
-   APIs to be implemented by developers: Based on the provided platform driver, add the device descriptions as well as private configuration of the input device and implement differentiated APIs to greatly shorten the time required for developing input drivers.

For details, see  [Touchscreen Overview](https://gitee.com/openharmony/docs/blob/master/en/device-dev/driver/driver-peripherals-touch-des.md).

### WLAN<a name="section11408103183114"></a>

The WLAN module is developed based on the HDF and supports cross-OS migration, component adaptation, and modular assembly and compilation. Based on the unified APIs provided by the WLAN module, driver developers of WLAN vendors can adapt their driver code and developers of the Hardware Driver Interfaces \(HDIs\) are capable of creating, disabling, scanning, and connecting to WLAN hotspots.

-   APIs for implementing WLAN driver module capabilities: Implement the APIs of the WLAN HDI layer and provide capabilities of setting/obtaining the MAC address, obtaining the feature type, and setting the transmit power for upper-layer input services, as well as the capabilities of creating/releasing a  **WifiModule**, connecting to/disconnecting from a WLAN hotspot, and applying for/releasing a  **NetBuf**  for developers.
-   APIs to be implemented by developers: Based on the provided platform driver, complete the board-level HCS configuration as well as the differentiated WLAN configuration, and offer APIs for initializing, deregistering, enabling, and disabling a network device.

For details, see  [WLAN Overview](https://gitee.com/openharmony/docs/blob/master/en/device-dev/driver/driver-peripherals-external-des.md).

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral/blob/master/README.md)

