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
 
import badgeJsunit from './BadgeJsunit.test.ets';
import dividerJsunit from './DividerJsunit.test.ets'
import buttonJsunit from './ButtonJsunit.test.ets'
import blankJsunit from './BlankJsunit.test.ets'
import datapanelJsunit from './DataPanelJsunit.test.ets'
import textJsunit from './TextJsunit.test.ets'
import rowSplitJsunit from './RowSplitJsunit.test.ets'
import ScrollJsunit from './ScrollJsunit.test.ets'
import StackJsunit from './StackJsunit.test.ets'
import imageJsunit from './ImageJsunit.test.ets'
import progressJsunit from './ProgressJsunit.test.ets'
import qrcodeJsunit from './QrcodeJsunit.test.ets'
import ratingJsunit from './RatingJsunit.test.ets'
import spanJsunit from './SpanJsunit.test.ets'
import sliderJsunit from './SliderJsunit.test.ets'
import gridContainerJsunit from './GridContainerJsunit.test.ets';
import flexJsunit from './FlexJsunit.test.ets';
import counterJsunit from './CounterJsunit.test.ets';
import columnSplitJsunit from './ColumnSplitJsunit.test.ets';
import columnJsunit from './ColumnJsunit.test.ets';
export default function testsuite() {
  buttonJsunit()
  blankJsunit()
  datapanelJsunit()
  dividerJsunit()
  textJsunit()
  rowSplitJsunit()
  ScrollJsunit()
  StackJsunit()
  imageJsunit()
  progressJsunit()
  qrcodeJsunit()
  ratingJsunit()
  spanJsunit()
  sliderJsunit()
  badgeJsunit()
  columnJsunit()
  columnSplitJsunit()
  counterJsunit()
  flexJsunit()
  gridContainerJsunit()

}