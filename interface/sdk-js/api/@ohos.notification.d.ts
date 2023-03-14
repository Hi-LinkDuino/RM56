/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { AsyncCallback } from './basic';
import { NotificationSlot } from './notification/notificationSlot';
import { NotificationSubscriber } from './notification/notificationSubscriber';
import { NotificationSubscribeInfo } from './notification/notificationSubscribeInfo';
import { NotificationRequest } from './notification/notificationRequest';

/**
 * Manages notifications.
 *
 * <p>Generally, only system applications have permissions on notification subscription and unsubscription.
 * You can specify the content of a notification to be published and the content is carried by
 * {@link NotificationRequest}. A notification ID is unique in an application and must be specified
 * when using {@link NotificationRequest} to carry the notification content. If a notification
 * with this ID has been published and you need to use this ID to publish another notification,
 * the original notification will be updated. In addition, the notification ID can be used to cancel
 * a notification by calling the {@link #cancel(int)} method.
 *
 * @name notification
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @import import notification from '@ohos.notification';
 * @permission N/A
 */
declare namespace notification {
  /**
   * Publishes a notification.
   *
   * <p>If a notification with the same ID has been published by the current application and has not been deleted,
   * this method will update the notification.
   *
   * @param request notification request
   * @param callback callback function
   */
  function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
  function publish(request: NotificationRequest): Promise<void>;

  /**
   * Publishes a notification to the specified user.
   *
   * @since 8
   * @param Publishes a notification.
   * @param userId of subscriber receiving the notification
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   *
   */
   function publish(request: NotificationRequest, userId: number, callback: AsyncCallback<void>): void;
   function publish(request: NotificationRequest, userId: number): Promise<void>;

  /**
   * Cancels a notification with the specified ID.
   *
   * @param id of the notification to cancel, which must be unique in the application.
   * @param callback callback function
   */
  function cancel(id: number, callback: AsyncCallback<void>): void;

  /**
   * Cancels a notification with the specified label and ID.
   *
   * @param id ID of the notification to cancel, which must be unique in the application.
   * @param label Label of the notification to cancel.
   * @param callback callback function
   */
  function cancel(id: number, label: string, callback: AsyncCallback<void>): void;
  function cancel(id: number, label?: string): Promise<void>;

  /**
   * Cancels all notifications of the current application.
   */
  function cancelAll(callback: AsyncCallback<void>): void;
  function cancelAll(): Promise<void>;

  /**
   * Creates a notification slot.
   *
   * @param slot Indicates the notification slot to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   * @param callback callback function
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function addSlot(slot: NotificationSlot, callback: AsyncCallback<void>): void;

  /**
   * Creates a notification slot.
   *
   * @param slot Indicates the notification slot to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function addSlot(slot: NotificationSlot): Promise<void>;

  /**
   * Adds a slot type.
   *
   * @param type Slot type to add.
   * @param callback callback function
   */
  function addSlot(type: SlotType, callback: AsyncCallback<void>): void;
  function addSlot(type: SlotType): Promise<void>;

  /**
   * Creates a notification slot.
   *
   * @param slots Indicates the notification slots to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   * @param callback callback function
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function addSlots(slots: Array<NotificationSlot>, callback: AsyncCallback<void>): void;

  /**
   * Creates a notification slot.
   *
   * @param slots Indicates the notification slots to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function addSlots(slots: Array<NotificationSlot>): Promise<void>;

  /**
   * Obtains a notification slot of the specified slot type.
   *
   * @param slotType Type of the notification slot to obtain.
   * @param callback callback function
   * @return Returns the created {@link NotificationSlot}.
   */
  function getSlot(slotType: SlotType, callback: AsyncCallback<NotificationSlot>): void;
  function getSlot(slotType: SlotType): Promise<NotificationSlot>;

  /**
   * Obtains all NotificationSlot objects created by the current application.
   *
   * @return Returns all notification slots of this application.
   */
  function getSlots(callback: AsyncCallback<Array<NotificationSlot>>): void;
  function getSlots(): Promise<Array<NotificationSlot>>;

