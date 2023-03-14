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
import Vector from "@ohos.util.Vector";

describe("VectorTest", function () {
  it("SR000GGR47_testConstructor001", 0, function () {
    try {
      let vector = new Vector();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new vector");
    }
  });
  it("SR000GGR47_testAdd002", 0, function () {
    let vector = new Vector();
    vector.add("四");
    let res = vector.getFirstElement();
    expect(res).assertEqual("四");
  });
  it("SR000GGR47_testAdd003 ", 0, function () {
    let vector = new Vector();
    vector.add(4);
    let res = vector.getFirstElement();
    expect(res).assertEqual(4);
  });
  it("SR000GGR47_testAdd004 ", 0, function () {
    let vector = new Vector();
    let a = {name: "lala", age: "13岁"};
    vector.add(a);
    let res = vector.getFirstElement();
    expect(res).assertEqual(a);
  });
  it("SR000GGR47_testAdd005 ", 0, function () {
    let vector = new Vector();
    let a = [1, 2, 3, 4];
    vector.add(a);
    let res = vector.getFirstElement();
    expect(res).assertEqual(a);
  });
  it("SR000GGR47_testInsert006", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    let res = vector.get(2);
    expect(res).assertEqual(8);
  });
  it("SR000GGR47_testInsert007", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    try {
      vector.insert(-1, 2);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testInsert008", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    let capacity = vector.getCapacity();
    vector.insert(capacity, 2);
    let res = vector.get(2);
    expect(res).assertEqual(10);
  });
  it("SR000GGR47_testInsert009", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    let capacity = vector.getCapacity();
    try {
      vector.insert(2, capacity + 1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testLength010", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    let res = vector.length;
    expect(res).assertEqual(4);
  });
  it("SR000GGR47_testHas011", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    let res = vector.has(8);
    expect(res).assertEqual(true);
  });
  it("SR000GGR47_testHas012", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    let res1 = vector.has("二");
    expect(res1).assertEqual(false);
  });
  it("SR000GGR47_testGet013", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    let res1 = vector.get(1);
    expect(res1).assertEqual("三");
  });
  it("SR000GGR47_testGet014", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    try {
      let res = vector.get(10);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testGetIndexOf015", 0, function () {
    let vector = new Vector();
    vector.add(1);
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(1, 2);
    let res1 = vector.getIndexOf(1);
    expect(res1).assertEqual(0);
  });
  it("SR000GGR47_testGetFirstElement016", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    let res = vector.getFirstElement();
    expect(res).assertEqual("四");
  });
  it("SR000GGR47_testGetFirstElement017", 0, function () {
    let vector = new Vector();
    let res = vector.getFirstElement();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR47_testSet018", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.insert(8, 2);
    vector.set(1, "二");
    let res = vector.get(1);
    expect(res).assertEqual("二");
  });
  it("SR000GGR47_testRemoveByIndex019", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.removeByIndex(2);
    let res = vector.has(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR47_testRemove020", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.remove("三");
    let res = vector.has("三");
    expect(res).assertEqual(false);
  });
  it("SR000GGR47_testRemove021", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    let res = vector.remove(2);
    expect(res).assertEqual(false);
  });
  it("SR000GGR47_testGetLastElement022", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    let res1 = vector.length;
    let res = vector.getLastElement();
    expect(res).assertEqual("a");
  });
  it("SR000GGR47_testGetLastElement023", 0, function () {
    let vector = new Vector();
    let res = vector.getLastElement();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR47_testGetLastIndexOf024", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    let res = vector.getLastIndexOf(1);
    expect(res).assertEqual(4);
  });
  it("SR000GGR47_testGetLastIndexFrom025", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    let res = vector.getLastIndexFrom(1, 5);
    expect(res).assertEqual(4);
  });
  it("SR000GGR47_testGetIndexFrom026", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    let res = vector.getIndexFrom(1, 5);
    expect(res).assertEqual(7);
  });
  it("SR000GGR47_testRemoveByRange027", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    vector.removeByRange(1, 3);
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["四", "a", 1, "b", "c", 1];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testRemoveByRange028", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    try {
      vector.removeByRange(3, 1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex cannot be less than or equal to toIndex");
    }
  });
  it("SR000GGR47_testRemoveByRange029", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    let length = vector.length;
    try {
      vector.removeByRange(length + 1, 7);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR47_testRemoveByRange030", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    let length = vector.length;
    try {
      vector.removeByRange(1, 7);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testRemoveByRange031", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    let capacity = vector.getCapacity();
    vector.removeByRange(0, capacity);
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR47_testRemoveByRange032", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    let capacity = vector.getCapacity();
    try {
      vector.removeByRange(-1, capacity);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR47_testRemoveByRange033", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    let capacity = vector.getCapacity();
    vector.removeByRange(0, capacity + 1);
    let length = vector.length;
    expect(length).assertEqual(0);
  });
  it("SR000GGR47_testSetLength034", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    vector.add(1);
    vector.add("a");
    vector.add(1);
    vector.add("b");
    vector.add("c");
    vector.add(1);
    vector.setLength(5);
    let res1 = vector.length;
    expect(res1).assertEqual(5);
  });
  it("SR000GGR47_testReplaceAllElements035", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.replaceAllElements((item, index) => {
      return (item = 2 * item);
    });
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    let a = [8, 6, 2, 4, 28];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testForEach036", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testForEach037", 0, function () {
    let vector = new Vector();
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR47_testSubVector038", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let res = vector.subVector(2, 4);
    let arr = [];
    res.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testSubVector039", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    try {
      let res = vector.subVector(4, 2);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex cannot be less than or equal to toIndex");
    }
  });
  it("SR000GGR47_testSubVector040", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let length = vector.length;
    try {
      vector.subVector(length + 1, length + 3);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR47_testSubVector041", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let length = vector.length;
    try {
      let res = vector.subVector(1, length + 1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR47_testClear042", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.clear();
    let res = vector.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR47_testConvertToArray043", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let arr = vector.convertToArray();
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testCopyToArray044", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let arr1 = ["a", "b", "c"];
    let res = vector.copyToArray(arr1);
    let a = [4, 3, 1];
    for (let i = 0; i < a.length; i++) {
      expect(arr1[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testToString045", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let res = vector.toString();
    expect(res).assertEqual("4,3,1,2,14");
  });
  it("SR000GGR47_testClone046", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let res = vector.clone();
    let arr = [];
    res.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testGetCapacity047", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    let res = vector.getCapacity();
    expect(res).assertEqual(10);
  });
  it("SR000GGR47_testGetCapacity048", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.add("a");
    vector.add("b");
    vector.add("c");
    vector.add("d");
    vector.add("v");
    let a = [1, 2, 3, 4];
    vector.add(a);
    let res = vector.getCapacity();
    expect(res).assertEqual(20);
  });
  it("SR000GGR47_testIncreaseCapacityTo049", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.increaseCapacityTo(30);
    let res = vector.getCapacity();
    expect(res).assertEqual(30);
  });
  it("SR000GGR47_testTrimToCurrentLength050", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.trimToCurrentLength();
    let res = vector.getCapacity();
    expect(res).assertEqual(5);
  });
  it("SR000GGR47_testSetLength051", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.setLength(7);
    let res1 = vector.length;
    expect(res1).assertEqual(7);
  });
  it("SR000GGR47_testIterator052", 0, function () {
    let vector = new Vector();
    vector.add(8);
    vector.add("一");
    vector.add("二");
    vector.add(5);
    let c = [1, 2, 3, 4];
    vector.add(c);
    vector.add(6);
    vector.add("三");
    vector.add("四");
    let arr = [];
    for (let item of vector) {
      arr.push(item);
    }
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testIteratorAndRemove053", 0, function () {
    let vector = new Vector();
    vector.add(8);
    vector.add("一");
    vector.add("二");
    vector.add(5);
    let c = [1, 2, 3, 4];
    vector.add(c);
    vector.add(6);
    vector.add(8);
    vector.add("三");
    vector.add("四");
    let arr = [];
    for (let item of vector) {
      if (item == 8) {
        vector.remove(8);
      }
    }
    for (let item of vector) {
      arr.push(item);
    }
    let a = ["一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testIteratorAndRemove054", 0, function () {
    let vector = new Vector();
    vector.add(8);
    vector.add("一");
    vector.add("二");
    vector.add(5);
    let c = [1, 2, 3, 4];
    vector.add(c);
    vector.add(6);
    vector.add("三");
    vector.add("四");
    let arr = [];
    for (let item of vector) {
      if (item == 123) {
        vector.remove(123);
      } else {
        arr.push(item);
      }
    }
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testIterator055", 0, function () {
    let vector = new Vector();
    let arr = [];
    for (let item of vector) {
      arr.push(item);
    }
    expect(arr.length).assertEqual(0);
  });
  it("SR000GGR47_testSort056", 0, function () {
    let vector = new Vector();
    vector.add(4);
    vector.add(3);
    vector.add(1);
    vector.add(2);
    vector.add(14);
    vector.sort((a, b) => a - b);
    let arr = [];
    vector.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2, 3, 4, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR47_testisEmpty057", 0, function () {
    let vector = new Vector();
    vector.add("四");
    let res = vector.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR47_testisEmpty058", 0, function () {
    let vector = new Vector();
    let res = vector.isEmpty();
    expect(res).assertEqual(true);
  });
  it("SR000GGR47_testAdd060", 0, function () {
    let vector = new Vector();
    for (let i = 0; i < 100; i++) {
      let res3 = vector.add(i);
    }
    let res = vector.getLastElement();
    let res1 = vector.length;
    expect(res).assertEqual(99);
    expect(res1).assertEqual(100);
  });
  it("SR000GGR47_testAdd061 ", 0, function () {
    let vector = new Vector();
    vector.add("%");
    let res = vector.getFirstElement();
    expect(res).assertEqual("%");
  });
  it("SR000GGR47_testAdd062 ", 0, function () {
    let vector = new Vector();
    vector.add(1.89);
    let res = vector.getFirstElement();
    expect(res).assertEqual(1.89);
  });
  it("SR000GGR47_testAdd063 ", 0, function () {
    let vector = new Vector();
    vector.add("");
    let res = vector.getFirstElement();
    expect(res).assertEqual("");
  });
  it("SR000GGR47_testAdd064 ", 0, function () {
    let vector = new Vector();
    vector.add(true);
    let res = vector.getFirstElement();
    expect(res).assertEqual(true);
  });
  it("SR000GGR47_testGetIndexOf065", 0, function () {
    let vector = new Vector();
    vector.add(1);
    vector.add(2);
    vector.add(3);
    let res = vector.getIndexOf(23);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR47_testSet066", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    try {
      vector.set(12, "二");
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testRemoveByIndex067", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    try {
      vector.removeByIndex(12);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR47_testGetLastIndexOf068", 0, function () {
    let vector = new Vector();
    vector.add("四");
    vector.add("三");
    let res = vector.getLastIndexOf("二");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR47_testIterator069", 0, function () {
    let vector = new Vector();
    vector.add(8);
    vector.add("一");
    vector.add("二");
    vector.add(5);
    let c = [1, 2, 3, 4];
    vector.add(c);
    vector.add(6);
    vector.add("三");
    vector.add("四");
    let arr = [];
    let itr = vector[Symbol.iterator]();
    let tmp = undefined;
    do {
      tmp = itr.next().value;
      arr.push(tmp);
    } while (tmp != undefined);
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
});
