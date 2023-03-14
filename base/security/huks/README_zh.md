# HUKS组件<a name="ZH-CN_TOPIC_0000001148528849"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

HUKS（OpenHarmony Universal KeyStore，OpenHarmony通用密钥库系统）向应用提供密钥库能力，包括密钥管理及密钥的密码学操作等功能。HUKS所管理的密钥可以由应用导入或者由应用调用HUKS接口生成。

HUKS模块可以分为如下三大部分：

-   HUKS SDK层：提供HUKS API供应用调用。

-   HUKS Service层：实现HUKS密钥管理、存储等功能。
-   HUKS Engine层：HUKS核心模块，负责密钥生成以及加解密等工作。对于标准系统设备，该部分模块在商用场景下必须在安全环境下运行，包括TEE或者具备安全能力的芯片等。由于安全环境需要特定硬件支持，因此在开源代码中为模拟实现。对于小型和轻量系统，HUKS模块仅提供根密钥保护方案的模拟实现，商用场景下必须根据产品能力适配硬件根密钥或者使用其他根密钥保护方案。

## 目录<a name="section161941989596"></a>

```
base/security/huks/
├── build                             # 编译配置文件
├── frameworks                        # 框架代码, 作为基础功能目录, 被interfaces和services使用.
│   └── huks_standard                 # 代码实现
│   └── huks_lite                     # 小型和轻量系统编译脚本
│   └── crypto_lite                   # 加解密实现
├── interfaces                        # 接口API代码
│   └── innerkits
│   └── kits
├── services
│   └── huks_standard
├── test
├── unittest
└── utils
   └── crypto_adapter
   └── file_operator
   └── list
   └── mutex
```

## 相关仓<a name="section1371113476307"></a>

**安全子系统**

**security_huks**


