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

#ifndef BLE_ADVERTISER_IMPL_H
#define BLE_ADVERTISER_IMPL_H

#include <map>
#include <mutex>

#include "adapter_manager.h"
#include "ble_defs.h"
#include "gap_le_if.h"
#include "interface_adapter_ble.h"
#include "log.h"
#include "log_util.h"
#include "timer.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief BLE advertiser wrap data.
 */
struct BleAdvertiserImplWrapData {
    BleAdvertiserImplWrapData(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
        const BleAdvertiserDataImpl &rspData, int advStatus,
        const STOP_ALL_ADV_TYPE &isUserStopAdv = STOP_ADV_TYPE_SINGLE)
        : settings_(settings), advData_(advData), rspData_(rspData), timer_(nullptr)
    {
        advStatus_ = advStatus;
        stopAllAdvType_ = isUserStopAdv;
    }

    BleAdvertiserSettingsImpl settings_ {};
    BleAdvertiserDataImpl advData_ {};
    BleAdvertiserDataImpl rspData_ {};
    std::unique_ptr<utility::Timer> timer_ {};
    int advStatus_ {};
    STOP_ALL_ADV_TYPE stopAllAdvType_ {};
};
/**
 * @brief BLE advertiser.
 */
class BleAdvertiserImpl {
public:
    /**
     * @brief Constructor.
     */
    explicit BleAdvertiserImpl(
        IBleAdvertiserCallback &callback, IAdapterBle &bleAdapter, utility::Dispatcher &dispatch);

    /**
     * @brief Destructor.
     */
    ~BleAdvertiserImpl();

    /**
     * @brief Get advertising status
     *
     * @return @c advertiser status.
     */
    int GetAdvertisingStatus() const;

    /**
     * @brief Restart Bluetooth LE Advertising.
     *
     */
    void ReStartLegacyAdvertising() const;

    /**
     * @brief Start Bluetooth LE Advertising.
     *
     * @param [in] Advertising parameters.
     * @param [in] Advertising data.
     * @param [in] Scan response data.
     * @param [in] Advertising handle.
     */
    void StartAdvertising(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
        const BleAdvertiserDataImpl &scanResponse, uint8_t advHandle);

    /**
     * @brief Stop Bluetooth LE advertising.
     *
     * @param [in] Advertise handle.
     * @param [in] True:RPA timer auto stop.
     */
    void StopAdvertising(uint8_t advHandle) const;
    void StartOrStopAllAdvertising(const STOP_ALL_ADV_TYPE &stopAllAdvType, bool isStartAdv = false) const;
    /**
     * @brief deregister scan callback to gap
     *
     * @return @c status.
     */
    int DeregisterCallbackToGap();

    /**
     * @brief Cleans up advertisers.
     */
    void Close(uint8_t advHandle) const;

    /**
     * @brief Create advertising set handle.
     *
     * @return @c  Advertising handle.
     */
    uint8_t CreateAdvertiserSetHandle() const;

    /**
     * @brief Add characteristic value.
     *
     * @param adtype Type of the field.
     * @param data Field data.
     */
    void AddCharacteristicValue(uint8_t adtype, const std::string &data) const;
    void GAPExAdvClearHandle();

private:
    void LegacyAdvertising(uint8_t advHandle);
    void ExtentAdvertising(uint8_t advHandle);
    /**
     * @brief Gap set avertising parameter callback
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void AdvSetParamResult(uint8_t status, void *context);

    /**
     * @brief Gap set avertising read tx power callback
     *
     * @param [in] status.
     * @param [in] tx power level.
     * @param [in] context.
     */
    static void AdvReadTxPower(uint8_t status, int8_t txPower, void *context);

