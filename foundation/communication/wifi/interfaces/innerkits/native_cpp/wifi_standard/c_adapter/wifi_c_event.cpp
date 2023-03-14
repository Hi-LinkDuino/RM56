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

#include "native_c/wifi_event.h"
#include "native_c/wifi_device.h"
#include "native_c/wifi_scan_info.h"
#include "wifi_logger.h"
#include "common_event_manager.h"
#include <set>

DEFINE_WIFILOG_LABEL("WifiCEvent");

const std::string WIFI_USUAL_EVENT_CONN_STATE = "usual.event.wifi.CONN_STATE";
const std::string WIFI_USUAL_EVENT_SCAN_STATE = "usual.event.wifi.SCAN_FINISHED";
const std::string WIFI_USUAL_EVENT_HOTSPOT_STATE = "usual.event.wifi.HOTSPOT_STATE";
const std::string WIFI_USUAL_EVENT_STA_JOIN = "usual.event.wifi.WIFI_HS_STA_JOIN";
const std::string WIFI_USUAL_EVENT_STA_LEAVE = "usual.event.wifi.WIFI_HS_STA_LEAVE";

using ConnectionChangeCb = void (*)(int, WifiLinkedInfo*);
using ScanStateChangeCb = void (*)(int, int);
using HotspotStateChangeCb = void (*)(int);
using HotspotJoinCb = void (*)(StationInfo*);
using HotspotLeaveCb = void (*)(StationInfo*);

using namespace OHOS::EventFwk;
class EventManager {
public:
    EventManager() {
    }

    virtual ~EventManager() {
    }

    bool AddConnectionChangeCb(const ConnectionChangeCb cb) {
        if (m_setConnectionChangeCb.empty()) {
            if (!SubscribeServiceEvent(WIFI_USUAL_EVENT_CONN_STATE)) {
                return false;
            }
        }
        return m_setConnectionChangeCb.insert(cb).second;
    }

    void RemoveConnectionChangeCb(const ConnectionChangeCb cb) {
        m_setConnectionChangeCb.erase(cb);
        if (m_setConnectionChangeCb.empty()) {
            UnsubscribeServiceEvent(WIFI_USUAL_EVENT_CONN_STATE);
        }
    }

    static std::set<ConnectionChangeCb> GetConnectionChangeCb() {
        return m_setConnectionChangeCb;
    }

    bool AddScanStateChangeCb(const ScanStateChangeCb cb) {
        if (m_setScanStateChangeCb.empty()) {
            if (!SubscribeServiceEvent(WIFI_USUAL_EVENT_SCAN_STATE)) {
                return false;
            }
        }
        return m_setScanStateChangeCb.insert(cb).second;
    }

    void RemoveScanStateChangeCb(const ScanStateChangeCb cb) {
        m_setScanStateChangeCb.erase(cb);
        if (m_setScanStateChangeCb.empty()) {
            UnsubscribeServiceEvent(WIFI_USUAL_EVENT_SCAN_STATE);
        }
    }

    static std::set<ScanStateChangeCb> GetScanStateChangeCb() {
        return m_setScanStateChangeCb;
    }

    bool AddHotspotChangeCb(const HotspotStateChangeCb cb) {
        if (m_setHotspotChangeCb.empty()) {
            if (!SubscribeServiceEvent(WIFI_USUAL_EVENT_HOTSPOT_STATE)) {
                return false;
            }
        }
        return m_setHotspotChangeCb.insert(cb).second;
    }

    void RemoveHotspotChangeCb(const HotspotStateChangeCb cb) {
        m_setHotspotChangeCb.erase(cb);
        if (m_setHotspotChangeCb.empty()) {
            UnsubscribeServiceEvent(WIFI_USUAL_EVENT_HOTSPOT_STATE);
        }
    }

    static std::set<HotspotStateChangeCb> GetHotspotChangeCb() {
        return m_setHotspotChangeCb;
    }

