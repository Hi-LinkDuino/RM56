# vibrator

- [简介](##简介)
- [目录](##目录)
- [说明](##说明)
  - [接口说明](###接口说明)
  - [使用说明](###使用说明)

- [相关仓](##相关仓)

## 简介

Vibrator驱动模型主要包含Vibrator（传感器）相关的HDI接口与实现，提供Vibrator HDI（ Hardware Device Interface ）能力接口，支持静态HCS配置的时间序列和动态配置持续时间两种振动效果。调用StartOnce接口动态配置持续振动时间；调用StartEffect接口启动静态配置的振动效果。

**图 1** Vibrator驱动模型图

![Vibrator驱动模型图](figures/Vibrator%E9%A9%B1%E5%8A%A8%E6%A8%A1%E5%9E%8B%E5%9B%BE.png)

## 目录

Vibraor驱动下源代码目录结构如下所示：

```
/drivers/peripheral/misc/vibrator
├── hal              # vibrator模块hal层代码
│   ├── include      # vibrator模块hal层内部头文件
│   └── src          # vibrator模块hal层代码的实现 
├── interfaces       # vibrator模块对上层服务提供的驱动能力接口
│   └── include      # vibrator模块对外提供的接口定义
└── test             # vibrator模块测试代码
    └── unittest     # vibrator模块单元测试代码
```

## 说明

### 接口说明

马达主要提供的功能：触发震动，停止震动。开发能力如下表1：

**表 1**马达的主要接口

| 接口名                                 | 功能描述                                                   |
| -------------------------------------- | ---------------------------------------------------------- |
| int32_t  StartOnce(uint32_t duration)  | 按照指定持续时间触发震动，duration为振动持续时长。         |
| int32_t  Start(const char *effectType) | 按照指定预置效果启动马达，effectType表示预置的振动效果串。 |
| int32_t  Stop(enum VibratorMode mode)  | 按照指定的振动模式停止震动。                               |

### 使用说明

代码示例

```
#include "vibrator_if.h"

enum VibratorMode {
    VIBRATOR_MODE_ONCE   = 0,    // 指定时间内的一次振动
    VIBRATOR_MODE_PRESET = 1,    // 指定预置效果的周期性振动
};

void VibratorSample(void)
{
	int32_t startRet;
	int32_t endRet;
	uint32_t g_duration = 1000;
	uint32_t g_sleepTime1 = 2000;
	uint32_t g_sleepTime2 = 5000;
	const char *g_timeSequence = "haptic.clock.timer";
	/* 创建传感器接口实例 */
    struct VibratorInterface *g_vibratorDev = NewVibratorInterfaceInstance();
    if (g_vibratorDev == NULL) {
        return;
    }
	/* 按照指定持续时间触发震动*/
	startRet = g_vibratorDev->StartOnce(g_duration);
    if (startRet != 0) {
        return;
    }
    OsalMSleep(g_sleepTime1);
    /* 按照指定的振动模式停止震动 */
    endRet = g_vibratorDev->Stop(VIBRATOR_MODE_ONCE);
    if (endRet != 0) {
        return;
    }
    /* 释放传感器接口实例 */
    ret = FreeVibratorInterfaceInstance();
    if (ret != 0) {
        return;
    }
    /* 创建传感器接口实例 */
    struct VibratorInterface *g_vibratorDev = NewVibratorInterfaceInstance();
    if (g_vibratorDev == NULL) {
        return;
    }
    /* 按照指定预置效果启动马达 */
    startRet = g_vibratorDev->Start(g_timeSequence);
    if (endRet != 0) {
        return;
    }
    OsalMSleep(g_sleepTime2);
	/* 按照指定的振动模式停止震动 */ 
    endRet = g_vibratorDev->Stop(VIBRATOR_MODE_PRESET);
    if (endRet != 0) {
        return;
    }
    /* 释放传感器接口实例 */
    ret = FreeVibratorInterfaceInstance();
    if (ret != 0) {
        return;
    }
}
```

## 相关仓

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers_adapter_khdf_linuk](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

[drivers_peripheral](https://gitee.com/openharmony/drivers_peripheral)
