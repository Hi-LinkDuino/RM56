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

interface IPropertySubscriberLookup {

  /*
  chk subscriber is known
  same as Map.has
*/
  has(id: number): boolean;

  /*
  retrieve subscriber by id
  same as Map.get
  */
  get(id: number): IPropertySubscriber;

  /*
    unregister a subscriber
    same as Map.delete
    */
  delete(id: number): boolean;

  /*
  add a new subscriber
  same as Map.set
  */
  add(newSubsriber: IPropertySubscriber): boolean;
}
