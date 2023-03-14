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
struct qrcode_textcase {
  @State Color: Color = Color.Pink

  onPageShow() {
    console.info('Qrcode page show called');
  }

  onBuildDone() {
    console.info('Qrcode page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(`Qrcode page`)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      QRCode('hello world')
        .color("#FFFFC0CB")
        .backgroundColor(this.Color)
        .width("200")
        .height("100")
        .key('qrcode')
    }
  }
}


