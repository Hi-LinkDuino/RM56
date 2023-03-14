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

/**
 * Data Change Listener.
 * @since 7
 */
declare interface DataChangeListener {
  /**
   * Data ready.
   * @since 7
   */
  onDataReloaded(): void;

  /**
   * Data added.
   * @since 7
   * @deprecated since 8
   */
  onDataAdded(index: number): void;

  /**
   * Data added.
   * @since 8
   */
  onDataAdd(index: number): void;

  /**
   * Data moved.
   * @since 7
   * @deprecated since 8
   */
  onDataMoved(from: number, to: number): void;

  /**
   * Data moved.
   * @since 8
   */
  onDataMove(from: number, to: number): void;

  /**
   * Data deleted.
   * @since 7
   * @deprecated since 8
   */
  onDataDeleted(index: number): void;

  /**
   * Data deleted.
   * @since 8
   */
  onDataDelete(index: number): void;

  /**
   * Call when has data change.
   * @since 7
   * @deprecated since 8
   */
  onDataChanged(index: number): void;

  /**
   * Call when has data change.
   * @since 8
   */
  onDataChange(index: number): void;
}

/**
 * Developers need to implement this interface to provide data to LazyForEach component.
 * @since 7
 */
declare interface IDataSource {
  /**
   * Total data count.
   * @since 7
   */
  totalCount(): number;

  /**
   * Return the data of index.
   * @since 7
   */
  getData(index: number): any;

  /**
   * Register data change listener.
   * @since 7
   */
  registerDataChangeListener(listener: DataChangeListener): void;

  /**
   * Unregister data change listener.
   * @since 7
   */
  unregisterDataChangeListener(listener: DataChangeListener): void;
}

/**
 * Lazy loading.
 * @since 7
 */
interface LazyForEachInterface {
  /**
   * Enter the value to obtain the LazyForEach.
   * @since 7
   */
  (
    dataSource: IDataSource,
    itemGenerator: (item: any, index?: number) => void,
    keyGenerator?: (item: any, index?: number) => string,
  ): LazyForEachInterface;
}

declare const LazyForEach: LazyForEachInterface;
