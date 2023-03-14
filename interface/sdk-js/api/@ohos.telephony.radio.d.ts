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

import {AsyncCallback} from "./basic";

/**
 * Provides interfaces for applications to obtain the network state, cell information, signal information,
 * and device ID of the wireless cellular network (WCN), and provides a callback registration mechanism to
 * listen for changes of the network, cell, and signal status of the WCN.
 *
 * @since 6
 * @syscap SystemCapability.Telephony.CoreService
 */
declare namespace radio {
  /**
   * Obtains radio access technology (RAT) of the registered network. The system
   * returns RAT of the packet service (PS) and circuit service (CS) domain.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns an integer indicating the RAT in use. The values are as follows:
   * <ul>
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_UNKNOWN}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_GSM}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_1XRTT}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_WCDMA}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_HSPA}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_HSPAP}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_TD_SCDMA}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_EVDO}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_EHRPD}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_LTE}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_LTE_CA}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_IWLAN}
   * <li>{@code RadioTechnology#RADIO_TECHNOLOGY_NR}
   * </ul>
   * @permission ohos.permission.GET_NETWORK_INFO
   */
  function getRadioTech(slotId: number,
    callback: AsyncCallback<{psRadioTech: RadioTechnology, csRadioTech: RadioTechnology}>): void;
  function getRadioTech(slotId: number): Promise<{psRadioTech: RadioTechnology, csRadioTech: RadioTechnology}>;

  /**
   * Obtains the network state of the registered network.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns a {@code NetworkState} object.
   * @permission ohos.permission.GET_NETWORK_INFO
   */
  function getNetworkState(callback: AsyncCallback<NetworkState>): void;
  function getNetworkState(slotId: number, callback: AsyncCallback<NetworkState>): void;
  function getNetworkState(slotId?: number): Promise<NetworkState>;

  /**
   * Proactively requests to update location information.
   *
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function sendUpdateCellLocationRequest(callback: AsyncCallback<void>): void;
  function sendUpdateCellLocationRequest(): Promise<void>;

  /**
   * @permission ohos.permission.LOCATION
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getCellInformation(callback: AsyncCallback<Array<CellInformation>>): void;
  function getCellInformation(slotId: number, callback: AsyncCallback<Array<CellInformation>>): void;
  function getCellInformation(slotId?: number): Promise<Array<CellInformation>>;

  /**
   * Obtains the network search mode of the SIM card in a specified slot.
   *
   * @param slotId Indicates the ID of the SIM card slot.
   * @param callback Returns the network search mode of the SIM card. Available values are as follows:
   * <ul>
   * <li>{@link NetworkSelectionMode#NETWORK_SELECTION_UNKNOWN}
   * <li>{@link NetworkSelectionMode#NETWORK_SELECTION_AUTOMATIC}
   * <li>{@link NetworkSelectionMode#NETWORK_SELECTION_MANUAL}
   * <ul>
   */
  function getNetworkSelectionMode(slotId: number, callback: AsyncCallback<NetworkSelectionMode>): void;
  function getNetworkSelectionMode(slotId: number): Promise<NetworkSelectionMode>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   */
  function setNetworkSelectionMode(options: NetworkSelectionModeOptions, callback: AsyncCallback<void>): void;
  function setNetworkSelectionMode(options: NetworkSelectionModeOptions): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   */
  function getNetworkSearchInformation(slotId: number, callback: AsyncCallback<NetworkSearchResult>): void;
  function getNetworkSearchInformation(slotId: number): Promise<NetworkSearchResult>;

  /**
   * Obtains the ISO-defined country code of the country where the registered network is deployed.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the country code defined in ISO 3166-2;
   * returns an empty string if the device is not registered with any network.
   * @since 7
   */
  function getISOCountryCodeForNetwork(slotId: number, callback: AsyncCallback<string>): void;
  function getISOCountryCodeForNetwork(slotId: number): Promise<string>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getNrOptionMode(callback: AsyncCallback<NrOptionMode>): void;
  function getNrOptionMode(slotId: number, callback: AsyncCallback<NrOptionMode>): void;
  function getNrOptionMode(slotId?: number): Promise<NrOptionMode>;

