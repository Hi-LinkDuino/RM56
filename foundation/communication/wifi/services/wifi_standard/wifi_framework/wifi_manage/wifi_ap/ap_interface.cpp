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
#include "ap_interface.h"
#include "wifi_ap_nat_manager.h"

namespace OHOS {
namespace Wifi {
ApInterface::ApInterface()
    : m_ApRootState(),
      m_ApStartedState(m_ApStateMachine, m_ApConfigUse, m_ApMonitor),
      m_ApIdleState(m_ApStateMachine),
      m_ApMonitor(),
      m_ApStateMachine(m_ApStationsManager, m_ApRootState, m_ApIdleState, m_ApStartedState, m_ApMonitor),
      m_ApService(m_ApStateMachine),
      m_ApStationsManager(),
      m_ApConfigUse()
{}

ApInterface::~ApInterface()
{}

extern "C" IApService *Create(void)
{
    return new ApInterface();
}

extern "C" void Destroy(IApService *pservice)
{
    delete pservice;
    pservice = nullptr;
}
ErrCode ApInterface::EnableHotspot()
{
    return m_ApService.EnableHotspot();
}

ErrCode ApInterface::DisableHotspot()
{
    return m_ApService.DisableHotspot();
}

ErrCode ApInterface::AddBlockList(const StationInfo &stationInfo)
{
    return m_ApService.AddBlockList(stationInfo);
}

ErrCode ApInterface::DelBlockList(const StationInfo &stationInfo)
{
    return m_ApService.DelBlockList(stationInfo);
}

ErrCode ApInterface::SetHotspotConfig(const HotspotConfig &hotspotConfig)
{
    return m_ApService.SetHotspotConfig(hotspotConfig);
}

ErrCode ApInterface::DisconnetStation(const StationInfo &stationInfo)
{
    return m_ApService.DisconnetStation(stationInfo);
}

ErrCode ApInterface::GetStationList(std::vector<StationInfo> &result)
{
    return m_ApService.GetStationList(result);
}

ErrCode ApInterface::GetValidBands(std::vector<BandType> &bands)
{
    return m_ApService.GetValidBands(bands);
}

ErrCode ApInterface::GetValidChannels(BandType band, std::vector<int32_t> &validchannel)
{
    return m_ApService.GetValidChannels(band, validchannel);
}

ErrCode ApInterface::RegisterApServiceCallbacks(const IApServiceCallbacks &callbacks)
{
    return m_ApService.RegisterApServiceCallbacks(callbacks);
}
}  // namespace Wifi
}  // namespace OHOS