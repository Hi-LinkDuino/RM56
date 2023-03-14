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
 * Provides applications with APIs for obtaining SIM card status, card file information, and card specifications.
 * SIM cards include SIM, USIM, and CSIM cards.
 *
 * @since 6
 * @syscap SystemCapability.Telephony.CoreService
 */
declare namespace sim {
  /**
   * Checks whether the SIM card in a specified slot is activated.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from {@code 0} to the maximum card slot index number supported by the device.
   * @param callback Returns {@code true} if the SIM card is activated; returns {@code false} otherwise.
   * @since 7
   */
  function isSimActive(slotId: number, callback: AsyncCallback<boolean>): void;
  function isSimActive(slotId: number): Promise<boolean>;

  /**
   * Obtains the default card slot for the voice service.
   *
   * @param callback Returns {@code 0} if card 1 is used as the default card slot for the voice service;
   * returns {@code 1} if card 2 is used as the default card slot for the voice service;
   * returns {@code -1} if no card is available for the voice service.
   * @since 7
   */
  function getDefaultVoiceSlotId(callback: AsyncCallback<number>): void;
  function getDefaultVoiceSlotId(): Promise<number>;

  /**
   * Checks whether your application (the caller) has been granted the operator permissions.
   *
   * @param slotId Indicates the ID of the SIM card slot.
   * @param callback Returns {@code true} if your application has been granted the operator permissions;
   * returns {@code false} otherwise.
   * @since 7
   */
  function hasOperatorPrivileges(slotId: number, callback: AsyncCallback<boolean>): void;
  function hasOperatorPrivileges(slotId: number): Promise<boolean>;

  /**
   * Obtains the ISO country code of the SIM card in a specified slot.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the country code defined in ISO 3166-2; returns an empty string if no SIM card is inserted.
   */
  function getISOCountryCodeForSim(slotId: number, callback: AsyncCallback<string>): void;
  function getISOCountryCodeForSim(slotId: number): Promise<string>;

  /**
   * Obtains the home PLMN number of the SIM card in a specified slot.
   *
   * <p>The value is recorded in the SIM card and is irrelevant to the network
   * with which the SIM card is currently registered.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the PLMN number; returns an empty string if no SIM card is inserted.
   */
  function getSimOperatorNumeric(slotId: number, callback: AsyncCallback<string>): void;
  function getSimOperatorNumeric(slotId: number): Promise<string>;

  /**
   * Obtains the service provider name (SPN) of the SIM card in a specified slot.
   *
   * <p>The value is recorded in the EFSPN file of the SIM card and is irrelevant to the network
   * with which the SIM card is currently registered.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the SPN; returns an empty string if no SIM card is inserted or
   * no EFSPN file in the SIM card.
   */
  function getSimSpn(slotId: number, callback: AsyncCallback<string>): void;
  function getSimSpn(slotId: number): Promise<string>;

  /**
   * Obtains the state of the SIM card in a specified slot.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from {@code 0} to the maximum card slot index number supported by the device.
   * @param callback Returns one of the following SIM card states:
   * <ul>
   * <li>{@code SimState#SIM_STATE_UNKNOWN}
   * <li>{@code SimState#SIM_STATE_NOT_PRESENT}
   * <li>{@code SimState#SIM_STATE_LOCKED}
   * <li>{@code SimState#SIM_STATE_NOT_READY}
   * <li>{@code SimState#SIM_STATE_READY}
   * <li>{@code SimState#SIM_STATE_LOADED}
   * </ul>
   */
  function getSimState(slotId: number, callback: AsyncCallback<SimState>): void;
  function getSimState(slotId: number): Promise<SimState>;

  /**
   * Obtains the type of the SIM card installed in a specified slot.
   *
   * @param slotId Indicates the ID of the specified slot.
   * @param callback Returns the SIM card type.
   * @since 7
   */
  function getCardType(slotId: number, callback: AsyncCallback<CardType>): void;
  function getCardType(slotId: number): Promise<CardType>;

