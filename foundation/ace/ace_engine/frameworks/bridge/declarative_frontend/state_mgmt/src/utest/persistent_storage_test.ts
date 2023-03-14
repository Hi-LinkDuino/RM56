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

const testPersistentStorage = tsuite("PersistentStorage", () => {
  let ms = new MockStorage("./src/utest/resources/test.db");
  PersistentStorage.ConfigureBackend(ms);

  tcase("persisting existing values in AppStorage", () => {
    AppStorage.SetOrCreate<number>("existingPropToPersist", 123);
    PersistentStorage.PersistProp<number>("existingPropToPersist", 456);

    test("PersistentStorage 'existingPropToPersist' value retrieved from AppStorage correct", AppStorage.Get<number>("existingPropToPersist") == 123);
    test("PersistentStorage 'existingPropToPersist' value retrieved from MockStorage correct", ms.get("existingPropToPersist") == 123);
  });
  tcase("retrieval of values", () => {
    AppStorage.SetOrCreate<string>("persistentSay", "Terve");
    AppStorage.SetOrCreate<string>("persistentSubject", "Maailma");

    PersistentStorage.PersistProps([{
      key: "persistentSay",
      defaultValue: "Terve"
    }, {
      key: "persistentSubject",
      defaultValue: "Maailma"
    }]);

    test(`PersistentStorage 'persistentSay' value retrieved from AppStorage correct`, AppStorage.Get<string>("persistentSay") === "Terve");
    test("PersistentStorage 'persistentSubject' value retrieved from AppStorage correct", AppStorage.Get<string>("persistentSubject") == "Maailma");
  });

  tcase("writing values to IStorage via PersistentStorage", () => {
    PersistentStorage.PersistProps([{
      key: "writeToDb",
      defaultValue: ""
    }, {
      key: "numberToDb",
      defaultValue: 10
    }]);
    AppStorage.Set("writeToDb", "test");
    AppStorage.Set("numberToDb", 42);
    test("PersistentStorage 'persistentSay' value retrieved from AppStorage correct", AppStorage.Get<string>("persistentSay") == "Terve");
    test("PersistentStorage 'persistentSubject' value retrieved from AppStorage correct", AppStorage.Get<string>("persistentSubject") == "Maailma");
    test("PersistentStorage 'writeToDb' value retrieved from AppStorage correct", AppStorage.Get<string>("writeToDb") == "test");
    test("PersistentStorage 'numberToDb' value retrieved from AppStorage correct", AppStorage.Get<number>("numberToDb") == 42);

    test("PersistentStorage 'writeToDb' value retrieved from MockStorage correct", ms.get<string>("writeToDb") == "test");
    test("PersistentStorage 'numberToDb' value retrieved from MockStorage correct", ms.get("numberToDb") == 42);
  });

  tcase("deleting values from IStorage via PersistentStorage", () => {
    PersistentStorage.DeleteProp("writeToDb");
    PersistentStorage.DeleteProp("numberToDb");
    AppStorage.Delete("writeToDb");
    AppStorage.Delete("numberToDb");
    test("MockStorage 'writeToDb' value deleted", ms.get("writeToDb") == undefined);
    test("MockStorage 'numberToDb' value deleted", ms.get("numberToDb") == undefined);
  });

  tcase("Persisting new property that does not exist in db", () => {
    PersistentStorage.PersistProp("newProp", "new property value");

   test("PersistentStorage 'newProp' value retrieved from AppStorage correct", AppStorage.Get<string>("newProp") == "new property value");
   test("MockStorage 'newProp' value retrieved from MockStorage correct", ms.get("newProp") == "new property value");
  });

  tcase("keys retrieval", () => {
    const keys = PersistentStorage.Keys();
    for (var i in keys) {
      console.debug(`${keys[i]}`);
    }
  });

  tcase("teardown", () => {
    PersistentStorage.AboutToBeDeleted();
    const deleteOk = AppStorage.Delete("persistentSay") && AppStorage.Delete("persistentSubject") && AppStorage.Delete("newProp") && AppStorage.Delete("existingPropToPersist");

    SubscriberManager.Get().dumpSubscriberInfo();

    test("PersistentStorage teardown complete", deleteOk);
    test(`SubscriberManager num of subscribers, is ${SubscriberManager.Get().numberOfSubscrbers()} should be 0`, SubscriberManager.Get().numberOfSubscrbers() == 0);
  });
});
