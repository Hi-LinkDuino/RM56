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
struct button_testcase {
  @State stateEffect: boolean = true
  @State ButtonType: ButtonType = ButtonType.Circle
  private content: string = "Button Page";

  onPageShow() {
    console.info('Button page show called');
  }

  onBuildDone() {
    console.info('Button page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`${this.content}`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Button('1.value', { type: ButtonType.Circle, stateEffect: this.stateEffect })
        .fontSize(12)
        .fontWeight(FontWeight.Bold)
        .fontColor(0xCCCCCC)
        .height(40)
        .width(40)
        .key('button')
        .backgroundColor('#0D9FFB')
        .onClick(() => {
          router.back()
        })
        .padding(10)
      Button() {
        Text("2.Capsule")
      }
      .type(ButtonType.Capsule)
      .stateEffect(false)
      .backgroundColor('#0D9FFB')
      .padding(10)
      .key('button1')

      Button() {
        Text("3.Normal")
      }.type(ButtonType.Normal)
      .backgroundColor('#0D9FFB')
      .key('button2')

    }.key('flex')
    .width('100%')
    .height('100%')
  }
}

