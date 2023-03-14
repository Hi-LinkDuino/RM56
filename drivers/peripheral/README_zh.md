# peripheral<a name="ZH-CN_TOPIC_0000001101652164"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

此仓主要包含各外设器件驱动相关的HDI（Hardware Driver Interface）接口、HAL实现、驱动模型及测试用例等，根据模块划分不同目录，具体模块的相关信息可参阅各模块子目录下的readme。

## 目录<a name="section161941989596"></a>

该仓代码目录为 /drivers/peripheral，其中包含的各子目录简介如下。

-   audio：Audio HDI接口的定义，用于管理声卡驱动的加载和卸载、创建音频播放对象及录音对象、选择音频场景、设置音频属性及音量、控制音频播放及录音的启停等，详见audio目录下的[readme](audio/README_zh.md)文档。
-   codec：Codec HDI接口的定义，这些接口对上层服务提供媒体编解码的驱动能力，详见codec目录下的[readme](codec/README_zh.md)文档。
-   display：Display HDI 接口定义及其默认实现，对上层图形服务提供显示驱动能力，包括显示图层的管理、显示内存的管理及硬件图形加速，详见display目录下的[readme](display/README_zh.md)文档。

-   format：Format HDI接口定义，此类接口对上层服务提供媒体文件复用和解复用的驱动能力，详见format目录下的[readme](format/README_zh.md)文档。
-   input：Input HDI接口定义及其实现，对上层输入系统服务提供操作input设备的驱动能力，包括input设备管理、业务流控制、数据上报等，详见input目录下的[readme](input/README_zh.md)文档。
-   sensor：Sensor HDI接口定义与实现，接口主要包括所有Sensor信息查询、Sensor启停、Sensor订阅/去订阅、Sensor参数配置等稳定的接口，简化服务开发，详见sensor目录下的[readme](sensor/README_zh.md)文档。
-   wlan：WLAN HDI接口定义与实现，包括创建和销毁HAL层和WLAN驱动的通道、获取本设备支持的WLAN特性等，详见wlan目录下的[readme](wlan/README_zh.md)文档。
-   usb：USB DDK接口定义与实现，负责提供USB Host侧和USB Device侧的设备管理、配置、数据读写等接口，详见usb目录下的[readme](usb/README_zh.md)文档。
-   camera：Camera HDI接口定义及其实现，对上层Camera Service提供HDI的具体实现，对下管理Camera各个硬件设备等，详见camera目录下的[readme](camera/README_zh.md)文档。
## 相关仓<a name="section1371113476307"></a>

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)



