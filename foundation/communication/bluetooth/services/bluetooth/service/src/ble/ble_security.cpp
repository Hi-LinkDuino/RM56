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

#include "ble_security.h"

#include <iostream>

#include "ble_adapter.h"
#include "ble_config.h"
#include "ble_properties.h"
#include "ble_utils.h"

#include "btm.h"
#include "compat.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct BleSecurity::impl {
public:
    uint8_t pairMethod_ = 0;
    std::map<int, func> funcMap_ {};
};
BleSecurity::BleSecurity(
    IAdapterBle &bleAdapter, utility::Dispatcher &dispatch, BaseObserverList<IAdapterBleObserver> &observer)
    : bleAdapter_(&bleAdapter),
      dispatcher_(&dispatch),
      baseCallback_(&observer),
      pimpl(std::make_unique<BleSecurity::impl>())
{
    InitGapEventFuncTable();
    int ret = RegisterCallbackToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s", __func__);
    }
}

BleSecurity::~BleSecurity()
{
    pimpl->funcMap_.clear();
    int ret = DeregisterCallbackToGap();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s", __func__);
    }
}

void BleSecurity::InitGapEventFuncTable() const
{
    pimpl->funcMap_.clear();
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_ENCRYPTION_COMPLETE_EVT, &BleSecurity::GapEncryptionComplete));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_LOCAL_ENCRYPTION_KEY_REQ_EVT, &BleSecurity::GapLeLocalEncryptionKeyReqEvent));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_REMOTE_ENCRYPTION_KEY_REQ_EVT, &BleSecurity::GapLeRemoteEncryptionKeyReqEvent));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_SIGN_COUNTER_CHANGE_NOTIF_EVT, &BleSecurity::GapLeSignCounterChangeNotification));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_REQ_SIGNING_ALGORITHM_INFO_EVT, &BleSecurity::GapRequestSigningAlgorithmInfoEvt));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_FEATURE_REQ_EVT, &BleSecurity::GapLePairFeatureReq));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_FEATURE_IND_EVT, &BleSecurity::GapLePairFeatureInd));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_METHOD_NOTI_EVT, &BleSecurity::GapLePairMethodNotify));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_PAIR_KEY_PRESS_NOTI_EVT, &BleSecurity::GapLePairKeyPressNotification));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_PASSKEY_REQ_EVT, &BleSecurity::GapLePairPassKeyReq));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_PAIR_PASSKEY_NOTI_EVT, &BleSecurity::GapLePairPassKeyNotification));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_OOB_REQ_EVT, &BleSecurity::GapLePairOobReq));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_SC_OOB_REQ_EVT, &BleSecurity::GapLePairScOobReq));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_PAIR_SC_USER_CONFIRM_REQ_EVT, &BleSecurity::GapLePairScUserConfirmReq));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_COMELETE_EVT, &BleSecurity::GapLePairComplete));
    pimpl->funcMap_.insert(std::make_pair(BLE_GAP_LE_PAIR_KEY_NOTI_EVT, &BleSecurity::GapLePairKeyNotify));
    pimpl->funcMap_.insert(
        std::make_pair(BLE_GAP_LE_REQUEST_SECURITY_RESULT, &BleSecurity::GapLeRequestSecurityResultEvt));
}

void BleSecurity::EncryptionComplete(uint8_t status, const BtAddr *peerAddr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    gapCallbackParam.encryptionComplete_.status = status;
    (void)memcpy_s(&gapCallbackParam.encryptionComplete_.peerAddr, sizeof(BtAddr), peerAddr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_ENCRYPTION_COMPLETE_EVT, gapCallbackParam));
}