  /**
   * Obtains the ICCID of the SIM card in a specified slot.
   *
   * <p>The ICCID is a unique identifier of a SIM card. It consists of 20 digits
   * and is recorded in the EFICCID file of the SIM card.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the ICCID; returns an empty string if no SIM card is inserted.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getSimIccId(slotId: number, callback: AsyncCallback<string>): void;
  function getSimIccId(slotId: number): Promise<string>;

  /**
   * Obtains the alpha identifier of the voice mailbox of the SIM card in a specified slot.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from {@code 0} to the maximum card slot index number supported by the device.
   * @param callback Returns the voice mailbox alpha identifier;
   * returns an empty string if no voice mailbox alpha identifier is written into the SIM card.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getVoiceMailIdentifier(slotId: number, callback: AsyncCallback<string>): void;
  function getVoiceMailIdentifier(slotId: number): Promise<string>;

  /**
   * Obtains the voice mailbox number of the SIM card in a specified slot.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from {@code 0} to the maximum card slot index number supported by the device.
   * @param callback Returns the voice mailbox number;
   * returns an empty string if no voice mailbox number is written into the SIM card.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getVoiceMailNumber(slotId: number, callback: AsyncCallback<string>): void;
  function getVoiceMailNumber(slotId: number): Promise<string>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setVoiceMailInfo(slotId: number, mailName: string, mailNumber: string, callback: AsyncCallback<void>): void;
  function setVoiceMailInfo(slotId: number, mailName: string, mailNumber: string): Promise<void>;

  /**
   * Obtains the MSISDN of the SIM card in a specified slot.
   * The MSISDN is recorded in the EFMSISDN file of the SIM card.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the MSISDN; returns an empty string if no SIM card is inserted or
   * no MSISDN is recorded in the EFMSISDN file.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getSimTelephoneNumber(slotId: number, callback: AsyncCallback<string>): void;
  function getSimTelephoneNumber(slotId: number): Promise<string>;

  /**
   * Obtains the Group Identifier Level 1 (GID1) of the SIM card in a specified slot.
   * The GID1 is recorded in the EFGID1 file of the SIM card.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns the GID1; returns an empty string if no SIM card is inserted or
   * no GID1 in the SIM card.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getSimGid1(slotId: number, callback: AsyncCallback<string>): void;
  function getSimGid1(slotId: number): Promise<string>;

  /**
   * Obtains the maximum number of SIM cards that can be used simultaneously on the device,
   * that is, the maximum number of SIM card slots.
   *
   * @return Returns the maximum number of SIM card slots.
   * @since 7
   */
  function getMaxSimCount(): number;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   */
  function getIMSI(slotId: number, callback: AsyncCallback<string>): void;
  function getIMSI(slotId: number): Promise<string>;

