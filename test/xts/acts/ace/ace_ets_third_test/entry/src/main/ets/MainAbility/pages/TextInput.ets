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
struct TextInputExample {
  @State text: string = ''
  @State textNew: string = ''

  onPageShow() {
    console.info('textInput page show called');
  }

  onBuildDone() {
    console.info('textInput page build done called');
  }

  build() {
    Column({ space: 10 }) {
      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 1', text: this.text })
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Go)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .caretColor('#FF000000')
          .height(60)
          .fontSize(40)
          .fontWeight(FontWeight.Bold)
          .fontFamily('cursive')
          .fontStyle(FontStyle.Italic)
          .fontColor('#FFFF0000')
          .maxLength(10)
          .onChange((value: string) => {
            this.textNew = value
          })
          .onSubmit((enterKey) => {
            this.textNew = 'onSubmit'
          })
          .onEditChanged((isEditing) => {
            this.textNew = 'onEditChanged'
          })
          .onCopy((value: string) => {
          })

          .key('textInput1')

        TextInput({ placeholder: 'textInput 2', text: this.text })
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Search)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput2')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 3', text: this.text })
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Send)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput3')

        TextInput({ placeholder: 'textInput 4', text: this.text })
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Next)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput4')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 5', text: this.text })
          .type(InputType.Normal)
          .enterKeyType(EnterKeyType.Done)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput5')

        TextInput({ placeholder: 'textInput 6', text: this.text })
          .type(InputType.Password)
          .enterKeyType(EnterKeyType.Go)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput6')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 7', text: this.text })
          .type(InputType.Password)
          .enterKeyType(EnterKeyType.Search)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput7')

        TextInput({ placeholder: 'textInput 8', text: this.text })
          .type(InputType.Password)
          .enterKeyType(EnterKeyType.Send)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput8')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 9', text: this.text })
          .type(InputType.Password)
          .enterKeyType(EnterKeyType.Next)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput9')

        TextInput({ placeholder: 'textInput 10', text: this.text })
          .type(InputType.Password)
          .enterKeyType(EnterKeyType.Done)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput10')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 11', text: this.text })
          .type(InputType.Email)
          .enterKeyType(EnterKeyType.Go)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput11')
        TextInput({ placeholder: 'textInput 12', text: this.text })
          .type(InputType.Email)
          .enterKeyType(EnterKeyType.Search)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput12')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 13', text: this.text })
          .type(InputType.Email)
          .enterKeyType(EnterKeyType.Send)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput13')

        TextInput({ placeholder: 'textInput 14', text: this.text })
          .type(InputType.Email)
          .enterKeyType(EnterKeyType.Next)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput14')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 15', text: this.text })
          .type(InputType.Email)
          .enterKeyType(EnterKeyType.Done)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput15')

        TextInput({ placeholder: 'textInput 16', text: this.text })
          .type(InputType.Number)
          .enterKeyType(EnterKeyType.Go)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput16')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 17', text: this.text })
          .type(InputType.Number)
          .enterKeyType(EnterKeyType.Search)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput17')
        TextInput({ placeholder: 'textInput 18', text: this.text })
          .type(InputType.Number)
          .enterKeyType(EnterKeyType.Send)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput18')
      }

      Flex({ direction: FlexDirection.Row }) {
        TextInput({ placeholder: 'textInput 19', text: this.text })
          .type(InputType.Number)
          .enterKeyType(EnterKeyType.Next)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput19')
        TextInput({ placeholder: 'textInput 20', text: this.text })
          .type(InputType.Number)
          .enterKeyType(EnterKeyType.Done)
          .placeholderColor('#FFC0C0C0')
          .placeholderFont({ size: 30, weight: 400, family: 'Microsoft YaHei', style: FontStyle.Normal })
          .key('textInput20')
      }
    }
    .width('100%')
    .height('100%')
  }
}
