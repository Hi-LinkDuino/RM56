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
struct divider_textcase {
  @State vertical: boolean = true
  @State color: Color = Color.Red
  @State strokeWidth: number = 7
  @State lineCap: LineCapStyle = LineCapStyle.Butt
  @State active: boolean = false
  private content: string = "Divider Page";

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
      Divider()
        .lineCap(LineCapStyle.Butt)
        .strokeWidth(this.strokeWidth)
        .color(this.color)
        .vertical(this.vertical)
        .height(150)
        .key('divider')
        .onClick(() => {
          router.back()
        })
      Divider()
        .lineCap(LineCapStyle.Round)
        .vertical(false)
        .color(Color.Green)
        .strokeWidth(10)
        .height(150)
        .key('divider1')
      Divider()
        .lineCap(LineCapStyle.Square)
        .color(Color.Green)
        .vertical(true)
        .strokeWidth(10)
        .height(150)
        .key('divider2')
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}


