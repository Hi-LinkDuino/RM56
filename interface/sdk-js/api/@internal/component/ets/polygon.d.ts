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
 * Provides the polygon drawing interface.
 * @since 7
 */
interface PolygonInterface{
  /**
   * Uses new to create Polygon.
   * @since 7
   */
  new (value?: { width?: string | number; height?: string | number }): PolygonAttribute;

  /**
   * Called when drawing a polygon.
   * @since 7
   */
  (value?: { width?: string | number; height?: string | number }): PolygonAttribute;
}

declare class PolygonAttribute extends CommonShapeMethod<PolygonAttribute> {
  /**
   * Called when the vertex coordinate list of a polygon is set.
   * @since 7
   */
  points(value: Array<any>): PolygonAttribute;
}

declare const Polygon: PolygonInterface;
declare const PolygonInstance: PolygonAttribute;
