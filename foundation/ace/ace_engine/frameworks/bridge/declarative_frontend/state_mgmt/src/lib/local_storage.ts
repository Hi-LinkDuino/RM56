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
 * LocalStorage
 * 
 * Class implements a Map of ObservableObjectBase UI state variables.
 * Instances can be created to manage UI state within a limited "local"
 * access, and life cycle as defined by the app. 
 * AppStorage singleton is sub-class of LocalStorage for
 * UI state of app-wide access and same life cycle as the app.
 * 
 */
class LocalStorage extends NativeLocalStorage {

  protected storage_: Map<string, ObservedPropertyAbstract<any>>;

  /**
   * Construct new instance
   * initialzie with all properties and their values that Object.keys(params) returns
   * Property values must not be undefined.
   * @param initializingProperties
   */
  constructor(initializingProperties: Object = {}) {
    super();
    console.log(`${this.constructor.name} constructor: initializing with Object: ${JSON.stringify(initializingProperties)} .`)
    this.storage_ = new Map<string, ObservedPropertyAbstract<any>>();
    Object.keys(initializingProperties).filter((propName) => initializingProperties[propName] != undefined).forEach((propName) =>
      this.addNewPropertyInternal(propName, initializingProperties[propName])
    );
  }

  /**
   * Use before  deleting (letting it go out of scope) the 
   * owning Ability, Windows, or service UI.
   * 
   * Do NO use this method directly of the LocalStorage instance is managed by
   * LocalStorageLookup.
   * 
   * This method orderly closes down a LocalStorage instance
   * This requires that no property is left with one or more subscribers.
   * @returns true if all properties could be removed from storage
   */
  public aboutToBeDeleted(): boolean {
    return this.clear();
  }

  /**
   * return true if prooperty with given name exists
   * same as Map.has
   * @param propName 
   * @returns 
   */
  public has(propName: string): boolean {
    return this.storage_.has(propName);
  }


  /**
   * return a Map Iterator
   * same as Map.keys
   * @param propName 
   * @returns 
   */
  public keys(): IterableIterator<string> {
    return this.storage_.keys();
  }


  /**
   * return number of properties
   * same as Map.size
   * @param propName 
   * @returns 
   */
  public size(): number {
    return this.storage_.size;
  }


  /**
   * returns value of given property
   * return undefined if no property with this name
   * @param propName 
   * @returns 
   */
  public get<T>(propName: string): T | undefined {
    var p: ObservedPropertyAbstract<T> | undefined = this.storage_.get(propName);
    return (p) ? p.get() : undefined;
  }


