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
 * common enum of color
 * @since 7
 */
declare enum Color {
  /**
   * White.
   * @since 7
   */
  White,

  /**
   * Black.
   * @since 7
   */
  Black,

  /**
   * Blue.
   * @since 7
   */
  Blue,

  /**
   * Brown.
   * @since 7
   */
  Brown,

  /**
   * Gray.
   * @since 7
   */
  Gray,

  /**
   * Green.
   * @since 7
   */
  Green,

  /**
   * Grey.
   * @since 7
   */
  Grey,

  /**
   * Orange.
   * @since 7
   */
  Orange,

  /**
   * color.
   * @since 7
   */
  Pink,

  /**
   * Red.
   * @since 7
   */
  Red,

  /**
   * Yellow.
   * @since 7
   */
  Yellow,
}

/**
 * Image display mode
 * @since 7
 */
declare enum ImageFit {
  /**
   * Keep the aspect ratio to zoom out or zoom in so that the image is completely displayed within the display boundary.
   * @since 7
   */
  Contain,

  /**
   * Keep the aspect ratio to zoom out or zoom in so that both sides of the image are greater than or equal to the display boundary.
   * @since 7
   */
  Cover,

  /**
   * Adaptive display
   * @since 7
   */
  Auto,

  /**
   * Zoom in or out without maintaining the aspect ratio so that the image fills the display boundary.
   * @since 7
   */
  Fill,

  /**
   * Keep the aspect ratio displayed, and the image zooms out or remains unchanged.
   * @since 7
   */
  ScaleDown,

  /**
   * Keep the original size and display it in the center.
   * @since 7
   */
  None,
}

/**
 * Border Style
 * @since 7
 */
declare enum BorderStyle {
  /**
   * Displays as a series of dots with a radius of half the borderWidth.
   * @since 7
   */
  Dotted,

  /**
   * Shows as a series of short square dashed lines.
   * @since 7
   */
  Dashed,

  /**
   * Shows as a solid line.
   * @since 7
   */
  Solid,
}

/**
 * Line Join Style
 * @since 7
 */
declare enum LineJoinStyle {
  /**
   * Connect path segments using bevels.
   * @since 7
   */
  Miter,

  /**
   * Connect path segments using sharp corners.
   * @since 7
   */
  Round,

  /**
   * Connect path segments using fillets.
   * @since 7
   */
  Bevel,
}

/**
 * Function Called by Touch
 * @since 7
 */
declare enum TouchType {
  /**
   * Triggered when the finger is pressed.
   * @since 7
   */
  Down,

  /**
   * Triggered when the finger is raised.
   * @since 7
   */
  Up,

  /**
   * Triggered when the finger presses on the screen.
   * @since 7
   */
  Move,

  /**
   * Triggers when the touch event cancels.
   * @since 7
   */
  Cancel,
}

/**
 * Function Called by Mouse
 * @since 8
 */
declare enum MouseButton {
  /**
   * Mouse Left Button.
   * @since 8
   */
  Left,

  /**
   * Mouse Right Button.
   * @since 8
   */
  Right,

  /**
   * Mouse Middle Button.
   * @since 8
   */
  Middle,

  /**
   * Mouse Back Button.
   * @since 8
   */
  Back,

  /**
   * Mouse Forward Button.
   * @since 8
   */
  Forward,

  /**
   * Mouse None Button.
   * @since 8
   */
  None,
}

/**
 * Function Called by Mouse
 * @since 8
 */
declare enum MouseAction {
  /**
   * Triggered when the mouse is pressed.
   * @since 8
   */
  Press,

  /**
   * Triggered when the mouse is released.
   * @since 8
   */
  Release,

  /**
   * Triggered when the mouse is Moved.
   * @since 8
   */
  Move,

  /**
   * Triggered when the mouse is Hovered.
   * @since 8
   */
  Hover
}

/**
 * Animation status.
 * @since 7
 */
declare enum AnimationStatus {
  /**
   * Initial state of the animation.
   * @since 7
   */
  Initial,

  /**
   * The animation is playing.
   * @since 7
   */
  Running,

  /**
   * The animation is paused.
   * @since 7
   */
  Paused,

  /**
   * The animation is stopped.
   * @since 7
   */
  Stopped,
}

/**
 * Sets the animation playback mode. By default, the animation starts to play again after the playback is complete.
 * @since 7
 */
declare enum Curve {
  /**
   * Indicates that the speed of the animation is the same from start to finish.
   * @since 7
   */
  Linear,

