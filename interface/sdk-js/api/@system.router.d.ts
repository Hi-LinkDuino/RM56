/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * @author yongxianglai@bestechnic.com
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * 转场动画参数字段
 */
export interface BesTransitionAnimOptions {
  /** 动画名称 */
  animName?: "translateX" | "translateY" | "opacity" | "scale";

  /** 动画数据，数据数量和定义由 animName 字段决定 */
  animValues: [number, number, number, number];

  /** 动画持续时长，单位为毫秒 */
  animDuration?: number;

  /** 动画（时间曲线）插值器 */
  animInterpolation?: "linear" | "ease_in" | "ease_out" | "ease_in_out";

  /** 动画淡入淡出类型 */
  animFade?: "fade-in" | "fade_out" | "none";

  /** 动画基准位置 */
  animPivot?: "center" | "left_center" | "right_center" | "top_center" | "bottom_center" | "left_top" | "right_top" | "left_bottom" | "right_bottom";

  /** 动画执行的延时时长，单位为毫秒 */
  animDelay?: number;
}

/**
 * @author yongxianglai@bestechnic.com
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * 页面转场动画字段
 */
export interface BesTransitionOptions {
  /** 预置的转场方案。当存在此字段，其他字段都将被忽略 */
  animator?: "dealer" | "fade" 
    | "translate-forward" | "translate-backward" | "translate-fall" | "translate-rise"
    | "zoom-in" | "zoom-out" 
    | "stuff" | "overflow" 
    | "scene-x" | "scene-y"
    | "crt-on" | "crt-off" 
    | "expand-lt" | "expand-rt" | "expand-lb"| "expand-rb"
    | "shrink-lt" | "shrink-rt" | "shrink-lb" | "shrink-rb"
    | "occupied-lr" | "occupied-rl" | "occupied-tb" | "occupied-bt"
    | "expulsion-lr" | "expulsion-rl";

  /** 转场动作强调。被强调的转场动作将置于最顶层，以防止被其他动作遮挡 */
  perfer?: "enter" | "exit";

  /** 入场 & 退场动画名称 */
  animName?: "translateX" | "translateY" | "opacity" | "scale";

  /** 页面转场时长，单位为毫秒，取值范围为 [100, 60000] */
  animDuration?: number;

  /** 入场 & 退场动画的时间（时间曲线）估值器 */
  animInterpolation?: "ease_in" | "ease_out" | "ease_in_out";

  /** 页面的入场动画字段 */
  enter?: BesTransitionAnimOptions;

  /** 页面的退场动画字段 */
  exit?: BesTransitionAnimOptions;
}

/**
 * Defines the option of router.
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export interface RouterOptions {
  /**
   * URI of the destination page, which supports the following formats:
   * 1. Absolute path of the page, which is provided by the pages list in the config.json file.
   *    Example:
   *      pages/index/index
   *      pages/detail/detail
   * 2. Particular path. If the URI is a slash (/), the home page is displayed.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  uri: string;

  /**
   * Data that needs to be passed to the destination page during navigation.
   * After the destination page is displayed, the parameter can be directly used for the page.
   * For example, this.data1 (data1 is the key value of the params used for page navigation.)
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  params?: Object;

  /**
   * @date 2023年2月15日
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   *
   * 页面转场参数，可作用于当前页面和目标页面
   */
  besTransition?: BesTransitionOptions;
}

/**
 * Defines the option of router back.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 7
 */
export interface BackRouterOptions {
  /**
   * Returns to the page of the specified path.
   * If the page with the specified path does not exist in the page stack, router.back() is called by default.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 7
   */
  uri?: string;

  /**
   * Data that needs to be passed to the destination page during navigation.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 7
   */
  params?: Object;
}

/**
 * Defines the state of router.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export interface RouterState {
  /**
   * Index of the current page in the stack.
   * NOTE: The index starts from 1 from the bottom to the top of the stack.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  index: number;

  /**
   * Name of the current page, that is, the file name.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  name: string;

  /**
   * Path of the current page.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  path: string;
}

/**
 * Defines the option of EnableAlertBeforeBackPage.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 6
 */
export interface EnableAlertBeforeBackPageOptions {
  /**
   * dialog context.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  message: string;

  /**
   * Called when the dialog box is displayed.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  success?: (errMsg: string) => void;

  /**
   * Called when the operation is cancelled.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  cancel?: (errMsg: string) => void;

  /**
   * Called when the dialog box is closed.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  complete?: () => void;
}

/**
 * Defines the option of DisableAlertBeforeBackPage.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 6
 */
export interface DisableAlertBeforeBackPageOptions {
  /**
   * Called when the dialog box is displayed.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  success?: (errMsg: string) => void;

  /**
   * Called when the operation is cancelled.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  cancel?: (errMsg: string) => void;

  /**
   * Called when the dialog box is closed.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  complete?: () => void;
}

type ParamsInterface = {
  [key: string]: Object;
};

/**
 * Defines the Router interface.
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export default class Router {
  /**
   * Navigates to a specified page in the application based on the page URL and parameters.
   * @param options Options.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  static push(options: RouterOptions): void;

  /**
   * Replaces the current page with another one in the application. The current page is destroyed after replacement.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @param options Options.
   * @since 3
   */
  static replace(options: RouterOptions): void;

  /**
   * Returns to the previous page or a specified page.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param options Options.
   * @since 3
   */
  static back(options?: BackRouterOptions): void;

  /**
   * Obtains information about the current page params.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @returns Page params.
   * @since 7
   */
  static getParams(): ParamsInterface;

  /**
   * Clears all historical pages and retains only the current page at the top of the stack.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  static clear(): void;

  /**
   * Obtains the number of pages in the current stack.
   * @returns Number of pages in the stack. The maximum value is 32.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  static getLength(): string;

  /**
   * Obtains information about the current page state.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @returns Page state.
   * @since 3
   */
  static getState(): RouterState;

  /**
   * Pop up dialog to ask whether to back
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param options Options.
   * @since 6
   */
  static enableAlertBeforeBackPage(options: EnableAlertBeforeBackPageOptions): void;

  /**
   * cancel enableAlertBeforeBackPage
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param options Options.
   * @since 6
   */
  static disableAlertBeforeBackPage(options?: DisableAlertBeforeBackPageOptions): void;
}
