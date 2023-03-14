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

import events_emitter from '@ohos.emitter';

@Entry
@Component
struct NavigatorCode {
  @State active: boolean = false;
  @State Text: string = 'news';
  @State typeValue: NavigationType = NavigationType.Back;
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      var navigationTypeValue = eventData.data.typeValue;
      console.info("navigationTypeValue:" + navigationTypeValue);
      if (navigationTypeValue != null) {
        this.typeValue = navigationTypeValue;
        console.info("this.typeValue:" + this.typeValue);
      } else {
        console.info("navigationTypeValue is null or empty " + NavigatorCode);
      }
    } else {
      console.info("NavigatorCode page color not change called:" + JSON.stringify(eventData));
    }
  }

  onPageShow() {
    console.info('navigatorCode page show called');
    var stateChangeEvent = {
      eventId: 137,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent = {
      eventId: 138,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
      Navigator({ target: 'pages/NavigatorDetailCode', type: NavigationType.Push }) {
        Text('Go to ' + this.Text + ' page').width('100%').textAlign(TextAlign.Center)
      }.params({ text: this.Text }).key('button')

      Navigator() {
        Text('Back to previous page').width('100%').textAlign(TextAlign.Center)
      }.active(this.active).key('NavigatorType')
      .onClick(() => {
        this.active = true
      })
    }.height(150).width(350).padding(35)
  }
}