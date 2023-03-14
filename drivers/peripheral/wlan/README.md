# WLAN<a name="EN-US_TOPIC_0000001078436908"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Available APIs](#section1564411661810)
-   [Usage Guidelines](#section19806524151819)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

This repository defines and implements the WLAN-related Hardware Driver Interfaces \(HDIs\) which provide the following functionalities:

1.  Creating and stopping a channel between the hardware abstraction layer \(HAL\) and the WLAN driver
2.  Obtaining the WLAN features supported by the device
3.  Creating a WLAN feature instance

**Figure  1**  WLAN driver module architecture<a name="fig14142101381112"></a>  
![](figures/wlan-driver-module-architecture.png "wlan-driver-module-architecture")

## Directory Structure<a name="section161941989596"></a>

The directory structure of the WLAN repository is as follows:

```
/drivers/peripheral/wlan
├── client             # Client that implements the communication between the user space and kernel space
│   └── include       # Client header files
│   └── src           # Client code
├── hal                # HAL code
│   └── include       # HAL header files
│   └── src           # HAL code implementation
├── interfaces         # APIs exposed externally
│   └── include       # Header files containing APIs exposed externally
```

## Available APIs<a name="section1564411661810"></a>

The WLAN HAL module provides APIs for the Wi-Fi service, such as creating and destroying an  **IWiFi**  object and setting the MAC address. The following table lists the APIs.

**Table  1**  APIs provided by the WLAN HAL module

<a name="table1521573319472"></a>
<table><thead align="left"><tr id="row121519334474"><th class="cellrowborder" valign="top" width="15.079999999999998%" id="mcps1.2.4.1.1"><p id="p1221510339475"><a name="p1221510339475"></a><a name="p1221510339475"></a>Header File</p>
</th>
<th class="cellrowborder" valign="top" width="60.33%" id="mcps1.2.4.1.2"><p id="p0215153344716"><a name="p0215153344716"></a><a name="p0215153344716"></a>API</p>
</th>
<th class="cellrowborder" valign="top" width="24.59%" id="mcps1.2.4.1.3"><p id="p1421503315478"><a name="p1421503315478"></a><a name="p1421503315478"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row112150333476"><td class="cellrowborder" rowspan="12" valign="top" width="15.079999999999998%" headers="mcps1.2.4.1.1 "><p id="p2155710125317"><a name="p2155710125317"></a><a name="p2155710125317"></a>wifi_hal.h</p>
<p id="p189132019183"><a name="p189132019183"></a><a name="p189132019183"></a></p>
</td>
<td class="cellrowborder" valign="top" width="60.33%" headers="mcps1.2.4.1.2 "><p id="p363110387399"><a name="p363110387399"></a><a name="p363110387399"></a>int32_t WifiConstruct(struct IWiFi **wifiInstance);</p>
</td>
<td class="cellrowborder" valign="top" width="24.59%" headers="mcps1.2.4.1.3 "><p id="p1363012387393"><a name="p1363012387393"></a><a name="p1363012387393"></a>Creates an <strong id="b1851191156"><a name="b1851191156"></a><a name="b1851191156"></a>IWiFi</strong> object with basic capabilities.</p>
</td>
</tr>
<tr id="row112151233194714"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7629163817393"><a name="p7629163817393"></a><a name="p7629163817393"></a>int32_t WifiDestruct(struct IWiFi **wifiInstance);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p2627638173917"><a name="p2627638173917"></a><a name="p2627638173917"></a>Destroys an <strong id="b1088016455520"><a name="b1088016455520"></a><a name="b1088016455520"></a>IWiFi</strong> object.</p>
</td>
<tr id="row1214611564542"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4950434142314"><a name="p4950434142314"></a><a name="p4950434142314"></a>int32_t (*start)(struct IWiFi *);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1162543816393"><a name="p1162543816393"></a><a name="p1162543816393"></a>Creates a channel between the HAL and the driver and obtains the NIC supported by the driver.</p>
</td>
</tr>
<tr id="row167876105514"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p162433816392"><a name="p162433816392"></a><a name="p162433816392"></a>int32_t (*stop)(struct IWiFi *);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p186235383393"><a name="p186235383393"></a><a name="p186235383393"></a>Stops the channel between the HAL and the driver.</p>
</td>
</tr>
<tr id="row77607815516"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6760128135514"><a name="p6760128135514"></a><a name="p6760128135514"></a>int32_t (*getSupportFeature)(uint8_t *supType, uint32_t size);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1176014845516"><a name="p1176014845516"></a><a name="p1176014845516"></a>Obtains the WLAN features available for the device no matter whether it works as an AP, STA, or P2P server/client.</p>
</td>
</tr>
<tr id="row93918373572"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2039103714572"><a name="p2039103714572"></a><a name="p2039103714572"></a>int32_t (*getSupportCombo)(uint64_t *combo, uint32_t size);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13916375570"><a name="p13916375570"></a><a name="p13916375570"></a>Obtains the WLAN features available for the device that plays different roles simultaneously (any combination of AP, STA, and P2P server/client).</p>
</td>
</tr>
<tr id="row25031321195815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13503142195814"><a name="p13503142195814"></a><a name="p13503142195814"></a>int32_t (*createFeature)(int32_t type, struct IWiFiBaseFeature **ifeature);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p11503172115585"><a name="p11503172115585"></a><a name="p11503172115585"></a>Creates an <strong id="b85498454446"><a name="b85498454446"></a><a name="b85498454446"></a>IWiFiBaseFeature</strong> object of a specified type.</p>
</td>
</tr>
<tr id="row14371833185819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1443863325820"><a name="p1443863325820"></a><a name="p1443863325820"></a>int32_t (*getFeatureByIfName)(const char *ifName, struct IWiFiBaseFeature **ifeature);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1343819334584"><a name="p1343819334584"></a><a name="p1343819334584"></a>Obtains an <strong id="b1678312213451"><a name="b1678312213451"></a><a name="b1678312213451"></a>IWiFiBaseFeature</strong> object based on a specified network interface name.</p>
</td>
</tr>
<tr id="row03398561587"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1933965615812"><a name="p1933965615812"></a><a name="p1933965615812"></a>int32_t (*registerEventCallback)(CallbackFunc cbFunc);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p103396560587"><a name="p103396560587"></a><a name="p103396560587"></a>Registers a callback to listen for IWiFi asynchronous events.</p>
</td>
</tr>
<tr id="row894162955918"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1694112914590"><a name="p1694112914590"></a><a name="p1694112914590"></a>int32_t (*unRegisterEventCallback)(void);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p18948295597"><a name="p18948295597"></a><a name="p18948295597"></a>Unregisters an <strong id="b1979834134512"><a name="b1979834134512"></a><a name="b1979834134512"></a>IWiFi</strong> callback.</p>
</td>
</tr>
<tr id="row122731451103"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2273125104"><a name="p2273125104"></a><a name="p2273125104"></a>int32_t (*destroyFeature)(struct IWiFiBaseFeature *ifeature);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p92731553017"><a name="p92731553017"></a><a name="p92731553017"></a>Destroys a specified <strong id="b15348165794518"><a name="b15348165794518"></a><a name="b15348165794518"></a>IWiFiBaseFeature</strong> object.</p>
</td>
</tr>
<tr id="row172153335473"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1292417115515"><a name="p1292417115515"></a><a name="p1292417115515"></a>int32_t (*resetDriver)(const uint8_t chipId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p17272317145516"><a name="p17272317145516"></a><a name="p17272317145516"></a>Resets the WLAN driver with a specified chip ID.</p>
</td>
</tr>
<tr id="row27321001058"><td class="cellrowborder" rowspan="2" valign="top" width="15.079999999999998%" headers="mcps1.2.4.1.1 "><p id="p134208191041"><a name="p134208191041"></a><a name="p134208191041"></a>wifi_hal_ap_feature.h</p>
</td>
<td class="cellrowborder" valign="top" width="60.33%" headers="mcps1.2.4.1.2 "><p id="p77321105516"><a name="p77321105516"></a><a name="p77321105516"></a>int32_t (*getAsscociatedStas)(const struct IWiFiAp *apFeature, struct StaInfo *staInfo, uint32_t count, uint32_t *num);</p>
</td>
<td class="cellrowborder" valign="top" width="24.59%" headers="mcps1.2.4.1.3 "><p id="p1673213014513"><a name="p1673213014513"></a><a name="p1673213014513"></a>Obtains information (MAC addresses only in the current version) about all the connected STAs.</p>
</td>
</tr>
<tr id="row39802291664"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p149801729465"><a name="p149801729465"></a><a name="p149801729465"></a>int32_t (*setCountryCode)(const struct IWiFiAp *apFeature, const char *code, uint32_t len);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p098022910610"><a name="p098022910610"></a><a name="p098022910610"></a>Sets the country/region code.</p>
</td>
<tr id="row14149145512411"><td class="cellrowborder" rowspan="1" valign="top" width="15.079999999999998%" headers="mcps1.2.4.1.1 "><p id="p18149155246"><a name="p18149155246"></a><a name="p18149155246"></a>wifi_hal_sta_feature.h</p>
</td>
<td class="cellrowborder" valign="top" width="60.33%" headers="mcps1.2.4.1.2 "><p id="p17149165511414"><a name="p17149165511414"></a><a name="p17149165511414"></a>int32_t (*setScanningMacAddres)(const struct IWiFiSta *staFeature, unsigned char *scanMac, uint8_t len);</p>
</td>
<td class="cellrowborder" valign="top" width="24.59%" headers="mcps1.2.4.1.3 "><p id="p181491255149"><a name="p181491255149"></a><a name="p181491255149"></a>Sets a single MAC address to scan for.</p>
</td>
<tr id="row451796205011"><td class="cellrowborder" rowspan="8" valign="top" width="15.079999999999998%" headers="mcps1.2.4.1.1 "><p id="p2659417135013"><a name="p2659417135013"></a><a name="p2659417135013"></a>wifi_hal_base_feature.h</p>
</td>
<td class="cellrowborder" valign="top" width="60.33%" headers="mcps1.2.4.1.2 "><p id="p73831421111010"><a name="p73831421111010"></a><a name="p73831421111010"></a>const char *(*getNetworkIfaceName)(const struct IWiFiBaseFeature *baseFeature);</p>
</td>
<td class="cellrowborder" valign="top" width="24.59%" headers="mcps1.2.4.1.3 "><p id="p16363321101014"><a name="p16363321101014"></a><a name="p16363321101014"></a>Obtains the name of a network interface.</p>
</td>
</tr>
<tr id="row1657914017107"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15117233152315"><a name="p15117233152315"></a><a name="p15117233152315"></a>int32_t (*getFeatureType)(const struct IWiFiBaseFeature *);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p195182610507"><a name="p195182610507"></a><a name="p195182610507"></a>Obtains the feature type.</p>
</td>
</tr>
<tr id="row5518663503"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p125181260501"><a name="p125181260501"></a><a name="p125181260501"></a>int32_t (*setMacAddress)(const struct IWiFiBaseFeature *, unsigned char *, uint8_t);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1151815635014"><a name="p1151815635014"></a><a name="p1151815635014"></a>Sets the MAC address.</p>
</td>
</tr>
<tr id="row851915617503"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20519865500"><a name="p20519865500"></a><a name="p20519865500"></a>int32_t (*getDeviceMacAddress)(const struct IWiFiBaseFeature *, unsigned char *, uint8_t);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p14519469509"><a name="p14519469509"></a><a name="p14519469509"></a>Obtains the device MAC address.</p>
</td>
</tr>
<tr id="row986761516115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2867181581116"><a name="p2867181581116"></a><a name="p2867181581116"></a>int32_t (*getValidFreqsWithBand)(const struct IWiFiBaseFeature *baseFeature, int32_t band, int32_t *freqs, uint32_t count, uint32_t *num);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p15867101551112"><a name="p15867101551112"></a><a name="p15867101551112"></a>Obtains the frequencies supported by the 2.4 GHz or 5 GHz band.</p>
</td>
</tr>
<tr id="row18232258171117"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p145195620502"><a name="p145195620502"></a><a name="p145195620502"></a>int32_t (*setTxPower)(const struct IWiFiBaseFeature *, int32_t);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p175191863503"><a name="p175191863503"></a><a name="p175191863503"></a>Sets the transmit power.</p>
</td>
</tr>
<tr id="row1744948201219"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7449128101215"><a name="p7449128101215"></a><a name="p7449128101215"></a>int32_t (*getChipId)(const struct IWiFiBaseFeature *baseFeature, uint8_t *chipId);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1244998131219"><a name="p1244998131219"></a><a name="p1244998131219"></a>Obtains the chip ID of the current driver.</p>
</td>
</tr>
<tr id="row34351131216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p743711161213"><a name="p743711161213"></a><a name="p743711161213"></a>int32_t (*getIfNamesByChipId)(const uint8_t chipId, char **ifNames, uint32_t *num);</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p164361118121"><a name="p164361118121"></a><a name="p164361118121"></a>Obtains names of all the NICs of the current chip based on the chip ID.</p>
</td>
</tbody>
</table>

## Usage Guidelines<a name="section19806524151819"></a>

The following describes how to use the WLAN HAL module.

1.  Call the  **WifiConstruct**  function to create an  **IWiFi**  object.
2.  Use the created  **IWiFi**  object to call the  **start**  function to create a channel between the HAL and the driver and obtain the driver NIC information.
3.  Call the  **createFeature**  function to create an AP feature or STA feature. You can call functions to perform operations on the created feature \(use an AP feature as an example\).
4.  Call functions to perform operations, such as calling the  **setMacAddress**  function to set the MAC address and calling the  **getDeviceMacAddress**  function to obtain the device MAC address.
5.  Call the  **destroyFeature**  function to destroy the created feature.
6.  Call the  **stop**  function to stop the channel between the HAL and the driver.
7.  Call the  **WifiDestruct**  function to destroy the  **IWiFi**  object.

The sample code is as follows:

```
#include "wifi_hal.h"
#include "wifi_hal_sta_feature.h"
#include "wifi_hal_ap_feature.h"
#include "wifi_hal_cmd.h"
#include "wifi_hal_event.h"

#define MAC_LEN 6

static void *hal_main()
{
    int ret;
    struct IWiFi *wifi;

    /* Create an IWiFi object. */
    ret = WifiConstruct(&wifi);
    if (ret != 0 || wifi == NULL) {
        return;
    }

    /* Create a channel between the HAL and the driver. */
    ret = wifi->start(wifi);
    if (ret != 0) {
        return;
    }

    /* Create an AP feature. */
    ret = wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    if (ret != 0) {
        return;
    }

    /* Obtain the device MAC address. */
    unsigned char mac[MAC_LEN] = {0};
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, mac, MAC_LEN);
    if (ret != 0) {
        return;
    }

    /* Destroy the created AP feature. */
    ret = wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    if (ret != 0) {
        return;
    }

    /* Stop the created channel. */
    ret = wifi->stop(wifi);
    if (ret != 0) {
        return;
    }

    /* Destroy the created IWiFi object. */
    ret = WifiDestruct(&wifi);
    if (ret != 0) {
        return;
    }
    return;
}
```

## Repositories Involved<a name="section1371113476307"></a>

[Driver subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/driver-subsystem.md)

[drivers\_framework](https://gitee.com/openharmony/drivers_framework/blob/master/README.md)

[drivers\_adapter](https://gitee.com/openharmony/drivers_adapter/blob/master/README.md)

[drivers\_adapter\_khdf\_linux](https://gitee.com/openharmony/drivers_adapter_khdf_linux/blob/master/README.md)

[drivers\_peripheral](https://gitee.com/openharmony/drivers_peripheral)

