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

#ifndef BLE_SECURITY_H
#define BLE_SECURITY_H

#include <map>

#include "base_observer_list.h"
#include "ble_defs.h"
#include "dispatcher.h"
#include "gap_le_if.h"
#include "interface_adapter_ble.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
/*
 * @brief BLE filter.
 */
class BleSecurity {
public:
    /**
     * @brief Constructor.
     */
    explicit BleSecurity(
        IAdapterBle &bleAdapter, utility::Dispatcher &dispatch, BaseObserverList<IAdapterBleObserver> &observer);

    /**
     * @brief Destructor.
     */
    virtual ~BleSecurity();

    static bool StartPair(const RawAddress &device, uint8_t peerAddrType = BT_PUBLIC_DEVICE_ADDRESS);
    int SetDevicePasskey(const RawAddress &device, int passkey, int accept) const;
    int SetUserConfirm(const RawAddress &device, int accept) const;
    int GapLeRequestSecurity(uint16_t connectionHandle, const BtAddr &addr, uint8_t role);
    int CancelPairing(const RawAddress &device) const;
    bool PairRequestReply(const RawAddress &addr, int addrType, bool accept) const;

    /**
     * @brief Register avertising callback to gap
     *
     * @return @c status.
     */
    int RegisterCallbackToGap();

    /**
     * @brief Deregister avertising callback to gap
     *
     * @return @c status.
     */
    int DeregisterCallbackToGap() const;

private:
    // gap callback
    static void EncryptionComplete(uint8_t status, const BtAddr *peerAddr, void *context);
    static void LeLocalEncryptionKeyReqEvent(const BtAddr *addr, uint64_t rand, uint16_t ediv, void *context);
    static void LeRemoteEncryptionKeyReqEvent(const BtAddr *addr, void *context);
    static void LeSignCounterChangeNotification(
        const BtAddr *addr, GAP_SignCounterType type, uint32_t counter, void *context);
    static void GapRequestSigningAlgorithmInfo(const BtAddr *addr, void *context);

    static void LePairFeatureReq(const BtAddr *peerAddr, bool localPair, void *context);
    static void LePairFeatureInd(const BtAddr *addr, GapLePairFeature remoteFrature, void *context);
    static void LePairMethodNotify(const BtAddr *addr, uint8_t pairMethod, void *context);
    static void LePairKeyPressNotification(const BtAddr *addr, uint8_t pressType, void *context);
    static void LePairPassKeyReq(const BtAddr *addr, void *context);
    static void LePairPassKeyNotification(const BtAddr *addr, uint32_t number, void *context);
    static void LePairOobReq(const BtAddr *addr, void *context);
    static void LePairScOobReq(const BtAddr *addr, void *context);
    static void LePairScUserConfirmReq(const BtAddr *addr, uint32_t number, void *context);
    static void LePairComplete(const BtAddr *addr, uint8_t result, uint8_t keyType, void *context);
    static void LePairKeyNotify(const BtAddr *addr, LePairedKeys leKeys, void *context);
    static void GapLeRequestSecurityResult(
        const BtAddr *addr, uint8_t result, GAP_LeSecurityStatus status, void *context);
    /**
     *  @brief Internal status
     *
     *  @param [in] event gap event.
     *  @param [in] status gap callback status.
     */
    void HandleGapEvent(const BLE_GAP_CB_EVENT &event, const BleGapCallbackParam &param);
    bool SavePairKeyNotify(const BleGapCallbackParam &param) const;
    static bool SaveLocalPairKey(const RawAddress &addr, const BleGapCallbackParam &param);
    static bool SavePeerPairKey(const RawAddress &addr, const BleGapCallbackParam &param);
    bool GapEncryptionComplete(const BleGapCallbackParam &param) const;
    bool GapLeLocalEncryptionKeyReqEvent(const BleGapCallbackParam &param) const;
    bool GapLeRemoteEncryptionKeyReqEvent(const BleGapCallbackParam &param) const;
    bool GapLeSignCounterChangeNotification(const BleGapCallbackParam &param) const;
    bool GapRequestSigningAlgorithmInfoEvt(const BleGapCallbackParam &param) const;
    bool GapLePairFeatureReq(const BleGapCallbackParam &param) const;
    bool GapLePairFeatureInd(const BleGapCallbackParam &param) const;
    bool GapLePairMethodNotify(const BleGapCallbackParam &param) const;
    bool GapLePairKeyPressNotification(const BleGapCallbackParam &param) const;
    bool GapLePairPassKeyReq(const BleGapCallbackParam &param) const;
    bool GapLePairPassKeyNotification(const BleGapCallbackParam &param) const;
    bool GapLePairOobReq(const BleGapCallbackParam &param) const;
    bool GapLePairScOobReq(const BleGapCallbackParam &param) const;
    bool GapLePairScUserConfirmReq(const BleGapCallbackParam &param) const;
    bool GapLePairComplete(const BleGapCallbackParam &param) const;
    bool GapLePairKeyNotify(const BleGapCallbackParam &param) const;
    bool GapLeRequestSecurityResultEvt(const BleGapCallbackParam &param) const;
    static bool LePairFeatureReq(const BleGapCallbackParam &param);

    void InitGapEventFuncTable() const;

    typedef bool (BleSecurity::*func)(const BleGapCallbackParam &param) const;

    IAdapterBle *bleAdapter_ = nullptr;
    /// The dispatcher that is used to switch to the thread.
    utility::Dispatcher *dispatcher_ = nullptr;
    BaseObserverList<IAdapterBleObserver> *baseCallback_ = nullptr;

    BT_DISALLOW_COPY_AND_ASSIGN(BleSecurity);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BLE_SECURITY_H