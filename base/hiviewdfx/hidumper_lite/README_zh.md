# hiviewdfx_blackbox

-   [简介](#简介)
-   [目录](#目录)
-   [hidumper_lite的使用](#hidumper_lite的使用)
-   [系统调用接口定义](#系统调用接口定义)
-   [重写接口](#重写接口)
-   [相关仓](#相关仓)

## 简介

hidumper_lite提供liteOS_M系统下dump接口，帮助开发者提供故障日志、内存、CPU等信息的导出功能。

## 目录

Hiview源代码目录结构如下：

```
/base/hiviewdfx/hidumper_lite.
├── hidumper.c                              #AT命令处理过程
└── mini
    ├── hidumper_adapter.c                  #hidumper功能函数适配层
    ├── hidumper_core.c                     #hidumper核心层
    └── interfaces                          #公共接口文件

```

## hidumper_lite的使用

hidumper_lite在liteOS_M下面是通过AT命令进行数据查询的

- 参数：
  | 命令格式 | 命令含义 |
  | -------- | -------- |
  | AT+HIDUMPER= | 打印出CPU、内存和全部的task使用信息 |
  | AT+HIDUMPER=-dc | 打印出CPU使用信息 |
  | AT+HIDUMPER=-dm | 打印出内存使用信息 |
  | AT+HIDUMPER=-df | 打印出最后的故障日志信息 |
  | AT+HIDUMPER=-dt | 打印出全部task信息 |
  | AT+HIDUMPER=-h | 打印出帮助信息 |
  | AT+HIDUMPER=-ikc | 向内核注入故障 |
  | AT+HIDUMPER=-m | 以16进制形式打印出全部内存数据 |
  | AT+HIDUMPER=-m,memstart,memsize | 以16进制形式打印出特定内存区域数据。两个参数，memstart：内存起始地址，memsize：内存大小 |

## 系统调用接口定义

### at_hidumper

```
int at_hidumper(unsigned int argc, const char **argv)
```

注入到系统的AT指令处理函数

- 参数：
  | 成员 | 类型 | 必填 | 说明 |
  | -------- | -------- | -------- | -------- |
  | argc | unsigned int | 是 | 输入参数的个数。 |
  | argv | const char** | 是 | 输入参数的内容 |


## 重写接口

实例可以参考hi3861芯片的重写，对应位置为device/soc/hisilicon/hi3861v100/sdk_liteos/components/at/src/hidumper_adapter_impl.c

以下的函数都是需要在不同的芯片下重写的：

### DumpSysInfo

```
int DumpSysInfo(void);
```

重写打印系统信息函数。该函数打印当前设备系统信息。

### DumpCpuUsage

```
int DumpCpuUsage(void);
```

重写打印CPU使用信息函数。该函数打印当前设备CPU使用信息。

### DumpMemUsage

```
int DumpMemUsage(void);
```

重写打印内存使用信息函数。该函数打印当前设备内存使用信息。

### DumpTaskInfo

```
int DumpTaskInfo(void);
```

重写打印系统全部Task信息函数。该函数打印系统中全部的task信息。

### DumpFaultLog

```
int DumpFaultLog(void);
```

重写打印异常日志信息函数。该函数打印最后保存的日志信息。

### DumpMemRegion

```
int DumpMemRegion(unsigned long long addr, unsigned long long size)
```

重写打印指定区域内存数据函数。该函数打印从addr开始一共size这大小的内存区域的数据。

- 参数：
  | 参数名 | 类型 | 必填 | 说明 |
  | -------- | -------- | -------- | -------- |
  | addr | unsigned long long | 是 | 需要查询的内存起始地址 |
  | size | unsigned long long | 是 | 需要查询的内大小 |

### DumpAllMem

```
int DumpAllMem(void); 
```

重写打印全部内存数据函数。该函数能打印全部内存数据。

### PlatformHiDumperIinit

```
int PlatformHiDumperIinit(void); 
```

重写平台hidumper初始化函数。该函数根据不同平台初始化HiDumper的at命令等特性。

## 相关仓

[DFX子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/DFX%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**hiviewdfx\_hidumper\_lite**

[hiviewdfx\_blackbox](https://gitee.com/openharmony/hiviewdfx_blackbox/blob/master/README_zh.md)

[hiviewdfx\_hilog\_lite](https://gitee.com/openharmony/hiviewdfx_hilog_lite/blob/master/README_zh.md)

[hiviewdfx\_hievent\_lite](https://gitee.com/openharmony/hiviewdfx_hievent_lite/blob/master/README_zh.md)

[hiviewdfx\_hiview\_lite](https://gitee.com/openharmony/hiviewdfx_hiview_lite/blob/master/README_zh.md)
