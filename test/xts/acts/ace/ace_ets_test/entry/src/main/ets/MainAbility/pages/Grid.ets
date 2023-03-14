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
 
@Entry
@Component
struct grid_testcase {
  @State Number: String[] = ['0', '1', '2', '3', '4']
  @State active: boolean = false

  build() {
    Column() {
      Navigator({ target: "pages/index", type: NavigationType.Back }) {
        Column({ space: 5 }) {
          Grid() {
            ForEach(this.Number, (day: string) => {
              ForEach(this.Number, (day: string) => {
                GridItem() {
                  Text(day)
                    .fontSize(16)
                    .backgroundColor(0xF9CF93)
                    .width('100%')
                    .height('100%')
                    .textAlign(TextAlign.Center)
                }
              }, day => day)
            }, day => day)
          }
          .columnsTemplate('1fr 1fr 1fr 1fr 1fr')
          .rowsTemplate('1fr 1fr 1fr 1fr 1fr')
          .columnsGap(10)
          .rowsGap(10)
          .width('90%')
          .backgroundColor(0xFAEEE0)
          .height(300)
          .key('Grid')
        }.width('100%').margin({ top: 5 })
      }.active(this.active)
      .params({
        data: 19
      })
    }.width(300)
    .height(350)
    .borderColor(Color.Pink)
    .borderWidth(2)
  }
}