# 项目介绍<a name="ZH-CN_TOPIC_0000001078674782"></a>

-   [简介](#section469617221261)
-   [架构](#section15884114210197)
-   [相关仓](#section767551120815)

## 简介<a name="section469617221261"></a>

为系统内业务组件提供故障、用户行为、功耗统计三类事件打点接口，支持对事件进行序列化。

## 架构<a name="section15884114210197"></a>

1.  事件打点时通过接口先创建一个事件；
2.  通过接口向事件中添加数据；
3.  添加完成后通过接口上报事件；
4.  event组件检查参数有效性后，对事件进行二进制序列化处理将事件转换为结构化数据；
5.  event调用输出接口将事件写入文件，每次新写文件时，先向事件文件中加入公共头信息；
6.  事件信息上报至手机侧的处理由上传组件被动或主动定时完成；

## 相关仓<a name="section767551120815"></a>

[DFX子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/DFX%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[hiviewdfx\_hilog\_lite](https://gitee.com/openharmony/hiviewdfx_hilog_lite/blob/master/README_zh.md)

[hiviewdfx\_hiview\_lite](https://gitee.com/openharmony/hiviewdfx_hiview_lite/blob/master/README_zh.md)

**hiviewdfx\_hievent\_lite**
