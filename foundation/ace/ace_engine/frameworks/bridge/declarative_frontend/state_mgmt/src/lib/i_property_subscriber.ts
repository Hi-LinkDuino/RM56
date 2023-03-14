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

type PropertyInfo = string;

/*
  an instance that gets and sets the value of an IProperty
  these are OneWaySyncedProperty, SyncedPropertyTwoWay
  and (to be designed) View
*/
interface IPropertySubscriber {

  // globally unique id
  id__(): number;

  // inform the subscribed property
  // that the subscriber is about to be deleted
  // hence , unsubscribe
  aboutToBeDeleted(owningView?: IPropertySubscriber): void;
}

interface ISinglePropertyChangeSubscriber<T> extends IPropertySubscriber {
  // get informed after the property has changed.
  hasChanged(newValue: T): void;
}

interface IMultiPropertiesChangeSubscriber extends IPropertySubscriber {
  // get informed that given property has changed.
  // Note: a Property's PropertyInfo can be undefined.
  propertyHasChanged(info?: PropertyInfo): void;
}

interface IMultiPropertiesReadSubscriber extends IPropertySubscriber {
  // get informed that given property has been read (get access).
  // Note: a Property's PropertyInfo can be undefined.
  propertyRead(info?: PropertyInfo): void;
}
