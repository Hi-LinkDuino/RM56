# 项目介绍<a name="ZH-CN_TOPIC_0000001124444687"></a>

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [约束](#section13591154411)
-   [涉及仓](#section12828123401219)

## 简介<a name="section11660541593"></a>

Wi-Fi Aware模块提供了近场通信能力，该模块可被上层应用所使用。

## 目录<a name="section1464106163817"></a>

```
foundation/communication/wifi_aware/         # 模块目录
├── frameworks
│  └── source                            # 模块源文件
├── hals                                  # 硬件抽象层头文件
└── interfaces
    └── kits                              # 模块对外提供接口文件
```

## 约束<a name="section13591154411"></a>

WiFi Aware模块使用C语言编写，目前仅支持Hi3861开发板。如果您想接入其它芯片，那么仅需要适配实现鸿蒙的集成接口即可，将实现放在device下。

## 涉及仓<a name="section12828123401219"></a>

[分布式软总线子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%88%86%E5%B8%83%E5%BC%8F%E8%BD%AF%E6%80%BB%E7%BA%BF%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**communication\_wifi\_aware**

[communication\_softbus\_lite](https://gitee.com/openharmony/communication_softbus_lite/blob/master/README_zh.md)

[communication\_ipc\_lite](https://gitee.com/openharmony/communication_ipc_lite/blob/master/README_zh.md)

