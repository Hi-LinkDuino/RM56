/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/**
 * Provides an interface for attribute subscribers.
 * @since 7
 * @systemapi
 */
interface IPropertySubscriber {
  /**
   * Called when the ID of the property subscriber is queried.
   * @since 7
   * @systemapi
   */
  id(): number;

  /**
   * Provides a single attribute change user interface.
   * @since 7
   * @systemapi
   */
  aboutToBeDeleted(owningView?: IPropertySubscriber): void;
}

/**
 * Defines the subscriber.
 * @since 7
 * @systemapi
 */
interface ISinglePropertyChangeSubscriber<T> extends IPropertySubscriber {
  /**
   * Provides a single attribute change user interface.
   * @since 7
   * @systemapi
   */
  hasChanged(newValue: T): void;
}

/**
 * Defines the subscribed abstract property.
 * @since 7
 * @systemapi
 */
declare abstract class SubscribedAbstractProperty<T> {
  /**
   * Setting Subscribers.
   * @since 7
   * @systemapi
   */
  protected subscribers_: Set<number>;

  /**
   * Private user ID.
   * @since 7
   * @systemapi
   */
  private id_;

  /**
   * Private user information.
   * @since 7
   * @systemapi
   */
  private info_?;

  /**
   * @since 7
   * @systemapi
   */
  constructor(
    /**
     * Subscriber IPropertySubscriber.
     * @since 7
     * @systemapi
     */
    subscribeMe?: IPropertySubscriber,
    /**
     * Subscriber info.
     * @since 7
     * @systemapi
     */
    info?: string,
  );

  /**
   * Called when the subscriber ID is entered.
   * @since 7
   * @systemapi
   */
  id(): number;

  /**
   * Called when a subscriber information description is entered.
   * @since 7
   * @systemapi
   */
  info(): string;

  /**
   * Called when data is obtained.
   * @since 7
   * @systemapi
   */
  abstract get(): T;

  /**
   * Called when data is created.
   * @since 7
   * @systemapi
   */
  abstract set(newValue: T): void;

  /**
   * Called when a two-way synchronization is created.
   * @since 7
   * @systemapi
   */
  createTwoWaySync(subscribeMe?: IPropertySubscriber, info?: string): SyncedPropertyTwoWay<T>;

  /**
   * Called when a one-way synchronization is created.
   * @since 7
   * @systemapi
   */
  createOneWaySync(subscribeMe?: IPropertySubscriber, info?: string): SyncedPropertyOneWay<T>;

  /**
   * Called when the subscriber is unlinked.
   * @since 7
   * @systemapi
   */
  unlinkSuscriber(subscriberId: number): void;

  /**
   * Called when the notification has changed.
   * @since 7
   * @systemapi
   */
  protected notifyHasChanged(newValue: T): void;

  /**
   * Called when the notification property is read.
   * @since 7
   * @systemapi
   */
  protected notifyPropertyRead(): void;

  /**
   * Called when the number of users is queried.
   * @since 7
   * @systemapi
   */
  numberOfSubscrbers(): number;
}

/**
 * Defines the state value.
 * @since 7
 * @systemapi
 */
declare class SyncedPropertyTwoWay<T>
  extends SubscribedAbstractProperty<T>
  implements ISinglePropertyChangeSubscriber<T>
{
  /**
   * Sources of synchronization attributes bidirectionally.
   * @since 7
   * @systemapi
   */
  private source_;

  /**
   * constructor parameters.
   * @since 7
   * @systemapi
   */
  constructor(source: SubscribedAbstractProperty<T>, subscribeMe?: IPropertySubscriber, info?: string);

  /**
   * Called when processing information about to be deleted.
   * @since 7
   * @systemapi
   */
  aboutToBeDeleted(unsubscribeMe?: IPropertySubscriber): void;

  /**
   * Information Changed.
   * @since 7
   * @systemapi
   */
  hasChanged(newValue: T): void;

  /**
   * Called when data is obtained.
   * @since 7
   * @systemapi
   */
  get(): T;

  /**
   * Called when data is created.
   * @since 7
   * @systemapi
   */
  set(newValue: T): void;
}

/**
 * Defines the prop state value.
 * @since 7
 * @systemapi
 */
declare class SyncedPropertyOneWay<T>
  extends SubscribedAbstractProperty<T>
  implements ISinglePropertyChangeSubscriber<T>
{
  /**
   * Pack value for single-item binding.
   * @since 7
   * @systemapi
   */
  private wrappedValue_;

  /**
   * Sources of synchronization attributes bidirectionally.
   * @since 7
   * @systemapi
   */
  private source_;

  /**
   * Constructor parameters.
   * @since 7
   * @systemapi
   */
  constructor(source: SubscribedAbstractProperty<T>, subscribeMe?: IPropertySubscriber, info?: string);

  /**
   * Called when processing information about to be deleted.
   * @since 7
   * @systemapi
   */
  aboutToBeDeleted(unsubscribeMe?: IPropertySubscriber): void;

  /**
   * Information Changed.
   * @since 7
   * @systemapi
   */
  hasChanged(newValue: T): void;

  /**
   * Called when data is obtained.
   * @since 7
   * @systemapi
   */
  get(): T;

  /**
   * Called when data is created.
   * @since 7
   * @systemapi
   */
  set(newValue: T): void;
}

/**
 * Defines the AppStorage interface.
 * @since 7
 */
declare class AppStorage {
  /**
   * Called when a link is set.
   * @since 7
   */
  static Link(propName: string): any;

