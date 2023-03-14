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

import {Callback} from "./basic";
import radio from "./@ohos.telephony.radio";
import data from "./@ohos.telephony.data";
import call from "./@ohos.telephony.call";
import sim from "./@ohos.telephony.sim";

/**
 * Monitors telephony state updates of a device, including updates of the network state,
 * signal strength, call state, the data link connection state and others.
 *
 * @since 6
 * @syscap SystemCapability.Telephony.StateRegistry
 */
declare namespace observer {
  type NetworkState = radio.NetworkState;
  type SignalInformation = radio.SignalInformation;
  type CellInformation = radio.CellInformation;
  type DataConnectState = data.DataConnectState;
  type RatType = radio.RadioTechnology;
  type DataFlowType = data.DataFlowType;
  type CallState = call.CallState;
  type CardType = sim.CardType;
  type SimState = sim.SimState;

  /**
   * Called when the network state corresponding to a monitored {@code slotId} updates.
   *
   * @param type networkStateChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including an instance of the {@code NetworkState} class.
   * @permission ohos.permission.GET_NETWORK_INFO
   */
  function on(type: 'networkStateChange', callback: Callback<NetworkState>): void;
  function on(type: 'networkStateChange', options: { slotId: number }, callback: Callback<NetworkState>): void;

  function off(type: 'networkStateChange', callback?: Callback<NetworkState>): void;

  /**
   * Called when the signal strength corresponding to a monitored {@code slotId} updates.
   *
   * @param type signalInfoChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including an array of instances of the classes derived from {@link SignalInformation}.
   */
  function on(type: 'signalInfoChange', callback: Callback<Array<SignalInformation>>): void;
  function on(type: 'signalInfoChange', options: { slotId: number },
    callback: Callback<Array<SignalInformation>>): void;

  function off(type: 'signalInfoChange', callback?: Callback<Array<SignalInformation>>): void;

  /**
   * Called back when the cell information corresponding to a monitored {@code slotId} updates.
   *
   * @param type cellInfoChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including an array of instances of the classes derived from {@link CellInformation}.
   * @permission ohos.permission.LOCATION
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function on(type: 'cellInfoChange', callback: Callback<Array<CellInformation>>): void;
  function on(type: 'cellInfoChange', options: { slotId: number },
    callback: Callback<Array<CellInformation>>): void;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function off(type: 'cellInfoChange', callback?: Callback<Array<CellInformation>>): void;

  /**
   * Called when the cellular data link connection state updates.
   *
   * @param type cellularDataConnectionStateChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including state Indicates the cellular data link connection state,
   *   and networkType Indicates the radio access technology for cellular data services.
   * @since 7
   */
  function on(type: 'cellularDataConnectionStateChange',
    callback: Callback<{ state: DataConnectState, network: RatType }>): void;
  function on(type: 'cellularDataConnectionStateChange', options: { slotId: number },
    callback: Callback<{ state: DataConnectState, network: RatType }>): void;

  /**
   * @since 7
   */
  function off(type: 'cellularDataConnectionStateChange',
    callback?: Callback<{ state: DataConnectState, network: RatType }>): void;

  /**
   * Called when the uplink and downlink data flow state of cellular data services updates.
   *
   * @param type cellularDataFlowChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including the cellular data flow state.
   * @since 7
   */
  function on(type: 'cellularDataFlowChange', callback: Callback<DataFlowType>): void;
  function on(type: 'cellularDataFlowChange', options: { slotId: number },
    callback: Callback<DataFlowType>): void;

  /**
   * @since 7
   */
  function off(type: 'cellularDataFlowChange', callback?: Callback<DataFlowType>): void;

  /**
   * Receives a call state change. This callback is invoked when the call state of a specified card updates
   * and the observer is added to monitor the updates.
   *
   * @param type callStateChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including state Indicates the call state, and number Indicates the called number.
   *   The value of number is an empty string if the application does not have
   *     the ohos.permission.READ_CALL_LOG permission.
   */
  function on(type: 'callStateChange', callback: Callback<{ state: CallState, number: string }>): void;
  function on(type: 'callStateChange', options: { slotId: number },
    callback: Callback<{ state: CallState, number: string }>): void;

  function off(type: 'callStateChange', callback?: Callback<{ state: CallState, number: string }>): void;

  /**
   * Receives a sim state change. This callback is invoked when the sim state of a specified card updates
   * and the observer is added to monitor the updates.
   *
   * @param type simStateChange
   * @param options including slotId Indicates the ID of the target card slot.
   *   The value {@code 0} indicates card 1, and the value {@code 1} indicates card 2.
   * @param callback including state Indicates the sim state, and reason Indicates the cause of the change.
   *   The value of reason is an empty string if the application does not have
   * @since 7
   */
  function on(type: 'simStateChange', callback: Callback<SimStateData>): void;
  function on(type: 'simStateChange', options: { slotId: number }, callback: Callback<SimStateData>): void;

  /**
   * @since 7
   */
  function off(type: 'simStateChange', callback?: Callback<SimStateData>): void;

  /**
   * @since 7
   */
  export interface SimStateData {
    type: CardType;
    state: SimState;
    /**
     * @since 8
     */
    reason: LockReason;
  }

  /**
   * @since 8
   */
  export enum LockReason {
    SIM_NONE,
    SIM_PIN,
    SIM_PUK,
    SIM_PN_PIN, //Network Personalization (refer 3GPP TS 22.022 [33])
    SIM_PN_PUK,
    SIM_PU_PIN, //network sUbset Personalization (refer 3GPP TS 22.022 [33])
    SIM_PU_PUK,
    SIM_PP_PIN, //service Provider Personalization (refer 3GPP TS 22.022 [33])
    SIM_PP_PUK,
    SIM_PC_PIN, //Corporate Personalization (refer 3GPP TS 22.022 [33])
    SIM_PC_PUK,
    SIM_SIM_PIN, //SIM/USIM personalisation (refer 3GPP TS 22.022 [33])
    SIM_SIM_PUK,
  }
}

export default observer;