  /**
   * Obtains the IMEI of a specified card slot of the device.
   *
   * @param slotId Indicates the card slot index number, ranging from 0 to the maximum card slot index number
   * supported by the device.
   * @param callback Returns the IMEI; returns an empty string if the IMEI does not exist.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getIMEI(callback: AsyncCallback<string>): void;
  function getIMEI(slotId: number, callback: AsyncCallback<string>): void;
  function getIMEI(slotId?: number): Promise<string>;

  /**
   * Obtains the MEID of a specified card slot of the device.
   *
   * @param slotId Indicates the card slot index number, ranging from 0 to the maximum card slot index number
   * supported by the device.
   * @param callback Returns the MEID; returns an empty string if the MEID does not exist.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getMEID(callback: AsyncCallback<string>): void;
  function getMEID(slotId: number, callback: AsyncCallback<string>): void;
  function getMEID(slotId?: number): Promise<string>;

  /**
   * Obtains the unique device ID of a specified card slot of the device.
   *
   * <p>If the device is registered with a 3GPP-compliant network, the international mobile equipment identity
   * (IMEI) is returned. If the device is registered with a 3GPP2-compliant network, the mobile equipment identifier
   * (MEID) is returned.
   *
   * @param slotId Indicates the card slot index number, ranging from 0 to the maximum card slot index number
   * supported by the device.
   * @param callback Returns the unique device ID; returns an empty string if the unique device ID does not exist.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getUniqueDeviceId(callback: AsyncCallback<string>): void;
  function getUniqueDeviceId(slotId: number, callback: AsyncCallback<string>): void;
  function getUniqueDeviceId(slotId?: number): Promise<string>;

  /**
   * Obtains the index number of the card slot where the primary card is located if multiple SIM cards are inserted.
   *
   * <p>The primary card is the SIM card inserted in the card slot that uses data services by default.
   *
   * @param callback Returns the index number of the primary card slot.
   * @since 7
   */
  function getPrimarySlotId(callback: AsyncCallback<number>): void;
  function getPrimarySlotId(): Promise<number>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setPrimarySlotId(slotId: number, callback: AsyncCallback<void>): void;
  function setPrimarySlotId(slotId: number): Promise<void>;

  /**
   * Obtains the list of signal strength information of the registered network corresponding to a specified SIM card.
   *
   * @param slotId Indicates the card slot index number, ranging from 0 to the maximum card slot index number
   * supported by the device.
   * @param callback Returns the instance list of the child classes derived from {@link SignalInformation}.
   * @since 7
   */
  function getSignalInformation(slotId: number, callback: AsyncCallback<Array<SignalInformation>>): void;
  function getSignalInformation(slotId: number): Promise<Array<SignalInformation>>;

  /**
   * Checks whether the device supports 5G New Radio (NR).
   *
   * @return Returns {@code true} if the device supports 5G NR; returns {@code false} otherwise.
   * @deprecated Advised to use isNrSupported(slotId: number) instead.
   * @since 7
   */
  function isNrSupported(): boolean;

  /**
   * Checks whether the device supports 5G New Radio (NR) by according card slot.
   *
   * @param slotId Indicates the card slot index number, ranging from 0 to the maximum card slot index number
   * supported by the device.
   * @return Returns {@code true} if the device supports 5G NR; returns {@code false} otherwise.
   * @since 8
   */
  function isNrSupported(slotId: number): boolean;

  /**
   * Checks whether the radio service is enabled.
   *
   * @param slotId Indicates the card slot index number,
   *   ranging from 0 to the maximum card slot index number supported by the device.
   * @permission ohos.permission.GET_NETWORK_INFO
   * @since 7
   */
  function isRadioOn(callback: AsyncCallback<boolean>): void;
  function isRadioOn(slotId: number, callback: AsyncCallback<boolean>): void
  function isRadioOn(slotId?: number): Promise<boolean>;