    bool AddHotspotJoinCb(const HotspotJoinCb cb) {
        if (m_setHotspotJoinCb.empty()) {
            if (!SubscribeServiceEvent(WIFI_USUAL_EVENT_STA_JOIN)) {
                return false;
            }
        }
        return m_setHotspotJoinCb.insert(cb).second;
    }

    void RemoveHotspotJoinCb(const HotspotJoinCb cb) {
        m_setHotspotJoinCb.erase(cb);
        if (m_setHotspotJoinCb.empty()) {
            UnsubscribeServiceEvent(WIFI_USUAL_EVENT_STA_JOIN);
        }
    }

    static std::set<HotspotJoinCb> GetHotspotJoinCb() {
        return m_setHotspotJoinCb;
    }

    bool AddHotspotLeaveCb(const HotspotLeaveCb cb) {
        if (m_setHotspotLeaveCb.empty()) {
            if (!SubscribeServiceEvent(WIFI_USUAL_EVENT_STA_LEAVE)) {
                return false;
            }
        }
        return m_setHotspotLeaveCb.insert(cb).second;
    }

    void RemoveHotspotLeaveCb(const HotspotLeaveCb cb) {
        m_setHotspotLeaveCb.erase(cb);
        if (m_setHotspotLeaveCb.empty()) {
            UnsubscribeServiceEvent(WIFI_USUAL_EVENT_STA_LEAVE);
        }
    }

    static std::set<HotspotLeaveCb> GetHotspotLeaveCb() {
        return m_setHotspotLeaveCb;
    }

    class WifiEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
    public:
        explicit WifiEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscribeInfo)
            : CommonEventSubscriber(subscribeInfo) {
        }

        virtual ~WifiEventSubscriber() {
        }

        virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override {
            std::string event = data.GetWant().GetAction();
            int code = data.GetCode();
            WIFI_LOGI("Received event: %{public}s, value: %{public}d", event.c_str(), code);
            if (event == WIFI_USUAL_EVENT_CONN_STATE && !GetConnectionChangeCb().empty()) {
                WifiLinkedInfo linkInfo;
                WifiErrorCode ret = GetLinkedInfo(&linkInfo);
                if (ret != WIFI_SUCCESS) {
                    WIFI_LOGE("Received event get linked info failed");
                    return;
                }
                for (auto& cb : GetConnectionChangeCb()) {
                    cb(code, &linkInfo);
                }
            }

            if (event == WIFI_USUAL_EVENT_SCAN_STATE && !EventManager::m_setScanStateChangeCb.empty()) {
                for (auto& cb : EventManager::m_setScanStateChangeCb) {
                    cb(code, WIFI_SCAN_HOTSPOT_LIMIT);
                }
            }

            if (event == WIFI_USUAL_EVENT_HOTSPOT_STATE && !EventManager::m_setHotspotChangeCb.empty()) {
                for (auto& cb : EventManager::m_setHotspotChangeCb) {
                    cb(code);
                }
            }

            if (event == WIFI_USUAL_EVENT_STA_JOIN && !EventManager::m_setHotspotJoinCb.empty()) {
            }

            if (event == WIFI_USUAL_EVENT_STA_LEAVE && !EventManager::m_setHotspotLeaveCb.empty()) {
            }
        }
    };

    bool SubscribeServiceEvent(const std::string& event) {
        MatchingSkills matchingSkills;
        matchingSkills.AddEvent(event);
        CommonEventSubscribeInfo subscriberInfo(matchingSkills);
        std::shared_ptr<WifiEventSubscriber> subscriber = std::make_shared<WifiEventSubscriber>(subscriberInfo);
        if (subscriber == nullptr) {
            WIFI_LOGE("Create subscriber failed");
            return false;
        }
        WIFI_LOGI("Subscribe event: %{public}s", event.c_str());
        bool subscribeResult = CommonEventManager::SubscribeCommonEvent(subscriber);
        if (subscribeResult) {
            m_mapEventSubscriber[event] = subscriber;
        } else {
            WIFI_LOGE("Subscribe service event fail: %{public}s", event.c_str());
        }
        return subscribeResult;
    }

    bool UnsubscribeServiceEvent(const std::string& event) {
        std::map<std::string, std::shared_ptr<WifiEventSubscriber>>::iterator iter;
        iter = m_mapEventSubscriber.find(event);
        if (iter == m_mapEventSubscriber.end()) {
            return false;
        }

        bool unsubscribeResult = CommonEventManager::SubscribeCommonEvent(iter->second);
        if (!unsubscribeResult) {
            WIFI_LOGE("Unsubscribe event fail: %{public}s", event.c_str());
        }
        return unsubscribeResult;
    }

