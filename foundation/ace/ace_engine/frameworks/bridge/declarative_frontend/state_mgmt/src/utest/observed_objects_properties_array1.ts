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

const array1ObsObjProps = tsuite("Array of Observed Object and Properties", () => {

  @Observed
  class ClassA {
    public c: number;

    constructor(c: number) {
      this.c = c;
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

  tcase("Array of ClassA", () => {
    let arr: ClassA[] = [new ClassA(0), new ClassA(1), new ClassA(2)];
    let parentView = new SubscribingView("parentView");
    let parentViewProp: ObservedPropertyObjectAbstract<ClassA[]>
      = new ObservedPropertyObject<ClassA[]>(arr, parentView, "parentViewProp");
    let parentViewSpy = spyOn(parentView, "propertyHasChanged");

    let childViews = [
      new SubscribingView("childView0"),
      new SubscribingView("childView1"),
      new SubscribingView("childView2")
    ];

    let childViewProps: ObservedPropertyObjectAbstract<ClassA>[] = [];
    [0, 1, 2].forEach((i) =>
      childViewProps.push(
        new SynchedPropertyNesedObject<ClassA>(arr[i], childViews[i], `${i}`))
    );


    let childViewsSpys = [
      spyOn(childViews[0], "propertyHasChanged"),
      spyOn(childViews[1], "propertyHasChanged"),
      spyOn(childViews[2], "propertyHasChanged")
    ];

    // parentViewProp.get()[0].c = 48;
    parentViewProp.get()[0] = new ClassA(48);

    test("readback item 0", parentViewProp.get()[0].c == 48);
    test("readback item 1", parentViewProp.get()[1].c == 1);
    test("readback item 2", parentViewProp.get()[2].c == 2);

    test("parentView: propertyHasChanged called", parentViewSpy.called);

    arr.push(new ClassA(3));
    childViews.push(new SubscribingView("childViewAdded"));
    childViewProps.push(
      new SynchedPropertyNesedObject<ClassA>(arr[3], childViews[3], `${3}`)
    );

    childViewsSpys.push(spyOn(childViews[3], "propertyHasChanged"));
    test("readback added item 3", parentViewProp.get()[3].c == 3);

    arr[3].c = 17;
    test("readback added item 3 after value change", parentViewProp.get()[3].c == 17);
    test("childViews propertyHasChanged", childViewsSpys[3].called);

    // reset spys
    childViewsSpys[3].called = false;
    parentViewSpy.called = false;

    parentViewProp.get()[3] = new ClassA(4711);
    test("readback added item 3 after object change", parentViewProp.get()[3].c == 4711);
    test("parentView propertyHasChanged", parentViewSpy.called);


    [0, 1, 2, 3].forEach(i => {
      childViewProps[i].aboutToBeDeleted();
      childViews[i].aboutToBeDeleted();
    });
    parentViewProp.aboutToBeDeleted();
    parentView.aboutToBeDeleted();

    test(`After cleanup: SubscriberManager num of subscribers is ${SubscriberManager.Get().numberOfSubscrbers()} should be 0 .`, SubscriberManager.Get().numberOfSubscrbers() == 0);
  });
});
