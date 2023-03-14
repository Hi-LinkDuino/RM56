# Hiview介绍<a name="ZH-CN_TOPIC_0000001079122822"></a>

-   [简介](#section469617221261)
-   [架构](#section15884114210197)
-   [相关仓](#section767551120815)

## 简介<a name="section469617221261"></a>

提供DFX子系统整体的初始化功能，控制各组件按需启动

## 架构<a name="section15884114210197"></a>

DFX子系统初始化分为两个阶段：DEVICE\_INIT、CORE\_INIT。

DEVICE\_INIT阶段：

1\)   初始化config模块，初始化DFX子系统核心配置参数（配置开启关闭dump\\event功能）。

2\)   初始化log组件，不能涉及内存动态分配、文件操作能力。

3\)   记录当前DFX子系统的状态信息到config中。

CORE\_INIT阶段：

1\)   根据config的参数，按需初始化log、dump、event及对应的output组件。

2\)   该阶段内存管理、文件系统已经正常启动，可按需申请内存并创建文件。

-   [相关仓](#section767551120815)


## 相关仓<a name="section767551120815"></a>

[DFX子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/DFX%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[hiviewdfx\_hilog\_lite](https://gitee.com/openharmony/hiviewdfx_hilog_lite/blob/master/README_zh.md)

**hiviewdfx\_hiview\_lite**

[hiviewdfx\_hievent\_lite](https://gitee.com/openharmony/hiviewdfx_hievent_lite/blob/master/README_zh.md)