void BleSecurity::LeLocalEncryptionKeyReqEvent(const BtAddr *addr, uint64_t rand, uint16_t ediv, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.leLocalEncryptionKeyReqEvent_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.leLocalEncryptionKeyReqEvent_.ediv = ediv;
    gapCallbackParam.leLocalEncryptionKeyReqEvent_.rand = rand;
    bleSecurity->dispatcher_->PostTask(std::bind(
        &BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_LOCAL_ENCRYPTION_KEY_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LeRemoteEncryptionKeyReqEvent(const BtAddr *addr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.leRemoteEncryptionKeyReqEvent_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(std::bind(
        &BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_REMOTE_ENCRYPTION_KEY_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LeSignCounterChangeNotification(
    const BtAddr *addr, GAP_SignCounterType type, uint32_t counter, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.leSignCounterChangeNotification_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.leSignCounterChangeNotification_.type = type;
    gapCallbackParam.leSignCounterChangeNotification_.counter = counter;
    bleSecurity->dispatcher_->PostTask(std::bind(
        &BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_SIGN_COUNTER_CHANGE_NOTIF_EVT, gapCallbackParam));
}

void BleSecurity::LePairFeatureReq(const BtAddr *peerAddr, bool localPair, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairFeatureReq_.peerAddr, sizeof(BtAddr), peerAddr, sizeof(BtAddr));
    gapCallbackParam.lePairFeatureReq_.localPair = localPair;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_FEATURE_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LePairFeatureInd(const BtAddr *addr, GapLePairFeature remoteFrature, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairFeatureInd_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairFeatureInd_.remoteFrature = remoteFrature;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_FEATURE_IND_EVT, gapCallbackParam));
}

void BleSecurity::LePairMethodNotify(const BtAddr *addr, uint8_t pairMethod, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairMethodNotify_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairMethodNotify_.pairMethod = pairMethod;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_METHOD_NOTI_EVT, gapCallbackParam));
}

void BleSecurity::LePairKeyPressNotification(const BtAddr *addr, uint8_t pressType, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairKeyPressNotification_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairKeyPressNotification_.pressType = pressType;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_KEY_PRESS_NOTI_EVT, gapCallbackParam));
}

void BleSecurity::LePairPassKeyReq(const BtAddr *addr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairPassKeyReq_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_PASSKEY_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LePairOobReq(const BtAddr *addr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairOobReq_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_OOB_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LePairScOobReq(const BtAddr *addr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairScOob_req_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_SC_OOB_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LePairScUserConfirmReq(const BtAddr *addr, uint32_t number, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairScUserConfirmReq_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairScUserConfirmReq_.number = number;
    bleSecurity->dispatcher_->PostTask(std::bind(
        &BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_SC_USER_CONFIRM_REQ_EVT, gapCallbackParam));
}

void BleSecurity::LePairComplete(const BtAddr *addr, uint8_t result, uint8_t keyType, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairComplete_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairComplete_.result = result;
    gapCallbackParam.lePairComplete_.keyType = keyType;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_COMELETE_EVT, gapCallbackParam));
}

void BleSecurity::LePairKeyNotify(const BtAddr *addr, LePairedKeys leKeys, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);
    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }
    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairKeyNotify_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    if (leKeys.remoteEncKey != nullptr) {
        (void)memcpy_s(
            &gapCallbackParam.lePairKeyNotify_.remoteEncKey, sizeof(LeEncKey), leKeys.remoteEncKey, sizeof(LeEncKey));
        gapCallbackParam.lePairKeyNotify_.hasRemoteEncKey = true;
    } else {
        gapCallbackParam.lePairKeyNotify_.hasRemoteEncKey = false;
    }
    if (leKeys.remoteIdKey != nullptr) {
        (void)memcpy_s(
            &gapCallbackParam.lePairKeyNotify_.remoteIdKey, sizeof(LeIdKey), leKeys.remoteIdKey, sizeof(LeIdKey));
        gapCallbackParam.lePairKeyNotify_.hasRemoteIdKey = true;
    } else {
        gapCallbackParam.lePairKeyNotify_.hasRemoteIdKey = false;
    }
    if (leKeys.remoteSignKey != nullptr) {
        (void)memcpy_s(&gapCallbackParam.lePairKeyNotify_.remoteSignKey,
            sizeof(LeSignKey),
            leKeys.remoteSignKey,
            sizeof(LeSignKey));
        gapCallbackParam.lePairKeyNotify_.hasRemoteSignKey = true;
    } else {
        gapCallbackParam.lePairKeyNotify_.hasRemoteSignKey = false;
    }
    if (leKeys.localEncKey != nullptr) {
        (void)memcpy_s(
            &gapCallbackParam.lePairKeyNotify_.localEncKey, sizeof(LeEncKey), leKeys.localEncKey, sizeof(LeEncKey));
        gapCallbackParam.lePairKeyNotify_.hasLocalEncKey = true;
    } else {
        gapCallbackParam.lePairKeyNotify_.hasLocalEncKey = false;
    }
    if (leKeys.localSignKey != nullptr) {
        (void)memcpy_s(
            &gapCallbackParam.lePairKeyNotify_.localSignKey, sizeof(LeSignKey), leKeys.localSignKey, sizeof(LeSignKey));
        gapCallbackParam.lePairKeyNotify_.hasLocalSignKey = true;
    } else {
        gapCallbackParam.lePairKeyNotify_.hasLocalSignKey = false;
    }

    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_KEY_NOTI_EVT, gapCallbackParam));
}

