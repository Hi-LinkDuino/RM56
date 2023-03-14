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
 * Provides the interface for scoring bars.
 * @since 7
 */
interface RatingInterface {
  /**
   * Called when a score bar is created.
   * @since 7
   */
  (options?: { rating: number; indicator?: boolean }): RatingAttribute;
}

/**
 * Defines the rating attibute functions.
 * @since 7
 */
declare class RatingAttribute extends CommonMethod<RatingAttribute> {
  /**
   * Called when the total number of stars is set.
   * @since 7
   */
  stars(value: number): RatingAttribute;

  /**
   * Called when the step size of the operation rating.
   * @since 7
   */
  stepSize(value: number): RatingAttribute;

  /**
   * Called when a picture is set.
   * @since 7
   */
  starStyle(value: { backgroundUri: string; foregroundUri: string; secondaryUri?: string }): RatingAttribute;

  /**
   * Called when the star rating of the operation scoring bar changes.
   * @since 7
   */
  onChange(callback: (value: number) => void): RatingAttribute;
}

declare const Rating: RatingInterface;
declare const RatingInstance: RatingAttribute;
