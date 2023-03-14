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
struct alphabetIndexer {
  private value: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']
  private value1: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']
  @State active: boolean = false
  private content: string = "AlphabetIndexer Page"

  onPageShow() {
    console.info('alphabetIndexer page show called');
  }

  onBuildDone() {
    console.info('alphabetIndexer page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Text("AlphabetIndexer0")
      AlphabetIndexer({ arrayValue: this.value, selected: 0 })
        .color('#FF00DDDD')
        .selectedColor(0xffffff)
        .key('alphabetIndexer')
        .popupColor('#FF737373')
        .selectedBackgroundColor('#FF0000E6')
        .popupBackground('#FF00DDDD')
        .usingPopup(true)
        .selectedFont({ size: 16, weight: FontWeight.Regular })
        .popupFont({ size: 30, weight: FontWeight.Bolder, style: FontStyle.Normal })
        .itemSize(28)
        .alignStyle(IndexerAlign.Left)
        .onSelected((index: number) => {
          console.info(this.value[index] + ' on selected')
        })
      Text("AlphabetIndexer1")
      AlphabetIndexer({ arrayValue: this.value1, selected: 0 })
        .selectedColor(0xffffff)
        .key('alphabetIndexer1')
        .popupColor('#FF737373')
        .selectedBackgroundColor('#FF0000E6')
        .popupBackground('#FF00DDDD')
        .usingPopup(false)
        .selectedFont({ size: 16, weight: FontWeight.Lighter })
        .popupFont({ size: 30, weight: FontWeight.Bold, style: FontStyle.Italic })
        .itemSize(28)
        .alignStyle(IndexerAlign.Right)
        .margin({ left: 50 })
        .onSelected((index: number) => {
          console.info(this.value[index] + ' on selected')
        })
    }
  }
}
