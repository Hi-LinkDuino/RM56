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
 * Declare the stepper.
 * @since 8
 */
interface StepperInterface {
  /**
   * Called when the stepper component is used.
   * @since 8
   */
  (value?: { index?: number }): StepperAttribute;
}

/**
 * Defines the stepper attribute functions
 * @since 8
 */
declare class StepperAttribute extends CommonMethod<StepperAttribute> {
  /**
   * Callback when the finish label is clicked.
   * @since 8
   */
  onFinish(callback: () => void): StepperAttribute;

  /**
   * Callback when the skip label is clicked.
   * @since 8
   */
  onSkip(callback: () => void): StepperAttribute;

  /**
   * Callback when the change label is clicked.
   * @since 8
   */
  onChange(callback: (prevIndex?: number, index?: number) => void): StepperAttribute;

  /**
   * Callback when the next label is clicked.
   * @since 8
   */
  onNext(callback: (index?: number, pendingIndex?: number) => void): StepperAttribute;

  /**
   * Callback when the previous label is clicked.
   * @since 8
   */
  onPrevious(callback: (index?: number, pendingIndex?: number) => void): StepperAttribute;
}

declare const Stepper: StepperInterface;
declare const StepperInstance: StepperAttribute;
