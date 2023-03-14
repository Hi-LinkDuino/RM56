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
import PlainArray from "@ohos.util.PlainArray";

describe("PlainArrayTest", function () {
  it("SR000GGR45_testConstructor001", 0, function () {
    try {
      let plainArray = new PlainArray();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new PlainArray");
    }
  });
  it("SR000GGR45_testAdd002", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    let res = plainArray.get(1);
    expect(res).assertEqual("A");
  });
  it("SR000GGR45_testAdd003", 0, function () {
    let plainArray = new PlainArray();
    let a = [1, 2, 3, 4];
    plainArray.add(2, a);
    let res = plainArray.get(2);
    expect(res).assertEqual(a);
  });
  it("SR000GGR45_testAdd004", 0, function () {
    let plainArray = new PlainArray();
    let c = {name: "lili", age: "13"};
    plainArray.add(1, c);
    let res = plainArray.get(1);
    expect(res).assertEqual(c);
  });
  it("SR000GGR45_testAdd005", 0, function () {
    let plainArray = new PlainArray();
    let c = false;
    plainArray.add(1, c);
    let res = plainArray.get(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR45_testAdd006", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, 3);
    let res = plainArray.get(1);
    expect(res).assertEqual(3);
  });
  it("SR000GGR45_testGet007", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.get(4);
    expect(res).assertEqual("D");
    res = plainArray.get(10);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testLength008", 0, function () {
    let plainArray = new PlainArray();
    let res = plainArray.length;
    expect(res).assertEqual(0);
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    res = plainArray.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR45_testClone009", 0, function () {
    let plainArray = new PlainArray();
    let clonePlainarray = plainArray.clone();
    let cloneLength = clonePlainarray.length;
    expect(cloneLength).assertEqual(0);
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.clone();
    let arr = [];
    res.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = ["A", "B", "C", "D", "E"];
    for (let i = 0; i < arr.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR45_testClear010", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.length;
    expect(res).assertEqual(5);
    plainArray.clear();
    let res1 = plainArray.length;
    expect(res1).assertEqual(0);
    plainArray.clear();
    let res2 = plainArray.length;
    expect(res2).assertEqual(0);
  });
  it("SR000GGR45_testHas011", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.has("a");
    expect(res).assertEqual(false);
    let res1 = plainArray.has(1);
    expect(res1).assertEqual(true);
  });
  it("SR000GGR45_testGetIndexOfKey012", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getIndexOfKey(2);
    expect(res).assertEqual(1);
    res = plainArray.getIndexOfKey(12);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR45_testGetIndexOfValue013", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getIndexOfValue("A");
    expect(res).assertEqual(0);
    res = plainArray.getIndexOfValue("Z");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR45_testIsEmpty014", 0, function () {
    let plainArray = new PlainArray();
    let result = plainArray.length;
    expect(result).assertEqual(0);
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR45_testGetKeyAt015", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getKeyAt(2);
    expect(res).assertEqual(3);
    res = plainArray.getKeyAt(10);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testReMove016", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.remove(2);
    expect(res).assertEqual("B");
    let value = plainArray.get(2);
    expect(value).assertEqual(undefined);
    res = plainArray.remove(12);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testReMoveAt017", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.removeAt(2);
    expect(res).assertEqual("C");
    let value = plainArray.get(3);
    expect(value).assertEqual(undefined);
    res = plainArray.removeAt(12);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testReMoveRangeFrom018", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    plainArray.removeRangeFrom(2, 2);
    let keys = [1, 2, 5];
    let values = ["A", "B", "E"];
    for (let i = 0; i < keys.length; i++) {
      expect(values[i]).assertEqual(plainArray.get(keys[i]));
    }
    try {
      plainArray.removeRangeFrom(15, 5);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
    try {
      plainArray.removeRangeFrom(1, -1);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the size cannot be less than 0");
    }
  });
  it("SR000GGR45_testSetValueAt019", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    plainArray.setValueAt(2, "V");
    let keys = [1, 2, 3, 4, 5];
    let values = ["A", "B", "V", "D", "E"];
    for (let i = 0; i < keys.length; i++) {
      expect(values[i]).assertEqual(plainArray.get(keys[i]));
    }
    try {
      plainArray.setValueAt(-1, "X");
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR45_testToString020", 0, function () {
    let plainArray = new PlainArray();
    let res1 = plainArray.toString();
    expect(res1).assertEqual("");
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    let res = plainArray.toString();
    expect(res).assertEqual("1:A,2:B,3:C");
  });
  it("SR000GGR45_testForEach021", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let arr = [];
    plainArray.forEach((value, index) => {
      arr.push(value);
    });
    let arr1 = ["A", "B", "C", "D", "E"];
    for (let i = 0; i < arr1.length; i++) {
      expect(arr[i]).assertEqual(arr1[i]);
    }
  });
  it("SR000GGR45_testIterator022", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let keys = [1, 2, 3, 4, 5];
    let values = ["A", "B", "C", "D", "E"];
    for (let i = 0; i < keys.length; i++) {
      expect(values[i]).assertEqual(plainArray.get(keys[i]));
    }
  });
  it("SR000GGR45_testGetValueAt023", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getValueAt(2);
    expect(res).assertEqual("C");
    res = plainArray.getValueAt(12);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testAdd024", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, 3.1);
    let res = plainArray.get(1);
    expect(res).assertEqual(3.1);
  });
  it("SR000GGR45_testAdd024", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, null);
    let res = plainArray.get(1);
    expect(res).assertEqual(null);
  });
  it("SR000GGR45_testAdd025", 0, function () {
    let plainArray = new PlainArray();
    try {
      let res = plainArray.add("123", null);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
  });
  it("SR000GGR45_testGet026", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.get(8);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testGetIndexOfKey027", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getIndexOfKey(9);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR45_testGetIndexOfValue028", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getIndexOfValue("Asad");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR45_testGetKeyAt029", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getKeyAt(50);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testGetValueAt030", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let res = plainArray.getValueAt(50);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testRemove031", 0, function () {
    let plainArray = new PlainArray();
    let res = plainArray.remove(2);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR45_testRemoveAt032", 0, function () {
    let plainArray = new PlainArray();
    try {
      let res = plainArray.removeAt(2);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR45_testReMoveRangeFrom033", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    plainArray.removeRangeFrom(0, 5);
    let res = plainArray.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR45_testSetValueAt034", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    try {
      plainArray.setValueAt(8, "V");
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR45_testAdd035", 0, function () {
    let plainArray = new PlainArray();
    for (let i = 0; i < 100; i++) {
      plainArray.add(i, i + 1);
    }
    let res = plainArray.get(1);
    let res1 = plainArray.length;
    expect(res).assertEqual(2);
    expect(res1).assertEqual(100);
  });
  it("SR000GGR45_testAdd036", 0, function () {
    let plainArray = new PlainArray();
    for (let i = 0; i < 1000; i++) {
      plainArray.add(i, i + 1);
    }
    let res = plainArray.get(99);
    let res1 = plainArray.length;
    expect(res).assertEqual(100);
    expect(res1).assertEqual(1000);
  });
  it("SR000GGR45_testAdd037", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(3, "a");
    plainArray.add(1, "b");
    plainArray.add(2, "c");
    plainArray.add(5, "d");
    plainArray.add(4, "e");
    let res = plainArray.get(2);
    expect(res).assertEqual("c");
  });
  it("SR000GGR45_testAdd038", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "ABCd");
    plainArray.add(2, "~!@#$^&*()_+-*/=.?<>:;|{}[]");
    plainArray.add(3, null);
    plainArray.add(4, undefined);
    plainArray.add(5, []);
    plainArray.add(6, {});
    plainArray.add(7, true);
    plainArray.add(8, 1234);
    plainArray.add(9, 1.234);
    plainArray.add(10, -1234);
    let res = plainArray.get(2);
    expect(res).assertEqual("~!@#$^&*()_+-*/=.?<>:;|{}[]");
  });
  it("SR000GGR45_testAdd039", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1.23, "a");
    plainArray.add(-2, "b");
    try {
      plainArray.add("a", "c");
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("the index is not integer");
    }
    let res = plainArray.get(-2);
    expect(res).assertEqual("b");
  });
  it("SR000GGR45_testAdd040", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, 1);
    plainArray.add(2, "b");
    plainArray.add(3, true);
    plainArray.add(4, [1, 2, 3, 4]);
    plainArray.add(5, { name: "lili", age: "13" });
    plainArray.add(6, undefined);
    plainArray.add(7, null);
    let res = plainArray.get(2);
    expect(res).assertEqual("b");
  });
  it("SR000GGR45_testIterator041", 0, function () {
    let plainArray = new PlainArray();
    plainArray.add(1, "A");
    plainArray.add(2, "B");
    plainArray.add(3, "C");
    plainArray.add(4, "D");
    plainArray.add(5, "E");
    let iters = plainArray[Symbol.iterator]();
    let flag = true;
    for (let i = 0, len = plainArray.length; i < len; i++) {
      let entry = iters.next().value;
      let res = plainArray.get(entry[0]);
      if (res != entry[1]) {
        flag = false;
        break;
      }
    }
    expect(flag).assertEqual(true);
  });
});
