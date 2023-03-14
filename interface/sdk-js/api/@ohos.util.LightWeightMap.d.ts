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
declare class LightWeightMap<K, V> {
  /**
   * A constructor used to create a LightWeightMap object.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  constructor();
  /**
   * Gets the element number of the LightWeightMap.
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  length: number;
  /**
   * Returns whether this map has all the object in a specified map
   * @param map the Map object to compare
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  hasAll(map: LightWeightMap<K, V>): boolean;
  /**
   * Returns whether a key is contained in this map
   * @param key need to determine whether to include the key
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  hasKey(key: K): boolean;
  /**
   * Returns whether a value is contained in this map
   * @param value need to determine whether to include the value
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  hasValue(value: V): boolean;
  /**
   * Ensures that the capacity of an LightWeightMap container is greater than or equal to a apecified value,
   * and that the container has all the original objects after capacity expansion
   * @param minimumCapacity Minimum capacity to be reserved
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  increaseCapacityTo(minimumCapacity: number): void;
  /**
   * Returns a new Iterator object that contains the [key, value] pairs for each element in the Map object in insertion order
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  entries(): IterableIterator<[K, V]>;
  /**
   * Returns the value to which the specified key is mapped, or undefined if this map contains no mapping for the key
   * @param key the index in LightWeightMap
   * @return value or undefined
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  get(key: K): V;
  /**
   * Obtains the index of the key equal to a specified key in an LightWeightMap container
   * @param key Looking for goals
   * @return Subscript corresponding to target
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOfKey(key: K): number;
  /**
   * Obtains the index of the value equal to a specified value in an LightWeightMap container
   * @param value Looking for goals
   * @return Subscript corresponding to target
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getIndexOfValue(value: V): number;
  /**
   * Returns whether the Map object contains elements
   * @return the boolean type
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  isEmpty(): boolean;
  /**
   * Obtains the key at the loaction identified by index in an LightWeightMap container
   * @param index Target subscript for search
   * @return the key of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getKeyAt(index: number): K;
  /**
   * Obtains a ES6 iterator that contains all the keys of an LightWeightMap container
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  keys(): IterableIterator<K>;
  /**
   * Adds all element groups in one map to another map
   * @param map the Map object to add members
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  setAll(map: LightWeightMap<K, V>): void;
  /**
   * Adds or updates a(new) key-value pair with a key and value specified for the Map object
   * @param key Added or updated targets
   * @param value Added or updated value
   * @returns the map object after set
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  set(key: K, value: V): Object;
  /**
   * Remove the mapping for this key from this map if present
   * @param key  Target to be deleted
   * @return Target mapped value
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  remove(key: K): V;
  /**
   * Deletes a key-value pair at the loaction identified by index from an LightWeightMap container
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
   * Sets the value identified by index in an LightWeightMap container to a specified value
   * @param index Target subscript for search
   * @param value Updated the target mapped value
   * @return the boolean type(Is there a value corresponding to the subscript)
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  setValueAt(index: number, newValue: V): boolean;
  /**
   * Executes the given callback function once for each real key in the map.
   * It does not perform functions on deleted keys
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  forEach(callbackfn: (value?: V, key?: K, map?: LightWeightMap<K, V>) => void,
  thisArg?: Object): void;
  /**
   * returns an ES6 iterator.Each item of the iterator is a Javascript Object
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  [Symbol.iterator](): IterableIterator<[K, V]>;
  /**
   * Obtains a string that contains all the keys and values in an LightWeightMap container
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  toString(): String;
  /**
   * Obtains the value identified by index in an LightWeightMap container
   * @param index Target subscript for search
   * @return the value of key-value pairs
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  getValueAt(index: number): V;
  /**
   * Returns an iterator of the values contained in this map
   * @since 8
   * @syscap SystemCapability.Utils.Lang
   */
  values(): IterableIterator<V>;
}

export default LightWeightMap;