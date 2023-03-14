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
struct link {
  private content: string = "link Page";
  @State isPlaying: boolean = false

  onPageShow() {
    console.info('link page show called');
  }

  onBuildDone() {
    console.info('link page build done called');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Scroll() {
        Column() {
          Text(`${this.content}`)
            .fontSize(50)
            .fontWeight(FontWeight.Bold)
          PlayButton({ stateEffect: $isPlaying })
          Button("Button1 stateEffect false", { type: ButtonType.Normal })
            .backgroundColor(Color.Gray)
            .stateEffect(this.isPlaying)
            .key("Button")
        }
      }
    }
    .key('flex')
    .width('100%')
    .height('100%')
  }
}

@Component
struct PlayButton {
  @Link stateEffect: boolean

  build() {
    Column() {

      Text("子组件button")
      Button("Button2 stateEffect ", { type: ButtonType.Capsule })
        .backgroundColor(Color.Pink)
        .stateEffect(this.stateEffect)
        .key("Button1")
    }
  }
}