    /**
     * @brief Gap set advertising data callback
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void AdvSetDataResult(uint8_t status, void *context);

    /**
     * @brief Gap set scan rsp data callback
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void AdvSetScanRspDataResult(uint8_t status, void *context);

    /**
     * @brief Gap set advertising enable callback
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void AdvSetEnableResult(uint8_t status, void *context);

    // Extended Advertising callback
    static void ExAdvSetRandAddrResult(uint8_t status, void *context);
    static void ExAdvSetParamResult(uint8_t status, uint8_t selectTxPower, void *context);
    static void ExAdvSetDataResult(uint8_t status, void *context);
    static void ExAdvSetScanRspDataResult(uint8_t status, void *context);
    static void ExAdvSetEnableResult(uint8_t status, void *context);
    static void ExAdvRemoveHandleResult(uint8_t status, void *context);
    static void ExAdvClearHandleResult(uint8_t status, void *context);
    static void ExAdvScanRequestReceived(uint8_t advHandle, const BtAddr *scannerAddr, void *context);
    static void ExAdvTerminatedAdvSet(
        uint8_t status, uint8_t advHandle, uint16_t connectionHandle, uint8_t completedNumber, void *context);

    /**
     * @brief Set avertising parameter to gap
     *
     * @param [in] Avertising setting parameter.
     * @return @c status.
     */
    int SetAdvParamToGap(const BleAdvertiserSettingsImpl &settings) const;

    /**
     * @brief Set extend avertising parameter to gap
     *
     * @return @c status.
     */
    int SetExAdvParamToGap(const BleAdvertiserSettingsImpl &settings) const;

    /**
     * @brief Set avertising parameter
     *
     * @param [in] Avertising setting parameter.
     * @return @c status.
     */
    void SetAdvParam(const BleAdvertiserSettingsImpl &settings) const;

    /**
     * @brief Set avertising data to gap
     *
     * @param [in] avertising data.
     * @param [in] avertising setting data.
     * @param [in] local txpower.
     * @return @c status.
     */
    int SetAdvDataToGap(
        const BleAdvertiserDataImpl &advData, const BleAdvertiserSettingsImpl &settings, int8_t txPowerLevel) const;

    /**
     * @brief Set extend avertising data to gap
     *
     * @param [in] avertising data.
     * @param [in] avertising setting data.
     * @param [in] local txpower.
     * @return @c status.
     */
    int SetExAdvDataToGap(
        const BleAdvertiserDataImpl &advData, const BleAdvertiserSettingsImpl &settings, int8_t txPowerLevel) const;

    /**
     * @brief Set avertising response data to gap
     *
     * @param [in] avertising response data.
     * @param [in] local txpower value.
     * @return @c status.
     */
    int SetAdvScanRspDataToGap(const BleAdvertiserDataImpl &scanResponse, const BleAdvertiserSettingsImpl &settings,
        int8_t txPowerLevel) const;

    /**
     * @brief Set extend avertising response data to gap
     *
     * @param [in] avertising response data.
     * @param [in] local txpower value.
     * @return @c status.
     */
    int SetExAdvScanRspDataToGap(const BleAdvertiserDataImpl &scanResponse, const BleAdvertiserSettingsImpl &settings,
        int8_t txPowerLevel) const;

    /**
     * @brief Set avertising enable status
     *
     * @param [in] avertising enable.
     * @return @c status.
     */
    int SetAdvEnableToGap(bool isEnable) const;
    /**
     * @brief Set extend avertising enable status
     *
     * @param [in] avertising enable.
     * @return @c status.
     */
    int SetExAdvEnableToGap(uint8_t advHandle, bool isEnable) const;

    /**
     * @brief Setextend avertising  batch enable status
     *
     * @param [in] avertising enable.
     * @return @c status.
     */
    int SetExAdvBatchEnableToGap(bool isEnable) const;

    /**
     * @brief Register avertising callback to gap
     *
     * @return @c status.
     */
    int RegisterCallbackToGap();

    /**
     * @brief Register extend avertising callback to gap
     *
     * @return @c status.
     */
    int RegisterExAdvCallbackToGap();

    /**
     * @brief Check avertising parameter
     *
     * @param [in] avertising parameters.
     * @param [in] avertising data.
     * @param [in] scan response data.
     * @return @c status.
     */
    int CheckAdvertiserPara(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
        const BleAdvertiserDataImpl &scanResponse) const;
    int CheckAdvertiserFlag(const std::string &payload) const;
    static int CheckAdvertiserLen(uint8_t payload, uint8_t advType);

