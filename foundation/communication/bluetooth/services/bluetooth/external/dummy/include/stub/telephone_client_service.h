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

#ifndef TELEPHONE_CLIENT_SERVICE_H
#define TELEPHONE_CLIENT_SERVICE_H

#include <list>
#include <string>
#include <memory>
#include <mutex>

namespace stub {
static const int HFP_STUB_VOLUME_TYPE_SPEAKER = 0;
static const int HFP_STUB_VOLUME_TYPE_MICROPHONE = 1;

class TelephoneClientServiceObserver {
public:
    virtual ~TelephoneClientServiceObserver() = default;
    virtual void OnBatteryLevelChanged(int batteryLevel)
    {}
    virtual void OnVolumeChanged(int type, int volume)
    {}
    virtual void OnEnhancedDriverSafety(int state)
    {}
    virtual void OnVolumeChangedForAutotest(int type, int volume)
    {}
};

class TelePhoneClientService {
public:
    static TelePhoneClientService& GetInstance();

    void SetStreamVolume(int type, int volume)
    {}
    int GetEnhancedDriverSafetyStatus()
    {
        return 0;
    }
    void SetEnhancedDriverSafetyStatus(int status)
    {}
    void SetHfVolume(int type, int volume)
    {}
    int GetStreamVolume(int type)
    {
        return 0;
    }
    void SetBatteryLevel(int level)
    {}
    int GetBatteryLevel()
    {
        return 0;
    }
    bool SetAudioParameters(const std::string &parameters)
    {
        return false;
    }
    void RegisterObserver(TelephoneClientServiceObserver* observer)
    {}
    void DeregisterObserver(TelephoneClientServiceObserver* observer)
    {}
    void NotifyBatteryLevel(int batteryLevel)
    {}
    void NotifyVolumeChanged(int type, int volume)
    {}
    void NotifyEnhancedDriverSafety(int state)
    {}
    void NotifyVolumeChangedForAutotest(int type, int volume)
    {}

private:
    TelePhoneClientService() = default;
    ~TelePhoneClientService() = default;

    TelePhoneClientService(const TelePhoneClientService &) = delete;
    TelePhoneClientService &operator=(const TelePhoneClientService &) = delete;
};
} // namespace stub
#endif // TELEPHONE_CLIENT_SERVICE_H