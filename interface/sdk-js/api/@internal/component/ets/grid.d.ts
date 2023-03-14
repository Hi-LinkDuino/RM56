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
 * Defines the grid interface.
 * @since 7
 */
interface GridInterface {
  /**
   * Grid is returned when the parameter is transferred.
   * @since 7
   */
  (scroller?: Scroller): GridAttribute;
}

/**
 * The enum of property layoutDirection
 * @since 8
 */
declare enum GridDirection {
  /**
   * The row direction.
   * @since 8
   */
  Row,
  /**
   * The column direction.
   * @since 8
   */
  Column,
  /**
   * The row reverse direction.
   * @since 8
   */
  RowReverse,
  /**
   * The column reverse direction.
   * @since 8
   */
  ColumnReverse,
}

/**
 * Defines the grid attibute functions.
 * @since 7
 */
declare class GridAttribute extends CommonMethod<GridAttribute> {
  /**
   * This parameter specifies the number of columns in the current grid layout.
   * @since 7
   */
  columnsTemplate(value: string): GridAttribute;

  /**
   * Lets you set the number of rows in the current grid layout,
   * @since 7
   */
  rowsTemplate(value: string): GridAttribute;

  /**
   * Allows you to set the spacing between columns.
   * @since 7
   */
  columnsGap(value: Length): GridAttribute;

  /**
   * Lets you set the spacing between rows.
   * @since 7
   */
  rowsGap(value: Length): GridAttribute;

  /**
   * This parameter specifies the width of the scroll bar.
   * @since 7
   */
  scrollBarWidth(value: number | string): GridAttribute;

  /**
   * Sets the color of the scroll bar.
   * @since 7
   */
  scrollBarColor(value: Color | number | string): GridAttribute;

  /**
   * Lets you set the spacing between rows.
   * @since 7
   */
  scrollBar(value: BarState): GridAttribute;

  /**
   * Sets the status of the scroll bar.
   * @since 7
   */
  onScrollIndex(event: (first: number) => void): GridAttribute;

  /**
   * cached Count
   * @since 7
   */
  cachedCount(value: number): GridAttribute;

  /**
   * editMode
   * @since 8
   */
  editMode(value: boolean): GridAttribute;

  /**
   * Called when judging whether it is multiSelectable.
   * @since 8
   */
  multiSelectable(value: boolean): GridAttribute;

  /**
   * maxCount
   * @since 8
   */
  maxCount(value: number): GridAttribute;

  /**
   * minCount
   * @since 8
   */
  minCount(value: number): GridAttribute;

  /**
   * cellLength
   * @since 8
   */
  cellLength(value: number): GridAttribute;

  /**
   * control GridDirection of the grid.
   * @since 8
   */
  layoutDirection(value: GridDirection): GridAttribute;

  /**
   * control if the grid supports animation.
   * @since 8
   */
  supportAnimation(value: boolean): GridAttribute;

  /**
   * After a listener is bound, the component can be dragged. After the drag occurs, a callback is triggered.
   * (To be triggered, press and hold for 170 milliseconds (ms))
   * @since 8
   */
  onItemDragStart(event: (event: ItemDragInfo, itemIndex: number) => (() => any) | void): GridAttribute;

  /**
   * After binding, a callback is triggered when the component is dragged to the range of the component.
   * @since 8
   */
  onItemDragEnter(event: (event: ItemDragInfo) => void): GridAttribute;

  /**
   * After binding, a callback is triggered when the drag moves within the range of a placeable component.
   * @since 8
   */
  onItemDragMove(event: (event: ItemDragInfo, itemIndex: number, insertIndex: number) => void): GridAttribute;

  /**
   * After binding, a callback is triggered when the component is dragged out of the component range.
   * @since 8
   */
  onItemDragLeave(event: (event: ItemDragInfo, itemIndex: number) => void): GridAttribute;

  /**
   * The component bound to this event can be used as the drag release target.
   * This callback is triggered when the drag behavior is stopped within the scope of the component.
   * @since 8
   */
  onItemDrop(
    event: (event: ItemDragInfo, itemIndex: number, insertIndex: number, isSuccess: boolean) => void,
  ): GridAttribute;
}

declare const Grid: GridInterface;
declare const GridInstance: GridAttribute;
