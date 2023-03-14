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
 * Provides an interface for writing texts.
 * @since 7
 */
interface TextInterface {
  /**
   * Called when writing text.
   * @since 7
   */
  (content?: string | Resource): TextAttribute;
}

/**
 * @since 7
 */
declare class TextAttribute extends CommonMethod<TextAttribute> {
  /**
   * Called when the font color is set.
   * @since 7
   */
  fontColor(value: ResourceColor): TextAttribute;

  /**
   * Called when the font size is set.
   * @since 7
   */
  fontSize(value: number | string | Resource): TextAttribute;

  /**
   * Called when the minimum font size of the font is set.
   * @since 7
   */
  minFontSize(value: number | string | Resource): TextAttribute;

  /**
   * Called when the maximum font size of the font is set.
   * @since 7
   */
  maxFontSize(value: number | string | Resource): TextAttribute;

  /**
   * Called when the font style of a font is set.
   * @since 7
   */
  fontStyle(value: FontStyle): TextAttribute;

  /**
   * Called when the font weight is set.
   * @since 7
   */
  fontWeight(value: number | FontWeight | string): TextAttribute;

  /**
   * Called when the horizontal center mode of the font is set.
   * @since 7
   */
  textAlign(value: TextAlign): TextAttribute;

  /**
   * Called when the vertical center mode of the font is set.
   * @since 7
   */
  lineHeight(value: number | string | Resource): TextAttribute;

  /**
   * Called when the overflow mode of the font is set.
   * @since 7
   */
  textOverflow(value: { overflow: TextOverflow }): TextAttribute;

  /**
   * Called when the font list of text is set.
   * @since 7
   */
  fontFamily(value: string | Resource): TextAttribute;

  /**
   * Called when the maximum number of lines of text is set.
   * @since 7
   */
  maxLines(value: number): TextAttribute;

  /**
   * Called when the text decoration of the text is set.
   * @since 7
   */
  decoration(value: { type: TextDecorationType; color?: ResourceColor }): TextAttribute;

  /**
   * Called when the distance between text fonts is set.
   * @since 7
   */
  letterSpacing(value: number | string): TextAttribute;

  /**
   * Called when the type of letter in the text font is set.
   * @since 7
   */
  textCase(value: TextCase): TextAttribute;

  /**
   * Called when the baseline offset is set.
   * @since 7
   */
  baselineOffset(value: number | string): TextAttribute;
}

declare const TextInstance: TextAttribute;
declare const Text: TextInterface;
