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
import systemRouterJsunit from './SystemRouterJsunit.test.ets';
import systemAppJsunit from './SystemAppJsunit.test.ets';
import patternLockJsunit from './PatternLockJsunit.test.ets';
import customDialogControllerJsunit from './CustomDialogControllerJsunit.test.ets';
import ohosPromptJsunit from './OhosPromptJsunit.test.ets';
import videoJsunit from './VideoJsunit.test.ets';
import refreshJsunit from './RefreshJsunit.test.ets';
import richTextJsunit from './RichTextJsunit.test.ets';
import textPickerJsunit from './TextPickerJsunit.test.ets';
import textTimerJsunit from './TextTimerJsunit.test.ets';
import subscribedAbstractPropertyJsunit from './SubscribedAbstractPropertyJsunit.test.ets';
import contentStorageJsunit from './ContentStorage.test.ets';
import appStorageJsunit from './AppStorageJsunit.test.ets';
import subscribaleAbstractJsunit from './SubscribaleAbstractJsunit.test.ets';
import pathJsunit from './PathJsunit.test.ets';
import enumsLineCapStyleJsunit from './EnmusLineCapStyleJsunit.test.ets';
import enmusItemAlignJsunit from './EnmusItemAlignJsunit.test.ets';
import playModeJsunit from './PlayModeJsunit.test.ets';
import GestureModeJsunit from './GestureModeJsunit.test.ets';
import PanDirectionJsunit from './PanDirectionJsunit.test.ets';
import basicJsunit from './BasicJsunit.test.ets';
import focusJsunit from './FocusJsunit.test.ets';
import textAreaJsunit from './TextAreaJsunit.test.ets';
import dragEventJsunit from './DragEventJsunit.test.ets';
import calendarJsunit from './CalendarJsunit.test.ets';
import geometryTransitionJsunit from './GeometryTransitionJsunit.test.ets';
import textClockJsunit from './TextClockJsunit.test.ets';
import textInputJsunit from './TextInputJsunit.test.ets';
import polygonJsunit from './PolygonJsunit.test.ets';
import polylineJsunit from './PolylineJsunit.test.ets';
import imageJsunit from './ImageJsunit.test.ets';

export default function testsuite() {
  videoJsunit();
  enmusItemAlignJsunit();
  patternLockJsunit();
  dragEventJsunit();
  polygonJsunit();
  customDialogControllerJsunit();
  ohosPromptJsunit();
  richTextJsunit();
  refreshJsunit();
  imageJsunit();
  textPickerJsunit();
  textTimerJsunit();
  subscribedAbstractPropertyJsunit();
  contentStorageJsunit();
  textClockJsunit();
  appStorageJsunit();
  subscribaleAbstractJsunit();
  pathJsunit();
  enumsLineCapStyleJsunit();
  playModeJsunit();
  GestureModeJsunit();
  basicJsunit();
  focusJsunit();
  textAreaJsunit();
  textInputJsunit();
  polylineJsunit();
  calendarJsunit();
  PanDirectionJsunit();
  geometryTransitionJsunit();
  systemRouterJsunit();
  systemAppJsunit();
}