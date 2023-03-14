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

class SynchedPropertyObjectTwoWay<C extends Object>
  extends ObservedPropertyObjectAbstract<C>
  implements ISinglePropertyChangeSubscriber<C> {

  private linkedParentProperty_: ObservedPropertyObjectAbstract<C>;

  constructor(linkSouce: ObservedPropertyObjectAbstract<C>,
    owningChildView: IPropertySubscriber,
    thisPropertyName: PropertyInfo) {
    super(owningChildView, thisPropertyName);
    this.linkedParentProperty_ = linkSouce;
    // register to the parent property
    this.linkedParentProperty_.subscribeMe(this);

    // register to the ObservedObject
    ObservedObject.addOwningProperty(this.getObject(), this);
  }

  /*
  like a destructor, need to call this before deleting
  the property.
  */
  aboutToBeDeleted() {
    // unregister from parent of this link
    this.linkedParentProperty_.unlinkSuscriber(this.id__());

    // unregister from the ObservedObject
    ObservedObject.removeOwningProperty(this.getObject(), this);
    super.aboutToBeDeleted();
  }

  private getObject(): C {
    this.notifyPropertyRead();
    return this.linkedParentProperty_.get();
  }

  private setObject(newValue: C): void {
    this.linkedParentProperty_.set(newValue)
  }

  // this object is subscriber to ObservedObject
  // will call this cb function when property has changed
  hasChanged(newValue: C): void {
    console.debug(`SynchedPropertyObjectTwoWay[${this.id__()}, '${this.info() || "unknown"}']: contained ObservedObject hasChanged'.`)
    this.notifyHasChanged(this.getObject());
  }



  // get 'read through` from the ObservedProperty
  public get(): C {
    console.debug(`SynchedPropertyObjectTwoWay[${this.id__()}, '${this.info() || "unknown"}']: get`)
    return this.getObject();
  }

  // set 'writes through` to the ObservedProperty
  public set(newValue: C): void {
    if (this.getObject() == newValue) {
      console.debug(`SynchedPropertyObjectTwoWay[${this.id__()}IP, '${this.info() || "unknown"}']: set with unchanged value '${newValue}'- ignoring.`);
      return;
    }

    console.debug(`SynchedPropertyObjectTwoWay[${this.id__()}, '${this.info() || "unknown"}']: set to newValue: '${newValue}'.`);

    ObservedObject.removeOwningProperty(this.getObject(), this);
    this.setObject(newValue);
    ObservedObject.addOwningProperty(this.getObject(), this);

    this.notifyHasChanged(newValue);
  }

  /**
 * These functions are meant for use in connection with the App Stoage and
 * business logic implementation.
 * the created Link and Prop will update when 'this' property value
 * changes.
 */
  public createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<C> {
    return new SynchedPropertyObjectTwoWay(this, subscribeOwner, linkPropName);
  }
  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<C> {
    throw new Error("Creating a 'Prop' proerty is unsuppoeted for Object type prperty value.");
  }

}
