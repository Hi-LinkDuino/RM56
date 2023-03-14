/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_bt_gatt.h"

#include <unistd.h>
#include <cstdint>
#include <sstream>
#include <vector>
#include "__config"
#include "bluetooth_ble_advertiser.h"
#include "bluetooth_ble_central_manager.h"
#include "bluetooth_log.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_utils.h"
#include "cstdint"
#include "iosfwd"
#include "istream"
#include "new"
#include "ohos_bt_adapter_utils.h"
#include "ohos_bt_def.h"
#include "ostream"
#include "securec.h"
#include "streambuf"
#include "string"
#include "uuid.h"


#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

namespace OHOS {
namespace Bluetooth {
#define MAX_BLE_ADV_NUM 7

class BleCentralManagerCallbackWapper;
class BleAdvCallback;

static BtGattCallbacks *g_AppCallback;
static BleCentralManager *g_BleCentralManager;
static BleCentralManagerCallbackWapper *g_scanCallback;

static BleAdvCallback *g_bleAdvCallbacks[MAX_BLE_ADV_NUM];
static BleAdvertiser *g_BleAdvertiser = nullptr;

class BleCentralManagerCallbackWapper : public BleCentralManagerCallback {
public:
    /**
     * @brief Scan result callback.
     *
     * @param result Scan result.
     * @since 6
     */
    void OnScanCallback(const BleScanResult &result) {
        BtScanResultData scanResult;
        scanResult.eventType = OHOS_BLE_EVT_LEGACY_CONNECTABLE; // result.IsConnectable();
        scanResult.dataStatus = OHOS_BLE_DATA_COMPLETE;
        scanResult.addrType = OHOS_BLE_RANDOM_DEVICE_ADDRESS;
        scanResult.primaryPhy = OHOS_BLE_SCAN_PHY_1M;
        scanResult.secondaryPhy = OHOS_BLE_SCAN_PHY_1M;
        scanResult.advSid = 1;
        scanResult.txPower = 1;
        scanResult.rssi = result.GetRssi();
        scanResult.directAddrType = OHOS_BLE_RANDOM_DEVICE_ADDRESS;
        GetAddrFromString(result.GetPeripheralDevice().GetDeviceAddr(), scanResult.addr.addr);
        vector<uint8_t> vec = result.GetPayload();
        scanResult.advData = vec.data();
        scanResult.advLen = vec.size();

        std::string strs;
        std::stringstream strStream;
        for (int i = 0; i < scanResult.advLen; i++) {
            char token[3] = {0};
            (void)sprintf_s(token, 3, "%02X", scanResult.advData[i]);
            strStream << token;
        }
        strStream >> strs;
        string address = result.GetPeripheralDevice().GetDeviceAddr();
        HILOGI("device: %{public}s, scan data: %{public}s", GetEncryptAddr(address).c_str(), strs.c_str());
        if (g_AppCallback != nullptr && g_AppCallback->scanResultCb != nullptr) {
            g_AppCallback->scanResultCb(&scanResult);
        } else {
            HILOGW("call back is null.");
        }
    }

    /**
     * @brief Batch scan results event callback.
     *
     * @param results Scan results.
     * @since 6
     */
    void OnBleBatchScanResultsEvent(const std::vector<BleScanResult> &results) {}

    /**
     * @brief Start or Stop scan event callback.
     *
     * @param resultCode Scan result code.
     * @param isStartScan true->start scan, false->stop scan.
     * @since 6
     */
    void OnStartOrStopScanEvent(int resultCode, bool isStartScan) {}
};

class BleAdvCallback : public BleAdvertiseCallback {
public:
    explicit BleAdvCallback(int advId)
    {
        advId_ = advId;
    }

