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
 * Declare item ceiling attribute.
 * @since 7
 */
declare enum Sticky {
  /**
   * No sticky.
   * @since 7
   */
  None,

  /**
   * Normal mode
   * @since 7
   */
  Normal,

  /**
   * Set opacity.
   * @since 7
   */
  Opacity,
}

/**
 * Declare whether the ListItem element is editable.
 * @since 7
 */
declare enum EditMode {
  /**
   * Unrestricted operations.
   * @since 7
   */
  None,

  /**
   * Deleteable.
   * @since 7
   */
  Deletable,

  /**
   * Movable.
   * @since 7
   */
  Movable,
}

/**
 * @since 7
 */

/**
 * Values in the list
 * @since 7
 */
interface ListItemInterface {
  /**
   * Called when an interface is used.
   * @since 7
   */
  (value?: string): ListItemAttribute;
}

/**
 * @since 7
 */
declare class ListItemAttribute extends CommonMethod<ListItemAttribute> {
  /**
   * Called when setting whether item is ceiling effect.
   * @since 7
   */
  sticky(value: Sticky): ListItemAttribute;

  /**
   * Called when judging whether it is editable.
   * @since 7
   */
  editable(value: boolean | EditMode): ListItemAttribute;

  /**
   * Called when judging whether it is selectable.
   * @since 8
   */
  selectable(value: boolean): ListItemAttribute;

  /**
   * Called when the listItem is selected.
   * @since 8
   */
  onSelect(event: (isSelected: boolean) => void): ListItemAttribute;
}

/**
 * @since 7
 */
declare const ListItemInstance: ListItemAttribute;
declare const ListItem: ListItemInterface;
