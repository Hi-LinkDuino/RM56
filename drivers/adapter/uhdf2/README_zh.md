# full\_uhdf<a name="ZH-CN_TOPIC_0000001138458641"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

该仓主要存放OpenHarmony驱动子系统用户态驱动框架的代码和编译脚本，在用户态中提供驱动框架的能力。

## 目录<a name="section161941989596"></a>

```
/drivers/adapter/uhdf2
├── config        #用户态驱动配置解析模块
├── hdi           #用户态驱动的hdi接口
├── host          #用户态驱动宿主环境模块
├── include       #用户态驱动框架提供能力头文件
├── ipc           #用户态驱动进程间通信模块
├── manager       #用户态驱动框架管理模块
├── osal          #适配posix接口代码的编译脚本适配
├── security      #用户态驱动安全模块
└── shared        #manager和host共享代码模块
```

## 相关仓<a name="section1371113476307"></a>

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

