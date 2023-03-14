# Codec<a name="ZH-CN_TOPIC_0000001131751215"></a>

- [Codec<a name="ZH-CN_TOPIC_0000001131751215"></a>](#codec)
  - [简介<a name="section11660541593"></a>](#简介)
  - [目录<a name="section161941989596"></a>](#目录)
    - [接口说明<a name="section1551164914237"></a>](#接口说明)
    - [使用说明<a name="section129654513264"></a>](#使用说明)
  - [相关仓<a name="section1371113476307"></a>](#相关仓)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

该仓下主要包含Codec模块HDI（Hardware Driver Interface）接口定义及其实现，对上层提供媒体编解码的驱动能力接口，HDI接口主要提供如下功能：

-   创建、销毁编解码器对象；
-   启停编解码器
-   原始码流编码为压缩码流；
-   压缩码流恢复为原始码流
-   清除编解码缓存。

## 目录<a name="section161941989596"></a>

该仓下源代码目录结构如下所示

```
/drivers/peripheral/codec
├── interfaces         # codec模块对上层服务提供的驱动能力接口
│   └── include       # codec模块对外提供的接口定义
```

### 接口说明<a name="section1551164914237"></a>

Codec驱动提供给framework层可直接调用的能力接口，主要功能有：创建、销毁编解码器对象，启停编解码器操作，编解码处理、清除缓存、事件上报等。

提供的部分接口说明如下表所示：

**表 1**  Codec HDI 1.0接口列表

<a name="table1513255710559"></a>
<table><thead align="left"><tr id="row171321857155517"><th class="cellrowborder" align="center" valign="top" width="12.821282128212822%" id="mcps1.2.4.1.1"><p id="p6132957115511"><a name="p6132957115511"></a><a name="p6132957115511"></a>头文件</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="63.61636163616362%" id="mcps1.2.4.1.2"><p id="p14132125715552"><a name="p14132125715552"></a><a name="p14132125715552"></a>接口名称</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="23.562356235623565%" id="mcps1.2.4.1.3"><p id="p18132205755516"><a name="p18132205755516"></a><a name="p18132205755516"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1267017500274"><td class="cellrowborder" rowspan="17" align="left" valign="top" width="12.821282128212822%" headers="mcps1.2.4.1.1 "><p id="p15674038913"><a name="p15674038913"></a><a name="p15674038913"></a></p>
<p id="p169466917181"><a name="p169466917181"></a><a name="p169466917181"></a></p>
<p id="p12106161012185"><a name="p12106161012185"></a><a name="p12106161012185"></a></p>
<p id="p7255161017181"><a name="p7255161017181"></a><a name="p7255161017181"></a></p>
<p id="p1839901041815"><a name="p1839901041815"></a><a name="p1839901041815"></a></p>
<p id="p4540171015188"><a name="p4540171015188"></a><a name="p4540171015188"></a></p>
<p id="p368816102189"><a name="p368816102189"></a><a name="p368816102189"></a></p>
<p id="p679481051814"><a name="p679481051814"></a><a name="p679481051814"></a></p>
<p id="p7399191241817"><a name="p7399191241817"></a><a name="p7399191241817"></a></p>
<p id="p11902121271818"><a name="p11902121271818"></a><a name="p11902121271818"></a></p>
<p id="p1296415129182"><a name="p1296415129182"></a><a name="p1296415129182"></a></p>
<p id="p1742101311180"><a name="p1742101311180"></a><a name="p1742101311180"></a></p>
<p id="p171041613101815"><a name="p171041613101815"></a><a name="p171041613101815"></a></p>
<p id="p10172141314183"><a name="p10172141314183"></a><a name="p10172141314183"></a></p>
<p id="p723216132180"><a name="p723216132180"></a><a name="p723216132180"></a></p>
<p id="p825185015460"><a name="p825185015460"></a><a name="p825185015460"></a>codec_interface.h</p>
<p id="p2133757135510"><a name="p2133757135510"></a><a name="p2133757135510"></a></p>
<p id="p14171441118"><a name="p14171441118"></a><a name="p14171441118"></a></p>
<p id="p57063567463"><a name="p57063567463"></a><a name="p57063567463"></a></p>
<p id="p1285144710118"><a name="p1285144710118"></a><a name="p1285144710118"></a></p>
<p id="p854114711117"><a name="p854114711117"></a><a name="p854114711117"></a></p>
<p id="p15741647517"><a name="p15741647517"></a><a name="p15741647517"></a></p>
<p id="p7909447418"><a name="p7909447418"></a><a name="p7909447418"></a></p>
<p id="p12525910165811"><a name="p12525910165811"></a><a name="p12525910165811"></a></p>
<p id="p942322013262"><a name="p942322013262"></a><a name="p942322013262"></a></p>
<p id="p1040515339526"><a name="p1040515339526"></a><a name="p1040515339526"></a></p>
<p id="p19405533115216"><a name="p19405533115216"></a><a name="p19405533115216"></a></p>
</td>
<td class="cellrowborder" valign="top" width="63.61636163616362%" headers="mcps1.2.4.1.2 "><p id="p174121125175619"><a name="p174121125175619"></a><a name="p174121125175619"></a>int32_t CodecInit();</p>
</td>
<td class="cellrowborder" valign="top" width="23.562356235623565%" headers="mcps1.2.4.1.3 "><p id="p114126258569"><a name="p114126258569"></a><a name="p114126258569"></a>codec模块初始化处理</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1341212595610"><a name="p1341212595610"></a><a name="p1341212595610"></a>int32_t CodecDeinit();</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1341272515610"><a name="p1341272515610"></a><a name="p1341272515610"></a>codec模块去初始化处理</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14413122555620"><a name="p14413122555620"></a><a name="p14413122555620"></a>int32_t CodecEnumerateCapbility(uint32_t index, CodecCapbility *cap);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1141372575614"><a name="p1141372575614"></a><a name="p1141372575614"></a>获取某一个编解码能力</p>
</td>
</tr>
<tr id="row181371630162816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1941362515561"><a name="p1941362515561"></a><a name="p1941362515561"></a>int32_t CodecGetCapbility(AvCodecMime mime, CodecType type, uint32_t flags, CodecCapbility *cap);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p44131025115619"><a name="p44131025115619"></a><a name="p44131025115619"></a>获取指定编解码能力</p>
</td>
</tr>
<tr id="row01531026142811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p241312251562"><a name="p241312251562"></a><a name="p241312251562"></a>int32_t CodecCreate(const char* name, const Param *attr, int len, CODEC_HANDLETYPE *handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p12413525135615"><a name="p12413525135615"></a><a name="p12413525135615"></a>创建编解码对象</p>
</td>
</tr>
<tr id="row11460182372815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p44133252564"><a name="p44133252564"></a><a name="p44133252564"></a>int32_t CodecDestroy(CODEC_HANDLETYPE handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p041392535610"><a name="p041392535610"></a><a name="p041392535610"></a>销毁编解码对象</p>
</td>
</tr>
<tr id="row61462216285"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p84134251566"><a name="p84134251566"></a><a name="p84134251566"></a>int32_t CodecSetPortMode(CODEC_HANDLETYPE handle, DirectionType type, BufferMode mode);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p204131258569"><a name="p204131258569"></a><a name="p204131258569"></a>设置端口buffer模式：内部buffer或外部buffer</p>
</td>
</tr>
<tr id="row791355812713"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10413112575617"><a name="p10413112575617"></a><a name="p10413112575617"></a>int32_t CodecSetParameter(CODEC_HANDLETYPE handle, const Param *params, int paramCnt);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p94138253562"><a name="p94138253562"></a><a name="p94138253562"></a>扩展接口。设置编解码属性</p>
</td>
</tr>
<tr id="row3731175622711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p441318251564"><a name="p441318251564"></a><a name="p441318251564"></a>int32_t CodecGetParameter(CODEC_HANDLETYPE handle, Param *params, int paramCnt);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p74132257566"><a name="p74132257566"></a><a name="p74132257566"></a>扩展接口。获取编解码属性</p>
</td>
</tr>
<tr id="row131131754172718"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14132257567"><a name="p14132257567"></a><a name="p14132257567"></a>int32_t CodecStart(CODEC_HANDLETYPE handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1741382510566"><a name="p1741382510566"></a><a name="p1741382510566"></a>启动工作</p>
</td>
</tr>
<tr id="row133541652202712"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11413125185617"><a name="p11413125185617"></a><a name="p11413125185617"></a>int32_t CodecStop(CODEC_HANDLETYPE handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p154137256566"><a name="p154137256566"></a><a name="p154137256566"></a>停止工作</p>
</td>
</tr>
<tr id="row148624820271"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p124141425125610"><a name="p124141425125610"></a><a name="p124141425125610"></a>int32_t CodecFlush(CODEC_HANDLETYPE handle, DirectionType directType);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p64141258565"><a name="p64141258565"></a><a name="p64141258565"></a>清除缓存处理</p>
</td>
</tr>
<tr id="row1676314450275"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p144141225165615"><a name="p144141225165615"></a><a name="p144141225165615"></a>int32_t CodecQueueInput(CODEC_HANDLETYPE handle, const InputInfo *inputData, uint32_t timeoutMs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6414182535616"><a name="p6414182535616"></a><a name="p6414182535616"></a>送入输入buffer</p>
</td>
</tr>
<tr id="row194281341102710"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15414112510561"><a name="p15414112510561"></a><a name="p15414112510561"></a>int32_t CodecDequeInput(CODEC_HANDLETYPE handle, uint32_t timeoutMs, InputInfo *inputData);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p341492555613"><a name="p341492555613"></a><a name="p341492555613"></a>取出输入buffer</p>
</td>
</tr>
<tr id="row560213915277"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p8414525145610"><a name="p8414525145610"></a><a name="p8414525145610"></a>int32_t CodecQueueOutput(CODEC_HANDLETYPE handle, OutputInfo *outInfo, uint32_t timeoutMs, int releaseFenceFd);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1241442513569"><a name="p1241442513569"></a><a name="p1241442513569"></a>送入输出buffer</p>
</td>
</tr>
<tr id="row933244161410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6414425185619"><a name="p6414425185619"></a><a name="p6414425185619"></a>int32_t CodecDequeueOutput(CODEC_HANDLETYPE handle, uint32_t timeoutMs, int *acquireFd, OutputInfo *outInfo);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p16414112518560"><a name="p16414112518560"></a><a name="p16414112518560"></a>取出输出buffer</p>
</td>
</tr>
<tr id="row182432045121416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p104148252565"><a name="p104148252565"></a><a name="p104148252565"></a>int32_t CodecSetCallback(CODEC_HANDLETYPE handle, const CodecCallback *cb, UINTPTR instance);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p20414172595617"><a name="p20414172595617"></a><a name="p20414172595617"></a>设置回调函数</p>
</td>
</tr>
</tbody>
</table>

除了上述接口外，基于OMX接口提供了Codec HDI 2.0接口，接口说明如下表所示：

**表 2**  Codec HDI 2.0接口列表

<a name="table1513255710560"></a>
<table><thead align="left"><tr id="row171321857155518"><th class="cellrowborder" align="center" valign="top" width="12.821282128212822%" id="mcps1.2.4.2.1"><p id="p6132957115521"><a name="p6132957115521"></a><a name="p6132957115521"></a>头文件</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="63.61636163616362%" id="mcps1.2.4.2.2"><p id="p14132125715562"><a name="p14132125715562"></a><a name="p14132125715562"></a>接口名称</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="23.562356235623565%" id="mcps1.2.4.2.3"><p id="p18132205755526"><a name="p18132205755526"></a><a name="p18132205755526"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1267017500275"><td class="cellrowborder" rowspan="4" align="left" valign="top" width="12.821282128212822%" headers="mcps1.2.4.2.1 "><p id="p15674038923"><a name="p15674038923"></a><a name="p15674038923"></a>codec_component
_manager.h</p>
</td>
<td class="cellrowborder" valign="top" width="63.61636163616362%" headers="mcps1.2.4.2.2 "><p id="p174121125175619"><a name="p174121125175619"></a><a name="p174121125175619"></a>int32_t (*GetComponentNum)();</p>
</td>
<td class="cellrowborder" valign="top" width="23.562356235623565%" headers="mcps1.2.4.2.3 "><p id="p114126258569"><a name="p114126258569"></a><a name="p114126258569"></a>获取Codec编解码组件数量</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p1341212595610"><a name="p1341212595610"></a><a name="p1341212595610"></a>int32_t (*GetComponentCapabilityList)(CodecCompCapability *capList, int32_t count);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1341272515610"><a name="p1341272515610"></a><a name="p1341272515610"></a>获取编解码能力集表</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p14413122555620"><a name="p14413122555620"></a><a name="p14413122555620"></a>int32_t (*CreateComponent)(struct CodecComponentType **component, char *compName, void *appData, int32_t appDataSize, struct CodecCallbackType *callbacks);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1141372575614"><a name="p1141372575614"></a><a name="p1141372575614"></a>创建Codec组件实例</p>
</td>
</tr>
<tr id="row181371630162816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p1941362515561"><a name="p1941362515561"></a><a name="p1941362515561"></a>int32_t (*DestoryComponent)(struct CodecComponentType *component);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p44131025115619"><a name="p44131025115619"></a><a name="p44131025115619"></a>销毁组件实例</p>
</td>
</tr>
</tbody>
<tbody><tr id="row1267017500275"><td class="cellrowborder" rowspan="18" align="left" valign="top" width="12.821282128212822%" headers="mcps1.2.4.2.1 "><p id="p15674038923"><a name="p15674038923"></a><a name="p15674038923"></a>codec_component
_if.h</p>
</td>
<td class="cellrowborder" valign="top" width="63.61636163616362%" headers="mcps1.2.4.2.2 "><p id="p174121125175619"><a name="p174121125175619"></a><a name="p174121125175619"></a>int32_t (*GetComponentVersion)(struct CodecComponentType *self, char *compName, union OMX_VERSIONTYPE *compVersion, union OMX_VERSIONTYPE *specVersion, uint8_t *compUUID, uint32_t compUUIDLen);</p>
</td>
<td class="cellrowborder" valign="top" width="23.562356235623565%" headers="mcps1.2.4.2.3 "><p id="p114126258569"><a name="p114126258569"></a><a name="p114126258569"></a>获取Codec组件版本号</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p1341212595610"><a name="p1341212595610"></a><a name="p1341212595610"></a>int32_t (*SendCommand)(struct CodecComponentType *self, enum OMX_COMMANDTYPE cmd, uint32_t param, int8_t *cmdData, uint32_t cmdDataLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1341272515610"><a name="p1341272515610"></a><a name="p1341272515610"></a>发送命令给组件</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p14413122555620"><a name="p14413122555620"></a><a name="p14413122555620"></a>int32_t (*GetParameter)(struct CodecComponentType *self, uint32_t paramIndex, int8_t *paramStruct, uint32_t paramStructLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1141372575614"><a name="p1141372575614"></a><a name="p1141372575614"></a>获取组件参数设置</p>
</td>
</tr>
<tr id="row181371630162816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p1941362515561"><a name="p1941362515561"></a><a name="p1941362515561"></a>int32_t (*SetParameter)(struct CodecComponentType *self, uint32_t index, int8_t *paramStruct, uint32_t paramStructLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p44131025115619"><a name="p44131025115619"></a><a name="p44131025115619"></a>设置组件需要的参数</p>
</td>
</tr>
<tr id="row01531026142811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p241312251562"><a name="p241312251562"></a><a name="p241312251562"></a>int32_t (*GetConfig)(struct CodecComponentType *self, uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p12413525135615"><a name="p12413525135615"></a><a name="p12413525135615"></a>获取组件的配置结构</p>
</td>
</tr>
<tr id="row11460182372815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p44133252564"><a name="p44133252564"></a><a name="p44133252564"></a>int32_t (*SetConfig)(struct CodecComponentType *self, uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p041392535610"><a name="p041392535610"></a><a name="p041392535610"></a>设置组件的配置</p>
</td>
</tr>
<tr id="row61462216285"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p84134251566"><a name="p84134251566"></a><a name="p84134251566"></a>int32_t (*GetExtensionIndex)(struct CodecComponentType *self, const char *paramName, uint32_t *indexType);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p204131258569"><a name="p204131258569"></a><a name="p204131258569"></a>根据字符串获取组件的扩展索引</p>
</td>
</tr>
<tr id="row791355812713"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p10413112575617"><a name="p10413112575617"></a><a name="p10413112575617"></a>int32_t (*GetState)(struct CodecComponentType *self, enum OMX_STATETYPE *state);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p94138253562"><a name="p94138253562"></a><a name="p94138253562"></a>获取组件的状态</p>
</td>
</tr>
<tr id="row3731175622711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p441318251564"><a name="p441318251564"></a><a name="p441318251564"></a>int32_t (*ComponentTunnelRequest)(struct CodecComponentType *self, uint32_t port, int32_t tunneledComp, uint32_t tunneledPort, struct OMX_TUNNELSETUPTYPE *tunnelSetup);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p74132257566"><a name="p74132257566"></a><a name="p74132257566"></a>设置组件Tunneled方式通信</p>
</td>
</tr>
<tr id="row131131754172718"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p14132257567"><a name="p14132257567"></a><a name="p14132257567"></a>int32_t (*UseBuffer)(struct CodecComponentType *self, uint32_t portIndex, struct OmxCodecBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1741382510566"><a name="p1741382510566"></a><a name="p1741382510566"></a>指定组件端口的buffer</p>
</td>
</tr>
<tr id="row133541652202712"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p11413125185617"><a name="p11413125185617"></a><a name="p11413125185617"></a>int32_t (*AllocateBuffer)(struct CodecComponentType *self, struct OmxCodecBuffer *buffer, uint32_t portIndex);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p154137256566"><a name="p154137256566"></a><a name="p154137256566"></a>向组件申请端口buffer</p>
</td>
</tr>
<tr id="row148624820271"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p124141425125610"><a name="p124141425125610"></a><a name="p124141425125610"></a>int32_t (*FreeBuffer)(struct CodecComponentType *self, uint32_t portIndex, const struct OmxCodecBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p64141258565"><a name="p64141258565"></a><a name="p64141258565"></a>释放buffer</p>
</td>
</tr>
<tr id="row1676314450275"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p144141225165615"><a name="p144141225165615"></a><a name="p144141225165615"></a>int32_t (*EmptyThisBuffer)(struct CodecComponentType *self, const struct OmxCodecBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p6414182535616"><a name="p6414182535616"></a><a name="p6414182535616"></a>编解码输入待处理buffer</p>
</td>
</tr>
<tr id="row194281341102710"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p15414112510561"><a name="p15414112510561"></a><a name="p15414112510561"></a>int32_t (*FillThisBuffer)(struct CodecComponentType *self, const struct OmxCodecBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p341492555613"><a name="p341492555613"></a><a name="p341492555613"></a>编解码输出填充buffer</p>
</td>
</tr>
<tr id="row560213915277"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p8414525145610"><a name="p8414525145610"></a><a name="p8414525145610"></a>int32_t (*SetCallbacks)(struct CodecComponentType *self, struct CodecCallbackType *callback, int8_t *appData, uint32_t appDataLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1241442513569"><a name="p1241442513569"></a><a name="p1241442513569"></a>设置Codec组件的回调函数</p>
</td>
</tr>
<tr id="row933244161410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p6414425185619"><a name="p6414425185619"></a><a name="p6414425185619"></a>int32_t (*ComponentDeInit)(struct CodecComponentType *self);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p16414112518560"><a name="p16414112518560"></a><a name="p16414112518560"></a>组件去初始化</p>
</td>
</tr>
<tr id="row182432045121416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p104148252565"><a name="p104148252565"></a><a name="p104148252565"></a>int32_t (*UseEglImage)(struct CodecComponentType *self, struct OmxCodecBuffer *buffer, uint32_t portIndex, int8_t *eglImage, uint32_t eglImageLen);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p20414172595617"><a name="p20414172595617"></a><a name="p20414172595617"></a>使用已在ELG中申请的空间</p>
</td>
</tr>
<tr id="row182432045121416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p104148252565"><a name="p104148252565"></a><a name="p104148252565"></a>int32_t (*ComponentRoleEnum)(struct CodecComponentType *self, uint8_t *role, uint32_t roleLen, uint32_t index);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p20414172595617"><a name="p20414172595617"></a><a name="p20414172595617"></a>获取组件角色</p>
</td>
</tr>
</tbody>
<tbody><tr id="row1267017500275"><td class="cellrowborder" rowspan="3" align="left" valign="top" width="12.821282128212822%" headers="mcps1.2.4.2.1 "><p id="p15674038923"><a name="p15674038923"></a><a name="p15674038923"></a>codec_callback_if.h</p>
</td>
<td class="cellrowborder" valign="top" width="63.61636163616362%" headers="mcps1.2.4.2.2 "><p id="p174121125175619"><a name="p174121125175619"></a><a name="p174121125175619"></a>int32_t (*EventHandler)(struct CodecCallbackType *self, int8_t *appData, uint32_t appDataLen, enum OMX_EVENTTYPE eEvent, uint32_t data1, uint32_t data2, int8_t *eventData, uint32_t eventDataLen);</p>
</td>
<td class="cellrowborder" valign="top" width="23.562356235623565%" headers="mcps1.2.4.2.3 "><p id="p114126258569"><a name="p114126258569"></a><a name="p114126258569"></a>事件上报</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p1341212595610"><a name="p1341212595610"></a><a name="p1341212595610"></a>int32_t (*EmptyBufferDone)(struct CodecCallbackType *self, int8_t *appData, uint32_t appDataLen, const struct OmxCodecBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1341272515610"><a name="p1341272515610"></a><a name="p1341272515610"></a>上报输入buffer编码或者解码处理完毕</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.2.1 "><p id="p14413122555620"><a name="p14413122555620"></a><a name="p14413122555620"></a>int32_t (*FillBufferDone)(struct CodecCallbackType *self, int8_t* appData, uint32_t appDataLen, struct OmxCodecBuffer* buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.2.2 "><p id="p1141372575614"><a name="p1141372575614"></a><a name="p1141372575614"></a>上报输出buffer填充完毕</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

该仓核心功能包括两个方面：

-   提供Codec HDI接口供framework层调用，实现编码和解码基本功能。
-   作为标准南向接口，保证南向OEM产商实现HDI-adapter的规范性，保证生态良性演进。

具体接口调用及实现，以接口注释为准。

## 相关仓<a name="section1371113476307"></a>

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)

