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

/**
 * Abstract class that manages subscribing properties
 * that implement the interfaces ISinglePropertyChangeSubscriber
 * and/or IMultiPropertiesChangeSubscriber. Each using @State, @Link, etc
 * decorated varibale in a component will make its own subscription.
 * When the component is created the subscription is added, and when the
 * component is deleted it unsubscribes.
 *
 * About lifecycle: It is legal use for two components with two @State
 * decorated variables to share the same instance to a SubscribaleAbstract
 * object. Each such decorated variable implementation makes its own
 * subscription to the SubscribaleAbstract object. Hence, when both variables
 * have unsubscribed the SubscribaleAbstract may do its own de-initilialization.,
 * e.g. release held external resources.
 *
 * How to extend:
 * A subclass manages the get and set to one or several properties on its own.
 * The subclass needs to notify all relevant value changes to the framework for the
 * UI to be updated. Notification should only be given for class properties that
 * are used to generate the UI.
 *
 * A subclass must call super() in its constructor to let this base class
 * initialize itself.
 *
 * A subclass must call 'notifyPropertyHasChanged' after the relevant property
 * has changes. The framework will notify all dependent components to re-render.
 *
 * A sub-class may overwrite the 'addOwningProperty' function to add own
 * functionality, but it must call super.addowningOwningProperty(..). E.g.
 * the sub-class could connect to external resources upon the first subscriber.
 *
 * A sub-class may also overwrite the 'removeOwningProperty' function or
 * 'removeOwningPropertyById' function to add own functionality,
 * but it must call super.removeOwningProperty(..).
 * E.g. the sub-class could release held external resources upon loosing the
 * last subscriber.
 *
 */

abstract class SubscribaleAbstract {

  // keeps track of all subscribing properties
  private owningProperties_: Set<number>;

  /**
   * make sure the call super from subclass constructor!
   */
  constructor() {
    this.owningProperties_ = new Set<number>();
    console.debug(`SubscribaleAbstract: construcstor done`);
  }

  /**
  * A subsclass must call this function whenever one of its properties has
   * changed that is used to construct the UI.
   * @param propName name of the change property
   * @param newValue the property value after the change
   */
  protected notifyPropertyHasChanged(propName: string, newValue: any) {
    console.debug(`SubscribaleAbstract: notifyPropertyHasChanged '${propName}'.`)
    var registry: IPropertySubscriberLookup = SubscriberManager.Get();
    this.owningProperties_.forEach((subscribedId) => {
      var owningProperty: IPropertySubscriber = registry!.get(subscribedId)
      if (owningProperty) {
        if ('hasChanged' in owningProperty) {
          (owningProperty as ISinglePropertyChangeSubscriber<any>).hasChanged(newValue);
        }
        if ('propertyHasChanged' in owningProperty) {
          (owningProperty as IMultiPropertiesChangeSubscriber).propertyHasChanged(propName);
        }
      } else {
        console.error(`SubscribaleAbstract: notifyHasChanged: unknown subscriber.'${subscribedId}' error!.`);
      }
    });
  }

  /**
   * Method used by the framework to add subscribing decorated variables
   * Subclass may overwrite this function but must call the function of the base
   * class from its own implementation.
   * @param subscriber new subscriber that implements ISinglePropertyChangeSubscriber
   * and/or IMultiPropertiesChangeSubscriber interfaces
   */

  public addOwningProperty(subscriber: IPropertySubscriber): void {
    console.debug(`SubscribaleAbstract: addOwningProperty: subscriber '${subscriber.id__()}'.`)
    this.owningProperties_.add(subscriber.id__());
  }

  /**
   * Method used by the framework to ubsubscribing decorated variables
   * Subclass may overwrite this function but must call the function of the base
   * class from its own implementation.
   * @param subscriber subscriber that implements ISinglePropertyChangeSubscriber
   * and/or IMultiPropertiesChangeSubscriber interfaces
   */
  public removeOwningProperty(property: IPropertySubscriber): void {
    return this.removeOwningPropertyById(property.id__());
  }

  public removeOwningPropertyById(subscriberId: number): void {
    console.debug(`SubscribaleAbstract: removeOwningProperty '${subscriberId}'.`)
    this.owningProperties_.delete(subscriberId);
  }
}
