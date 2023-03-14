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
struct columnSplit {
  private content: string = "ColumnSplit Page"

  onPageShow() {
    console.info('ColumnSplit page show called');
  }

  onBuildDone() {
    console.info('ColumnSplit page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Scroll() {
        Column() {
          Text(`${this.content}`)
            .fontSize(50)
            .fontWeight(FontWeight.Bold)
          Column() {
            Text('ColumnSplit').fontSize(9).fontColor(0xCCCCCC).width('90%')
            ColumnSplit() {
              Text('1').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
              Text('2').width('100%').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center)
              Text('3').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
              Text('4').width('100%').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center)
              Text('5').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
            }.key('columnSplit')
            .resizeable(true)
            .width('90%').height('60%')

          }.width("100%")

          Column() {
            Text('ColumnSplit1').fontSize(9).fontColor(0xCCCCCC).width('90%')
            ColumnSplit() {
              Text('1').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
              Text('2').width('100%').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center)
              Text('3').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
              Text('4').width('100%').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center)
              Text('5').width('100%').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center)
            }.key('columnSplit1')
            .resizeable(false)
            .width('90%').height('60%')

          }.width("100%")
        }
      }
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}