void BleSecurity::GapRequestSigningAlgorithmInfo(const BtAddr *addr, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.gapRequestSigningAlgorithmInfo_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    bleSecurity->dispatcher_->PostTask(std::bind(
        &BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_REQ_SIGNING_ALGORITHM_INFO_EVT, gapCallbackParam));
}

void BleSecurity::LePairPassKeyNotification(const BtAddr *addr, uint32_t number, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.lePairPassKeyNotification_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.lePairPassKeyNotification_.number = number;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_PAIR_PASSKEY_NOTI_EVT, gapCallbackParam));
}

void BleSecurity::GapLeRequestSecurityResult(
    const BtAddr *addr, uint8_t result, GAP_LeSecurityStatus status, void *context)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BleSecurity *bleSecurity = static_cast<BleSecurity *>(context);
    if (bleSecurity == nullptr) {
        return;
    }

    BleGapCallbackParam gapCallbackParam;
    (void)memset_s(&gapCallbackParam, sizeof(gapCallbackParam), 0x00, sizeof(gapCallbackParam));
    (void)memcpy_s(&gapCallbackParam.leRequestSecurityResult_.addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    gapCallbackParam.leRequestSecurityResult_.result = result;
    gapCallbackParam.leRequestSecurityResult_.status = status;
    bleSecurity->dispatcher_->PostTask(
        std::bind(&BleSecurity::HandleGapEvent, bleSecurity, BLE_GAP_LE_REQUEST_SECURITY_RESULT, gapCallbackParam));
}

bool BleSecurity::SavePairKeyNotify(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    bool ret = false;
    RawAddress addr = RawAddress::ConvertToString(param.lePairKeyNotify_.addr.addr);

    if (bleAdapter_ != nullptr) {
        ret = (static_cast<BleAdapter *>(bleAdapter_))->IsRemovePairedDevice(addr);
        if (ret) {
            return ret;
        }
    }

    /// before saving key, delete the same identity addr device
    std::vector<std::string> pairedAddrList = BleConfig::GetInstance().GetPairedAddrList();
    for (auto address : pairedAddrList) {
        RawAddress rawAddr(address);
        if ((!INVALID_MAC_ADDRESS.compare(rawAddr.GetAddress())) || (rawAddr.GetAddress().empty())) {
            continue;
        }
        std::string peerIdentityAddr = BleConfig::GetInstance().GetPeerIdentityAddr(rawAddr.GetAddress());
        if (param.lePairKeyNotify_.hasRemoteIdKey) {
            RawAddress peerAddr = RawAddress::ConvertToString(param.lePairKeyNotify_.remoteIdKey.identityAddr.addr);
            if (peerIdentityAddr.compare(peerAddr.GetAddress()) == 0) {
                BleConfig::GetInstance().RemovePairedDevice(rawAddr.GetAddress());
            }
        }
    }
    ret = SaveLocalPairKey(addr, param);
    ret &= SavePeerPairKey(addr, param);
    return ret;
}

bool BleSecurity::SaveLocalPairKey(const RawAddress &addr, const BleGapCallbackParam &param)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    bool ret = true;
    // local
    if (param.lePairKeyNotify_.hasLocalEncKey) {
        ret &= BleConfig::GetInstance().SetLocalEdivRand(addr.GetAddress(),
            std::to_string(param.lePairKeyNotify_.localEncKey.ediv),
            std::to_string(param.lePairKeyNotify_.localEncKey.rand));
        std::vector<uint8_t> ltk(
            param.lePairKeyNotify_.localEncKey.ltk, param.lePairKeyNotify_.localEncKey.ltk + GAP_LTK_SIZE);
        ret &= BleConfig::GetInstance().SetLocalLtk(addr.GetAddress(), BleUtils::ConvertIntToHexString(ltk));
        ret &= BleConfig::GetInstance().SetLocalKeySize(
            addr.GetAddress(), std::to_string(param.lePairKeyNotify_.localEncKey.keySize));
    }

    if (param.lePairKeyNotify_.hasLocalSignKey) {
        std::vector<uint8_t> csrk(
            param.lePairKeyNotify_.localSignKey.csrk, param.lePairKeyNotify_.localSignKey.csrk + GAP_LTK_SIZE);
        ret &= BleConfig::GetInstance().SetLocalCsrk(addr.GetAddress(), BleUtils::ConvertIntToHexString(csrk));
        ret &= BleConfig::GetInstance().SetLocalSignCounter(
            addr.GetAddress(), param.lePairKeyNotify_.localSignKey.counter);
    }
    return ret;
}

