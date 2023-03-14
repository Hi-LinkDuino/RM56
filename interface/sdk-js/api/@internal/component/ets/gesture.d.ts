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
 * Creating an Object
 * @since 7
 */
declare enum PanDirection {
  /**
   * Default.
   * @since 7
   */
  None,

  /**
   * Sliding horizontally.
   * @since 7
   */
  Horizontal,

  /**
   * Sliding left.
   * @since 7
   */
  Left,

  /**
   * Sliding right.
   * @since 7
   */
  Right,

  /**
   * Sliding Vertical
   * @since 7
   */
  Vertical,

  /**
   * Sliding up.
   * @since 7
   */
  Up,

  /**
   * Sliding Down.
   * @since 7
   */
  Down,

  /**
   * Sliding in all directions.
   * @since 7
   */
  All,
}

/**
 * Creating an Object
 * @since 8
 */
declare enum SwipeDirection {
  /**
   * Default.
   * @since 8
   */
  None,

  /**
   * Sliding horizontally.
   * @since 8
   */
  Horizontal,

  /**
   * Sliding Vertical
   * @since 8
   */
  Vertical,

  /**
   * Sliding in all directions.
   * @since 8
   */
  All,
}

/**
 * Creating an Object
 * @since 7
 */
declare enum GestureMode {
  /**
   * Sequential gesture recognition is performed in sequence according to the gesture registration sequence.
   * @since 7
   */
  Sequence,

  /**
   * Simultaneous recognition. Registration gestures participate in recognition. Everything can be triggered.
   * @since 7
   */
  Parallel,

  /**
   * Mutually exclusive recognition. Only one gesture is successfully recognized.
   * @since 7
   */
  Exclusive,
}

/**
 * Creating an Object
 * @since 7
 */
declare enum GestureMask {
  /**
   * High-priority response to the current gesture.When the current gesture fails to be recognized, other gesture responses are triggered.For gestures with the same priority, responses are performed based on the recognition sequence.
   * @since 7
   */
  Normal,

  /**
   * Ignore internal gestures and recognize the current gesture first.
   * @since 7
   */
  IgnoreInternal,
}

/**
 * Type of the finger information.
 * @since 8
 */
interface FingerInfo {
  /**
   * Finger unique identifier.
   * @since 8
   */
  id: number;

  /**
   * X coordinate of the touch point relative to the left edge of the device screen.
   * @since 8
   */
  globalX: number;

  /**
   * The Y coordinate of the touch point relative to the upper edge of the device screen.
   * @since 8
   */
  globalY: number;

  /**
   * X coordinate of the touch point relative to the left edge of the touched element.
   * @since 8
   */
  localX: number;

  /**
   * Y coordinate of the touch point relative to the upper edge of the touched element.
   * @since 8
   */
  localY: number;
}

/**
 * Defines the Gesture Type.
 * @since 7
 */
declare type GestureType =
  | TapGestureInterface
  | LongPressGestureInterface
  | PanGestureInterface
  | PinchGestureInterface
  | SwipeGestureInterface
  | RotationGestureInterface
  | GestureGroupInterface;

/**
 * Defines event info for gesture.
 * @since 7
 */
interface GestureEvent extends BaseEvent {
  /**
   * Indicates whether an event is triggered repeatedly.
   * Used in LongPressGesture.
   * @since 7
   */
  repeat: boolean;

  /**
   * All finger information.
   * Used in LongPressGesture and TapGesture.
   * @since 8
   */
  fingerList: FingerInfo[];

  /**
   * Gesture event offset X.
   * The unit is vp.
   * Used in PanGesture.
   * @since 7
   */
  offsetX: number;

  /**
   * Gesture event offset Y.
   * The unit is vp.
   * Used in PanGesture.
   * @since 7
   */
  offsetY: number;

  /**
   * Gesture event direction angle.
   * The unit is deg.
   * Used in RotationGesture and SwipeGesture.
   * @since 7
   */
  angle: number;

  /**
   * Gesture event slide speed.
   * The unit is vp.
   * Used in SwipeGesture.
   * @since 8
   */
  speed: number;

  /**
   * Scaling ratio.
   * Used in PinchGesture.
   * @since 7
   */
  scale: number;

  /**
   * X-axis coordinate of the kneading center point.
   * The unit is vp.
   * Used in PinchGesture.
   * @since 7
   */
  pinchCenterX: number;

  /**
   * Y-axis coordinate of the kneading center point.
   * The unit is vp.
   * Used in PinchGesture.
   * @since 7
   */
  pinchCenterY: number;
}

/**
 * Defines TapGesture interface.
 * @since 7
 */
interface TapGestureInterface {
  /**
   * Set the value.
   * count:Number of consecutive clicks recognized. If the value is less than 1, the default value is used.
   * fingers:The hand index that triggers the click. If the value is less than 1, the default value is used.
   * @since 7
   */
  (value?: { count?: number; fingers?: number }): TapGestureInterface;

  /**
   * Tap gesture recognition success callback.
   * @since 7
   */
  onAction(event: (event?: GestureEvent) => void): TapGestureInterface;
}

/**
 * Defines LongPressGesture interface.
 * @since 7
 */
interface LongPressGestureInterface {
  /**
   * Set the value.
   * fingers: Indicates the hand index that triggers the long press.
   * repeat: Indicates whether to trigger event callback continuously.
   * duration: Minimum press and hold time, in milliseconds.
   * @since 7
   */
  (value?: { fingers?: number; repeat?: boolean; duration?: number }): LongPressGestureInterface;

