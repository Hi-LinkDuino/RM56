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
#include "wifi_service_manager.h"
#include <dlfcn.h>
#include "wifi_logger.h"
#include "define.h"
#include "wifi_settings.h"

DEFINE_WIFILOG_LABEL("WifiServiceManager");

namespace OHOS {
namespace Wifi {
WifiServiceManager &WifiServiceManager::GetInstance()
{
    static WifiServiceManager gWifiServiceManager;
    return gWifiServiceManager;
}

WifiServiceManager::WifiServiceManager()
{}

WifiServiceManager::~WifiServiceManager()
{}

int WifiServiceManager::Init()
{
    mServiceDllMap.insert(std::make_pair(WIFI_SERVICE_STA, "libwifi_sta_service.z.so"));
    mServiceDllMap.insert(std::make_pair(WIFI_SERVICE_SCAN, "libwifi_scan_service.z.so"));
    mServiceDllMap.insert(std::make_pair(WIFI_SERVICE_AP, "libwifi_ap_service.z.so"));
    mServiceDllMap.insert(std::make_pair(WIFI_SERVICE_P2P, "libwifi_p2p_service.z.so"));
    mServiceDllMap.insert(std::make_pair(WIFI_SERVICE_AWARE, "libwifi_aware_service.z.so"));
    return 0;
}

int WifiServiceManager::GetServiceDll(const std::string &name, std::string &dlname)
{
    auto iter = mServiceDllMap.find(name);
    if (iter == mServiceDllMap.end()) {
        return -1;
    }
    dlname = iter->second;
    return 0;
}

int WifiServiceManager::CheckPreLoadService(void)
{
    for (auto iter = mServiceDllMap.begin(); iter != mServiceDllMap.end(); ++iter) {
        bool bLoad = WifiSettings::GetInstance().IsModulePreLoad(iter->first);
        if (bLoad) {
            int ret = CheckAndEnforceService(iter->first, false);
            if (ret < 0) {
                return -1;
            }
        }
    }
    return 0;
}

int WifiServiceManager::LoadStaService(const std::string &dlname, bool bCreate)
{
    if (mStaServiceHandle.handle != nullptr) {
        return 0;
    }
    mStaServiceHandle.handle = dlopen(dlname.c_str(), RTLD_LAZY);
    if (mStaServiceHandle.handle == nullptr) {
        WIFI_LOGE("dlopen %{public}s failed: %{public}s!", dlname.c_str(), dlerror());
        return -1;
    }
    mStaServiceHandle.create = (IStaService *(*)()) dlsym(mStaServiceHandle.handle, "Create");
    mStaServiceHandle.destroy = (void *(*)(IStaService *))dlsym(mStaServiceHandle.handle, "Destroy");
    if (mStaServiceHandle.create == nullptr || mStaServiceHandle.destroy == nullptr) {
        WIFI_LOGE("%{public}s dlsym Create or Destory failed!", dlname.c_str());
        dlclose(mStaServiceHandle.handle);
        mStaServiceHandle.Clear();
        return -1;
    }
    if (bCreate) {
        mStaServiceHandle.pService = mStaServiceHandle.create();
    }
    return 0;
}

int WifiServiceManager::LoadScanService(const std::string &dlname, bool bCreate)
{
    if (mScanServiceHandle.handle != nullptr) {
        return 0;
    }
    mScanServiceHandle.handle = dlopen(dlname.c_str(), RTLD_LAZY);
    if (mScanServiceHandle.handle == nullptr) {
        WIFI_LOGE("dlopen %{public}s failed: %{public}s!", dlname.c_str(), dlerror());
        return -1;
    }
    mScanServiceHandle.create = (IScanService *(*)()) dlsym(mScanServiceHandle.handle, "Create");
    mScanServiceHandle.destroy = (void *(*)(IScanService *))dlsym(mScanServiceHandle.handle, "Destroy");
    if (mScanServiceHandle.create == nullptr || mScanServiceHandle.destroy == nullptr) {
        WIFI_LOGE("%{public}s dlsym Create or Destory failed!", dlname.c_str());
        dlclose(mScanServiceHandle.handle);
        mScanServiceHandle.Clear();
        return -1;
    }
    if (bCreate) {
        mScanServiceHandle.pService = mScanServiceHandle.create();
    }
    return 0;
}

int WifiServiceManager::LoadApService(const std::string &dlname, bool bCreate)
{
    if (mApServiceHandle.handle != nullptr) {
        return 0;
    }
    mApServiceHandle.handle = dlopen(dlname.c_str(), RTLD_LAZY);
    if (mApServiceHandle.handle == nullptr) {
        WIFI_LOGE("dlopen %{public}s failed: %{public}s!", dlname.c_str(), dlerror());
        return -1;
    }
    mApServiceHandle.create = (IApService *(*)()) dlsym(mApServiceHandle.handle, "Create");
    mApServiceHandle.destroy = (void *(*)(IApService *))dlsym(mApServiceHandle.handle, "Destroy");
    if (mApServiceHandle.create == nullptr || mApServiceHandle.destroy == nullptr) {
        WIFI_LOGE("%{public}s dlsym Create or Destory failed!", dlname.c_str());
        dlclose(mApServiceHandle.handle);
        mApServiceHandle.Clear();
        return -1;
    }
    if (bCreate) {
        mApServiceHandle.pService = mApServiceHandle.create();
    }
    return 0;
}

int WifiServiceManager::LoadP2pService(const std::string &dlname, bool bCreate)
{
    if (mP2pServiceHandle.handle != nullptr) {
        return 0;
    }
    mP2pServiceHandle.handle = dlopen(dlname.c_str(), RTLD_LAZY);
    if (mP2pServiceHandle.handle == nullptr) {
        WIFI_LOGE("dlopen %{public}s failed: %{public}s!", dlname.c_str(), dlerror());
        return -1;
    }
    mP2pServiceHandle.create = (IP2pService *(*)()) dlsym(mP2pServiceHandle.handle, "Create");
    mP2pServiceHandle.destroy = (void *(*)(IP2pService *))dlsym(mP2pServiceHandle.handle, "Destroy");
    if (mP2pServiceHandle.create == nullptr || mP2pServiceHandle.destroy == nullptr) {
        WIFI_LOGE("%{public}s dlsym Create or Destory failed!", dlname.c_str());
        dlclose(mP2pServiceHandle.handle);
        mP2pServiceHandle.Clear();
        return -1;
    }
    if (bCreate) {
        mP2pServiceHandle.pService = mP2pServiceHandle.create();
    }
    return 0;
}
int WifiServiceManager::CheckAndEnforceService(const std::string &name, bool bCreate)
{
    WIFI_LOGD("WifiServiceManager::CheckAndEnforceService name: %{public}s", name.c_str());
    std::string dlname;
    if (GetServiceDll(name, dlname) < 0) {
        WIFI_LOGE("%{public}s does not support", name.c_str());
        return -1;
    }
    WIFI_LOGD("WifiServiceManager::CheckAndEnforceService get dllname: %{public}s", dlname.c_str());
    std::unique_lock<std::mutex> lock(mMutex);
    if (name == WIFI_SERVICE_STA) {
        return LoadStaService(dlname, bCreate);
    }
    if (name == WIFI_SERVICE_SCAN) {
        return LoadScanService(dlname, bCreate);
    }
    if (name == WIFI_SERVICE_AP) {
        return LoadApService(dlname, bCreate);
    }
    if (name == WIFI_SERVICE_P2P) {
        return LoadP2pService(dlname, bCreate);
    }
    return -1;
}

IStaService *WifiServiceManager::GetStaServiceInst()
{
    if (mStaServiceHandle.handle == nullptr) {
        return nullptr;
    }
    if (mStaServiceHandle.pService == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mStaServiceHandle.pService == nullptr) {
            mStaServiceHandle.pService = mStaServiceHandle.create();
        }
    }
    return mStaServiceHandle.pService;
}

IScanService *WifiServiceManager::GetScanServiceInst()
{
    if (mScanServiceHandle.handle == nullptr) {
        return nullptr;
    }
    if (mScanServiceHandle.pService == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mScanServiceHandle.pService == nullptr) {
            mScanServiceHandle.pService = mScanServiceHandle.create();
        }
    }
    return mScanServiceHandle.pService;
}