bool BleSecurity::SavePeerPairKey(const RawAddress &addr, const BleGapCallbackParam &param)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    bool ret = true;
    // peer
    if (param.lePairKeyNotify_.hasRemoteEncKey) {
        ret &= BleConfig::GetInstance().SetPeerEdivRand(addr.GetAddress(),
            std::to_string(param.lePairKeyNotify_.remoteEncKey.ediv),
            std::to_string(param.lePairKeyNotify_.remoteEncKey.rand));
        std::vector<uint8_t> ltk(
            param.lePairKeyNotify_.remoteEncKey.ltk, param.lePairKeyNotify_.remoteEncKey.ltk + GAP_LTK_SIZE);
        ret &= BleConfig::GetInstance().SetPeerLtk(addr.GetAddress(), BleUtils::ConvertIntToHexString(ltk));
        ret &= BleConfig::GetInstance().SetPeerKeySize(
            addr.GetAddress(), std::to_string(param.lePairKeyNotify_.remoteEncKey.keySize));
    }

    if (param.lePairKeyNotify_.hasRemoteIdKey) {
        RawAddress peerAddr = RawAddress::ConvertToString(param.lePairKeyNotify_.remoteIdKey.identityAddr.addr);
        ret &= BleConfig::GetInstance().SetPeerIdentityAddr(
            addr.GetAddress(), param.lePairKeyNotify_.remoteIdKey.identityAddr.type, peerAddr.GetAddress());
        std::vector<uint8_t> irk(
            param.lePairKeyNotify_.remoteIdKey.irk, param.lePairKeyNotify_.remoteIdKey.irk + GAP_LTK_SIZE);
        ret &= BleConfig::GetInstance().SetPeerIrk(addr.GetAddress(), BleUtils::ConvertIntToHexString(irk));
    }

    if (param.lePairKeyNotify_.hasRemoteSignKey) {
        std::vector<uint8_t> csrk(
            param.lePairKeyNotify_.remoteSignKey.csrk, param.lePairKeyNotify_.remoteSignKey.csrk + GAP_LTK_SIZE);
        ret &= BleConfig::GetInstance().SetPeerCsrk(addr.GetAddress(), BleUtils::ConvertIntToHexString(csrk));
        ret &= BleConfig::GetInstance().SetPeerSignCounter(
            addr.GetAddress(), param.lePairKeyNotify_.remoteSignKey.counter);
    }
    return ret;
}

bool BleSecurity::GapEncryptionComplete(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    if (param.encryptionComplete_.status != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble encryption failed!");
        return false;
    } else {
        RawAddress addr = RawAddress::ConvertToString(param.encryptionComplete_.peerAddr.addr);
        if (bleAdapter_ != nullptr) {
            (static_cast<BleAdapter *>(bleAdapter_))->EncryptionComplete(addr);
        }
        LOG_DEBUG("[BleSecurity] %{public}s:%{public}s", __func__, "Ble encryption success!");
    }
    return true;
}

