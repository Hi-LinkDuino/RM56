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
struct FormComponentPage {
  @State dimension:FormDimension = FormDimension.Dimension_2_2
  build() {
    Column(){

      FormComponent({
        id:0,
        name:"four_four",
        bundle:"com.example.myapplication",
        ability:"com.example.myapplication.MainAbility",
        module:"fa_widget",
        dimension:this.dimension,
        temporary:true
      })
        .size({width:100,height:100})
        .moduleName("fa_widget")
        .dimension(this.dimension)
        .allowUpdate(true)
        .visibility(Visibility.Visible)
        .onAcquired((e)=>{
          console.log('onAcquired id : '+ e.id)
        })
        .onRouter(()=>{
          console.log('onRouter : ')
        })
        .onError((error)=>{
          console.log('error code :'+error.errcode);
          console.log('error msg :'+error.msg);
        })


      Text('卡片格式 ：')
        .fontSize(15)
        .width('100%')
        .backgroundColor(0xFAEEE0)
        .height(40)
        .textAlign(TextAlign.Start)
        .fontWeight(FontWeight.Bolder)
      Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.Start }) {
        Button('1X2')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.dimension = FormDimension.Dimension_1_2
          })
        Button('2X2')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.dimension = FormDimension.Dimension_2_2
          })
        Button('2X4')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.dimension = FormDimension.Dimension_2_4
          })
        Button('4X4')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.dimension = FormDimension.Dimension_4_4
          })

      }
      .width('90%')
      .margin({ top: 5, bottom: 5, left: 10, right: 10 })

    }
  }
}