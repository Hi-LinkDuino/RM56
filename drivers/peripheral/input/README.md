# input<a name="EN-US_TOPIC_0000001124789109"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
    -   [Available APIs](#section1551164914237)
    -   [Usage Guidelines](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository mainly defines and implements the following types of Hardware Driver Interfaces \(HDIs\) of the input module, allowing upper-layer input services to perform operations for the input devices:

-   **Input Manager**: manages input devices, including enabling and disabling input devices and obtaining the device list.
-   **Input Reporter**: reports input events, including registering and unregistering data reporting callbacks.
-   **Input Controller**: controls input devices, including obtaining the device information and device type, and setting power supply status.

**Figure  1**  HDI architecture of the input module<a name="fig9489629122818"></a>  
![](figures/hdi-architecture-of-the-input-module.png "hdi-architecture-of-the-input-module")

## Directory Structure<a name="section161941989596"></a>

The source code directory structure is as follows:

```
/drivers/peripheral/input
├── hal                # HAL code
│   └── include       # HAL header files
│   └── src           # HAL code implementation
├── interfaces         # Driver capability APIs provided for upper-layer services
│   └── include       # APIs exposed externally
├── test               # Test code
│   └── unittest      # Unit test code
```

### Available APIs<a name="section1551164914237"></a>

The input driver provides input services with driver capability APIs that can be directly called. The APIs involve the Input Manager module, Input Reporter module, and Input Controller module. For example, you can call the APIs to enable or disable an input device, register a listener callback, query the device information, and control the power status.

[Table 1](#table1513255710559)  describes major HDI APIs provided by the input module.

**Table  1**  Major HDI APIs of the input module

<a name="table1513255710559"></a>
<table><thead align="left"><tr id="row171321857155517"><th class="cellrowborder" align="center" valign="top" width="12.031203120312032%" id="mcps1.2.4.1.1"><p id="p6132957115511"><a name="p6132957115511"></a><a name="p6132957115511"></a>Header File</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="66.006600660066%" id="mcps1.2.4.1.2"><p id="p14132125715552"><a name="p14132125715552"></a><a name="p14132125715552"></a>API</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="21.96219621962196%" id="mcps1.2.4.1.3"><p id="p18132205755516"><a name="p18132205755516"></a><a name="p18132205755516"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row13132357165514"><td class="cellrowborder" rowspan="4" align="left" valign="top" width="12.031203120312032%" headers="mcps1.2.4.1.1 "><p id="p15674038913"><a name="p15674038913"></a><a name="p15674038913"></a></p>
<p id="p825185015460"><a name="p825185015460"></a><a name="p825185015460"></a>input_manager.h</p>
<p id="p2133757135510"><a name="p2133757135510"></a><a name="p2133757135510"></a></p>
</td>
<td class="cellrowborder" valign="top" width="66.006600660066%" headers="mcps1.2.4.1.2 "><p id="p11132857135517"><a name="p11132857135517"></a><a name="p11132857135517"></a>int32_t (*OpenInputDevice)(uint32_t devIndex);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="21.96219621962196%" headers="mcps1.2.4.1.3 "><p id="p8233134675314"><a name="p8233134675314"></a><a name="p8233134675314"></a>Opens a specified input device file.</p>
</td>
</tr>
<tr id="row9132135715515"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p106067813482"><a name="p106067813482"></a><a name="p106067813482"></a>int32_t (*CloseInputDevice)(uint32_t devIndex);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p8233144617532"><a name="p8233144617532"></a><a name="p8233144617532"></a>Closes a specified input device file.</p>
</td>
</tr>
<tr id="row2973185044814"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1974125024812"><a name="p1974125024812"></a><a name="p1974125024812"></a>int32_t (*GetInputDevice)(uint32_t devIndex, DeviceInfo **devInfo);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p152331046155310"><a name="p152331046155310"></a><a name="p152331046155310"></a>Gets information about a specified input device.</p>
</td>
</tr>
<tr id="row171330575555"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p913315573557"><a name="p913315573557"></a><a name="p913315573557"></a>int32_t (*GetInputDeviceList)(uint32_t *devNum, DeviceInfo **devList, uint32_t size);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1523319466539"><a name="p1523319466539"></a><a name="p1523319466539"></a>Gets information about all input devices in the device list.</p>
</td>
</tr>
<tr id="row1513316577554"><td class="cellrowborder" rowspan="3" align="left" valign="top" width="12.031203120312032%" headers="mcps1.2.4.1.1 "><p id="p14171441118"><a name="p14171441118"></a><a name="p14171441118"></a></p>
<p id="p57063567463"><a name="p57063567463"></a><a name="p57063567463"></a>input_reporter.h</p>
</td>
<td class="cellrowborder" valign="top" width="66.006600660066%" headers="mcps1.2.4.1.2 "><p id="p14794193010499"><a name="p14794193010499"></a><a name="p14794193010499"></a>int32_t (*RegisterReportCallback)(uint32_t devIndex, InputReportEventCb *callback);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="21.96219621962196%" headers="mcps1.2.4.1.3 "><p id="p72331046145316"><a name="p72331046145316"></a><a name="p72331046145316"></a>Registers a callback for reporting subscribed data of specified input devices.</p>
</td>
</tr>
<tr id="row171331657185514"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p123921240124910"><a name="p123921240124910"></a><a name="p123921240124910"></a>int32_t (*UnregisterReportCallback)(uint32_t devIndex);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p19233174675311"><a name="p19233174675311"></a><a name="p19233174675311"></a>Unregisters the callback for reporting subscribed data of specified input devices.</p>
</td>
</tr>
<tr id="row41331557165518"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11499124705012"><a name="p11499124705012"></a><a name="p11499124705012"></a>void (*ReportEventPkgCallback)(const EventPackage **pkgs, uint32_t count);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p52331246135315"><a name="p52331246135315"></a><a name="p52331246135315"></a>Reports input event data by the registered callback.</p>
</td>
</tr>
<tr id="row1452521025813"><td class="cellrowborder" rowspan="9" align="left" valign="top" width="12.031203120312032%" headers="mcps1.2.4.1.1 "><p id="p1285144710118"><a name="p1285144710118"></a><a name="p1285144710118"></a></p>
<p id="p854114711117"><a name="p854114711117"></a><a name="p854114711117"></a></p>
<p id="p15741647517"><a name="p15741647517"></a><a name="p15741647517"></a></p>
<p id="p7909447418"><a name="p7909447418"></a><a name="p7909447418"></a></p>
<p id="p12525910165811"><a name="p12525910165811"></a><a name="p12525910165811"></a>input_controller.h</p>
<p id="p942322013262"><a name="p942322013262"></a><a name="p942322013262"></a></p>
<p id="p1040515339526"><a name="p1040515339526"></a><a name="p1040515339526"></a></p>
<p id="p19405533115216"><a name="p19405533115216"></a><a name="p19405533115216"></a></p>
</td>
<td class="cellrowborder" valign="top" width="66.006600660066%" headers="mcps1.2.4.1.2 "><p id="p14402132815113"><a name="p14402132815113"></a><a name="p14402132815113"></a>int32_t (*SetPowerStatus)(uint32_t devIndex, uint32_t status);</p>
</td>
<td class="cellrowborder" align="center" valign="top" width="21.96219621962196%" headers="mcps1.2.4.1.3 "><p id="p1510016065413"><a name="p1510016065413"></a><a name="p1510016065413"></a>Sets the power status.</p>
</td>
</tr>
<tr id="row172902161193"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2062643555111"><a name="p2062643555111"></a><a name="p2062643555111"></a>int32_t (*GetPowerStatus)(uint32_t devIndex, uint32_t *status);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p1323394615539"><a name="p1323394615539"></a><a name="p1323394615539"></a>Gets the power status.</p>
</td>
</tr>
<tr id="row1948179195"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p8207194414510"><a name="p8207194414510"></a><a name="p8207194414510"></a>int32_t (*GetDeviceType)(uint32_t devIndex, uint32_t *deviceType);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p18233154655317"><a name="p18233154655317"></a><a name="p18233154655317"></a>Gets the type of the input device based on the specified device index.</p>
</td>
</tr>
<tr id="row1331121813197"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1321125217514"><a name="p1321125217514"></a><a name="p1321125217514"></a>int32_t (*GetChipInfo)(uint32_t devIndex, char *chipInfo, uint32_t length);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p14233104614536"><a name="p14233104614536"></a><a name="p14233104614536"></a>Gets the chip information of the specified device.</p>
</td>
</tr>
<tr id="row1393181951920"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p79410191191"><a name="p79410191191"></a><a name="p79410191191"></a>int32_t (*GetVendorName)(uint32_t devIndex, char *vendorName, uint32_t length);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p7233124695317"><a name="p7233124695317"></a><a name="p7233124695317"></a>Gets the module vendor name of the specified device.</p>
</td>
</tr>
<tr id="row8960121911198"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p17712110185218"><a name="p17712110185218"></a><a name="p17712110185218"></a>int32_t (*GetChipName)(uint32_t devIndex, char *chipName, uint32_t length);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p923316468539"><a name="p923316468539"></a><a name="p923316468539"></a>Gets the driver chip name of the specified device.</p>
</td>
</tr>
<tr id="row10812112081919"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20738171735219"><a name="p20738171735219"></a><a name="p20738171735219"></a>int32_t (*SetGestureMode)(uint32_t devIndex, uint32_t gestureMode);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p5233546175310"><a name="p5233546175310"></a><a name="p5233546175310"></a>Sets the gesture mode.</p>
</td>
</tr>
<tr id="row12422102092613"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p56701925155215"><a name="p56701925155215"></a><a name="p56701925155215"></a>int32_t (*RunCapacitanceTest)(uint32_t devIndex, uint32_t testType, char *result, uint32_t length);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p13234346125310"><a name="p13234346125310"></a><a name="p13234346125310"></a>Conducts a capacitance self-test.</p>
</td>
</tr>
<tr id="row124041233155211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16405113375215"><a name="p16405113375215"></a><a name="p16405113375215"></a>int32_t (*RunExtraCommand)(uint32_t devIndex, InputExtraCmd *cmd);</p>
</td>
<td class="cellrowborder" align="center" valign="top" headers="mcps1.2.4.1.2 "><p id="p192341246185314"><a name="p192341246185314"></a><a name="p192341246185314"></a>Executes the specified extra command.</p>
</td>
</tr>
</tbody>
</table>

### Usage Guidelines<a name="section129654513264"></a>

The core function of this repository is to provide HDIs for upper-layer input system services to implement input driver capabilities.

The following sample code describes how to use the input HDIs:

```
#include "input_manager.h"
#define DEV_INDEX 1

IInputInterface *g_inputInterface;
InputReportEventCb g_callback;

/* Define the callback for data reporting. */
static void ReportEventPkgCallback(const EventPackage **pkgs, uint32_t count)
{
    if (pkgs == NULL || count > MAX_PKG_NUM) {
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        HDF_LOGI("%s: pkgs[%d] = 0x%x, 0x%x, %d", __func__, i, pkgs[i]->type, pkgs[i]->code, pkgs[i]->value);
    }
}

int InputServiceSample(void)
{
    uint32_t devType = INIT_DEFAULT_VALUE;

    /* Get interfaces of input driver capabilities. */
    int ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get input interfaces failed, ret = %d", __func__, ret);
        return ret;
    }

    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Open a specified input device file. */
    ret = g_inputInterface->iInputManager->OpenInputDevice(DEV_INDEX);
    if (ret) {
        HDF_LOGE("%s: open input device failed, ret = %d", __func__, ret);
 	return ret;
    }

    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Get the type of the input device. */
    ret = g_inputInterface->iInputController->GetDeviceType(DEV_INDEX, &devType);
    if (ret) {
        HDF_LOGE("%s: get device type failed, ret: %d", __FUNCTION__, ret);
        return ret;
    }
    HDF_LOGI("%s: device1's type is %u\n", __FUNCTION__, devType);

    /* Register the data reporting callback for a specified input device. */
    g_callback.ReportEventPkgCallback = ReportEventPkgCallback;
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(DEV_INDEX, &g_callback);
    if (ret) {
        HDF_LOGE("%s: register callback failed, ret: %d", __FUNCTION__, ret);
	return ret;
    }
    HDF_LOGI("%s: wait 10s for testing, pls touch the panel now", __FUNCTION__);
    OsalMSleep(KEEP_ALIVE_TIME_MS);

    /* Unregister the callback of the specified input device. */
    ret  = g_inputInterface->iInputReporter->UnregisterReportCallback(DEV_INDEX);
    if (ret) {
        HDF_LOGE("%s: unregister callback failed, ret: %d", __FUNCTION__, ret);
        return ret;
    }

    /* Close a specified input device file. */
    ret = g_inputInterface->iInputManager->CloseInputDevice(DEV_INDEX);
    if (ret) {
        HDF_LOGE("%s: close device failed, ret: %d", __FUNCTION__, ret);
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

