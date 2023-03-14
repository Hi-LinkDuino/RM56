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

#ifndef WIFI_NET_AGENT_H
#define WIFI_NET_AGENT_H

#include <memory>
#include <string>
#include <singleton.h>
#include <utility>
#include <vector>

#include "i_net_conn_service.h"
#include "net_all_capabilities.h"
#include "net_supplier_callback_base.h"

#include "wifi_internal_msg.h"
#include "sta_service_callback.h"
#include "wifi_log.h"

namespace OHOS {
namespace Wifi {
class WifiNetAgent : public DelayedRefSingleton<WifiNetAgent> {
    DECLARE_DELAYED_REF_SINGLETON(WifiNetAgent);

public:
    /**
     * Register the network information with the network management module
     *
     * @return true if register success else return false;
     */
    bool RegisterNetSupplier();
    /**
     * Register the network callback with the network management module
     *
     * @return true if register success else return false;
     */
    bool RegisterNetSupplierCallback(const StaServiceCallback &callback);

    /**
     * Cancel the registration information to the network management
     */
    void UnregisterNetSupplier();

    /**
     * Update network information
     *
     * @param supplierId network unique identity id returned after network registration
     * @param netSupplierInfo network data information
     */
    void UpdateNetSupplierInfo(sptr<NetManagerStandard::NetSupplierInfo> &netSupplierInfo);

    /**
     * Update link information
     *
     * @param supplierId network unique identity id returned after network registration
     * @param netLinkInfo network link data information
     */
    void UpdateNetLinkInfo(std::string &ip, std::string &mask, std::string &gateWay,
        std::string &strDns, std::string &strBakDns);

    /**
     * Add route
     *
     * @param interface interface name
     * @param ipAddress IP address
     * @param prefixLength prefix length
     */
    bool AddRoute(const std::string interface, const std::string ipAddress, int prefixLength);

public:
    class NetConnCallback : public NetManagerStandard::NetSupplierCallbackBase {
    public:
        /**
         * @Description : Construct a new NetConn object
         *
         */
        explicit NetConnCallback(const StaServiceCallback &callback);

        /**
         * @Description : Destroy the NetConn object
         *
         */
        ~NetConnCallback() override;

        /**
         * @Description : Connection Management triggers the open automatic connection feature.
         *
         * @param ident - identity
         * @param netCaps - Net capability to request a network
         */
        int32_t RequestNetwork(const std::string &ident, const std::set<NetManagerStandard::NetCap> &netCaps) override;
        /**
         * @Description : Connection Management triggers the close automatic connection feature.
         *
         * @param ident - identity
         * @param netCaps - Net capability to request a network
         */
        int32_t ReleaseNetwork(const std::string &ident, const std::set<NetManagerStandard::NetCap> &netCaps) override;
    private:
        void LogNetCaps(const std::string &ident, const std::set<NetManagerStandard::NetCap> &netCaps) const;
    private:
        StaServiceCallback staCallback;
    };

private:
    uint32_t supplierId;
};
} // namespace Wifi
} // namespace OHOS
#endif // CELLULAR_DATA_NET_AGENT_H
