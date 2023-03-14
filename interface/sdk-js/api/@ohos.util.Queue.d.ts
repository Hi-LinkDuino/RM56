/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
declare class Queue<T> {
  /**
   * A constructor used to create a Queue object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the Queue.This is a number one higher than the highest index in the queue.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Inserting specified element at the end of a queue if it is possible to do
   * so immediately without violating capacity restrictions.
   * @param element to be appended to this queue
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(element: T): boolean;
  /**
   * Obtains the header element of a queue.
   * @return the T type
   * @throws an exception if the queue is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getFirst(): T;
  /**
   * Retrieves and removes the head of this queue
   * @return the T type
   * @throws an exception if the queue is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  pop(): T;
  /**
   * Executes a provided function once for each value in the queue object.
   * @param callbackfn (required) A function that accepts up to four arguments.The function to
   * be called for each element in the queue
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param Queue (Optional) The queue object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, Queue?: Queue<T>) => void,
  thisArg?: Object): void;
  /**
   * returns an iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default Queue;
