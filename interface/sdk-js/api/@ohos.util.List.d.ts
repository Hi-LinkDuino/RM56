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
declare class List<T> {
  /**
   * A constructor used to create a List object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the List. This is a number one higher than the highest index in the list.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Appends the specified element to the end of this list.
   * @param element to be appended to this list
   * @returns the boolean type, returns true if the addition is successful, and returns false if it fails.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(element: T): boolean;
  /**
   * Inserts the specified element at the specified position in this list.
   * @param index index at which the specified element is to be inserted
   * @param element element to be inserted
   * @throws If index is greater than or equal to length, or less than 0, an exception is thrown and cannot be inserted
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  insert(element: T, index: number): void;
  /**
   * Returns the element at the specified position in this list,
   * or returns undefined if this list is empty
   * @param index specified position
   * @return the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  get(index: number): T;
  /**
   * Check if list contains the specified element
   * @param element element to be contained
   * @return the boolean type,if list contains the specified element,return true,else return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(element: T): boolean;
  /**
   * Returns the index of the first occurrence of the specified element
   * in this list, or -1 if this list does not contain the element.
   * @param element element to be contained
   * @return the number type ,returns the lowest index such that or -1 if there is no such index.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOf(element: T): number;
  /**
   * Find the corresponding element according to the index,
   * @param index the index in the list
   * @return the T type ,returns undefined if list is empty,If the index is
   * out of bounds (greater than or equal to length or less than 0), throw an exception
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeByIndex(index: number): T;
  /**
   * Removes the first occurrence of the specified element from this list,
   * if it is present.  If the list does not contain the element, it is
   * unchanged.  More formally, removes the element with the lowest index
   * @param element element to remove
   * @return the boolean type ,If there is no such element, return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(element: T): boolean;
  /**
   * Returns in the index of the last occurrence of the specified element in this list ,
   * or -1 if the list does not contain the element.
   * @param element element to find
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastIndexOf(element: T): number;
  /**
   * Returns the first element (the item at index 0) of this list.
   * or returns undefined if list is empty
   * @return the T type ,returns undefined if list is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getFirst(): T;
  /**
   * Returns the Last element (the item at index length-1) of this list.
   * or returns undefined if list is empty
   * @return the T type ,returns undefined if list is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLast(): T;
  /**
   * Replaces the element at the specified position in this List with the specified element
   * @param element replaced element
   * @param index index to find
   * @return the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  set(index: number, element: T): T;
  /**
   * Compares the specified object with this list for equality.if the object are the same as this list
   * return true, otherwise return false.
   * @param obj Compare objects
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  equal(obj: Object): boolean;
  /**
   * Replaces each element of this list with the result of applying the operator to that element.
   * @param callbackfn (required) A function that accepts up to four arguments.
   * The function to be called for each element in the list,Returns the result of an operation
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param List (Optional) The list object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, List?: List<T>) => void,
  thisArg?: Object): void;
  /**
   * Sorts this list according to the order induced by the specified comparator
   * @param comparator (required) A function that accepts up to two arguments.
   * Specifies the sort order. Must be a function,return number type,If it returns firstValue 
   * minus secondValue, it returns an list sorted in ascending order;If it returns secondValue 
   * minus firstValue, it returns an list sorted in descending order;
   * @param firstValue (Optional) previous element
   * @param secondValue (Optional) next elements
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  sort(comparator: (firstValue: T, secondValue: T) => number): void;
  /**
   * Removes all of the elements from this list.The list will
   * be empty after this call returns.length becomes 0
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Returns a view of the portion of this list between the specified fromIndex,inclusize,and toIndex,exclusive
   * @param fromIndex The starting position of the index, containing the value at that index position
   * @param toIndex the end of the index, excluding the value at that index
   * @throws If the fromIndex or toIndex index is out of range (greater than or equal to length or less than 0),
   * or if toIndex is less than fromIndex, an IndexOutOfBoundException is thrown
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getSubList(fromIndex: number, toIndex: number): List<T>;
  /**
   * Replaces each element of this list with the result of applying the operator to that element.
   * @param callbackfn (required) A function that accepts up to four arguments.
   * The function to be called for each element in the list,Returns the result of an operation
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param list (Optional) The list object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  replaceAllElements(callbackfn: (value: T, index?: number, list?: List<T>) => T,
  thisArg?: Object): void;
  /**
   * convert list to array
   * @return the Array type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  convertToArray(): Array<T>;
  /**
   * Determine whether list is empty and whether there is an element
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * returns an iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default List;
