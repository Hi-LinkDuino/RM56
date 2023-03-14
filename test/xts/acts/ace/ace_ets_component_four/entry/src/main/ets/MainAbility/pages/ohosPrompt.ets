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
import prompt from '@ohos.prompt'

@Entry
@Component
struct OhosPrompt {
  build() {
    Column() {
      Button("get Point").padding(40)
        .onClick(() => {
          prompt.showToast({ message: "sucessful", duration: 200 })
          this.onClickValue = true;
          try {
            var backData = {
              data: {
                "matrix1": "[100, 10]",
                "ACTION": this.onClickValue
              }
            }
            var backEvent = {
              eventId: 142,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("click to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("click action state err: " + JSON.stringify(err.message))
          }
          prompt.showActionMenu({
            title: 'Title Info',
            buttons: [
              {
                text: 'item1',
                color: '#666666',
              }
            ],
            success: function(data) {
              console.info('dialog success callback，click button : ' + data.tapIndex);
            },
            fail: function(data) {
              console.info('dialog fail callback' + data.errMsg);
            },
          })
          prompt.showDialog({
            title: 'Title Info',
            message: 'Message Info',
            buttons: [
              {
                text: 'showDialog',
                color: '#666666',
              },
            ],
            success: function(data) {
              console.info('dialog success callback，click button : ' + data.index);
            },
            cancel: function() {
              console.info('dialog cancel callback');
            },
          });
        }).backgroundColor(0x2788D9).key('rotate5')
    }
    .width('100%')
    .height('100%')
  }
}