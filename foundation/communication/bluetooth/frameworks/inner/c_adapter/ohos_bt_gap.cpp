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

#include "ohos_bt_gap.h"

#include <string.h>
#include "__config"
#include "bluetooth_def.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_utils.h"
#include "iosfwd"
#include "ohos_bt_adapter_utils.h"
#include "ohos_bt_def.h"
#include "string"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

namespace OHOS {
namespace Bluetooth {
static BluetoothHost *g_BluetoothHost;
static BtGapCallBacks *g_GapCallback;

class BluetoothHostObserverWapper : public BluetoothHostObserver {
public:
    /**
     * @brief Adapter state change function.
     *
     * @param transport Transport type when state change.
     *        BTTransport::ADAPTER_BREDR : classic;
     *        BTTransport::ADAPTER_BLE : ble.
     * @param state Change to the new state.
     *        BTStateID::STATE_TURNING_ON;
     *        BTStateID::STATE_TURN_ON;
     *        BTStateID::STATE_TURNING_OFF;
     *        BTStateID::STATE_TURN_OFF.
     * @since 6
     */
    NO_SANITIZE("cfi") void OnStateChanged(const int transport, const int status) {
        int cvtTransport = OHOS_BT_TRANSPORT_LE;
        if (transport == BTTransport::ADAPTER_BREDR) {
            cvtTransport = OHOS_BT_TRANSPORT_BR_EDR;
        }
        HILOGI("transport: %{public}d, status: %{public}d", cvtTransport, status);
        if (g_GapCallback != nullptr && g_GapCallback->stateChangeCallback != nullptr) {
            g_GapCallback->stateChangeCallback(cvtTransport, status);
        } else {
            HILOGI("callback func is null!");
        }
    }

    /**
     * @brief Discovery state changed observer.
     *
     * @param status Device discovery status.
     * @since 6
     */
    void OnDiscoveryStateChanged(int status)
    {
        (void)status;
    }

    /**
     * @brief Discovery result observer.
     *
     * @param device Remote device.
     * @since 6
     */
    void OnDiscoveryResult(const BluetoothRemoteDevice &device) {}

    /**
     * @brief Pair request observer.
     *
     * @param device Remote device.
     * @since 6
     */
    NO_SANITIZE("cfi") void OnPairRequested(const BluetoothRemoteDevice &device)
    {
        BdAddr remoteAddr;
        GetAddrFromString(device.GetDeviceAddr(), remoteAddr.addr);
        int transport = device.GetTransportType();
        if (transport == BT_TRANSPORT_BREDR) {
            transport = OHOS_BT_TRANSPORT_BR_EDR;
        } else if (transport == BT_TRANSPORT_BLE) {
            transport = OHOS_BT_TRANSPORT_LE;
        }
        HILOGI("device: %{public}s, transport:%{public}d", GET_ENCRYPT_ADDR(device), transport);
        if (g_GapCallback != nullptr && g_GapCallback->pairRequestedCallback != nullptr) {
            g_GapCallback->pairRequestedCallback(&remoteAddr, transport);
        } else {
            HILOGW("callback func is null!");
        }
    }

    /**
     * @brief Pair confirmed observer.
     *
     * @param device Remote device.
     * @param reqType Pair type.
     * @param number Paired passkey.
     * @since 6
     */
    NO_SANITIZE("cfi") void OnPairConfirmed(const BluetoothRemoteDevice &device, int reqType, int number)
    {
        int transport = device.GetTransportType();
        HILOGI("reqType: %{public}d, number: %{public}d, transport: %{public}d",
            reqType, number, transport);
        BdAddr remoteAddr;
        GetAddrFromString(device.GetDeviceAddr(), remoteAddr.addr);
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (g_GapCallback == nullptr || g_GapCallback->pairConfiremedCallback == nullptr) {
            HILOGW("callback func is null!");
            return;
        }
        if (transport == BT_TRANSPORT_BREDR) {
            if (reqType == PAIR_CONFIRM_TYPE_NUMERIC || reqType == PAIR_CONFIRM_TYPE_CONSENT) {
                g_GapCallback->pairConfiremedCallback(&remoteAddr, OHOS_BT_TRANSPORT_BR_EDR, reqType, number);
            }
        } else if (transport == BT_TRANSPORT_BLE) {
            if (reqType == PAIR_CONFIRM_TYPE_NUMERIC) {
                g_GapCallback->pairConfiremedCallback(&remoteAddr, OHOS_BT_TRANSPORT_LE, reqType, number);
            }
        } else {
            HILOGE("transport: %{public}d is invalid", transport);
        }
    };

    /**
     * @brief Scan mode changed observer.
     *
     * @param mode Device scan mode.
     * @since 6
     */
    void OnScanModeChanged(int mode)
    {
        HILOGI("mode: %{public}d", mode);
        if (g_GapCallback != nullptr && g_GapCallback->scanModeChangedCallback != nullptr) {
            g_GapCallback->scanModeChangedCallback(mode);
        } else {
            HILOGW("mode: %{public}d, but callback is null!", mode);
        }
    };

    /**
     * @brief Device name changed observer.
     *
     * @param deviceName Device name.
     * @since 6
     */
    void OnDeviceNameChanged(const std::string &deviceName)
    {
        (void)deviceName;
    };

