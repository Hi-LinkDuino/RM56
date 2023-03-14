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
 * Defines XComponentController
 * @since 8
 */
declare class XComponentController {
  /**
   * constructor.
   * @since 8
   */
  constructor();

  /**
   * get the id of surface created by XComponent.
   * @since 8
   * @systemapi
   */
  getXComponentSurfaceId();

  /**
   * get the context of native XComponent.
   * @since 8
   */
  getXComponentContext();

  /**
   * set the surface size created by XComponent.
   * @since 8
   * @systemapi
   */
  setXComponentSurfaceSize(value: {
    surfaceWidth: number;
    surfaceHeight: number;
  });
}

/**
 * Defines XComponent.
 * @since 8
 */
interface XComponentInterface {
  /**
   * Constructor parameters
   * @since 8
   */
  (value: { id: string; type: string; libraryname?: string; controller?: XComponentController }): XComponentAttribute;
}

/**
 * Defines XComponentAttribute.
 * @since 8
 */
declare class XComponentAttribute extends CommonMethod<XComponentAttribute> {
  /**
   * Called when judging whether the xcomponent surface is created.
   * @since 8
   */
  onLoad(callback: (event?: object) => void): XComponentAttribute;

  /**
   * Called when judging whether the xcomponent is destroyed.
   * @since 8
   */
  onDestroy(event: () => void): XComponentAttribute;
}

declare const XComponent: XComponentInterface;
declare const XComponentInstance: XComponentAttribute;
