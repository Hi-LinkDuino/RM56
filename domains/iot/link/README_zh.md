# IOT子系统<a name="ZH-CN_TOPIC_0000001085756528"></a>

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [涉及仓](#section1718733212019)

## 简介<a name="section11660541593"></a>

IOT子系统为平台开发者提供了集成三方SDK的示例参考。

## 目录<a name="section1464106163817"></a>

```
domains/iot/                              # 仓目录
└── link
    ├── BUILD.gn                       # 构建脚本
    ├── demolink                       # 三方厂商与平台接口的适配层构建目录
    │   ├── BUILD.gn
    │   ├── demosdk_adapter.c
    │   └── demosdk_adapter.h
    └── libbuild                       # 三方厂商SDK构建目录
        ├── BUILD.gn
        ├── demosdk.c
        └── demosdk.h
```

## 涉及仓<a name="section1718733212019"></a>

iot\_link