bool BleSecurity::GapLeLocalEncryptionKeyReqEvent(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.leLocalEncryptionKeyReqEvent_.addr.addr);
    int accept = GAP_NOT_ACCEPT;
    LeEncKey encKey;
    (void)memset_s(&encKey, sizeof(encKey), 0x00, sizeof(encKey));
    std::string ltk = BleConfig::GetInstance().GetLocalLtk(addr.GetAddress());
    std::string rand = BleConfig::GetInstance().GetLocalRand(addr.GetAddress());
    std::string ediv = BleConfig::GetInstance().GetLocalEdiv(addr.GetAddress());
    if ((rand.compare(std::to_string(param.leLocalEncryptionKeyReqEvent_.rand)) == 0) &&
        (ediv.compare(std::to_string(param.leLocalEncryptionKeyReqEvent_.ediv)) == 0) && (!ltk.empty())) {
        accept = GAP_ACCEPT;
        std::vector<uint8_t> vec;
        BleUtils::ConvertHexStringToInt(ltk, vec);
        (void)memcpy_s(encKey.ltk, GAP_CSRK_SIZE, &vec[0], vec.size());
        encKey.rand = std::stoull(rand);
        encKey.ediv = std::stoull(ediv);
    }

    int ret = GAPIF_LeLocalEncryptionKeyRsp(&param.leLocalEncryptionKeyReqEvent_.addr, accept, encKey, 1);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble remote encryption key rsp failed!");
    }
    (void)memset_s(&encKey, sizeof(encKey), 0x00, sizeof(encKey));
    return ret;
}

bool BleSecurity::GapLeRemoteEncryptionKeyReqEvent(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.leRemoteEncryptionKeyReqEvent_.addr.addr);
    int accept = GAP_NOT_ACCEPT;
    LeEncKey encKey;
    (void)memset_s(&encKey, sizeof(encKey), 0x00, sizeof(encKey));
    std::string ltk = BleConfig::GetInstance().GetPeerLtk(addr.GetAddress());
    std::string rand = BleConfig::GetInstance().GetPeerRand(addr.GetAddress());
    std::string ediv = BleConfig::GetInstance().GetPeerEdiv(addr.GetAddress());
    if ((!rand.empty()) && (!ediv.empty()) && (!ltk.empty())) {
        accept = GAP_ACCEPT;
        std::vector<uint8_t> vec;
        BleUtils::ConvertHexStringToInt(ltk, vec);
        (void)memcpy_s(encKey.ltk, GAP_CSRK_SIZE, &vec[0], vec.size());
        encKey.rand = std::stoull(rand);
        encKey.ediv = std::stoull(ediv);
    }

    int ret = GAPIF_LeRemoteEncryptionKeyRsp(&param.leRemoteEncryptionKeyReqEvent_.addr, accept, encKey, 1);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble remote encryption key rsp failed!");
    }
    (void)memset_s(&encKey, sizeof(encKey), 0x00, sizeof(encKey));
    return ret;
}

bool BleSecurity::GapLeSignCounterChangeNotification(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.leSignCounterChangeNotification_.addr.addr);

    if (param.leSignCounterChangeNotification_.type == LOCAL_SIGN_COUNTER) {
        BleConfig::GetInstance().SetLocalSignCounter(addr.GetAddress(), param.leSignCounterChangeNotification_.counter);
    } else {
        BleConfig::GetInstance().SetPeerSignCounter(addr.GetAddress(), param.leSignCounterChangeNotification_.counter);
    }
    return true;
}

bool BleSecurity::GapRequestSigningAlgorithmInfoEvt(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.gapRequestSigningAlgorithmInfo_.addr.addr);
    int accept = GAP_NOT_ACCEPT;
    GapSigningAlgorithmInfo info;
    (void)memset_s(&info, sizeof(info), 0x00, sizeof(info));
    LeSignKey localKey;
    (void)memset_s(&localKey, sizeof(localKey), 0x00, sizeof(localKey));

    if (!BleConfig::GetInstance().GetLocalCsrk(addr.GetAddress()).empty()) {
        localKey.counter = BleConfig::GetInstance().GetLocalSignCounter(addr.GetAddress());
        std::vector<uint8_t> vec;
        BleUtils::ConvertHexStringToInt(BleConfig::GetInstance().GetLocalCsrk(addr.GetAddress()), vec);
        (void)memcpy_s(localKey.csrk, GAP_CSRK_SIZE, &vec[0], vec.size());
        info.localKey = &localKey;
        accept = GAP_ACCEPT;
    }

    LeSignKey remoteKey;
    if (!BleConfig::GetInstance().GetPeerCsrk(addr.GetAddress()).empty()) {
        remoteKey.counter = BleConfig::GetInstance().GetPeerSignCounter(addr.GetAddress());
        std::vector<uint8_t> vec;
        BleUtils::ConvertHexStringToInt(BleConfig::GetInstance().GetPeerCsrk(addr.GetAddress()), vec);
        (void)memcpy_s(remoteKey.csrk, GAP_CSRK_SIZE, &vec[0], vec.size());
        info.remoteKey = &remoteKey;
        accept &= GAP_ACCEPT;
    }

    int ret = GAPIF_RequestSigningAlgorithmInfoRsp(&param.gapRequestSigningAlgorithmInfo_.addr, accept, info);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble request signing algorithm info rsp failed!");
    }
    return ret;
}

