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
struct column {
  @State active: boolean = false
  private content: string = "Column Page"

  onPageShow() {
    console.info('Column page show called');
  }

  onBuildDone() {
    console.info('Column page build done called');
  }

  build() {
    Column({ space: 5 }) {
      Text('alignItems(Center)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Column() {
        Column().width('50%').height(50).backgroundColor(0xAFEEEE)
        Column().width('50%').height(50).backgroundColor(0x00FFFF)
      }.alignItems(HorizontalAlign.Center).width('90%').border({ width: 1 }).key('column')

      Text('alignItems(Start)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Column({ space: '5' }) {
        Column().width('50%').height(50).backgroundColor(0xAFEEEE)
        Column().width('50%').height(50).backgroundColor(0x00FFFF)
      }.alignItems(HorizontalAlign.Start).width('90%').border({ width: 1 }).key('column1')

      Text('alignItems(End)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Column() {
        Column().width('50%').height(50).backgroundColor(0xAFEEEE)
        Column().width('50%').height(50).backgroundColor(0x00FFFF)
      }.alignItems(HorizontalAlign.End).width('90%').border({ width: 1 }).key('column2')
    }.width('100%').padding({ top: 5 })
  }
}
