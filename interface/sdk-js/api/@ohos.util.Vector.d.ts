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
declare class Vector<T> {
  /**
   * A constructor used to create a Vector object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the Vector. This is a number one higher than the highest index in the vector.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Appends the specified element to the end of this vector.
   * @param element to be appended to this vector
   * @returns the boolean type, returns true if the addition is successful, and returns false if it fails.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(element: T): boolean;
  /**
   * Inserts the specified element at the specified position in this
   * vector. Shifts the element currently at that position (if any) and
   * any subsequent elements to the right (adds one to their index).
   * @param index index at which the specified element is to be inserted
   * @param element element to be inserted
   * @throws If index is greater than or equal to length, or less than 0, an exception is thrown and cannot be inserted
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  insert(element: T, index: number): void;
  /**
   * Check if vector contains the specified element
   * @param element element to be contained
   * @return the boolean type,if vector contains the specified element,return true,else return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(element: T): boolean;
  /**
   * Returns the element at the specified position in this Vector,or returns undefined if vector is empty
   * @param element element to be contained
   * @return the number type ,returns the lowest index such that or -1 if there is no such index.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  get(index: number): T;
  /**
   * Returns the index of the first occurrence of the specified element
   * in this vector, or -1 if this vector does not contain the element.
   * @param element current index
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOf(element: T): number;
  /**
   * Returns the first component (the item at index 0) of this vector.
   * or returns undefined if vector is empty
   * @return the T type ,returns undefined if vector is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getFirstElement(): T;
  /**
   * Returns the Last component (the item at index length-1) of this vector.
   * or returns undefined if vector is empty
   * @return the T type ,returns undefined if vector is empty
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastElement(): T;
  /**
   * Find the corresponding element according to the index,
   * delete the element, and move the index of all elements to the right of the element forward by one.
   * @param index the index in the vector
   * @return the T type ,returns undefined if vector is empty,If the index is
   * out of bounds (greater than or equal to length or less than 0), throw an exception
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeByIndex(index: number): T;
  /**
   * Removes the first occurrence of the specified element from this vector,
   * if it is present.  If the vector does not contain the element, it is
   * unchanged.  More formally, removes the element with the lowest index
   * @param element element to remove
   * @return the boolean type ,If there is no such element, return false
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(element: T): boolean;
  /**
   * Replaces the element at the specified position in this Vector with the specified element
   * @param element replaced element
   * @param index index to find
   * @return the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  set(index: number, element: T): T;
  /**
   * Returns in the index of the last occurrence of the specified element in this vector ,
   * or -1 if the vector does not contain the element.
   * @param element element to find
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastIndexOf(element: T): number;
  /**
   * Returns the index of the last occurrence of the specified element in this vector ,searching backwards from index,
   * or returns -1 if the element is not found,or -1 if there is no such index
   * @param element element to find
   * @param index start index
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastIndexFrom(element: T, index: number): number;
  /**
   * Returns the index of the first occurrence of the specified element in this vector ,searching forwards from index,
   * or returns -1 if the element is not found,or -1 if there is no such index
   * @param element element to find
   * @param index start index
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexFrom(element: T, index: number): number;
  /**
   * Removes from this vector all of the elements whose index is between fromIndex,inclusive,and toIndex ,exclusive.
   * @param fromIndex The starting position of the index, containing the value at that index position
   * @param toIndex the end of the index, excluding the value at that index
   * @throws If the fromIndex is out of range (greater than or equal to length or less than 0),
   *  or if toIndex is less than fromIndex, an IndexOutOfBoundException is thrown
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeByRange(fromIndex: number, toIndex: number): void;
  /**
   * Replaces each element of this vector with the result of applying the operator to that element.
   * @param callbackfn (required) A function that accepts up to four arguments.The function to be called
   * for each element in the vector,Returns the result of an operation
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param vector (Optional) The vector object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  replaceAllElements(callbackfn: (value: T, index?: number, vector?: Vector<T>) => T,
  thisArg?: Object): void;
  /**
   * Executes a provided function once for each value in the vector object.
   * @param callbackfn (required) A function that accepts up to four arguments.The function to be 
   * called for each element in the vector
   * @param Value (required) current element
   * @param Index (Optional) The index value of the current element.
   * @param vector (Optional) The vector object to which the current element belongs.
   * @param thisArg (Optional) The value passed to the function generally uses the "this" value.
   * If this parameter is empty, "undefined" will be passed to the "this" value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, vector?: Vector<T>) => void,
  thisArg?: Object): void;
  /**
   * Sorts this vector according to the order induced by the specified comparator,without comparator 
   * this parameter, it will default to ASCII sorting
   * @param comparator (Optional) A function that accepts up to two arguments.Specifies the sort order.
   * Must be a function,return number type,If it returns firstValue minus secondValue, it returns an vector sorted 
   * in ascending order;If it returns secondValue minus firstValue, it returns an vector sorted in descending order;
   * @param firstValue (Optional) previous element
   * @param secondValue (Optional) next elements
   * If this parameter is empty, it will default to ASCII sorting
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  sort(comparator?: (firstValue: T, secondValue: T) => number): void;
  /**
   * Returns a view of the portion of this vector between the specified fromIndex,inclusize,and toIndex,exclusive
   * @param fromIndex The starting position of the index, containing the value at that index position
   * @param toIndex the end of the index, excluding the value at that index
   * @throws If the fromIndex or toIndex index is out of range (greater than or equal to length or less than 0),
   * or if toIndex is less than fromIndex, an IndexOutOfBoundException is thrown
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  subVector(fromIndex: number, toIndex: number): Vector<T>;
  /**
   * Removes all of the elements from this vector.The vector will
   * be empty after this call returns.length becomes 0
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Returns a shallow copy of this instance. (The elements themselves are not copied.)
   * @return this vector instance
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clone(): Vector<T>;
  /**
   * Sets the length of this vector
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  setLength(newSize: number): void;
  /**
   * returns the capacity of this vector
   * @return the number type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getCapacity(): number;
  /**
   * convert vector to array
   * @return the Array type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  convertToArray(): Array<T>;
  /**
   * Determine whether vector is empty and whether there is an element
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * If the newCapacity provided by the user is greater than or equal to length,
   * change the capacity of the vector to newCapacity, otherwise the capacity will not be changed
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  increaseCapacityTo(newCapacity: number): void;
  /**
   * Returns a string representation of this Vector,
   * containing the String representation of each element
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  toString(): string;
  /**
   * Limit the capacity to the current length
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  trimToCurrentLength(): void;
  /**
   * Copies the components of this vector into the specified array,
   * to overwrite elements of the same index
   * @param Array replaced array
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  copyToArray(array: Array<T>): void;
  /**
   * returns an ES6 iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default Vector;
