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
 * Load style of progress bar.
 * @since 8
 */
declare enum LoadingProgressStyle {
  /**
   * Default style.
   * @since 8
   */
  Default,

  /**
   * Announcement style.
   * @since 8
   */
  Circular,

  /**
   * The style of the track.
   * @since 8
   */
  Orbital,
}

/**
 * Provides an interface for extending the loading progress.
 * @since 8
 */
interface LoadingProgressInterface {
  /**
   * Called when the progress bar progress is viewed.
   * @since 8
   */
  (): LoadingProgressAttribute;
}

/**
 * Declare the progress bar being loaded
 * @since 8
 */
declare class LoadingProgressAttribute extends CommonMethod<LoadingProgressAttribute> {
  /**
   * Load the color of the progress bar.
   * @since 8
   */
  color(value: ResourceColor): LoadingProgressAttribute;
}

/**
 * @since 8
 */
declare const LoadingProgress: LoadingProgressInterface;
/**
 * Loading Progress Extensions on Declarative Classes
 * @since 8
 */
declare const LoadingProgressInstance: LoadingProgressAttribute;
