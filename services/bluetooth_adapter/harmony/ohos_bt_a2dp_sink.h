/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: A2DP sink interfaces
 */

#ifndef OHOS_BT_A2DP_SINK_H
#define OHOS_BT_A2DP_SINK_H

#include "ohos_bt_def.h"

/* Callback invoked in response to device's connection state changed. connState-One of BtProfileConnectState */
typedef void (*A2dpSinkConnectStateChangedCallback)(BdAddr *bdAddr, int connState);

/* Callback invoked in response to device's playing state changed. playingState-BtA2dpPlayingState */
typedef void (*A2dpSinkPlayingStateChangedCallback)(BdAddr *bdAddr, int playingState);

typedef struct {
    A2dpSinkConnectStateChangedCallback connectstateChangedCb;
    A2dpSinkPlayingStateChangedCallback playingStateChangedCb;
} BtA2dpSinkCallbacks;

/*
 * @brief Get device connection state by address.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @return One of BtProfileConnectState, connecting/connected/disconnecting/disconnected
 */
int A2dpSinkGetDeviceConnectState(const BdAddr *bdAddr);

/*
 * @brief Get device playing state by address when target device is on connected state.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @return BtA2dpPlayingState, OHOS_A2DP_NOT_PLAYING/OHOS_A2DP_IS_PLAYING.
 */
int A2dpSinkGetPlayingState(const BdAddr *bdAddr);

/*
 * @brief Connect to the peer bluetooth device.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @return 0-(success)Perform normal connection processing.
 *         other-(fail)Target device is on connected/connecting state,
 *           or device is not allowed to connect, or connect fails.
 */
int A2dpSinkConnect(const BdAddr *bdAddr);

/*
 * @brief Disconnect with the peer bluetooth service.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @return 0-(success)Perform normal disconnect processing.
 *         other-(fail)Target device is on disconnected/disconnecting state, or disconnect fails.
 */
int A2dpSinkDisconnect(const BdAddr *bdAddr);

/*
 * @brief Set connection strategy for peer bluetooth device.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @param[in] <strategy> BtConnectStrategyType, unknown/allowed/forbidden.
 * @return 0-(success), other-(fail)
 */
int A2dpSinkSetConnectStrategy(const BdAddr *bdAddr, int strategy);

/*
 * @brief Get connection strategy of peer bluetooth device.
 * @param[in] <bdAddr> The address of the peer bluetooth device.
 * @return BtConnectStrategyType, unknown/allowed/forbidden.
 */
int A2dpSinkGetConnectStrategy(const BdAddr *bdAddr);

/*
 * @brief Register callback function of framework.
 * @param[in] <func> Callback funcs
 * @return 0-success, other-fail
 */
int A2dpSinkRegisterCallbacks(BtA2dpSinkCallbacks *func);

/*
 * @brief Deregister callback function of framework.
 * @param[in] <func> Callback funcs
 * @return 0-success, other-fail
 */
int A2dpSinkDeregisterCallbacks(BtA2dpSinkCallbacks *func);
#endif