    /**
     * @brief Set avertising max interval
     *
     * @param [in] interval.
     */
    void SetMaxInterval(uint16_t maxinterval) const;

    /**
     * @brief Set avertising min interval
     *
     * @param [in] min interval.
     */
    void SetMinInterval(uint16_t mininterval) const;

    /**
     * @brief Set scan filter
     *
     * @param [in] White list.
     * @param [in] White list.
     */
    void SetFilter(bool scanReqWhitelistOnly, bool connectWhitelistOnly) const;

    /**
     * @brief Set channel map
     *
     * @param [in] channel map.
     */
    void SetChannelMap(const BLE_ADV_CHANNEL &channelMap) const;

    /**
     * @brief timer call back
     *
     */
    static void TimerCallback(void *context, uint8_t advHandle);
    static void TimerCallbackEx(void *context, uint8_t advHandle);
    static void GenResPriAddrResult(uint8_t result, const uint8_t addr[BT_ADDRESS_SIZE], void *context);

    void HandleGapEvent(const BLE_GAP_CB_EVENT &event, int status, int8_t txPower = 0);
    void HandleGapExAdvEvent(const BLE_GAP_CB_EVENT &event, int status, int8_t txPower = 0, uint8_t handle = 0);
    void RemoveAdvHandle(uint8_t handle) const;
    void RemoveAllAdvHandle(int status) const;
    void AdvSetEnableResultTask(uint8_t status);
    void ExAdvSetEnableResultTask(uint8_t status);
    void GapAdvParamSetCompleteEvt(int status) const;
    void GapAdvReadTxPowerEvt(int status, int8_t txPower) const;
    void GapAdvDataSetCompleteEvt(int status, int8_t txPower) const;
    void GapAdvScanRspDataSetCompleteEvt(int status) const;
    void GapAdvStartCompleteEvt(int status);
    void GapAdvStopCompleteEvt(int status) const;

    void GapExAdvSetRandAddrResultEvt(int status) const;
    void GapExAdvParamSetCompleteEvt(int status, int8_t txPower) const;
    void GapExAdvDataSetCompleteEvt(int status, int8_t txPower);
    void GapExAdvScanRspDataSetCompleteEvt(int status);
    void GapExAdvStartCompleteEvt(int status);
    void GapExAdvStopCompleteEvt(int status) const;
    void GapExAdvResolvingListStartCompleteEvt(int status) const;
    void GapExAdvResolvingListStopCompleteEvt(int status) const;
    void GapExAdvStopAllCompleteEvt(int status) const;
    void GapExAdvRemoveHandleResultEvt(int status) const;
    void GapExAdvTerminatedAdvSetEvt(int status, uint8_t handle) const;
    void StartLegacyAdvOrExAdv(uint8_t advHandle);
    static int GetMaxAdvertisingDataLength(const BleAdvertiserSettingsImpl &settings);
    int ExAdvDataFragment(const BleAdvertiserDataImpl &data) const;
    int ExResDataFragment(const BleAdvertiserDataImpl &data) const;
    void GenResPriAddrResultTask(uint8_t result, BtAddr btAddr) const;

    void StartAllAdvertising(const STOP_ALL_ADV_TYPE &stopAllAdvType, bool isStartAdv) const;
    void StopAllAdvertising(const STOP_ALL_ADV_TYPE &stopAllAdvType, bool isStartAdv) const;
    void UpdateAllAdvertisingStatus(const ADVERTISE_STATUS &advStatus) const;

    /// Advertising callback
    IBleAdvertiserCallback *callback_ = nullptr;
    /// Gap exadv callback
    GapExAdvCallback exAdvCallback_ {};
    IAdapterBle *bleAdapter_ = nullptr;
    utility::Dispatcher *dispatcher_ = nullptr;

    BT_DISALLOW_COPY_AND_ASSIGN(BleAdvertiserImpl);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_ADVERTISER_IMPL_H
