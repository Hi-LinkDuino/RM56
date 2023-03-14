/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_THREAD_TEST_H
#define BATTERY_THREAD_TEST_H

#include "battery_thread.h"
#include "charger_thread.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
    struct BatteryThreadUnitTest {};
    struct BatteryConfigUnitTest {};
    struct ChargerThreadUnitTest {};
}

int32_t OpenUeventSocketTest(BatteryThread& bthread);
void UpdateEpollIntervalTest(const int32_t chargeState, BatteryThread& bthread);
int GetEpollIntervalTest(BatteryThread& bthread);
int32_t InitTest(void* service, BatteryThread& bthread);
int32_t GetEpollFdTest(BatteryThread& bthread);
int32_t InitTimerTest(BatteryThread& bthread);
void SetTimerFdTest(const int32_t fd, BatteryThread& bthread);
int32_t GetTimerFdTest(BatteryThread& bthread);
void SetTimerIntervalTest(const int interval, BatteryThread& bthread);
int32_t GetTimerIntervalTest(BatteryThread& bthread);
int32_t InitUeventTest(BatteryThread& bthread);
int32_t GetUeventFdTest(BatteryThread& bthread);
void ParseConfigTest(const std::string filename, BatteryConfig& bconfig);
void ChargerThreadInitTest(ChargerThread& cthread);
void CycleMattersTest(ChargerThread& cthread);
bool GetBatteryInfoTest(ChargerThread& cthread);
void SetChargeStateTest(const int32_t state, ChargerThread& cthread);
int32_t GetChargeStateTest(ChargerThread& cthread);
void HandleCapacityTest(const int32_t& capacity, ChargerThread& cthread);
void ChargerThreadHandleTemperatureTest(const int32_t& temperature, ChargerThread& cthread);

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class OpenUeventSocketImplement {
    friend int32_t OpenUeventSocketTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class UpdateEpollIntervalImplement {
    friend void UpdateEpollIntervalTest(const int32_t chargeState, BatteryThread& bthread)
    {
        (bthread.*privateFun)(chargeState);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetEpollIntervalImplement {
    friend int GetEpollIntervalTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class InitImplement {
    friend int32_t InitTest(void* service, BatteryThread& bthread)
    {
        return (bthread.*privateFun)(service);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetEpollFdImplement {
    friend int32_t GetEpollFdTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class InitTimerImplement {
    friend int32_t InitTimerTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetTimerFdImplement {
    friend int32_t GetTimerFdTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class SetTimerFdImplement {
    friend void SetTimerFdTest(const int32_t fd, BatteryThread& bthread)
    {
        (bthread.*privateFun) = fd;
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetTimerIntervalImplement {
    friend int32_t GetTimerIntervalTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class SetTimerIntervalImplement {
    friend void SetTimerIntervalTest(const int interval, BatteryThread& bthread)
    {
        (bthread.*privateFun) = interval;
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class InitUeventImplement {
    friend int32_t InitUeventTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class ChargerThreadInitImplement {
    friend void ChargerThreadInitTest(ChargerThread& cthread)
    {
        (cthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetUeventFdImplement {
    friend int32_t GetUeventFdTest(BatteryThread& bthread)
    {
        return (bthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class ParseConfigImplement {
    friend void ParseConfigTest(const std::string filename, BatteryConfig& bconfig)
    {
        return (bconfig.*privateFun)(filename);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class CycleMattersImplement {
    friend void CycleMattersTest(ChargerThread& cthread)
    {
        (cthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetBatteryInfoImplement {
    friend bool GetBatteryInfoTest(ChargerThread& cthread)
    {
        return (cthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class HandleCapacityImplement {
    friend void HandleCapacityTest(const int32_t& capacity, ChargerThread& cthread)
    {
        (cthread.*privateFun)(capacity);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetChargeStateImplement {
    friend int32_t GetChargeStateTest(ChargerThread& cthread)
    {
        return (cthread.*privateFun);
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class SetChargeStateImplement {
    friend void SetChargeStateTest(const int32_t state, ChargerThread& cthread)
    {
        (cthread.*privateFun) = state;
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class ChargerThreadHandleTemperatureImplement {
    friend void ChargerThreadHandleTemperatureTest(const int32_t& temperature, ChargerThread& cthread)
    {
        (cthread.*privateFun)(temperature);
    }
};

template class OpenUeventSocketImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::OpenUeventSocket),
    &BatteryThread::OpenUeventSocket
>;

template class UpdateEpollIntervalImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::UpdateEpollInterval),
    &BatteryThread::UpdateEpollInterval
>;

template class GetEpollIntervalImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::epollInterval_),
    &BatteryThread::epollInterval_
>;

template class InitImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::Init),
    &BatteryThread::Init
>;

template class GetEpollFdImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::epFd_),
    &BatteryThread::epFd_
>;

template class InitTimerImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::InitTimer),
    &BatteryThread::InitTimer
>;

template class GetTimerFdImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::timerFd_),
    &BatteryThread::timerFd_
>;

template class SetTimerFdImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::timerFd_),
    &BatteryThread::timerFd_
>;

template class GetTimerIntervalImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::timerInterval_),
    &BatteryThread::timerInterval_
>;

template class SetTimerIntervalImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::timerInterval_),
    &BatteryThread::timerInterval_
>;

template class InitUeventImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::InitUevent),
    &BatteryThread::InitUevent
>;

template class ChargerThreadInitImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::Init),
    &ChargerThread::Init
>;

template class CycleMattersImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::CycleMatters),
    &ChargerThread::CycleMatters
>;

template class GetUeventFdImplement <
    BatteryThreadUnitTest,
    decltype(&BatteryThread::ueventFd_),
    &BatteryThread::ueventFd_
>;

template class ParseConfigImplement <
    BatteryConfigUnitTest,
    decltype(&BatteryConfig::ParseConfig),
    &BatteryConfig::ParseConfig
>;

template class GetBatteryInfoImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::started_),
    &ChargerThread::started_
>;

template class HandleCapacityImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::HandleCapacity),
    &ChargerThread::HandleCapacity
>;

template class GetChargeStateImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::chargeState_),
    &ChargerThread::chargeState_
>;

template class SetChargeStateImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::chargeState_),
    &ChargerThread::chargeState_
>;

template class ChargerThreadHandleTemperatureImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::HandleTemperature),
    &ChargerThread::HandleTemperature
>;

class BatteryThreadTest {
public:
    void SetKeyState(int code, int value, int64_t now, ChargerThread& ct)
    {
        ct.SetKeyState(code, value, now);
    }

    void HandlePowerKey(int keycode, int64_t now, ChargerThread& ct)
    {
        ct.HandlePowerKey(keycode, now);
    }

    void UpdateBatteryInfo(void* arg, ChargerThread& ct)
    {
        ct.UpdateBatteryInfo(arg);
    }

    void Init(ChargerThread& ct)
    {
        ct.Init();
    }
};
} // namespace V1_0
} // namespace Battery
} // namespace HDI
} // namespace OHOS
#endif // BATTERY_THREAD_TEST_H
