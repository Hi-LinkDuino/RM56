/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef HID_HOST_HOGP_H
#define HID_HOST_HOGP_H

#include <map>

#include "dispatcher.h"
#include "gap_le_if.h"
#include "gatt_data.h"
#include "gatt/gatt_defines.h"
#include "hid_host_defines.h"
#include "hid_host_message.h"
#include "interface_profile_gatt_client.h"
#include "interface_profile_manager.h"

namespace OHOS {
namespace bluetooth {
using utility::Dispatcher;

class HidHostHogp {
public:
    /**
     * @brief Construct a new HidHostHogp object
     *
     */
    explicit HidHostHogp(const std::string &address);
    /**
     * @brief Destroy the HidHostHogp object
     *
     */
    ~HidHostHogp();

    int Connect();
    int Disconnect();
    int SendData(const HidHostMessage &msg);
    PnpInformation& GetRemotePnpInfo();
    HidInformation& GetRemoteHidInfo();
    void ProcessEvent(const HidHostMessage &event);
    static std::string GetEventName(int what);

private:
    class HogpGattClientCallback : public IGattClientCallback {
    public:
        explicit HogpGattClientCallback(HidHostHogp *hogp);
        ~HogpGattClientCallback() = default;
        void OnConnectionStateChanged(int state, int newState) override;
        void OnServicesChanged(const std::vector<Service> &services) override;
        void OnCharacteristicRead(int ret, const Characteristic &characteristic) override;
        void OnCharacteristicWrite(int ret, const Characteristic &characteristic) override;
        void OnCharacteristicChanged(const Characteristic &characteristic) override;
        void OnDescriptorRead(int ret, const Descriptor &descriptor) override;
        void OnDescriptorWrite(int ret, const Descriptor &descriptor) override;
        void OnMtuChanged(int state, int mtu) override;
        void OnConnectionParameterChanged(int interval, int latency, int timeout, int status) override;
        void OnServicesDiscovered(int status) override;

    private:
        HidHostHogp *hogp_;
    };
    std::string address_ {""};
    int appId_ = -1;
    PnpInformation pnpInf_ {};
    HidInformation hidInf_ {};
    std::unique_ptr<HogpGattClientCallback> gattCallback_ = nullptr;
    uint8_t state_;
    std::condition_variable cvfull_;
    std::mutex mutexWaitGattCallback_;
    std::unique_ptr<Characteristic> characteristicTemp_ = nullptr;
    std::unique_ptr<Descriptor> descriptorTemp_ = nullptr;
    std::map<uint16_t, std::unique_ptr<HogpReport>> reports_ {};
    std::unique_ptr<Dispatcher> dispatcher_ {};

    IProfileGattClient *GetGattClientService();
    int DiscoverStart();
    int RegisterGattClientApplication(const RawAddress &addr);
    int DeregisterGattClientApplication();

    static void GapLeRequestSecurityResult(
        const BtAddr *addr, uint8_t result, GAP_LeSecurityStatus status, void *context);

    void OnConnectionStateChangedTask(int newState);
    void OnServicesDiscoveredTask(int status);
    void OnServicesDiscoveredTask_(int status);
    void SendStopHogpThread();
    void SendData_(const HidHostMessage &msg);

    int GetPnpInformation(Service service);
    int GetHidInformation(Service service);
    int GetHidReportMap(Characteristic character);
    void GetHidReport(Characteristic character);
    int SavePnpInformation(Characteristic character);
    int SaveHidInformation(Characteristic character);
    int SaveReportMap(Characteristic character);
    void SaveReport(Characteristic character, Descriptor descriptor, Descriptor config);
    void SetInputReportNotification();

    int SendGetReport(uint8_t reportId, Characteristic character);
    int SendSetReport(Characteristic character, int length, uint8_t* pkt);
    void ReceiveHandShake(uint16_t err);
    int ReceiveControlData(Characteristic character, uint8_t reportId);
    HogpReport* FindReportByTypeAndId(uint8_t type, uint8_t reportId);

    // For PTS
    void GetBatteryInformation(Service service);
    void GetExternalRptRefInfo(Characteristic character);
    void GetExternalCfgInfo(Characteristic character);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // HID_HOST_HOGP_H