bool BleSecurity::LePairFeatureReq(const BleGapCallbackParam &param)
{
    RawAddress addr = RawAddress::ConvertToString(param.lePairFeatureReq_.peerAddr.addr);
    GapLePairFeature feature;
    (void)memset_s(&feature, sizeof(feature), 0x00, sizeof(feature));
    feature.ioCapability = BleConfig::GetInstance().GetIoCapability();
    feature.maxEncKeySize = GAP_LINKKEY_SIZE;

    if (BleConfig::GetInstance().GetBleSecurity()) {
        feature.authReq = GAP_LE_AUTH_REQ_BONDING | GAP_LE_AUTH_REQ_BIT_MITM | GAP_LE_AUTH_REQ_BIT_SC;
    } else {
        feature.authReq = GAP_LE_AUTH_REQ_BONDING | GAP_LE_AUTH_REQ_BIT_MITM;
    }

    /// Compat check
    GAP_LeSecMode1Level level1 = static_cast<GAP_LeSecMode1Level>(BleConfig::GetInstance().GetBleModel1Level());
    if ((level1 != LE_MODE_1_LEVEL_4) &&
        (Compat::CompatCheck(CompatType::COMPAT_DISABLE_BLE_SECURE_CONNECTIONS, addr.GetAddress()))) {
        BtmLocalVersionInformation version;
        (void)memset_s(&version, sizeof(version), 0x00, sizeof(version));
        if ((BTM_GetLocalVersionInformation(&version) == BT_NO_ERROR) &&
            (version.hciVersion >= BLUETOOTH_CORE_SPECIFICATION_4_2)) {
            feature.authReq &= ~GAP_LE_AUTH_REQ_BIT_SC;
        }
    }

    feature.respKeyDis = GAP_LE_KEY_DIST_ENC_KEY | GAP_LE_KEY_DIST_ID_KEY | GAP_LE_KEY_DIST_SIGN_KEY;
    feature.initKeyDis = GAP_LE_KEY_DIST_ENC_KEY | GAP_LE_KEY_DIST_ID_KEY | GAP_LE_KEY_DIST_SIGN_KEY;

    int ret = GAPIF_LePairFeatureRsp(&param.lePairFeatureReq_.peerAddr, feature);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble pair feature rsp failed!");
    }
    return ret == BT_NO_ERROR;
}

bool BleSecurity::PairRequestReply(const RawAddress &addr, int addrType, bool accept) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    if (accept) {
        BleGapCallbackParam param;
        (void)memset_s(&param, sizeof(param), 0x00, sizeof(param));
        addr.ConvertToUint8(param.lePairFeatureReq_.peerAddr.addr);
        param.lePairFeatureReq_.peerAddr.type = addrType;
        return LePairFeatureReq(param);
    } else {
        return CancelPairing(addr);
    }
}

bool BleSecurity::GapLePairFeatureReq(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    if (param.lePairFeatureReq_.localPair) {
        return LePairFeatureReq(param);
    } else {
        if (baseCallback_ != nullptr) {
            RawAddress addr = RawAddress::ConvertToString(param.lePairFeatureReq_.peerAddr.addr);
            if (bleAdapter_ != nullptr) {
                (static_cast<BleAdapter *>(bleAdapter_))->LePairingStatus(addr);
            }
            baseCallback_->ForEach(
                [addr](IAdapterBleObserver &observer) { observer.OnPairRequested(BTTransport::ADAPTER_BLE, addr); });
            return true;
        }
        return false;
    }
}

bool BleSecurity::GapLePairFeatureInd(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairFeatureInd_.addr.addr);
    BleConfig::GetInstance().SetPeerDeviceIoCapability(
        addr.GetAddress(), param.lePairFeatureInd_.remoteFrature.ioCapability);
    return true;
}

bool BleSecurity::GapLePairMethodNotify(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    pimpl->pairMethod_ = param.lePairMethodNotify_.pairMethod;
    return true;
}

