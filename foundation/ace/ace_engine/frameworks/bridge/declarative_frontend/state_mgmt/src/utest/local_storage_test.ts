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


const testLocalStorage = tsuite("LocalStorage Tests", () => {

  
    class TestSubscriber<C> implements ISinglePropertyChangeSubscriber<C>  {

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
      hasChanged(newValue: C): void {
        console.log(`TestSubscriber '${this.label_}' property changed value (shown stringified): '${ JSON.stringify(newValue) }' `);
      }
    }
  

  let storage: LocalStorage;

  tcase("size", () => {
    storage = LocalStorageLookup.GetOrCreate("utest/local_storage_test.ts", { say: "Hello", name: "Gudio" });
    test("LocalStorage has initial two ObservedProperties", storage.size() == 2);
  });

  tcase("LocalStorage has been init with value", () => {
    test("added 'say'is in LocalStorage", storage.has("say"));
    test("added 'name'is in LocalStorage", storage.has("name"));

    test("added 'say' correct value", storage.get<string>("say") == "Hello");
    test("added 'name' wrong value", storage.get<string>("name") == "Gudio");
  });

  tcase("LocalStorage remembers self set boolean value", () => {
    storage.setOrCreate<boolean>("aBool", true);
    test("added 'aBool' correct boolean value", storage.get<boolean>("aBool"));
  });

  tcase("LocalStorage remembers self set number value", () => {
    storage.setOrCreate<number>("favorite", 47);

    test("added 'favorite', has correct number value", storage.get<number>("favorite") == 47);
    test("added 'favorite' converted to string is '47'", storage.get<string>("favorite") == "47");
  });

  let boolChangeListener = new TestSubscriber<boolean>("boolChangeListener");
  storage.subscribeToChangesOf("aBool", boolChangeListener);
  let childboolChangeListenerSpy = spyOn(boolChangeListener, "hasChanged");
  
  tcase("Notifies listener on property value change", () => {
  storage.set<boolean>("aBool", false);
    test("boolean value readback correct", storage.get<boolean>("aBool") == false);
    test("subscribing listener's 'hasChanged' called, param is newValue", childboolChangeListenerSpy.called && childboolChangeListenerSpy.args[0] == false);
  });


  class TestAClass {
    public a: number;
    public b: number;
  }

  let objAClass1: TestAClass = { a: 1, b: 2 };
  let objAClass2: TestAClass = { a: 101, b: 201 };

  tcase("ObservedObject create, get, set", () => {
    storage.setOrCreate<TestAClass>("objAClass", objAClass1);
    test("ObservedObject create, value read back prop 'a'", (storage.get<TestAClass>("objAClass").a == 1 && storage.get<TestAClass>("objAClass").b == 2));

    storage.get<TestAClass>("objAClass").a = 47;
    test("ObservedObject property value change, value read back", storage.get<TestAClass>("objAClass").a == 47);

    storage.set<TestAClass>("objAClass", objAClass2);
    test("ObservedProperty of type ObservedObject set new ObservedObject, value read back",
      storage.get<TestAClass>("objAClass").a == 101 && storage.get<TestAClass>("objAClass").b == 201);

    storage.get<TestAClass>("objAClass").a = 102;
    test("Followed by prop value change, value read back",
      storage.get<TestAClass>("objAClass").a == 102 && storage.get<TestAClass>("objAClass").b == 201);
  })

  let objAChangeListener = new TestSubscriber<TestAClass>("objAChangeListener");
  storage.subscribeToChangesOf("objAClass", objAChangeListener);
  let objAChangeListenerSpy1 = spyOn(objAChangeListener, "hasChanged");
  let objAClass3: TestAClass = { a: 101, b: 102 };


  tcase("Notifies listener on 'objAClass' property value change", () => {
    storage.set<TestAClass>("objAClass", objAClass3);
    test("ObservedObject property value change, value read back", storage.get<TestAClass>("objAClass").a == 101);
    test("subscribing listener's 'hasChanged' called with correct param", objAChangeListenerSpy1.called && objAChangeListenerSpy1.args[0].b == 102);
  });

  let objAChangeListenerSpy2 = spyOn(objAChangeListener, "hasChanged");

  tcase("Notifies listener on 'objAClass' property's property value change", () => {
    storage.get<TestAClass>("objAClass").b = 103;
    test("ObservedObject property value change, value read back", storage.get<TestAClass>("objAClass").b == 103);
    test("subscribing listener's 'hasChanged' called with correct param", objAChangeListenerSpy2.called && objAChangeListenerSpy2.args[0].b == 103);
  });

  tcase("cleanup ok", () => {
    storage.unsubscribeFromChangesOf("aBool", boolChangeListener.id__());
    boolChangeListener.aboutToBeDeleted();

    storage.unsubscribeFromChangesOf("objAClass", objAChangeListener.id__());
    objAChangeListener.aboutToBeDeleted();

    const deleteOk = storage.delete("name") && storage.delete("say") && storage.delete("aBool") && storage.delete("favorite") && storage.delete("objAClass");
    test(`Deletion of props form AppStrorage without isuses`, deleteOk)

    // test that manual cleanup has been complete, before calling storage.boutToBeDeleted();
    test(`AppStrorage has ${storage.size()} ObservedProperties: >${Array.from(storage.keys())}<, should be none.`, storage.size() == 0);

    storage.aboutToBeDeleted();

    test(`SubscriberManager num of subscribers is ${SubscriberManager.Get().numberOfSubscrbers()} should be 0 .`, SubscriberManager.Get().numberOfSubscrbers() == 0);
  });

});