private:
    static std::set<ConnectionChangeCb> m_setConnectionChangeCb;
    static std::set<ScanStateChangeCb> m_setScanStateChangeCb;
    static std::set<HotspotStateChangeCb> m_setHotspotChangeCb;
    static std::set<HotspotJoinCb> m_setHotspotJoinCb;
    static std::set<HotspotLeaveCb> m_setHotspotLeaveCb;
    std::map<std::string, std::shared_ptr<WifiEventSubscriber> > m_mapEventSubscriber;
};
std::set<ConnectionChangeCb> EventManager::m_setConnectionChangeCb;
std::set<ScanStateChangeCb> EventManager::m_setScanStateChangeCb;
std::set<HotspotStateChangeCb> EventManager::m_setHotspotChangeCb;
std::set<HotspotJoinCb> EventManager::m_setHotspotJoinCb;
std::set<HotspotLeaveCb> EventManager::m_setHotspotLeaveCb;

static EventManager g_eventManager;

WifiErrorCode RegisterWifiEvent(WifiEvent *event)
{
    WIFI_LOGI("Register wifi event");
    if (event == nullptr) {
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (event->OnWifiConnectionChanged != nullptr) {
        return g_eventManager.AddConnectionChangeCb(event->OnWifiConnectionChanged)
            ? WIFI_SUCCESS : ERROR_WIFI_INVALID_ARGS;
    }
    if (event->OnWifiScanStateChanged != nullptr) {
        return g_eventManager.AddScanStateChangeCb(event->OnWifiScanStateChanged)
            ? WIFI_SUCCESS : ERROR_WIFI_INVALID_ARGS;
    }
    if (event->OnHotspotStateChanged != nullptr) {
        return g_eventManager.AddHotspotChangeCb(event->OnHotspotStateChanged)
            ? WIFI_SUCCESS : ERROR_WIFI_INVALID_ARGS;
    }
    if (event->OnHotspotStaJoin != nullptr) {
        return g_eventManager.AddHotspotJoinCb(event->OnHotspotStaJoin)
            ? WIFI_SUCCESS : ERROR_WIFI_INVALID_ARGS;
    }
    if (event->OnHotspotStaLeave != nullptr) {
        return g_eventManager.AddHotspotLeaveCb(event->OnHotspotStaLeave)
            ? WIFI_SUCCESS : ERROR_WIFI_INVALID_ARGS;
    }
    return ERROR_WIFI_INVALID_ARGS;
}

WifiErrorCode UnRegisterWifiEvent(const WifiEvent *event)
{
    WIFI_LOGI("Unregister wifi event");
    if (event == nullptr) {
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (event->OnWifiConnectionChanged != nullptr) {
        g_eventManager.RemoveConnectionChangeCb(event->OnWifiConnectionChanged);
    }
    if (event->OnWifiScanStateChanged != nullptr) {
        g_eventManager.RemoveScanStateChangeCb(event->OnWifiScanStateChanged);
    }
    if (event->OnHotspotStateChanged != nullptr) {
        g_eventManager.RemoveHotspotChangeCb(event->OnHotspotStateChanged);
    }
    if (event->OnHotspotStaJoin != nullptr) {
        g_eventManager.RemoveHotspotJoinCb(event->OnHotspotStaJoin);
    }
    if (event->OnHotspotStaLeave != nullptr) {
        g_eventManager.RemoveHotspotLeaveCb(event->OnHotspotStaLeave);
    }
    return WIFI_SUCCESS;
}
