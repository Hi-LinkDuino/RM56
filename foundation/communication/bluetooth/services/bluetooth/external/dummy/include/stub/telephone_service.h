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

#ifndef TELEPHONE_SERVICE_H
#define TELEPHONE_SERVICE_H

#include <list>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace stub {
typedef struct {
    int index = 0;
    int direction = 0;
    int status = 0;
    int mode = 0;
    bool mpty = false;
    std::string number = "";
    int type = 0;
}CurrentCalls;

class TelephoneServiceObserver {
public:
    virtual ~TelephoneServiceObserver() = default;
    virtual void OnRegistrationStatusChanged(int status)
    {}
    virtual void OnSignalStrengthChanged(int signalStrength)
    {}
    virtual void OnRoamingStatusChanged(int status)
    {}
    virtual void OnBatteryLevelChanged(int batteryLevel)
    {}
    virtual void OnVolumeChanged(int type, int volume)
    {}
    virtual void RejectCall()
    {}
    virtual void DialOutCall(std::string number)
    {}
    virtual void HangupCall()
    {}
    virtual void AnswerCall()
    {}
    virtual void SendDtmf(int dtmf)
    {}
    virtual void ProcessChld(int chld)
    {}
    virtual void OnVolumeChangedForAutotest(int type, int volume)
    {}
};

class TelephoneService {
public:
    static TelephoneService *GetInstance();
    int GetPhoneType()
    {
        return 0;
    }
    std::string GetPhoneNumber()
    {
        return "";
    }
    std::vector<std::string> GetTelephoneAccount()
    {
        std::vector<std::string> ret;
        return ret;
    }
    void RejectCall()
    {}
    void DialOutCall(const std::string &number)
    {}
    void HangupCall()
    {}
    void AnswerCall()
    {}
    bool SendDtmf(int dtmf)
    {
        return false;
    }
    bool ProcessChld(int chld)
    {
        return false;
    }
    std::string GetNetworkOperator()
    {
        return "";
    }
    std::string GetSubscriberNumber()
    {
        return "";
    }
    std::string GetLastDialNumber()
    {
        return "";
    }
    bool ListCurrentCalls()
    {
        return false;
    }
    void QueryPhoneState()
    {}
    bool StartVoiceRecognition(const std::string &address)
    {
        return false;
    }
    bool StopVoiceRecogition(const std::string &address)
    {
        return false;
    }
    bool SetAudioParameters(const std::string &parameters)
    {
        return false;
    }
    bool SetStreamVolume(int type, int volume, int flag)
    {
        return false;
    }
    bool SetHfVolume(int type, int volume)
    {
        return false;
    }
    int GetRegistrationStatus()
    {
        return 0;
    }
    int GetSignalStrength()
    {
        return 0;
    }
    int GetRoamingStatus()
    {
        return 0;
    }
    int GetBatteryLevel()
    {
        return 0;
    }
    void NotifyHfBatteryLevel(int level)
    {}
    void NotifyHfEnhancedDriverSafety(int status)
    {}
    void SetPhoneState(int numActive, int numHeld, int callState,
        const std::string &number, int type, const std::string &name)
    {}
    void SetCurrentCalls(CurrentCalls &call)
    {}
    void RemoveCall(int index)
    {}
    void ShowCallList()
    {}
    void ClearCallList()
    {}
    void SetOperatorName(std::string &name)
    {}
    void SetSubscriberNumber(std::string &number)
    {}
    void SetLastDialNumber(std::string &number)
    {}
    void NotifyCurrentCalls()
    {}
    void SetRegistrationStatus(int registrationStatus)
    {}
    void SetSignalStrength(int signalStrength)
    {}
    void SetRoamingStatus(int roamingStatus)
    {}
    void SetBatteryLevel(int batteryLevel)
    {}
    void RegisterObserver(TelephoneServiceObserver *observer)
    {}
    void DeregisterObserver(TelephoneServiceObserver *observer)
    {}
    void NotifyRegistrationStatusChanged(int status)
    {}
    void NotifySignalStrengthChanged(int signalStrength)
    {}
    void NotifyRoamingStatusChanged(int status)
    {}
    void NotifyBatteryLevelChanged(int batteryLevel)
    {}
    void NotifyVolumeChanged(int type, int volume)
    {}
    void NotifyRejectCall()
    {}
    void NotifyDialOutCall(std::string number)
    {}
    void NotifyHangupCall()
    {}
    void NotifyAnswerCall()
    {}
    void NotifySendDtmf(int dtmf)
    {}
    void NotifyProcessChld(int chld)
    {}
    void NotifyVolumeChangedForAutotest(int type, int volume)
    {}

private:
    TelephoneService() = default;
    ~TelephoneService() = default;

    TelephoneService(const TelephoneService &) = delete;
    TelephoneService &operator=(const TelephoneService &) = delete;
};
} // namespace stub
#endif // TELEPHONE_SERVICE_H