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
import LightWeightSet from "@ohos.util.LightWeightSet";

describe("LightWeightSetTest", function () {
  it("SR000GGR43_testNew001", 0, function () {
    try {
      let lightWeightSet = new LightWeightSet();
      expect(lightWeightSet != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new TreeMap");
    }
  });
  it("SR000GGR43_testAdd002", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    let res = lightWeightSet.has(1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd003", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("a");
    let res = lightWeightSet.has("a");
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd004", 0, function () {
    let lightWeightSet = new LightWeightSet();
    let a = [1, 2, 3, 4];
    lightWeightSet.add(a);
    let res = lightWeightSet.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd005", 0, function () {
    let lightWeightSet = new LightWeightSet();
    let c = {name: "lili", age: "13"};
    lightWeightSet.add(c);
    let res = lightWeightSet.has(c);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd006", 0, function () {
    let lightWeightSet = new LightWeightSet();
    let c = false;
    lightWeightSet.add(c);
    let res = lightWeightSet.has(c);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testLength007", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR43_testAddAll008", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let lightWeightSet1 = new LightWeightSet();
    lightWeightSet1.add(6);
    lightWeightSet1.add(7);
    let res1 = lightWeightSet.addAll(lightWeightSet1);
    for (let item of lightWeightSet) {
      console.log("lightWeightSet-----" + item);
    }
    for (let item of lightWeightSet1) {
      console.log("lightWeightSet1-----" + item);
    }
    for (let i = 1; i < 8; i++) {
      expect(lightWeightSet.has(i)).assertEqual(true);
    }
    expect(res1).assertEqual(true);
  });
  it("SR000GGR43_testHasAll009", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("a");
    lightWeightSet.add("b");
    lightWeightSet.add("c");
    lightWeightSet.add("d");
    lightWeightSet.add("e");
    let lightWeightSet1 = new LightWeightSet();
    lightWeightSet1.add("a");
    lightWeightSet1.add("d");
    let res = lightWeightSet.hasAll(lightWeightSet1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testHasAllr010", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("a");
    lightWeightSet.add("b");
    lightWeightSet.add("c");
    lightWeightSet.add("e");
    let lightWeightSet1 = new LightWeightSet();
    lightWeightSet1.add("a");
    lightWeightSet1.add("d");
    let res = lightWeightSet.hasAll(lightWeightSet1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR43_testHas011", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("a");
    lightWeightSet.add("b");
    lightWeightSet.add("c");
    lightWeightSet.add("d");
    lightWeightSet.add("e");
    let res = lightWeightSet.has("a");
    expect(res).assertEqual(true);
    let res1 = lightWeightSet.has(1);
    expect(res1).assertEqual(false);
  });
  it("SR000GGR43_testEntries012", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.entries();
    expect(JSON.stringify(res.next().value)).assertEqual("[1,1]");
    expect(JSON.stringify(res.next().value)).assertEqual("[2,2]");
    expect(JSON.stringify(res.next().value)).assertEqual("[3,3]");
    expect(JSON.stringify(res.next().value)).assertEqual("[4,4]");
    expect(JSON.stringify(res.next().value)).assertEqual("[5,5]");
  });
  it("SR000GGR43_testGetIndexOf013", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.getIndexOf(2);
    expect(res).assertEqual(1);
  });
  it("SR000GGR43_testIsEmpty014", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR43_testRemove015", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.remove(1);
    expect(res).assertEqual(1);
    let arr = [];
    lightWeightSet.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = [2, 3, 4, 5];
    for (let i = 0; i < arr.length; i++) {
      expect(arr1[i]).assertEqual(arr[i]);
    }
  });
  it("SR000GGR43_testRemoveAt016", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.removeAt(1);
    expect(res).assertEqual(true);
    let arr = [];
    lightWeightSet.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = [1, 3, 4, 5];
    for (let i = 0; i < arr.length; i++) {
      expect(arr1[i]).assertEqual(arr[i]);
    }
  });
  it("SR000GGR43_testClear017", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    lightWeightSet.clear();
    let res = lightWeightSet.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR43_testForEach018", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let arr = [];
    lightWeightSet.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = [1, 2, 3, 4, 5];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR43_testToString019", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    let res = lightWeightSet.toString();
    expect(res).assertEqual("1,2,3");
  });
  it("SR000GGR43_testToArray020", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.toArray();
    let arr1 = [1, 2, 3, 4, 5];
    for (let i = 0; i < arr1.length; i++) {
      expect(res[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR43_testGetValueAt021", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.getValueAt(1);
    expect(res).assertEqual(2);
  });
  it("SR000GGR43_testIterator022", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let arr = [];
    for (let item of lightWeightSet) {
      let res = arr.push(item);
    }
    let arr1 = [1, 2, 3, 4, 5];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR43_testValues023", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.values();
    expect(res.next().value).assertEqual(1);
    expect(res.next().value).assertEqual(2);
    expect(res.next().value).assertEqual(3);
    expect(res.next().value).assertEqual(4);
    expect(res.next().value).assertEqual(5);
  });
  it("SR000GGR43_testAdd024", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(null);
    let res = lightWeightSet.has(null);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd025", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1.23);
    let res1 = lightWeightSet.has(1.23);
    expect(res1).assertEqual(true);
  });
  it("SR000GGR43_testHasAll026", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let lightWeightSet1 = new LightWeightSet();
    lightWeightSet1.add("a1");
    lightWeightSet1.add("d1");
    let res = lightWeightSet.hasAll(lightWeightSet1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR43_testHasAll027", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let lightWeightSet1 = new LightWeightSet();
    lightWeightSet1.add(1);
    lightWeightSet1.add("d");
    let res = lightWeightSet.hasAll(lightWeightSet1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR43_testRemove028", 0, function () {
    let lightWeightSet = new LightWeightSet();
    let res = lightWeightSet.remove(3);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR43_testRemoveAt029", 0, function () {
    let lightWeightSet = new LightWeightSet();
    let res = lightWeightSet.removeAt(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR43_testIncreaseCapacityTo030", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    try {
      lightWeightSet.increaseCapacityTo(3);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR43_testIncreaseCapacityTo031", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    try {
      let res = lightWeightSet.increaseCapacityTo("qwe");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR43_testRemoveAt032", 0, function () {
    let lightWeightSet = new LightWeightSet();
    try {
      let res = lightWeightSet.removeAt("123");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR43_testIncreaseCapacityTo033", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.increaseCapacityTo(20);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR43_testGetValueAt034", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    try {
      let res = lightWeightSet.getValueAt("123");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR43_testIsEmpty035", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    lightWeightSet.clear();
    let res = lightWeightSet.isEmpty();
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd036", 0, function () {
    let lightWeightSet = new LightWeightSet();
    for (let i = 0; i < 10000; i++) {
      lightWeightSet.add(i);
    }
    let res = lightWeightSet.getValueAt(9999);
    let res1 = lightWeightSet.length;
    expect(res).assertEqual(9999);
    expect(res1).assertEqual(10000);
  });
  it("SR000GGR43_testAdd037", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("");
    let res = lightWeightSet.has("");
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testAdd038", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add("$");
    let res = lightWeightSet.has("$");
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testClear039", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let res = lightWeightSet.length;
    lightWeightSet.clear();
    let res1 = lightWeightSet.length;
    expect(res).assertEqual(5);
    expect(res1).assertEqual(0);
  });
  it("SR000GGR43_testRemove040", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    try {
      let res = lightWeightSet.remove("A");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR43_testIterator41", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    lightWeightSet.add(4);
    lightWeightSet.add(5);
    let arr = [];
    let res = lightWeightSet[Symbol.iterator]();
    let temp = undefined;
    do {
      temp = res.next().value;
      arr.push(temp);
    } while (temp != undefined);
    let arr1 = [1, 2, 3, 4, 5];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR43_testEqual42", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    let obj = [1,2,3];
    let res = lightWeightSet.equal(obj);
    expect(res).assertEqual(true);
  });
  it("SR000GGR43_testEqual43", 0, function () {
    let lightWeightSet = new LightWeightSet();
    lightWeightSet.add(1);
    lightWeightSet.add(2);
    lightWeightSet.add(3);
    let obj = [1,2];
    let res = lightWeightSet.equal(obj);
    expect(res).assertEqual(false);
  });
});
