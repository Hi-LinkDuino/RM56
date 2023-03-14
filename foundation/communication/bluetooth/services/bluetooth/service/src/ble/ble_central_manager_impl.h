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

#ifndef BLE_CENTRAL_MANAGER_IMPL_H
#define BLE_CENTRAL_MANAGER_IMPL_H

#include <map>
#include <vector>

#include "ble_defs.h"
#include "ble_scan_filter/include/i_ble_scan_filter.h"
#include "dispatcher.h"
#include "gap_le_if.h"
#include "interface_adapter_ble.h"
#include "log.h"
#include "log_util.h"
#include "semaphore_utils.h"
#include "timer.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief BLE scan manager.
 */
class BleCentralManagerImpl {
public:
    /**
     * @brief Constructor.
     */
    explicit BleCentralManagerImpl(
        IBleCentralManagerCallback &callback, IAdapterBle &bleAdapter, utility::Dispatcher &dispatch);

    /**
     * @brief Destructor.
     */
    ~BleCentralManagerImpl();

    /**
     * @brief  Start Bluetooth LE scan with filters.
     *
     * @param [in] for finding exact BLE devices.
     */
    void StartScan() const;

    /**
     * @brief Start Bluetooth LE scan with filters,dutyRatio and matching mode.
     *
     * @param [in] for finding exact BLE devices.
     * @param [in] callback type.
     */
    void StartScan(const BleScanSettingsImpl &setting) const;

    /**
     * @brief Stops Bluetooth LE scan
     */
    void StopScan() const;

    /**
     * @brief Start or stop Bluetooth LE scan
     */
    void StartOrStopScan(const STOP_SCAN_TYPE &scanType, bool isStartScan = false) const;

    /**
     * @brief config scan filter.
     *
     * @param [in] filter filter list
     * @return @c client id
     */
    int ConfigScanFilter(const int clientId, const std::vector<BleScanFilterImpl> &filter);

    /**
     * @brief remove scan filter.
     *
     * @param [in] clientId client id
     */
    void RemoveScanFilter(const int clientId);

    /**
     * @brief Get device address type
     *
     * @param [in] address
     * @return @c address type
     */
    int GetDeviceAddrType(const std::string &address) const;

    /**
     * @brief Get scan status
     *
     * @return @c scan status
     */
    int GetScanStatus() const;

    /**
     * @brief find scan device
     *
     * @param [in] device address
     * @return @c true: finded, otherwise false
     */
    int GetDeviceType(const std::string &address) const;

    /**
     * @brief deregister scan callback to gap
     *
     * @return @c status.
     */
    int DeregisterCallbackToGap() const;

    /**
     * @brief get device name
     *
     * @return @c name.
     */
    std::string GetDeviceName(const std::string &address) const;

    /**
     * @brief clear scan result
     *
     */
    void ClearResults() const;

private:
    /**
     * @brief Perform an active or passive scan?
     * The default is a passive scan.
     * @param [in] active If true, perform an active scan otherwise a passive scan.
     */
    void SetActiveScan(bool active) const;

    /**
     * @brief Set the interval to scan.
     * @param [in] The interval in msecs.
     */
    void SetInterval(uint16_t intervalMSecs) const;

    /**
     * @brief Start scan.
     *
     * @param [in] enable.
     * @return @c status
     */
    int SetScanEnable(bool enable) const;
    int SetExScanEnable(bool enable) const;

    /**
     * @brief Set the window to actively scan.
     * @param [in] windowMSecs How long to actively scan.
     */
    void SetWindow(uint16_t windowMSecs) const;

    /**
     * @brief Start scanning.
     * @param [in] are we continue scan (true) or we want to clear stored devices (false)
     * @return True if scan started or false if there was an error.
     */
    bool Start(bool isContinue = false) const;

    /**
     * @brief Stop scan.
     */
    void Stop() const;

    /**
     * @brief Set scan parameter.
     *
     */
    int SetScanParamToGap() const;
    int SetExScanParamToGap() const;

