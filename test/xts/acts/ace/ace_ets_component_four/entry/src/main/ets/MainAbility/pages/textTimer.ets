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

@Entry
@Component
struct TextTimerExample {
  myTimerController: TextTimerController = new TextTimerController()
  @State format: string = 'hh:mm:ss.ms'

  build() {
    Column() {
      TextTimer({ controller: this.myTimerController })
        .key('textTimer')
        .format(this.format)
        .fontColor(Color.Black)
        .fontSize(50)
        .onTimer((utc: number, elapsedTime: number) => {
          console.info('textTimer notCountDown utc is：' + utc + ', elapsedTime: ' + elapsedTime)
        })
      Row() {
        Button("start").onClick(() => {
          this.myTimerController.start();
        });
        Button("pause").onClick(() => {
          this.myTimerController.pause();
        });
        Button("reset").onClick(() => {
          this.myTimerController.reset();
        });
      }
    }
  }
}