IApService *WifiServiceManager::GetApServiceInst()
{
    if (mApServiceHandle.handle == nullptr) {
        return nullptr;
    }
    if (mApServiceHandle.pService == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mApServiceHandle.pService == nullptr) {
            mApServiceHandle.pService = mApServiceHandle.create();
        }
    }
    return mApServiceHandle.pService;
}

IP2pService *WifiServiceManager::GetP2pServiceInst()
{
    if (mP2pServiceHandle.handle == nullptr) {
        return nullptr;
    }
    if (mP2pServiceHandle.pService == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mP2pServiceHandle.pService == nullptr) {
            mP2pServiceHandle.pService = mP2pServiceHandle.create();
        }
    }
    return mP2pServiceHandle.pService;
}

int WifiServiceManager::UnloadStaService(bool bPreLoad)
{
    if (mStaServiceHandle.handle == nullptr) {
        return 0;
    }
    if (mStaServiceHandle.pService != nullptr) {
        mStaServiceHandle.destroy(mStaServiceHandle.pService);
        mStaServiceHandle.pService = nullptr;
    }
    if (!bPreLoad) {
        dlclose(mStaServiceHandle.handle);
        mStaServiceHandle.Clear();
    }
    return 0;
}

int WifiServiceManager::UnloadScanService(bool bPreLoad)
{
    if (mScanServiceHandle.handle == nullptr) {
        return 0;
    }
    if (mScanServiceHandle.pService != nullptr) {
        mScanServiceHandle.destroy(mScanServiceHandle.pService);
        mScanServiceHandle.pService = nullptr;
    }
    if (!bPreLoad) {
        dlclose(mScanServiceHandle.handle);
        mScanServiceHandle.Clear();
    }
    return 0;
}