bool BleSecurity::GapLePairKeyPressNotification(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    return true;
}

bool BleSecurity::GapLePairPassKeyReq(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairPassKeyReq_.addr.addr);
    uint8_t pairMethod = pimpl->pairMethod_;
    if (baseCallback_ != nullptr) {
        baseCallback_->ForEach([addr, pairMethod](IAdapterBleObserver &observer) {
            observer.OnPairConfirmed(BTTransport::ADAPTER_BLE, addr, pairMethod, 0);
        });
    }
    return true;
}

bool BleSecurity::GapLePairPassKeyNotification(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairPassKeyNotification_.addr.addr);
    uint8_t pairMethod = pimpl->pairMethod_;
    uint32_t number = param.lePairPassKeyNotification_.number;
    if (baseCallback_ != nullptr) {
        baseCallback_->ForEach([addr, pairMethod, number](IAdapterBleObserver &observer) {
            observer.OnPairConfirmed(BTTransport::ADAPTER_BLE, addr, pairMethod, number);
        });
    }
    return true;
}

bool BleSecurity::GapLePairOobReq(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairOobReq_.addr.addr);
    uint8_t pairMethod = pimpl->pairMethod_;
    if (baseCallback_ != nullptr) {
        baseCallback_->ForEach([addr, pairMethod](IAdapterBleObserver &observer) {
            observer.OnPairConfirmed(BTTransport::ADAPTER_BLE, addr, pairMethod, 0);
        });
    }
    return true;
}

bool BleSecurity::GapLePairScOobReq(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairScOob_req_.addr.addr);
    uint8_t pairMethod = pimpl->pairMethod_;
    if (baseCallback_ != nullptr) {
        baseCallback_->ForEach([addr, pairMethod](IAdapterBleObserver &observer) {
            observer.OnPairConfirmed(BTTransport::ADAPTER_BLE, addr, pairMethod, 0);
        });
    }
    return true;
}

bool BleSecurity::GapLePairScUserConfirmReq(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairScUserConfirmReq_.addr.addr);
    uint8_t pairMethod = pimpl->pairMethod_;
    uint32_t number = param.lePairScUserConfirmReq_.number;
    if (baseCallback_ != nullptr) {
        baseCallback_->ForEach([addr, pairMethod, number](IAdapterBleObserver &observer) {
            observer.OnPairConfirmed(BTTransport::ADAPTER_BLE, addr, pairMethod, number);
        });
    }
    return true;
}

bool BleSecurity::GapLePairComplete(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    RawAddress addr = RawAddress::ConvertToString(param.lePairComplete_.addr.addr);

    if (param.lePairComplete_.result == BT_NO_ERROR) {
        BleConfig::GetInstance().SetPeerKeyType(addr.GetAddress(), std::to_string(param.lePairComplete_.keyType));
        BleConfig::GetInstance().Save();
    }

    if (bleAdapter_ != nullptr) {
        (static_cast<BleAdapter *>(bleAdapter_))->LePairComplete(addr, param.lePairComplete_.result);
    }

    LOG_DEBUG("[BleSecurity] %{public}s:Ble pair comelete event result = %u", __func__, param.lePairComplete_.result);
    return true;
}

bool BleSecurity::GapLePairKeyNotify(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    return SavePairKeyNotify(param);
}

bool BleSecurity::GapLeRequestSecurityResultEvt(const BleGapCallbackParam &param) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    return true;
}

int BleSecurity::GapLeRequestSecurity(uint16_t connectionHandle, const BtAddr &addr, uint8_t role)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    int ret =
        GAPIF_LeRequestSecurity(&addr, GAP_LE_AUTHENTICATED_ENCRYPTION, &BleSecurity::GapLeRequestSecurityResult, this);
    return ret;
}

void BleSecurity::HandleGapEvent(const BLE_GAP_CB_EVENT &event, const BleGapCallbackParam &param)
{
    LOG_DEBUG("[BleSecurity] %{public}s:[event no: %{public}d].", __func__, static_cast<int>(event));
    if (pimpl->funcMap_.find(event) == pimpl->funcMap_.end()) {
        LOG_ERROR("[BleSecurity] %{public}s:[Not exist event no: %{public}d].", __func__, static_cast<int>(event));
    } else {
        (this->*pimpl->funcMap_[event])(param);
    }
}

