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
import HashSet from "@ohos.util.HashSet";

describe("HashSetTest", function () {
  it("SR000GGR3J_testNew001", 0, function () {
    try {
      let hashSet = new HashSet();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new HashSet");
    }
  });
  it("SR000GGR3J_testAdd001", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    let res = hashSet.has(1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd002", 0, function () {
    let hashSet = new HashSet();
    hashSet.add("b");
    hashSet.add("c");
    hashSet.add("d");
    hashSet.add("a");
    hashSet.add("g");
    let res = hashSet.has("a");
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd004", 0, function () {
    let hashSet = new HashSet();
    let c = [1, 2, 3];
    hashSet.add(c);
    let res = hashSet.has(c);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd005", 0, function () {
    let hashSet = new HashSet();
    let c = {name: "lili", age: "13"};
    hashSet.add(c);
    let res = hashSet.has(c);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testLength006", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(2);
    hashSet.add(3);
    hashSet.add(4);
    hashSet.add(5);
    let res = hashSet.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR3J_testHas007", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(4);
    hashSet.add(1);
    hashSet.add(3);
    hashSet.add(2);
    hashSet.add(5);
    let res = hashSet.has(1);
    expect(res).assertEqual(true);
    let res1 = hashSet.has("A");
    expect(res1).assertEqual(false);
  });
  it("SR000GGR3J_testIsEmpty008", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(4);
    hashSet.add(1);
    hashSet.add(3);
    hashSet.add(2);
    hashSet.add(5);
    let res = hashSet.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR3J_testRemove009", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(4);
    hashSet.add(1);
    hashSet.add(3);
    hashSet.add(2);
    hashSet.add(5);
    let res = hashSet.remove(1);
    expect(res).assertEqual(true);
    let arr = [];
    hashSet.forEach((value, index) => {
      arr.push(value);
    });
    for (let i = 0; i < arr.length; i++) {
      let has = hashSet.has(arr[i]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testClear010", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(4);
    hashSet.add(1);
    hashSet.add(3);
    hashSet.add(2);
    hashSet.add(5);
    let res = hashSet.length;
    expect(res).assertEqual(5);
    hashSet.clear();
    let res1 = hashSet.length;
    expect(res1).assertEqual(0);
  });
  it("SR000GGR3J_testValues011", 0, function () {
    let hashSet = new HashSet();
    hashSet.add("A");
    hashSet.add("B");
    hashSet.add("C");
    hashSet.add("D");
    hashSet.add("E");
    let res = hashSet.values();
    for (let i = 0; i < hashSet.length; i++) {
      let has = hashSet.has(res.next().value);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testForEach012", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(2);
    hashSet.add(3);
    hashSet.add(4);
    hashSet.add(5);
    let arr = [];
    hashSet.forEach((value, index) => {
      arr.push(value);
    });
    for (let i = 0; i < arr.length; i++) {
      let has = hashSet.has(arr[i]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testIterator013", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(2);
    hashSet.add(3);
    hashSet.add(4);
    hashSet.add(5);
    let arr = [];
    for (let item of hashSet) {
      arr.push(item);
    }
    for (let i = 0; i < arr.length; i++) {
      let has = hashSet.has(arr[i]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testIterator014", 0, function () {
    let hashSet = new HashSet();
    hashSet.add("A");
    hashSet.add("B");
    hashSet.add("C");
    hashSet.add("D");
    hashSet.add("E");
    let arr = [];
    for (let item of hashSet) {
      arr.push(item);
    }
    for (let i = 0; i < arr.length; i++) {
      let has = hashSet.has(arr[i]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testEntries015", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(2);
    hashSet.add(3);
    hashSet.add(4);
    hashSet.add(5);
    let res = hashSet.entries();
    for (let i = 0; i < hashSet.length; i++) {
      let has = hashSet.has(res.next().value[1]);
      expect(has).assertEqual(true);
    }
  });
  it("SR000GGR3J_testAdd016", 0, function () {
    let hashSet = new HashSet();
    hashSet.add("");
    let res = hashSet.has("");
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd017", 0, function () {
    let hashSet = new HashSet();
    hashSet.add("$");
    let res = hashSet.has("$");
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd017", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1.34);
    let res = hashSet.has(1.34);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd018", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(-1);
    let res = hashSet.has(-1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd019", 0, function () {
    let hashSet = new HashSet();
    let a = {};
    hashSet.add(a);
    let res = hashSet.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testAdd020", 0, function () {
    let hashSet = new HashSet();
    for (let i = 0; i < 100; i++) {
      hashSet.add(i);
      let res = hashSet.has(i);
      expect(res).assertEqual(true);
    }
    let res1 = hashSet.length;
    expect(res1).assertEqual(100);
  });
  it("SR000GGR3J_testIterator021", 0, function () {
    let hashSet = new HashSet();
    let i = 0;
    for (let item of hashSet) {
      expect(item).assertEqual(i);
      i++;
    }
  });
  it("SR000GGR3J_testForEach022", 0, function () {
    let hashSet = new HashSet();
    let arr = [];
    hashSet.forEach((value, index) => {
      arr.push(value);
    });
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR3J_testIsEmpty023", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(4);
    hashSet.add(1);
    hashSet.add(3);
    hashSet.add(2);
    hashSet.add(5);
    hashSet.clear();
    let res = hashSet.isEmpty();
    expect(res).assertEqual(true);
  });
  it("SR000GGR3J_testHas024", 0, function () {
    let hashSet = new HashSet();
    let res = hashSet.has(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR3J_testRemove025", 0, function () {
    let hashSet = new HashSet();
    let res = hashSet.remove(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR3J_testClear026", 0, function () {
    let hashSet = new HashSet();
    let res = hashSet.clear();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3J_testEntries027", 0, function () {
    let hashSet = new HashSet();
    let res = hashSet.entries();
    expect(res.next().value).assertEqual(undefined);
  });
  it("SR000GGR3J_testIterator028", 0, function () {
    let hashSet = new HashSet();
    for (let i = 0; i < 100; i++) {
      let text = hashSet.add(i);
    }
    let arr = [];
    for (let item of hashSet) {
      arr.push(Number.parseInt(item));
    }
    for (let i = 0; i < 100; i++) {
      let a = arr[i];
      let res = hashSet.has(a);
      expect(res).assertEqual(true);
    }
  });
  it("SR000GGR3J_testForEach029", 0, function () {
    let hashSet = new HashSet();
    for (let i = 0; i < 100; i++) {
      hashSet.add(i);
    }
    let arr = [];
    hashSet.forEach((value, index) => {
      arr.push(value);
    });
    for (let i = 0; i < 100; i++) {
      let a = arr[i];
      let res = hashSet.has(a);
      expect(res).assertEqual(true);
    }
  });
  it("SR000GGR3J_testAdd030", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(1);
    let has = hashSet.has(1);
    let size = hashSet.length;
    expect(has).assertEqual(true);
    expect(size).assertEqual(1);
  });
  it("SR000GGR3J_testIterator031", 0, function () {
    let hashSet = new HashSet();
    hashSet.add(1);
    hashSet.add(2);
    hashSet.add(3);
    hashSet.add(4);
    hashSet.add(5);
    let res = hashSet[Symbol.iterator]();
    let temp = undefined;
    let arr = [];
    do {
      temp = res.next().value;
      arr.push(temp);
    } while (temp != undefined);
    arr.sort(function (a, b) {
      return a - b;
    });
    let arr1 = [1, 2, 3, 4, 5];
    for (let i = 0; i < arr.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
});
