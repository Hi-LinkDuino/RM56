# light

- [简介](##简介)
- [目录](##目录)
- [说明](##说明)
  - [接口说明](###接口说明)
  - [使用说明](###使用说明)

- [相关仓](##相关仓)

## 简介

 Light驱动模型为上层Light硬件服务层提供稳定的灯控制能力接口，包括获取灯类型、配置点灯模式、配置灯闪烁效果、点灯、熄灯等。基于HDF（ Hardware Device Interface ）驱动框架开发的Light驱动模型，实现跨操作系统迁移，器件差异配置等功能。实现Light驱动“一次开发，多系统部署”的目标。

**图 1**  Light驱动模型图

![Light驱动模型图](figures/Light%E9%A9%B1%E5%8A%A8%E6%A8%A1%E5%9E%8B%E5%9B%BE.png)

## 目录

Light驱动下源代码目录结构如下所示：

```
/drivers/peripheral/misc/light
├── hal              # light模块hal层代码
│   ├── include      # light模块hal层内部头文件
│   └── src          # light模块hal层代码的实现 
├── interfaces       # light模块对上层服务提供的驱动能力接口
│   └── include      # light模块对外提供的接口定义
└── test             # light模块测试代码
    └── unittest     # light模块单元测试代码
```

## 说明

### 接口说明

Light驱动模型支持获取系统中所有灯的信息，动态配置闪烁模式和闪烁时间的能力。Light硬件服务调用GetLightInfo获取Light设备的基本信息；调用TurnOnLight接口启动配置的闪烁效果。开发能力如下表1：

**表 1** Light的主要接口

| 接口名                                                       | 功能描述                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| int32_t (*GetLightInfo)(struct LightInfo **lightInfo, uint32_t *count) | 获取系统中所有灯的信息，lightInfo表示灯设备的基本信息，count表示获取灯的个数。 |
| int32_t (*TurnOnLight)(uint32_t type, struct LightEffect *effect) | 打开指定类型的灯，type表示灯类型，effect表示要设置的效果信息。 |
| int32_t (*TurnOffLight)(uint32_t type)                       | 关闭指定类型的灯。                                           |

### 使用说明

代码示例，以小型系统为例。

```c++
#include "light_if.h"

void LightSample(void)
{
	int32_t ret;
    static struct LightInfo *g_lightInfo = nullptr;
    static uint32_t g_count = 0;
    const int32_t g_onTime = 500;
    const int32_t g_offTime = 500;
    const int32_t LIGHT_WAIT_TIME = 30;
    const int32_t g_minLightType = LIGHT_TYPE_NONE;
    const int32_t g_maxLightType = LIGHT_TYPE_BUTT;
    struct LightEffect effect;

	/* 创建Light接口实例 */
    struct LightInterface *g_lightDev = NewLightInterfaceInstance();
    if (g_lightDev == NULL) {
        return;
    }
	/* 获取设备支持的Light列表 */
 	ret = g_lightDev->GetLightInfo(&g_lightInfo, &g_count);

    /* 按照指定的灯颜色，常亮模式打开灯列表中可用的灯 */
    effect.lightBrightness = 0x80000000; // 亮度值，RGB最高位表示颜色：R:16-31位、G:8-15位、B:0-7位
    effect.flashEffect.flashMode = LIGHT_FLASH_NONE;

    for (i = 0; i < g_count; ++i) {
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightType, &effect);
        EXPECT_EQ(0, ret);

        OsalSleep(LIGHT_WAIT_TIME);

        ret = g_lightDev->TurnOffLight(g_lightInfo[i].lightType);
        EXPECT_EQ(0, ret);
    }
    /* 按照指定的灯颜色，闪烁模式打开灯列表中可用的灯 */
    effect.lightBrightness = 0x80000000;
    effect.flashEffect.flashMode = LIGHT_FLASH_TIMED;
    effect.flashEffect.onTime = g_onTime;    // 一个闪烁周期内亮灯时长（ms）
    effect.flashEffect.offTime = g_offTime;    // 一个闪烁周期内熄灯时长（ms）
    
    for (i = 0; i < g_count; ++i) {
        ret = g_lightDev->TurnOnLight(g_lightInfo[i].lightType, &effect);
        EXPECT_EQ(0, ret);

        OsalSleep(LIGHT_WAIT_TIME);

        ret = g_lightDev->TurnOffLight(g_lightInfo[i].lightType);
        EXPECT_EQ(0, ret);
    }

    /* 关闭灯列表中指定类型的灯 */
    ret = g_lightDev->TurnOffLight(lightType);
    EXPECT_EQ(0, ret);

    /* 释放Light接口实例 */
    if (g_lightDev != nullptr) {
        FreeLightInterfaceInstance();
        g_lightDev = nullptr;
    }
}
```

## 相关仓

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers_adapter_khdf_linuk](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)
[drivers_peripheral](https://gitee.com/openharmony/drivers_peripheral)

[drivers_interface](https://gitee.com/openharmony/drivers_interface)

