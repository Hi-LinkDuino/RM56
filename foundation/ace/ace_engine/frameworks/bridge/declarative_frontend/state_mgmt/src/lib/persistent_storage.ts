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

class PersistentStorage implements IMultiPropertiesChangeSubscriber {
  private static Storage_: IStorage;
  private static Instance_: PersistentStorage = undefined;

  private id_: number;
  private links_: Map<string, ObservedPropertyAbstract<any>>;

  /**
   *
   * @param storage method to be used by the framework to set the backend
   * this is to be done during startup
   */
  public static ConfigureBackend(storage: IStorage): void {
    PersistentStorage.Storage_ = storage;
  }

  private static GetOrCreate(): PersistentStorage {
    if (PersistentStorage.Instance_) {
      // already initialized
      return PersistentStorage.Instance_;
    }

    PersistentStorage.Instance_ = new PersistentStorage();
    return PersistentStorage.Instance_;
  }

  public static AboutToBeDeleted(): void {
    if (!PersistentStorage.Instance_) {
      return;
    }

    PersistentStorage.GetOrCreate().aboutToBeDeleted();
    PersistentStorage.Instance_ = undefined;
  }

  public static PersistProp<T>(key: string, defaultValue: T): void {
    PersistentStorage.GetOrCreate().persistProp(key, defaultValue);
  }

  public static DeleteProp(key: string): void {
    PersistentStorage.GetOrCreate().deleteProp(key);
  }

  public static PersistProps(properties: {
    key: string,
    defaultValue: any
  }[]): void {
    PersistentStorage.GetOrCreate().persistProps(properties);
  }

  public static Keys(): Array<string> {
    let result = [];
    const it = PersistentStorage.GetOrCreate().keys();
    let val = it.next();

    while (!val.done) {
      result.push(val.value);
      val = it.next();
    }

    return result;
  }

  private constructor() {
    this.links_ = new Map<string, ObservedPropertyAbstract<any>>();
    this.id_ = SubscriberManager.Get().MakeId();
    SubscriberManager.Get().add(this);
  }

  private keys(): IterableIterator<string> {
    return this.links_.keys();
  }

  private persistProp<T>(propName: string, defaultValue: T): void {
    if (this.persistProp1(propName, defaultValue)) {
      // persist new prop
      console.debug(`PersistentStorage: writing '${propName}' - '${this.links_.get(propName)}' to storage`);
      PersistentStorage.Storage_.set(propName, this.links_.get(propName).get());
    }
  }


  // helper function to persist a property
  // does everything except writing prop to disk
  private persistProp1<T>(propName: string, defaultValue: T): boolean {
    if (defaultValue == null || defaultValue == undefined) {
      console.error(`PersistentStorage: persistProp for ${propName} called with 'null' or 'undefined' default value!`);
      return false;
    }

    if (this.links_.get(propName)) {
      console.warn(`PersistentStorage: persistProp: ${propName} is already persisted`);
      return false;
    }

    let link = AppStorage.Link(propName, this);
    if (link) {
      console.debug(`PersistentStorage: persistProp ${propName} in AppStorage, using that`);
      this.links_.set(propName, link);
    } else {
      let newValue: T = PersistentStorage.Storage_.get(propName);
      let returnValue: T;
      if (!newValue) {
        console.debug(`PersistentStorage: no entry for ${propName}, will initialize with default value`);
        returnValue = defaultValue;
      }
      else {
        returnValue = newValue;
      }
      link = AppStorage.SetAndLink(propName, returnValue, this);
      this.links_.set(propName, link);
      console.debug(`PersistentStorage: created new persistent prop for ${propName}`);
    }
    return true;
  }

  private persistProps(properties: {
    key: string,
    defaultValue: any
  }[]): void {
    properties.forEach(property => this.persistProp1(property.key, property.defaultValue));
    this.write();
  }

  private deleteProp(propName: string): void {
    let link = this.links_.get(propName);
    if (link) {
      link.aboutToBeDeleted();
      this.links_.delete(propName);
      PersistentStorage.Storage_.delete(propName);
      console.debug(`PersistentStorage: deleteProp: no longer persisting '${propName}'.`);
    } else {
      console.warn(`PersistentStorage: '${propName}' is not a persisted property warning.`);
    }
  }

  private write(): void {
    this.links_.forEach((link, propName, map) => {
      console.debug(`PersistentStorage: writing ${propName} to storage`);
      PersistentStorage.Storage_.set(propName, link.get());
    });
  }

  public propertyHasChanged(info?: PropertyInfo): void {
    console.debug("PersistentStorage: property changed");
    this.write();
  }

  // public required by the interface, use the static method instead!
  public aboutToBeDeleted(): void {
    console.debug("PersistentStorage: about to be deleted");
    this.links_.forEach((val, key, map) => {
      console.debug(`PersistentStorage: removing ${key}`);
      val.aboutToBeDeleted();
    });

    this.links_.clear();
    SubscriberManager.Get().delete(this.id__());
  }

  public id__(): number {
    return this.id_;
  }

  /**
  * This methid offers a way to force writing the property value with given
  * key to persistent storage.
  * In the general case this is unnecessary as the framework observed changes
  * and triggers writing to disk by itself. For nested objects (e.g. array of
  * objects) however changes of a property of a property as not observed. This
  * is the case where the application needs to signal to the framework.
  * @param key property that has changed
  */
  public static NotifyHasChanged(propName: string) {
    console.debug(`PersistentStorage: force writing '${propName}'-
        '${PersistentStorage.GetOrCreate().links_.get(propName)}' to storage`);
    PersistentStorage.Storage_.set(propName,
      PersistentStorage.GetOrCreate().links_.get(propName).get());
  }
};
