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
 * Naivagtion title mode.
 * @since 8
 */
declare enum NavigationTitleMode {
  /**
   * The title is free mode.
   * @since 8
   */
  Free = 0,

  /**
   * The title is full mode.
   * @since 8
   */
  Full,

  /**
   * The title is mini mode.
   * @since 8
   */
  Mini,
}

declare interface NavigationMenuItem {
  /**
   * The value of navigation menu item.
   * @since 8
   */
  value: string;
  /**
   * The icon of navigation menu item.
   * @since 8
   */
  icon?: string;
  /**
   * Trigger by navigation menu item click.
   * @since 8
   */
  action?: () => void;
}

/**
 * Provide navigator view interface
 * @since 8
 */
interface NavigationInterface {
  /**
   * Called when the navigator view interface is used.
   * @since 8
   */
  (): NavigationAttribute;
}

/**
 * Declare Navigation view properties.
 * @since 8
 */
declare class NavigationAttribute extends CommonMethod<NavigationAttribute> {
  /**
   * Navigation title
   * @since 8
   */
  title(value: string | CustomBuilder): NavigationAttribute;

  /**
   * Navigation subtitle
   * @since 8
   */
  subTitle(value: string): NavigationAttribute;

  /**
   * Hide navigation bar
   * @since 8
   */
  hideTitleBar(value: boolean): NavigationAttribute;

  /**
   * Hide navigation back button
   * @since 8
   */
  hideBackButton(value: boolean): NavigationAttribute;

  /**
   * Navigation title mode
   * @since 8
   */
  titleMode(value: NavigationTitleMode): NavigationAttribute;

  /**
   * Navigation title bar's menus
   * @since 8
   */
  menus(value: Array<NavigationMenuItem> | CustomBuilder): NavigationAttribute;

  /**
   * Tool bar
   * @since 8
   */
  toolBar(value: object | CustomBuilder): NavigationAttribute;

  /**
   * Hide tool bar
   * @since 8
   */
  hideToolBar(value: boolean): NavigationAttribute;

  /**
   * Trigger callback when title mode change finished at free mode.
   * @since 8
   */
  onTitleModeChange(callback: (titleMode: NavigationTitleMode) => void): NavigationAttribute;
}

declare const Navigation: NavigationInterface;
declare const NavigationInstance: NavigationAttribute;
