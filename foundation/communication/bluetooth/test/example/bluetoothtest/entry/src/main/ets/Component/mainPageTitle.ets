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
// @ts-nocheck

/**
 * Main-Page Title
 */
@Component
export struct MainPageTitle {
  build() {
    Flex({ alignItems: ItemAlign.Start }) {
      Text('蓝牙调试助手')
        .fontSize(26)
        .margin({ left: 17.4 })
        .padding({ top: 13 })
    }
    .height(80)
    .backgroundColor($r("app.color.blue"))
    .padding({ top: 12, bottom: 5, left: 28 })
  }
}