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
 * Provides the capabilities and methods for obtaining Short Message Service (SMS) management objects.
 *
 * @since 6
 * @syscap SystemCapability.Telephony.SmsMms
 */
declare namespace sms {
  /**
   * Splits a long SMS message into multiple fragments.
   *
   * <p>If the length of an SMS message exceeds the maximum length allowed (140 bytes),
   *     the SMS message is split into multiple segments for processing.
   *
   * @param content Indicates the short message content, which cannot be {@code null}.
   * @param callback Returns a list of split segments, which can be combined into a complete SMS message;
   *     returns an empty string if no permission is granted or the short message content is {@code null}.
   * @permission ohos.permission.SEND_MESSAGES
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function splitMessage(content: string, callback: AsyncCallback<Array<string>>): void;
  function splitMessage(content: string): Promise<Array<string>>;

  /**
   * Creates an SMS message instance based on the protocol data unit (PDU) and the specified SMS protocol.
   *
   * <p>After receiving the original PDU data, the system creates an SMS message instance according to the specified
   * SMS protocol.
   *
   * @param pdu Indicates the original data, which is obtained from the received SMS.
   * @param specification Indicates the SMS protocol type. The value {@code 3gpp} indicates GSM/UMTS/LTE SMS,
   *     and the value {@code 3gpp2} indicates CDMA/LTE SMS.
   * @param callback Returns an SMS message instance; returns {@code null} if {@code pdu} is empty or
   *     {@code specification} is not supported.
   */
  function createMessage(pdu: Array<number>, specification: string, callback: AsyncCallback<ShortMessage>): void;
  function createMessage(pdu: Array<number>, specification: string): Promise<ShortMessage>;

  /**
   * Sends a text or data SMS message.
   *
   * <p>This method checks whether the length of an SMS message exceeds the maximum length. If the
   * maximum length is exceeded, the SMS message is split into multiple parts and sent separately.
   *
   * @param options Indicates the parameters and callback for sending the SMS message.
   * @permission ohos.permission.SEND_MESSAGES
   */
  function sendMessage(options: SendMessageOptions): void;

  /**
   * Sets the default SIM card for sending SMS messages. You can obtain the default SIM card by
   * using {@code getDefaultSmsSlotId}.
   *
   * @param slotId Indicates the default SIM card for sending SMS messages. The value {@code 0} indicates card slot 1,
   *     and the value {@code 1} indicates card slot 2.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setDefaultSmsSlotId(slotId: number, callback: AsyncCallback<void>): void;
  function setDefaultSmsSlotId(slotId: number): Promise<void>;

  /**
   * Obtains the default SIM card for sending SMS messages.
   *
   * @param callback Returns {@code 0} if the default SIM card for sending SMS messages is in card slot 1;
   *     returns {@code 1} if the default SIM card for sending SMS messages is in card slot 2.
   * @since 7
   */
  function getDefaultSmsSlotId(callback: AsyncCallback<number>): void;
  function getDefaultSmsSlotId(): Promise<number>;

  /**
   * Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
   *
   * @param slotId Indicates the ID of the slot holding the SIM card for sending SMS messages.
   * @param smscAddr Indicates the SMSC address.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setSmscAddr(slotId: number, smscAddr: string, callback: AsyncCallback<void>): void;
  function setSmscAddr(slotId: number, smscAddr: string): Promise<void>;

  /**
   * Obtains the SMSC address based on a specified slot ID.
   *
   * @param slotId Indicates the ID of the slot holding the SIM card for sending SMS messages.
   * @param callback Returns the SMSC address.
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getSmscAddr(slotId: number, callback: AsyncCallback<string>): void;
  function getSmscAddr(slotId: number): Promise<string>;

  /**
   * Returns whether a device is capable of sending and receiving SMS messages.
   *
   * @return Returns {@code true} if the device is capable of sending and receiving SMS messages;
   *     returns {@code false} otherwise.
   * @since 7
   */
  function hasSmsCapability(): boolean;

