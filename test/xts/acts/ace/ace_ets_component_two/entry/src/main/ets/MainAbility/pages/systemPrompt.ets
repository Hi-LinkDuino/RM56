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
import prompt from '@system.prompt';

@Entry
@Component
struct promptExample {
  @State text: string = 'item1'
  @State color: string = '#000000'
  build(){
    Column(){
      Button('showActionMenu')
      .key('button1')
      .backgroundColor('#FF0B8FE2')
      .fontSize(20)
      .margin({top: 20})
      .onClick(()=>{
        prompt.showActionMenu({
              title: 'Title Info',
              buttons: [
                {
                  text: this.text,
                  color: this.color,
                },
              ],
              success: function(data) {
                console.log('dialog success callback，click button : ' + data.tapIndex);
              },
              fail: function(data) {
                console.log('dialog fail callback' + data.errMsg);
              },
            });
      })
      Button('showDialog')
      .key('button2')
      .backgroundColor('#FF0B8FE2')
      .fontSize(20)
      .margin({top: 20})
      .onClick(()=> {
        prompt.showDialog({
          title: 'Title Info',
          message: 'Message Info',
          buttons: [
            {
              text: this.text,
              color: this.color,
            },
          ],
          success: function(data) {
            console.log('dialog success callback，click button : ' + data.index);
          },
          cancel: function() {
            console.log('dialog cancel callback');
          },
        });
      })
      }
    }
}