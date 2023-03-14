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
 * Provides the method of switching the cursor position.
 * @since 8
 */
declare class TextAreaController {
  /**
   * constructor.
   * @since 8
   */
  constructor();
  /**
   * Called when the position of the insertion cursor is set.
   * @since 8
   */
  caretPosition(value: number): void;
}

/**
 * Defines the options of TextArea.
 * @since 7
 */
declare interface TextAreaOptions {
  /**
   * The place holder text string.
   * @since 7
   */
  placeholder?: ResourceStr;

  /**
   * Sets the current value of TextArea.
   * @since 7
   */
  text?: ResourceStr;

  /**
   * Called when the position of the insertion cursor is set.
   * @since 8
   */
  controller?: TextAreaController;
}

/**
 * Provides an interface for the multi-line text input component.
 * @since 7
 */
interface TextAreaInterface {
  /**
   * Called when writing multiple lines of text.
   * @since 7
   */
  (value?: TextAreaOptions): TextAreaAttribute;
}

/**
 * Defines the attribute functions of TextArea.
 * @since 7
 */
declare class TextAreaAttribute extends CommonMethod<TextAreaAttribute> {
  /**
   * Called when the color of the placeholder is set.
   * @since 7
   */
  placeholderColor(value: ResourceColor): TextAreaAttribute;

  /**
   * Called when the font property of the placeholder is set.
   * @since 7
   */
  placeholderFont(value: Font): TextAreaAttribute;

  /**
   * Called when the alignment of the contents of a multiline text box is set.
   * @since 7
   */
  textAlign(value: TextAlign): TextAreaAttribute;

  /**
   * Called when the insertion cursor color is set.
   * @since 7
   */
  caretColor(value: ResourceColor): TextAreaAttribute;

  /**
   * Called when the font color is set.
   * @since 7
   */
  fontColor(value: ResourceColor): TextAreaAttribute;

  /**
   * Called when the font size is set.
   * @since 7
   */
  fontSize(value: Length): TextAreaAttribute;

  /**
   * Called when the font style of a font is set.
   * @since 7
   */
  fontStyle(value: FontStyle): TextAreaAttribute;

  /**
   * Called when the font weight is set.
   * @since 7
   */
  fontWeight(value: number | FontWeight | string): TextAreaAttribute;

  /**
   * Called when the font list of text is set.
   * @since 7
   */
  fontFamily(value: ResourceStr): TextAreaAttribute;

  /**
   * Called when the inputFilter of text is set.
   * @since 8
   */
  inputFilter(value: ResourceStr, error?: (value: string) => void): TextAreaAttribute;

  /**
   * Called when the input changes.
   * @since 7
   */
  onChange(callback: (value: string) => void): TextAreaAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 7
   */
  onCopy(callback: (value: string) => void): TextAreaAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 7
   */
  onCut(callback: (value: string) => void): TextAreaAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 7
   */
  onPaste(callback: (value: string) => void): TextAreaAttribute;
}

declare const TextArea: TextAreaInterface;
declare const TextAreaInstance: TextAreaAttribute;
