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
 * Provides the path drawing interface.
 * @since 7
 */
interface PathInterface {
  /**
   * Use new to create Path.
   * @since 7
   */
  new (value?: { width?: number | string; height?: number | string; commands?: string }): PathAttribute;

  /**
   * Called when drawing path.
   * @since 7
   */
  (value?: { width?: number | string; height?: number | string; commands?: string }): PathAttribute;
}

/**
 * @since 7
 */
declare class PathAttribute extends CommonShapeMethod<PathAttribute> {
  /**
   * Called when the command string drawn by the path is set.
   * @since 7
   */
  commands(value: string): PathAttribute;
}

declare const Path: PathInterface;
declare const PathInstance: PathAttribute;
