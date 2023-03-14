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
import router from '@system.router';

@Entry
@Component
struct OhosRouterA {
  build() {
    Flex({direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center}) {
      Text('A Page')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Button('Push')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
      Button('back')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
      Button('clear')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
      Button('getLength')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
      Button('enableAlertBeforeBackPage')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
      Button('disableAlertBeforeBackPage')
        .key('pushButtonA')
        .backgroundColor('#FF0B8FE2')
        .fontSize(20)
        .margin({top: 20})
        .onClick(() => {
          router.push({
            uri: 'pages/ohosRouterB',
            params: {
              data: 'B Page'
            }
          });
        });
    }.width('100%').height('100%')
  }
}