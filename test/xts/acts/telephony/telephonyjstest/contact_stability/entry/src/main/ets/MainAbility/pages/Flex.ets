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
struct flex {
  private content: string = "Flex Page"

  onPageShow() {
    console.info('Flex page show called');
  }

  onBuildDone() {
    console.info('Flex page build done called');
  }

  build() {
    Column() {
      Column({ space : 2 }) {
        Text('direction:Column').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          direction: FlexDirection.Column,
          wrap: FlexWrap.NoWrap,
          justifyContent: FlexAlign.Center,
          alignItems:ItemAlign.Start,
          alignContent: FlexAlign.Start,
        }) {
          Text('1').width('100%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('100%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('100%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex')
        .height(70)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('direction:Row').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          direction: FlexDirection.Row,
          wrap: FlexWrap.Wrap,
          justifyContent: FlexAlign.Start,
          alignItems:ItemAlign.Start,
          alignContent: FlexAlign.Center,
        }) {
          Text('1').width('20%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('20%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('20%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex1')
        .height(70)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('direction:RowReverse').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          direction: FlexDirection.RowReverse,
          wrap: FlexWrap.Wrap,
          justifyContent: FlexAlign.End,
          alignItems:ItemAlign.End,
          alignContent: FlexAlign.End,
        }) {
          Text('1').width('50%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('50%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('50%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex2')
        .height(90)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('justifyContent:SpaceBetween').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          justifyContent: FlexAlign.SpaceBetween,
          wrap: FlexWrap.Wrap,
        }) {
          Text('1').width('100%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('100%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('100%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex3')
        .height(100)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('justifyContent AND alignContent').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          justifyContent: FlexAlign.SpaceEvenly,
          alignContent: FlexAlign.SpaceEvenly,
          wrap: FlexWrap.Wrap,
        }) {
          Text('1').width('50%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('50%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('50%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex4')
        .height(90)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('justifyContent AND alignContent').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({
          justifyContent: FlexAlign.SpaceAround,
          alignContent: FlexAlign.SpaceAround,
          wrap: FlexWrap.Wrap,
        }) {
          Text('1').width('50%').height(20).backgroundColor(0xF5DEB3)
          Text('2').width('50%').height(20).backgroundColor(0xD2B48C)
          Text('3').width('50%').height(20).backgroundColor(0xF5DEB3)
        }
        .key('flex5')
        .height(90)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)
      }.width('100%').margin({ top: 5 })
    }.width('100%')
  }
}
