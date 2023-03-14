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
import checkboxJsunit from './CheckboxJsunit.test.ets';
import ohosMatrix4Jsunit from './OhosMatrix4Jsunit.test.ets';
import selectJsunit from './SelectJsunit.test.ets';
import fillModeJsunit from './FillModeJsunit.test.ets';
import enumsImageJsunit from './EnmusImageJsunit.test.ets';
import checkboxGroupJsunit from './CheckboxGroupJsunit.test.ets';
import sliderJsunit from './SliderJsunit.test.ets';
import edgeEffectJsunit from './EdgeEffectJsunit.test.ets';
import transFormJsunit from './TransFormJsunit.test.ets';
import visibilityJsunit from './VisibilityJsunit.test.ets';
import positionSettingJsunit from './PositionSettingJsunit.test.ets';
import shapeClippingJsunit from './ShapeClippingJsunit.test.ets';
import backgroundAndSizeSettingJsunit from './BackgroundAndSizeSettingJsunit.test.ets';
import borderJsunit from './BorderJsunit.test.ets';
import colorGradientJsunit from './ColorGradientJsunit.test.ets';
import enmusHorizontalAlignJsunit from './EnmusHorizontalAlignJsunit.test.ets';

export default function testsuite() {
  checkboxJsunit();
  selectJsunit();
  fillModeJsunit();
  enumsImageJsunit();
  ohosMatrix4Jsunit();
  sliderJsunit();
  edgeEffectJsunit();
  checkboxGroupJsunit();
  transFormJsunit();
  borderJsunit();
  colorGradientJsunit();
  visibilityJsunit();
  positionSettingJsunit();
  backgroundAndSizeSettingJsunit();
  shapeClippingJsunit();
  enmusHorizontalAlignJsunit();
}