  /**
   * Set value of given property
   * set nothing and return false if property with this name does not exist
   * or if newValuye is undefined (undefined value is not allowed for state variables)
   * @param propName 
   * @param newValue 
   * @returns 
   */
  public set<T>(propName: string, newValue: T): boolean {
    if (newValue == undefined) {
      console.warn(`${this.constructor.name}: set('${propName}') with newValue == undefined not allowed.`);
      return false;
    }
    var p: ObservedPropertyAbstract<T> | undefined = this.storage_.get(propName);
    if (p == undefined) {
      console.warn(`${this.constructor.name}: set: no property ${propName} error.`);
      return false;
    }
    p.set(newValue);
    return true;
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
  public setOrCreate<T>(propName: string, newValue: T): boolean {
    if (newValue == undefined) {
      console.warn(`${this.constructor.name}: setOrCreate('${propName}') with newValue == undefined not allowed.`);
      return false;
    }

    var p: ObservedPropertyAbstract<T> = this.storage_.get(propName);
    if (p) {
      console.log(`${this.constructor.name}.setOrCreate(${propName}, ${newValue}) update existing property`);
      p.set(newValue);
    } else {
      console.log(`${this.constructor.name}.setOrCreate(${propName}, ${newValue}) create new entry and set value`);
      this.addNewPropertyInternal(propName, newValue);
    }
    return true;
  }


  /**
   * Internal use helper function to create and initialize a new property.
   * caller needs to be all the checking beforehand
   * @param propName 
   * @param value 
   */
  private addNewPropertyInternal<T>(propName: string, value: T) {
    const newProp = (typeof value === "object") ?
      new ObservedPropertyObject<T>(value, undefined, propName)
      : new ObservedPropertySimple<T>(value, undefined, propName);
    this.storage_.set(propName, newProp);
  }


  /**
   * create and return a 'link' (two-way sync) to named property
   * @param propName name of source property in LocalStorage
   * @param linkUser IPropertySubscriber to be notified when source changes,
   * @param subscribersName the linkUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns  SynchedPropertyTwoWay{Simple|Object| object with given LocalStoage prop as  its source.
   * return undefiend if named property does not already exist in LocalStorage
   */
  public link<T>(propName: string, linkUser?: IPropertySubscriber, subscribersName?: string): ObservedPropertyAbstract<T> | undefined {
    var p: ObservedPropertyAbstract<T> | undefined = this.storage_.get(propName);
    if (p == undefined) {
      console.warn(`${this.constructor.name}: link: no property ${propName} error.`);
      return undefined;
    }
    let linkResult = p.createLink(linkUser, propName);
    linkResult.setInfo(subscribersName);
    return linkResult;
  }


  /**
   * Like link(), will create and initialize a new source property in LocalStorge if missing
   * @param propName name of source property in LocalStorage
   * @param defaultValue value to be used for initializing if new creating new property in LocalStorage
   * @param linkUser IPropertySubscriber to be notified when return 'link' changes,
   * @param subscribersName the linkUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns SynchedPropertyTwoWay{Simple|Object| object with given LocalStoage prop as  its source.
   */
  public setAndLink<T>(propName: string, defaultValue: T, linkUser?: IPropertySubscriber, subscribersName?: string): ObservedPropertyAbstract<T> {
    var p: ObservedPropertyAbstract<T> | undefined = this.storage_.get(propName);
    if (!p) {
      this.setOrCreate(propName, defaultValue);
    }
    return this.link(propName, linkUser, subscribersName);
  }


  /**
   * create and return a 'prop' (one-way sync) to named property
   * @param propName name of source property in LocalStorage
   * @param propUser IPropertySubscriber to be notified when source changes,
   * @param subscribersName the linkUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns  SynchedPropertyOneWay{Simple|Object| object with given LocalStoage prop as  its source.
   * return undefiend if named property does not already exist in LocalStorage
   */
  public prop<S>(propName: string, propUser?: IPropertySubscriber, subscribersName?: string): ObservedPropertyAbstract<S> | undefined {
    var p: ObservedPropertyAbstract<S> | undefined = this.storage_.get(propName);
    if (p == undefined) {
      console.warn(`${this.constructor.name}: prop: no property ${propName} error.`);
      return undefined;
    }
    let propResult = p.createProp(propUser, propName)
    propResult.setInfo(subscribersName);
    return propResult;
  }

  /**
   * Like prop(), will create and initialize a new source property in LocalStorage if missing
   * @param propName name of source property in LocalStorage
   * @param defaultValue value to be used for initializing if new creating new property in LocalStorage
   * @param propUser IPropertySubscriber to be notified when returned 'prop' changes,
   * @param subscribersName the propUser (subscriber) uses this name for the property 
   *      this name will be used in propertyChange(propName) callback of IMultiPropertiesChangeSubscriber
   * @returns  SynchedPropertyOneWay{Simple|Object| object with given LocalStoage prop as  its source.
   */
  public setAndProp<S>(propName: string, defaultValue: S, propUser?: IPropertySubscriber, subscribersName?: string): ObservedPropertyAbstract<S> {
    var p: ObservedPropertyAbstract<S> | undefined = this.storage_.get(propName);

    if (!p) {
      if (typeof defaultValue === "boolean" ||
        typeof defaultValue === "number" || typeof defaultValue === "string") {
        this.setOrCreate(propName, defaultValue);
      } else {
        return undefined;
      }
    }
    return this.prop(propName, propUser, subscribersName);
  }

  /**
   * Delete property from StorageBase
   * must only use with caution:
   * Before deleting a prop from app storage all its subscribers need to
   * unsubscribe from the property.
   * This method fails and returns false if given property still has subscribers
   * Another reason for failing is unkmown property.
   *
   * @param propName
   * @returns false if method failed
   */
  public delete(propName: string): boolean {
    var p: ObservedPropertyAbstract<any> | undefined = this.storage_.get(propName);
    if (p) {
      if (p.numberOfSubscrbers()) {
        console.error(`${this.constructor.name}: Attempt to delete property ${propName} that has \
          ${p.numberOfSubscrbers()} subscribers. Subscribers need to unsubscribe before prop deletion.`);
        return false;
      }
      p.aboutToBeDeleted();
      this.storage_.delete(propName);
      return true;
    } else {
      console.warn(`${this.constructor.name}: Attempt to delete unknown property ${propName}.`);
      return false;
    }
  }

  /**
   * delete all properties from the StorageBase
   * precondition is that there are no subscribers anymore
   * method returns false and deletes no poperties if there is any property
   * that still has subscribers
   */
  protected clear(): boolean {
    for (let propName of this.keys()) {
      var p: ObservedPropertyAbstract<any> = this.storage_.get(propName);
      if (p.numberOfSubscrbers()) {
        console.error(`${this.constructor.name}.deleteAll: Attempt to delete property ${propName} that \
          has ${p.numberOfSubscrbers()} subscribers. Subscribers need to unsubscribe before prop deletion.`);
        return false;
      }
    }
    for (let propName of this.keys()) {
      var p: ObservedPropertyAbstract<any> = this.storage_.get(propName);
      p.aboutToBeDeleted();
    }
    console.log(`${this.constructor.name}.deleteAll: success`);
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
  public subscribeToChangesOf<T>(propName: string, subscriber: ISinglePropertyChangeSubscriber<T>): boolean {
    var p: ObservedPropertyAbstract<T> | undefined = this.storage_.get(propName);
    if (p) {
      p.subscribeMe(subscriber);
      return true;
    }
    return false;
  }

  /**
   * inverse of subscribeToChangesOf
   * @param propName 
   * @param subscriberId 
   * @returns 
   */
  public unsubscribeFromChangesOf(propName: string, subscriberId: number): boolean {
    var p: ObservedPropertyAbstract<any> | undefined = this.storage_.get(propName);
    if (p) {
      p.unlinkSuscriber(subscriberId);
      return true;
    }
    return false;
  }

  /** 
   * return number of subscribers to named property
   *  useful for debug purposes
  */
  public numberOfSubscrbersTo(propName: string): number | undefined {
    var p: ObservedPropertyAbstract<any> | undefined = this.storage_.get(propName);
    if (p) {
      return p.numberOfSubscrbers();
    }
    return undefined;
  }
}
