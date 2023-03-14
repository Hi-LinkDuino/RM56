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
struct span_textcase {

  onPageShow() {
    console.info('Span page show called');
  }

  onBuildDone() {
    console.info('Span page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Column() {
        Text() {
          Span('Test Span demo')
            .decoration({ type: TextDecorationType.LineThrough, "color": Color.Red })
            .textCase(TextCase.UpperCase)
            .fontColor(Color.Blue)
            .fontSize("20fp")
            .letterSpacing(5)
            .fontFamily("sans-serif")
            .fontWeight(FontWeight.Bold)
            .fontStyle(FontStyle.Italic)
            .key('span1')
          Span('The second test Span demo')
            .textCase(TextCase.LowerCase)
            .fontColor(Color.Black)
            .letterSpacing('5')
            .fontSize(30)
            .key('span2')
          Span('The third test Span demo')
            .textCase(TextCase.Normal)
            .fontColor(Color.Yellow)
            .fontSize("30fp")
            .key('span3')
        }
      }
      .width(300)
      .height(300)
      .borderColor(Color.Pink)
      .borderWidth(4)
    }
  }
}



