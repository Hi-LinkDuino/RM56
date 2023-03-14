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

class SubscriberManager implements IPropertySubscriberLookup {
  private subscriberById_: Map<number, IPropertySubscriber>;
  private nextFreeId_: number;

  private static INSTANCE_: SubscriberManager = new SubscriberManager();

  public static Get() { return SubscriberManager.INSTANCE_; }

  constructor() {
    this.subscriberById_ = new Map<number, IPropertySubscriber>();
    this.nextFreeId_ = 0;
    console.debug("SubscriberManager has been created.");
  }

  public has(id: number): boolean {
    return this.subscriberById_.has(id);
  }

  public get(id: number): IPropertySubscriber {
    return this.subscriberById_.get(id);
  }

  public delete(id: number): boolean {
    return this.subscriberById_.delete(id);
  }

  public add(newSubsriber: IPropertySubscriber): boolean {
    if (this.has(newSubsriber.id__())) {
      return false;
    }
    this.subscriberById_.set(newSubsriber.id__(), newSubsriber);
    return true;
  }

  /**
   * Method for testing purposes
   * @returns number of subscribers
   */
  public numberOfSubscrbers(): number {
    return this.subscriberById_.size;
  }

  /**
   * for debug purposes dump all known subscriber's info to comsole
   */
  public dumpSubscriberInfo(): void {
    console.debug("Dump of SubscriberManager +++ (sart)")
    for (let [id, subscriber] of this.subscriberById_) {
      console.debug(`Id: ${id} -> ${subscriber['info'] ? subscriber['info']() : 'unknown'}`)
    }
    console.debug("Dump of SubscriberManager +++ (end)")
  }

  MakeId() {
    return this.nextFreeId_++;
  }
}