    NO_SANITIZE("cfi") void OnStartResultEvent(int result) {
        if (result != 0) {
            HILOGE("result : %{public}d", result);
            return;
        }

        HILOGI("adv started. advId_: %{public}d", advId_);
        if (g_AppCallback != nullptr && g_AppCallback->advEnableCb != nullptr) {
            g_AppCallback->advEnableCb(advId_, 0);
        } else {
            HILOGW("call back is null.");
        }
    }

protected:
    BleAdvertiserData *advData;
    BleAdvertiserData *advResponseData;
    BleAdvertiserSettings *advSetting;

private:
    int advId_;
};

/**
 * @brief Initializes the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is initialized;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int InitBtStack(void) {
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Enables the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is enabled;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int EnableBtStack(void) {
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Disables the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is disabled;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int DisableBtStack(void) {
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Sets the Bluetooth device name.
 *
 * @param name Indicates the pointer to the name to set.
 * @param len Indicates the length of the name to set.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth device name is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int SetDeviceName(const char *name, unsigned int len) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Sets advertising data.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param data Indicates the pointer to the advertising data. For details, see {@link StartAdvRawData}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising data is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetAdvData(int advId, const StartAdvRawData data)
{
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Starts advertising.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param param Indicates the pointer to the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartAdv(int advId, const BleAdvParams *param)
{
    return OHOS_BT_STATUS_UNSUPPORTED;
}

static bool IsAllAdvStopped()
{
    for (int i = 0; i < MAX_BLE_ADV_NUM; i++) {
        if (g_bleAdvCallbacks[i] != nullptr) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Stops advertising.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising is stopped;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
NO_SANITIZE("cfi") int BleStopAdv(int advId)
{
    HILOGI("BleStopAdv, advId: %{public}d.", advId);
    if (advId < 0 || advId >= MAX_BLE_ADV_NUM) {
        HILOGE("BleStopAdv fail, advId is invalid.");
        return OHOS_BT_STATUS_FAIL;
    }
    if (g_BleAdvertiser == nullptr || g_bleAdvCallbacks[advId] == nullptr) {
        HILOGE("BleStopAdv fail, the current adv is not started.");
        return OHOS_BT_STATUS_FAIL;
    }

    g_BleAdvertiser->StopAdvertising(*g_bleAdvCallbacks[advId]);

    usleep(100);
    if (g_AppCallback != nullptr && g_AppCallback->advDisableCb != nullptr) {
        HILOGI("adv stopped advId: %{public}d.", advId);
        g_AppCallback->advDisableCb(advId, 0);
    }
    delete g_bleAdvCallbacks[advId];
    g_bleAdvCallbacks[advId] = nullptr;

    if (IsAllAdvStopped()) {
        HILOGI("All adv have been stopped.");
        delete g_BleAdvertiser;
        g_BleAdvertiser = nullptr;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Updates advertising parameters.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param param Indicates the pointer to the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising parameters are updated;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleUpdateAdv(int advId, const BleAdvParams *param) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Sets the secure I/O capability mode.
 *
 * @param mode Indicates the capability mode to set. For details, see {@link BleIoCapMode}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the capability mode is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetSecurityIoCap(BleIoCapMode mode) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Sets the authentication mode for secure connection requests.
 *
 * @param mode Indicates the authentication mode to set. For details, see {@link BleAuthReqMode}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the authentication mode is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Responds to a secure connection request.
 *
 * @param bdAddr Indicates the address of the device that sends the request.
 * @param accept Specifies whether to accept the request. The value <b>true</b> means to accept the request,
 * and <b>false</b> means to reject the request.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the request is responded to;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Obtains the device MAC address.
 *
 * @param mac Indicates the pointer to the device MAC address.
 * @param len Indicates the length of the device MAC address.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the device MAC address is obtained;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len) {
    return OHOS_BT_STATUS_UNSUPPORTED;
}

/**
 * @brief Sets scan parameters.
 *
 * @param clientId Indicates the client ID, which is obtained during client registration.
 * @param param Indicates the pointer to the scan parameters. For details, see {@link BleScanParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan parameters are set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetScanParameters(int clientId, BleScanParams *param)
{
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Starts a scan.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartScan(void)
{
    HILOGI("BleStartScan enter");
    if (g_BleCentralManager == nullptr) {
        HILOGE("BleStartScan fail, ble centra manager is null.");
        return OHOS_BT_STATUS_FAIL;
    }

    g_BleCentralManager->StartScan();
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Stops a scan.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is stopped;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStopScan(void)
{
    HILOGI("BleStopScan enter");
    if (g_BleCentralManager == nullptr) {
        HILOGE("BleStopScan fail, ble centra manager is null.");
        return OHOS_BT_STATUS_FAIL;
    }

    g_BleCentralManager->StopScan();
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Registers GATT callbacks.
 *
 * @param func Indicates the pointer to the callbacks to register. For details, see {@link BtGattCallbacks}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the GATT callbacks are registered;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    HILOGI("BleGattRegisterCallbacks enter");
    if (func == nullptr) {
        HILOGE("func is null.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    g_AppCallback = func;

    if (g_scanCallback == nullptr) {
        g_scanCallback = new BleCentralManagerCallbackWapper();
    }

    if (g_BleCentralManager == nullptr) {
        g_BleCentralManager = new BleCentralManager(*g_scanCallback);
    }
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Sets advertising data and parameters and starts advertising.
 *
 * This function is available for system applications only. \n
 *
 * @param advId Indicates the pointer to the advertisement ID.
 * @param rawData Indicates the advertising data. For details, see {@link StartAdvRawData}.
 * @param advParam Indicates the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam)
{
    HILOGI("BleStartAdvEx enter");
    if (g_BleAdvertiser == nullptr) {
        g_BleAdvertiser = new BleAdvertiser();
    }
    int i = 0;
    for (i = 0; i < MAX_BLE_ADV_NUM; i++) {
        if (g_bleAdvCallbacks[i] == nullptr) {
            g_bleAdvCallbacks[i] = new BleAdvCallback(i);
            break;
        }
    }

    if (i == MAX_BLE_ADV_NUM) {
        return OHOS_BT_STATUS_UNHANDLED;
    }

    *advId = i;
    HILOGI("ret advId: %{public}d.", *advId);

    BleAdvertiserSettings settings;
    settings.SetInterval(advParam.minInterval);
    if (advParam.advType == OHOS_BLE_ADV_SCAN_IND ||
        advParam.advType == OHOS_BLE_ADV_NONCONN_IND) {
        settings.SetConnectable(false);
    }

    vector<uint8_t> advData;
    if (rawData.advData != nullptr) {
        for (unsigned int i = 0; i < rawData.advDataLen; i++) {
            advData.push_back(rawData.advData[i]);
        }
    }

    vector<uint8_t> scanResponse;
    if (rawData.rspData != nullptr) {
        for (unsigned int i = 0; i < rawData.rspDataLen; i++) {
            scanResponse.push_back(rawData.rspData[i]);
        }
    }

    g_BleAdvertiser->StartAdvertising(settings, advData, scanResponse, *g_bleAdvCallbacks[i]);
    return OHOS_BT_STATUS_SUCCESS;
}

static bool SetServiceUuidParameter(BleScanFilter &scanFilter, BleScanNativeFilter *nativeScanFilter)
{
    HILOGI("SetServiceUuidParameter enter");
    if (nativeScanFilter->serviceUuidLength != 0 && nativeScanFilter->serviceUuid != nullptr) {
        if (!regex_match(std::string(reinterpret_cast<char *>(nativeScanFilter->serviceUuid)), uuidRegex)) {
            HILOGE("match the UUID faild.");
            return false;
        }
        UUID serviceUuid = UUID::FromString((char *)nativeScanFilter->serviceUuid);
        scanFilter.SetServiceUuid(serviceUuid);
        if (nativeScanFilter->serviceUuidMask != nullptr) {
            if (!regex_match(std::string(reinterpret_cast<char *>(nativeScanFilter->serviceUuidMask)), uuidRegex)) {
                HILOGE("match the UUID faild.");
                return false;
            }
            UUID serviceUuidMask = UUID::FromString((char *)nativeScanFilter->serviceUuidMask);
            scanFilter.SetServiceUuidMask(serviceUuidMask);
        }
    }
    return true;
}

/**
 * @brief Sets one scan filter config.
 *
 * @param scanFilter Indicates the framework object of scan filter, see {@link BleScanFilter}.
 * @param nativeScanFilter Indicates the pointer to the scan filter. For details, see {@link BleScanNativeFilter}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if set onr scan filter config success;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
static int SetOneScanFilter(BleScanFilter &scanFilter, BleScanNativeFilter *nativeScanFilter)
{
    HILOGI("SetOneScanFilter enter");
    if (nativeScanFilter->address != nullptr) {
        scanFilter.SetDeviceId(nativeScanFilter->address);
    }
    if (nativeScanFilter->deviceName != nullptr) {
        scanFilter.SetName(nativeScanFilter->deviceName);
    }
    bool isSuccess = SetServiceUuidParameter(scanFilter, nativeScanFilter);
    if (!isSuccess) {
        HILOGE("SetServiceUuidParameter faild.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    if (nativeScanFilter->serviceData != nullptr) {
        std::vector<uint8_t> serviceData;
        for (unsigned int i = 0; i < nativeScanFilter->serviceDataLength; i++) {
            serviceData.push_back(nativeScanFilter->serviceData[i]);
        }
        scanFilter.SetServiceData(serviceData);

        if (nativeScanFilter->serviceDataMask != nullptr) {
            std::vector<uint8_t> serviceDataMask;
            for (unsigned int i = 0; i < nativeScanFilter->serviceDataLength; i++) {
                serviceDataMask.push_back(nativeScanFilter->serviceDataMask[i]);
            }
            scanFilter.SetServiceDataMask(serviceDataMask);
        }
    }

    if (nativeScanFilter->manufactureData != nullptr) {
        std::vector<uint8_t> manufactureData;
        for (unsigned int i = 0; i < nativeScanFilter->manufactureDataLength; i++) {
            manufactureData.push_back(nativeScanFilter->manufactureData[i]);
        }
        scanFilter.SetManufactureData(manufactureData);

        if (nativeScanFilter->manufactureDataMask != nullptr) {
            std::vector<uint8_t> manufactureDataMask;
            for (unsigned int i = 0; i < nativeScanFilter->manufactureDataLength; i++) {
                manufactureDataMask.push_back(nativeScanFilter->manufactureDataMask[i]);
            }
            scanFilter.SetManufactureDataMask(manufactureDataMask);
        }

        if (nativeScanFilter->manufactureId != 0) {
            scanFilter.SetManufacturerId(nativeScanFilter->manufactureId);
        }
    }
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Sets scan filter configs.
 *
 * @param filter Indicates the pointer to the scan filter. For details, see {@link BleScanNativeFilter}.
 * @param filterSize Indicates the number of the scan filter.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if set scan filter configs success;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
static int SetConfigScanFilter(BleScanNativeFilter *filter, unsigned int filterSize)
{
    HILOGI("SetConfigScanFilter enter");
    vector<BleScanFilter> scanFilters;
    for (unsigned int i = 0; i < filterSize; i++) {
        BleScanNativeFilter nativeScanFilter = filter[i];
        BleScanFilter scanFilter;
        int result = SetOneScanFilter(scanFilter, &nativeScanFilter);
        if (result != OHOS_BT_STATUS_SUCCESS) {
            HILOGE("SetOneScanFilter faild, result: %{public}d", result);
            return OHOS_BT_STATUS_PARM_INVALID;
        }
        scanFilters.push_back(scanFilter);
    }
    g_BleCentralManager->ConfigScanFilter(scanFilters);
    return OHOS_BT_STATUS_SUCCESS;
}

/**
 * @brief Starts a scan with BleScanConfigs.
 * If don't need ble scan filter, set BleScanNativeFilter to NULL or filterSize to zero.
 * If one of the ble scan filtering rules is not required, set it to NULL.
 * For example, set the address to NULL when you don't need it.
 * Don't support only using manufactureId as filter conditions, need to use it with manufactureData.
 * The manufactureId need to be set a related number when you need a filtering condition of manufactureData.
 *
 * @param configs Indicates the pointer to the scan filter. For details, see {@link BleScanConfigs}.
 * @param filter Indicates the pointer to the scan filter. For details, see {@link BleScanNativeFilter}.
 * @param filterSize Indicates the number of the scan filter.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartScanEx(BleScanConfigs *configs, BleScanNativeFilter *filter, unsigned int filterSize)
{
    HILOGI("BleStartScanEx enter, filterSize %{public}u", filterSize);
    if (g_BleCentralManager == nullptr || configs == nullptr) {
        HILOGE("BleStartScanEx fail, ble centra manager is null or configs is null.");
        return OHOS_BT_STATUS_FAIL;
    }

    if (filter != nullptr && filterSize != 0) {
        int result = SetConfigScanFilter(filter, filterSize);
        if (result != OHOS_BT_STATUS_SUCCESS) {
            HILOGE("SetConfigScanFilter faild, result: %{public}d", result);
            return OHOS_BT_STATUS_PARM_INVALID;
        }
    }

    HILOGI("scanMode: %{public}d", configs->scanMode);
    BleScanSettings settings;
    settings.SetScanMode(configs->scanMode);
    g_BleCentralManager->StartScan(settings);
    return OHOS_BT_STATUS_SUCCESS;
}
}  // namespace Bluetooth
}  // namespace OHOS
#ifdef __cplusplus
}
#endif
/** @} */