  /**
   * Turn on the radio service.
   *
   * @param slotId Indicates the card slot index number,
   *   ranging from 0 to the maximum card slot index number supported by the device.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function turnOnRadio(callback: AsyncCallback<void>): void;
  function turnOnRadio(slotId: number, callback: AsyncCallback<void>): void;
  function turnOnRadio(slotId?: number): Promise<void>;

  /**
   * Turn off the radio service.
   *
   * @param slotId Indicates the card slot index number,
   *   ranging from 0 to the maximum card slot index number supported by the device.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function turnOffRadio(callback: AsyncCallback<void>): void;
  function turnOffRadio(slotId: number, callback: AsyncCallback<void>): void;
  function turnOffRadio(slotId?: number): Promise<void>;

  /**
   * @since 7
   */
  function getOperatorName(slotId: number, callback: AsyncCallback<string>): void;
  function getOperatorName(slotId: number): Promise<string>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setPreferredNetwork(slotId: number, networkMode: PreferredNetworkMode, callback: AsyncCallback<void>): void;
  function setPreferredNetwork(slotId: number, networkMode: PreferredNetworkMode): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getPreferredNetwork(slotId: number, callback: AsyncCallback<PreferredNetworkMode>): void;
  function getPreferredNetwork(slotId: number): Promise<PreferredNetworkMode>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum PreferredNetworkMode {
    PREFERRED_NETWORK_MODE_GSM = 1,
    PREFERRED_NETWORK_MODE_WCDMA = 2,
    PREFERRED_NETWORK_MODE_LTE = 3,
    PREFERRED_NETWORK_MODE_LTE_WCDMA = 4,
    PREFERRED_NETWORK_MODE_LTE_WCDMA_GSM = 5,
    PREFERRED_NETWORK_MODE_WCDMA_GSM = 6,
    PREFERRED_NETWORK_MODE_CDMA = 7,
    PREFERRED_NETWORK_MODE_EVDO = 8,
    PREFERRED_NETWORK_MODE_EVDO_CDMA = 9,
    PREFERRED_NETWORK_MODE_WCDMA_GSM_EVDO_CDMA = 10,
    PREFERRED_NETWORK_MODE_LTE_EVDO_CDMA = 11,
    PREFERRED_NETWORK_MODE_LTE_WCDMA_GSM_EVDO_CDMA = 12,
    PREFERRED_NETWORK_MODE_TDSCDMA = 13,
    PREFERRED_NETWORK_MODE_TDSCDMA_GSM = 14,
    PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA = 15,
    PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA_GSM = 16,
    PREFERRED_NETWORK_MODE_LTE_TDSCDMA = 17,
    PREFERRED_NETWORK_MODE_LTE_TDSCDMA_GSM = 18,
    PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA = 19,
    PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM = 20,
    PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA_GSM_EVDO_CDMA = 21,
    PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA = 22,
    PREFERRED_NETWORK_MODE_NR = 31,
    PREFERRED_NETWORK_MODE_NR_LTE = 32,
    PREFERRED_NETWORK_MODE_NR_LTE_WCDMA = 33,
    PREFERRED_NETWORK_MODE_NR_LTE_WCDMA_GSM = 34,
    PREFERRED_NETWORK_MODE_NR_LTE_EVDO_CDMA = 35,
    PREFERRED_NETWORK_MODE_NR_LTE_WCDMA_GSM_EVDO_CDMA = 36,
    PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA = 37,
    PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_GSM = 38,
    PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA = 39,
    PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM = 40,
    PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA = 41,
    PREFERRED_NETWORK_MODE_MAX_VALUE = 99,
  }

  /**
   * Describes the radio access technology.
   */
  export enum RadioTechnology {
    /**
     * Indicates unknown radio access technology (RAT).
     */
    RADIO_TECHNOLOGY_UNKNOWN = 0,

    /**
     * Indicates that RAT is global system for mobile communications (GSM), including GSM, general packet
     * radio system (GPRS), and enhanced data rates for GSM evolution (EDGE).
     */
    RADIO_TECHNOLOGY_GSM = 1,

    /**
     * Indicates that RAT is code division multiple access (CDMA), including Interim Standard 95 (IS95) and
     * Single-Carrier Radio Transmission Technology (1xRTT).
     */
    RADIO_TECHNOLOGY_1XRTT = 2,

    /**
     * Indicates that RAT is wideband code division multiple address (WCDMA).
     */
    RADIO_TECHNOLOGY_WCDMA = 3,

    /**
     * Indicates that RAT is high-speed packet access (HSPA), including HSPA, high-speed downlink packet
     * access (HSDPA), and high-speed uplink packet access (HSUPA).
     */
    RADIO_TECHNOLOGY_HSPA = 4,

    /**
     * Indicates that RAT is evolved high-speed packet access (HSPA+), including HSPA+ and dual-carrier
     * HSPA+ (DC-HSPA+).
     */
    RADIO_TECHNOLOGY_HSPAP = 5,

    /**
     * Indicates that RAT is time division-synchronous code division multiple access (TD-SCDMA).
     */
    RADIO_TECHNOLOGY_TD_SCDMA = 6,

    /**
     * Indicates that RAT is evolution data only (EVDO), including EVDO Rev.0, EVDO Rev.A, and EVDO Rev.B.
     */
    RADIO_TECHNOLOGY_EVDO = 7,

