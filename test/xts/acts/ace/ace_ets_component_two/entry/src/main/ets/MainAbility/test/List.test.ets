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
import bindMenuJsunit from './BindMenuJsunit.test.ets';
import bindPopupJsunit from './BindPopupJsunit.test.ets';
import directionJsunit from './DirectionJsunit.test.ets';
import listItemJsunit from './ListItemJsunit.test.ets';
import navigatorCodeJsunit from './NavigatorCodeJsunit.test.ets';
import pieceJsunit from './PieceJsunit.test.ets';
import dividerJsunit from './DividerJsunit.test.ets';
import systempromptJsunit from './SystempromptJsunit.test.ets';
import configurationJsunit from './ConfigurationJsunit.test.ets';
import sourceTypeJsunit from './SourceTypeJsunit.test.ets';
import pageTransitionJsunit from './PageTransitionJsunit.test.ets';
import navigationJsunit from './NavigationJsunit.test.ets';
import loadingProgressJsunit from './LoadingProgressJsunit.test.ets';
import radioJsunit from './RadioJsunit.test.ets';
import rowJsunit from './RowJsunit.test.ets';
import scrollBarJsunit from './ScrollBarJsunit.test.ets';
import searchJsunit from './SearchJsunit.test.ets';
import placementJsunit from './PlacementJsunit.test.ets';
import imageAnimatorJsunit from './ImageAnimatorJsunit.test.ets'
import animationStatusJsunit from './AnimationStatusJsunit.test.ets';
import curvesJsunit from './CurvesJsunit.test.ets';
import gradientDirectionJsunit from './GradientDirectionJsunit.test.ets';
import ohosRouterJsunit from './OhosRouterJsnuit.test.ets';
import flexDirectionJsunit from './FlexDirectionJsunit.test.ets';
import flexWrapJsunit from './FlexWrapJsunit.test.ets';
import datePickerJsunit from './DatePickerJsunit.test.ets';
import direction2Jsunit from './Direction2Jsunit.test.ets';

export default function testsuite() {
  navigationJsunit();
  ohosRouterJsunit();
  pageTransitionJsunit();
  datePickerJsunit();
  flexWrapJsunit();
  flexDirectionJsunit();
  gradientDirectionJsunit();
  dividerJsunit();
  directionJsunit();
  direction2Jsunit();
  animationStatusJsunit();
  searchJsunit();
  scrollBarJsunit();
  bindMenuJsunit();
  bindPopupJsunit();
  configurationJsunit();
  curvesJsunit();
  imageAnimatorJsunit();
  loadingProgressJsunit();
  listItemJsunit();
  navigatorCodeJsunit();
  pieceJsunit();
  placementJsunit();
  radioJsunit();
  rowJsunit();
  systempromptJsunit();
  sourceTypeJsunit();
}