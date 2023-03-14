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

const testAppStorageArray = tsuite("AppStorage Array", () => {

  class ArrayChangeSubscriber implements ISinglePropertyChangeSubscriber<number[]> {

    private id_: number = SubscriberManager.Get().MakeId();

    constructor() {
      AppStorage.SubscribeToChangesOf("arr", this);
      SubscriberManager.Get().add(this);
    }

    // globally unique id
    id__(): number {
      return this.id_;
    }

    // inform the subscribed property
    // that the subscriber is about to be deleted
    // hence , unsubscribe
    aboutToBeDeleted(owningView?: IPropertySubscriber): void {
      AppStorage.UnsubscribeFromChangesOf("arr", this.id__());
      SubscriberManager.Get().delete(this.id__());
    }

    // get informed after the property has changed.
    hasChanged(newValue: number[]): void {
      console.log(`ArrayChangeSubscriber.newValue ${newValue}.`)
    }
  }

  class ClassPropertyChangeSubscriber<T extends Object>
    implements ISinglePropertyChangeSubscriber<T>, IMultiPropertiesChangeSubscriber {

    private id_: number = SubscriberManager.Get().MakeId();
    private classInfo_: string;

    constructor(classInfo: string) {
      this.classInfo_ = classInfo;
      SubscriberManager.Get().add(this);
    }

    // globally unique id
    id__(): number {
      return this.id_;
    }

    // inform the subscribed property
    // that the subscriber is about to be deleted
    // hence , unsubscribe
    aboutToBeDeleted(owningView?: IPropertySubscriber): void {
      SubscriberManager.Get().delete(this.id__());
    }

    // get informed after the property has changed.
    // implements: SinglePropertyChangeSubscriber<T>
    hasChanged(newValue: T): void {
      console.log(`Class ${this.classInfo_} object inside property has changed. newValue ${newValue}.`)
    }

    // implements IMultiPropertiesChangePublisher
    propertyHasChanged(info?: PropertyInfo): void {
      console.log(`Class ${this.classInfo_} object property ${info ? info : 'unknown'} has changed.`);
    }
  }

  tcase("ObservedObject.IsObservedObject test", () => {

    test("array", !ObservedObject.IsObservedObject([1, 2, 3]));
    test("object", !ObservedObject.IsObservedObject({ a: 1 }));
    test("number", !ObservedObject.IsObservedObject(1));
    test("boolean", !ObservedObject.IsObservedObject(true));
    test("undefined", !ObservedObject.IsObservedObject(undefined));
    test("undefined", !ObservedObject.IsObservedObject(undefined));

    let sub1 = new ClassPropertyChangeSubscriber<number[]>("number[] #1 sub");
    let arr1 = ObservedObject.createNew([1, 2, 3], sub1);
    let sub2 = new ClassPropertyChangeSubscriber<number[]>("number[] #2 sub");
    let arr2 = ObservedObject.createNew({ a: 1 }, sub2);

    test("observed array", ObservedObject.IsObservedObject(arr1));
    test("observed object", ObservedObject.IsObservedObject(arr2));

    ObservedObject.removeOwningProperty(arr1, sub1);
    sub1.aboutToBeDeleted();

    ObservedObject.removeOwningProperty(arr2, sub2);
    sub2.aboutToBeDeleted();
  });


  tcase("create and read back", () => {
    let arr: number[] = [1, 2, 3, 4];
    AppStorage.SetOrCreate<number[]>("arr", arr);

    test("AppStrorage has initial one ObservedProperties", AppStorage.Size() == 1);
    test("Initial array value readback.", AppStorage.Get<number[]>("arr").toString() == arr.toString());
    test(`Initial array value readback, check Array.isArray`, Array.isArray(AppStorage.Get<number[]>("arr")));
    test(`Initial array value readback, check instance of ObservedObject`, ObservedObject.IsObservedObject(AppStorage.Get<number[]>("arr")));
  });

  tcase("push to array", () => {
    let arrSub = new ArrayChangeSubscriber();
    let spyArrayChange = spyOn(arrSub, "hasChanged");
    AppStorage.Get<number[]>("arr").push(5);
    test("Changed array value readback", AppStorage.Get<number[]>("arr").toString() == [1, 2, 3, 4, 5].toString());
    test(`hasChanged was called`, spyArrayChange.called);
    test(`hasChanged param is the correct changed array.`, spyArrayChange.args[0].toString() == [1, 2, 3, 4, 5].toString());
    test(`Initial array value readback, check instance of ObservedObject`, ObservedObject.IsObservedObject(AppStorage.Get<number[]>("arr")));
    arrSub.aboutToBeDeleted();
  });

  tcase("change property value gets observed #1", () => {
    let arrSub = new ArrayChangeSubscriber();
    let spyArrayChange = spyOn(arrSub, "hasChanged");

    AppStorage.Set<number[]>("arr", [2, 3, 4]);

    test("Changed array value readback", AppStorage.Get<number[]>("arr").toString() == [2, 3, 4].toString());
    test(`hasChanged was called`, spyArrayChange.called);
    test(`hasChanged param is the correct changed array.`, spyArrayChange.args[0].toString() == [2, 3, 4].toString());
    test(`Initial array value readback, check instance of ObservedObject`, ObservedObject.IsObservedObject(AppStorage.Get<number[]>("arr")));

    arrSub.aboutToBeDeleted();
  });


  tcase("change property value gets observed #2", () => {
    let arrSub = new ArrayChangeSubscriber();
    AppStorage.Set<number[]>("arr", [2, 3, 4]);
    let spyArrayChange = spyOn(arrSub, "hasChanged");
    AppStorage.Get<number[]>("arr")[1] = 7;

    test("Changed array value readback", AppStorage.Get<number[]>("arr").toString() == [2, 7, 4].toString());
    test(`hasChanged was called`, spyArrayChange.called);
    test(`hasChanged param is the correct changed array.`, spyArrayChange.args[0].toString() == [2, 7, 4].toString());
    arrSub.aboutToBeDeleted();
  });

  tcase("ObservedObject Array proxying is transparent", () => {
    AppStorage.Set<number[]>("arr", [2, 3, 4]);
    let link = AppStorage.Link<number[]>("arr");
    test("link readback value OK", link.get().toString() == [2, 3, 4].toString());
    test("link readback value OK", link.get().length == 3);
    test("Array.isArray(link) OK", Array.isArray(link.get()));
    test("link value is instance of ObservedObject OK", ObservedObject.IsObservedObject(link.get()));

    const rawArray = ObservedObject.GetRawObject(link.get());
    test("ObservedObject.GetRawObject value readback OK", rawArray.toString() == [2, 3, 4].toString());
    test("GetRawObject on link value returns a raw", !ObservedObject.IsObservedObject(rawArray));

    link.aboutToBeDeleted();
  });


  tcase("cleanup ok", () => {

    // manual cleanup to see everything works as expected
    const deleteOk = AppStorage.Delete("arr");
    test(`Deletion of props form AppStrorage without isuses`, deleteOk);
    test(`AppStrorage has ${AppStorage.Size()} ObservedProperties: >${Array.from(AppStorage.Keys())}<, should be none.`, AppStorage.Size() == 0);

    test(`SubscriberManager num of subscribers us ${SubscriberManager.Get().numberOfSubscrbers()} should be 0 .`, SubscriberManager.Get().numberOfSubscrbers() == 0);

    // this will do nothing because manual cleanup was done already
    AppStorage.AboutToBeDeleted();
  });
});
