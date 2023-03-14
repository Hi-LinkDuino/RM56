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
 * Defines the ColumnSplit component.
 * @since 7
 */
interface ColumnSplitInterface {
  /**
   * Layout the subassemblies vertically and insert a horizontal divider line between each subassemblies.
   * @since 7
   */
  (): ColumnSplitAttribute;
}

/**
 * Defines the ColumnSplit component attribute functions.
 * @since 7
 */
declare class ColumnSplitAttribute extends CommonMethod<ColumnSplitAttribute> {
  /**
   * Indicates whether the split line can be dragged. The default value is false.
   * @since 7
   */
  resizeable(value: boolean): ColumnSplitAttribute;
}

declare const ColumnSplitInstance: ColumnSplitAttribute;
declare const ColumnSplit: ColumnSplitInterface;
