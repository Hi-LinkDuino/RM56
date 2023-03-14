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
declare class TreeSet<T> {
  /**
   * A constructor used to create a TreeSet object.
   * @param comparator (Optional) User-defined comparison functions 
   * @param firstValue (Optional) previous element
   * @param secondValue (Optional) next element
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor(comparator?: (firstValue: T, secondValue: T) => boolean)
  /**
   * Gets the element number of the TreeSet.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Returns whether the Set object contains elements
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * Returns whether the Set object contain s the elements
   * @param value need to determine whether to include the element
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(value: T): boolean;
  /**
   * If the set does not contain the element, the specified element is added
   * @param value Added element
   * @returns the boolean type(Is there contain this element)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(value: T): boolean;
  /**
   * Remove a specified element from a Set object
   * @param value  Target to be deleted
   * @return the boolean type(Is there contain this element)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(value: T): boolean;
  /**
   * Clears all element groups in a set
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Gets the first elements in a set
   * @return value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getFirstValue(): T;
  /**
   * Gets the last elements in a set
   * @return value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLastValue(): T;
  /**
   * Returns the greatest element smaller than or equal to the specified key
   * if the key does not exist, undefied is returned
   * @param key Objective of comparison
   * @return key or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getLowerValue(key: T): T;
  /**
   * Returns the least element greater than or equal to the specified key
   * if the key does not exist, undefied is returned
   * @param key Objective of comparison
   * @return key or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getHigherValue(key: T): T;
  /**
   * Return and delete the first element, returns undefined if tree set is empty
   * @return first value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  popFirst(): T;
  /**
   * Return and delete the last element, returns undefined if tree set is empty
   * @return last value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  popLast(): T;
  /**
   * Executes a provided function once for each value in the Set object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value?: T, key?: T, set?: TreeSet<T>) => void,
  thisArg?: Object): void;
  /**
   * Returns a new Iterator object that contains the values contained in this set
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  values(): IterableIterator<T>;
  /**
   * Returns a new Iterator object that contains the [key, value] pairs for each element in the Set object in insertion order
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  entries(): IterableIterator<[T, T]>;
  /**
   * returns an ES6 iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
}

export default TreeSet;
