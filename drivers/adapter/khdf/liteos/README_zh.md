# lite\_khdf<a name="ZH-CN_TOPIC_0000001078489610"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

该仓主要存放OpenHarmony驱动子系统适配liteos内核的代码和编译脚本，在liteos内核中部署OpenHarmony驱动框架。

## 目录<a name="section161941989596"></a>

```
/drivers/adapter/khdf/liteos
├── model                #驱动模型适配liteos代码
│   ├── bus              #总线驱动模型
│   ├── display          #显示驱动模型
│   ├── input            #输入驱动模型
│   ├── network          #wifi驱动模型
│   └── sensor           #传感器驱动模型
├── ndk                  #适配NDK的编译脚本
├── network              #适配liteos内核网络代码
├── osal                 #适配liteos内核的系统接口
├── test                 #liteos内核驱动框架测试代码
└── tools                #liteos内核驱动开发工具
```

## 相关仓<a name="section1371113476307"></a>

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers\_adapter\_uhdf](https://gitee.com/openharmony/drivers_adapter/blob/master/uhdf/README_zh.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

[drivers\_adapter\_khdf\_liteos](https://gitee.com/openharmony/drivers_adapter/blob/master/khdf/liteos/README_zh.md)