int BleSecurity::RegisterCallbackToGap()
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);
    GapLeSecurityCallback gapCallbacks_ {};
    GapLePairCallback gapPairCallbacks_ {};

    gapCallbacks_.encryptionComplete = &BleSecurity::EncryptionComplete;
    gapCallbacks_.leLocalEncryptionKeyReqEvent = &BleSecurity::LeLocalEncryptionKeyReqEvent;
    gapCallbacks_.leRemoteEncryptionKeyReqEvent = &BleSecurity::LeRemoteEncryptionKeyReqEvent;
    gapCallbacks_.leSignCounterChangeNotification = &BleSecurity::LeSignCounterChangeNotification;
    gapCallbacks_.GapRequestSigningAlgorithmInfo = &BleSecurity::GapRequestSigningAlgorithmInfo;
    int ret = GAPIF_RegisterLeSecurityCallback(&gapCallbacks_, this);

    gapPairCallbacks_.lePairFeatureReq = &BleSecurity::LePairFeatureReq;
    gapPairCallbacks_.lePairFeatureInd = &BleSecurity::LePairFeatureInd;
    gapPairCallbacks_.lePairMethodNotify = &BleSecurity::LePairMethodNotify;
    gapPairCallbacks_.lePairKeyPressNotification = &BleSecurity::LePairKeyPressNotification;
    gapPairCallbacks_.lePairPassKeyReq = &BleSecurity::LePairPassKeyReq;
    gapPairCallbacks_.lePairPassKeyNotification = &BleSecurity::LePairPassKeyNotification;
    gapPairCallbacks_.lePairOobReq = &BleSecurity::LePairOobReq;
    gapPairCallbacks_.lePairScOobReq = &BleSecurity::LePairScOobReq;
    gapPairCallbacks_.lePairScUserConfirmReq = &BleSecurity::LePairScUserConfirmReq;
    gapPairCallbacks_.lePairComplete = &BleSecurity::LePairComplete;
    gapPairCallbacks_.lePairKeyNotify = &BleSecurity::LePairKeyNotify;
    ret &= GAPIF_RegisterLePairCallback(&gapPairCallbacks_, this);
    return ret;
}

int BleSecurity::DeregisterCallbackToGap() const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    int ret = GAPIF_DeregisterLeSecurityCallback();
    ret &= GAPIF_DeregisterLePairCallback();
    return ret;
}

bool BleSecurity::StartPair(const RawAddress &device, uint8_t peerAddrType)
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    BtAddr addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, peerAddrType};
    device.ConvertToUint8(addr.addr);
    int ret = GAPIF_LePair(&addr);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:GAP_LePair failed!", __func__);
    }
    return (ret == BT_NO_ERROR);
}

int BleSecurity::SetDevicePasskey(const RawAddress &device, int passkey, int accept) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    if (bleAdapter_ == nullptr) {
        return BT_BAD_PARAM;
    }
    uint8_t peerAddrType = (uint8_t)bleAdapter_->GetPeerDeviceAddrType(device);
    BtAddr addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, peerAddrType};
    device.ConvertToUint8(addr.addr);
    int ret = GAPIF_LePairPassKeyRsp(&addr, accept, passkey);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble pair passkey rsp failed!");
    }
    return ret;
}

int BleSecurity::SetUserConfirm(const RawAddress &device, int accept) const
{
    LOG_DEBUG("[BleSecurity] %{public}s", __func__);

    if (bleAdapter_ == nullptr) {
        return BT_BAD_PARAM;
    }
    uint8_t peerAddrType = (uint8_t)bleAdapter_->GetPeerDeviceAddrType(device);
    BtAddr addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, peerAddrType};
    device.ConvertToUint8(addr.addr);
    int ret = GAPIF_LePairScUserConfirmRsp(&addr, accept);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleSecurity] %{public}s:%{public}s", __func__, "Ble pair user confirm failed!");
    }
    return ret;
}

int BleSecurity::CancelPairing(const RawAddress &device) const
{
    if (bleAdapter_ == nullptr) {
        return BT_BAD_PARAM;
    }
    uint8_t peerAddrType = (uint8_t)bleAdapter_->GetPeerDeviceAddrType(device);
    BtAddr addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, peerAddrType};
    device.ConvertToUint8(addr.addr);
    return GAPIF_LeCancelPair(&addr);
}
}  // namespace bluetooth
}  // namespace OHOS