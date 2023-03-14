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
import List from "@ohos.util.List";

describe("ListTest", function () {
  it("SR000GGR4A_testConstructor001", 0, function () {
    try {
      let list = new List();
      expect(list != undefined).assertEqual(true);
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("cannot create new list");
    }
  });
  it("SR000GGR4A_testGet002", 0, function () {
    let list = new List();
    list.add("四");
    let res = list.get(0);
    expect(res).assertEqual("四");
  });
  it("SR000GGR4A_testAdd002", 0, function () {
    let list = new List();
    list.add(8);
    let res = list.get(0);
    expect(res).assertEqual(8);
  });
  it("SR000GGR4A_testAdd003", 0, function () {
    let list = new List();
    let a = [1, 2, 3, 4];
    list.add(a);
    let res = list.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR4A_testAdd004", 0, function () {
    let list = new List();
    let a = {name: "lala", age: "13"};
    list.add(a);
    let res = list.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR4A_testHas005", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    let res = list.has("a");
    expect(res).assertEqual(true);
  });
  it("SR000GGR4A_testHas006", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    let res = list.has("d");
    expect(res).assertEqual(false);
  });
  it("SR000GGR4A_testEqual007", 0, function () {
    let list = new List();
    let list1 = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list1.add("a");
    list1.add("b");
    list1.add("d");
    let res = list.equal(list1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR4A_testEqual008", 0, function () {
    let list = new List();
    let list1 = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list1.add("a");
    list1.add("b");
    list1.add("c");
    let res = list.equal(list1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR4A_testEqual009", 0, function () {
    let list = new List();
    let list1 = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list1.add("a");
    list1.add("b");
    let res = list.equal(list1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR4A_testGet010", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.get(2);
    expect(res).assertEqual("c");
  });
  it("SR000GGR4A_testGetLastIndexOf011", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getLastIndexOf("a");
    expect(res).assertEqual(3);
  });
  it("SR000GGR4A_testGetIndexOf012", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getIndexOf("b");
    expect(res).assertEqual(1);
  });
  it("SR000GGR4A_testRemoveByIndex013", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.removeByIndex(2);
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "b", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testRemove014", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.remove("a");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["b", "c", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testConstructor015", 0, function () {
    let list = new List();
    list.add(1);
    list.add(2);
    list.add(4);
    list.add(5);
    list.replaceAllElements((item, index) => {
      return (item = 2 * item);
    });
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = [2, 4, 8, 10];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testGetFirst016", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.getFirst();
    expect(res).assertEqual("a");
  });
  it("SR000GGR4A_testGetLast017", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.getLast();
    expect(res).assertEqual("b");
  });
  it("SR000GGR4A_testInsert018", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.insert("d", 3);
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "b", "c", "d", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testSet019", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    let res = list.set(2, "d");
    let res1 = list.get(2);
    expect(res1).assertEqual("d");
  });
  it("SR000GGR4A_testSort020", 0, function () {
    let list = new List();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    list.sort((a, b) => a - b);
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2, 3, 4, 14];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testGetSubList021", 0, function () {
    let list = new List();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    let res = list.getSubList(2, 4);
    let arr = [];
    res.forEach((item, index) => {
      arr.push(item);
    });
    let a = [1, 2];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testConvertToArray022", 0, function () {
    let list = new List();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    let res = list.convertToArray();

    let a = [4, 3, 1, 2, 14];
    for (let i = 0; i < a.length; i++) {
      expect(res[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testLength023", 0, function () {
    let list = new List();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    let res = list.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR4A_testConstructor024", 0, function () {
    let list = new List();
    list.add(8);
    list.add("一");
    list.add("二");
    list.add(5);
    let c = [1, 2, 3, 4];
    list.add(c);
    list.add(6);
    list.add("三");
    list.add("四");
    let arr = [];
    for (let item of list) {
      arr.push(item);
    }
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testForEach025", 0, function () {
    let list = new List();
    list.add(8);
    list.add("一");
    list.add("二");
    list.add(5);
    let c = [1, 2, 3, 4];
    list.add(c);
    list.add(6);
    list.add("三");
    list.add("四");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testIsEmpty026", 0, function () {
    let list = new List();
    list.add(8);
    list.add("一");
    list.add("二");
    list.add(5);
    let c = [1, 2, 3, 4];
    list.add(c);
    list.add(6);
    list.add("三");
    list.add("四");
    let res = list.isEmpty();
    expect(res).assertEqual(false);
  });
  it("SR000GGR4A_testClear027", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.clear();
    let res = list.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR4A_testGet028", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.get(6);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4A_testGetLastIndexOf029", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getLastIndexOf("F");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR4A_testRemoveByIndex030", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    try {
      let res = list.removeByIndex(5);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR4A_testRemove031", 0, function () {
    let list = new List();
    list.add();
    let res = list.remove();
    expect(res).assertEqual(true);
  });
  it("SR000GGR4A_testInsert032", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    try {
      let res = list.insert("d", 8);
    } catch(err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR4A_testSort033", 0, function () {
    let list = new List();
    list.add(";");
    list.add("一");
    list.add("a");
    list.add("1");
    list.add("2");
    list.add("14");
    list.sort((a, b) => a - b);
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = [";", "一", "a", "1", "2", "14"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testGetSubList034", 0, function () {
    let list = new List();
    list.add("4");
    list.add("3");
    list.add("1");
    list.add("2");
    list.add("14");
    try {
      list.getSubList(2, 8);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR4A_testGetSubList035", 0, function () {
    let list = new List();
    list.add("4");
    list.add("3");
    list.add("1");
    list.add("2");
    list.add("14");
    try {
      let res = list.getSubList(6, 8);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the fromIndex or the toIndex is out-of-bounds");
    }
  });
  it("SR000GGR4A_testGetSubList036", 0, function () {
    let list = new List();
    list.add("4");
    list.add("3");
    list.add("1");
    list.add("2");
    list.add("14");
    try {
      let res = list.getSubList(6, 2);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the toIndex cannot be less than or equal to fromIndex");
    }
  });
  it("SR000GGR4A_testAdd037", 0, function () {
    let list = new List();
    list.add(".");
    list.add("*");
    list.add("|");
    let res = list.get(0);
    expect(res).assertEqual(".");
  });
  it("SR000GGR4A_testAdd038", 0, function () {
    let list = new List();
    list.add(".");
    list.add("*");
    list.add("|");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = [".", "*", "|"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR4A_testAdd039", 0, function () {
    let list = new List();
    list.add(" ");
    let res = list.get(0);
    expect(res).assertEqual(" ");
  });
  it("SR000GGR4A_testAdd040", 0, function () {
    let list = new List();
    list.add("a");
    list.add("a");
    let res = list.get(0);
    let res1 = list.get(1);
    expect(res).assertEqual("a");
    expect(res1).assertEqual("a");
  });
  it("SR000GGR4A_testListGet041", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    let res = list[0];
    expect(res).assertEqual("a");
  });
  it("SR000GGR4A_testListSet042", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list[0] = "c";
    let res = list[0];
    expect(res).assertEqual("c");
    list.forEach((item, index) => {
      console.log(item);
    });
  });
  it("SR000GGR4A_testAdd043", 0, function () {
    let list = new List();
    for (let i = 0; i < 100; i++) {
      list.add(i);
    }
    let res = list.get(99);
    let res1 = list.length;
    expect(res).assertEqual(99);
    expect(res1).assertEqual(100);
  });
  it("SR000GGR4A_testAdd044", 0, function () {
    let list = new List();
    list.add(null);
    let res = list.get(0);
    expect(res).assertEqual(null);
  });
  it("SR000GGR4A_testAdd045", 0, function () {
    let list = new List();
    list.add(0.1111);
    let res = list.get(0);
    expect(res).assertEqual(0.1111);
  });
  it("SR000GGR4A_testAdd046", 0, function () {
    let list = new List();
    list.add(-1);
    let res = list.get(0);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR4A_testAdd047", 0, function () {
    let list = new List();
    const obj = {};
    list.add(obj);
    let res = list.get(0);
    expect(res).assertEqual(obj);
  });
  it("SR000GGR4A_testSort048", 0, function () {
    let list = new List();
    list.sort((a, b) => a - b);
    let res1 = list.length;
    expect(res1).assertEqual(0);
  });
  it("SR000GGR4A_testRemoveByIndex049", 0, function () {
    let list = new List();
    try {
      let res = list.removeByIndex(1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR4A_testEqual050", 0, function () {
    let list = new List();
    let list1 = new List();
    let res = list.equal(list1);
    expect(res).assertEqual(true);
  });
  it("SR000GGR4A_testGetIndexOf051", 0, function () {
    let list = new List();
    let res = list.getIndexOf(1);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR4A_testForEach052", 0, function () {
    let list = new List();
    let num = 0;
    list.forEach((item, index) => {
      num++;
    });
    expect(num).assertEqual(0);
  });
  it("SR000GGR4A_testIsEmpty053", 0, function () {
    let list = new List();
    let res = list.isEmpty();
    expect(res).assertEqual(true);
  });
  it("SR000GGR4A_testHas054", 0, function () {
    let list = new List();
    let res = list.has(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR4A_testGet055", 0, function () {
    let list = new List();
    let res = list.get(1);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4A_testClear056", 0, function () {
    let list = new List();
    list.clear();
    let res1 = list.length;
    expect(res1).assertEqual(0);
  });
  it("SR000GGR4A_testGetLast057", 0, function () {
    let list = new List();
    let res = list.getLast();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4A_testReplaceAllElements058", 0, function () {
    let list = new List();
    let num = 0;
    list.replaceAllElements((item, index) => {
      num++;
    });
    expect(num).assertEqual(0);
  });
  it("SR000GGR4A_testConvertToArray059", 0, function () {
    let list = new List();
    const res = list.convertToArray();
    expect(res.length).assertEqual(0);
  });
  it("SR000GGR4A_testConvertToArray060", 0, function () {
    let list = new List();
    const res = list.convertToArray();
    expect(res.length).assertEqual(0);
  });
  it("SR000GGR4A_testlterator061", 0, function () {
    let list = new List();
    for (let index = 0; index < 10; index++) {
      list.add(index);
    }
    let num = 0;
    for (const iterator of list) {
      expect(iterator).assertEqual(num);
      num++;
    }
  });
  it("SR000GGR4A_testGetFirst062", 0, function () {
    let list = new List();
    let res = list.getFirst();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR4A_testIterator063", 0, function () {
    let list = new List();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.removeByIndex(2);
    let arr = [];
    let itr = list[Symbol.iterator]();
    let tmp = undefined;
    do {
      tmp = itr.next().value;
      arr.push(tmp);
    } while (tmp != undefined);
    let a = ["a", "b", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  });
