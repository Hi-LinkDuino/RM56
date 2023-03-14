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

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "bt_def.h"
#include "dispatcher.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief request status
 * use to
 * StatusUpdate().
 */
enum class RequestStatus : int { CONNECT_ON, CONNECT_OFF, SCO_ON, SCO_OFF, BUSY, IDLE };

/**
 * @brief power mode
 * use to
 * GetPowerMode().
 */
enum class BTPowerMode : int {
    MODE_INVALID = 0x00,
    MODE_ACTIVE = 0x100,
    MODE_SNIFF_LEVEL_LOW = 0x201,
    MODE_SNIFF_LEVEL_MID = 0x202,
    MODE_SNIFF_LEVEL_HIG = 0x203,
};

/**
 * @brief Represents power manager interface.
 *
 * @since 6
 */
class IPowerManager {
public:
    virtual ~IPowerManager() = default;

    /**
     * @brief Get power manager singleton interface reference.
     *
     * @return Returns the singleton interface reference.
     * @since 6
     */
    static IPowerManager &GetInstance();

    /**
     * @brief initialize power manager.
     *
     * @param dispatcher dispatcher.
     * @since 6
     */
    static void Initialize(utility::Dispatcher &dispatcher);

    /**
     * @brief Uninitialize power manager.
     *
     * @since 6
     */
    static void Uninitialize();

    /**
     * @brief enable power manager.
     *
     * @param dispatcher dispatcher.
     * @since 6
     */
    virtual void Enable() = 0;

    /**
     * @brief disable power manager.
     *
     * @since 6
     */
    virtual void Disable() = 0;

    /**
     * @brief Update profile connect status.
     *
     * @param status Profile Status.
     * @param profileName Profile Name.
     * @param addr Peer Address.
     * @since 6
     */
    virtual void StatusUpdate(
        const RequestStatus status, const std::string &profileName, const RawAddress &addr) const = 0;

    /**
     * @brief Get power mode.
     *
     * @param address Device address.
     * @return Returns power mode grade.
     *         BTPowerMode::MODE_INVALID = 0x00,
     *         BTPowerMode::MODE_ACTIVE = 0x100,
     *         BTPowerMode::MODE_SNIFF_LEVEL_LOW = 0x201,
     *         BTPowerMode::MODE_SNIFF_LEVEL_MID = 0x202,
     *         BTPowerMode::MODE_SNIFF_LEVEL_HIG = 0x203,
     * @since 6
     */
    virtual BTPowerMode GetPowerMode(const RawAddress &addr) const = 0;
};

/**
 * @brief Represents power manager.
 *
 * @since 6
 */
class PowerManager : public IPowerManager {
public:
    /**
     * @brief Construct PowerManager object.
     *
     * @since 6
     */
    explicit PowerManager(utility::Dispatcher &dispatcher);

    /**
     * @brief Destruct PowerManager object.
     *
     * @since 6
     */
    ~PowerManager();

    /**
     * @brief Get power manager singleton object reference.
     *
     * @return Returns the singleton object reference.
     * @since 6
     */
    static PowerManager &GetInstance();

    /**
     * @brief initialize power manager.
     *
     * @param dispatcher dispatcher.
     * @since 6
     */
    static void Initialize(utility::Dispatcher &dispatcher);

    /**
     * @brief Uninitialize power manager.
     *
     * @since 6
     */
    static void Uninitialize();

    /**
     * @brief enable power manager.
     *
     * @param dispatcher dispatcher.
     * @since 6
     */
    void Enable() override;

    /**
     * @brief disable power manager.
     *
     * @since 6
     */
    void Disable() override;

    /**
     * @brief Update profile connect status.
     *
     * @param status Profile Status.
     * @param profileName Profile Name.
     * @param addr Peer Address.
     * @since 6
     */
    void StatusUpdate(RequestStatus status, const std::string &profileName, const RawAddress &addr) const override;

    /**
     * @brief Get power mode.
     *
     * @param address Device address.
     * @return Returns power mode grade.
     *         BTPowerMode::MODE_INVALID = 0x00,
     *         BTPowerMode::MODE_ACTIVE = 0x100,
     *         BTPowerMode::MODE_SNIFF_LEVEL_LOW = 0x201,
     *         BTPowerMode::MODE_SNIFF_LEVEL_MID = 0x202,
     *         BTPowerMode::MODE_SNIFF_LEVEL_HIG = 0x203,
     * @since 6
     */
    BTPowerMode GetPowerMode(const RawAddress &addr) const override;

private:
    BT_DISALLOW_COPY_AND_ASSIGN(PowerManager);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // POWER_MANAGER_H