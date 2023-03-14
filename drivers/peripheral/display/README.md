# Display<a name="EN-US_TOPIC_0000001124650033"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
    -   [Available APIs](#section1551164914237)
    -   [Usage](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository defines and implements the following Hardware Driver Interfaces \(HDIs\) of the display module. The HDIs are classified into the following types:

-   DisplayLayer: manages display layers, including creating layers, setting display attributes, and filling and flushing layers.
-   DisplayGralloc: manages memory of the display module, including memory application, release, and mapping.
-   DisplayGfx: implements hardware acceleration, including the initialization of the hardware acceleration module, bit blit, and regular graphics drawing and filling.

**Figure  1**  HDI architecture of the display module<a name="fig7690246193216"></a>  
![](figures/hdi-architecture-of-the-display-module.png "hdi-architecture-of-the-display-module")

## Directory Structure<a name="section161941989596"></a>

The source code directory structure is as follows:

```
/drivers/peripheral/display
├── hal                # HAL code
│   └── default       # Default HDI implementation
├── interfaces         # Driver capability APIs provided for upper-layer services
│   └── include       # APIs exposed externally
├── test               # Test code
│   └── unittest      # Unit test code
```

### Available APIs<a name="section1551164914237"></a>

The display driver provides the Window Manager Service \(WMS\) with driver capability APIs that can be classified into the  **DisplayLayer**,  **DisplayGralloc**, and  **DisplayGfx**  types. By calling these APIs, you can create or destroy a layer, set layer attributes, allocate, release, map, and buffer memory, fill in rectangles, and blit bitmaps.

[Table 1](#table1513255710559)  describes major HDIs provided by the display module.

**Table  1**  Major HDIs of the display module

<a name="table1513255710559"></a>
<table><thead align="left"><tr id="row171321857155517"><th class="cellrowborder" align="center" valign="top" width="12.121212121212123%" id="mcps1.2.4.1.1"><p id="p6132957115511"><a name="p6132957115511"></a><a name="p6132957115511"></a>Header File</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="64.95649564956496%" id="mcps1.2.4.1.2"><p id="p14132125715552"><a name="p14132125715552"></a><a name="p14132125715552"></a>API</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="22.922292229222922%" id="mcps1.2.4.1.3"><p id="p18132205755516"><a name="p18132205755516"></a><a name="p18132205755516"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row13132357165514"><td class="cellrowborder" rowspan="9" valign="top" width="12.121212121212123%" headers="mcps1.2.4.1.1 "><p id="p829618389386"><a name="p829618389386"></a><a name="p829618389386"></a></p>
<p id="p35261387384"><a name="p35261387384"></a><a name="p35261387384"></a></p>
<p id="p776383812388"><a name="p776383812388"></a><a name="p776383812388"></a></p>
<p id="p11950123812382"><a name="p11950123812382"></a><a name="p11950123812382"></a></p>
<p id="p13168103915381"><a name="p13168103915381"></a><a name="p13168103915381"></a></p>
<p id="p825185015460"><a name="p825185015460"></a><a name="p825185015460"></a>display_layer.h</p>
<p id="p2133757135510"><a name="p2133757135510"></a><a name="p2133757135510"></a></p>
<p id="p1476175815372"><a name="p1476175815372"></a><a name="p1476175815372"></a></p>
</td>
<td class="cellrowborder" valign="top" width="64.95649564956496%" headers="mcps1.2.4.1.2 "><p id="p1365411515117"><a name="p1365411515117"></a><a name="p1365411515117"></a>int32_t (*InitDisplay)(uint32_t devId);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.3 "><p id="p041814588404"><a name="p041814588404"></a><a name="p041814588404"></a>Initializes the layer.</p>
</td>
</tr>
<tr id="row9132135715515"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1398804043612"><a name="p1398804043612"></a><a name="p1398804043612"></a>int32_t (*DeinitDisplay)(uint32_t devId);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1341845816404"><a name="p1341845816404"></a><a name="p1341845816404"></a>Deinitializes the layer.</p>
</td>
</tr>
<tr id="row171330575555"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1974125024812"><a name="p1974125024812"></a><a name="p1974125024812"></a>int32_t (*GetDisplayInfo)(uint32_t devId, DisplayInfo *dispInfo);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1041815816403"><a name="p1041815816403"></a><a name="p1041815816403"></a>Obtains the display device information.</p>
</td>
</tr>
<tr id="row576145883720"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p8761658183714"><a name="p8761658183714"></a><a name="p8761658183714"></a>int32_t (*OpenLayer)(uint32_t devId, const LayerInfo *layerInfo, uint32_t *layerId);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p15418165812409"><a name="p15418165812409"></a><a name="p15418165812409"></a>Creates a layer.</p>
</td>
</tr>
<tr id="row1957862120383"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p55781217381"><a name="p55781217381"></a><a name="p55781217381"></a>int32_t (*CloseLayer)(uint32_t devId, uint32_t layerId);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1641875834010"><a name="p1641875834010"></a><a name="p1641875834010"></a>Destroys a layer.</p>
</td>
</tr>
<tr id="row127635162380"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p3763916143816"><a name="p3763916143816"></a><a name="p3763916143816"></a>int32_t (*SetLayerVisible)(uint32_t devId, uint32_t layerId, bool visible);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p10418195874019"><a name="p10418195874019"></a><a name="p10418195874019"></a>Sets whether a layer is visible.</p>
</td>
</tr>
<tr id="row14230131383819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1723081310386"><a name="p1723081310386"></a><a name="p1723081310386"></a>int32_t (*SetLayerDirtyRegion)(uint32_t devId, uint32_t layerId, IRect *region);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p10418658184012"><a name="p10418658184012"></a><a name="p10418658184012"></a>Sets the flushing area for a layer.</p>
</td>
</tr>
<tr id="row159636983816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p89635916383"><a name="p89635916383"></a><a name="p89635916383"></a>int32_t (*Flush)(uint32_t devId, uint32_t layerId, LayerBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p19418205894012"><a name="p19418205894012"></a><a name="p19418205894012"></a>Flushes a layer.</p>
</td>
</tr>
<tr id="row76872047153720"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1368818472376"><a name="p1368818472376"></a><a name="p1368818472376"></a>int32_t (*GetLayerBuffer)(uint32_t devId, uint32_t layerId, LayerBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p144182582405"><a name="p144182582405"></a><a name="p144182582405"></a>Obtains the buffer of a layer.</p>
</td>
</tr>
<tr id="row1513316577554"><td class="cellrowborder" rowspan="7" valign="top" width="12.121212121212123%" headers="mcps1.2.4.1.1 "><p id="p14171441118"><a name="p14171441118"></a><a name="p14171441118"></a></p>
<p id="p154814318410"><a name="p154814318410"></a><a name="p154814318410"></a></p>
<p id="p3481154311418"><a name="p3481154311418"></a><a name="p3481154311418"></a></p>
<p id="p57063567463"><a name="p57063567463"></a><a name="p57063567463"></a>display_gralloc.h</p>
<p id="p7909447418"><a name="p7909447418"></a><a name="p7909447418"></a></p>
</td>
<td class="cellrowborder" valign="top" width="64.95649564956496%" headers="mcps1.2.4.1.2 "><p id="p228510326414"><a name="p228510326414"></a><a name="p228510326414"></a>int32_t AllocMem(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.3 "><p id="p17421321134612"><a name="p17421321134612"></a><a name="p17421321134612"></a>Allocates memory.</p>
</td>
</tr>
<tr id="row171331657185514"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12841932114117"><a name="p12841932114117"></a><a name="p12841932114117"></a>void FreeMem(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1874202174615"><a name="p1874202174615"></a><a name="p1874202174615"></a>Releases memory.</p>
</td>
</tr>
<tr id="row41331557165518"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p92831132184119"><a name="p92831132184119"></a><a name="p92831132184119"></a>void *Mmap(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p474262184610"><a name="p474262184610"></a><a name="p474262184610"></a>Maps memory.</p>
</td>
</tr>
<tr id="row77021769584"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p8283123284110"><a name="p8283123284110"></a><a name="p8283123284110"></a>void *MmapCache(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p107422021204615"><a name="p107422021204615"></a><a name="p107422021204615"></a>Maps memory to memory with cache.</p>
</td>
</tr>
<tr id="row71857914585"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4282032114118"><a name="p4282032114118"></a><a name="p4282032114118"></a>int32_t Unmap(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p374210219468"><a name="p374210219468"></a><a name="p374210219468"></a>Unmaps memory.</p>
</td>
</tr>
<tr id="row884115357415"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p68421035114115"><a name="p68421035114115"></a><a name="p68421035114115"></a>int32_t FlushCache(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1674212113467"><a name="p1674212113467"></a><a name="p1674212113467"></a>Flushes data from the cache to memory and invalidates the data in the cache.</p>
</td>
</tr>
<tr id="row18831119115815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p172641732134117"><a name="p172641732134117"></a><a name="p172641732134117"></a>int32_t FlushMCache(GrallocBuffer *buffer);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p10742182114462"><a name="p10742182114462"></a><a name="p10742182114462"></a>Flushes data from the cache via Mmap to memory and invalidates the data in the cache.</p>
</td>
</tr>
<tr id="row1452521025813"><td class="cellrowborder" rowspan="4" valign="top" width="12.121212121212123%" headers="mcps1.2.4.1.1 "><p id="p033128174618"><a name="p033128174618"></a><a name="p033128174618"></a></p>
<p id="p4252162854616"><a name="p4252162854616"></a><a name="p4252162854616"></a></p>
<p id="p10421192894615"><a name="p10421192894615"></a><a name="p10421192894615"></a></p>
<p id="p12525910165811"><a name="p12525910165811"></a><a name="p12525910165811"></a>display_gfx.h</p>
</td>
<td class="cellrowborder" valign="top" width="64.95649564956496%" headers="mcps1.2.4.1.2 "><p id="p16761419154811"><a name="p16761419154811"></a><a name="p16761419154811"></a>int32_t InitGfx(void);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.3 "><p id="p1675964994818"><a name="p1675964994818"></a><a name="p1675964994818"></a>Initializes hardware acceleration.</p>
</td>
</tr>
<tr id="row172902161193"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1249042564815"><a name="p1249042564815"></a><a name="p1249042564815"></a>int32_t DeinitGfx(void);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p17591149104819"><a name="p17591149104819"></a><a name="p17591149104819"></a>Deinitializes hardware acceleration.</p>
</td>
</tr>
<tr id="row1948179195"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p5783143154816"><a name="p5783143154816"></a><a name="p5783143154816"></a>int32_t Blit(ISurface *srcSurface, IRect *srcRect, ISurface *dstSurface, IRect *dstRect);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1759749134818"><a name="p1759749134818"></a><a name="p1759749134818"></a>Blits bitmaps.</p>
</td>
</tr>
<tr id="row1331121813197"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2728173711481"><a name="p2728173711481"></a><a name="p2728173711481"></a>int32_t FillRect(ISurface *surface, IRect *rect, uint32_t color, GfxOpt *opt);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p107591749104810"><a name="p107591749104810"></a><a name="p107591749104810"></a>Fills a rectangle with a given color on the canvas.</p>
</td>
</tr>
</tbody>
</table>

### Usage<a name="section129654513264"></a>

The core function of this repository is to provide HDIs for upper-layer display system services to implement display driver capabilities.

The following sample code describes how to use the display HDIs:

```
#include "display_gfx.h"
#include "display_gralloc.h"
#include "display_layer.h"
#include "display_type.h"

#define DEVID 0
DisplayTest g_displayTest;
static GrallocBuffer g_buffer;

static int32_t GetDisplayInterfaces(void)
{
    int32_t ret;
    ret = LayerInitialize(&g_displayTest.layerFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.layerFuncs == NULL) {
        HDF_LOGE("initialize layer failed");
        return DISPLAY_FAILURE;
    }
    ret = GrallocInitialize(&g_displayTest.grallocFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.layerFuncs == NULL) {
        HDF_LOGE("initialize gralloc failed");
        return DISPLAY_FAILURE;
    }
    ret = GfxInitialize(&g_displayTest.gfxFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.gfxFuncs == NULL) {
        HDF_LOGE("initialize gralloc failed");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t DisplayUninit(void)
{
    LayerUninitialize(g_displayTest.layerFuncs);
    GrallocUninitialize(g_displayTest.grallocFuncs);
    GfxUninitialize(g_displayTest.gfxFuncs);
    return DISPLAY_SUCCESS;
}

static void GetLayerInfo(LayerInfo *layInfo)
{
    layInfo->width = g_displayTest.displayInfo.width;
    layInfo->height = g_displayTest.displayInfo.height;
    layInfo->bpp = LAYER_BPP;
    layInfo->pixFormat = PIXEL_FMT_RGBA_5551;
    layInfo->type = LAYER_TYPE_GRAPHIC;
}

static void WriteDataToBuf(int32_t width, int32_t height, uint16_t *pBuf)
{
    int32_t x;
    int32_t y;

    for (y = ((height / LINE_WIDTH) - LINE_WIDTH); y < ((height / LINE_WIDTH) + LINE_WIDTH); y++) {
        for (x = 0; x < width; x++) {
            *((uint16_t*)pBuf + y * width + x) = HIFB_RED_1555;
        }
    }
    for (y = 0; y < height; y++) {
        for (x = ((width / LINE_WIDTH) - LINE_WIDTH); x < ((width / LINE_WIDTH) + LINE_WIDTH); x++) {
            *((uint16_t*)pBuf + y * width + x) = HIFB_RED_1555;
        }
    }
}

int DisplayServiceSample(void)
{    
    int32_t ret;
    g_displayTest.devId = DEVID;

    /* Obtain display driver APIs. */
    ret = GetDisplayInterfaces();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("get display interfaces ops failed");
        return ret;
    }
    /* Initialize a display device. */
    if (g_displayTest.layerFuncs->InitDisplay != NULL) {
        ret = g_displayTest.layerFuncs->InitDisplay(g_displayTest.devId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("initialize display failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Obtain the display device information. */
    if (g_displayTest.layerFuncs->GetDisplayInfo != NULL) {
        ret = g_displayTest.layerFuncs->GetDisplayInfo(g_displayTest.devId, &g_displayTest.displayInfo);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("get disp info failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Open a specific layer of the display device. */
    if (g_displayTest.layerFuncs->OpenLayer != NULL) {
        LayerInfo layInfo;
        GetLayerInfo(&layInfo);
        ret = g_displayTest.layerFuncs->OpenLayer(g_displayTest.devId, &layInfo, &g_displayTest.layerId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("open layer failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Obtain the buffer of the layer and add data to the buffer. */
    if (g_displayTest.layerFuncs->GetLayerBuffer != NULL) {
        ret = g_displayTest.layerFuncs->GetLayerBuffer(g_displayTest.devId, g_displayTest.layerId, &g_displayTest.buffer);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("get layer buffer failed");
            return DISPLAY_FAILURE;
        }
        uint16_t *pBuf = (uint16_t *)g_displayTest.buffer.data.virAddr;
        WriteDataToBuf(g_displayTest.displayInfo.width, g_displayTest.displayInfo.height, pBuf);
    }
    /* Flush the layer data for display. */
    if (g_displayTest.layerFuncs->Flush != NULL) {
        ret = g_displayTest.layerFuncs->Flush(g_displayTest.devId, g_displayTest.layerId, &g_displayTest.buffer);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("flush layer failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Close the specified layer of the display device. */
    if (g_displayTest.layerFuncs->CloseLayer != NULL) {
        ret = g_displayTest.layerFuncs->CloseLayer(g_displayTest.devId, g_displayTest.layerId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("close layer failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Close the display device. */
    if (g_displayTest.layerFuncs->DeinitDisplay != NULL) {
        ret = g_displayTest.layerFuncs->DeinitDisplay(g_displayTest.devId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("deinit display failed");
            return DISPLAY_FAILURE;
        }
    }
    /* Unregister display driver APIs. */
    ret = DisplayUninit();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("DisplayUninit fail");
        return ret;
    }
    return 0;
}
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)

