# Format<a name="EN-US_TOPIC_0000001131858907"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
    -   [Available APIs](#section1551164914237)
    -   [Usage](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository mainly defines and implements the Hardware Driver Interfaces \(HDIs\) of the format module, allowing upper-layer services to perform the following operations for media files:

-   Creating and destroying a muxer and demuxer
-   Starting and stopping a muxer and demuxer
-   Obtaining the attributes of a media file
-   Obtaining the data frame information of a media file
-   Adding data streams and frames to a muxer

## Directory Structure<a name="section161941989596"></a>

The source code directory structure is as follows:

```
/drivers/peripheral/format
├── interfaces         # Driver capability APIs provided for upper-layer services
│   └── include       # APIs exposed externally
```

### Available APIs<a name="section1551164914237"></a>

The format module provides APIs that can be directly called by the framework layer to create or destroy a muxer and demuxer, start or stop a muxer and demuxer, obtain the attributes of a media file, read data frames from a media file, add data streams and frames to a muxer component, and set a callback.

[Table 1](#table1513255710559)  describes major HDI APIs provided by the format module.

**Table  1**  Major HDI APIs of the format module

<a name="table1513255710559"></a>
<table><thead align="left"><tr id="row171321857155517"><th class="cellrowborder" align="center" valign="top" width="12.821282128212822%" id="mcps1.2.4.1.1"><p id="p6132957115511"><a name="p6132957115511"></a><a name="p6132957115511"></a>Header File</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="62.16621662166217%" id="mcps1.2.4.1.2"><p id="p14132125715552"><a name="p14132125715552"></a><a name="p14132125715552"></a>API</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="25.012501250125013%" id="mcps1.2.4.1.3"><p id="p18132205755516"><a name="p18132205755516"></a><a name="p18132205755516"></a>Description</p>
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
<td class="cellrowborder" valign="top" width="25.012501250125013%" headers="mcps1.2.4.1.3 "><p id="p13264114182817"><a name="p13264114182817"></a><a name="p13264114182817"></a>Initializes the format.</p>
</td>
</tr>
<tr id="row2661171172814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p126514172811"><a name="p126514172811"></a><a name="p126514172811"></a>void FormatDeInit(void);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15265164122819"><a name="p15265164122819"></a><a name="p15265164122819"></a>Deinitializes the format.</p>
</td>
</tr>
<tr id="row4385112822818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10265134111284"><a name="p10265134111284"></a><a name="p10265134111284"></a>int32_t FormatDemuxerCreate(const FormatSource *source, FormatHandle * const handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p726554110289"><a name="p726554110289"></a><a name="p726554110289"></a>Creates a demuxer and returns its context handle.</p>
</td>
</tr>
<tr id="row181371630162816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p426517411284"><a name="p426517411284"></a><a name="p426517411284"></a>int32_t FormatDemuxerSetParameter(const FormatHandle handle, int32_t trackId,    const ParameterItem *metaData, int32_t metaDataCnt);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1926512413287"><a name="p1926512413287"></a><a name="p1926512413287"></a>Sets demuxer attributes.</p>
</td>
</tr>
<tr id="row01531026142811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p182651741162812"><a name="p182651741162812"></a><a name="p182651741162812"></a>int32_t FormatDemuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *metaData);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p142651241152813"><a name="p142651241152813"></a><a name="p142651241152813"></a>Obtains demuxer attributes.</p>
</td>
</tr>
<tr id="row11460182372815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1626534118284"><a name="p1626534118284"></a><a name="p1626534118284"></a>int32_t FormatDemuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p72661241112817"><a name="p72661241112817"></a><a name="p72661241112817"></a>Sets a callback for the demuxer.</p>
</td>
</tr>
<tr id="row61462216285"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p526624122817"><a name="p526624122817"></a><a name="p526624122817"></a>int32_t FormatDemuxerSetBufferConfig(const FormatHandle handle, const FormatBufferSetting *setting);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p14266114119283"><a name="p14266114119283"></a><a name="p14266114119283"></a>(Optional) Sets buffer information for the demuxer.</p>
</td>
</tr>
<tr id="row791355812713"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15266144111283"><a name="p15266144111283"></a><a name="p15266144111283"></a>int32_t FormatDemuxerGetBufferConfig(const FormatHandle handle, FormatBufferSetting *setting);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p162661841172812"><a name="p162661841172812"></a><a name="p162661841172812"></a>(Optional) Obtains the buffer information of the demuxer.</p>
</td>
</tr>
<tr id="row3731175622711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1526664162814"><a name="p1526664162814"></a><a name="p1526664162814"></a>int32_t FormatDemuxerPrepare(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6266941142816"><a name="p6266941142816"></a><a name="p6266941142816"></a>Makes preparations for the demuxer.</p>
</td>
</tr>
<tr id="row131131754172718"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p5266104113281"><a name="p5266104113281"></a><a name="p5266104113281"></a>int32_t FormatDemuxerGetFileInfo(const FormatHandle handle, FileInfo *info);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13266341202815"><a name="p13266341202815"></a><a name="p13266341202815"></a>Obtains the attributes of a media file.</p>
</td>
</tr>
<tr id="row133541652202712"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1826619419281"><a name="p1826619419281"></a><a name="p1826619419281"></a>int32_t FormatDemuxerSelectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p182671341102811"><a name="p182671341102811"></a><a name="p182671341102811"></a>Selects a specified media track from which the demuxer reads data frames by calling <strong id="b0679191319363"><a name="b0679191319363"></a><a name="b0679191319363"></a>FormatDemuxerReadFrame</strong>.)</p>
</td>
</tr>
<tr id="row148624820271"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p17267341102812"><a name="p17267341102812"></a><a name="p17267341102812"></a>int32_t FormatDemuxerUnselectTrack(const FormatHandle handle, int32_t programId, int32_t trackId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1267141192812"><a name="p1267141192812"></a><a name="p1267141192812"></a>Unselects a specified media track from which the demuxer will not read data frames when calling <strong id="b183885214111"><a name="b183885214111"></a><a name="b183885214111"></a>FormatDemuxerReadFrame</strong>.</p>
</td>
</tr>
<tr id="row1676314450275"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p02671641122810"><a name="p02671641122810"></a><a name="p02671641122810"></a>int32_t FormatDemuxerStart(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p3267104112289"><a name="p3267104112289"></a><a name="p3267104112289"></a>Starts the demuxer. After being started, the caller can read data frames from the demuxer.</p>
</td>
</tr>
<tr id="row194281341102710"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1526719418280"><a name="p1526719418280"></a><a name="p1526719418280"></a>int32_t FormatDemuxerGetSelectedTrack(const FormatHandle handle, int32_t *programId, int32_t trackId[], int32_t *nums);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p142671441122820"><a name="p142671441122820"></a><a name="p142671441122820"></a>Obtains the ID of the media track selected by the demuxer for output.</p>
</td>
</tr>
<tr id="row560213915277"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1268641112812"><a name="p1268641112812"></a><a name="p1268641112812"></a>int32_t FormatDemuxerReadFrame(const FormatHandle handle, FormatFrame *frame, int32_t timeOutMs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p18268174192819"><a name="p18268174192819"></a><a name="p18268174192819"></a>Reads data frames.</p>
</td>
</tr>
<tr id="row933244161410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15483141102218"><a name="p15483141102218"></a><a name="p15483141102218"></a>int32_t FormatDemuxerFreeFrame(const FormatHandle handle, FormatFrame *frame);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15484194152211"><a name="p15484194152211"></a><a name="p15484194152211"></a>Frees data frames.</p>
</td>
</tr>
<tr id="row182432045121416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p0484241102215"><a name="p0484241102215"></a><a name="p0484241102215"></a>int32_t FormatDemuxerSeek(const FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p10484641162215"><a name="p10484641162215"></a><a name="p10484641162215"></a>Seeks for a specified position for the demuxer.</p>
</td>
</tr>
<tr id="row122643187161"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14842418228"><a name="p14842418228"></a><a name="p14842418228"></a>int32_t FormatDemuxerStop(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9484124132212"><a name="p9484124132212"></a><a name="p9484124132212"></a>Stops the demuxer from working.</p>
</td>
</tr>
<tr id="row1168154414167"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p948418417222"><a name="p948418417222"></a><a name="p948418417222"></a>int32_t FormatDemuxerDestory(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6484174172215"><a name="p6484174172215"></a><a name="p6484174172215"></a>Destroys demuxer resources.</p>
</td>
</tr>
<tr id="row565634691619"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p124840414221"><a name="p124840414221"></a><a name="p124840414221"></a>int32_t FormatMuxerCreate(FormatHandle * const handle, const FormatOutputConfig *outputConfig);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p184841741192215"><a name="p184841741192215"></a><a name="p184841741192215"></a>Creates a muxer and returns its context handle.</p>
</td>
</tr>
<tr id="row1586205081614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1348474117228"><a name="p1348474117228"></a><a name="p1348474117228"></a>int32_t FormatMuxerDestory(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p848464120229"><a name="p848464120229"></a><a name="p848464120229"></a>Destroys a muxer and releases its resources created by calling <strong id="b454274814618"><a name="b454274814618"></a><a name="b454274814618"></a>FormatMuxerCreate</strong>.</p>
</td>
</tr>
<tr id="row1815361272220"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1948574113229"><a name="p1948574113229"></a><a name="p1948574113229"></a>int32_t FormatMuxerAddTrack(const FormatHandle handle, const TrackSource *trackSource);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1248524162220"><a name="p1248524162220"></a><a name="p1248524162220"></a>Adds a media track source for the muxer.</p>
</td>
</tr>
<tr id="row14720016182214"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12485184152211"><a name="p12485184152211"></a><a name="p12485184152211"></a>int32_t FormatMuxerSetCallBack(const FormatHandle handle, const FormatCallback *callBack);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p5485114114229"><a name="p5485114114229"></a><a name="p5485114114229"></a>Sets a callback for the muxer.</p>
</td>
</tr>
<tr id="row1466424191620"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p164854415224"><a name="p164854415224"></a><a name="p164854415224"></a>int32_t FormatMuxerSetOrientation(const FormatHandle handle, int degrees);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p948534113222"><a name="p948534113222"></a><a name="p948534113222"></a>Sets the orientation of the video track for the muxer.</p>
</td>
</tr>
<tr id="row99291794226"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10485154119227"><a name="p10485154119227"></a><a name="p10485154119227"></a>int32_t FormatMuxerSetLocation(const FormatHandle handle, int latitude, int longitude);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p144851241102210"><a name="p144851241102210"></a><a name="p144851241102210"></a>Sets the geographical information for the output file of the muxer.</p>
</td>
</tr>
<tr id="row21561639131614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p24851941172211"><a name="p24851941172211"></a><a name="p24851941172211"></a>int32_t FormatMuxerSetMaxFileSize(const FormatHandle handle, int64_t bytes);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p12485124132219"><a name="p12485124132219"></a><a name="p12485124132219"></a>Sets the maximum size (in bytes) for the output file of the muxer.</p>
</td>
</tr>
<tr id="row1024914496149"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p9485164152218"><a name="p9485164152218"></a><a name="p9485164152218"></a>int32_t FormatMuxerSetMaxFileDuration(const FormatHandle handle, int64_t durationUs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1548519411228"><a name="p1548519411228"></a><a name="p1548519411228"></a>Sets the maximum duration (in seconds) for the output file.</p>
</td>
</tr>
<tr id="row5647191181614"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p948515417222"><a name="p948515417222"></a><a name="p948515417222"></a>int32_t FormatMuxerSetFileSplitDuration(const FormatHandle handle, ManualSplitType type,                                          int64_t timestampUs, uint32_t durationUs);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9486441142216"><a name="p9486441142216"></a><a name="p9486441142216"></a>Manually splits a file.</p>
</td>
</tr>
<tr id="row96242146164"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1448614116229"><a name="p1448614116229"></a><a name="p1448614116229"></a>int32_t FormatMuxerStart(const FormatHandle handle);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p448624162217"><a name="p448624162217"></a><a name="p448624162217"></a>Starts the muxer.</p>
</td>
</tr>
<tr id="row4719558166"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p448664112224"><a name="p448664112224"></a><a name="p448664112224"></a>int32_t FormatMuxerWriteFrame(const FormatHandle handle, const FormatFrame *frameData);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p114869411222"><a name="p114869411222"></a><a name="p114869411222"></a>Writes data frames into the muxer.</p>
</td>
</tr>
<tr id="row18785159171611"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p148624112222"><a name="p148624112222"></a><a name="p148624112222"></a>int32_t FormatMuxerSetNextOutputFile(const FormatHandle handle, int32_t fd);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p6486164113221"><a name="p6486164113221"></a><a name="p6486164113221"></a>Sets the descriptor for the next output file.</p>
</td>
</tr>
<tr id="row119401052191419"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14486204116224"><a name="p14486204116224"></a><a name="p14486204116224"></a>int32_t FormatMuxerStop(const FormatHandle handle, bool block);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p648612417226"><a name="p648612417226"></a><a name="p648612417226"></a>Stops the muxer that was started by calling <strong id="b934812298562"><a name="b934812298562"></a><a name="b934812298562"></a>FormatMuxerStart</strong>.</p>
</td>
</tr>
<tr id="row1244610316169"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p14861341192214"><a name="p14861341192214"></a><a name="p14861341192214"></a>int32_t FormatMuxerSetParameter(const FormatHandle handle, int32_t trackId, const ParameterItem *item, int32_t itemNum);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1486134102210"><a name="p1486134102210"></a><a name="p1486134102210"></a>Sets muxer attributes.</p>
</td>
</tr>
<tr id="row17305165551410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1448654102214"><a name="p1448654102214"></a><a name="p1448654102214"></a>int32_t FormatMuxerGetParameter(const FormatHandle handle, int32_t trackId, ParameterItem *item, int32_t itemNum);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p348617419229"><a name="p348617419229"></a><a name="p348617419229"></a>Obtains muxer attributes.</p>
</td>
</tr>
</tbody>
</table>

### Usage<a name="section129654513264"></a>

The core functionalities of this repository are as follows:

-   Provides format HDIs that can be directly called by the framework layer to perform muxer and demuxer-related operations.
-   Provides standard interfaces for device developers to ensure that the OEM vendors comply with the HDI adapter standard. This promises a healthy evolution of the ecosystem.

For details about the invocation and implementation, see the API reference.

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)

