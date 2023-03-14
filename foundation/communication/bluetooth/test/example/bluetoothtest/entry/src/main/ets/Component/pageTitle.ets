/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { TestData } from '../MainAbility/model/testData'
import Router from '@system.router';
/**
 * Page-Title Of Bluetooth test
 */
@Component
export struct PageTitle {
  private testItem: TestData = null
  private detail: string
  build() {
    Flex({ alignItems: ItemAlign.Start }) {
      Image($r('app.media.Back'))
        .width("10%")
        .height("100%")
      Text(this.testItem != null ? this.testItem.detail : this.detail)
        .fontSize(21.8)
        .margin({ left: 17.4 })
    }
    .height(61)
    .backgroundColor('#FFedf2f5')
    .padding({ top: 23, bottom: 15, left: 18 })
    .borderColor($r("app.color.black"))
    .backgroundColor($r("app.color.blue"))
    .onClick(() => {
      Router.back()
    })
  }
}