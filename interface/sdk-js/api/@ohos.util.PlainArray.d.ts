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
declare class PlainArray<T> {
  /**
   * A constructor used to create a PlainArray object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the PlainArray.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Appends a key-value pair to PlainArray
   * @param key Added the key of key-value
   * @param value Added the value of key-value
   * @throws Throws this exception if input is invaild
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(key: number, value: T): void;
  /**
   * Clears the current PlainArray object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Obtains a clone of the current PlainArray object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clone(): PlainArray<T>;
  /**
   * Checks whether the current PlainArray object contains the specified key
   * @param key need to determine whether to include the key
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(key: number): boolean;
  /**
   * Queries the value associated with the specified key
   * @param key Looking for goals
   * @return the value of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  get(key: number): T;
  /**
   * Queries the index for a specified key
   * @param key Looking for goals
   * @return Subscript corresponding to target
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOfKey(key: number): number;
  /**
   * Queries the index for a specified value
   * @param value Looking for goals
   * @return Subscript corresponding to target
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOfValue(value: T): number;
  /**
   * Checks whether the current PlainArray object is empty
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * Queries the key at a specified index
   * @param index Target subscript for search
   * @return the key of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getKeyAt(index: number): number;
  /**
   * Remove the key-value pair based on a specified key if it exists and return the value
   * @param key  Target to be deleted
   * @return Target mapped value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(key: number): T;
  /**
   * Remove the key-value pair at a specified index if it exists and return the value
   * @param index  Target subscript for search
   * @return the T type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeAt(index: number): T;
  /**
   * Remove a group of key-value pairs from a specified index
   * @param index  remove start index
   * @param size Expected deletion quantity
   * @return Actual deleted quantity
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeRangeFrom(index: number, size: number): number;
  /**
   * Update value on specified index
   * @param index Target subscript for search
   * @param value Updated the target mapped value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  setValueAt(index: number, value: T): void;
  /**
   * Obtains the string representation of the PlainArray object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  toString(): String;
  /**
   * Queries the value at a specified index
   * @param index Target subscript for search
   * @return the value of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getValueAt(index: number): T;
  /**
   * Executes a provided function once for each value in the PlainArray object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value: T, index?: number, PlainArray?: PlainArray<T>) => void,
  thisArg?: Object): void;
  /**
   * returns an iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<[number, T]>;
}

export default PlainArray;
