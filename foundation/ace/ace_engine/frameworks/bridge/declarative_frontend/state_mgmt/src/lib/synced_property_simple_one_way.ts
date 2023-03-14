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

class SynchedPropertySimpleOneWay<T> extends ObservedPropertySimpleAbstract<T> {

  private wrappedValue_: T;

  constructor(value: T, subscribeMe?: IPropertySubscriber, info?: PropertyInfo) {
    super(subscribeMe, info);
    // add a test here that T is a simple type
    this.wrappedValue_ = value;
  }

  /*
    like a destructor, need to call this before deleting
    the property.
  */
  aboutToBeDeleted() {
    super.aboutToBeDeleted();
  }


  // get 'read through` from the ObservedProperty
  public get(): T {
    console.debug(`SynchedPropertySimpleOneWay[${this.id__()}, '${this.info() || "unknown"}']: get returns '${this.wrappedValue_}'`);
    this.notifyPropertyRead();
    return this.wrappedValue_;
  }

  public set(newValue: T): void {
    if (this.wrappedValue_ == newValue) {
      console.debug(`SynchedPropertySimpleOneWay[${this.id__()}, '${this.info() || "unknown"}']: set with unchanged value '${this.wrappedValue_}'- ignoring.`);
      return;
    }

    console.debug(`SynchedPropertySimpleOneWay[${this.id__()}, '${this.info() || "unknown"}']: set from '${this.wrappedValue_} to '${newValue}'.`);
    this.wrappedValue_ = newValue;
    this.notifyHasChanged(newValue);
  }

  /**
   * These functions are meant for use in connection with the App Stoage and
   * business logic implementation.
   * the created Link and Prop will update when 'this' property value
   * changes.
   */
  public createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    throw new Error("Can not create a 'Link' from a 'Prop' property. ");
  }
  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    throw new Error("Method not supported, create a SynchedPropertySimpleOneWaySubscribing from, where to create a Prop.");
  }
}


/*
  This exrension of SynchedPropertySimpleOneWay needs to be used for AppStorage
  because it needs to be notified about the source property changing
  ( there is no re-render process as in Views to update the wrappedValue )
*/
class SynchedPropertySimpleOneWaySubscribing<T> extends SynchedPropertySimpleOneWay<T>
  implements ISinglePropertyChangeSubscriber<T> {

  private linkedParentProperty_: ObservedPropertySimpleAbstract<T>;

  constructor(linkedProperty: ObservedPropertySimpleAbstract<T>, subscribeMe?: IPropertySubscriber, info?: PropertyInfo) {
    super(linkedProperty.get(), subscribeMe, info);
    this.linkedParentProperty_ = linkedProperty;
    this.linkedParentProperty_.subscribeMe(this);
  }


  aboutToBeDeleted() {
    // unregister from parent of this prop
    this.linkedParentProperty_.unlinkSuscriber(this.id__());
    super.aboutToBeDeleted();
  }


  hasChanged(newValue: T): void {
    console.debug(`SynchedPropertySimpleOneWaySubscribing[${this.id__()}, '${this.info() || "unknown"}']: source property hasChanged'.`)
    this.set(newValue);
  }

  /**
   * These functions are meant for use in connection with the App Stoage and
   * business logic implementation.
   * the created Link and Prop will update when 'this' property value
   * changes.
   */
  public createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    throw new Error("Can not create a 'Link' from a 'Prop' property. ");
  }
  public createProp(subscribeOwner?: IPropertySubscriber,
    propPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    return new SynchedPropertySimpleOneWaySubscribing<T>(this, subscribeOwner, propPropName);
  }
}
