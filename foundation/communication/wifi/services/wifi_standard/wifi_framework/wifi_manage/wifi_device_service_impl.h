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

#ifndef OHOS_WIFI_DEVICE_SERVICE_IMPL_H
#define OHOS_WIFI_DEVICE_SERVICE_IMPL_H

#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "i_wifi_device_callback.h"
#include "system_ability.h"
#include "wifi_device_stub.h"
#include "iremote_object.h"
#include "wifi_p2p_service_impl.h"

namespace OHOS {
namespace Wifi {
class WifiDeviceServiceImpl : public SystemAbility, public WifiDeviceStub {
DECLARE_SYSTEM_ABILITY(WifiDeviceServiceImpl);
public:
    WifiDeviceServiceImpl();
    virtual ~WifiDeviceServiceImpl();

    static sptr<WifiDeviceServiceImpl> GetInstance();

    void OnStart() override;
    void OnStop() override;

    ErrCode EnableWifi() override;

    ErrCode DisableWifi() override;

    ErrCode InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName) override;

    ErrCode GetWifiProtectRef(const WifiProtectMode &protectMode, const std::string &protectName) override;

    ErrCode PutWifiProtectRef(const std::string &protectName) override;

    ErrCode AddDeviceConfig(const WifiDeviceConfig &config, int &result) override;

    ErrCode UpdateDeviceConfig(const WifiDeviceConfig &config, int &result) override;

    ErrCode RemoveDevice(int networkId) override;

    ErrCode RemoveAllDevice() override;

    ErrCode GetDeviceConfigs(std::vector<WifiDeviceConfig> &result) override;

    ErrCode EnableDeviceConfig(int networkId, bool attemptEnable) override;

    ErrCode DisableDeviceConfig(int networkId) override;

    ErrCode ConnectToNetwork(int networkId) override;

    ErrCode ConnectToDevice(const WifiDeviceConfig &config) override;

    bool IsConnected() override;

    ErrCode ReConnect() override;

    ErrCode ReAssociate(void) override;

    ErrCode Disconnect(void) override;

    ErrCode StartWps(const WpsConfig &config) override;

    ErrCode CancelWps(void) override;

    ErrCode IsWifiActive(bool &bActive) override;

    ErrCode GetWifiState(int &state) override;

    ErrCode GetLinkedInfo(WifiLinkedInfo &info) override;

    ErrCode GetIpInfo(IpInfo &info) override;

    ErrCode SetCountryCode(const std::string &countryCode) override;

    ErrCode GetCountryCode(std::string &countryCode) override;

    ErrCode RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback) override;

    ErrCode GetSignalLevel(const int &rssi, const int &band, int &level) override;

    ErrCode GetSupportedFeatures(long &features) override;

    ErrCode GetDeviceMacAddress(std::string &result) override;

    bool SetLowLatencyMode(bool enabled) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

private:
    bool Init();
    ErrCode CheckCanEnableWifi(void);
    bool IsStaServiceRunning();
    bool IsScanServiceRunning();
    bool CheckConfigPwd(const WifiDeviceConfig &config);
    static void SaBasicDump(std::string& result);

private:
    static constexpr int MAX_PRESHAREDKEY_LEN = 63;
    static constexpr int MAX_HEX_LEN = 64;
    static constexpr int MIN_PSK_LEN = 8;
    static constexpr int MIN_SAE_LEN = 1;
    static constexpr int WEP_KEY_LEN1 = 5;
    static constexpr int WEP_KEY_LEN2 = 13;
    static constexpr int WEP_KEY_LEN3 = 16;

    static sptr<WifiDeviceServiceImpl> g_instance;
    static std::mutex g_instanceLock;
    bool mPublishFlag;
    ServiceRunningState mState;
};
}  // namespace Wifi
}  // namespace OHOS
#endif