int WifiServiceManager::UnloadApService(bool bPreLoad)
{
    if (mApServiceHandle.handle == nullptr) {
        return 0;
    }
    if (mApServiceHandle.pService != nullptr) {
        mApServiceHandle.destroy(mApServiceHandle.pService);
        mApServiceHandle.pService = nullptr;
    }
    if (!bPreLoad) {
        dlclose(mApServiceHandle.handle);
        mApServiceHandle.Clear();
    }
    return 0;
}

int WifiServiceManager::UnloadP2pService(bool bPreLoad)
{
    if (mP2pServiceHandle.handle == nullptr) {
        return 0;
    }
    if (mP2pServiceHandle.pService != nullptr) {
        mP2pServiceHandle.destroy(mP2pServiceHandle.pService);
        mP2pServiceHandle.pService = nullptr;
    }
    if (!bPreLoad) {
        dlclose(mP2pServiceHandle.handle);
        mP2pServiceHandle.Clear();
    }
    return 0;
}

int WifiServiceManager::UnloadService(const std::string &name)
{
    bool bPreLoad = WifiSettings::GetInstance().IsModulePreLoad(name);
    WIFI_LOGD("WifiServiceManager::UnloadService name: %{public}s", name.c_str());
    std::unique_lock<std::mutex> lock(mMutex);
    if (name == WIFI_SERVICE_STA) {
        return UnloadStaService(bPreLoad);
    }
    if (name == WIFI_SERVICE_SCAN) {
        return UnloadScanService(bPreLoad);
    }
    if (name == WIFI_SERVICE_AP) {
        return UnloadApService(bPreLoad);
    }
    if (name == WIFI_SERVICE_P2P) {
        return UnloadP2pService(bPreLoad);
    }
    return -1;
}

void WifiServiceManager::UninstallAllService()
{
    WIFI_LOGD("WifiServiceManager::UninstallAllService");
    UnloadStaService(false);
    UnloadScanService(false);
    UnloadApService(false);
    UnloadP2pService(false);
    return;
}
} // namespace Wifi
} // namespace OHOS