  /**
   * LongPress gesture recognition success callback.
   * @since 7
   */
  onAction(event: (event?: GestureEvent) => void): LongPressGestureInterface;

  /**
   * The LongPress gesture is successfully recognized. When the finger is lifted, the callback is triggered.
   * @since 7
   */
  onActionEnd(event: (event?: GestureEvent) => void): LongPressGestureInterface;

  /**
   * The LongPress gesture is successfully recognized and a callback is triggered when the touch cancel event is received.
   * @since 7
   */
  onActionCancel(event: () => void): LongPressGestureInterface;
}

/**
 * Defines the PanGesture options.
 * @since 7
 */
declare class PanGestureOptions {
  /**
   * Constructor parameters.
   * @since 7
   */
  constructor(value?: { fingers?: number; direction?: PanDirection; distance?: number });

  /**
   * Sets the direction attribute.
   * @since 7
   */
  setDirection(value: PanDirection);

  /**
   * Sets the setDistance attribute.
   * @since 7
   */
  setDistance(value: number);

  /**
   * Sets the setFingers attribute.
   * @since 7
   */
  setFingers(value: number);
}

/**
 * Defines the PanGesture interface.
 * @since 7
 */
interface PanGestureInterface {
  /**
   * Set the value.
   * @since 7
   */
  (value?: { fingers?: number; direction?: PanDirection; distance?: number } | PanGestureOptions): PanGestureInterface;

  /**
   * Pan gesture recognition success callback.
   * @since 7
   */
  onActionStart(event: (event?: GestureEvent) => void): PanGestureInterface;

  /**
   * Callback when the Pan gesture is moving.
   * @since 7
   */
  onActionUpdate(event: (event?: GestureEvent) => void): PanGestureInterface;

  /**
   * The Pan gesture is successfully recognized. When the finger is lifted, the callback is triggered.
   * @since 7
   */
  onActionEnd(event: (event?: GestureEvent) => void): PanGestureInterface;

  /**
   * The Pan gesture is successfully recognized and a callback is triggered when the touch cancel event is received.
   * @since 7
   */
  onActionCancel(event: () => void): PanGestureInterface;
}

/**
 * Defines the SwipeGesture interface.
 * @since 8
 */
interface SwipeGestureInterface {
  /**
   * Set the value.
   * @since 8
   */
  (value?: { fingers?: number; direction?: SwipeDirection; speed?: number }): SwipeGestureInterface;

  /**
   * Slide gesture recognition success callback.
   * @since 8
   */
  onAction(event: (event?: GestureEvent) => void): SwipeGestureInterface;
}

/**
 * Defines the PinchGesture interface.
 * @since 7
 */
interface PinchGestureInterface {
  /**
   * Set the value.
   * @since 7
   */
  (value?: { fingers?: number; distance?: number }): PinchGestureInterface;

  /**
   * Pan gesture recognition success callback.
   * @since 7
   */
  onActionStart(event: (event?: GestureEvent) => void): PinchGestureInterface;

  /**
   * Callback when the Pan gesture is moving.
   * @since 7
   */
  onActionUpdate(event: (event?: GestureEvent) => void): PinchGestureInterface;

  /**
   * The Pan gesture is successfully recognized. When the finger is lifted, the callback is triggered.
   * @since 7
   */
  onActionEnd(event: (event?: GestureEvent) => void): PinchGestureInterface;

  /**
   * The Pan gesture is successfully recognized and a callback is triggered when the touch cancel event is received.
   * @since 7
   */
  onActionCancel(event: () => void): PinchGestureInterface;
}

/**
 * Defines the RotationGesture interface.
 * @since 7
 */
interface RotationGestureInterface {
  /**
   * Set the value.
   * @since 7
   */
  (value?: { fingers?: number; angle?: number }): RotationGestureInterface;

  /**
   * Pan gesture recognition success callback.
   * @since 7
   */
  onActionStart(event: (event?: GestureEvent) => void): RotationGestureInterface;

  /**
   * Callback when the Pan gesture is moving.
   * @since 7
   */
  onActionUpdate(event: (event?: GestureEvent) => void): RotationGestureInterface;

  /**
   * The Pan gesture is successfully recognized. When the finger is lifted, the callback is triggered.
   * @since 7
   */
  onActionEnd(event: (event?: GestureEvent) => void): RotationGestureInterface;

  /**
   * The Pan gesture is successfully recognized and a callback is triggered when the touch cancel event is received.
   * @since 7
   */
  onActionCancel(event: () => void): RotationGestureInterface;
}

/**
 * Defines the GestureGroup interface.
 * @since 7
 */
interface GestureGroupInterface {
  /**
   * Return to Obtain GestureGroup.
   * @since 7
   */
  (mode: GestureMode, ...gesture: GestureType[]): GestureGroupInterface;

  /**
   * The Pan gesture is successfully recognized and a callback is triggered when the touch cancel event is received.
   * @since 7
   */
  onCancel(event: () => void): GestureGroupInterface;
}

declare const TapGesture: TapGestureInterface;
declare const LongPressGesture: LongPressGestureInterface;
declare const PanGesture: PanGestureInterface;
declare const SwipeGesture: SwipeGestureInterface;
declare const PinchGesture: PinchGestureInterface;
declare const RotationGesture: RotationGestureInterface;
declare const GestureGroup: GestureGroupInterface;