    /**
     * @brief scan callback result
     *
     * @param [in] advertising type.
     * @param [in] peer address.
     * @param [in] scan data length.
     * @param [in] rssi value.
     * @param [in] context.
     */
    static void AdvertisingReport(uint8_t advType, const BtAddr *peerAddr, GapAdvReportParam reportParam,
        const BtAddr *currentAddr, void *context);
    static void ExAdvertisingReport(
        uint8_t advType, const BtAddr *addr, GapExAdvReportParam reportParam, const BtAddr *currentAddr, void *context);

    void AdvertisingReportTask(
        uint8_t advType, const BtAddr &peerAddr, const std::vector<uint8_t> &data, int8_t rssi) const;
    void ExAdvertisingReportTask(uint8_t advType, const BtAddr &peerAddr, const std::vector<uint8_t> &data, int8_t rssi,
        const BtAddr &peerCurrentAddr) const;
    bool AddPeripheralDevice(uint8_t advType, const BtAddr &peerAddr, const std::vector<uint8_t> &data, int8_t rssi,
        const BlePeripheralDevice &dev) const;
    /**
     * @brief set scan parameters callback from gap
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void ScanSetParamResult(uint8_t status, void *context);
    static void ScanExSetParamResult(uint8_t status, void *context);

    /**
     * @brief set scan enable callback from gap
     *
     * @param [in] status.
     * @param [in] context.
     */
    static void ScanSetEnableResult(uint8_t status, void *context);
    static void ScanExSetEnableResult(uint8_t status, void *context);

    /**
     * @brief find scan device
     *
     * @param [in] device address
     * @return @c true: finded, otherwise false
     */
    bool FindDevice(const std::string &address, BlePeripheralDevice &dev) const;

    /**
     * @brief get scan inteval from scan mode
     *
     * @param [in] scan mode
     */
    void SetScanModeDuration(int scanMode, int type) const;

    /**
     * @brief set low power duration
     *
     * @param [in] type callback type
     */
    void SetLowPowerDuration(int type) const;

    /**
     * @brief set balanced duration
     *
     * @param [in] type callback type
     */
    void SetBalancedDuration(int type) const;

    /**
     * @brief set low latency duration
     *
     * @param [in] type callback type
     */
    void SetLowLatencyDuration(int type) const;

    /**
     * @brief set duty cycle duration
     *
     * @param [in] type callback type
     */
    void SetDutyCycle2Duration(int type) const;

    /**
     * @brief set duty cycle duration
     *
     * @param [in] type callback type
     */
    void SetDutyCycle10Duration(int type) const;

    /**
     * @brief set duty cycle duration
     *
     * @param [in] type callback type
     */
    void SetDutyCycle25Duration(int type) const;

    /**
     * @brief set duty cycle duration
     *
     * @param [in] type callback type
     */
    void SetDutyCycle100Duration(int type) const;

    /**
     *  @brief Gap scan event
     *
     *  @param [in] event gap event.
     *  @param [in] status gap callback status.
     */
    void HandleGapEvent(const BLE_GAP_CB_EVENT &event, int status) const;

    /**
     *  @brief Gap extend scan event
     *
     *  @param [in] event gap event.
     *  @param [in] status gap callback status.
     */
    void HandleGapExScanEvent(const BLE_GAP_CB_EVENT &event, int status) const;

    /**
     * @brief timer call back
     *
     */
    static void TimerCallback(void *context);

    /**
     * @brief Set scan enable callback from gap
     *
     * @param [in] status.
     */
    void ScanSetEnableResultTask(uint8_t status) const;
    void ScanExSetEnableResultTask(uint8_t status) const;

    static bool CheckBleScanMode(uint8_t falg);
    void GapScanParamSetCompleteEvt(int status) const;
    void GapScanResultEvt() const;
    void GapScanDelayReportResultEvt() const;
    void GapScanStartCompleteEvt(int status) const;
    void GapScanStopCompleteEvt(int status) const;
    void GapScanResolvingCompletEvt(int status, bool isStart) const;

