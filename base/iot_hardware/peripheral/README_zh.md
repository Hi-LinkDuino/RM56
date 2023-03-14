# IOT硬件子系统<a name="ZH-CN_TOPIC_0000001117425053"></a>

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [约束](#section1718733212019)

## 简介<a name="section11660541593"></a>

IOT（The Internet of Things）硬件设备操作的接口。本模块提供设备操作接口有：FLASH,GPIO,I2C,PWM,UART,WATCHDOG等

## 目录<a name="section1464106163817"></a>

```
/base/iot_hardware/peripheral
├── interfaces
    └── kits                      #IOT设备操作接口,对外接口存放目录  
```

## 约束<a name="section1718733212019"></a>

子系统使用C语言编写，目前仅支持Hi3861开发板。如果您想接入其它芯片，那么仅需要适配实现鸿蒙的集成接口即可。

