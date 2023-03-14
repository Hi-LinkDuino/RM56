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

const nestedObsObjProps = tsuite("Nested Observed Object and Properties", () => {

  @Observed
  class ClassA {
    public c: number;

    constructor(c: number) {
      this.c = c;
    }
  }

  @Observed
  class ClassB {
    public a: ClassA;

    constructor(a: ClassA) {
      this.a = a;
    }
  }

  class SubscribingView implements IMultiPropertiesChangeSubscriber {

    private id_: number = SubscriberManager.Get().MakeId();
    private label_: string;

    constructor(label: string) {
      SubscriberManager.Get().add(this);
      this.label_ = label;
    }

    // globally unique id
    id__(): number {
      return this.id_;
    }

    // inform the subscribed property
    // that the subscriber is about to be deleted
    // hence , unsubscribe
    aboutToBeDeleted(): void {
      SubscriberManager.Get().delete(this.id__());
    }

    // implements IMultiPropertiesChangePublisher
    propertyHasChanged(info?: PropertyInfo): void {
      console.log(`SubscribingView '${this.label_}' object property ${info ? info : 'unknown'} has changed.`);
    }
  }

  tcase("Two views having a property referring to a1: ClassA", () => {

    // View of property class B, has a property v1b1
    let v1p1b1 = new SubscribingView("v1p1b1");
    let p1b1: ObservedPropertyObjectAbstract<ClassB> = new ObservedPropertyObject<ClassB>(new ClassB(new ClassA(47)), v1p1b1, "p1-b1");
    let v1p1b1Spy = spyOn(v1p1b1, "propertyHasChanged");


    // 1. child view v1p1a1 of above View has a property 'a'
    let v1p1a1 = new SubscribingView("v1p1a1");
    let p1a1: ObservedPropertyObjectAbstract<ClassA> =
      new SynchedPropertyNesedObject<ClassA>(p1b1.get().a, v1p1a1, "a");
    let v1p1a1Spy = spyOn(v1p1a1, "propertyHasChanged");

    test("readback p1a1", p1a1.get().c == 47);

    // 2. child view v2p2a1 of above View has a property 'a'
    let v2p2a1 = new SubscribingView("v2p2a1");
    let p2a1: ObservedPropertyObjectAbstract<ClassA> =
      new SynchedPropertyNesedObject<ClassA>(p1b1.get().a, v2p2a1, "a");
    let v2p2a1Spy = spyOn(v2p2a1, "propertyHasChanged");

    test("readback p2a1", p2a1.get().c == 47);

    p1b1.get().a.c = 48;

    test("readback p1a1", p1a1.get().c == 48);
    test("readback p2a1", p2a1.get().c == 48)
    test("readback p2b1", JSON.stringify(p1b1.get().a) == JSON.stringify({ "c": 48 }))

    test("owning view of p1a1 (v1p1a1): propertyHasChanged called", v1p1a1Spy.called);
    test("owning view of v2p2a1 (v2p2a1): propertyHasChanged called", v2p2a1Spy.called);

    p1a1.aboutToBeDeleted();
    v1p1a1.aboutToBeDeleted();
    p2a1.aboutToBeDeleted();
    v2p2a1.aboutToBeDeleted();
    p1b1.aboutToBeDeleted();
    v1p1b1.aboutToBeDeleted();

    test(`After cleanup: SubscriberManager num of subscribers is ${SubscriberManager.Get().numberOfSubscrbers()} should be 0 .`, SubscriberManager.Get().numberOfSubscrbers() == 0);

  });

});
