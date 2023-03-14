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
 * Used to do matrix operations
 * @import import Matrix4 from '@ohos.matrix4'
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 7
 */
declare namespace matrix4 {
  /**
   * Set translation parameters
   * @since 7
   */
  interface TranslateOption {
    /**
     * Indicates the translation distance of the x-axis, in px.
     * @since 7
     */
    x?: number;

    /**
     * Indicates the translation distance of the y-axis, in px.
     * @since 7
     */
    y?: number;

    /**
     * Indicates the translation distance of the z-axis, in px.
     * @since 7
     */
    z?: number;
  }

  /**
   * Set scaling parameters
   * @since 7
   */
  interface ScaleOption {
    /**
     * Zoom factor of the x-axis.
     * @since 7
     */
    x?: number;

    /**
     * Zoom factor of the y-axis.
     * @since 7
     */
    y?: number;

    /**
     * Zoom factor of the z-axis.
     * @since 7
     */
    z?: number;

    /**
     * Transform the x-axis coordinate of the center point.
     * @since 7
     */
    centerX?: number;

    /**
     * Transform the y-axis coordinate of the center point.
     * @since 7
     */
    centerY?: number;
  }

  /**
   * Set Rotation Parameters.
   * @since 7
   */
  interface RotateOption {
    /**
     * Axis of rotation vector x coordinate.
     * @since 7
     */
    x?: number;

    /**
     * Axis of rotation vector y coordinate.
     * @since 7
     */
    y?: number;

    /**
     * Axis of rotation vector z coordinate.
     * @since 7
     */
    z?: number;

    /**
     * Transform the x-axis coordinate of the center point.
     * @since 7
     */
    centerX?: number;

    /**
     * Transform the y-axis coordinate of the center point.
     * @since 7
     */
    centerY?: number;

    /**
     * Rotation angle.
     * @since 7
     */
    angle?: number;
  }

  /**
   * Matrix4Transit.
   * @since 7
   */
  interface Matrix4Transit {
    /**
     * Copy function of Matrix, which can copy a copy of the current matrix object.
     * @since 7
     * @return Return to Matrix4Transit
     */
    copy(): Matrix4Transit;

    /**
     * The inverse function of Matrix returns an inverse matrix of the current matrix object, that is, the effect is exactly the opposite.
     * @since 7
     * @return Return to Matrix4Transit
     */
    invert(): Matrix4Transit;

    /**
     * Matrix superposition function, which can superpose the effects of two matrices to generate a new matrix object.
     * @since 7
     * @return Return to Matrix4Transit
     */
    combine(options: Matrix4Transit): Matrix4Transit;

    /**
     * Matrix translation function, which can add the x-axis, Y-axis, or Z-axis translation effect to the current matrix.
     * @since 7
     * @return Return to Matrix4Transit
     */
    translate(options: TranslateOption): Matrix4Transit;

    /**
     * Scaling function of the Matrix, which can add the x-axis, Y-axis, or Z-axis scaling effect to the current matrix.
     * @since 7
     * @return Return to Matrix4Transit
     */
    scale(options: ScaleOption): Matrix4Transit;

    /**
     * Rotation function of the Matrix. You can add the x-axis, Y-axis, or Z-axis rotation effect to the current matrix.
     * @since 7
     * @return Return to Matrix4Transit
     */
    rotate(options: RotateOption): Matrix4Transit;

    /**
     * Matrix coordinate point conversion function, which can apply the current transformation effect to a coordinate point.
     * @since 7
     * @return Return to Matrix4Transit
     */
    transformPoint(options: [number, number]): [number, number];
  }

  /**
   * Constructor of Matrix, which can create a fourth-order matrix based on the input parameters. The matrix is column-first.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function init(
    options: [
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
      number,
    ],
  ): Matrix4Transit;

  /**
   * Matrix initialization function, which can return an identity matrix object.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function identity(): Matrix4Transit;

  /**
   * Copy function of Matrix, which can copy a copy of the current matrix object.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function copy(): Matrix4Transit;

  /**
   * The inverse function of Matrix returns an inverse matrix of the current matrix object, that is, the effect is exactly the opposite.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function invert(): Matrix4Transit;

  /**
   * Matrix superposition function, which can superpose the effects of two matrices to generate a new matrix object.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function combine(options: Matrix4Transit): Matrix4Transit;

  /**
   * Matrix translation function, which can add the x-axis, Y-axis, or Z-axis translation effect to the current matrix.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function translate(options: TranslateOption): Matrix4Transit;

  /**
   * Scaling function of the Matrix, which can add the x-axis, Y-axis, or Z-axis scaling effect to the current matrix.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function scale(options: ScaleOption): Matrix4Transit;

  /**
   * Rotation function of the Matrix. You can add the x-axis, Y-axis, or Z-axis rotation effect to the current matrix.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function rotate(options: RotateOption): Matrix4Transit;

  /**
   * Matrix coordinate point conversion function, which can apply the current transformation effect to a coordinate point.
   * @since 7
   * @return Return to Matrix4Transit
   */
  function transformPoint(options: [number, number]): [number, number];
}

export default matrix4;
