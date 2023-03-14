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

import appearJsunit from './AppearJsunit.test.ets';
import areaChangeJsunit from './AreaChangeJsunit.test.ets';
import ellipseJsunit from './EllipseJsunit.test.ets';
import enableJsunit from './EnableJsunit.test.ets';
import gaugeJsunit from './GaugeJsunit.test.ets';
import globalJsunit from './GlobalJsunit.test.ets';
import girdContainerJsunit from './GridContainerJsunit.test.ets';
import gridSettingsJsunit from './GridSettingsJsunit.test.ets';
import imageEffectsJsunit from './ImageEffectsJsunit.test.ets';
import layoutConstraintsJsunit from './LayoutConstraintsJsunit.test.ets';
import listJsunit from './ListJsunit.test.ets';
import marqueeJsunit from './MarqueeJsunit.test.ets';
import overLayJsunit from './OverlayJsunit.test.ets';
import stepperJsunit from './StepperJsunit.test.ets';
import swiperJsunit from './SwiperJsunit.test.ets';
import textStyleJsunit from './TextStyleJsunit.test.ets';
import touchAbleJsunit from './TouchAbleJsunit.test.ets';
import touchJsunit from './TouchJsunit.test.ets';
import visibilityJsunit from './VisibilityJsunit.test.ets';
import zIndexJsunit from './ZIndexJsunit.test.ets';

export default function testsuite() {
  touchAbleJsunit();
  enableJsunit();
  appearJsunit();
  zIndexJsunit();
  ellipseJsunit();
  gaugeJsunit();
  globalJsunit();
  visibilityJsunit();
  girdContainerJsunit();
  areaChangeJsunit();
  imageEffectsJsunit();
  layoutConstraintsJsunit();
  listJsunit();
  marqueeJsunit();
  overLayJsunit();
  stepperJsunit();
  swiperJsunit();
  textStyleJsunit();
  touchJsunit();
  gridSettingsJsunit();
}