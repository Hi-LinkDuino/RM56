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
import ArrayList from "@ohos.util.ArrayList";

describe("ArraylistTest", function () {
  it("SR000GGR44_testConstructor001", 0, function () {
    try {
      let arrayList = new ArrayList();
      expect(arrayList != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new arrayList");
    }
  });
  it("SR000GGR44_testAdd002", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    let res = arrayList.has("四");
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd003", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(8);
    let res = arrayList.has(8);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd004 ", 0, function () {
    let arrayList = new ArrayList();
    let a = {name: "lala", age: "13岁"};
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd005 ", 0, function () {
    let arrayList = new ArrayList();
    let a = [1, 2, 3, 4];
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd006 ", 0, function () {
    let arrayList = new ArrayList();
    let a = true;
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testInsert007", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(8, 2);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["四", "三", 8, 1, "a"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testLength008", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(8, 2);
    let res = arrayList.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR44_testHas009", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(8, 2);
    let res = arrayList.has(8);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testGetIndexOf010", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(1);
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(1, 2);
    let res1 = arrayList.getIndexOf(1);
    expect(res1).assertEqual(0);
  });
  it("SR000GGR44_testRemoveByIndex011", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.removeByIndex(2);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["四", "三", "a"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testRemove012", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.remove("三");
    let res = arrayList.has("三");
    expect(res).assertEqual(false);
  });
  it("SR000GGR44_testGetLastIndexOf013", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.add(1);
    arrayList.add("b");
    let res = arrayList.getLastIndexOf(1);
    expect(res).assertEqual(4);
  });
  it("SR000GGR44_testRemoveByRange014", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.add(1);
    arrayList.add("b");
    arrayList.add("c");
    arrayList.add(1);
    arrayList.removeByRange(1, 3);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["四", "a", 1, "b", "c", 1];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testReplaceAllElements015", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.replaceAllElements((item, index) => {
      return (item = 2 * item);
    });
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = [8, 6, 2, 4, 28];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testForEach016", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testSubArrayList017", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let subArr = arrayList.subArrayList(2, 4);
    let arr = [];
    subArr.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testClear018", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.clear();
    let res = arrayList.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR44_testConvertToArray19", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let arr = arrayList.convertToArray();
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testClone020", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let res = arrayList.clone();
    let arr = [];
    res.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testGetCapacity021", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let res = arrayList.getCapacity();
    expect(res).assertEqual(10);
  });
  it("SR000GGR44_testGetCapacity022", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.add("a");
    arrayList.add("b");
    arrayList.add("c");
    arrayList.add("d");
    arrayList.add("v");
    let a = [1, 2, 3, 4];
    arrayList.add(a);
    let res = arrayList.getCapacity();
    expect(res).assertEqual(15);
  });
  it("SR000GGR44_testIncreaseCapacityTo023", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.increaseCapacityTo(8);
    let res = arrayList.getCapacity();
    expect(res).assertEqual(8);
  });
  it("SR000GGR44_testTrimToCurrentLength024", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.trimToCurrentLength();
    let res = arrayList.getCapacity();
    expect(res).assertEqual(5);
  });
  it("SR000GGR44_testIterator025", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(8);
    arrayList.add("一");
    arrayList.add("二");
    arrayList.add(5);
    let c = [1, 2, 3, 4];
    arrayList.add(c);
    arrayList.add(6);
    arrayList.add("三");
    arrayList.add("四");
    let arr = [];
    for (let item of arrayList) {
      arr.push(item);
    }
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testSort026", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.sort((a, b) => a - b);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2, 3, 4, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testInsert0027", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    try {
      let res = arrayList.insert(8, -1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the index is out-of-bounds") {
        expect(err.message).assertEqual("ArrayList: set out-of-bounds");
      } else {
        expect(err.message).assertEqual("the index is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testInsert0028", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    try {
      let res = arrayList.insert(8, 10);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the index is out-of-bounds") {
        expect(err.message).assertEqual("ArrayList: set out-of-bounds");
      } else {
        expect(err.message).assertEqual("the index is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testInsert029", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    try {
      let res = arrayList.insert(8, 11);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the index is out-of-bounds") {
        expect(err.message).assertEqual("ArrayList: set out-of-bounds");
      } else {
        expect(err.message).assertEqual("the index is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testisEmpty030", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.clear();
    let res = arrayList.isEmpty();
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testisEmpty031", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let res = arrayList.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR44_testHas032", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(8, 2);
    let res = arrayList.has("二");
    expect(res).assertEqual(false);
  });
  it("SR000GGR44_testGetIndexOf033", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(1);
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.insert(1, 2);
    let res = arrayList.getIndexOf("A");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR44_testRemove034", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    let res = arrayList.remove("A");
    expect(res).assertEqual(false);
  });
  it("SR000GGR44_testRemoveByIndex035", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(0);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(1);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.forEach((item, index) => {
      if (item == 1) {
        arrayList.remove(item);
      }
    });
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = [0, 2, 3];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testRemoveByIndex036", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("a");
    arrayList.add("b");
    arrayList.add("c");
    arrayList.add("a");
    arrayList.add("b");
    try {
      let res = arrayList.removeByIndex(5);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the index is out-of-bounds") {
        expect(err.message).assertEqual("removeByIndex is out-of-bounds");
      } else {
        expect(err.message).assertEqual("the index is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testRemoveByRange037", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.add(1);
    arrayList.add("b");
    arrayList.add("c");
    arrayList.add(1);
    try {
      let res = arrayList.removeByRange(3, 1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the fromIndex cannot be less than or equal to toIndex") {
        expect(err.message).assertEqual("fromIndex cannot be less than or equal to toIndex");
      } else {
        expect(err.message).assertEqual("the fromIndex cannot be less than or equal to toIndex");
      }
    }
  });
  it("SR000GGR44_testRemoveByRange038", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.add(1);
    try {
      let res = arrayList.removeByRange(6, 8);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the fromIndex or the toIndex is out-of-bounds") {
        expect(err.message).assertEqual("ArrayList: set out-of-bounds");
      } else {
        expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testRemoveByRange039", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("四");
    arrayList.add("三");
    arrayList.add(1);
    arrayList.add("a");
    arrayList.add(1);
    arrayList.add("b");
    arrayList.add("c");
    arrayList.add(1);
    arrayList.removeByRange(0, 9);
    let res = arrayList.length;
    let res1 = arrayList.getLastIndexOf(1);
    expect(res).assertEqual(1);
    expect(res1).assertEqual(0);
  });
  it("SR000GGR44_testSubArrayList040", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    try {
      let subArr = arrayList.subArrayList(4, 2);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the fromIndex cannot be less than or equal to toIndex") {
        expect(err.message).assertEqual("fromIndex cannot be less than or equal to toIndex");
      } else {
        expect(err.message).assertEqual("the fromIndex cannot be less than or equal to toIndex");
      }
    }
  });
  it("SR000GGR44_testSubArrayList041", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    let subArr = arrayList.subArrayList(0, 6);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(a[i]).assertEqual(arr[i]);
    }
  });
  it("SR000GGR44_testSubArrayList042", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    try {
      let subArr = arrayList.subArrayList(6, 9);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      if (err.message != "the fromIndex or the toIndex is out-of-bounds") {
        expect(err.message).assertEqual("fromIndex or toIndex is out-of-bounds");
      } else {
        expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
      }
    }
  });
  it("SR000GGR44_testIncreaseCapacityTo043", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    arrayList.increaseCapacityTo(-1);
    let res = arrayList.getCapacity();
    expect(res).assertEqual(10);
  });
  it("SR000GGR44_testIncreaseCapacityTo044", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add(4);
    arrayList.add(3);
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add(14);
    try {
      let res = arrayList.increaseCapacityTo();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      if (err.message != "index is not integer") {
        expect(err.message).assertEqual("newCapacity is not Integer");
      } else {
        expect(err.message).assertEqual("index is not integer");
      }
    }
  });
  it("SR000GGR44_testAdd045", 0, function () {
    let arrayList = new ArrayList();
    for (let i = 0; i < 100; i++) {
      arrayList.add(i);
    }
    let res = arrayList.getIndexOf(99);
    let res1 = arrayList.length;
    expect(res).assertEqual(99);
    expect(res1).assertEqual(100);
  });
  it("SR000GGR44_testAdd046", 0, function () {
    let arrayList = new ArrayList();
    let a = "*";
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd047", 0, function () {
    let arrayList = new ArrayList();
    let a = "3.14";
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testAdd048", 0, function () {
    let arrayList = new ArrayList();
    let a = "";
    arrayList.add(a);
    let res = arrayList.has(a);
    expect(res).assertEqual(true);
  });
  it("SR000GGR44_testSort049", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("刘");
    arrayList.add("张三");
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add("李四");
    arrayList.sort((a, b) => a - b);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["刘", "张三", 1, 2, "李四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testSort050", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("c");
    arrayList.add("a");
    arrayList.add("b");
    arrayList.add("e");
    arrayList.add("d");
    arrayList.sort((a, b) => a - b);
    let arr = [];
    arrayList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["c", "a", "b", "e", "d"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR44_testIterator051", 0, function () {
    let arrayList = new ArrayList();
    arrayList.add("刘");
    arrayList.add("张三");
    arrayList.add(1);
    arrayList.add(2);
    arrayList.add("李四");
    let arr = [];
    let itr = arrayList[Symbol.iterator]();
    let tmp = undefined;
    do {
      tmp = itr.next().value;
      arr.push(tmp);
    } while (tmp != undefined);
    let a = ["刘", "张三", 1, 2, "李四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
});