    void GapExScanParamSetCompleteEvt(int status) const;
    void GapExScanResultEvt() const;
    void GapExScanDelayReportResultEvt() const;
    void GapExScanStartCompleteEvt(int status) const;
    void GapExScanStopCompleteEvt(int status) const;
    void GapExScanResolvingCompletEvt(int status, bool isStart) const;
    bool SetScanParamOrExScanParamToGap() const;
    bool SetLegacyScanParamToGap() const;
    bool SetExtendScanParamToGap() const;

    static void DirectedAdvertisingReport(uint8_t advType, const BtAddr *addr, GapDirectedAdvReportParam reportParam,
        const BtAddr *currentAddr, void *context);
    static void ScanTimeoutEvent(void *context);
    bool ExtractIncompleteData(uint8_t advType, const std::string &advertisedAddress, const std::vector<uint8_t> &data,
        std::vector<uint8_t> &completeData) const;

    static void AddBleScanFilterResult(uint8_t result, void *context);
    static void StartBleScanFilterResult(uint8_t result, void *context);
    static void DeleteBleScanFilterResult(uint8_t result, void *context);
    static void StopBleScanFilterResult(uint8_t result, void *context);
    void HandleAddBleScanFilterResult(uint8_t result);
    void HandleDeleteBleScanFilterResult(uint8_t result);
    void HandleStartBleScanFilterResult(uint8_t result);
    void HandleStopBleScanFilterResult(uint8_t result);
    void LoadBleScanFilterLib();
    void UnloadBleScanFilterLib();
    bool CheckScanFilterConfig(const std::vector<BleScanFilterImpl> &filters);
    void PushFilterToWaitList(BleScanFilterImpl filter, int clientId, uint8_t action);
    void PushStartOrStopAction(const int clientId, uint8_t action);
    void AddBleScanFilter(BleScanFilterImpl filter);
    void DeleteBleScanFilter(BleScanFilterImpl filter);
    void StartBleScanFilter();
    void StopBleScanFilter();
    void HandleWaitFilters();
    void DoFilterStatusBad();
    void TryConfigScanFilter(int clientId);
    void BleScanFilterParamAddDeviceAddress(BleScanFilterParam &filterParam, BleScanFilterImpl filter);
    void BleScanFilterParamAddName(BleScanFilterParam &filterParam, BleScanFilterImpl filter);
    void BleScanFilterParamAddServiceUuid(BleScanFilterParam &filterParam, BleScanFilterImpl filter);
    void BleScanFilterParamAddSolicitationUuid(BleScanFilterParam &filterParam, BleScanFilterImpl filter);
    void BleScanFilterParamAddServiceData(BleScanFilterParam &filterParam, BleScanFilterImpl filter);
    void BleScanFilterParamAddManufactureData(BleScanFilterParam &filterParam, BleScanFilterImpl filter);

    /// scan callback
    IBleCentralManagerCallback *centralManagerCallbacks_ = nullptr;
    bool isDuplicates_ = false;
    IAdapterBle *bleAdapter_ = nullptr;
    /// The dispatcher that is used to switch to the thread.
    utility::Dispatcher *dispatcher_ = nullptr;
    std::map<std::string, std::vector<uint8_t>> incompleteData_{};
    IBleScanFilter* bleScanFilter_ = nullptr;
    void *bleScanFilterLib_ = nullptr;

    /// filter action
    static const uint8_t FILTER_ACTION_ADD = 0x00;
    static const uint8_t FILTER_ACTION_DELETE = 0x01;
    static const uint8_t FILTER_ACTION_START = 0x02;
    static const uint8_t FILTER_ACTION_STOP = 0x03;

    BT_DISALLOW_COPY_AND_ASSIGN(BleCentralManagerImpl);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_CENTRAL_MANAGER_IMPL_H
