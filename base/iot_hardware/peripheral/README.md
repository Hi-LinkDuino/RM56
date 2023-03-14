# IoT Hardware<a name="EN-US_TOPIC_0000001117425053"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Constraints](#section1718733212019)

## Introduction<a name="section11660541593"></a>

The Internet of Things \(IoT\) hardware subsystem provides APIs for operating IoT devices, including flash, general-purpose input/output \(GPIO\), Inter-Integrated Circuit \(I2C\), pulse width modulation \(PWM\), universal asynchronous receiver-transmitter \(UART\), and watchdog APIs.

## Directory Structure<a name="section1464106163817"></a>

```
/base/iot_hardware/peripheral
├── interfaces
    └── kits                      # APIs for operating IoT devices
```

## Constraints<a name="section1718733212019"></a>

The IoT hardware subsystem is compiled using the C language. Currently, only the Hi3861 development board is supported. If you want to use other chips, you need to implement OpenHarmony APIs.