  /**
   * Removes a NotificationSlot of the specified SlotType created by the current application.
   *
   * @param slotType Type of the NotificationSlot to remove.
   * @param callback callback function
   */
  function removeSlot(slotType: SlotType, callback: AsyncCallback<void>): void;
  function removeSlot(slotType: SlotType): Promise<void>;

  /**
   * Removes all NotificationSlot objects created by the current application.
   */
  function removeAllSlots(callback: AsyncCallback<void>): void;
  function removeAllSlots(): Promise<void>;

  /**
   * Describes NotificationSlot types.
   */
  export enum SlotType {
    /**
    * NotificationSlot of an unknown type.
    */
    UNKNOWN_TYPE = 0,

    /**
    * NotificationSlot for social communication.
    */
    SOCIAL_COMMUNICATION = 1,

    /**
    * NotificationSlot for service information.
    */
    SERVICE_INFORMATION = 2,

    /**
    * NotificationSlot for service information.
    */
    CONTENT_INFORMATION = 3,

    /**
    * NotificationSlot for other purposes.
    */
    OTHER_TYPES = 0xFFFF,
  }

  /**
   * Describes notification content types.
   *
   * @name ContentType
   * @since 7
   * @syscap SystemCapability.Notification.Notification
   * @permission N/A
   */
  export enum ContentType {
    /**
    * Normal text notification.
    */
    NOTIFICATION_CONTENT_BASIC_TEXT,

    /**
    * Long text notification.
    */
    NOTIFICATION_CONTENT_LONG_TEXT,

    /**
    * Picture-attached notification.
    */
    NOTIFICATION_CONTENT_PICTURE,

    /**
    * Conversation notification.
    */
    NOTIFICATION_CONTENT_CONVERSATION,

    /**
    * Multi-line text notification.
    */
    NOTIFICATION_CONTENT_MULTILINE,
  }

  /**
   * Indicates the level of the slot
   */
  export enum SlotLevel {
    /**
    * Indicates that the notification function is disabled.
    */
    LEVEL_NONE = 0,

