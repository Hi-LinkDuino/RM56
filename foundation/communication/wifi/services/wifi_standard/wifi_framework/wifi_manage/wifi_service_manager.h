/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_SERVICE_MANAGER_H
#define OHOS_WIFI_SERVICE_MANAGER_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "ista_service.h"
#include "iscan_service.h"
#include "i_ap_service.h"
#include "ip2p_service.h"

namespace OHOS {
namespace Wifi {
struct StaServiceHandle {
    void *handle;
    IStaService *(*create)();
    void *(*destroy)(IStaService *);
    IStaService *pService;
    StaServiceHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pService(nullptr)
    {}
    ~StaServiceHandle()
    {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pService = nullptr;
    }
};

struct ScanServiceHandle {
    void *handle;
    IScanService *(*create)();
    void *(*destroy)(IScanService *);
    IScanService *pService;
    ScanServiceHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pService(nullptr)
    {}
    ~ScanServiceHandle()
    {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pService = nullptr;
    }
};

struct ApServiceHandle {
    void *handle;
    IApService *(*create)();
    void *(*destroy)(IApService *);
    IApService *pService;
    ApServiceHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pService(nullptr)
    {}
    ~ApServiceHandle()
    {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pService = nullptr;
    }
};

struct P2pServiceHandle {
    void *handle;
    IP2pService *(*create)();
    void *(*destroy)(IP2pService *);
    IP2pService *pService;
    P2pServiceHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pService(nullptr)
    {}
    ~P2pServiceHandle()
    {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pService = nullptr;
    }
};

class WifiServiceManager {
public:
    WifiServiceManager();
    ~WifiServiceManager();

    /**
     * @Description Initialize the mapping between feature service names and SO paths
     *
     * @return int - init result, when 0 means success, other means some fails happened
     */
    int Init();

    /**
     * @Description Check preload config, maybe need preload feature service
     *
     * @return int - 0 need preload; other no need preload
     */
    int CheckPreLoadService(void);

    /**
     * @Description Check the feature service. If the service is not loaded, continue to load the service
     *
     * @param name - feature service name
     * @param bCreate - whether create the service instance
     * @return int - 0 success; -1 feature service name not correct or load service failed
     */
    int CheckAndEnforceService(const std::string &name, bool bCreate = true);

    /**
     * @Description Get the Sta Service Inst object
     *
     * @return IStaService* - sta service pointer, if sta not supported, nullptr is returned
     */
    IStaService *GetStaServiceInst(void);

    /**
     * @Description Get the Scan Service Inst object
     *
     * @return IScanService* - scan service pointer, if scan not supported, nullptr is returned
     */
    IScanService *GetScanServiceInst(void);

    /**
     * @Description Get the Ap Service Inst object
     *
     * @return IApService* - ap service pointer, if ap not supported, nullptr is returned
     */
    IApService *GetApServiceInst(void);

    /**
     * @Description Get the P2P Service Inst object
     *
     * @return IP2pService* - p2p service pointer, if p2p not supported, nullptr is returned
     */
    IP2pService *GetP2pServiceInst(void);

    /**
     * @Description unload a feature service
     *
     * @param name - feature service name
     * @return int - 0 success
     */
    int UnloadService(const std::string &name);

    /**
     * @Description Uninstall all loaded feature services
     *
     */
    void UninstallAllService();
    static WifiServiceManager &GetInstance();

private:
    int GetServiceDll(const std::string &name, std::string &dlname);
    int LoadStaService(const std::string &dlname, bool bCreate);
    int UnloadStaService(bool bPreLoad);
    int LoadScanService(const std::string &dlname, bool bCreate);
    int UnloadScanService(bool bPreLoad);
    int LoadApService(const std::string &dlname, bool bCreate);
    int UnloadApService(bool bPreLoad);
    int LoadP2pService(const std::string &dlname, bool bCreate);
    int UnloadP2pService(bool bPreLoad);

private:
    std::mutex mMutex;
    std::unordered_map<std::string, std::string> mServiceDllMap;
    StaServiceHandle mStaServiceHandle;
    ScanServiceHandle mScanServiceHandle;
    ApServiceHandle mApServiceHandle;
    P2pServiceHandle mP2pServiceHandle;
};
} // namespace Wifi
} // namespace OHOS
#endif