  /**
   * Indicates that the animation starts at a low speed, then accelerates, and then slows down before it ends, CubicBezier(0.25, 0.1, 0.25, 1.0).
   * @since 7
   */
  Ease,

  /**
   * Indicates that the animation starts at a low speed, CubicBezier(0.42, 0.0, 1.0, 1.0).
   * @since 7
   */
  EaseIn,

  /**
   * Indicates that the animation ends at a low speed, CubicBezier(0.0, 0.0, 0.78, 1.0).
   * @since 7
   */
  EaseOut,

  /**
   * Indicates that the animation starts and ends at a slow speed, CubicBezier(0.42, 0.0, 0.78, 1.0).
   * @since 7
   */
  EaseInOut,

  /**
   * Slow-in, fast-out
   * @since 7
   */
  FastOutSlowIn,

  /**
   * Linear Out Slow In
   * @since 7
   */
  LinearOutSlowIn,

  /**
   * Fast OutL inear In.
   * @since 7
   */
  FastOutLinearIn,

  /**
   * Extreme Deceleration.
   * @since 7
   */
  ExtremeDeceleration,

  /**
   * Sharp.
   * @since 7
   */
  Sharp,

  /**
   * Rhythm.
   * @since 7
   */
  Rhythm,

  /**
   * Smooth.
   * @since 7
   */
  Smooth,

  /**
   * Damping curve, CubicBezier(0.2, 0.0, 0.2, 1.0).
   * @since 7
   */
  Friction,
}

/**
 * Sets the state before and after the animation starts.
 * @since 7
 */
declare enum FillMode {
  /**
   * Restores to the initial state after the playback is complete.
   * @since 7
   */
  None,

  /**
   * Retains the state at the end of the animation when the playback is complete.
   * @since 7
   */
  Forwards,

  /**
   * Applies the start attribute value for the period specified by animation-delay before the animation is displayed.
   * @since 7
   */
  Backwards,

  /**
   * Both forward and backward fill modes are applied.
   * @since 7
   */
  Both,
}

/**
 * Play Mode
 * @since 7
 */
declare enum PlayMode {
  /**
   * The animation plays normally.
   * @since 7
   */
  Normal,

  /**
   * The animation plays backwards.
   * @since 7
   */
  Reverse,

  /**
   * The animation plays forward on odd numbers (1, 3, 7...) and backward on even numbers (2, 4, 6...).
   * @since 7
   */
  Alternate,

  /**
   * The animation plays backwards on odd numbers (1, 3, 7...) and forwards on even numbers (2, 4, 6...)..
   * @since 7
   */
  AlternateReverse,
}

/**
 * Type of a key.
 * @since 7
 */
declare enum KeyType {
  /**
   * Press the key.
   * @since 7
   */
  Down,

  /**
   * The key is released.
   * @since 7
   */
  Up,
}

/**
 * Type of the input device that triggers the current key.
 * @since 7
 */
declare enum KeySource {
  /**
   * The input device type is unknown.
   * @since 7
   */
  Unknown,

  /**
   * Set Device Type to Keyboard.
   * @since 7
   */
  Keyboard,
}

/**
 * Edge.
 * @since 7
 */
declare enum Edge {
  /**
   * The top is centered horizontally.
   * @since 7
   */
  Top,

  /**
   * Center horizontal and vertical.
   * @since 7
   */
  Center,

  /**
   * The bottom is centered horizontally.
   * @since 7
   */
  Bottom,

  /**
   * Cross axis direction text baseline alignment.
   * @since 7
   */
  Baseline,

  /**
   * Align the head of the cross axis direction.
   * @since 7
   */
  Start,

  /**
   * Middle
   * @since 7
   */
  Middle,

  /**
   * Align the head of the cross axis direction.
   * @since 7
   */
  End,
}

/**
 * Set Weekend
 * @since 7
 */
declare enum Week {
  /**
   * Monday.
   * @since 7
   */
  Mon,

  /**
   * Tuesday.
   * @since 7
   */
  Tue,

  /**
   * Wednesday.
   * @since 7
   */
  Wed,

  /**
   * Thursday.
   * @since 7
   */
  Thur,

  /**
   * Friday.
   * @since 7
   */
  Fri,

  /**
   * Saturday.
   * @since 7
   */
  Sat,

  /**
   * Sunday.
   * @since 7
   */
  Sun,
}

/**
 * Sets the horizontal layout of elements.
 * @since 7
 */
declare enum Direction {
  /**
   * Elements are laid out from left to right.
   * @since 7
   */
  Ltr,

