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

/*
  class that holds an actual property value of type T
  uses its base class to manage subscribers to this
  property.
*/
class ObservedPropertyObject<T extends Object> extends ObservedPropertyObjectAbstract<T>
  implements ISinglePropertyChangeSubscriber<T> {

  private wrappedValue_: T;

  constructor(value: T, owningView: IPropertySubscriber, propertyName: PropertyInfo) {
    super(owningView, propertyName);
    this.setValueInternal(value);
  }

  aboutToBeDeleted(unsubscribeMe?: IPropertySubscriber) {
    this.unsubscribeFromOwningProperty();
    if (unsubscribeMe) {
      this.unlinkSuscriber(unsubscribeMe.id__());
    }
    super.aboutToBeDeleted();
  }

  // notification from ObservedObject value one of its
  // props has chnaged. Implies the ObservedProperty has changed
  // Note: this function gets called when in this case:
  //       thisProp.aObsObj.aProp = 47  a object prop gets changed
  // It is NOT called when
  //    thisProp.aObsObj = new ClassA
  hasChanged(newValue: T): void {
    console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}']: hasChanged`);
    this.notifyHasChanged(this.wrappedValue_);
  }

  private unsubscribeFromOwningProperty() {
    if (this.wrappedValue_) {
      if (this.wrappedValue_ instanceof SubscribaleAbstract) {
        (this.wrappedValue_ as SubscribaleAbstract).removeOwningProperty(this);
      } else {
        ObservedObject.removeOwningProperty(this.wrappedValue_, this);
      }
    }
  }
  /*
    actually update this.wrappedValue_
    called needs to do value change check
    and also notify with this.aboutToChange();
  */
  private setValueInternal(newValue: T): boolean {
    if (typeof newValue !== 'object') {
      console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}'] new value is NOT an object. Application error. Ignoring set.`);
      return false;
    }

    this.unsubscribeFromOwningProperty();

    if (ObservedObject.IsObservedObject(newValue)) {
      console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}'] new value is an ObservedObject already`);
      ObservedObject.addOwningProperty(newValue, this);
      this.wrappedValue_ = newValue;
    } else if (newValue instanceof SubscribaleAbstract) {
      console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}'] new value is an SubscribaleAbstract, subscribiung to it.`);
      this.wrappedValue_ = newValue;
      (this.wrappedValue_ as unknown as SubscribaleAbstract).addOwningProperty(this);
    } else {
      console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}'] new value is an Object, needs to be wrapped in an ObservedObject.`);
      this.wrappedValue_ = ObservedObject.createNew(newValue, this);
    }
    return true;
  }

  public get(): T {
    console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}']: get`);
    this.notifyPropertyRead();
    return this.wrappedValue_;
  }

  public set(newValue: T): void {
    if (this.wrappedValue_ == newValue) {
      console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}']: set with unchanged value - ignoring.`);
      return;
    }
    console.debug(`ObservedPropertyObject[${this.id__()}, '${this.info() || "unknown"}']: set, changed`);
    this.setValueInternal(newValue);
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
    return new SynchedPropertyObjectTwoWay(this, subscribeOwner, linkPropName);
  }

  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    throw new Error("Creating a 'Prop' proerty is unsuppoeted for Object type prperty value.");
  }
}
