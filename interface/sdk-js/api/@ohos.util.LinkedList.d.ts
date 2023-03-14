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
declare class LinkedList<T> {
  /**
   * A constructor used to create a LinkedList object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the LinkedList. This is a number one higher than the highest index in the linkedlist.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Appends the specified element to the end of this linkedlist.
   * @param element to be appended to this linkedlist
   * @returns the boolean type, returns true if the addition is successful, and returns false if it fails.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(element: T): boolean;
  /**
   * Inserts the specified element at the specified position in this linkedlist.
   * @param index index at which the specified element is to be inserted
   * @param element element to be inserted
   * @throws If index is greater than or equal to length, or less than 0, an exception is thrown and cannot be inserted
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  insert(index: number, element: T): void;
  /**
   * Returns the element at the specified position in this linkedlist,
   * or returns undefined if this linkedlist is empty
   * @param index specified position
   * @return the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  get(index: number): T;
  /**
   * Inserts the specified element at the beginning of this LinkedList.
   * @param element the element to add
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  addFirst(element: T): void;
  /**
   * Retrieves and removes the head (first element) of this linkedlist.
   * @return the head of this list
   * @throws NoSuchElementException if this linkedlist is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeFirst(): T;
  /**
   * Removes and returns the last element from this linkedlist.
   * @return the head of this list
   * @throws NoSuchElementException if this linkedlist is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   *
   */
  removeLast(): T;
  /**
   * Check if linkedlist contains the specified element
   * @param element element to be contained
   * @return the boolean type,if linkedList contains the specified element,return true,else return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(element: T): boolean;
  /**
   * Returns the index of the first occurrence of the specified element
   * in this linkedlist, or -1 if this linkedlist does not contain the element.
   * @param element element to be contained
   * @return the number type ,returns the lowest index such that or -1 if there is no such index.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOf(element: T): number;
  /**
   * Find the corresponding element according to the index,
   * @param index the index in the linkedlist
   * @return the T type ,returns undefined if linkedlist is empty,If the index is
   * out of bounds (greater than or equal to length or less than 0), throw an exception
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeByIndex(index: number): T;
  /**
   * Removes the first occurrence of the specified element from this linkedlist,
   * if it is present.  If the linkedlist does not contain the element, it is
   * unchanged.  More formally, removes the element with the lowest index
   * @param element element to remove
   * @return the boolean type ,If there is no such element, return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(element: T): boolean;
  /**
   * Removes the first occurrence of the specified element from this linkedlist,
   * if it is present.  If the linkedlist does not contain the element, it is
   * unchanged.  More formally, removes the element with the lowest index
   * @param element element to remove
   * @return the boolean type ,If there is no such element, return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeFirstFound(element: T): boolean;
  /**
   * Removes the last occurrence of the specified element from this linkedlist,
   * if it is present.  If the linkedlist does not contain the element, it is
   * unchanged.  More formally, removes the element with the lowest index
   * @param element element to remove
   * @return the boolean type ,If there is no such element, return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeLastFound(element: T): boolean;
  /**
   * Returns in the index of the last occurrence of the specified element in this linkedlist ,
   * or -1 if the linkedlist does not contain the element.
   * @param element element to find
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastIndexOf(element: T): number;
  /**
   * Returns the first element (the item at index 0) of this linkedlist.
   * or returns undefined if linkedlist is empty
   * @return the T type ,returns undefined if linkedList is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getFirst(): T;
  /**
   * Returns the Last element (the item at index length-1) of this linkedlist.
   * or returns undefined if linkedlist is empty
   * @return the T type ,returns undefined if linkedList is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLast(): T;
  /**
   * Replaces the element at the specified position in this Vector with the specified element
   * @param element replaced element
   * @param index index to find
   * @return the T type ,returns undefined if linkedList is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  set(index: number, element: T): T;
  /**
   * Replaces each element of this linkedlist with the result of applying the operator to that element.
   * @param callbackfn (required) A function that accepts up to four arguments.
   * The function to be called for each element in the linkedlist,Returns the result of an operation
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param LinkedList (Optional) The linkedlist object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, LinkedList?: LinkedList<T>) => void,
  thisArg?: Object): void;
  /**
   * Removes all of the elements from this linkedlist.The linkedlist will
   * be empty after this call returns.length becomes 0
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Returns a shallow copy of this instance. (The elements themselves are not copied.)
   * @return this linkedlist instance
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clone(): LinkedList<T>;
  /**
   * convert linkedlist to array
   * @return the Array type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  convertToArray(): Array<T>;
  /**
   * returns an iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default LinkedList;
