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
 
import router from '@system.router'

@Entry
@Component
struct stack_testcase {
  @State alignContent: Alignment = Alignment.Bottom
  private content: string = "Stack Page";

  onPageShow() {
    console.info('Stack page show called');
  }

  onBuildDone() {
    console.info('Stack page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Stack({ alignContent: this.alignContent }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }
      .width('100%')
      .height(80)
      .margin({ top: 5 })
      .key('stack')
      .onClick(() => {
        router.back()
      })

      Stack({ alignContent: Alignment.BottomStart }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(80).margin({ top: 5 })
      .key('stack1')

      Stack({ alignContent: Alignment.BottomEnd }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(80).margin({ top: 5 })
      .key('stack2')

      Stack({ alignContent: Alignment.Start }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(80).margin({ top: 5 })
      .key('stack3')

      Stack({ alignContent: Alignment.Center }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(80).margin({ top: 5 })
      .key('stack4')

      Stack({ alignContent: Alignment.End }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(80).margin({ top: 5 })
      .key('stack5')

      Stack({ alignContent: Alignment.Top }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(60).margin({ top: 5 })
      .key('stack6')

      Stack({ alignContent: Alignment.TopStart }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(60).margin({ top: 5 })
      .key('stack7')

      Stack({ alignContent: Alignment.TopEnd }) {
        Text('First child, show in bottom').width('90%').height('100%').backgroundColor(0xd2cab3).align(Alignment.Top)
        Text('Second child, show in top').width('70%').height('60%').backgroundColor(0xc1cbac).align(Alignment.Top)
      }.width('100%').height(60).margin({ top: 5 })
      .key('stack8')
    }
    .key('flex')
    .width('100%')
    .height('100%')

  }
}

