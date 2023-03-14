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
 
import router from '@system.router'

AppStorage.SetOrCreate("PropC", 1)
var link3 = AppStorage.Link("PropC");

AppStorage.SetOrCreate("PropA", 47);
var link1 = AppStorage.Link("PropA");
var link2 = AppStorage.Link("PropA");
var prop1 = AppStorage.Prop("PropA");

AppStorage.SetAndLink("PropB", 50)
var PropBValue = String(AppStorage.Get("PropB"))

AppStorage.SetAndProp("PropA", 47)
var PropAValue = String(AppStorage.Get("PropA"))
for (let key of AppStorage.Keys()) {
  var keys = key
}
var setPropAValue = String(AppStorage.Set("PropA", 20))
var deleteValue = String(AppStorage.Delete("PropC"))
var hasValue = String(AppStorage.Has("PropB"))
var isMutableValue = String(AppStorage.IsMutable("PropB"))


@Entry
@Component
struct appStorage_test {
  private content: string = "appStorage Page";
  @State str1: string = link1.get().toString();
  @State str2: string = link2.get().toString();
  @State str3: string = prop1.get().toString()
  @State str6: string = link3.get().toString();
  @State PropB: string = PropBValue
  @State PropA: string = PropAValue
  @State keys: string = keys
  @State str4: string = link1.get().toString()
  @State str5: string = setPropAValue
  @State str7: string = deleteValue
  @State str8: string= hasValue
  @State str9: string= isMutableValue

  onPageShow() {
    console.info('appStorage page show called');
  }

  onBuildDone() {
    console.info('appStorage page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Scroll() {
        Column() {
          Text(`${this.content}`)
            .fontSize(50)
            .fontWeight(FontWeight.Bold)
          Button(this.str1)
            .margin(15)
            .key('Button0_1')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str3)
            .margin(15)
            .key('Button0_2')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.PropB)
            .margin(15)
            .key('Button0_3')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.PropA)
            .margin(15)
            .key('Button0_4')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str8)
            .margin({ top: 10 })
            .key('Button0_5')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.keys)
            .margin(15)
            .key('Button0_6')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str4)
            .margin(15)
            .key('Button0_7')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str5)
            .margin(15)
            .key('Button0_8')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str6)
            .margin(15)
            .key('Button0_9')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Button(this.str7)
            .margin(15)
            .key('Button0_10')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
          Text(this.str9)
            .margin({ top: 10 })
            .key('Button0_12')
            .fontWeight(FontWeight.Bold)
            .fontSize(20)
        }
      }
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}
