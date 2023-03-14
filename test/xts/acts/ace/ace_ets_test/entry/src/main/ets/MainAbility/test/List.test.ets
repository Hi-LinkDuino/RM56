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
 
import appStorageJsunit from './AppStorageJsunit.test.ets';
import linkJsunit from './LinkJsunit.test.ets';
import propJsunit from './PropJsunit.test.ets';
import alphabetIndexerJsunit from './AlphabetIndexerJsunit.test.ets';
import listJsunit from './ListJsunit.test.ets';
import tabJsunit from './TabJsunit.test.ets';
import swiperJsunit from './SwiperJsunit.test.ets';
import panelJsunit from './PanelJsunit.test.ets';
import navigatorJsunit from './NavigatorJsunit.test.ets';
import rowJsunit from './RowJsunit.test.ets';
import listItemJsunit from './ListItemJsunit.test.ets';
import gridItemJsunit from './GridItem.test.ets';
import gridJsunit from './GridJsnuit.test.ets';
import videoJsunit from './VideoJsunit.test.ets';
export default function testsuite() {
  alphabetIndexerJsunit()
  rowJsunit()
  gridJsunit()
  gridItemJsunit()
  navigatorJsunit()
  panelJsunit()
  swiperJsunit()
  tabJsunit()
  propJsunit()
  linkJsunit()
  appStorageJsunit()
  videoJsunit()
  listItemJsunit()
  listJsunit()
}