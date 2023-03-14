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
struct gridContainer {
  @State sizeType: SizeType = SizeType.XS
  @State active: boolean = false
  private content: string = "GridContainer Page"

  onPageShow() {
    console.info('GridContainer page show called');
  }

  onBuildDone() {
    console.info('GridContainer page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Scroll() {
        Column({ space: 5 }) {
          GridContainer({ columns: 12, sizeType: this.sizeType, gutter: 10, margin: 20 }) {
            Row() {
              Text('1')
                .useSizeType({
                  xs: { span: 6, offset: 0 },
                  sm: { span: 2, offset: 0 },
                  md: { span: 2, offset: 0 },
                  lg: { span: 2, offset: 0 }
                })
                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
              Text('2').gridSpan(2)

                .height(50).backgroundColor(0x00BFFF).textAlign(TextAlign.Center)
              Text('3').gridOffset(3)

                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
            }
          }.key('gridContainer')
          .width('90%')
          .alignItems(HorizontalAlign.Start)

          Text('Click Simulate to change the device width').fontSize(9).width('90%').fontColor(0xCCCCCC)
          Row() {
            Button('XS')
              .onClick(() => {
                this.sizeType = SizeType.XS
              }).backgroundColor(0x317aff)
            Button('SM')
              .onClick(() => {
                this.sizeType = SizeType.SM
              }).backgroundColor(0x317aff)
            Button('MD')
              .onClick(() => {
                this.sizeType = SizeType.MD
              }).backgroundColor(0x317aff)
            Button('LG')
              .onClick(() => {
                this.sizeType = SizeType.LG
              }).backgroundColor(0x317aff)
          }

          GridContainer({ columns: "auto", sizeType: SizeType.MD, gutter: '10', margin: '20' }) {
            Row() {
              Text('1')
                .useSizeType({
                  xs: { span: 6, offset: 0 },
                  sm: { span: 2, offset: 0 },
                  md: { span: 2, offset: 0 },
                  lg: { span: 2, offset: 0 }
                })
                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
              Text('2').gridSpan(2)

                .height(50).backgroundColor(0x00BFFF).textAlign(TextAlign.Center)
              Text('3').gridOffset(3)

                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
            }
          }.key('gridContainer1')
          .width('90%')
          .alignItems(HorizontalAlign.Center)

          GridContainer({ columns: 12, sizeType: SizeType.SM, gutter: 10, margin: 20 }) {
            Row() {
              Text('1')
                .useSizeType({
                  xs: { span: 6, offset: 0 },
                  sm: { span: 2, offset: 0 },
                  md: { span: 2, offset: 0 },
                  lg: { span: 2, offset: 0 }
                })
                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
              Text('2').gridSpan(2)

                .height(50).backgroundColor(0x00BFFF).textAlign(TextAlign.Center)
              Text('3').gridOffset(3)

                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
            }
          }.key('gridContainer2')
          .width('90%')
          .alignItems(HorizontalAlign.End)

          GridContainer({ columns: 12, sizeType: SizeType.LG, gutter: 10, margin: 20 }) {
            Row() {
              Text('1')
                .useSizeType({
                  xs: { span: 6, offset: 0 },
                  sm: { span: 2, offset: 0 },
                  md: { span: 2, offset: 0 },
                  lg: { span: 2, offset: 0 }
                })
                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
              Text('2').gridSpan(2)

                .height(50).backgroundColor(0x00BFFF).textAlign(TextAlign.Center)
              Text('3').gridOffset(3)

                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
            }
          }.key('gridContainer3')
          .width('90%')
          .alignItems(HorizontalAlign.End)

          GridContainer({ columns: 12, sizeType: SizeType.Auto, gutter: 10, margin: 20 }) {
            Row() {
              Text('1')
                .useSizeType({
                  xs: { span: 6, offset: 0 },
                  sm: { span: 2, offset: 0 },
                  md: { span: 2, offset: 0 },
                  lg: { span: 2, offset: 0 }
                })
                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
              Text('2').gridSpan(2)

                .height(50).backgroundColor(0x00BFFF).textAlign(TextAlign.Center)
              Text('3').gridOffset(3)

                .height(50).backgroundColor(0x4682B4).textAlign(TextAlign.Center)
            }
          }.key('gridContainer4')
          .width('90%')
          .alignItems(HorizontalAlign.End)

        }.width('100%').margin({ top: 5 })
      }
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}
