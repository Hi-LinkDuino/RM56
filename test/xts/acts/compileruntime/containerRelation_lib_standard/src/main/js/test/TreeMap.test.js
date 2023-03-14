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
import TreeMap from "@ohos.util.TreeMap";

describe("TreeMapTest", function () {
  it("SR000GGR3H_testConstructor001", 0, function () {
    try {
      let treeMap = new TreeMap();
      expect(treeMap != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new TreeMap");
    }
  });
  it("SR000GGR3H_testSet002", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "四");
    let res = treeMap.get(0);
    expect(res).assertEqual("四");
  });
  it("SR000GGR3H_testSet003", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, 8);
    let res = treeMap.get(0);
    expect(res).assertEqual(8);
  });
  it("SR000GGR3H_testSet004", 0, function () {
    let treeMap = new TreeMap();
    let a = [1, 2, 3, 4];
    treeMap.set(0, a);
    let res = treeMap.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR3H_testSet005", 0, function () {
    let treeMap = new TreeMap();
    let a = {name: "lala", age: "13"};
    treeMap.set(0, a);
    let res = treeMap.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR3H_testSet006", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(2, "*");
    let res = treeMap.get(2);
    expect(res).assertEqual("*");
  });
  it("SR000GGR3H_testSet007", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(-2, 3.14);
    let res = treeMap.get(-2);
    expect(res).assertEqual(3.14);
  });
  it("SR000GGR3H_testSet008", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(100, true);
    let res = treeMap.get(100);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3H_testSet009", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(100, "");
    let res = treeMap.get(100);
    expect(res).assertEqual("");
  });
  it("SR000GGR3H_testSet010", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set("a", "四");
    let res = treeMap.get("a");
    expect(res).assertEqual("四");
  });
  it("SR000GGR3H_testSet011", 0, function () {
    let treeMap = new TreeMap();
    for (let i = 0; i < 10000; i++) {
      treeMap.set(0, 8);
    }
    let res = treeMap.get(0);
    expect(res).assertEqual(8);
    expect(treeMap.length).assertEqual(1);
  });
  it("SR000GGR3H_testHasKey012", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    let res = treeMap.hasKey(1);
    expect(res).assertEqual(true);
    let res1 = treeMap.hasKey(6);
    expect(res1).assertEqual(false);
  });
  it("SR000GGR3H_testHasValue013", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    let res = treeMap.hasValue("a");
    expect(res).assertEqual(true);
    let res1 = treeMap.hasValue("d");
    expect(res1).assertEqual(false);
  });
  it("SR000GGR3H_testGet014", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    let res = treeMap.get(1);
    expect(res).assertEqual("b");
  });
  it("SR000GGR3H_testGet015", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.get(1);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testGet016", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.clear();
    expect(treeMap.length).assertEqual(0);
    let res = treeMap.get(0);
     expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testGetFirstKey017", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    let res = treeMap.getFirstKey();
    expect(res).assertEqual(0);
  });
  it("SR000GGR3H_testGetFirstKey018", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.getFirstKey();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testGetLastKey019", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    let res = treeMap.getLastKey();
    expect(res).assertEqual(2);
  });
  it("SR000GGR3H_testGetLastKey020", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.getLastKey();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testSetAll021", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    let treeMap1 = new TreeMap();
    treeMap1.set(0, "a");
    treeMap1.set(1, "b");
    treeMap1.set(2, "c");
    treeMap.setAll(treeMap1);
    let res = treeMap.get(0);
    expect(res).assertEqual("a");
    let res1 = treeMap.get(1);
    expect(res1).assertEqual("b");
    let res2 = treeMap.get(2);
    expect(res2).assertEqual("c");
  });
  it("SR000GGR3H_testRemove022", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    let res = treeMap.remove(1);
    expect(res).assertEqual("b");
  });
  it("SR000GGR3H_testRemove023", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    let res = treeMap.remove(1);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testClear024", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.clear();
    let res = treeMap.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR3H_testClear025", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.clear();
    let res = treeMap.length;
    expect(res).assertEqual(0);
    treeMap.set(3, "d");
    let va = treeMap.get(3);
    expect(va).assertEqual("d");
  });
  it("SR000GGR3H_testGetLowerKey026", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.getLowerKey(2);
    expect(res).assertEqual(1);
  });
  it("SR000GGR3H_testGetLowerKey027", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.getLowerKey(2);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testGetHigherKey028", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.getHigherKey(2);
    expect(res).assertEqual(3);
  });
  it("SR000GGR3H_testGetHigherKey029", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.getHigherKey(2);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3H_testKeys030", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.keys();
    expect(res.next().value).assertEqual(0);
    expect(res.next().value).assertEqual(1);
    expect(res.next().value).assertEqual(2);
    expect(res.next().value).assertEqual(3);
    expect(res.next().value).assertEqual(4);
  });
  it("SR000GGR3H_testValues031", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.values();
    expect(res.next().value).assertEqual("a");
    expect(res.next().value).assertEqual("b");
    expect(res.next().value).assertEqual("c");
    expect(res.next().value).assertEqual("d");
    expect(res.next().value).assertEqual("g");
  });
  it("SR000GGR3H_testReplace032", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.replace(2, "B");
    let res1 = treeMap.get(2);
    expect(res1).assertEqual("B");
  });
  it("SR000GGR3H_testReplace033", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    let res = treeMap.replace(2, "B");
    expect(res).assertEqual(false);
    let res1 = treeMap.get(2);
    expect(res1).assertEqual(undefined);
  });
  it("SR000GGR3H_testLength034", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let res = treeMap.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR3H_testLength035", 0, function () {
    let treeMap = new TreeMap();
    let res = treeMap.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR3H_testForEach036", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let arr1 = [];
    let arr2 = [];
    treeMap.forEach((value, key) => {
      arr1.push(value);
    });
    treeMap.forEach((value, key) => {
      arr2.push(key);
    });
    let arr = ["a", "b", "c", "d", "g"];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
    let arr3 = [0, 1, 2, 3, 4];
    for (let i = 0; i < arr3.length; i++) {
      expect(arr3[i]).assertEqual(arr2[i]);
    }
  });
  it("SR000GGR3H_testEntries037", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let entries = treeMap.entries();
    let flag = true;
    for (let i = 0, len = treeMap.length; i < len; i++) {
      let entry = entries.next().value;
      let res = treeMap.get(entry[0]);
      if (res != entry[1]) {
        flag = false;
        break;
      }
    }
    expect(flag).assertEqual(true);
  });
  it("SR000GGR3H_testIterator038", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let arr = ["0,a", "1,b", "2,c", "3,d", "4,g"];
    let iterArr = [];
    for (let [key, value] of treeMap) {
      iterArr.push(key + "," + value);
    }
    for (let i = 0; i < arr.length; i++) {
      expect(arr[i]).assertEqual(iterArr[i]);
    }
  });
  it("SR000GGR3H_testIterator039", 0, function () {
    let treeMap = new TreeMap();
    treeMap.set(0, "a");
    treeMap.set(1, "b");
    treeMap.set(2, "c");
    treeMap.set(3, "d");
    treeMap.set(4, "g");
    let iters = treeMap[Symbol.iterator]();
    let flag = true;
    for (let i = 0, len = treeMap.length; i < len; i++) {
      let entry = iters.next().value;
      let res = treeMap.get(entry[0]);
      if (res != entry[1]) {
        flag = false;
        break;
      }
    }
    expect(flag).assertEqual(true);
  });
});
