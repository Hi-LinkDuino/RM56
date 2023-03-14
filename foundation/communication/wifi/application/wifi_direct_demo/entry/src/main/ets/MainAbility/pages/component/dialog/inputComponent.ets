/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import prompt from '@system.prompt';
/**
 * 自定义输入弹窗
 */
@CustomDialog
export default struct InputComponent {
  title: string = '提示';
  cancelVal: string= '取消';
  confirmVal: string= '确定';
  inputValLength: number = 15; //输入字符长度限制
  @State inputHint: string= '';
  @State inputValue: string = '';
  @State inputType: number = InputType.Password;
  controller: CustomDialogController;
  eventConnect: (value: string) => void;

//  checkCharacter(str: string): boolean{
//    let pattern = new RegExp("[`+-\\~!@#$^&*()=|{}':;',\\[\\].<>《》/?~！@#￥……&*（）——|{}【】‘；：”“'。，、？ ]");
//    if (pattern.test(str)) {
//      return true;
//    }
//    return false;
//  }

  build() {
    Column() {
      Text(this.title)
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.Black)
        .margin({
          top: 15,
          bottom: 15,
          left: 20,
          right: 20
        })

      TextInput({ placeholder: this.inputHint, text: this.inputValue })
        .placeholderColor(Color.Black)
        .placeholderFont({ size: 20 })
        .height(50)
        .borderRadius(6)
        .type(this.inputType)
        .margin({
          top: 10,
          left: 15,
          right: 15,
          bottom: 10
        })
        .padding({ left: 10, right: 10 })
        .onChange((value: string) => {
          this.inputValue = value;
        })
        .height(60)
      //      Divider()
      //        .color('#bcbcbc')
      //        .height(1)
      //        .margin({
      //          left: 15,
      //          right: 15
      //        })

      Row() {
        Button(this.cancelVal, { type: ButtonType.Capsule, stateEffect: true })
          .fontSize(17)
          .fontColor(Color.Black)
          .backgroundColor(Color.White)
          .layoutWeight(1)
          .stateEffect(false)
          .padding({
            top: 5,
            bottom: 5
          })
          .onClick(() => {
            this.controller.close();
          })
        Divider()
          .width(1)
          .height(30)
          .vertical(true)
          .color('#bcbcbc')
        Button(this.confirmVal, { type: ButtonType.Capsule, stateEffect: true })
          .fontSize(17)
          .fontColor(Color.Black)
          .backgroundColor(Color.White)
          .layoutWeight(1)
          .stateEffect(false)
          .padding({
            top: 5,
            bottom: 5
          })
          .onClick(() => {
            if (this.inputValue == '') {
              prompt.showToast({ message: '请输入' + this.title })
            } else if (this.inputValue.length > this.inputValLength) {
              prompt.showToast({ message: '输入内容超出限制' })
            } else {
              this.controller.close();
              this.eventConnect(this.inputValue);
            }
          })
      }
    }
    .backgroundColor(Color.White)
    .borderRadius(10)
  }
}