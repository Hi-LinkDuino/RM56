# WLAN服务组件

## 简介
WLAN服务组件为设备提供接入与使用WLAN的相关接口，包括开启、关闭WLAN，监听WLAN状态等。

## 目录

```
foundation/communication/wifi_lite
├── interfaces         # 组件对外提供接口代码存放目录
│   └── wifiservice    # WLAN服务接口代码存放目录
├── LICENSE            # 版权声明文件

```

## 约束
WLAN服务组件使用C语言编写，目前支持wifi-iot开发板。

## 使用
- 设备需要完成以下开发步骤，以开启WLAN：

    ```
    //开启WLAN
    WifiErrorCode error = EnableWifi();
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以触发WLAN扫描：

    ```
    //开启WLAN
    WifiErrorCode error = Scan();
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以获取WLAN扫描结果：

    ```
    WifiScanInfo* infoList = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    error = GetScanInfoList(infoList, &size);
    if (error != WIFI_SUCCESS || size == 0) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以配置接入点的接入方式：

    ```
    int netId = 0;
    WifiDeviceConfig config = {0};
    config.freq = 20;
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.wapiPskType = WIFI_PSK_TYPE_ASCII;
    memcpy_s(config.ssid, WIFI_MAX_SSID_LEN, "Sample", 10); //在这里输入待连接WLAN的名称
    unsigned char bssid[WIFI_MAC_LEN] = {0xac, 0x75, 0x1d, 0xd8, 0x55, 0xc1}; //在这里输入待连接WLAN的BSSID
    memcpy_s(config.bssid, WIFI_MAC_LEN, bssid, WIFI_MAC_LEN);
    memcpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, "1234567890", 10); //在这里输入待连接WLAN的口令
    WifiErrorCode error = AddDeviceConfig(&config, &netId);
    if (error != WIFI_SUCCESS || netId == 0) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以连接配置的WLAN：

    ```
    error = ConnectTo(netId);  //netId来自上一步
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```