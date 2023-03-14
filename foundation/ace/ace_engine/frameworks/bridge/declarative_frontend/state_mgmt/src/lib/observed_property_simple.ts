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
class ObservedPropertySimple<T> extends ObservedPropertySimpleAbstract<T>
  implements ISinglePropertyChangeSubscriber<T> {

  private wrappedValue_: T;

  constructor(value: T, owningView: IPropertySubscriber, propertyName: PropertyInfo) {
    super(owningView, propertyName);
    if (typeof value === "object") {
      throw new SyntaxError("ObservedPropertySimple value must not be an object")!
    }
    this.setValueInternal(value);
  }

  aboutToBeDeleted(unsubscribeMe?: IPropertySubscriber) {
    if (unsubscribeMe) {
      this.unlinkSuscriber(unsubscribeMe.id__());
    }
    super.aboutToBeDeleted();
  }

  hasChanged(newValue: T): void {
    console.debug(`ObservedPropertySimple[${this.id__()}, '${this.info() || "unknown"}']: hasChanged`);
    this.notifyHasChanged(this.wrappedValue_);
  }

  /*
    actually update this.wrappedValue_
    called needs to do value change check
    and also notify with this.aboutToChange();
  */
  private setValueInternal(newValue: T): void {
    console.debug(`ObservedPropertySimple[${this.id__()}, '${this.info() || "unknown"}'] new value is of simple type`);
    this.wrappedValue_ = newValue;
  }


  public get(): T {
    console.debug(`ObservedPropertySimple[${this.id__()}, '${this.info() || "unknown"}']: get returns '${JSON.stringify(this.wrappedValue_)}' .`);
    this.notifyPropertyRead();
    return this.wrappedValue_;
  }

  public set(newValue: T): void {
    if (this.wrappedValue_ == newValue) {
      console.debug(`ObservedPropertySimple[${this.id__()}, '${this.info() || "unknown"}']: set with unchanged value - ignoring.`);
      return;
    }
    console.debug(`ObservedPropertySimple[${this.id__()}, '${this.info() || "unknown"}']: set, changed from '${JSON.stringify(this.wrappedValue_)}' to '${JSON.stringify(newValue)}.`);
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
    return new SynchedPropertySimpleTwoWay(this, subscribeOwner, linkPropName);
  }
  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T> {
    return new SynchedPropertySimpleOneWaySubscribing(this, subscribeOwner, linkPropName);
  }
}
