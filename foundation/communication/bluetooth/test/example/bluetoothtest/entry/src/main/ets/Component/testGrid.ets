/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { TestData } from '../MainAbility/model/testData'
import { TestGridItem } from './testGridItem'
/**
 * TestGrid
 */
@Component
export struct TestGrid {
  private testItems: TestData[]
  private gridRowTemplate: string = ''
  private heightValue: number

  aboutToAppear() {
    var rows = Math.round(this.testItems.length / 2);
    this.gridRowTemplate = '1fr '.repeat(rows);
    this.heightValue = rows * 192 - 8;
  }

  build() {
    Scroll() {
      Grid() {
        ForEach(this.testItems, (item: TestData) => {
          GridItem() {
            TestGridItem({ testItem: item })
          }
        }, (item: TestData) => item.id.toString())
      }
      .rowsTemplate(this.gridRowTemplate)
      .columnsTemplate('1fr 1fr')
      .columnsGap(8)
      .rowsGap(8)
      .height(this.heightValue)
    }
    .scrollBar(BarState.Off)
    .padding({ left: 16, right: 16 })
  }
}