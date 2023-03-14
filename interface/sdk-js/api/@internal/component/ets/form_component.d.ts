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
 * Defines the FormDimension enum.
 * @since 7
 * @systemapi
 */
declare enum FormDimension {
  /**
   * 1 x 2 cards
   * @since 7
   * @systemapi
   */
  Dimension_1_2,

  /**
   * 2 x 2 cards
   * @since 7
   * @systemapi
   */
  Dimension_2_2,

  /**
   * 2 x 4 cards
   * @since 7
   * @systemapi
   */
  Dimension_2_4,

  /**
   * 4 x 4 cards
   * @since 7
   * @systemapi
   */
  Dimension_4_4,
}

/**
 * Defines the FormComponent.
 * @since 7
 * @systemapi
 */
interface FormComponentInterface {
  /**
   * Set a new value.
   * @since 7
   * @systemapi
   */
  (value: {
    id: number;
    name: string;
    bundle: string;
    ability: string;
    module: string;
    dimension?: FormDimension;
    temporary?: boolean;
  }): FormComponentAttribute;
}

/**
 * @since 7
 * @systemapi
 */
declare class FormComponentAttribute extends CommonMethod<FormComponentAttribute> {
  /**
   * Sets the display area size of the card.
   * @since 7
   * @systemapi
   */
  size(value: { width: number; height: number }): FormComponentAttribute;

  /**
   * Card module name.
   * @since 7
   * @systemapi
   */
  moduleName(value: string): FormComponentAttribute;

  /**
   * Set the card size.
   * @since 7
   * @systemapi
   */
  dimension(value: FormDimension): FormComponentAttribute;

  /**
   * Indicates whether to allow card update.
   * @since 7
   * @systemapi
   */
  allowUpdate(value: boolean): FormComponentAttribute;

  /**
   * Whether the card is visible.
   * @since 7
   * @systemapi
   */
  visibility(value: Visibility): FormComponentAttribute;

  /**
   * This function is triggered after card information is obtained.
   * For details about the form information, see the definition of the original capability subsystem.
   * @since 7
   * @systemapi
   */
  onAcquired(callback: (info: { id: number }) => void): FormComponentAttribute;

  /**
   * Card loading error.
   * @since 7
   * @systemapi
   */
  onError(callback: (info: { errcode: number; msg: string }) => void): FormComponentAttribute;

  /**
   * Card to be redirected.
   * @since 7
   * @systemapi
   */
  onRouter(callback: (info: any) => void): FormComponentAttribute;

  /**
   * Uninstall Card.
   * @since 7
   * @systemapi
   */
  onUninstall(callback: (info: { id: number }) => void): FormComponentAttribute;
}

declare const FormComponent: FormComponentInterface;
declare const FormComponentInstance: FormComponentAttribute;
