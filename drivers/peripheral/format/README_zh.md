# Format<a name="ZH-CN_TOPIC_0000001131858907"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

该仓下主要包含Format模块HDI（Hardware Driver Interface）接口定义及其实现，对上层提供媒体文件复用和解复用的驱动能力接口，HDI接口主要提供如下功能：

-   创建、销毁复用和解复用器对象；
-   启停复用和解复用器
-   获取片源属性；
-   获取片源流数据帧信息
-   添加流和数据帧到复用器。

## 目录<a name="section161941989596"></a>

该仓下源代码目录结构如下所示

```
/drivers/peripheral/format
├── interfaces         # format模块对上层服务提供的驱动能力接口
│   └── include       # format模块对外提供的接口定义
```

### 接口说明<a name="section1551164914237"></a>

Format驱动提供给framework层可直接调用的能力接口，主要功能有：创建、销毁复用器和解复用器对象，启停复用和解复用操作、获取片源属性、从片源读取数据帧处理、添加流和数据帧到复用器、事件上报等

提供的部分接口说明如[表1 Format HDI接口列表](#table1513255710559)所示：

**表 1**  Format HDI接口列表

<a name="table1513255710559"></a>
<table><thead align="left"><tr id="row171321857155517"><th class="cellrowborder" align="center" valign="top" width="12.821282128212822%" id="mcps1.2.4.1.1"><p id="p6132957115511"><a name="p6132957115511"></a><a name="p6132957115511"></a>头文件</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="62.16621662166217%" id="mcps1.2.4.1.2"><p id="p14132125715552"><a name="p14132125715552"></a><a name="p14132125715552"></a>接口名称</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="25.012501250125013%" id="mcps1.2.4.1.3"><p id="p18132205755516"><a name="p18132205755516"></a><a name="p18132205755516"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1267017500274"><td class="cellrowborder" rowspan="34" align="left" valign="top" width="12.821282128212822%" headers="mcps1.2.4.1.1 "><p id="p15674038913"><a name="p15674038913"></a><a name="p15674038913"></a></p>
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
<p id="p825185015460"><a name="p825185015460"></a><a name="p825185015460"></a>format_interface.h</p>
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
<td class="cellrowborder" valign="top" width="62.16621662166217%" headers="mcps1.2.4.1.2 "><p id="p6264341172811"><a name="p6264341172811"></a><a name="p6264341172811"></a>void FormatInit(void);</p>
</td>
<td class="cellrowborder" valign="top" width="25.012501250125013%" headers="mcps1.2.4.1.3 "><p id="p13264114182817"><a name="p13264114182817"></a><a name="p13264114182817"></a>模块初始化处理</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p126514172811"><a name="p126514172811"></a><a name="p126514172811"></a>void FormatDeInit(void);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15265164122819"><a name="p15265164122819"></a><a name="p15265164122819"></a>模块去初始化处理</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10265134111284"><a name="p10265134111284"></a><a name="p10265134111284"></a>int32_t FormatDemuxerCreate(const FormatSource *source, FormatHandle * const handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p726554110289"><a name="p726554110289"></a><a name="p726554110289"></a>创建解复用器对象</p>
</td>
</tr>
<tr id="row181371630162816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p426517411284"><a name="p426517411284"></a><a name="p426517411284"></a>int32_t FormatDemuxerSetParameter(const FormatHandle handle, int32_t trackId,    const ParameterItem *metaData, int32_t metaDataCnt);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1926512413287"><a name="p1926512413287"></a><a name="p1926512413287"></a>扩展接口，设置解复用器属性信息</p>
</td>
</tr>
<tr id="row01531026142811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p182651741162812"><a name="p182651741162812"></a><a name="p182651741162812"></a>int32_t FormatDemuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *metaData);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p142651241152813"><a name="p142651241152813"></a><a name="p142651241152813"></a>扩展接口，获取解复用器属性信息</p>
</td>
</tr>
<tr id="row11460182372815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1626534118284"><a name="p1626534118284"></a><a name="p1626534118284"></a>int32_t FormatDemuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p72661241112817"><a name="p72661241112817"></a><a name="p72661241112817"></a>设置回调函数接口，用于事件上报</p>
</td>
</tr>
<tr id="row61462216285"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p526624122817"><a name="p526624122817"></a><a name="p526624122817"></a>int32_t FormatDemuxerSetBufferConfig(const FormatHandle handle, const FormatBufferSetting *setting);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p14266114119283"><a name="p14266114119283"></a><a name="p14266114119283"></a>设置解复用器数据帧缓存大小信息（可选）</p>
</td>
</tr>
<tr id="row791355812713"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15266144111283"><a name="p15266144111283"></a><a name="p15266144111283"></a>int32_t FormatDemuxerGetBufferConfig(const FormatHandle handle, FormatBufferSetting *setting);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p162661841172812"><a name="p162661841172812"></a><a name="p162661841172812"></a>获取解复用器数据帧缓存大小信息（可选）</p>
</td>
</tr>
<tr id="row3731175622711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1526664162814"><a name="p1526664162814"></a><a name="p1526664162814"></a>int32_t FormatDemuxerPrepare(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6266941142816"><a name="p6266941142816"></a><a name="p6266941142816"></a>准备工作处理，完成片源属性探测处理</p>
</td>
</tr>
<tr id="row131131754172718"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p5266104113281"><a name="p5266104113281"></a><a name="p5266104113281"></a>int32_t FormatDemuxerGetFileInfo(const FormatHandle handle, FileInfo *info);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13266341202815"><a name="p13266341202815"></a><a name="p13266341202815"></a>获取片源属性。</p>
</td>
</tr>
<tr id="row133541652202712"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1826619419281"><a name="p1826619419281"></a><a name="p1826619419281"></a>int32_t FormatDemuxerSelectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p182671341102811"><a name="p182671341102811"></a><a name="p182671341102811"></a>选择某个轨道。（通过FormatDemuxerReadFrame接口可以读取到对应轨道数据帧）</p>
</td>
</tr>
<tr id="row148624820271"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p17267341102812"><a name="p17267341102812"></a><a name="p17267341102812"></a>int32_t FormatDemuxerUnselectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1267141192812"><a name="p1267141192812"></a><a name="p1267141192812"></a>不选择某个轨道。（通过FormatDemuxerReadFrame接口不会读取到对应轨道数据帧）</p>
</td>
</tr>
<tr id="row1676314450275"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p02671641122810"><a name="p02671641122810"></a><a name="p02671641122810"></a>int32_t FormatDemuxerStart(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p3267104112289"><a name="p3267104112289"></a><a name="p3267104112289"></a>启动工作。调用该接口后可读取到数据帧处理</p>
</td>
</tr>
<tr id="row194281341102710"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1526719418280"><a name="p1526719418280"></a><a name="p1526719418280"></a>int32_t FormatDemuxerGetSelectedTrack(const FormatHandle handle, int32_t *programId, int32_t trackId[], int32_t *nums);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p142671441122820"><a name="p142671441122820"></a><a name="p142671441122820"></a>获取当前已选择的轨道</p>
</td>
</tr>
<tr id="row560213915277"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1268641112812"><a name="p1268641112812"></a><a name="p1268641112812"></a>int32_t FormatDemuxerReadFrame(const FormatHandle handle, FormatFrame *frame, int32_t timeOutMs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p18268174192819"><a name="p18268174192819"></a><a name="p18268174192819"></a>读取数据帧</p>
</td>
</tr>
<tr id="row933244161410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15483141102218"><a name="p15483141102218"></a><a name="p15483141102218"></a>int32_t FormatDemuxerFreeFrame(const FormatHandle handle, FormatFrame *frame);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15484194152211"><a name="p15484194152211"></a><a name="p15484194152211"></a>释放数据帧</p>
</td>
</tr>
<tr id="row182432045121416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p0484241102215"><a name="p0484241102215"></a><a name="p0484241102215"></a>int32_t FormatDemuxerSeek(const FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p10484641162215"><a name="p10484641162215"></a><a name="p10484641162215"></a>拖动到指定位置附近</p>
</td>
</tr>
<tr id="row122643187161"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14842418228"><a name="p14842418228"></a><a name="p14842418228"></a>int32_t FormatDemuxerStop(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9484124132212"><a name="p9484124132212"></a><a name="p9484124132212"></a>解复用器停止工作</p>
</td>
</tr>
<tr id="row1168154414167"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p948418417222"><a name="p948418417222"></a><a name="p948418417222"></a>int32_t FormatDemuxerDestory(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6484174172215"><a name="p6484174172215"></a><a name="p6484174172215"></a>销毁解复用器对象</p>
</td>
</tr>
<tr id="row565634691619"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p124840414221"><a name="p124840414221"></a><a name="p124840414221"></a>int32_t FormatMuxerCreate(FormatHandle * const handle, const FormatOutputConfig *outputConfig);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p184841741192215"><a name="p184841741192215"></a><a name="p184841741192215"></a>创建复用器对象</p>
</td>
</tr>
<tr id="row1586205081614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1348474117228"><a name="p1348474117228"></a><a name="p1348474117228"></a>int32_t FormatMuxerDestory(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p848464120229"><a name="p848464120229"></a><a name="p848464120229"></a>销毁复用器对象</p>
</td>
</tr>
<tr id="row1815361272220"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1948574113229"><a name="p1948574113229"></a><a name="p1948574113229"></a>int32_t FormatMuxerAddTrack(const FormatHandle handle, const TrackSource *trackSource);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1248524162220"><a name="p1248524162220"></a><a name="p1248524162220"></a>添加复用器流</p>
</td>
</tr>
<tr id="row14720016182214"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12485184152211"><a name="p12485184152211"></a><a name="p12485184152211"></a>int32_t FormatMuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p5485114114229"><a name="p5485114114229"></a><a name="p5485114114229"></a>设置复用器回调函数</p>
</td>
</tr>
<tr id="row1466424191620"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p164854415224"><a name="p164854415224"></a><a name="p164854415224"></a>int32_t FormatMuxerSetOrientation(const FormatHandle handle, int degrees);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p948534113222"><a name="p948534113222"></a><a name="p948534113222"></a>设置视频角度信息</p>
</td>
</tr>
<tr id="row99291794226"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10485154119227"><a name="p10485154119227"></a><a name="p10485154119227"></a>int32_t FormatMuxerSetLocation(const FormatHandle handle, int latitude, int longitude);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p144851241102210"><a name="p144851241102210"></a><a name="p144851241102210"></a>设置地理位置信息</p>
</td>
</tr>
<tr id="row21561639131614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p24851941172211"><a name="p24851941172211"></a><a name="p24851941172211"></a>int32_t FormatMuxerSetMaxFileSize(const FormatHandle handle, int64_t bytes);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p12485124132219"><a name="p12485124132219"></a><a name="p12485124132219"></a>设置复用器生成文件最大SIZE</p>
</td>
</tr>
<tr id="row1024914496149"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p9485164152218"><a name="p9485164152218"></a><a name="p9485164152218"></a>int32_t FormatMuxerSetMaxFileDuration(const FormatHandle handle, int64_t durationUs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1548519411228"><a name="p1548519411228"></a><a name="p1548519411228"></a>设置复用器生成文件最大时长</p>
</td>
</tr>
<tr id="row5647191181614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p948515417222"><a name="p948515417222"></a><a name="p948515417222"></a>int32_t FormatMuxerSetFileSplitDuration(const FormatHandle handle, ManualSplitType type,                                          int64_t timestampUs, uint32_t durationUs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9486441142216"><a name="p9486441142216"></a><a name="p9486441142216"></a>设置手动切分片信息</p>
</td>
</tr>
<tr id="row96242146164"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1448614116229"><a name="p1448614116229"></a><a name="p1448614116229"></a>int32_t FormatMuxerStart(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p448624162217"><a name="p448624162217"></a><a name="p448624162217"></a>复用器启动工作</p>
</td>
</tr>
<tr id="row4719558166"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p448664112224"><a name="p448664112224"></a><a name="p448664112224"></a>int32_t FormatMuxerWriteFrame(const FormatHandle handle, const FormatFrame *frameData);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p114869411222"><a name="p114869411222"></a><a name="p114869411222"></a>写入数据帧</p>
</td>
</tr>
<tr id="row18785159171611"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p148624112222"><a name="p148624112222"></a><a name="p148624112222"></a>int32_t FormatMuxerSetNextOutputFile(const FormatHandle handle, int32_t fd);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6486164113221"><a name="p6486164113221"></a><a name="p6486164113221"></a>设置下一个输出文件句柄fd</p>
</td>
</tr>
<tr id="row119401052191419"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14486204116224"><a name="p14486204116224"></a><a name="p14486204116224"></a>int32_t FormatMuxerStop(const FormatHandle handle, bool block);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p648612417226"><a name="p648612417226"></a><a name="p648612417226"></a>复用器停止工作</p>
</td>
</tr>
<tr id="row1244610316169"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14861341192214"><a name="p14861341192214"></a><a name="p14861341192214"></a>int32_t FormatMuxerSetParameter(const FormatHandle handle, int32_t trackId, const ParameterItem *item, int32_t itemNum);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1486134102210"><a name="p1486134102210"></a><a name="p1486134102210"></a>扩展接口，设置复用器属性</p>
</td>
</tr>
<tr id="row17305165551410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1448654102214"><a name="p1448654102214"></a><a name="p1448654102214"></a>int32_t FormatMuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *item, int32_t itemNum);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p348617419229"><a name="p348617419229"></a><a name="p348617419229"></a>扩展接口，获取复用器属性</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

该仓核心功能包括两个方面：

-   提供Format HDI接口供framework层调用，实现片源复用与解复用功能。
-   作为标准南向接口，保证南向OEM产商实现HDI-adapter的规范性，保证生态良性演进。

具体接口调用及实现，以接口注释为准。

## 相关仓<a name="section1371113476307"></a>

[驱动子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E9%A9%B1%E5%8A%A8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README_zh.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README_zh.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README_zh.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)

