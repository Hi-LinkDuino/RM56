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
import LightWeightMap from "@ohos.util.LightWeightMap";

describe("LightWeightMapTest", function () {
  it("SR00GGR3L_testConstructor001", 0, function () {
    try {
      let lightWeightMap = new LightWeightMap();
      expect(lightWeightMap != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new TreeMap");
    }
  });
  it("SR00GGR3L_testSet002", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    let res = lightWeightMap.hasValue("A");
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet003", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    let res = lightWeightMap.hasValue("A");
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey("a");
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet004", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let a = [1, 2, 3, 4];
    lightWeightMap.set(1, a);
    let res = lightWeightMap.hasValue(a);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet005", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let c = {name: "lili", age: "13"};
    lightWeightMap.set(1, c);
    let res = lightWeightMap.hasValue(c);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testGet006", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.get(4);
    expect(res).assertEqual("D");
  });
  it("SR00GGR3L_testLength007", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.length;
    expect(res).assertEqual(5);
  });
  it("SR00GGR3L_testHasAll008", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let lightWeightMap1 = new LightWeightMap();
    lightWeightMap1.set("a", "A");
    lightWeightMap1.set("d", "D");
    let res = lightWeightMap.hasAll(lightWeightMap1);
    expect(res).assertEqual(true);
  });
  it("SR00GGR3L_testHasKey009", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let res = lightWeightMap.hasKey("a");
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1);
    expect(res1).assertEqual(false);
  });
  it("SR00GGR3L_testHasValue010", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let res = lightWeightMap.hasValue("A");
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasValue(1);
    expect(res1).assertEqual(false);
  });
  it("SR00GGR3L_testIncreaseCapacityTo011", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    lightWeightMap.increaseCapacityTo(3);
    let length = lightWeightMap.length;
    expect(length).assertEqual(5);
  });
  it("SR00GGR3L_testEntries012", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.entries();
    expect(JSON.stringify(res.next().value)).assertEqual('[1,"A"]');
    expect(JSON.stringify(res.next().value)).assertEqual('[2,"B"]');
    expect(JSON.stringify(res.next().value)).assertEqual('[3,"C"]');
    expect(JSON.stringify(res.next().value)).assertEqual('[4,"D"]');
    expect(JSON.stringify(res.next().value)).assertEqual('[5,"E"]');
  });
  it("SR00GGR3L_testGetIndexOfKey013", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.getIndexOfKey(2);
    expect(res).assertEqual(1);
  });
  it("SR00GGR3L_testGetIndexOfValue014", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    lightWeightMap.setValueAt(4, "F");
    let res = lightWeightMap.getIndexOfValue("F");
    expect(res).assertEqual(4);
  });
  it("SR00GGR3L_testIsEmpty015", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let res1 = lightWeightMap.isEmpty();
    expect(res1).assertEqual(true);
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res2 = lightWeightMap.isEmpty();
    expect(res2).assertEqual(false);
  });
  it("SR00GGR3L_testGetKeyAt016", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.getKeyAt(1);
    expect(res).assertEqual(2);
  });
  it("SR00GGR3L_testKeys017", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.keys();
    expect(res.next().value).assertEqual(1);
    expect(res.next().value).assertEqual(2);
    expect(res.next().value).assertEqual(3);
    expect(res.next().value).assertEqual(4);
    expect(res.next().value).assertEqual(5);
  });
  it("SR00GGR3L_testSetAll018", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let lightWeightMap1 = new LightWeightMap();
    lightWeightMap1.set(6, "A");
    lightWeightMap1.set(7, "B");
    lightWeightMap.setAll(lightWeightMap1);
    for (let i = 1; i < 8; i++) {
      expect(lightWeightMap.hasKey(i)).assertEqual(true);
    }
  });
  it("SR00GGR3L_testRemove019", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.remove(3);
    expect(res).assertEqual("C");
    let res1 = lightWeightMap.hasValue("C");
    expect(res1).assertEqual(false);
  });
  it("SR00GGR3L_testRemoveAt020", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.removeAt(1);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasValue("A");
    expect(res1).assertEqual(true);
    let res2 = lightWeightMap.hasValue("B");
    expect(res2).assertEqual(false);
    let res3 = lightWeightMap.removeAt(10);
    expect(res3).assertEqual(false);
  });
  it("SR00GGR3L_testClear021", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    lightWeightMap.clear();
    let res = lightWeightMap.length;
    expect(res).assertEqual(0);
    let isEmpty = lightWeightMap.isEmpty();
    expect(isEmpty).assertEqual(true);
  });
  it("SR00GGR3L_testSetValueAt022", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    lightWeightMap.setValueAt(0, "a");
    let res = lightWeightMap.get(1);
    expect(res).assertEqual("a");
  });
  it("SR00GGR3L_testForEach023", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let arr = [];
    lightWeightMap.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = ["A", "B", "C", "D", "E"];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR00GGR3L_testToString024", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    let res = lightWeightMap.toString();
    expect(res).assertEqual("1:A,2:B,3:C");
  });
  it("SR00GGR3L_testValues025", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.values();
    expect(res.next().value).assertEqual("A");
    expect(res.next().value).assertEqual("B");
    expect(res.next().value).assertEqual("C");
    expect(res.next().value).assertEqual("D");
    expect(res.next().value).assertEqual("E");
  });
  it("SR00GGR3L_testGetValueAt026", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.getValueAt(1);
    expect(res).assertEqual("B");
  });
  it("SR00GGR3L_testIterator027", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let iters = lightWeightMap[Symbol.iterator]();
    let flag = true;
    for (let i = 0, len = lightWeightMap.length; i < len; i++) {
      let entry = iters.next().value;
      let res = lightWeightMap.get(entry[0]);
      if (res != entry[1]) {
        flag = false;
        break;
      }
    }
    expect(flag).assertEqual(true);
  });
  it("SR00GGR3L_testSet028", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, null);
    let res = lightWeightMap.hasValue(null);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet029", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(null, null);
    let res = lightWeightMap.hasValue(null);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(null);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet030", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(true, false);
    let res = lightWeightMap.hasValue(false);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(true);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet031", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1.23, 321);
    let res = lightWeightMap.hasValue(321);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(1.23);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testHasAll032", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let lightWeightMap1 = new LightWeightMap();
    lightWeightMap1.set("a1", "A1");
    lightWeightMap1.set("d1", "D1");
    let res = lightWeightMap.hasAll(lightWeightMap1);
    expect(res).assertEqual(false);
  });
  it("SR00GGR3L_testHasAll033", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let lightWeightMap1 = new LightWeightMap();
    lightWeightMap1.set("a", "A1");
    lightWeightMap1.set("d", "D1");
    let res = lightWeightMap.hasAll(lightWeightMap1);
    expect(res).assertEqual(false);
  });
  it("SR00GGR3L_testRemove034", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let res = lightWeightMap.remove(3);
    expect(res).assertEqual(undefined);
  });
  it("SR00GGR3L_testRemoveAt035", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let res = lightWeightMap.removeAt(1);
    expect(res).assertEqual(false);
  });
  it("SR00GGR3L_testIncreaseCapacityTo036", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let res = lightWeightMap.increaseCapacityTo(10);
    expect(res).assertEqual(undefined);
  });
  it("SR00GGR3L_testIncreaseCapacityTo037", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    try {
      lightWeightMap.increaseCapacityTo("qwe");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the size is not integer");
    }
  });
  it("SR00GGR3L_testRemoveAt038", 0, function () {
    let lightWeightMap = new LightWeightMap();
    try {
      let res = lightWeightMap.removeAt("123");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the size is not integer");
    }
  });
  it("SR00GGR3L_testGetValueAt039", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    try {
      let res = lightWeightMap.getValueAt("123");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR00GGR3L_testGetKeyAt040", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    try {
      let res = lightWeightMap.getKeyAt("123");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR00GGR3L_testHasAll041", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let lightWeightMap1 = new LightWeightMap();
    let res = lightWeightMap.hasAll(lightWeightMap1);
    expect(res).assertEqual(true);
  });
  it("SR00GGR3L_testSet042", 0, function () {
    let lightWeightMap = new LightWeightMap();
    for (let i = 0; i < 100; i++) {
      lightWeightMap.set(i, "A");
    }
    let res = lightWeightMap.get(99);
    expect(res).assertEqual("A");
    let res1 = lightWeightMap.length;
    expect(res1).assertEqual(100);
  });
  it("SR00GGR3L_testSet043", 0, function () {
    let lightWeightMap = new LightWeightMap();
    for (let i = 0; i < 100; i++) {
      lightWeightMap.set(1, i);
    }
    let res = lightWeightMap.get(1);
    expect(res).assertEqual(99);
    let res1 = lightWeightMap.length;
    expect(res1).assertEqual(1);
  });
  it("SR00GGR3L_testSet044", 0, function () {
    let lightWeightMap = new LightWeightMap();
    for (let i = 0; i < 10000; i++) {
      lightWeightMap.set(i, i);
    }
    let res1 = lightWeightMap.length;
    expect(res1).assertEqual(10000);
    for (let index = 0; index < 10000; index++) {
      let resKey = lightWeightMap.hasKey(index);
      expect(resKey).assertEqual(true);
      let resValue = lightWeightMap.hasValue(index);
      expect(resValue).assertEqual(true);
    }
  });
  it("SR00GGR3L_testSet045", 0, function () {
    let lightWeightMap = new LightWeightMap();
    for (let i = 0; i < 5000; i++) {
      lightWeightMap.set(i, i);
    }
    for (let i = -1; i > -5001; i--) {
      lightWeightMap.set(i, i);
    }
    let res1 = lightWeightMap.length;
    expect(res1).assertEqual(10000);
    for (let index = 0; index < 5000; index++) {
      let resKey = lightWeightMap.hasKey(index);
      expect(resKey).assertEqual(true);
      let resValue = lightWeightMap.hasValue(index);
      expect(resValue).assertEqual(true);
    }
    for (let i = -1; i > -5001; i--) {
      let resKey = lightWeightMap.hasKey(i);
      expect(resKey).assertEqual(true);
      let resValue = lightWeightMap.hasValue(i);
      expect(resValue).assertEqual(true);
    }
  });
  it("SR00GGR3L_testSet046", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(true, 0.001);
    let res = lightWeightMap.hasValue(0.001);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(true);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet047", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let a = [1, 2, 3, 4];
    lightWeightMap.set(a, 1);
    let res = lightWeightMap.hasValue(1);
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(a);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testSet048", 0, function () {
    let lightWeightMap = new LightWeightMap();
    let a = {abc: 1};
    lightWeightMap.set(a, "");
    let res = lightWeightMap.hasValue("");
    expect(res).assertEqual(true);
    let res1 = lightWeightMap.hasKey(a);
    expect(res1).assertEqual(true);
  });
  it("SR00GGR3L_testGet049", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set(1, "A");
    lightWeightMap.set(2, "B");
    lightWeightMap.set(3, "C");
    lightWeightMap.set(4, "D");
    lightWeightMap.set(5, "E");
    let res = lightWeightMap.get(10);
    expect(res).assertEqual(undefined);
  });
  it("SR00GGR3L_testHasAll050", 0, function () {
    let lightWeightMap = new LightWeightMap();
    lightWeightMap.set("a", "A");
    lightWeightMap.set("b", "B");
    lightWeightMap.set("c", "C");
    lightWeightMap.set("d", "D");
    lightWeightMap.set("e", "E");
    let lightWeightMap1 = new LightWeightMap();
    lightWeightMap1.set("a", "A");
    lightWeightMap1.set("d", "D1");
    let res = lightWeightMap.hasAll(lightWeightMap1);
    expect(res).assertEqual(false);
  });
});
