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
import Deque from "@ohos.util.Deque";

describe("DequeTest", function () {
  it("SR000GGR3K_testConstructor001", 0, function () {
    try {
      let deque = new Deque();
    } catch (err) {
      expect(err.name).assertEqual("TypeError");
      expect(err.message).assertEqual("Cannot create new deque");
    }
  });
  it("SR000GGR3K_testInsertFront001", 0, function () {
    let deque = new Deque();
    deque.insertFront("一");
    let res = deque.getFirst();
    expect(res).assertEqual("一");
  });
  it("SR000GGR3K_testInsertFront002", 0, function () {
    let deque = new Deque();
    deque.insertFront(8);
    let res = deque.getFirst();
    expect(res).assertEqual(8);
  });
  it("SR000GGR3K_testInsertFront003", 0, function () {
    let deque = new Deque();
    let a = {name: "lala", age: "15"};
    deque.insertFront(a);
    let res = deque.getFirst();
    expect(res).assertEqual(a);
  });
  it("SR000GGR3K_testInsertFront004", 0, function () {
    let deque = new Deque();
    let a = [1, 2, 3, 4];
    deque.insertFront(a);
    let res = deque.getFirst();
    expect(res).assertEqual(a);
  });
  it("SR000GGR3K_testInsertEnd005", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    let res = deque.getLast();
    expect(res).assertEqual(8);
  });
  it("SR000GGR3K_testInsertEnd006", 0, function () {
    let deque = new Deque();
    let a = ["a", "b", "c"];
    deque.insertEnd(a);
    let res = deque.getLast();
    expect(res).assertEqual(a);
  });
  it("SR000GGR3K_testInsertEnd007", 0, function () {
    let deque = new Deque();
    let a = {class: "6班", say: "we"};
    deque.insertEnd(a);
    let res = deque.getLast();
    expect(res).assertEqual(a);
  });
  it("SR000GGR3K_testGetFirst007", 0, function () {
    let deque = new Deque();
    deque.insertEnd("四");
    deque.insertEnd("三");
    let res = deque.getFirst();
    expect(res).assertEqual("四");
  });
  it("SR000GGR3K_testGetLast008", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertEnd("三");
    let res = deque.getLast();
    expect(res).assertEqual("三");
  });
  it("SR000GGR3K_testHas009", 0, function () {
    let deque = new Deque();
    deque.insertEnd(6);
    let res = deque.has(6);
    expect(res).assertEqual(true);
  });
  it("SR000GGR3K_testHas010", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    let res = deque.has(6);
    expect(res).assertEqual(false);
  });
  it("SR000GGR3K_testPopFirst011", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("一");
    let res = deque.popFirst();
    expect(res).assertEqual("一");
  });
  it("SR000GGR3K_testPopLast012", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("一");
    deque.insertFront("二");
    let res = deque.popLast();
    expect(res).assertEqual(8);
  });
  it("SR000GGR3K_testForEach013", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("一");
    deque.insertFront("二");
    deque.insertEnd(1);
    deque.insertEnd(2);
    deque.insertEnd(3);
    deque.insertEnd(4);
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.insertFront("三");
    deque.insertFront("四");
    let arr = [];
    deque.forEach((item, index) => {
      arr.push(item);
    });
    let a = ["四", "三", "二", "一", 8, 1, 2, 3, 4, 5, 6];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR3K_testIterator014", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("一");
    deque.insertFront("二");
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.insertFront("三");
    deque.insertFront("四");
    let arr = [];
    for (let item of deque) {
      arr.push(item);
    }
    let a = ["四", "三", "二", "一", 8, 5, 6];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR3K_testInsertFront015", 0, function () {
    let deque = new Deque();
    deque.insertFront("$");
    let res = deque.getFirst();
    expect(res).assertEqual("$");
  });
  it("SR000GGR3K_testInsertFront016", 0, function () {
    let deque = new Deque();
    deque.insertFront(" ");
    let res = deque.getFirst();
    expect(res).assertEqual(" ");
  });
  it("SR000GGR3K_testInsertFront017", 0, function () {
    let deque = new Deque();
    deque.insertFront(null);
    let res = deque.getFirst();
    expect(res).assertEqual(null);
  });
  it("SR000GGR3K_testInsertFront018", 0, function () {
    let deque = new Deque();
    deque.insertFront(undefined);
    let res = deque.getFirst();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testInsertFront019", 0, function () {
    let deque = new Deque();
    for (let i = 0; i < 100; i++) {
      deque.insertFront(i);
      let res = deque.getFirst();
      expect(res).assertEqual(i);
    }
    let res1 = deque.length;
    expect(res1).assertEqual(100);
  });
  it("SR000GGR3K_testInsertEnd020", 0, function () {
    let deque = new Deque();
    deque.insertEnd("$");
    let res = deque.getLast();
    expect(res).assertEqual("$");
  });
  it("SR000GGR3K_testInsertEnd021", 0, function () {
    let deque = new Deque();
    deque.insertEnd(" ");
    let res = deque.getLast();
    expect(res).assertEqual(" ");
  });
  it("SR000GGR3K_testInsertEnd022", 0, function () {
    let deque = new Deque();
    deque.insertEnd(null);
    let res = deque.getLast();
    expect(res).assertEqual(null);
  });
  it("SR000GGR3K_testInsertEnd023", 0, function () {
    let deque = new Deque();
    deque.insertEnd(undefined);
    let res = deque.getLast();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testInsertFront024", 0, function () {
    let deque = new Deque();
    for (let i = 0; i < 100; i++) {
      deque.insertEnd(i);
      let res = deque.getLast();
      expect(res).assertEqual(i);
    }
    let res1 = deque.length;
    expect(res1).assertEqual(100);
  });
  it("SR000GGR3K_testHas025", 0, function () {
    let deque = new Deque();
    let res = deque.has(6);
    expect(res).assertEqual(false);
  });
  it("SR000GGR3K_testPopFirst026", 0, function () {
    let deque = new Deque();
    let res = deque.popFirst();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testGetFirst027", 0, function () {
    let deque = new Deque();
    let res = deque.getFirst();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testPopLast028", 0, function () {
    let deque = new Deque();
    let res = deque.popLast();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testGetLast029", 0, function () {
    let deque = new Deque();
    let res = deque.getLast();
    expect(res).assertEqual(undefined);
  });
  it("SR000GGR3K_testForEach030", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertEnd(1);
    deque.insertEnd(2);
    deque.insertEnd(3);
    deque.insertEnd(3);
    deque.insertEnd(4);
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.popFirst();
    deque.popLast();
    deque.insertFront(8);
    deque.insertEnd(6);
    let arr = [];
    deque.forEach((item, index) => {
      arr.push(item);
    });
    let a = [8, 1, 2, 3, 3, 4, 5, 6];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR3K_testIterator031", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("一");
    deque.insertFront("二");
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.insertFront("三");
    deque.insertFront("四");
    deque.popFirst();
    deque.popLast();
    deque.insertFront("四");
    deque.insertEnd(6);
    let arr = [];
    for (let item of deque) {
      arr.push(item);
    }
    let a = ["四", "三", "二", "一", 8, 5, 6];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR3K_testIterator032", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("a");
    deque.insertFront("b");
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.insertFront("s");
    deque.insertFront("z");
    deque.popFirst();
    deque.popLast();
    deque.insertFront("g");
    deque.insertEnd(6);
    let size = deque.length;
    let arr = [];
    let itr = deque[Symbol.iterator]();
    for (let i = 0; i < size; i++) {
      arr.push(itr.next().value);
    }
    let a = ["g", "s", "b", "a", 8, 5, 6];
    for (let i = 0; i < a.length; i++) {
      expect(arr[i]).assertEqual(a[i]);
    }
  });
  it("SR000GGR3K_testLength033", 0, function () {
    let deque = new Deque();
    deque.insertEnd(8);
    deque.insertFront("a");
    deque.insertFront("b");
    deque.insertEnd(5);
    deque.insertEnd(6);
    deque.insertFront("s");
    deque.insertFront("z");
    deque.popFirst();
    deque.popLast();
    deque.insertFront("g");
    deque.insertEnd(6);
    let size = deque.length;
    expect(size).assertEqual(7);
  });
});
