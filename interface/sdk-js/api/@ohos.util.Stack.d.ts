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
declare class Stack<T> {
  /**
   * A constructor used to create a Stack object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the Stack. This is a number one higher than the highest index in the Stack.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Tests if this stack is empty
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * Looks at the object at the top of this stack without removing it from the stack
   * Return undfined if this stack is empty
   * @return the top value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  peek(): T;
  /**
   * Removes the object at the top of this stack and returns that object as the value of this function
   * an exception if the stack is empty
   * @returns Stack top value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  pop(): T;
  /**
   * Pushes an item onto the top of this stack
   * @param item to be appended to this Stack
   * @returns the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  push(item: T): T;
  /**
   * Returns the 1-based position where an object is on this stack
   * @param element Target to be deleted
   * @returns the T type,If there is no such element, return -1
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  locate(element: T): number;
  /**
   * Executes a provided function once for each value in the Stack object.
   * @param callbackfn (required) A function that accepts up to four arguments.The function 
   * to be called for each element in the Stack
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param stack (Optional) The Stack object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, stack?: Stack<T>) => void,
  thisArg?: Object): void;
  /**
   * returns an ES6 iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default Stack;