  /**
   * Elements are laid out from right to left.
   * @since 7
   */
  Rtl,

  /**
   * Use the default layout direction.
   * @since 7
   */
  Auto,
}

/**
 * Used to set the status of the scroll bar.
 * @since 7
 */
declare enum BarState {
  /**
   * Not displayed.
   * @since 7
   */
  Off,

  /**
   * On-demand display (displayed when you touch it and disappears after 2 seconds).
   * @since 7
   */
  Auto,

  /**
   * Resident display.
   * @since 7
   */
  On,
}

/**
 * Sliding effect
 * @since 7
 */
declare enum EdgeEffect {
  /**
   * Elastic physical action, sliding to the edge can continue to slide for a distance based on the initial speed or touch event, and spring back when released.
   * @since 7
   */
  Spring,

  /**
   * Fade.
   * @since 7
   */
  Fade,

  /**
   * Sliding to the edge has no effect.
   * @since 7
   */
  None,
}

/**
 * Alignment enumeration description.
 * @since 7
 */
declare enum Alignment {
  /**
   * Top Start.
   * @since 7
   */
  TopStart,

  /**
   * The top is centered horizontally.
   * @since 7
   */
  Top,

  /**
   * Top tail end.
   * @since 7
   */
  TopEnd,

  /**
   * The starting end is centered longitudinally.
   * @since 7
   */
  Start,

  /**
   * Center horizontal and vertical.
   * @since 7
   */
  Center,

  /**
   * The tail end is centered longitudinally.
   * @since 7
   */
  End,

  /**
   * Bottom starting end.
   * @since 7
   */
  BottomStart,

  /**
   * The bottom is centered horizontally.
   * @since 7
   */
  Bottom,

  /**
   * Bottom end.
   * @since 7
   */
  BottomEnd,
}

/**
 * TransitionType enumeration description.
 * @since 7
 */
declare enum TransitionType {
  /**
   * Specifies that the current transition action takes effect in all change scenarios of the component.
   * @since 7
   */
  All,

  /**
   * Specifies the insertion scenario in which the current transition action takes effect.
   * @since 7
   */
  Insert,

  /**
   * Specifies the deletion scenario in which the current transition action takes effect.
   * @since 7
   */
  Delete,
}

/**
 * RelateType enumeration description
 * @since 7
 */
declare enum RelateType {
  /**
   * Scales the current component to fill the parent component.
   * @since 7
   */
  FILL,

  /**
   * Scales the current component to fit the parent component.
   * @since 7
   */
  FIT,
}

/**
 * Controls the display or hide of the current component
 * @since 7
 */
declare enum Visibility {
  /**
   * Show
   * @since 7
   */
  Visible,

  /**
   * Hide, but participate in layout for placeholder.
   * @since 7
   */
  Hidden,

  /**
   * Hides but does not participate in layout and does not take place.
   * @since 7
   */
  None,
}

/**
 * LineCapStyle enumeration description
 * @since 7
 */
declare enum LineCapStyle {
  /**
   * The two ends of the dividing line are parallel lines.
   * @since 7
   */
  Butt,

  /**
   * The two ends of the dividing line are semicircles.
   * @since 7
   */
  Round,

  /**
   * Extends half a circle at the end of the path with a width equal to half the line width and a height equal to the line width.
   * @since 7
   */
  Square,
}

/**
 * Axis enumeration description.
 * @since 7
 */
declare enum Axis {
  /**
   * Longitudinal arrangement
   * @since 7
   */
  Vertical,

  /**
   * Horizontal arrangement.
   * @since 7
   */
  Horizontal,
}

/**
 * HorizontalAlign enumeration description.
 * @since 7
 */
declare enum HorizontalAlign {
  /**
   * Aligns the start end in the language direction.
   * @since 7
   */
  Start,

  /**
   * Center alignment. The default alignment mode is used.
   * @since 7
   */
  Center,

  /**
   * Aligns the ends in the language direction.
   * @since 7
   */
  End,
}

/**
 * FlexAlign enumeration description.
 * @since 7
 */
declare enum FlexAlign {
  /**
   * The element is aligned at the head of the principal axis,
   * the first element is aligned with the head of the row, and subsequent elements are aligned with the previous one.
   * @since 7
   */
  Start,

  /**
   * The elements are centered in the direction of the principal axis,
   * and the first element is the same distance from the beginning of the row as the last element is from the end of the row.
   * @since 7
   */
  Center,

  /**
   * The element is aligned at the tail of the principal axis,
   * the last element is aligned at the end of the row, and the other elements are aligned with the next.
   * @since 7
   */
  End,

