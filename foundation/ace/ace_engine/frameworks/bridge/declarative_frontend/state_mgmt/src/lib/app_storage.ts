/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

class AppStorage extends LocalStorage {

  /** 
   * ccreate and initialize singleton 
   * initialzie with all properties and their values that Object.keys(params) returns
   * Property values must not be undefined.
   */
  public static CreateSingleton(initializingPropersties?: Object): void {
    if (!AppStorage.Instance_) {
      console.log("Creating AppStorage instance.");
      AppStorage.Instance_ = new AppStorage(initializingPropersties);
    } else {
      console.error("AppStorage.CreateNewInstance(..): instance exists already, internal error!")
    }
  }


  /**
   * create and return a 'link' (two-way sync) to named property
   * @param propName name of source property in AppStorage
   * @param linkUser IPropertySubscriber to be notified when return 'link' changes,
   *      e.g. a View or PersistentStorage
   * @param subscribersName the linkUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns  SynchedPropertyTwoWay{Simple|Object| object with given AppStoage prop as  its source.
   * return undefiend if named property does not already exist in AppStorage
   */
  public static Link<T>(key: string, linkUser?: IPropertySubscriber, subscribersName? : string): ObservedPropertyAbstract<T> {
    return AppStorage.GetOrCreate().link(key, linkUser, subscribersName);
  }

  /**
   * Like link(), will create and initialize a new source property in AppStorage if missing
   * @param propName 
   * @param defaultValue value to be used for initializing if new creating new property in AppStorage
   * @param linkUser IPropertySubscriber to be notified when return 'link' changes
   * @param subscribersName the linkUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns 
   */
  public static SetAndLink<T>(key: string, defaultValue: T, linkUser?: IPropertySubscriber, subscribersName? : string): ObservedPropertyAbstract<T> {
    return AppStorage.GetOrCreate().setAndLink(key, defaultValue, linkUser, subscribersName);
  }

  /**
   * create and return a 'prop', one-way sync from named property to returned object
   * @param propName name of source property in AppStorage
   * @param propUser IPropertySubscriber to be notified when return 'prop' changes
   * @returns  SynchedPropertyOneWaySimple object with given AppStoage prop as its source.
   * return undefiend if named property does not already exist in AppStorage
   */
  public static Prop<T>(propName: string, propUser?: IPropertySubscriber, subscribersName?: string): ObservedPropertyAbstract<T> {
    return AppStorage.GetOrCreate().prop(propName, propUser, subscribersName);
  }

  /**
   * like prop, will create and initialize a new source property in AppStorage if missing
   * @param propName 
   * @param defaultValue value to be used for initializing if new creating new property in AppStorage
   * @param propUser 
   * @returns SynchedPropertyOneWaySimple object with given AppStoage prop as its source.
   */
   public static SetAndProp<S>(key: string, defaultValue: S, propUser?: IPropertySubscriber, subscribersName? : string): ObservedPropertyAbstract<S> {
    return AppStorage.GetOrCreate().setAndProp(key, defaultValue, propUser, subscribersName);
  }


  /**
   * return true if prooperty with given name exists
   * same as Map.has
   * @param propName 
   * @returns 
   */
   public static Has(key: string): boolean {
    return AppStorage.GetOrCreate().has(key);
  }


  /**
     * returns value of given property
     * return undefined if no property with this name
     * @param propName 
     * @returns 
     */
   public static Get<T>(key: string): T | undefined {
    return AppStorage.GetOrCreate().get(key);
  }

  /**
   * Set value of given property
   * set nothing and return false if property with this name does not exist
   * or if newValuye is undefined (undefined value is not allowed for state variables)
   * @param propName 
   * @param newValue 
   * @returns 
   */
   public static Set<T>(key: string, newValue: T): boolean {
    return AppStorage.GetOrCreate().set(key, newValue);
  }

  /**
   * add property if not property with given name
   * Set value of given property
   * set nothing and return false if newValuye is undefined 
   * (undefined value is not allowed for state variables)
   * @param propName 
   * @param newValue 
   * @returns 
   */
   public static SetOrCreate<T>(key: string, newValue: T): void {
    AppStorage.GetOrCreate().setOrCreate(key, newValue);
  }

  /**
   * Delete property from StorageBase
   * must only use with caution:
   * Before deleting a prop from app storage all its subscribers need to
   * unsubscribe from the property.
   * This method fails and returns false if given property still has subscribers
   * Another reason for failing is unknown property.
   *
   * @param propName
   * @returns false if method failed
   */
   public static Delete(key: string): boolean {
    return AppStorage.GetOrCreate().delete(key);
  }

  /**
   * return a Map Iterator
   * same as Map.keys
   * @param propName 
   * @returns 
   */
   public static Keys(): IterableIterator<string> {
    return AppStorage.GetOrCreate().keys();
  }


  /**
   * return number of properties
   * same as Map.size
   * @param propName 
   * @returns 
   */
   public static Size(): number {
    return AppStorage.GetOrCreate().size();
  }

  public static Clear(): boolean {
    return AppStorage.GetOrCreate().clear();
  }

  public static AboutToBeDeleted(): void {
    AppStorage.GetOrCreate().aboutToBeDeleted();
  }


  /** 
   * return number of subscribers to named property
   * useful for debug purposes
  */
   public static NumberOfSubscribersTo(propName: string): number | undefined {
    return AppStorage.GetOrCreate().numberOfSubscrbersTo(propName);
  }


  /**
   * Subscribe to value change notifications of named property
   * Any object implementing ISinglePropertyChangeSubscriber interface 
   * and registerign itself to SubscriberManager can register
   * Caution: do remember to unregister, otherwise the property will block 
   * cleanup, see delete() and clear() 
   * returns false if named property does not exist
   * @param propName 
   * @param subscriber 
   * @returns 
   */
   public static SubscribeToChangesOf<T>(propName: string, subscriber: ISinglePropertyChangeSubscriber<T>): boolean {
    return AppStorage.GetOrCreate().subscribeToChangesOf(propName, subscriber);
  }


  /**
   * inverse of subscribeToChangesOf
   * @param propName 
   * @param subscriberId 
   * @returns 
   */
   public static UnsubscribeFromChangesOf(propName: string, subscriberId: number): boolean {
    return AppStorage.GetOrCreate().unsubscribeFromChangesOf(propName, subscriberId);
  }

  public static IsMutable(key: string): boolean {
    return true;
  }


  // instance functions below:
  // Should all be protected, but TS lang does not allow access from static member to protected member

  private static Instance_: AppStorage = undefined;

  private static GetOrCreate(): AppStorage {
    if (!AppStorage.Instance_) {
      console.warn("AppStorage instance missing. Use AppStorage.CreateInstance(initObj). Creating instance without any initialization.");
      AppStorage.Instance_ = new AppStorage({});
    }
    return AppStorage.Instance_;
  }

  /** singleton class, app can not create instances */
  protected constructor(initializingProperties: Object) {
    super(initializingProperties);
  }
}


