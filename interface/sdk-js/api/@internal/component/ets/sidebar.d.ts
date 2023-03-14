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
 * Sets the sidebar style of showing
 * @since 8
 */
declare enum SideBarContainerType {
  /**
   * The sidebar invisible
   * @since 8
   */
  Embed,
  /**
   * The sidebar visible
   * @since 8
   */
  Overlay,
}

/**
 * Sets the control button style
 * @since 8
 */
declare interface ButtonStyle {
  /**
   * Set the left of control button
   * @since 8
   */
  left?: number;
  /**
   * Set the top of control button
   * @since 8
   */
  top?: number;
  /**
   * Set the width of control button
   * @since 8
   */
  width?: number;

  /**
   * Set the heigth of control button
   * @since 8
   */
  height?: number;

  /**
   * Set the button icon when sidebar status has changed
   * @since 8
   */
  icons?: {
    shown: string | PixelMap | Resource;
    hidden: string | PixelMap | Resource;
    switching?: string | PixelMap | Resource;
  };
}

/**
 * The construct function of sidebar
 * @since 8
 */
interface SideBarContainerInterface {
  /**
   * Called when showing the sidebar of a block entry.
   * @since 8
   */
  (type?: SideBarContainerType): SideBarContainerAttribute;
}

/**
 * The attribute function of sidebar
 * @since 8
 */
declare class SideBarContainerAttribute extends CommonMethod<SideBarContainerAttribute> {
  /**
   * Callback showControlButton function when setting the status of sidebar
   * @since 8
   */
  showSideBar(value: boolean): SideBarContainerAttribute;
  /**
   * Callback controlButton function when setting the style of button
   * @since 8
   */
  controlButton(value: ButtonStyle): SideBarContainerAttribute;
  /**
   * Callback showControlButton function when setting the status of button
   * @since 8
   */
  showControlButton(value: boolean): SideBarContainerAttribute;
  /**
   * Trigger callback when sidebar style of showing change finished.
   * @since 8
   */
  onChange(callback: (value: boolean) => void): SideBarContainerAttribute;
  /**
   * Sets the length of sidebar.
   * @since 8
   */
  sideBarWidth(value: number): SideBarContainerAttribute;
  /**
   * Sets the min length of sidebar.
   * default value is 200vp.
   * @since 8
   */
  minSideBarWidth(value: number): SideBarContainerAttribute;
  /**
   * Sets the max length of sidebar.
   * default value is 280vp.
   * @since 8
   */
  maxSideBarWidth(value: number): SideBarContainerAttribute;
}

declare const SideBarContainer: SideBarContainerInterface;
declare const SideBarContainerInstance: SideBarContainerAttribute;
