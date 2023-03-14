/**
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
import gridJsunit from './GridJsunit.test.ets';
import canvasJsunit from './CanvasJsunit.test.ets';
import textJsunit from './TextJsunit.test.ets';
import badgeJsunit from './BadgeJsunit.test.ets';
import longPressGestureJsUnit from './LongPressGesture.test.ets';
import buttonJsunit from './ButtonJsunit.test.ets';
import responseRegionJsunit from './ResponseRegionJsunit.test.ets';
import panGestureJsunit from './PanGestureJsunit.test.ets';
import tabsJsunit from './TabsJsunit.test.ets';
import qrCodeJsunit from './QrCodeJsunit.test.ets';
import tapGesture from './TapGesture.test.ets';
import progressJsunit from './ProgressJsunit.test.ets';
import animateJsunit from './AnimateJsunit.test.ets';
import commonJsunit from './CommonJsunit.test.ets';
import spanJsunit from './SpanJsunit.test.ets';
import ratingJsunit from './RatingJsunit.test.ets';
import canvas2Jsunit from './Canvas2Jsunit.test.ets';
import toggleJsunit from './ToggleJsunit.test.ets';
import shapeJsunit from './ShapeJsunit.test.ets';
import motionPathJsunit from './MotionPathJsunit.test.ets';
import scrollCodeJsunit from './ScrollCodeJsunit.test.ets';
import transitionJsunit from './TransitionJsunit.test.ets';
import priorityGestureJsunit from './PriorityGestureJsunit.test.ets';
import parallelGestureJsunit from './ParallelGestureJsunit.test.ets';
import blankJsunit from './BlankJsunit.test.ets';
import sharedTransitionJsunit from './SharedTransitionJsunit.test.ets';
import lineJsunit from './LineJsunit.test.ets';
import circleJsunit from './CircleJsunit.test.ets';
import aboutToDisappearJsunit from './AboutToDisappearJsunit.test.ets';
import timeStampJsunit from './TimeStampJsunit.test.ets';

export default function testsuite() {
  badgeJsunit();
  lineJsunit();
  progressJsunit();
  textJsunit();
  buttonJsunit();
  tabsJsunit();
  qrCodeJsunit();
  circleJsunit();
  gridJsunit();
  tapGesture();
  panGestureJsunit();
  canvasJsunit();
  canvas2Jsunit();
  longPressGestureJsUnit();
  responseRegionJsunit();
  animateJsunit();
  commonJsunit();
  spanJsunit();
  ratingJsunit();
  toggleJsunit();
  shapeJsunit();
  motionPathJsunit();
  scrollCodeJsunit();
  transitionJsunit();
  priorityGestureJsunit();
  parallelGestureJsunit();
  blankJsunit();
  sharedTransitionJsunit();
  aboutToDisappearJsunit();
  timeStampJsunit();
}