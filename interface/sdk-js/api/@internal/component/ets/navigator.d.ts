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
 * Route jump.
 * @since 7
 */
declare enum NavigationType {
  /**
   * Jump to the next page.
   * @since 7
   */
  Push,

  /**
   * Return to the previous page.
   * @since 7
   */
  Back,

  /**
   * Replace page.
   * @since 7
   */
  Replace,
}

/**
 * Create route
 * @since 7
 */
interface NavigatorInterface {
  /**
   * Called when the route jumps.
   * @since 7
   */
  (value?: { target: string; type?: NavigationType }): NavigatorAttribute;

  /**
   * Called when using the navigator.
   * @since 7
   */
  (): NavigatorAttribute;
}

/**
 * Declare navigator properties.
 * @since 7
 */
declare class NavigatorAttribute extends CommonMethod<NavigatorAttribute> {
  /**
   * Called when determining whether the routing component is active.
   * @since 7
   */
  active(value: boolean): NavigatorAttribute;

  /**
   * Called when determining whether the routing component is active.
   * @since 7
   */
  type(value: NavigationType): NavigatorAttribute;

  /**
   * Called when the path to the specified jump target page is set.
   * @since 7
   */
  target(value: string): NavigatorAttribute;

  /**
   * Called when data is passed to the target page at the same time during jump.
   * @since 7
   */
  params(value: object): NavigatorAttribute;
}

declare const Navigator: NavigatorInterface;
declare const NavigatorInstance: NavigatorAttribute;