  /**
   * @permission ohos.permission.RECEIVE_SMS and ohos.permission.SEND_MESSAGES
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function addSimMessage(options: SimMessageOptions, callback: AsyncCallback<void>): void;
  function addSimMessage(options: SimMessageOptions): Promise<void>;

  /**
   * @permission ohos.permission.RECEIVE_SMS and ohos.permission.SEND_MESSAGES
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function delSimMessage(slotId: number, msgIndex: number, callback: AsyncCallback<void>): void;
  function delSimMessage(slotId: number, msgIndex: number): Promise<void>;

  /**
   * @permission ohos.permission.RECEIVE_SMS and ohos.permission.SEND_MESSAGES
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function updateSimMessage(options: UpdateSimMessageOptions, callback: AsyncCallback<void>): void;
  function updateSimMessage(options: UpdateSimMessageOptions): Promise<void>;

  /**
   * @permission ohos.permission.RECEIVE_SMS
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getAllSimMessages(slotId: number, callback: AsyncCallback<Array<SimShortMessage>>): void;
  function getAllSimMessages(slotId: number): Promise<Array<SimShortMessage>>;

  /**
   * @permission ohos.permission.RECEIVE_SMS
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setCBConfig(options: CBConfigOptions, callback: AsyncCallback<void>): void;
  function setCBConfig(options: CBConfigOptions): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getSmsSegmentsInfo(slotId: number, message: string, force7bit: boolean, callback: AsyncCallback<SmsSegmentsInfo>): void;
  function getSmsSegmentsInfo(slotId: number, message: string, force7bit: boolean): Promise<SmsSegmentsInfo>;

  /**
   * SMS over IMS is supported if IMS is registered and SMS is supported on IMS.
   *
   * @param callback Returns true if SMS over IMS is supported, false otherwise.
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function isImsSmsSupported(callback: AsyncCallback<boolean>): void;
  function isImsSmsSupported(): Promise<boolean>;

  /**
   * Gets SMS format supported on IMS. SMS over IMS format is either 3GPP or 3GPP2.
   *
   * @param callback Returns format, 3gpp, 3gpp2 or unknown.
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getImsShortMessageFormat(callback: AsyncCallback<string>): void;
  function getImsShortMessageFormat(): Promise<string>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function decodeMms(mmsFilePathName: string | Array<number>, callback: AsyncCallback<MmsInformation>): void;
  function decodeMms(mmsFilePathName: string | Array<number>): Promise<MmsInformation>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function encodeMms(mms: MmsInformation, callback: AsyncCallback<Array<number>>): void;
  function encodeMms(mms: MmsInformation): Promise<Array<number>>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsInformation {
    messageType: MessageType;
    mmsType: MmsSendReq | MmsSendConf | MmsNotificationInd | MmsRespInd | MmsRetrieveConf | MmsAcknowledgeInd | MmsDeliveryInd | MmsReadOrigInd | MmsReadRecInd;
    attachment?: Array<MmsAttachment>;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsSendReq {
    from: MmsAddress;
    transactionId: string;
    contentType: string;
    version: MmsVersionType;
    to?: Array<MmsAddress>;
    date?: number;
    cc?: Array<MmsAddress>;
    bcc?: Array<MmsAddress>;
    subject?: string;
    messageClass?: number;
    expiry?: number;
    priority?: MmsPriorityType;
    senderVisibility?: number;
    deliveryReport?: number;
    readReport?: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsSendConf {
    responseState: number;
    transactionId: string;
    version: MmsVersionType;
    messageId?: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsNotificationInd {
    transactionId: string;
    messageClass: number;
    messageSize: number;
    expiry: number;
    contentLocation: string;
    version: MmsVersionType;
    from?: MmsAddress;
    subject?: string;
    deliveryReport?: number;
    contentClass?: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsRespInd {
    transactionId: string;
    status: number;
    version: MmsVersionType;
    reportAllowed?: ReportType;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsRetrieveConf {
    transactionId: string;
    messageId: string;
    date: number;
    contentType: string;
    to: Array<MmsAddress>;
    version: MmsVersionType;
    from?: MmsAddress;
    cc?: Array<MmsAddress>;
    subject?: string;
    priority?: MmsPriorityType;
    deliveryReport?: number;
    readReport?: number;
    retrieveStatus?: number;
    retrieveText?: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsAcknowledgeInd {
    transactionId: string;
    version: MmsVersionType;
    reportAllowed?: ReportType;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsDeliveryInd {
    messageId: string;
    date: number;
    to: Array<MmsAddress>;
    status: number;
    version: MmsVersionType;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsReadOrigInd {
    version: MmsVersionType;
    messageId: string;
    to: Array<MmsAddress>;
    from: MmsAddress;
    date: number;
    readStatus: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsReadRecInd {
    version: MmsVersionType;
    messageId: string;
    to: Array<MmsAddress>;
    from: MmsAddress;
    readStatus: number;
    date?: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsAttachment {
    contentId: string;
    contentLocation: string;
    contentDisposition: DispositionType;
    contentTransferEncoding: string;
    contentType: string;
    isSmil: boolean;
    path?: string;
    inBuff?: Array<number>;
    fileName?: string;
    charset?: MmsCharSets;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface MmsAddress {
    address: string;
    charset: MmsCharSets;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum MessageType {
    TYPE_MMS_SEND_REQ = 128,
    TYPE_MMS_SEND_CONF,
    TYPE_MMS_NOTIFICATION_IND,
    TYPE_MMS_RESP_IND,
    TYPE_MMS_RETRIEVE_CONF,
    TYPE_MMS_ACKNOWLEDGE_IND,
    TYPE_MMS_DELIVERY_IND,
    TYPE_MMS_READ_REC_IND,
    TYPE_MMS_READ_ORIG_IND,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum MmsPriorityType {
    MMS_LOW = 128,
    MMS_NORMAL,
    MMS_HIGH,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum MmsVersionType {
    MMS_VERSION_1_0 = 0x10,
    MMS_VERSION_1_1,
    MMS_VERSION_1_2,
    MMS_VERSION_1_3,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum MmsCharSets {
    BIG5 = 0X07EA,
    ISO_10646_UCS_2 = 0X03E8,
    ISO_8859_1 = 0X04,
    ISO_8859_2,
    ISO_8859_3,
    ISO_8859_4,
    ISO_8859_5,
    ISO_8859_6,
    ISO_8859_7,
    ISO_8859_8,
    ISO_8859_9,
    SHIFT_JIS = 0X11,
    US_ASCII = 0X03,
    UTF_8 = 0X6A,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum DispositionType {
    FROM_DATA = 0,
    ATTACHMENT,
    INLINE,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum ReportType {
    MMS_YES = 128,
    MMS_NO,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface CBConfigOptions {
    slotId: number;
    enable: boolean;
    startMessageId: number;
    endMessageId: number;
    ranType: RanType;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface SimMessageOptions {
    slotId: number;
    smsc: string;
    pdu: string;
    status: SimMessageStatus;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface UpdateSimMessageOptions {
    slotId: number;
    msgIndex: number;
    newStatus: SimMessageStatus;
    pdu: string;
    smsc: string;
  }

  export interface ShortMessage {
    /** Indicates the SMS message body. */
    visibleMessageBody: string;
    /** Indicates the address of the sender, which is to be displayed on the UI. */
    visibleRawAddress: string;
    /** Indicates the SMS type. */
    messageClass: ShortMessageClass;
    /** Indicates the protocol identifier. */
    protocolId: number;
    /** Indicates the short message service center (SMSC) address. */
    scAddress: string;
    /** Indicates the SMSC timestamp. */
    scTimestamp: number;
    /** Indicates whether the received SMS is a "replace short message". */
    isReplaceMessage: boolean;
    /** Indicates whether the received SMS contains "TP-Reply-Path". */
    hasReplyPath: boolean;
    /** Indicates Protocol Data Units (PDUs) from an SMS message. */
    pdu: Array<number>;
    /**
     * Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     */
    status: number;
    /** Indicates whether the current message is SMS-STATUS-REPORT. */
    isSmsStatusReportMessage: boolean;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface SimShortMessage {
    shortMessage: ShortMessage;

    /** Indicates the storage status of SMS messages in the SIM */
    simMessageStatus: SimMessageStatus;
    /** Indicates the index of SMS messages in the SIM */
    indexOnSim: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum SimMessageStatus {
    /** status free space ON SIM */
    SIM_MESSAGE_STATUS_FREE = 0,
    /** REC READ received read message */
    SIM_MESSAGE_STATUS_READ = 1,
    /** REC UNREAD received unread message */
    SIM_MESSAGE_STATUS_UNREAD = 3,
    /** STO SENT stored sent message (only applicable to SMs) */
    SIM_MESSAGE_STATUS_SENT = 5,
    /** STO UNSENT stored unsent message (only applicable to SMs) */
    SIM_MESSAGE_STATUS_UNSENT = 7,
  }

  export enum ShortMessageClass {
    /** Indicates an unknown type. */
    UNKNOWN,
    /** Indicates an instant message, which is displayed immediately after being received. */
    INSTANT_MESSAGE,
    /** Indicates an SMS message that can be stored on the device or SIM card based on the storage status. */
    OPTIONAL_MESSAGE,
    /** Indicates an SMS message containing SIM card information, which is to be stored in a SIM card. */
    SIM_MESSAGE,
    /** Indicates an SMS message to be forwarded to another device. */
    FORWARD_MESSAGE
  }

  export interface SendMessageOptions {
    /** Indicates the ID of the SIM card slot used for sending the SMS message. */
    slotId: number;
    /** Indicates the address to which the SMS message is sent. */
    destinationHost: string;
    /** Indicates the SMSC address. If the value is {@code null}, the default SMSC address of the SIM card*/
    serviceCenter?: string;
    /** If the content is a string, this is a short message. If the content is a byte array, this is a data message. */
    content: string | Array<number>;
    /** If send data message, destinationPort is mandatory. Otherwise is optional. */
    destinationPort?: number;
    /** Indicates the callback invoked after the SMS message is sent. */
    sendCallback?: AsyncCallback<ISendShortMessageCallback>;
    /** Indicates the callback invoked after the SMS message is delivered. */
    deliveryCallback?: AsyncCallback<IDeliveryShortMessageCallback>;
  }

  export interface ISendShortMessageCallback {
    /** Indicates the SMS message sending result. */
    result: SendSmsResult;
    /** Indicates the URI to store the sent SMS message. */
    url: string;
    /** Specifies whether this is the last part of a multi-part SMS message. */
    isLastPart: boolean;
  }

  export interface IDeliveryShortMessageCallback {
    /** Indicates the SMS delivery report. */
    pdu: Array<number>;
  }

  export enum SendSmsResult {
    /**
     * Indicates that the SMS message is successfully sent.
     */
    SEND_SMS_SUCCESS = 0,

    /**
     * Indicates that sending the SMS message fails due to an unknown reason.
     */
    SEND_SMS_FAILURE_UNKNOWN = 1,

    /**
     * Indicates that sending the SMS fails because the modem is powered off.
     */
    SEND_SMS_FAILURE_RADIO_OFF = 2,

    /**
     * Indicates that sending the SMS message fails because the network is unavailable
     * or does not support sending or reception of SMS messages.
     */
    SEND_SMS_FAILURE_SERVICE_UNAVAILABLE = 3
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum RanType {
    TYPE_GSM = 1, // GSM
    TYPE_CDMA = 2, // CDMA
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface SmsSegmentsInfo {
    splitCount: number;
    encodeCount: number;
    encodeCountRemaining: number;
    scheme: SmsEncodingScheme;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum SmsEncodingScheme {
    SMS_ENCODING_UNKNOWN = 0,
    SMS_ENCODING_7BIT,
    SMS_ENCODING_8BIT,
    SMS_ENCODING_16BIT,
  }
}

export default sms;