  /**
   * Checks whether a SIM card is inserted in a specified slot.
   *
   * @param slotId Indicates the card slot index number,
   * ranging from 0 to the maximum card slot index number supported by the device.
   * @param callback Returns true if a SIM card is inserted; return false otherwise.
   * @since 7
   */
  function hasSimCard(slotId: number, callback: AsyncCallback<boolean>): void;
  function hasSimCard(slotId: number): Promise<boolean>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getSimAccountInfo(slotId: number, callback: AsyncCallback<IccAccountInfo>): void;
  function getSimAccountInfo(slotId: number): Promise<IccAccountInfo>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getActiveSimAccountInfoList(callback: AsyncCallback<Array<IccAccountInfo>>): void;
  function getActiveSimAccountInfoList(): Promise<Array<IccAccountInfo>>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setDefaultVoiceSlotId(slotId: number, callback: AsyncCallback<void>): void;
  function setDefaultVoiceSlotId(slotId: number): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function activateSim(slotId: number, callback: AsyncCallback<void>): void;
  function activateSim(slotId: number): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function deactivateSim(slotId: number, callback: AsyncCallback<void>): void;
  function deactivateSim(slotId: number): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setShowName(slotId: number, name: string, callback: AsyncCallback<void>): void;
  function setShowName(slotId: number, name: string): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getShowName(slotId: number, callback: AsyncCallback<string>): void;
  function getShowName(slotId: number): Promise<string>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setShowNumber(slotId: number, number: string, callback: AsyncCallback<void>): void;
  function setShowNumber(slotId: number, number: string): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getShowNumber(slotId: number, callback: AsyncCallback<string>): void;
  function getShowNumber(slotId: number): Promise<string>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getOperatorConfigs(slotId: number, callback: AsyncCallback<Array<OperatorConfig>>): void;
  function getOperatorConfigs(slotId: number): Promise<Array<OperatorConfig>>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function unlockPin(slotId: number, pin: string, callback: AsyncCallback<LockStatusResponse>): void;
  function unlockPin(slotId: number, pin: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function unlockPuk(slotId: number, newPin: string, puk: string, callback: AsyncCallback<LockStatusResponse>): void;
  function unlockPuk(slotId: number, newPin: string, puk: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function alterPin(slotId: number, newPin: string, oldPin: string, callback: AsyncCallback<LockStatusResponse>): void;
  function alterPin(slotId: number, newPin: string, oldPin: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setLockState(slotId: number, options: LockInfo, callback: AsyncCallback<LockStatusResponse>): void;
  function setLockState(slotId: number, options: LockInfo): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function unlockPin2(slotId: number, pin2: string, callback: AsyncCallback<LockStatusResponse>): void;
  function unlockPin2(slotId: number, pin2: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function unlockPuk2(slotId: number, newPin2: string, puk2: string, callback: AsyncCallback<LockStatusResponse>): void;
  function unlockPuk2(slotId: number, newPin2: string, puk2: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function alterPin2(slotId: number, newPin2: string, oldPin2: string, callback: AsyncCallback<LockStatusResponse>): void;
  function alterPin2(slotId: number, newPin2: string, oldPin2: string): Promise<LockStatusResponse>;

  /**
   * @permission ohos.permission.READ_CONTACTS
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function queryIccDiallingNumbers(slotId: number, type: ContactType, callback: AsyncCallback<Array<DiallingNumbersInfo>>): void
  function queryIccDiallingNumbers(slotId: number, type: ContactType): Promise<Array<DiallingNumbersInfo>>;

  /**
   * @permission ohos.permission.WRITE_CONTACTS
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function addIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo, callback: AsyncCallback<void>): void;
  function addIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo): Promise<void>;

  /**
   * @permission ohos.permission.WRITE_CONTACTS
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function delIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo, callback: AsyncCallback<void>): void;
  function delIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo): Promise<void>;

  /**
   * @permission ohos.permission.WRITE_CONTACTS
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function updateIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo, callback: AsyncCallback<void>): void;
  function updateIccDiallingNumbers(slotId: number, type: ContactType, diallingNumbers: DiallingNumbersInfo): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getLockState(slotId: number, lockType: LockType, callback: AsyncCallback<LockState>): void;
  function getLockState(slotId: number, lockType: LockType): Promise<LockState>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function sendEnvelopeCmd(slotId: number, cmd: string, callback: AsyncCallback<void>): void;
  function sendEnvelopeCmd(slotId: number, cmd: string): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function sendTerminalResponseCmd(slotId: number, cmd: string, callback: AsyncCallback<void>): void;
  function sendTerminalResponseCmd(slotId: number, cmd: string): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function unlockSimLock(slotId: number, lockInfo: PersoLockInfo, callback: AsyncCallback<LockStatusResponse>): void;
  function unlockSimLock(slotId: number, lockInfo: PersoLockInfo): Promise<LockStatusResponse>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface OperatorConfig {
    field: string;
    value: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface IccAccountInfo {
    /**
     * sim Id for card.
     */
    simId: number;
    /**
     * slot id.
     */
    slotIndex: number;
    /**
     * mark card is eSim or not.
     */
    isEsim: boolean;
    /**
     * active status for card.
     */
    isActive: boolean;
    /**
     * iccId for card.
     */
    iccId: string;
    /**
     * display name for card.
     */
    showName: string;
    /**
     * display number for card.
     */
    showNumber: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface LockStatusResponse {
    /**
     * Current operation result
     */
    result: number;
    /**
     * Operations remaining
     */
    remain?: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface DiallingNumbersInfo {
    alphaTag: string;
    number: string;
    recordNumber?: number;
    pin2?: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface LockInfo {
    lockType: LockType;
    password: string;
    state: LockState;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface PersoLockInfo {
    lockType: PersoLockType;
    password: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum LockType {
    PIN_LOCK = 1,
    FDN_LOCK = 2,
  }

  /**
   * @since 7
   */
  export enum CardType {
    /** Icc card type: Unknow type Card. */
    UNKNOWN_CARD = -1,

    /** Icc card type: Single sim card type. */
    SINGLE_MODE_SIM_CARD = 10,

    /** Icc card type: Single usim card type. */
    SINGLE_MODE_USIM_CARD = 20,

    /** Icc card type: Single ruim card type. */
    SINGLE_MODE_RUIM_CARD = 30,

    /** Icc card type: Double card C+G. */
    DUAL_MODE_CG_CARD = 40,

    /** Icc card type: China Telecom Internal Roaming Card (Dual Mode). */
    CT_NATIONAL_ROAMING_CARD = 41,

    /** Icc card type: China Unicom Dual Mode Card. */
    CU_DUAL_MODE_CARD = 42,

    /** Icc card type: China Telecom LTE Card (Dual Mode). */
    DUAL_MODE_TELECOM_LTE_CARD = 43,

    /** Icc card type: Double card U+G. */
    DUAL_MODE_UG_CARD = 50,

    /**
     * Icc card type: Single isim card type.
     * @since 8
     */
    SINGLE_MODE_ISIM_CARD = 60
  }

  export enum SimState {
    /**
     * Indicates unknown SIM card state, that is, the accurate status cannot be obtained.
     */
    SIM_STATE_UNKNOWN,

    /**
     * Indicates that the SIM card is in the <b>not present</b> state, that is, no SIM card is inserted
     * into the card slot.
     */
    SIM_STATE_NOT_PRESENT,

    /**
     * Indicates that the SIM card is in the <b>locked</b> state, that is, the SIM card is locked by the
     * personal identification number (PIN)/PIN unblocking key (PUK) or network.
     */
    SIM_STATE_LOCKED,

    /**
     * Indicates that the SIM card is in the <b>not ready</b> state, that is, the SIM card is in position
     * but cannot work properly.
     */
    SIM_STATE_NOT_READY,

    /**
     * Indicates that the SIM card is in the <b>ready</b> state, that is, the SIM card is in position and
     * is working properly.
     */
    SIM_STATE_READY,

    /**
     * Indicates that the SIM card is in the <b>loaded</b> state, that is, the SIM card is in position and
     * is working properly.
     */
    SIM_STATE_LOADED
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum LockState {
    /**Indicates that the lock state card is in the <b>off</b> state. */
    LOCK_OFF = 0,

    /**Indicates that the lock state card is in the <b>on</b> state. */
    LOCK_ON = 1,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum ContactType {
    GENERAL_CONTACT = 1,
    FIXED_DIALING = 2,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum PersoLockType {
    PN_PIN_LOCK, //Network Personalization (refer 3GPP TS 22.022 [33])
    PN_PUK_LOCK,
    PU_PIN_LOCK, //network sUbset Personalization (refer 3GPP TS 22.022 [33])
    PU_PUK_LOCK,
    PP_PIN_LOCK, //service Provider Personalization (refer 3GPP TS 22.022 [33])
    PP_PUK_LOCK,
    PC_PIN_LOCK, //Corporate Personalization (refer 3GPP TS 22.022 [33])
    PC_PUK_LOCK,
    SIM_PIN_LOCK, //SIM/USIM personalisation (refer 3GPP TS 22.022 [33])
    SIM_PUK_LOCK,
  }
}

export default sim;