    /**
     * Indicates that RAT is evolved high rate packet data (EHRPD).
     */
    RADIO_TECHNOLOGY_EHRPD = 8,

    /**
     * Indicates that RAT is long term evolution (LTE).
     */
    RADIO_TECHNOLOGY_LTE = 9,

    /**
     * Indicates that RAT is LTE carrier aggregation (LTE-CA).
     */
    RADIO_TECHNOLOGY_LTE_CA = 10,

    /**
     * Indicates that RAT is interworking WLAN (I-WLAN).
     */
    RADIO_TECHNOLOGY_IWLAN = 11,

    /**
     * Indicates that RAT is 5G new radio (NR).
     */
    RADIO_TECHNOLOGY_NR = 12
  }

  export interface SignalInformation {
    /**
     * Obtains the network type corresponding to the signal.
     */
    signalType: NetworkType;

    /**
     * Obtains the signal level of the current network.
     */
    signalLevel: number;
  }

  /**
   * Describes the network type.
   */
  export enum NetworkType {
    /**
     * Indicates unknown network type.
     */
    NETWORK_TYPE_UNKNOWN,

    /**
     * Indicates that the network type is GSM.
     */
    NETWORK_TYPE_GSM,

    /**
     * Indicates that the network type is CDMA.
     */
    NETWORK_TYPE_CDMA,

    /**
     * Indicates that the network type is WCDMA.
     */
    NETWORK_TYPE_WCDMA,

    /**
     * Indicates that the network type is TD-SCDMA.
     */
    NETWORK_TYPE_TDSCDMA,

    /**
     * Indicates that the network type is LTE.
     */
    NETWORK_TYPE_LTE,

    /**
     * Indicates that the network type is 5G NR.
     */
    NETWORK_TYPE_NR
  }

  /**
   * Describes the network registration state.
   */
  export interface NetworkState {
    /**
     * Obtains the operator name in the long alphanumeric format of the registered network.
     *
     * @return Returns the operator name in the long alphanumeric format as a string;
     * returns an empty string if no operator name is obtained.
     */
    longOperatorName: string;

    /**
     * Obtains the operator name in the short alphanumeric format of the registered network.
     *
     * @return Returns the operator name in the short alphanumeric format as a string;
     * returns an empty string if no operator name is obtained.
     */
    shortOperatorName: string;

    /**
     * Obtains the PLMN code of the registered network.
     *
     * @return Returns the PLMN code as a string; returns an empty string if no operator name is obtained.
     */
    plmnNumeric: string;

    /**
     * Checks whether the device is roaming.
     *
     * @return Returns {@code true} if the device is roaming; returns {@code false} otherwise.
     */
    isRoaming: boolean;

    /**
     * Obtains the network registration status of the device.
     *
     * @return Returns the network registration status {@code RegState}.
     */
    regState: RegState;

    /**
     * Obtains the radio Access technology after config conversion.
     *
     * @return Returns the radio Access technology {@code RadioTechnology}.
     * @since 8
     */
    cfgTech: RadioTechnology;

    /**
     * Obtains the NSA network registration status of the device.
     *
     * @return Returns the NSA network registration status {@code NsaState}.
     */
    nsaState: NsaState;

    /**
     * Obtains the status of CA.
     *
     * @return Returns {@code true} if CA is actived; returns {@code false} otherwise.
     */
    isCaActive: boolean;

    /**
     * Checks whether this device is allowed to make emergency calls only.
     *
     * @return Returns {@code true} if this device is allowed to make emergency calls only;
     * returns {@code false} otherwise.
     */
    isEmergency: boolean;
  }

  /**
   * Describes the network registration state.
   */
  export enum RegState {
    /**
     * Indicates a state in which a device cannot use any service.
     */
    REG_STATE_NO_SERVICE = 0,

    /**
     * Indicates a state in which a device can use services properly.
     */
    REG_STATE_IN_SERVICE = 1,

    /**
     * Indicates a state in which a device can use only the emergency call service.
     */
    REG_STATE_EMERGENCY_CALL_ONLY = 2,

    /**
     * Indicates that the cellular radio is powered off.
     */
    REG_STATE_POWER_OFF = 3
  }

  /**
   * Describes the nsa state.
   */
  export enum NsaState {
    /**
     * Indicates that a device is idle under or is connected to an LTE cell that does not support NSA.
     */
    NSA_STATE_NOT_SUPPORT = 1,

