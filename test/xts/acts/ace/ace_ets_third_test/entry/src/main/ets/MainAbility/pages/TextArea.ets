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
struct TextAreaExample {
  @State text: string = ''
  @State text1: string = 'TextArea1'
  @State text2: string = 'TextArea2'
  @State text3: string = 'TextArea3'

  onPageShow() {
    console.info('textArea page show called');
  }

  onBuildDone() {
    console.info('textArea page build done called');
  }

  build() {
    Column() {
      Text(this.text1).width('90%').height(60).fontSize(40)
      TextArea({ placeholder: 'Please entry keywords', text: this.text })
        .placeholderColor('#FFC0C0C0')
        .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
        .textAlign(TextAlign.Start)
        .caretColor('#FF000000')
        .height(60)
        .fontSize(40)
        .fontWeight(FontWeight.Bold)
        .fontFamily('cursive')
        .fontStyle(FontStyle.Italic)
        .fontColor('#FFFF0000')
        .onChange((value: string) => {
          this.text1 = value
        })
        .onCopy((value: string) => {
        })
        .key('textArea1')

      Text(this.text2).width('90%').height(60).fontSize(40)
      TextArea({ placeholder: 'Please entry keywords', text: this.text })
        .placeholderColor('#FFC0C0C0')
        .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
        .textAlign(TextAlign.Center)
        .caretColor('#FF000000')
        .height(60)
        .fontSize(40)
        .fontWeight(FontWeight.Bold)
        .fontFamily('cursive')
        .fontStyle(FontStyle.Italic)
        .fontColor('#FFFF0000')
        .onChange((value: string) => {
          this.text2 = value
        })
        .key('textArea2')

      Text(this.text3).width('90%').height(60).fontSize(40)
      TextArea({ placeholder: 'Please entry keywords', text: this.text })
        .placeholderColor('#FFC0C0C0')
        .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
        .textAlign(TextAlign.End)
        .caretColor('#FF000000')
        .height(60)
        .fontSize(40)
        .fontWeight(FontWeight.Bold)
        .fontFamily('cursive')
        .fontStyle(FontStyle.Italic)
        .fontColor('#FFFF0000')
        .onChange((value: string) => {
          this.text3 = value
        })
        .key('textArea3')
    }
    .width('100%')
    .height('100%')
  }
}
