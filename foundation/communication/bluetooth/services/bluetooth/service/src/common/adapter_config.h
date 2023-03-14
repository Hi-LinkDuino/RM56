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

#ifndef ADAPTER_CONFIG_H
#define ADAPTER_CONFIG_H

#include <memory>
#include <string>
#include "base/base_def.h"

namespace OHOS {
namespace bluetooth {
// define section name
const std::string SECTION_CLASSIC_ADAPTER = "ClassicAdapter";
const std::string SECTION_BLE_ADAPTER = "BleAdapter";
const std::string SECTION_A2DP_SRC_SERVICE = "A2dpSrcService";
const std::string SECTION_A2DP_SNK_SERVICE = "A2dpSnkService";
const std::string SECTION_HFP_HF_SERVICE = "HfpHfService";
const std::string SECTION_HFP_AG_SERVICE = "HfpAgService";
const std::string SECTION_GATT_CLIENT_SERVICE = "GattClientService";
const std::string SECTION_GATT_SERVICE = "GattService";
const std::string SECTION_AVRCP_CT_SERVICE = "AvrcpCtService";
const std::string SECTION_AVRCP_TG_SERVICE = "AvrcpTgService";
const std::string SECTION_SOCKET_SERVICE = "SocketService";
const std::string SECTION_MAP_MCE_SERVICE = "MapMceService";
const std::string SECTION_MAP_MSE_SERVICE = "MapMseService";
const std::string SECTION_PAN_SERVICE = "PanService";
const std::string SECTION_PBAP_PCE_SERVICE = "PbapPceService";
const std::string SECTION_PBAP_PSE_SERVICE = "PbapPseService";
const std::string SECTION_HID_HOST_SERVICE = "HidHostService";
const std::string SECTION_OPP_SERVICE = "OppService";
const std::string SECTION_DI_SERVICE = "DIService";
const std::string SECTION_OUTPUT_SETTING = "OutputSetting";

// Output setting name
const std::string PROPERTY_BTSNOOP_OUTPUT = "BtsnoopOutput";
const std::string PROPERTY_BTSNOOP_OUTPUT_PATH = "BtsnoopOutputPath";
const std::string PROPERTY_HCILOG_OUTPUT = "HciLogOutput";
const std::string PROPERTY_DESENSITIZATION = "Desensitization";

// define property name
const std::string PROPERTY_IS_VALID = "IsValid";
const std::string PROPERTY_A2DP_OFFLOAD_ENABLE = "A2dpOffloadEnable";
const std::string PROPERTY_CONNECT_POLICY = "ConnectPolicy";
const std::string PROPERTY_CLASSIC_ADAPTER_ENABLE = "ClassicAdapterEnable";
const std::string PROPERTY_BLE_ADAPTER_ENABLE = "BleAdapterEnable";
const std::string PROPERTY_SERVICE_ENABLE = "ServiceEnable";
const std::string PROPERTY_MAX_CONNECTED_DEVICES = "MaxConnectedDevices";
const std::string PROPERTY_MAP_VERSION = "Version";

const std::string PROPERTY_GATT_CLIENT_SERVICE = "GattClientService";
const std::string PROPERTY_GATT_SERVER_SERVICE = "GattServerService";
const std::string PROPERTY_A2DP_SNK_SERVICE = "A2dpSnkService";
const std::string PROPERTY_A2DP_SRC_SERVICE = "A2dpSrcService";
const std::string PROPERTY_AVRCP_CT_SERVICE = "AvrcpCtService";
const std::string PROPERTY_AVRCP_TG_SERVICE = "AvrcpTgService";
const std::string PROPERTY_HFP_HF_SERVICE = "HfpHfService";
const std::string PROPERTY_HFP_AG_SERVICE = "HfpAgService";
const std::string PROPERTY_MAP_MCE_SERVICE = "MapMceService";
const std::string PROPERTY_MAP_MSE_SERVICE = "MapMseService";
const std::string PROPERTY_PBAP_PCE_SERVICE = "PbapPceService";
const std::string PROPERTY_PBAP_PSE_SERVICE = "PbapPseService";
const std::string PROPERTY_SOCKET_SERVICE = "SocketService";
const std::string PROPERTY_DI_SERVICE = "DIService";

// GATT property
const std::string PROPERTY_GATT_PROFILE_MAX_MTU = "GattProfileMaxMtu";
const std::string PROPERTY_BLE_MAX_CONNECTED_DEVICES = "BleMaxConnectedDevices";
const std::string PROPERTY_BLE_MIN_CONNECTION_INTERVAL = "BleMinConnectionInterval";
const std::string PROPERTY_BLE_MAX_CONNECTION_INTERVAL = "BleMaxConnectionInterval";
const std::string PROPERTY_BLE_CONNECTION_LATENCY = "BleConnectionLatency";
const std::string PROPERTY_BLE_CONNECTION_SUPERVISION_TIMEOUT = "BleConnectionSupervisionTimeout";
const std::string PROPERTY_BLE_GATTSERVER_EXCHANGE_MTU = "BleGattServerExchangeMtu";
const std::string PROPERTY_CLASSIC_MAX_CONNECTED_DEVICES = "ClassicMaxConnectedDevices";
const std::string PROPERTY_CLASSIC_CONNECTION_MTU = "ClassicConnectionMtu";
const std::string PROPERTY_CLASSIC_CONNECTION_MODE = "ClassicConnectionMode";
const std::string PROPERTY_CLASSIC_CONNECTION_FLUSH_TIMEOUT = "ClassicConnectionFlushTimeout";
const std::string PROPERTY_CLASSIC_CONNECTION_SECURITY_MODE = "ClassicConnectionSecurityMode";
const std::string PROPERTY_HIGH_PRIORITY_MIN_INTERVAL = "HighPriorityMinInterval";
const std::string PROPERTY_BALANCED_PRIORITY_MIN_INTERVAL = "BalancedPriorityMinInterval";
const std::string PROPERTY_LOW_POWER_MIN_INTERVAL = "LowPowerMinInterval";
const std::string PROPERTY_HIGH_PRIORITY_MAX_INTERVAL = "HighPriorityMaxInterval";
const std::string PROPERTY_BALANCED_PRIORITY_MAX_INTERVAL = "BalancedPriorityMaxInterval";
const std::string PROPERTY_LOW_POWER_MAX_INTERVAL = "LowPowerMaxInterval";
const std::string PROPERTY_HIGH_PRIORITY_LATENCY = "HighPriorityLatency";
const std::string PROPERTY_BALANCED_PRIORITY_LATENCY = "BalancedPriorityLatency";
const std::string PROPERTY_LOW_POWER_LATENCY = "LowPowerLatency";
const std::string PROPERTY_HIGH_PRIORITY_TIMEOUT = "HighPriorityTimeout";
const std::string PROPERTY_BALANCED_PRIORITY_TIMEOUT = "BalancedPriorityTimeout";
const std::string PROPERTY_LOW_POWER_TIMEOUT = "LowPowerTimeout";

const std::string PROPERTY_CONTROL_MTU = "ControlMtu";
const std::string PROPERTY_BROWSE_MTU = "BrowseMtu";

// MAP/PBAP property
const std::string PROPERTY_SRM_ENABLE = "SrmEnable";
const std::string PROPERTY_L2CAP_MTU = "L2capMtu";
const std::string PROPERTY_RFCOMM_MTU = "RfcommMtu";

// MCE property
const std::string PROPERTY_MAP_MAX_OF_GET_UREAD = "MaxOfGetUreadMessage";

// A2dpSnk property
const std::string PROPERTY_CODEC_SBC = "CodecSbc";
const std::string PROPERTY_CODEC_AAC = "CodecAac";

const std::string PROPERTY_CODEC_SBC_SUPPORT = "CodecSbcSupport";
const std::string PROPERTY_CODEC_AAC_SUPPORT = "CodecAacSupport";

class IAdapterConfig {
public:
    virtual ~IAdapterConfig() = default;
    /**
     * @brief Load XML Document from specified path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Load() = 0;

    /**
     * @brief Reload XML Document from specified path.
     * @return true Success reload XML Document.
     * @return false Failed reload XML Document.
     */
    virtual bool Reload() = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Int type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, int &value) = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) = 0;

    /**
     * @brief Whether XML document has specified section.
     * @param[in] section
     * @return true XML document has specified section.
     * @return false XML document doesnot have specified section.
     */
    virtual bool HasSection(const std::string &section) = 0;
};

class AdapterConfig : public IAdapterConfig {
public:
    /**
     * @brief Get the Instance object
     * @return IAdapterConfig*
     */
    static IAdapterConfig *GetInstance();

    /**
     * @brief Load XML Document from specified path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Load() override;

    /**
     * @brief Reload XML Document from specified path.
     * @return true Success reload XML Document.
     * @return false Failed reload XML Document.
     */
    virtual bool Reload() override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Int type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, int &value) override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) override;

    /**
     * @brief Whether XML document has specified section.
     * @param[in] section
     * @return true XML document has specified section.
     * @return false XML document doesnot have specified section.
     */
    virtual bool HasSection(const std::string &section) override;

private:
    /**
     * @brief Construct a new Adapter Config object
     */
    AdapterConfig();

    /**
     * @brief Destroy the Adapter Config object
     */
    ~AdapterConfig();

    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // ADAPTER_CONFIG_H