  /**
   * Called when a hyperlink is set.
   * @since 7
   */
  static SetAndLink<T>(propName: string, defaultValue: T): SubscribedAbstractProperty<T>;

  /**
   * Called when a property is set.
   * @since 7
   */
  static Prop(propName: string): any;

  /**
   * Called when dynamic properties are set.
   * @since 7
   */
  static SetAndProp<S>(propName: string, defaultValue: S): SubscribedAbstractProperty<S>;

  /**
   * Called when owning or not.
   * @since 7
   */
  static Has(propName: string): boolean;

  /**
   * Called when data is obtained.
   * @since 7
   */
  static Get<T>(propName: string): T | undefined;

  /**
   * Called when setting.
   * @since 7
   */
  static Set<T>(propName: string, newValue: T): boolean;

  /**
   * Called when setting or creating.
   * @since 7
   */
  static SetOrCreate<T>(propName: string, newValue: T): void;

  /**
   * Called when a deletion is made.
   * @since 7
   */
  static Delete(propName: string): boolean;

  /**
   * Called when a dictionary is sorted.
   * @since 7
   */
  static Keys(): IterableIterator<string>;

  /**
   * Called when a cleanup occurs.
   * @since 7
   */
  static staticClear(): boolean;

  /**
   * Called when the data can be changed.
   * @since 7
   */
  static IsMutable(propName: string): boolean;

  /**
   * Called when you check how much data is stored.
   * @since 7
   */
  static Size(): number;
}

/**
 * Defines the Environment interface.
 * @since 7
 */
declare class Environment {
  /**
   * Constructor.
   * @since 7
   * @systemapi
   * @hide
   */
  constructor();

  /**
   * Called when a property value is checked.
   * @since 7
   */
  static EnvProp<S>(key: string, value: S): boolean;

  /**
   * Called when multiple property values are checked.
   * @since 7
   */
  static EnvProps(
    props: {
      key: string;
      defaultValue: any;
    }[],
  ): void;

  /**
   * Set the key value.
   * @since 7
   */
  static Keys(): Array<string>;
}

/**
 * Defines the ColorMode of device.
 * @since 7
 */
declare enum ColorMode {
  /**
   * Light mode.
   * @since 7
   */
  LIGHT = 0,

  /**
   * Dark mode.
   * @since 7
   */
  DARK,
}

/**
 * Defines the LayoutDirection of device.
 * @since 7
 */
declare enum LayoutDirection {
  /**
   * Elements are laid out from left to right.
   * @since 7
   */
  LTR,

  /**
   * Elements are laid out from right to left.
   * @since 7
   */
  RTL,

  /**
   * Elements are laid out from auto.
   * @since 8
   */
  Auto,
}

/**
 * Defines the PersistentStorage interface.
 * @since 7
 */
declare class PersistentStorage {
  /**
   * Constructor parameters.
   * @since 7
   * @systemapi
   * @hide
   */
  constructor(appStorage: AppStorage, storage: Storage);

  /**
   * Called when a persistence property is stored.
   * @since 7
   */
  static PersistProp<T>(key: string, defaultValue: T): void;

  /**
   * Called when a property is deleted.
   * @since 7
   */
  static DeleteProp(key: string): void;

  /**
   * Called when multiple persistence properties are stored.
   * @since 7
   */
  static PersistProps(
    properties: {
      key: string;
      defaultValue: any;
    }[],
  ): void;

  /**
   * Set the key value.
   * @since 7
   */
  static Keys(): Array<string>;
}

/**
 * Defines the base class of storage.
 * @since 7
 * @systemapi
 */
declare class Storage {
  /**
   * Constructor parameters.
   * @since 7
   * @systemapi
   * @hide
   */
  constructor(needCrossThread?: boolean, file?: string);

  /**
   * Called when data is obtained.
   * @since 7
   * @systemapi
   * @hide
   */
  get(key: string): string | undefined;

  /**
   * Called when setting.
   * @since 7
   * @systemapi
   * @hide
   */
  set(key: string, val: any): void;

  /**
   * Called when data is cleared.
   * @since 7
   * @systemapi
   * @hide
   */
  clear(): void;

  /**
   * Called when data is deleted.
   * @since 7
   * @systemapi
   * @hide
   */
  delete(key: string): void;
}

/**
 * Defines the Subscribale base class.
 * @since 7
 * @systemapi
 * @hide
 */
declare abstract class SubscribaleAbstract {
  /**
   * Returns the ownership attribute set by the.
   * @since 7
   * @systemapi
   * @hide
   */
  private owningProperties_: Set<number>;

  /**
   * Constructor.
   * @since 7
   * @systemapi
   * @hide
   */
  constructor();

  /**
   * Called when the notification property has changed.
   * @since 7
   * @systemapi
   * @hide
   */
  protected notifyPropertyHasChanged(propName: string, newValue: any): void;

  /**
   * Called when adding an already owned property.
   * @since 7
   * @systemapi
   * @hide
   */
  public addOwningProperty(subscriber: IPropertySubscriber): void;

  /**
   * Called when an already owned property is deleted.
   * @since 7
   * @systemapi
   * @hide
   */
  public removeOwningProperty(property: IPropertySubscriber): void;

  /**
   * Called when an already owned property is deleted by ID
   * @since 7
   * @systemapi
   * @hide
   */
  public removeOwningPropertyById(subscriberId: number): void;
}

/**
 * Used for ide.
 * @since 7
 * @systemapi
 * @hide
 */
declare const appStorage: AppStorage;
