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

/*
  Overview of the Observed Property class hiararchy

  ObservedPropertyAbstract
     |-- ObservedSimplePropertyAbstract - boolean, number, string
     |         |-- ObservedSimpleProperty - owns the property
     |         |-- SynchedSimplePropertyOneWay - one way sync from ObservedSimpleProperty
     |         |        |--SynchedPropertySimpleOneWaySubscribing - one way sync
     |         |           from ObservedSimpleProperty, return value of AppStorage.prop(..)
     |         |-- SynchedSimplePropertyTwoWay - two way sync with ObservedSimpleProperty
     |
     |-- ObservedObjectPropertyAbstract - Object proxied by ObservedObject
               |-- ObservedObjectProperty - owns the property
               |-- SynchedObjectPropertyTwoWay - two way sync with ObservedObjectProperty

*/

/*
   manage subscriptions to a property
   managing the property is left to sub
   classes
   Extended by ObservedProperty, SyncedPropertyOneWay
   and SyncedPropertyTwoWay
*/
abstract class ObservedPropertyAbstract<T>  {
  protected subscribers_: Set<number>
  private id_: number;
  private info_?: PropertyInfo;

  constructor(subscribeMe?: IPropertySubscriber, info?: PropertyInfo) {
    this.subscribers_ = new Set<number>();
    this.id_ = SubscriberManager.Get().MakeId();
    SubscriberManager.Get().add(this);
    if (subscribeMe) {
      this.subscribers_.add(subscribeMe.id__());
    }
    if (info) {
      this.info_ = info;
    }
  }

  aboutToBeDeleted() {
    SubscriberManager.Get().delete(this.id__())
  }

  id__(): number {
    return this.id_;
  }

  public info(): PropertyInfo {
    return this.info_;
  }
  public setInfo(propName: PropertyInfo) {
    if (propName && propName != "") {
      this.info_ = propName;
    }
  }

  public abstract get(): T;
  public abstract set(newValue: T): void;


  public subscribeMe(subscriber: ISinglePropertyChangeSubscriber<T>): void {
    console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']: subscribeMe: Property new subscriber '${subscriber.id__()}'`);
    this.subscribers_.add(subscriber.id__());
  }

  /*
    the inverse function of createOneWaySync or createTwoWaySync
  */
  public unlinkSuscriber(subscriberId: number): void {
    this.subscribers_.delete(subscriberId);
  }

  protected notifyHasChanged(newValue: T) {
    //console.debug(`ObservedPropertyAbstract[${this.id()}, '${this.info() || "unknown"}']: notifyHasChanged to newValue '${JSON.stringify(newValue)}', notifying.`)
    console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']: notifyHasChanged, notifying.`);
    var registry: IPropertySubscriberLookup = SubscriberManager.Get();
    this.subscribers_.forEach((subscribedId) => {
      var subscriber: IPropertySubscriber = registry!.get(subscribedId)
      if (subscriber) {
        if ('hasChanged' in subscriber) {
          (subscriber as ISinglePropertyChangeSubscriber<T>).hasChanged(newValue);
        }
        if ('propertyHasChanged' in subscriber) {
          (subscriber as IMultiPropertiesChangeSubscriber).propertyHasChanged(this.info_);
        }
      } else {
        console.error(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']: notifyHasChanged: unknown subscriber ID '${subscribedId}' error!`);
      }
    });
  }

  protected notifyPropertyRead() {
    console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']: propertyRead.`)
    var registry: IPropertySubscriberLookup = SubscriberManager.Get();
    this.subscribers_.forEach((subscribedId) => {
      var subscriber: IPropertySubscriber = registry!.get(subscribedId)
      if (subscriber) {
        if ('propertyRead' in subscriber) {
          (subscriber as IMultiPropertiesReadSubscriber).propertyRead(this.info_);
        }
      }
    });
  }

  /*
  return numebr of subscribers to this property
  mostly useful for unit testin
  */
  public numberOfSubscrbers(): number {
    return this.subscribers_.size;
  }


  /**
   * These functions are meant for use in connection with the App Stoage and
   * business logic implementation.
   * the created Link and Prop will update when 'this' property value
   * changes.
   */
  public abstract createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T>;
  public abstract createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstract<T>;

  /**
   * factory function for concrete 'object' or 'simple' ObservedProperty object
   * depending if value is Class object
   * or simple type (boolean | number | string)
   * @param value
   * @param owningView
   * @param thisPropertyName
   * @returns either
   */
  static CreateObservedObject<C>(value: C, owningView: IPropertySubscriber, thisPropertyName: PropertyInfo)
    : ObservedPropertyAbstract<C> {
    return (typeof value === "object") ?
      new ObservedPropertyObject(value, owningView, thisPropertyName)
      : new ObservedPropertySimple(value, owningView, thisPropertyName);
  }
}
