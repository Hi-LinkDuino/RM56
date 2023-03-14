// @ts-nocheck
/**
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
struct ImageExample {
  @State fontSize: number = 9;
  @State fontColor: string = "0xCCCCCC";
  @State textAlign: TextAlign = TextAlign.Start;
  @State textOverflow: TextOverflow = TextOverflow.Clip;
  @State decoration: TextDecorationType = TextDecorationType.None;

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
      Text('string_hello')
        .fontSize(this.fontSize)
        .fontColor(this.fontColor)
        .textAlign(this.textAlign)
        .textOverflow({ overflow: this.textOverflow })
        .decoration({ type: this.decoration, color: Color.Red })
        .key('text')
      Image($rawfile('test.png'))
        .key('image')
    }
    .height(600)
    .width(350)
    .padding({ left: 35, right: 35, top: 35 })
  }
}