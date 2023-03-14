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
import LinkedList from "@ohos.util.LinkedList";

describe("LinkedListTest", function () {
  it("SR000GGR46_testConstructor001", 0, function () {
    try {
      let linkedList = new LinkedList();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new linkedList");
    }
  });
  it("SR000GGR46_testAdd002", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add("四");
    let res = linkedList.get(0);
    expect(res).assertEqual("四");
  });
  it("SR000GGR46_testAdd003", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add(8);
    let res = linkedList.get(0);
    expect(res).assertEqual(8);
  });
  it("SR000GGR46_testAdd004", 0, function () {
    let linkedList = new LinkedList();
    let a = [1, 2, 3, 4];
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd005", 0, function () {
    let linkedList = new LinkedList();
    let a = {name: "lala", age: "13"};
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd006", 0, function () {
    let linkedList = new LinkedList();
    let a = ".";
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd007", 0, function () {
    let linkedList = new LinkedList();
    let a = "*";
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd008", 0, function () {
    let linkedList = new LinkedList();
    let a = '/"';
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd009", 0, function () {
    let linkedList = new LinkedList();
    let a = "";
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
  });
  it("SR000GGR46_testAdd010", 0, function () {
    let linkedList = new LinkedList();
    let a = "a";
    linkedList.add(a);
    linkedList.add(a);
    let res = linkedList.get(0);
    expect(res).assertEqual(a);
    let res1 = linkedList.get(1);
    expect(res1).assertEqual(a);
  });
  it("SR000GGR46_testHas011", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add("a");
    linkedList.add("b");
    linkedList.add("c");
    let res = linkedList.has("a");
    expect(res).assertEqual(true);
    let res1 = linkedList.has("d");
    expect(res1).assertEqual(false);
  });
  it("SR000GGR46_testClone012", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    let list1 = list.clone();
    list1.addFirst("e");
    let res = list1.removeLast();
    expect(res).assertEqual("c");
    let arr = [];
    list1.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["e", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testAddFirst013", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add("a");
    linkedList.add("b");
    linkedList.add("c");
    linkedList.addFirst("e");
    let res = linkedList.get(0);
    expect(res).assertEqual("e");
  });
  it("SR000GGR46_testRemoveFirst014", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add("a");
    linkedList.add("b");
    linkedList.add("c");
    let res = linkedList.removeFirst();
    expect(res).assertEqual("a");
    let arr = [];
    linkedList.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["b", "c"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testGetLastIndexOf015", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getLastIndexOf("a");
    expect(res).assertEqual(3);
  });
  it("SR000GGR46_testGetLastIndexOf016", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getLastIndexOf("f");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR46_testGetIndexOf017", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getIndexOf("b");
    expect(res).assertEqual(1);
  });
  it("SR000GGR46_testGetIndexOf018", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list.add("b");
    let res = list.getIndexOf("f");
    expect(res).assertEqual(-1);
  });
  it("SR000GGR46_testRemoveByIndex019", 0, function () {
    let list = new LinkedList();
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
  it("SR000GGR46_testRemoveByIndex020", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    try {
      let res = list.removeByIndex(10);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR46_testRemove0021", 0, function () {
    let list = new LinkedList();
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
  it("SR000GGR46_testRemove022", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.remove("d");
    expect(res).assertEqual(false);
  });
  it("SR000GGR46_testRemove023", 0, function () {
    let list = new LinkedList();
    let res = list.remove("d");
    expect(res).assertEqual(false);
  });
  it("SR000GGR46_testRemoveFirstFound024", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.removeFirstFound("b");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "c", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testRemoveLastFound025", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.removeLastFound("b");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "b", "c", "a"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testGetFirst026", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.getFirst();
    expect(res).assertEqual("a");
  });
  it("SR000GGR46_testGetLast027", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.getLast();
    expect(res).assertEqual("b");
  });
  it("SR000GGR46_testInsert028", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.insert(3, "d");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "b", "c", "d", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testInsert029", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.insert(0, "d");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["d", "a", "b", "c", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testInsert030", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.insert(5, "d");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["a", "b", "c", "a", "b", "d"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testInsert031", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    try {
      let res = list.insert(8, "d");
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR46_testSet032", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    let res = list.set(2, "d");
    let res1 = list.get(2);
    expect(res1).assertEqual("d");
  });
  it("SR000GGR46_testConvertToArray033", 0, function () {
    let list = new LinkedList();
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
  it("SR000GGR46_testLength034", 0, function () {
    let list = new LinkedList();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    let res = list.length;
    expect(res).assertEqual(5);
  });
  it("SR000GGR46_testClear035", 0, function () {
    let list = new LinkedList();
    list.add(4);
    list.add(3);
    list.add(1);
    list.add(2);
    list.add(14);
    list.clear();
    let res = list.length;
    expect(res).assertEqual(0);
  });
  it("SR000GGR46_testIterator036", 0, function () {
    let list = new LinkedList();
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
    let a = [8, "一", "二", 5, c, 6, "三", "四"];
    for (let item of list) {
      arr.push(item);
    }
    for (let index = 0; index < list.length; index++) {
      expect(arr[index]).assertEqual(a[index]);
    }
  });
  it("SR000GGR46_testForEach037", 0, function () {
    let list = new LinkedList();
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
  it("SR000GGR46_testRemoveLast038", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.addFirst("e");
    let res = list.removeLast();
    expect(res).assertEqual("c");
    let arr = [];
    list.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["e", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR46_testGet039", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.get(1);
    expect(res).assertEqual("b");
  });
  it("SR000GGR46_testGet040", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list.get(10);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR46_testListGet041", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    let res = list[1];
    list.forEach((item, index) => {
      console.log(item);
    });
    expect(res).assertEqual("b");
  });
  it("SR000GGR46_testListSet042", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.add("a");
    list.add("b");
    list[1] = "f";
    let res = list[1];
    list.forEach((item, index) => {
      console.log(item);
    });
    expect(res).assertEqual("f");
  });
  it("SR000GGR46_testAdd043", 0, function () {
    let linkedList = new LinkedList();
    for (let i = 0; i < 100; i++) {
      linkedList.add(i);
    }
    let res = linkedList.get(99);
    expect(res).assertEqual(99);
  });
  it("SR000GGR46_testRemoveByIndex044", 0, function () {
    let linkedList = new LinkedList();
    try {
      let res = linkedList.removeByIndex(1);
    } catch (err) {
      expect(err.name).assertEqual("RangeError");
      expect(err.message).assertEqual("the index is out-of-bounds");
    }
  });
  it("SR000GGR46_testClone045", 0, function () {
    let linkedList = new LinkedList();
    let newlist = linkedList.clone();
    let res = true
    for (let i = 0; i < linkedList.length; i++) {
      if (linkedList[i] !== newlist[i]) {
        res = false
      }
    }
    expect(res).assertEqual(true);
  });
  it("SR000GGR46_testGetIndexOf046", 0, function () {
    let linkedList = new LinkedList();
    let res = linkedList.getIndexOf(1);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR46_testForEach047", 0, function () {
    let linkedList = new LinkedList();
    let num = 0;
    linkedList.forEach((item, index) => {
      num++;
    });
    expect(num).assertEqual(0);
  });
  it("SR000GGR46_testHas048", 0, function () {
    let linkedList = new LinkedList();
    let res = linkedList.has(1);
    expect(res).assertEqual(false);
  });
  it("SR000GGR46_testGet050", 0, function () {
    let linkedList = new LinkedList();
    let res = linkedList.get(1);
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR46_testClear051", 0, function () {
    let linkedList = new LinkedList();
    linkedList.clear();
    const len = linkedList.length;
    expect(len).assertEqual(0);
  });
  it("SR000GGR46_testGetLast052", 0, function () {
    let linkedList = new LinkedList();
    let res = linkedList.getLast();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR46_testGetFirst053", 0, function () {
    let linkedList = new LinkedList();
    let res = linkedList.getFirst();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR46_testAdd054", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add(null);
    let res = linkedList.get(0);
    expect(res).assertEqual(null);
  });
  it("SR000GGR46_testAdd055", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add(0.1);
    let res = linkedList.get(0);
    expect(res).assertEqual(0.1);
  });
  it("SR000GGR46_testAdd056", 0, function () {
    let linkedList = new LinkedList();
    linkedList.add(-1);
    let res = linkedList.get(0);
    expect(res).assertEqual(-1);
  });
  it("SR000GGR46_testAdd057", 0, function () {
    let linkedList = new LinkedList();
    const obj = {};
    linkedList.add(obj);
    let res = linkedList.get(0);
    expect(res).assertEqual(obj);
  });
  it("SR000GGR46_testIterator058", 0, function () {
    let list = new LinkedList();
    list.add("a");
    list.add("b");
    list.add("c");
    list.addFirst("e");
    let res = list.removeLast();
    expect(res).assertEqual("c");
    let arr = [];
    let itr = list[Symbol.iterator]();
    let tmp = undefined;
    do {
      tmp = itr.next().value;
      arr.push(tmp);
    } while (tmp != undefined);
    let a = ["e", "a", "b"];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
});