  /**
   * Elastic elements are evenly distributed in the direction of the Flex principal axis,
   * with the same distance between adjacent elements.
   * The first element aligns with the beginning of the line, and the last element aligns with the end of the line.
   * @since 7
   */
  SpaceBetween,

  /**
   * Elastic elements are evenly distributed in the direction of the Flex principal axis,
   *  with the same distance between adjacent elements. Half the distance between adjacent elements as the distance between
   * the first element and the distance between the last element and the end of the row.
   * @since 7
   */
  SpaceAround,

  /**
   * Elements in the Flex axis direction are evenly spaced.
   * The spacing between adjacent elements, the spacing between the first element and the beginning of the row,
   * and the spacing between the last element and the end of the row are the same.
   * @since 7
   */
  SpaceEvenly,
}

/**
 * ItemAlign enumeration description
 * @since 7
 */
declare enum ItemAlign {
  /**
   * Use the default configuration in the Flex container.
   * @since 7
   */
  Auto,

  /**
   * The element is in the Flex container with the cross-axis direction head aligned.
   * @since 7
   */
  Start,

  /**
   * The element is centered in the Flex container with the cross axis direction aligned.
   * @since 7
   */
  Center,

  /**
   * The element is bottom aligned in the Flex container with the cross axis direction.
   * @since 7
   */
  End,

  /**
   * Element In the Flex container, the cross-axis direction text baseline is aligned.
   * @since 7
   */
  Baseline,

  /**
   * Element In a Flex container, the fill is stretched across the axis and, when no dimension is set, to the container size.
   * @since 7
   */
  Stretch,
}

/**
 * FlexDirection enumeration description
 * @since 7
 */
declare enum FlexDirection {
  /**
   * The main axis is consistent with the row direction as the layout mode.
   * @since 7
   */
  Row,

  /**
   * The main axis is consistent with the column direction as the layout mode.
   * @since 7
   */
  Column,

  /**
   * The layout is in the opposite direction to the Row direction.
   * @since 7
   */
  RowReverse,

  /**
   * Layout in the opposite direction to the column.
   * @since 7
   */
  ColumnReverse,
}

/**
 * FlexWrap enumeration description
 * @since 7
 */
declare enum FlexWrap {
  /**
   * The Flex container has a single row/column layout of elements, and children are allowed to go beyond the container.
   * @since 7
   */
  NoWrap,

  /**
   * The elements of the Flex container are arranged in multiple rows or columns, and the sub-items are allowed to exceed the container.
   * @since 7
   */
  Wrap,

  /**
   * The elements of the Flex container are arranged in reverse multiple rows/columns, and children are allowed to exceed the container.
   * @since 7
   */
  WrapReverse,
}

/**
 * VerticalAlign enumeration description
 * @since 7
 */
declare enum VerticalAlign {
  /**
   * Top alignment.
   * @since 7
   */
  Top,

  /**
   * Center alignment. The default alignment mode is used.
   * @since 7
   */
  Center,

  /**
   * Bottom alignment.
   * @since 7
   */
  Bottom,
}

/**
 * ImageRepeat enumeration description
 * @since 7
 */
declare enum ImageRepeat {
  /**
   * Do not draw the picture again.
   * @since 7
   */
  NoRepeat,

  /**
   * Repeat the drawing only on the horizontal axis.
   * @since 7
   */
  X,

  /**
   * Repeat the drawing only on the vertical axis.
   * @since 7
   */
  Y,

  /**
   * Draw the picture repeatedly on both axes.
   * @since 7
   */
  XY,
}

/**
 * ImageSize enumeration description
 * @since 7
 */
declare enum ImageSize {
  /**
   * Keep the scale of the original image unchanged.
   * @since 7
   */
  Auto,

  /**
   * Default value. Keep the aspect ratio to zoom in or out the image so that both sides of the image are greater than or equal to the display boundary.
   * @since 7
   */
  Cover,

  /**
   * Keep the aspect ratio to zoom out or zoom in so that the image is completely displayed within the display boundary.
   * @since 7
   */
  Contain,
}

/**
 * GradientDirection enumeration description
 * @since 7
 */
declare enum GradientDirection {
  /**
   * Right to left.
   * @since 7
   */
  Left,

  /**
   * From the bottom up.
   * @since 7
   */
  Top,

  /**
   * From left to right.
   * @since 7
   */
  Right,

  /**
   * From the top down.
   * @since 7
   */
  Bottom,

  /**
   * Top Left
   * @since 7
   */
  LeftTop,

