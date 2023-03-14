# IPC/RPC组件<a name="ZH-CN_TOPIC_0000001103602398"></a>

-   [简介](#section11660541593)
-   [系统架构](#section1950291414611)
-   [目录](#section161941989596)
-   [约束](#section119744591305)
-   [编译构建](#section137768191623)
-   [说明](#section1312121216216)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

IPC（Inter-Process Communication）与RPC（Remote Procedure Call）机制用于实现跨进程通信，不同的是前者使用Binder驱动，用于设备内的跨进程通信，而后者使用软总线驱动，用于跨设备跨进程通信。IPC和RPC通常采用客户端-服务器（Client-Server）模型，服务请求方（Client）可获取提供服务提供方（Server）的代理 （Proxy），并通过此代理读写数据来实现进程间的数据通信。通常，系统能力（System Ability）Server侧会先注册到系统能力管理者（System Ability Manager，缩写SAMgr）中，SAMgr负责管理这些SA并向Client提供相关的接口。Client要和某个具体的SA通信，必须先从SAMgr中获取该SA的代理，然后使用代理和SA通信。三方应用可以使用FA提供的接口绑定服务提供方的Ability，获取代理，进行通信。下文使用Proxy表示服务请求方，Stub表示服务提供方。

## 系统架构<a name="section1950291414611"></a>

**图 1**  IPC通信机制架构图<a name="fig312319321710"></a>  
![](figures/ipc-architecture.png "IPC通信机制架构图")

## 目录<a name="section161941989596"></a>

```
/foundation/communication/ipc
├── interfaces        # 对外接口存放目录
│   └── innerkits     # 对内部子系统暴露的头文件存放目录
│       ├── ipc_core     # ipc 接口存放目录
│       └── libdbinder   # dbinder 接口存放目录
├── ipc            # ipc 框架代码
│   ├── native     # ipc native 实现存放目录
│       ├── src    # ipc native 源代码存放目录
│       └── test   # ipc native 单元测试用例存放目录
│   └── test       # ipc native 模块测试用例存放目录
├── service        # dbinder 实现存放目录
│   └── dbinder    # dbinder 源代码存放目录
```

## 约束<a name="section119744591305"></a>

1. 单个设备上跨进程通信时，传输的数据量最大约为1MB，过大的数据量请使用匿名共享内存。
2. 不支持把跨设备的Proxy对象传递回该Proxy对象所指向的Stub对象所在的设备。

## 编译构建<a name="section137768191623"></a>

**JS侧依赖**

```
import rpc from "@ohos.rpc"
import featureAbility from "@ohos.ability.featureAbility"
```

**Native侧编译依赖**

sdk依赖：

```
external_deps = [
  "ipc:ipc_core",
]
```

此外， IPC/RPC依赖的refbase实现在公共基础库实现//utils下，请增加对utils的源码依赖：

```
deps = [
  "//utils/native/base:utils",
]
```

## 说明<a name="section1312121216216"></a>

**JS侧实现跨进程通信基本步骤：**

1. 获取代理

   使用ohos.ability.featureAbility提供的connectAbility方法绑定Ability，在参数里指定要绑定的Ability所在应用的包名、组件名，如果是跨设备的情况，还需要指定所在设备的ID。用户需要在服务端的onConnect方法里返回一个继承自ohos.rpc.RemoteObject的对象，此对象会在onRemoteRequest方法里接收到请求。

2. 发送请求

   客户端在connectAbility参数指定的回调函数接收到代理对象后，使用ohos.rpc模块提供的方法完成RPC通信，其中MessageParcel提供了读写各种类型数据的方法，IRemoteObject提供了发送请求的方法，RemoteObject提供了处理请求的方法onRemoteRequest，用户需要重写。

**Native侧实现跨进程通信的基本步骤：**

1. 定义接口类

   接口类继承IRemoteBroker，定义描述符、业务函数和消息码。

2. 实现服务提供端\(Stub\)

   Stub继承IRemoteStub\(Native\)，除了接口类中未实现方法外，还需要实现AsObject方法及OnRemoteRequest方法。

3. 实现服务请求端\(Proxy\)

   Proxy继承IRemoteProxy\(Native\)，封装业务函数，调用SendRequest将请求发送到Stub。

4. 注册SA

   服务提供方所在进程启动后，申请SA的唯一标识，将Stub注册到SAMgr。

5. 获取SA

6. 通过SA的标识和设备标识，从SAMgr获取Proxy，通过Proxy实现与Stub的跨进程通信。

### 接口说明<a name="section1551164914237"></a>

**表 1**  JS侧IPC关键API

| 模块                        | 方法                                                         | 功能说明                                    |
| --------------------------- | ------------------------------------------------------------ | ------------------------------------------- |
| ohos.ability.featureAbility | connectAbility(request: Want, options:ConnectOptions ): number | 绑定指定的Ability，在回调函数里接收代理对象 |
| ohos.rpc.RemoteObject       | onRemoteRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): boolean | 服务端处理请求，返回结果                    |
| ohos.rpc.IRemoteObject      | sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): Promise<SendRequestResult> | 发送请求，在期约里接收结果                  |
| ohos.rpc.IRemoteObject      | sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption, callback: AsyncCallback<SendRequestResult>): void | 发送请求，在回调函数里接收结果              |
| ohos.rpc.MessageParcel      | writeRemoteObject(object: IRemoteObject): boolean            | 序列化IRemoteObject对象                     |
| ohos.rpc.MessageParcel      | readRemoteObject(): IRemoteObject                            | 反序列化IRemoteObject对象                   |



**表 2**  Native侧IPC接口

<a name="table178849240013"></a>

<table><thead align="left"><tr id="row6884924608"><th class="cellrowborder" valign="top" width="14.12141214121412%" id="mcps1.2.4.1.1"><p id="p98846241706"><a name="p98846241706"></a><a name="p98846241706"></a>类/接口</p>
</th>
<th class="cellrowborder" valign="top" width="52.54525452545254%" id="mcps1.2.4.1.2"><p id="p1488482414020"><a name="p1488482414020"></a><a name="p1488482414020"></a>方法</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p388516244016"><a name="p388516244016"></a><a name="p388516244016"></a>功能说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row15885824402"><td class="cellrowborder" valign="top" width="14.12141214121412%" headers="mcps1.2.4.1.1 "><p id="p08859241008"><a name="p08859241008"></a><a name="p08859241008"></a>IRemoteBroker</p>
</td>
<td class="cellrowborder" valign="top" width="52.54525452545254%" headers="mcps1.2.4.1.2 "><p id="p388572412010"><a name="p388572412010"></a><a name="p388572412010"></a>sptr&lt;IRemoteObject&gt; AsObject()</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p13885724405"><a name="p13885724405"></a><a name="p13885724405"></a>返回通信对象。派生类需要实现，Stub端返回RemoteObject对象本身，Proxy端返回代理对象。</p>
</td>
</tr>
<tr id="row138859241808"><td class="cellrowborder" valign="top" width="14.12141214121412%" headers="mcps1.2.4.1.1 "><p id="p1888515245012"><a name="p1888515245012"></a><a name="p1888515245012"></a>IRemoteStub</p>
</td>
<td class="cellrowborder" valign="top" width="52.54525452545254%" headers="mcps1.2.4.1.2 "><p id="p1388516240011"><a name="p1388516240011"></a><a name="p1388516240011"></a>virtual int OnRemoteRequest(uint32_t code, MessageParcel &amp;data, MessageParcel &amp;reply, MessageOption &amp;option)</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p1188582414016"><a name="p1188582414016"></a><a name="p1188582414016"></a>请求处理方法，派生类需要重写，处理Proxy的请求并返回结果。</p>
</td>
</tr>
<tr id="row108856241904"><td class="cellrowborder" valign="top" width="14.12141214121412%" headers="mcps1.2.4.1.1 "><p id="p6885924609"><a name="p6885924609"></a><a name="p6885924609"></a>IRemoteProxy</p>
</td>
<td class="cellrowborder" valign="top" width="52.54525452545254%" headers="mcps1.2.4.1.2 ">&nbsp;&nbsp;</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p688592413018"><a name="p688592413018"></a><a name="p688592413018"></a>业务Proxy类派生自IRemoteProxy类。</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

**JS侧使用说明**

1. 客户端构造变量want，指定要绑定的Ability所在应用的包名、组件名，如果是跨设备的场景，还需要目标设备ID。构造变量connect，指定绑定成功、绑定失败、断开连接时的回调函数。使用featureAbility提供的接口绑定Ability。

   ```
   import rpc from "@ohos.rpc"
   import featureAbility from "@ohos.ability.featureAbility"
   
   let proxy = null
   let connectId = null
   
   // 单个设备
   let want = {
       // 包名和组件名写实际的值
       "bundleName": "ohos.rpc.test.server",
       "abilityName": "ohos.rpc.test.server.ServiceAbility",
   }
   let connect = {
       onConnect:function(elementName, remote) {
           proxy = remote
       },
       onDisconnect:function(elementName) {
       },
       onFailed:function() {
           proxy = null
       }
   }
   connectId = featureAbility.connectAbility(want, connect)
   
   // 如果是跨设备绑定，可以使用deviceManager获取目标设备ID
   import deviceManager from '@ohos.distributedHardware.deviceManager'
   function deviceManagerCallback(deviceManager) {
       let deviceList = deviceManager.getTrustedDeviceListSync()
       let deviceId = deviceList[0].deviceId
       let want = {
           "bundleName": "ohos.rpc.test.server",
           "abilityName": "ohos.rpc.test.service.ServiceAbility",
           "deviceId": deviceId,
           "flags": 256
       }
       connectId = featureAbility.connectAbility(want, connect)
   }
   // 第一个参数是本应用的包名，第二个参数是接收deviceManager的回调函数
   deviceManager.createDeviceManager("ohos.rpc.test", deviceManagerCallback)
   ```

   

2. 服务端被绑定的Ability在onConnect方法里返回继承自rpc.RemoteObject的对象，该对象需要实现onRemoteRequest方法，处理客户端的请求。

   ```
   import rpc from "@ohos.rpc"
   onConnect(want) {
       return new Stub("rpcTestAbility")
   }
   class Stub extends rpc.RemoteObject {
       constructor(descriptor) {
           super(descriptor)
       }
       onRemoteRequest(code, data, reply, option) {
           // 根据code处理客户端的请求
           return true
       }
   }
   ```

   

3. 客户端在onConnect回调里接收到代理对象，调用sendRequest方法发起请求，在期约或者回调函数里接收结果。

   ```
   import rpc from "@ohos.rpc"
   // 使用期约
   let option = new rpc.MessageOption()
   let data = rpc.MessageParcel.create()
   let reply = rpc.MessageParcel.create()
   // 往data里写入参数
   proxy.sendRequest(1, data, reply, option)
       .then(function(result) {
           if (result.errCode != 0) {
               console.error("send request failed, errCode: " + result.errCode)
               return
           }
           // 从result.reply里读取结果
       })
       .catch(function(e) {
           console.error("send request got exception: " + e)
       }
       .finally(() => {
           data.reclaim()
           reply.reclaim()
       })
   
   // 使用回调函数
   function sendRequestCallback(result) {
       try {
           if (result.errCode != 0) {
               console.error("send request failed, errCode: " + result.errCode)
               return
           }
           // 从result.reply里读取结果
       } finally {
           result.data.reclaim()
           result.reply.reclaim()
       }
   }
   let option = new rpc.MessageOption()
   let data = rpc.MessageParcel.create()
   let reply = rpc.MessageParcel.create()
   // 往data里写入参数
   proxy.sendRequest(1, data, reply, option, sendRequestCallback)
   ```

   

4. IPC通信结束后，使用featureAbility的接口断开连接。

   ```
   import rpc from "@ohos.rpc"
   import featureAbility from "@ohos.ability.featureAbility"
   function disconnectCallback() {
       console.info("disconnect ability done")
   }
   featureAbility.disconnectAbility(connectId, disconnectCallback)
   ```

   

**Native侧使用说明**

1. 定义IPC接口ITestAbility

   IPC接口继承IPC基类接口IRemoteBroker，接口里定义描述符、业务函数和消息码，其中业务函数在Proxy端和Stub端都需要实现。

   ```
   class ITestAbility : public IRemoteBroker {
   public:
   // DECLARE_INTERFACE_DESCRIPTOR是必须的， 入参需使用std::u16string；
   DECLARE_INTERFACE_DESCRIPTOR(u"test.ITestAbility"); // DESCRIPTOR接口描述符建议使用"组件名.类名"的格式
   int TRANS_ID_PING_ABILITY = 1; // 定义消息码
   virtual int TestPingAbility(const std::u16string &dummy) = 0; // 定义业务函数
   };
   ```

   

2. 定义和实现服务端TestAbilityStub

   该类是和IPC框架相关的实现，需要继承 IRemoteStub<ITestAbility\>。Stub端作为接收请求的一端，需重写OnRemoteRequest方法用于接收客户端调用。

   ```
   class TestAbilityStub : public IRemoteStub<ITestAbility> {
   public:
       virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
       int TestPingAbility(const std::u16string &dummy) override;
   };
    
   int TestServiceStub::OnRemoteRequest(uint32_t code,
       MessageParcel &data, MessageParcel &reply, MessageOption &option)
   {
       if (data.ReadInterfaceToken() != GetDescriptor()) { //校验是否为本服务的接口描述符，避免中继攻击
           return -1;
       }
       switch (code) {
           case TRANS_ID_PING_ABILITY: {
               std::u16string dummy = data.ReadString16();
               int result = TestPingAbility(dummy);
               reply.WriteInt32(result);
               return 0;
           }
           default:
               return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
       }
   }
   ```

   

3. 定义服务端业务函数具体实现类TestAbility

   ```
   class TestAbility : public TestAbilityStub {
   public:
       int TestPingAbility(const std::u16string &dummy);
   }
   
   int TestAbility::TestPingAbility(const std::u16string &dummy) {
       return 0;
   }
   ```

   

4. 定义和实现客户端TestAbilityProxy

   该类是Proxy端实现，继承IRemoteProxy<ITestAbility\>，调用SendRequest接口向Stub端发送请求，对外暴露服务端提供的能力。

   ```
   class TestAbilityProxy : public IRemoteProxy<ITestAbility> {
   public:
       explicit TestAbilityProxy(const sptr<IRemoteObject> &impl);
       int TestPingService(const std::u16string &dummy) override;
   private:
       static inline BrokerDelegator<TestAbilityProxy> delegator_; // 方便使用iface_cast宏
   }
   
   TestAbilityProxy::TestAbilityProxy(const sptr<IRemoteObject> &impl)
       : IRemoteProxy<ITestAbility>(impl)
   {
   }
   
   int TestAbilityProxy::TestPingService(const std::u16string &dummy) {
       MessageOption option;
       MessageParcel dataParcel, replyParcel;
       if(!dataParcel.WriteInterfaceToken(GetDescriptor())) { //所有对外接口的proxy实现都要写入接口描述符，用于stub端检验
           return -1;
       }
       if(!dataParcel.WriteString16(dummy)) {
           return -1;
       }
       int error = Remote()->SendRequest(TRANS_ID_PING_ABILITY, dataParcel, replyParcel, option);
       int result = (error == ERR_NONE) ? replyParcel.ReadInt32() : -1;
       return result;
   }
   ```

   

5. 同步调用与异步调用

   essageOption作为发送接口（原型如下）的入参，可设定同步（TF\_SYNC）、异步（TF\_ASYNC）、接收FD（TF\_ACCEPT\_FDS），默认情况下设定为同步，其余可通过MessageOption构造方法或void SetFlags\(int flags\)设定。

   ```
   int SendRequest(uint32_t code, MessageParcel &data,
       MessageParcel &reply, MessageOption &option) override;
   MessageOption option = { MessageOption::TF_ASYNC };
   ```

   

6. SA注册与启动

   SA需要将自己的TestAbilityStub实例通过AddSystemAbility接口注册到SystemAbilityManager，设备内与分布式的注册参数不同。

   ```
   // 注册到本设备内
   auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
   samgr->AddSystemAbility(said, new TestAbility());
   
   // 在组网场景下，会被同步到其他设备上
   auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
   ISystemAbilityManager::SAExtraProp saExtra;
   saExtra.isDistributed = true; // 设置为分布式SA
   int result = samgr->AddSystemAbility(said, new TestAbility(), saExtra);
   ```

   

7. SA获取与调用

   通过SystemAbilityManager的GetSystemAbility方法可获取到对应SA的代理IRemoteObject，然后构造TestAbilityProxy即可。

   ```
   // 获取本设备内注册的SA的proxy
   sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
   sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(said);
   sptr<ITestAbility> testAbility = iface_cast<ITestAbility>(remoteObject); // 使用iface_cast宏转换成具体类型
   
   // 获取其他设备注册的SA的Proxy
   sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
   sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(sdid, deviceId); // deviceId是指定设备的标识符
   sptr<TestAbilityProxy> proxy(new TestAbilityProxy(remoteObject)); // 直接构造具体Proxy
   ```



## 相关仓<a name="section1371113476307"></a>

分布式软总线子系统

**communication\_ipc**

utils

utils\_native

distributedschedule\_samgr