    /**
     * Indicates that a device is idle under an LTE cell supporting NSA but not NR coverage detection.
     */
    NSA_STATE_NO_DETECT = 2,

    /**
     * Indicates that a device is connected to an LTE network under an LTE cell
     * that supports NSA and NR coverage detection.
     */
    NSA_STATE_CONNECTED_DETECT = 3,

    /**
     * Indicates that a device is idle under an LTE cell supporting NSA and NR coverage detection.
     */
    NSA_STATE_IDLE_DETECT = 4,

    /**
     * Indicates that a device is connected to an LTE + NR network under an LTE cell that supports NSA.
     */
    NSA_STATE_DUAL_CONNECTED = 5,

    /**
     * Indicates that a device is idle under or is connected to an NG-RAN cell while being attached to 5GC.
     */
    NSA_STATE_SA_ATTACHED = 6
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CellInformation {
    /**
     * Obtains the network type of the serving cell.
     *
     * <p>An application can call this method to determine the network type that the child class uses.
     *
     * @return Returns the the network type of the serving cell.
     */
    networkType: NetworkType;

    /**
     * Obtains the camp-on status of the serving cell.
     *
     * @return Returns {@code true} if the user equipment (UE) is camped on the cell; returns
     * {@code false} otherwise.
     */
    isCamped: boolean;

    /**
     * Obtains the timestamp when the cell information is obtained.
     *
     * @return Returns a timestamp since boot, in nanoseconds.
     */
    timeStamp: number;

    /**
     * An abstract method of the parent class whose implementation depends on the child classes.
     * Returned child class objects vary according to the network type.
     *
     * @return Returns child class objects specific to the network type.
     */
    signalInformation: SignalInformation;

    data: CdmaCellInformation | GsmCellInformation | LteCellInformation | NrCellInformation | TdscdmaCellInformation
      | WcdmaCellInformation;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CdmaCellInformation {
    baseId: number;
    latitude: number;
    longitude: number;
    nid: number;
    sid: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface GsmCellInformation {
    lac: number;
    cellId: number;
    arfcn: number;
    bsic: number;
    mcc: string;
    mnc: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface LteCellInformation {
    cgi: number;
    pci: number;
    tac: number;
    earfcn: number;
    bandwidth: number;
    mcc: string;
    mnc: string;
    isSupportEndc: boolean;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface NrCellInformation {
    nrArfcn: number;
    pci: number;
    tac: number;
    nci: number;
    mcc: string;
    mnc: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface TdscdmaCellInformation {
    lac: number;
    cellId: number;
    cpid: number;
    uarfcn: number;
    mcc: string;
    mnc: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface WcdmaCellInformation {
    lac: number;
    cellId: number;
    psc: number;
    uarfcn: number;
    mcc: string;
    mnc: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum NrOptionMode {
    /** Indicates unknown NR networking mode. */
    NR_OPTION_UNKNOWN,

    /** Indicates that the NR networking mode is NSA only. */
    NR_OPTION_NSA_ONLY,

    /** Indicates that the NR networking mode is SA only. */
    NR_OPTION_SA_ONLY,

    /** Indicates that the NR networking mode is NSA and SA. */
    NR_OPTION_NSA_AND_SA,
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface NetworkSearchResult {
    isNetworkSearchSuccess: boolean;
    networkSearchResult: Array<NetworkInformation>;
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface NetworkInformation {
    operatorName: string;
    operatorNumeric: string;
    state: NetworkInformationState;
    radioTech: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export enum NetworkInformationState {
    /** Indicates that the network state is unknown. */
    NETWORK_UNKNOWN,

    /** Indicates that the network is available for registration. */
    NETWORK_AVAILABLE,

    /** Indicates that you have already registered with the network. */
    NETWORK_CURRENT,

    /** Indicates that the network is unavailable for registration. */
    NETWORK_FORBIDDEN
  }

  /**
   * @systemapi Hide this for inner system use.
   */
  export interface NetworkSelectionModeOptions {
    slotId: number;
    selectMode: NetworkSelectionMode;
    networkInformation: NetworkInformation;
    resumeSelection: boolean;
  }

  export enum NetworkSelectionMode {
    /** Unknown network selection modes. */
    NETWORK_SELECTION_UNKNOWN,

    /** Automatic network selection modes. */
    NETWORK_SELECTION_AUTOMATIC,

    /** Manual network selection modes. */
    NETWORK_SELECTION_MANUAL
  }
}

export default radio;