  /**
   * Left Bottom
   * @since 7
   */
  LeftBottom,

  /**
   * Right Top
   * @since 7
   */
  RightTop,

  /**
   * Right Bottom
   * @since 7
   */
  RightBottom,

  /**
   * None
   * @since 7
   */
  None,
}

/**
 * SharedTransitionEffectType enumeration description
 * @since 7
 */
declare enum SharedTransitionEffectType {
  /**
   * The location of the destination page element remains unchanged, and you can configure the transparency animation. Currently,
   * only the static effect configured for redirecting to the target page takes effect.
   * @since 7
   */
  Static,

  /**
   * Move the source page element to the destination page element location and scale it appropriately.
   * @since 7
   */
  Exchange,
}

/**
 * Text style
 * @since 7
 */
declare enum FontStyle {
  /**
   * Default style.
   * @since 7
   */
  Normal,

  /**
   * Italic style.
   * @since 7
   */
  Italic,
}

/**
 * The font weight of the text
 * @since 7
 */
declare enum FontWeight {
  /**
   * Defines a lighter value than [Inherited Value]..
   * @since 7
   */
  Lighter,

  /**
   * Normal font. Equivalent to a digital value of 400.
   * @since 7
   */
  Normal,

  /**
   * Defines a more general value than [Inherited Value].
   * @since 7
   */
  Regular,

  /**
   * Defines a value that is more centered than [Inherited Value].
   * @since 7
   */
  Medium,

  /**
   * Bold. Equivalent to a numeric value of 700.
   * @since 7
   */
  Bold,

  /**
   * Defines a value that is heavier than [Inherited Value].
   * @since 7
   */
  Bolder,
}

/**
 * Alignment of text.
 * @since 7
 */
declare enum TextAlign {
  /**
   * Center the text.
   * @since 7
   */
  Center,

  /**
   * The text is aligned in the same direction as the writing
   * @since 7
   */
  Start,

  /**
   * The text is aligned in the opposite direction of writing
   * @since 7
   */
  End,
}

/**
 * Declare how text overflows.
 * @since 7
 */
declare enum TextOverflow {
  /**
   * When the text is too long, it will be cropped and displayed.
   * @since 7
   */

  Clip,
  /**
   * If the text is too long, the text that cannot be displayed shall be replaced by ellipsis.
   * @since 7
   */

  Ellipsis,

  /**
   * Text is not cropped when it is too long.
   * @since 7
   */
  None,
}

/**
 * Type of text modifier.
 * @since 7
 */
declare enum TextDecorationType {
  /**
   * Do not use text decorative lines.
   * @since 7
   */
  None,

  /**
   * Underline the words.
   * @since 7
   */
  Underline,

  /**
   * Text is in all uppercase.
   * @since 7
   */
  Overline,

  /**
   * A modifier line that passes through the text.
   * @since 7
   */
  LineThrough,
}

/**
 * Letter type in text
 * @since 7
 */
declare enum TextCase {
  /**
   * The default is normal.
   * @since 7
   */
  Normal,

  /**
   * The text is all lowercase.
   * @since 7
   */
  LowerCase,

  /**
   * The text is all uppercase.
   * @since 7
   */
  UpperCase,
}

/**
 * ResponseType for contextMenu
 * @since 8
 */
declare enum ResponseType {
  /**
   * Right click.
   * @since 8
   */
  RightClick,

  /**
   * Long press.
   * @since 8
   */
  LongPress,
}

/**
 * HoverEffect enumeration description
 * @since 8
 */
declare enum HoverEffect {
  /**
   * Default effect
   * @since 8
   */
  Auto,

  /**
   * Zoom in and out effect
   * @since 8
   */
  Scale,

  /**
   * Highlight effect
   * @since 8
   */
  Highlight,

  /**
   * None effect
   * @since 8
   */
  None,
}

/**
 * Placement enumeration description
 * @since 8
 */
declare enum Placement {
  /**
   * Left placement
   * @since 8
   */
  Left,

  /**
   * Right placement
   * @since 8
   */
  Right,

  /**
   * Top placement
   * @since 8
   */
  Top,

  /**
   * Bottom placement
   * @since 8
   */
  Bottom,

  /**
   * TopLeft placement
   * @since 8
   */
  TopLeft,

  /**
   * TopRight placement
   * @since 8
   */
  TopRight,

  /**
   * BottomLeft placement
   * @since 8
   */
  BottomLeft,

  /**
   * BottomRight placement
   * @since 8
   */
  BottomRight,
}
