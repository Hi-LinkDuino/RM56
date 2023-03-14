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

#ifndef POWER_DEVICE_H
#define POWER_DEVICE_H

#include "dispatcher.h"
#include "power_spec.h"
#include "raw_address.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents power device.
 *
 * @since 6
 */
class PowerDevice : public std::enable_shared_from_this<PowerDevice> {
public:
    /**
     * @brief Construct PowerDevice object.
     *
     * @param addr Peer device bluetooth addr.
     * @param dispatcher Reference of dispatcher resource.
     * @since 6
     */
    PowerDevice(const RawAddress rawAddr, utility::Dispatcher &dispatcher);

    /**
     * @brief Delete default Construct function.
     *
     * @since 6
     */
    PowerDevice() = delete;

    /**
     * @brief Destruct PowerDevice object.
     *
     * @since 6
     */
    ~PowerDevice();

    /**
     * @brief Update Request Power Map, update profile status.
     *
     * @param profileName Profile Name.
     * @param status Profile Status.
     * @since 6
     */
    void SetRequestPower(const std::string &profileName, const RequestStatus status) const;

    /**
     * @brief Update Request Power Map, delete profile status.
     *
     * @param profileName Profile Name.
     * @since 6
     */
    void DeleteRequestPower(const std::string &profileName) const;

    /**
     * @brief Set Power Mode, base on current connecting profiles and their status.
     *
     * @since 6
     */
    void SetPowerMode();

    /**
     * @brief Get power mode.
     *
     * @return Returns power mode grade.
     *         BTPowerMode::MODE_INVALID = 0x00,
     *         BTPowerMode::MODE_ACTIVE = 0x100,
     *         BTPowerMode::MODE_SNIFF_LEVEL_LOW = 0x201,
     *         BTPowerMode::MODE_SNIFF_LEVEL_MID = 0x202,
     *         BTPowerMode::MODE_SNIFF_LEVEL_HIG = 0x203,
     * @since 6
     */
    BTPowerMode GetPowerMode() const;

    /**
     * @brief Callback while acl link road power mode state update.
     *
     * @param status Hci status.
     * @param currentMode Current acl link road power mode.
     * @param interval Monitor interval.
     * @since 6
     */
    void ModeChangeCallBack(uint8_t status, uint8_t currentMode, uint16_t interval);

    /**
     * @brief Callback of Complete set sniff subrating params.
     *
     * @param status Hci status.
     * @since 6
     */
    void SniffSubratingCompleteCallback(uint8_t status) const;

private:
    /**
     * @brief Send SetActiveMode message to power state machine.
     *
     * @since 6
     */
    void SetActiveMode();

    /**
     * @brief Start sniffDelayTimer_
     *
     * @param timeoutMs time-out period(ms).
     * @since 6
     */
    void SetSniffMode(PowerInfo requestPower);

    /**
     * @brief Calculate max power mode, base on current profiles and status.
     *
     * @since 6
     */
    PowerInfo CalcMaxPower() const;

    /**
     * @brief Calculate lowest ssr level, base on current profiles and status.
     *
     * @since 6
     */
    PowerSsrLevel CalcLowestSsrLevel() const;

    /**
     * @brief Set sniff subrating params to btm.
     *
     * @param ssrParam Sniff subrating params.
     * @return
     * @since 6
     */
    int BtmSetSniffSubrating(const PowerSsrParam &ssrParam) const;

    /**
     * @brief Btm exit sniff subrating mode.
     *
     * @return
     * @since 6
     */
    int BtmExitSniffMode() const;

    /**
     * @brief Btm enter sniff subrating mode.
     *
     * @param param Sniff params.
     * @return
     * @since 6
     */
    int BtmEnterSniffMode(const PowerParam &param) const;

    /**
     * @brief Get present sniff params, used by power state machine.
     *
     * @return std::pair<PowerSsrLevel, PowerModeLevel>
     * @since 6
     */
    const std::pair<PowerSsrLevel, PowerModeLevel> &GetRequestPowerLevel() const;

    /**
     * @brief Set present sniff params.
     *
     * @param sniffParams std::pair<PowerSsrLevel, PowerModeLevel>.
     * @since 6
     */
    void SetRequestPowerLevel(const PowerSsrLevel ssr, const PowerModeLevel power) const;

    /**
     * @brief update control sniff subrating level.
     *
     * @param ssr PowerSsrLevel.
     * @since 6
     */
    void UpdateControlSniffSubrating(const PowerSsrLevel ssr) const;

    /**
     * @brief update control sniff power level.
     *
     * @param power PowerModeLevel.
     * @since 6
     */
    void UpdatecontrolPowerLevel(const PowerModeLevel powerLevel) const;

    /**
     * @brief Get present sniff params, used by power state machine.
     *
     * @return std::pair<PowerSsrLevel, PowerModeLevel>
     * @since 6
     */
    const std::pair<PowerSsrLevel, PowerModeLevel> &GetControlPowerLevel() const;

    /**
     * @brief Sniff Delay Timer callback.
     *
     * @since 6
     */
    static void DelayTimeoutCallback(const std::weak_ptr<PowerDevice>& weakDevice);

    /**
     * @brief Start Sniff Delay Timer.
     *
     * @param ms Time-out period(ms).
     * @since 6
     */
    void StartDelayTimer(int ms) const;

    /**
     * @brief Stop Sniff Delay Timer.
     *
     * @since 6
     */
    void StopDelayTimer() const;

    /**
     * @brief Get current delaytimer remain time(ms).
     *
     * @return Remain time(ms).
     * @since 6
     */
    uint64_t GetDelayTimerRemainMs() const;

    class PowerTimer : public utility::Timer {
    public:
        /**
         * @brief Construct PowerTimer object.
         *
         * @param callback PowerTimer timeout callback.
         * @since 6
         */
        explicit PowerTimer(const std::function<void()> &callback) : utility::Timer(callback){};

        /**
         * @brief Delete PowerTimer default Construct function.
         *
         * @since 6
         */
        PowerTimer() = delete;

        /**
         * @brief Start PowerTimer.
         *
         * @param ms Time-out period(ms).
         * @param isPeriodic Whether PowerTimer is periodic.
         * @return Returns <b>true</b> if the operation is successful;
         *         returns <b>false</b> if the operation fails.
         * @since 6
         */
        bool Start(int ms, bool isPeriodic = false);

        /**
         * @brief Stop PowerTimer.
         *
         * @return Returns <b>true</b> if the operation is successful;
         *         returns <b>false</b> if the operation fails.
         * @since 6
         */
        bool Stop();

        /**
         * @brief Get PowerTimer remain time(ms).
         *
         * @return Return remain time(ms).
         * @since 6
         */
        uint64_t GetRemainMs();

    private:
#define MS_PER_SECOND 1000
#define NS_PER_MS 1000000

        uint64_t deadLineMs_ = 0;
        BT_DISALLOW_COPY_AND_ASSIGN(PowerTimer);
    };

    friend class PowerActiveActivingState;
    friend class PowerActiveSniffingState;
    friend class PowerSniffActivingState;
    friend class PowerSniffSniffingState;
    friend class PowerSniffState;

    BT_DISALLOW_COPY_AND_ASSIGN(PowerDevice);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // POWER_DEVICE_H