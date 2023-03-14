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

class SynchedPropertyNesedObject<C extends Object>
  extends ObservedPropertyObjectAbstract<C>
  implements ISinglePropertyChangeSubscriber<C> {

  private obsObject_: C;

  /**
   * Construct a Property of a su component that links to a variable of parent view that holds an ObservedObject
   * example
   *   this.b.$a with b of type PC and a of type C, or
   *   this.$b[5] with this.b of type PC and array item b[5] of type C;
   *
   * @param subscribeMe
   * @param propName
   */
  constructor(obsObject: C,
    owningChildView: IPropertySubscriber, propertyName: PropertyInfo) {
    super(owningChildView, propertyName);
    this.obsObject_ = obsObject;

    // register to the ObservedObject
    ObservedObject.addOwningProperty(this.obsObject_, this);
  }

  /*
  like a destructor, need to call this before deleting
  the property.
  */
  aboutToBeDeleted() {
    // unregister from the ObservedObject
    ObservedObject.removeOwningProperty(this.obsObject_, this);
    super.aboutToBeDeleted();
  }


  // this object is subscriber to ObservedObject
  // will call this cb function when property has changed
  hasChanged(newValue: C): void {
    console.debug(`SynchedPropertyNesedObject[${this.id__()}, '${this.info() || "unknown"}']: contained ObservedObject hasChanged'.`)
    this.notifyHasChanged(this.obsObject_);
  }



  // get 'read through` from the ObservedProperty
  public get(): C {
    console.debug(`SynchedPropertyNesedObject[${this.id__()}, '${this.info() || "unknown"}']: get`)
    this.notifyPropertyRead();
    return this.obsObject_;
  }

  // set 'writes through` to the ObservedProperty
  public set(newValue: C): void {
    if (this.obsObject_ == newValue) {
      console.debug(`SynchedPropertyNesedObject[${this.id__()}IP, '${this.info() || "unknown"}']: set with unchanged value '${newValue}'- ignoring.`);
      return;
    }

    console.debug(`SynchedPropertyNesedObject[${this.id__()}, '${this.info() || "unknown"}']: set to newValue: '${newValue}'.`);

    // unsubscribe from the old value ObservedObject
    ObservedObject.removeOwningProperty(this.obsObject_, this);

    this.obsObject_ = newValue;

    // subscribe to the new value ObservedObject
    ObservedObject.addOwningProperty(this.obsObject_, this);

    // notify value change to subscribing View
    this.notifyHasChanged(this.obsObject_);
  }

  /**
 * These functions are meant for use in connection with the App Stoage and
 * business logic implementation.
 * the created Link and Prop will update when 'this' property value
 * changes.
 */
  public createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<C> {
    throw new Error("Method not supported for property linking to a nested objects.");
  }
  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<C> {
    throw new Error("Creating a 'Prop' proerty is unsuppoeted for Object type prperty value.");
  }
}
