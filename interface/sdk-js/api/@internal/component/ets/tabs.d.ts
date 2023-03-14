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
 * Declare the graphic format of the bar chart.
 * @since 7
 */
declare enum BarMode {
  /**
   * The actual layout width of the TabBar is used. If the width exceeds the total width, you can slide the tabbar.
   * @since 7
   */
  Scrollable,

  /**
   * The width of all TabBars is evenly allocated.
   * @since 7
   */
  Fixed,
}

/**
 * Declare the location of the bar chart.
 * @since 7
 */
declare enum BarPosition {
  /**
   * When the vertical attribute method is set to true, the tab is on the left of the container. When the vertical property method is set to false, the tab is at the top of the container.
   * @since 7
   */
  Start,

  /**
   * When the vertical attribute method is set to true, the tab is located on the right of the container. When the vertical property method is set to false, the tab is at the bottom of the container.
   * @since 7
   */
  End,
}

/**
 * @since 7
 */
declare class TabsController {
  /**
   * constructor.
   * @since 7
   */
  constructor();

  /**
   * Called when the tab is switched.
   * @since 7
   */
  changeIndex(value: number): void;
}

/**
 * Provides an interface for switching views.
 * @since 7
 */
interface TabsInterface {
  /**
   * Called when the view is switched.
   * @since 7
   */
  (value?: { barPosition?: BarPosition; index?: number; controller?: TabsController }): TabsAttribute;
}

/**
 * Defines the tabs attribute functions.
 * @since 7
 */
declare class TabsAttribute extends CommonMethod<TabsAttribute> {
  /**
   * Called when determining whether the tab is vertical.
   * @since 7
   */
  vertical(value: boolean): TabsAttribute;

  /**
   * Called when judging whether page switching can be performed by sliding left and right.
   * @since 7
   */
  scrollable(value: boolean): TabsAttribute;

  /**
   * Called when the graphic format of the bar chart is selected.
   * @since 7
   */
  barMode(value: BarMode): TabsAttribute;

  /**
   * Called when the width of the bar graph is set.
   * Notice: barWidth only supports Number type on 7, supports Length type since 8.
   * @since 8
   */
  barWidth(value: Length): TabsAttribute;

  /**
   * Called when the height of the bar graph is set.
   * Notice: barHeight only supports Number type on 7, supports Length type since 8.
   * @since 8
   */
  barHeight(value: Length): TabsAttribute;

  /**
   * Called when the animation duration of the bar graph is set.
   * @since 7
   */
  animationDuration(value: number): TabsAttribute;

  /**
   * Called when the tab is switched.
   * @since 7
   */
  onChange(event: (index: number) => void): TabsAttribute;
}

declare const Tabs: TabsInterface;
declare const TabsInstance: TabsAttribute;
