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
struct gridItem_testcasee {
  @State numbers: string[] = Array.apply(null, Array(16)).map(function (item, i) {
    return i.toString()
  })
  @State active: boolean = false
  build() {
    Column() {
      Navigator({ target: "pages/index", type: NavigationType.Back }) {
        Column() {
          Grid() {
            GridItem() {
              Text('4')
                .fontSize(16)
                .backgroundColor(0xFAEEE0)
                .width('100%')
                .height('100%')
                .textAlign(TextAlign.Center)
            }
            .rowStart(1)
            .rowEnd(4)
            .columnStart(1)
            .columnEnd(2)
            .forceRebuild(false)
            .key('GridItem1')

            ForEach(this.numbers, (item) => {
              GridItem() {
                Text(item)
                  .fontSize(16)
                  .backgroundColor(0xF9CF93)
                  .width('100%')
                  .height('100%')
                  .textAlign(TextAlign.Center)
              }.forceRebuild(false)
            }, item => item)
            GridItem() {
              Text('5')
                .fontSize(16)
                .backgroundColor(0xDBD0C0)
                .width('100%')
                .height('100%')
                .textAlign(TextAlign.Center)
            }.columnStart(1)
            .columnEnd(5)
            .forceRebuild(false)
            .key('GridItem2')
          }
          .columnsTemplate('1fr 1fr 1fr 1fr 1fr')
          .rowsTemplate('1fr 1fr 1fr 1fr 1fr')
          .width('90%').height(300)
        }.width('100%').margin({ top: 5 })
      }.active(this.active)
      .params({
        data: 20
      })
    }.width(300)
    .height(350)
    .borderColor(Color.Pink)
    .borderWidth(2)
  }
}