    /**
    * Indicates that the notification function is enabled but notification
    * icons are not displayed in the status bar, with no banner or prompt tone.
    */
    LEVEL_MIN = 1,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with no banner or prompt tone.
    */
    LEVEL_LOW = 2,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with no banner but with a prompt tone.
    */
    LEVEL_DEFAULT = 3,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with a banner and a prompt tone.
    */
    LEVEL_HIGH = 4,
  }

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, callback: AsyncCallback<void>): void;

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, info: NotificationSubscribeInfo, callback: AsyncCallback<void>): void;

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, info?: NotificationSubscribeInfo): Promise<void>;

  /**
   * unsubscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function unsubscribe(subscriber: NotificationSubscriber, callback: AsyncCallback<void>): void;

  /**
   * unsubscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function unsubscribe(subscriber: NotificationSubscriber): Promise<void>;

  /**
   * enableNotification
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableNotification(bundle: BundleOption, enable: boolean, callback: AsyncCallback<void>): void;

  /**
   * enableNotification
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableNotification(bundle: BundleOption, enable: boolean): Promise<void>;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isNotificationEnabled(bundle: BundleOption, callback: AsyncCallback<boolean>): void;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isNotificationEnabled(bundle: BundleOption): Promise<boolean>;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isNotificationEnabled(callback: AsyncCallback<boolean>): void;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isNotificationEnabled(): Promise<boolean>;

  /**
   * Checks whether this application has permission to publish notifications under the user.
   *
   * since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isNotificationEnabled(userId: number, callback: AsyncCallback<boolean>): void;
  function isNotificationEnabled(userId: number): Promise<boolean>;

  /**
   * displayBadge
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function displayBadge(bundle: BundleOption, enable: boolean, callback: AsyncCallback<void>): void;

  /**
   * displayBadge
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function displayBadge(bundle: BundleOption, enable: boolean): Promise<void>;

  /**
   * isBadgeDisplayed
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isBadgeDisplayed(bundle: BundleOption, callback: AsyncCallback<boolean>): void;

  /**
   * isBadgeDisplayed
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isBadgeDisplayed(bundle: BundleOption): Promise<boolean>;

  /**
   * setSlotByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function setSlotByBundle(bundle: BundleOption, slot: NotificationSlot, callback: AsyncCallback<void>): void;

  /**
   * setSlotByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;

  /**
   * getSlotsByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotsByBundle(bundle: BundleOption, callback: AsyncCallback<Array<NotificationSlot>>): void;

  /**
   * getSlotsByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotsByBundle(bundle: BundleOption): Promise<Array<NotificationSlot>>;

  /**
   * getSlotNumByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotNumByBundle(bundle: BundleOption, callback: AsyncCallback<number>): void;

  /**
   * getSlotNumByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotNumByBundle(bundle: BundleOption): Promise<number>;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(bundle: BundleOption, notificationKey: NotificationKey): Promise<void>;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(hashCode: string, callback: AsyncCallback<void>): void;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(hashCode: string): Promise<void>;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(bundle: BundleOption, callback: AsyncCallback<void>): void;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(callback: AsyncCallback<void>): void;

  /**
   * Remove all notifications under the specified user.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
   function removeAll(userId: number, callback: AsyncCallback<void>): void;
   function removeAll(userId: number): Promise<void>;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(bundle?: BundleOption): Promise<void>;

  /**
   * Obtains all active notifications in the current system. The caller must have system permissions to
   * call this method.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;

  /**
   * Obtains all active notifications in the current system. The caller must have system permissions to
   * call this method.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getAllActiveNotifications(): Promise<Array<NotificationRequest>>;

  /**
   * Obtains the number of all active notifications.
   */
  function getActiveNotificationCount(callback: AsyncCallback<number>): void;
  function getActiveNotificationCount(): Promise<number>;

  /**
   * Obtains an array of active notifications.
   */
  function getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
  function getActiveNotifications(): Promise<Array<NotificationRequest>>;

  /**
   * Cancel the notification of a specified group for this application.
   *
   * @since 8
   */
  function cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
  function cancelGroup(groupName: string): Promise<void>;

  /**
   * Delete the notification of a specified group for this application.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeGroupByBundle(bundle: BundleOption, groupName: string, callback: AsyncCallback<void>): void;
  function removeGroupByBundle(bundle: BundleOption, groupName: string): Promise<void>;

  /**
   * Set the Do Not Disturb date.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function setDoNotDisturbDate(date: DoNotDisturbDate, callback: AsyncCallback<void>): void;
  function setDoNotDisturbDate(date: DoNotDisturbDate): Promise<void>;

  /**
   * Set the Do Not Disturb date under the specified user.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
   function setDoNotDisturbDate(date: DoNotDisturbDate, userId: number, callback: AsyncCallback<void>): void;
   function setDoNotDisturbDate(date: DoNotDisturbDate, userId: number): Promise<void>;

  /**
   * Obtains the Do Not Disturb date.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getDoNotDisturbDate(callback: AsyncCallback<DoNotDisturbDate>): void;
  function getDoNotDisturbDate(): Promise<DoNotDisturbDate>;

  /**
   * Obtains the Do Not Disturb date.
   *
   * @since 8
   * @systemapi Hide this for inner system use under the specified user.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
   function getDoNotDisturbDate(userId: number, callback: AsyncCallback<DoNotDisturbDate>): void;
   function getDoNotDisturbDate(userId: number): Promise<DoNotDisturbDate>;

  /**
   * Obtains whether to support the Do Not Disturb mode.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function supportDoNotDisturbMode(callback: AsyncCallback<boolean>): void;
  function supportDoNotDisturbMode(): Promise<boolean>;

  /**
   * Obtains whether the template is supported by the system.
   *
   * @since 8
   * @param templateName Name of template to be Obtained
   * @param callback callback function
   */
  function isSupportTemplate(templateName: string, callback: AsyncCallback<boolean>): void;
  function isSupportTemplate(templateName: string): Promise<boolean>;

  /**
   * Request permission to send notification.
   *
   * @since 8
   */
   function requestEnableNotification(callback: AsyncCallback<void>): void;
   function requestEnableNotification(): Promise<void>;

  /**
   * Sets whether the device supports distributed notification.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableDistributed(enable: boolean, callback: AsyncCallback<void>): void;
  function enableDistributed(enable: boolean): Promise<void>;

  /**
   * Obtains whether the device supports distributed notification.
   *
   * @since 8
   */
  function isDistributedEnabled(callback: AsyncCallback<boolean>): void;
  function isDistributedEnabled(): Promise<boolean>;

  /**
   * Sets whether an application supports distributed notification.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableDistributedByBundle(bundle: BundleOption, enable: boolean, callback: AsyncCallback<void>): void;
  function enableDistributedByBundle(bundle: BundleOption, enable: boolean): Promise<void>;

  /**
   * Obtains whether an application supports distributed notification.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function isDistributedEnabledByBundle(bundle: BundleOption, callback: AsyncCallback<boolean>): void;
  function isDistributedEnabledByBundle(bundle: BundleOption): Promise<boolean>;

  /**
   * Obtains the remind modes of the notification.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getDeviceRemindType(callback: AsyncCallback<DeviceRemindType>): void;
  function getDeviceRemindType(): Promise<DeviceRemindType>;

  /**
   * Describes a BundleOption.
   */
  export interface BundleOption {
    bundle: string;
    uid?: number;
  }

  /**
   * Describes a NotificationKey, which can be used to identify a notification.
   */
  export interface NotificationKey {
    id: number;
    label?: string;
  }

  /**
   * The type of the Do Not Disturb.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  export enum DoNotDisturbType {
    /**
     * Non do not disturb type notification
     */
    TYPE_NONE = 0,

    /**
     * Execute do not disturb once in the set time period (only watch hours and minutes)
     */
    TYPE_ONCE = 1,

    /**
     * Execute do not disturb every day with a set time period (only watch hours and minutes)
     */
    TYPE_DAILY = 2,

    /**
     * Execute in the set time period (specify the time, month, day and hour)
     */
    TYPE_CLEARLY = 3,
  }

  /**
   * Describes a DoNotDisturbDate instance.
   *
   * @systemapi Hide this for inner system use.
   */
  export interface DoNotDisturbDate {
    /**
     * the type of the Do Not Disturb.
     *
     * @since 8
     */
    type: DoNotDisturbType;

    /**
     * the start time of the Do Not Disturb.
     *
     * @since 8
     */
    begin: Date;

    /**
     * the end time of the Do Not Disturb.
     *
     * @since 8
     */
    end: Date;
  }

  /**
   * The type of the Do Not Disturb.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
   export enum DoNotDisturbType {
    /**
     * Non do not disturb type notification
     */
    TYPE_NONE = 0,

    /**
     * Execute do not disturb once in the set time period (only watch hours and minutes)
     */
    TYPE_ONCE = 1,

    /**
     * Execute do not disturb every day with a set time period (only watch hours and minutes)
     */
    TYPE_DAILY = 2,

    /**
     * Execute in the set time period (specify the time, month, day and hour)
     */
    TYPE_CLEARLY = 3,
  }

  /**
   * Describes a DoNotDisturbDate instance.
   *
   * @systemapi Hide this for inner system use.
   */
  export interface DoNotDisturbDate {
    /**
     * the type of the Do Not Disturb.
     *
     * @since 8
     */
    type: DoNotDisturbType;

    /**
     * the start time of the Do Not Disturb.
     *
     * @since 8
     */
    begin: Date;

    /**
     * the end time of the Do Not Disturb.
     *
     * @since 8
     */
    end: Date;
  }

  /**
   * The remind type of the nofication.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  export enum DeviceRemindType {
    /**
     * The device is not in use, no reminder
     */
    IDLE_DONOT_REMIND = 0,

    /**
     * The device is not in use, remind
     */
    IDLE_REMIND = 1,

    /**
     * The device is in use, no reminder
     */
    ACTIVE_DONOT_REMIND = 2,

    /**
     * The device is in use, reminder
     */
    ACTIVE_REMIND = 3,
  }

  /**
   * Notification source type
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  export enum SourceType {
    /**
     * General notification
     */
    TYPE_NORMAL = 0,

    /**
     * Continuous notification
     */
    TYPE_CONTINUOUS = 1,

    /**
     * Scheduled notification
     */
    TYPE_TIMER = 2,
  }
}

export default notification;
