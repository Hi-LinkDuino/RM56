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
 * Declare the type of input box
 * @since 7
 */
declare enum InputType {
  /**
   * Basic input mode.
   * @since 7
   */
  Normal,

  /**
   * Password entry mode.
   * @since 7
   */
  Number,

  /**
   * E-mail address input mode.
   * @since 7
   */
  Email,

  /**
   * Pure digital input mode.
   * @since 7
   */
  Password,
}

/**
 * Declare the type of soft keyboard.
 * @since 7
 */
declare enum EnterKeyType {
  /**
   * Go.
   * @since 7
   */
  Go,

  /**
   * Search.
   * @since 7
   */
  Search,

  /**
   * Send.
   * @since 7
   */
  Send,

  /**
   * Next.
   * @since 7
   */
  Next,

  /**
   * Done.
   * @since 7
   */
  Done,
}

/**
 * Provides the method of switching the cursor position.
 * @since 8
 */
declare class TextInputController {
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
 * Defines the options of TextInput.
 * @since 7
 */
declare interface TextInputOptions {
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
  controller?: TextInputController;
}

/**
 * Provides a single-line text input component interface.
 * @since 7
 */
interface TextInputInterface {
  /**
   * Called when writing a single line of text.
   * @since 7
   */
  (value?: TextInputOptions): TextInputAttribute;
}

/**
 * Defines the TextInput attribute functions.
 * @since 7
 */
declare class TextInputAttribute extends CommonMethod<TextInputAttribute> {
  /**
   * Called when the input type is set.
   * @since 7
   */
  type(value: InputType): TextInputAttribute;

  /**
   * Called when the color of the placeholder is set.
   * @since 7
   */
  placeholderColor(value: ResourceColor): TextInputAttribute;

  /**
   * Called when the font property of the placeholder is set.
   * @since 7
   */
  placeholderFont(value?: Font): TextInputAttribute;

  /**
   * Called when the type of soft keyboard input button is set.
   * @since 7
   */
  enterKeyType(value: EnterKeyType): TextInputAttribute;

  /**
   * Called when the color of the insertion cursor is set.
   * @since 7
   */
  caretColor(value: ResourceColor): TextInputAttribute;

  /**
   * Called when judging whether the text editing change finished.
   * @since 7
   * @deprecated since 8
   */
  onEditChanged(callback: (isEditing: boolean) => void): TextInputAttribute;

  /**
   * Called when judging whether the text editing change finished.
   * @since 8
   */
  onEditChange(callback: (isEditing: boolean) => void): TextInputAttribute;

  /**
   * Called when submitted.
   * @since 7
   */
  onSubmit(callback: (enterKey: EnterKeyType) => void): TextInputAttribute;

  /**
   * Called when the input of the input box changes.
   * @since 7
   */
  onChange(callback: (value: string) => void): TextInputAttribute;

  /**
   * Called when the input of maximum text length is set.
   * @since 7
   */
  maxLength(value: number): TextInputAttribute;

  /**
   * Called when the font color is set.
   * @since 7
   */
  fontColor(value: ResourceColor): TextInputAttribute;

  /**
   * Called when the font size is set.
   * @since 7
   */
  fontSize(value: Length): TextInputAttribute;

  /**
   * Called when the font style of a font is set.
   * @since 7
   */
  fontStyle(value: FontStyle): TextInputAttribute;

  /**
   * Called when the font weight is set.
   * @since 7
   */
  fontWeight(value: number | FontWeight | string): TextInputAttribute;

  /**
   * Called when the font list of text is set.
   * @since 7
   */
  fontFamily(value: ResourceStr): TextInputAttribute;

  /**
   * Called when the inputFilter of text is set.
   * @since 8
   */
  inputFilter(value: ResourceStr, error?: (value: string) => void): TextInputAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 8
   */
  onCopy(callback: (value: string) => void): TextInputAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 8
   */
  onCut(callback: (value: string) => void): TextInputAttribute;

  /**
   * Called when using the Clipboard menu
   * @since 8
   */
  onPaste(callback: (value: string) => void): TextInputAttribute;
}

declare const TextInput: TextInputInterface;
declare const TextInputInstance: TextInputAttribute;
