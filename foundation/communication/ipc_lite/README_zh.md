# LiteIPC组件<a name="ZH-CN_TOPIC_0000001084861846"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [说明](#section1312121216216)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

LiteIPC为轻量级进程间通信组件，为面向服务的系统服务框架提供进程间通信能力，分为内核实现和用户态实现两部分，其中内核实现完成进程间消息收发、IPC内存管理、超时通知和死亡通知等功能；用户态提供序列化和反序列化能力，并完成IPC回调消息和死亡消息的分发。

## 目录<a name="section161941989596"></a>

```
/foundation/communication/ipc_lite
├── frameworks         # 框架代码
│   └── liteipc
│       ├── include   # 组件内部头文件存放目录
│       ├── src       # 源代码存放目录
├── interfaces         # 轻量级进程间通信组件接口层
│   └── kits          # 进程间通信及序列化反序列化接口
```

## 说明<a name="section1312121216216"></a>

LiteIPC包括进程间通信能力和序列化反序列化能力，其中进程间通信能力已由samgrlite组件进一步封装，开发者可以通过samgrlite提供的服务相关接口配合LiteIPC提供的序列化、反序列化接口完成进程间服务能力调用，服务开发注册、获取和调用的流程请参考[samgrlite组件README](zh-cn_topic_0000001081604584.md)。

## 相关仓<a name="section1371113476307"></a>

[分布式软总线子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%88%86%E5%B8%83%E5%BC%8F%E8%BD%AF%E6%80%BB%E7%BA%BF%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**communication\_ipc\_lite**

[communication\_softbus\_lite](https://gitee.com/openharmony/communication_softbus_lite/blob/master/README_zh.md)

[communication\_wifi\_aware](https://gitee.com/openharmony/communication_wifi_aware/blob/master/README_zh.md)

