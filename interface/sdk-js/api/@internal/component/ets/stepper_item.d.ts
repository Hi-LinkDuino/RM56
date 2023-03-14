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
 * ItemState
 * @since 8
 */
declare enum ItemState {
  /**
   * Default State
   * @since 8
   */
  Normal,

  /**
   * Disabled State
   * @since 8
   */
  Disabled,

  /**
   * Waiting State
   * @since 8
   */
  Waiting,

  /**
   * Skip State
   * @since 8
   */
  Skip,
}

/**
 * Provides an interface for switching the stepperItem view on stepper container.
 * @since 8
 */
interface StepperItemInterface {
  /**
   * Called when the stepperItem component is used.
   * @since 8
   */
  (): StepperItemAttribute;
}

/**
 * Defines the stepper item attrbute functions.
 * @since 8
 */
declare class StepperItemAttribute extends CommonMethod<StepperItemAttribute> {
  /**
   * Called when the value of stepperItem prevLabel is set
   * @since 8
   */
  prevLabel(value: string): StepperItemAttribute;

  /**
   * Called when the value of stepperItem nextLabel is set
   * @since 8
   */
  nextLabel(value: string): StepperItemAttribute;

  /**
   * Called when the value of stepperItem status is set
   * @since 8
   */
  status(value?: ItemState): StepperItemAttribute;
}

declare const StepperItemInstance: StepperItemAttribute;
declare const StepperItem: StepperItemInterface;
