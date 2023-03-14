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
 * Declare the jump method.
 * @since 7
 */
declare enum RouteType {
  /**
   * The page is not redirected.
   * @since 7
   */
  None,
  /**
   * Go to the next page.
   * @since 7
   */
  Push,
  /**
   * Redirect to a specified page.
   * @since 7
   */
  Pop,
}

/**
 * Declare the sliding effect of transition.
 * @since 7
 */
declare enum SlideEffect {
  /**
   * Swipe left.
   * @since 7
   */
  Left,

  /**
   * Swipe right.
   * @since 7
   */
  Right,

  /**
   * Swipe top.
   * @since 7
   */
  Top,

  /**
   * Swipe bottom.
   * @since 7
   */
  Bottom,
}

/**
 * Provides interfaces for common transitions.
 * @since 7
 */
declare class CommonTransition<T> {
  /**
   * Called when a transition method is required.
   * @since 7
   */
  constructor();
  /**
   * Called when the slide in effect of the transition is set.
   * @since 7
   */
  slide(value: SlideEffect): T;

  /**
   * Called when the translation effect of page transition is set.
   * @since 7
   */
  translate(value: { x?: number | string; y?: number | string; z?: number | string }): T;

  /**
   * Called when setting the zoom effect of page transition.
   * @since 7
   */
  scale(value: { x?: number; y?: number; z?: number; centerX?: number | string; centerY?: number | string }): T;

  /**
   * Called when the transparency value of the starting point of entry or the ending point of exit is set.
   * @since 7
   */
  opacity(value: number): T;
}

/**
 * Provides an interface for page rotation mode.
 * @since 7
 */
interface PageTransitionEnterInterface extends CommonTransition<PageTransitionEnterInterface> {
  /**
   * Called when page Jump animation is used.
   * @since 7
   */
  (value: { type?: RouteType; duration?: number; curve?: Curve | string; delay?: number }): PageTransitionEnterInterface;

  /**
   * Called when the incoming parameter is the normalized progress of the current incoming animation.
   * @since 7
   */
  onEnter(event: (type?: RouteType, progress?: number) => void): PageTransitionEnterInterface;
}

/**
 * Provide an interface to exit the transition.
 * @since 7
 */
interface PageTransitionExitInterface extends CommonTransition<PageTransitionExitInterface> {
  /**
   * Called when the transition is delayed.
   * @since 7
   */
  (value: { type?: RouteType; duration?: number; curve?: Curve | string; delay?: number }): PageTransitionExitInterface;

  /**
   * Called when the input parameter is the normalized progress of the current exit animation.
   * @since 7
   */
  onExit(event: (type?: RouteType, progress?: number) => void): PageTransitionExitInterface;
}

declare const PageTransitionEnter: PageTransitionEnterInterface;
declare const PageTransitionExit: PageTransitionExitInterface;
