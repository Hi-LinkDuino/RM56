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
import events_emitter from '@ohos.emitter'

@Entry
@Component
struct SearchExample {
  @State changevalue: string= ''
  @State submitvalue: string= ''
  @State controller: SearchController= new SearchController()
  @State textFont:Font=10
  @State searchButton:number=10

  private stateChangCallBack = (eventData) => {
    console.info("search page state change called:" + JSON.stringify(eventData));
    if (eventData != null) {
      if (eventData.data.textFont != null) {
        this.textFont = eventData.data.textFont;
      }
      if (eventData.data.searchButton != null) {
        this.searchButton = eventData.data.searchButton;
      }
    }
  }

  onPageShow() {
    console.info('search page show called');
    var stateChangeEvent = {
      eventId: 190,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Column() {
      Text(this.submitvalue)
      Text(this.changevalue)
      Search({ value: '', placeholder: 'input content', controller: this.controller })
        .searchButton('search')
        .textFont(this.textFont)
        .searchButton(this.searchButton)
        .key('search')
        .placeholderColor(Color.Red)
        .placeholderFont({
          size: 10,
          weight: 10,
          family: 'serif',
          style: FontStyle.Normal
        })
        .onSubmit((value: string) => {
          this.submitvalue = value
        })
        .onChange((value: string) => {
          this.changevalue = value
        })
    }
  }
}











