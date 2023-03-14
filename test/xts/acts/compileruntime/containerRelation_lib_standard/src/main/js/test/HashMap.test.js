/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License')
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index';
import HashMap from "@ohos.util.HashMap";

describe("HashMapTest", function () {
  it("SR000GGR4B_testConstructor001", 0, function () {
    try {
      let hashMap = new HashMap();
      expect(hashMap != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new HashMap");
    }
  });
  it("SR000GGR4B_testSet002", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    let res = hashMap.get(1);
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet003", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, 2);
    let res = hashMap.get(1);
    expect(res).assertEqual(2);
  });
  it("SR000GGR4B_testSet004", 0, function () {
    let hashMap = new HashMap();
    let c = [1, 2, 3];
    hashMap.set(1, c);
    let res = hashMap.get(1);
    expect(res).assertEqual(c);
  });
  it("SR000GGR4B_testSet005", 0, function () {
    let hashMap = new HashMap();
    let c = {name: "lili", age: "13"};
    hashMap.set(1, c);
    let res = hashMap.get(1);
    expect(res).assertEqual(c);
  });
  it("SR000GGR4B_testSet006", 0, function () {
    let hashMap = new HashMap();
    hashMap.set("a", "A");
    let res = hashMap.get("a");
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet007", 0, function () {
    let hashMap = new HashMap();
    hashMap.set("", "A");
    let res = hashMap.get("");
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet008", 0, function () {
    let hashMap = new HashMap();
    hashMap.set("$", "A");
    let res = hashMap.get("$");
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet009", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(3.14, "A");
    let res = hashMap.get(3.14);
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet010", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(0.3, "A");
    let res = hashMap.get(0.3);
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet011", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(-1, "A");
    let res = hashMap.get(-1);
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet012", 0, function () {
    let hashMap = new HashMap();
    let a = {};
    hashMap.set(a, "A");
    let res = hashMap.get(a);
    expect(res).assertEqual("A");
  });
  it("SR000GGR4B_testSet013", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, null);
    let res = hashMap.get(1);
    expect(res).assertEqual(null);
  });
  it("SR000GGR4B_testSet014", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "$");
    let res = hashMap.get(1);
    expect(res).assertEqual("$");
  });
  it("SR000GGR4B_testSet015", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, 3.14);
    let res = hashMap.get(1);
    expect(res).assertEqual(3.14);
  });
  it("SR000GGR4B_testSet016", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, -1);
    let res = hashMap.get(1);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR4B_testSet017", 0, function () {
    let hashMap = new HashMap();
    let a = {};
    hashMap.set(1, a);
    let res = hashMap.get(1);
    expect(res).assertEqual(a);
  });
  it("SR000GGR4B_testSet018", 0, function () {
    let hashMap = new HashMap();
    for (let i = 0; i < 100; i++) {
      hashMap.set(1, i);
    }
    let res = hashMap.get(1);
    let res1 = hashMap.length;
    expect(res).assertEqual(99);
    expect(res1).assertEqual(1);
  });
  it("SR000GGR4B_testSet019", 0, function () {
    let hashMap = new HashMap();
    for (let i = 0; i < 100; i++) {
      hashMap.set(i, 1);
      let res = hashMap.get(i);
      expect(res).assertEqual(1);
    }
    let res1 = hashMap.length;
    expect(res1).assertEqual(100);
  });
  it("SR000GGR4B_testLength020", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR4B_testHasKey021", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.hasKey(3);
    expect(res).assertEqual(true);
    let res1 = hashMap.hasKey(8);
    expect(res1).assertEqual(false);
  });
  it("SR000GGR4B_testHasKey022", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.hasKey(8);
    expect(res).assertEqual(false);
  });
  it("SR000GGR4B_testHasValue023", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.hasValue("C");
    expect(res).assertEqual(true);
    let res1 = hashMap.hasValue(8);
    expect(res1).assertEqual(false);
  });
  it("SR000GGR4B_testHasValue024", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.hasValue(8);
    expect(res).assertEqual(false);
  });
  it("SR000GGR4B_testGet025", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.get(3);
    expect(res).assertEqual("C");
  });
  it("SR000GGR4B_testGet026", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.get(3);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4B_testSetAll027", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let hashMap1 = new HashMap();
    hashMap1.set(1, 1);
    hashMap1.set(2, 2);
    hashMap.setAll(hashMap1);
    let arr = [];
    for (let [key, value] of hashMap) {
      arr.push([key, value]);
    }
    arr.sort(function (a, b) {
      return a[0] - b[0];
    });

    for (let i = 0; i < arr.length; i++) {
      expect(arr[i][0]).assertEqual(i + 1);
      expect(arr[i][1]).assertEqual(hashMap.get(i + 1));
    }
  });
  it("SR000GGR4B_testSetAll028", 0, function () {
    let hashMap = new HashMap();
    let hashMap1 = new HashMap();
    hashMap1.set(1, 1);
    hashMap1.set(2, 2);
    hashMap.setAll(hashMap1);
    let arr = [];
    for (let [key, value] of hashMap) {
      arr.push([key, value]);
    }
    arr.sort(function (a, b) {
      return a[0] - b[0];
    });
    for (let i = 0; i < arr.length; i++) {
      expect(JSON.stringify(arr[i])).assertEqual(
        JSON.stringify([i + 1, hashMap.get(i + 1)])
      );
    }
  });
  it("SR000GGR4B_testRemove029", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.remove(3);
    expect(res).assertEqual("C");
    let res1 = hashMap.length;
    expect(res1).assertEqual(4);
  });
  it("SR000GGR4B_testRemove030", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.remove(3);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4B_testClear031", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.length;
    expect(res).assertEqual(5);
    hashMap.clear();
    let res1 = hashMap.length;
    expect(res1).assertEqual(0);
  });
  it("SR000GGR4B_testClear032", 0, function () {
    let hashMap = new HashMap();
    hashMap.clear();
    let res = hashMap.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR4B_testKeys033", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.keys();
    for (let i = 0; i < hashMap.length; i++) {
      console.log(res.next());
    }
  });
  it("SR000GGR4B_testValues034", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.values();
    for (let i = 0; i < hashMap.length; i++) {
      let has = hashMap.hasValue(res.next().value);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR4B_testReplace035", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.replace(2, "G");
    expect(res).assertEqual(true);
    expect(hashMap.get(2)).assertEqual("G");
  });
  it("SR000GGR4B_testReplace036", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.replace(2, "G");
    expect(res).assertEqual(false);
  });
  it("SR000GGR4B_testForEach037", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let vals = [];
    let keys = [];
    hashMap.forEach((value, key) => {
      keys.push(key);
      vals.push(value);
    });
    for (let i = 0; i < keys.length; i++) {
      let has = hashMap.hasKey(keys[i]);
      expect(has).assertEqual(true);
    }
    for (let i = 0; i < vals.length; i++) {
      let has = hashMap.hasValue(vals[i]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR4B_testForEach038", 0, function () {
    let hashMap = new HashMap();
    let arr = [];
    hashMap.forEach((item, index) => {
      arr.push(item);
    });
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR4B_testIterator039", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let arr = [];
    for (let [key, value] of hashMap) {
      arr.push([key, value]);
    }
    arr.sort(function (a, b) {
      return a[0] - b[0];
    });
    for (let i = 0; i < arr.length; i++) {
      expect(arr[i][0]).assertEqual(i + 1);
      expect(arr[i][1]).assertEqual(hashMap.get(i + 1));
    }
  });
  it("SR000GGR4B_testIterator040", 0, function () {
    let hashMap = new HashMap();
    let arr = [];
    for (let item of hashMap) {
      arr.push(item);
    }
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR4B_testEntries041", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.entries();
    let arr = [];
    for (let i = 0; i < hashMap.length; i++) {
      let obj = res.next();
      arr.push([obj.value[0], obj.value[1]]);
    }
    arr.sort(function (a, b) {
      return a[0] - b[0];
    });
    for (let i = 0; i < arr.length; i++) {
      expect(arr[i][0]).assertEqual(i + 1);
      expect(arr[i][1]).assertEqual(hashMap.get(i + 1));
    }
  });
  it("SR000GGR4B_testEntries042", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.entries();
    expect(undefined).assertEqual(res.next().value);
  });
  it("SR000GGR4B_testIsEmpty043", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let res = hashMap.length;
    let res1 = hashMap.isEmpty();
    expect(res).assertEqual(5);
    expect(res1).assertEqual(false);
  });
  it("SR000GGR4B_testIsEmpty044", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    hashMap.clear();
    let res = hashMap.length;
    let res1 = hashMap.isEmpty();
    expect(res).assertEqual(0);
    expect(res1).assertEqual(true);
  });
  it("SR000GGR4B_testEntries045", 0, function () {
    let hashMap = new HashMap();
    let res = hashMap.entries();
    expect(undefined).assertEqual(res.next().value);
  });
  it("SR000GGR4B_testIterator046", 0, function () {
    let hashMap = new HashMap();
    hashMap.set(1, "A");
    hashMap.set(2, "B");
    hashMap.set(3, "C");
    hashMap.set(4, "D");
    hashMap.set(5, "E");
    let arr = [];
    let iters = hashMap[Symbol.iterator]();
    let flag = true;
    for (let i = 0, len = hashMap.length; i < len; i++) {
      let entry = iters.next().value;
      let res = hashMap.get(entry[0]);
      if (res != entry[1]) {
        flag = false;
        break;
      }
    }
    expect(flag).assertEqual(true);
  });
});