    /**
     * @brief Device address changed observer.
     *
     * @param address Device address.
     * @since 6
     */
    void OnDeviceAddrChanged(const std::string &address)
    {
        (void)address;
    };
};

static BluetoothHostObserverWapper g_hostObserver;

bool EnableBle(void)
{
    HILOGI("enter");
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    if (g_BluetoothHost->IsBleEnabled()) {
        HILOGI("ble enabled already");
        return true;
    }

    bool ret = g_BluetoothHost->EnableBle();
    HILOGI("result: %{public}d", ret);
    return ret;
}

bool DisableBle(void)
{
    HILOGI("enter");
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    if (!g_BluetoothHost->IsBleEnabled()) {
        HILOGI("ble disabled already");
        return true;
    }

    bool ret = g_BluetoothHost->DisableBle();
    HILOGI("result: %{public}d", ret);
    return ret;
}

bool EnableBt(void)
{
    HILOGI("enter");
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    int state = g_BluetoothHost->GetBtState();
    if (state == BTStateID::STATE_TURNING_ON ||
        state == BTStateID::STATE_TURN_ON) {
        HILOGI("br state is %{public}d", state);
        return true;
    }
    bool ret = g_BluetoothHost->EnableBt();
    HILOGI("result: %{public}d", ret);
    return ret;
}

bool DisableBt(void)
{
    HILOGI("enter");
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    int state = g_BluetoothHost->GetBtState();
    if (state == BTStateID::STATE_TURNING_OFF ||
        state == BTStateID::STATE_TURN_OFF) {
        HILOGI("br state is %{public}d", state);
        return true;
    }
    bool ret = g_BluetoothHost->DisableBt();
    HILOGI("result: %{public}d", ret);
    return ret;
}

int GetBtState()
{
    HILOGI("enter");
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    int state = g_BluetoothHost->GetBtState();
    HILOGI("br state: %{public}d", state);
    return state;
}

bool IsBleEnabled()
{
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    bool ret = g_BluetoothHost->IsBleEnabled();
    HILOGI("ble enable: %{public}d", ret);
    return ret;
}

bool GetLocalAddr(unsigned char *mac, unsigned int len)
{
    HILOGI("enter");
    if (mac == nullptr || len < OHOS_BD_ADDR_LEN) {
        HILOGE("invalid param, len:%{public}d", len);
        return false;
    }
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    string localAddress = g_BluetoothHost->GetLocalAddress();
    GetAddrFromString(localAddress, mac);
    HILOGI("device: %{public}s", GetEncryptAddr(localAddress).c_str());
    return true;
}

bool SetLocalName(unsigned char *localName, unsigned char length)
{
    HILOGI("enter");
    if (localName == nullptr) {
        HILOGE("localName is null");
        return false;
    }

    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }

    string newName(reinterpret_cast<const char *>(localName));
    bool ret = g_BluetoothHost->SetLocalName(newName);
    HILOGI("result %{public}d: LocalName : %{public}s", ret, g_BluetoothHost->GetLocalName().c_str());
    return ret;
}

bool SetBtScanMode(int mode, int duration)
{
    HILOGI("mode: %{public}d, duration: %{public}d", mode, duration);
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }
    bool ret = g_BluetoothHost->SetBtScanMode(mode, duration);
    g_BluetoothHost->SetBondableMode(BT_TRANSPORT_BREDR, BONDABLE_MODE_ON);
    return ret;
}

bool PairRequestReply(const BdAddr *bdAddr, int transport, bool accept)
{
    string strAddress;
    ConvertAddr(bdAddr->addr, strAddress);
    HILOGI("device: %{public}s", GetEncryptAddr(strAddress).c_str());
    BluetoothRemoteDevice remoteDevice;
    if (transport == OHOS_BT_TRANSPORT_BR_EDR) {
        remoteDevice = g_BluetoothHost->GetRemoteDevice(strAddress, BT_TRANSPORT_BREDR);
    } else if (transport == OHOS_BT_TRANSPORT_LE) {
        remoteDevice = g_BluetoothHost->GetRemoteDevice(strAddress, BT_TRANSPORT_BLE);
    } else {
        HILOGE("transport: %{public}d is invalid", transport);
        return false;
    }
    bool ret = remoteDevice.PairRequestReply(accept);
    HILOGI("transport: %{public}d, accept: %{public}d, ret: %{public}d", transport, accept, ret);
    return ret;
}

bool SetDevicePairingConfirmation(const BdAddr *bdAddr, int transport, bool accept)
{
    string strAddress;
    ConvertAddr(bdAddr->addr, strAddress);
    HILOGI("device: %{public}s, accept: %{public}d", GetEncryptAddr(strAddress).c_str(), accept);
    BluetoothRemoteDevice remoteDevice;
    if (transport == OHOS_BT_TRANSPORT_BR_EDR) {
        remoteDevice = g_BluetoothHost->GetRemoteDevice(strAddress, BT_TRANSPORT_BREDR);
    } else if (transport == OHOS_BT_TRANSPORT_LE) {
        remoteDevice = g_BluetoothHost->GetRemoteDevice(strAddress, BT_TRANSPORT_BLE);
    } else {
        HILOGE("transport: %{public}d is invalid", transport);
        return false;
    }
    bool ret = remoteDevice.SetDevicePairingConfirmation(accept);
    HILOGI("ret: %{public}d", ret);
    return ret;
}

int GapRegisterCallbacks(BtGapCallBacks *func)
{
    HILOGI("enter");
    if (func == nullptr) {
        HILOGE("func is null.");
        return OHOS_BT_STATUS_PARM_INVALID;
    }
    if (g_BluetoothHost == nullptr) {
        g_BluetoothHost = &BluetoothHost::GetDefaultHost();
    }
    g_GapCallback = func;
    g_BluetoothHost->RegisterObserver(g_hostObserver);
    return OHOS_BT_STATUS_SUCCESS;
}
}  // namespace Bluetooth
}  // namespace OHOS
#ifdef __cplusplus
}
#endif
