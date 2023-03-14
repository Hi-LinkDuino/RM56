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
declare class LightWeightSet<T> {
  /**
   * A constructor used to create a LightWeightSet object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the LightWeightSet.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * If the set does not contain the element, the specified element is added
   * @param value Added element
   * @returns the boolean type(Is there contain this element)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  add(obj: T): boolean;
  /**
   * Adds all the objects in a specified LightWeightSet container to the current LightWeightSet container
   * @param set the Set object to provide the added element
   * @returns the boolean type(Is there any new data added successfully)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  addAll(set: LightWeightSet<T>): boolean;
  /**
   * Returns whether this set has all the object in a specified set
   * @param set the Set object to compare
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  hasAll(set: LightWeightSet<T>): boolean;
  /**
   * Checks whether an LightWeightSet container has a specified key
   * @param key need to determine whether to include the key
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  has(key: T): boolean;
  /**
   * Checks whether an the objects of an LightWeighSet containeer are of the same type as a specified Object LightWeightSet
   * @param obj need to determine whether to include the obj
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  equal(obj: Object): boolean;
  /**
   * Ensures that the capacity of an LightWeightSet container is greater than or equal to a apecified value,
   * and that the container has all the original objects after capacity expansion
   * @param minimumCapacity Minimum capacity to be reserved
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  increaseCapacityTo(minimumCapacity: number): void;
  /**
   * Obtains the index of s key of a specified Object type in an LightWeightSet container
   * @param key Looking for goals
   * @return Subscript corresponding to target
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOf(key: T): number;
  /**
   * Deletes an object of a specified Object type from an LightWeightSet container
   * @param key  Target to be deleted
   * @return Target element
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(key: T): T;
  /**
   * Deletes an object at the loaction identified by index from an LightWeightSet container
   * @param index  Target subscript for search
   * @return the boolean type(Is there a delete value)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  removeAt(index: number): boolean;
  /**
   * Removes all of the mapping from this map
   * The map will be empty after this call returns
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  clear(): void;
  /**
   * Executes the given callback function once for each real key in the map.
   * It does not perform functions on deleted keys
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value?: T, key?: T, set?: LightWeightSet<T>) => void,
  thisArg?: Object): void;
  /**
   * returns an ES6 iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<T>;
  /**
   * Obtains a string that contains all the keys and values in an LightWeightSet container
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  toString(): String;
  /**
   * Obtains an Array that contains all the objects of an LightWeightSet container.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  toArray(): Array<T>;
  /**
   * Obtains the object at the location  identified by index in an LightWeightSet container
   * @param index Target subscript for search
   * @return the value of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getValueAt(index: number): T;
  /**
   * Returns a ES6 iterator of the values contained in this Set
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  values(): IterableIterator<T>;
  /**
   * Returns a Iterator object that contains the [key, value] pairs for each element in the Set object in insertion order
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  entries(): IterableIterator<[T, T]>;
  /**
   * Returns whether the set object contains elements
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
}

export default LightWeightSet;
