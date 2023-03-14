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
import animator from '@ohos.animator';
import prompt from '@system.prompt';

@Entry
@Component
struct ColumnExample {
  @State divWidth: number = 120;
  @State divHeight: number = 120;
  @State animation: AnimatorResult = animator.createAnimator({
    duration: 3000,
    fill: 'forwards',
    direction: "normal",
    begin: 200,
    end: 270
  });

  onPageShow() {
    this.animation.onrepeat(() => {
      var repeatoptions = {
        duration: 2000,
        iterations: 1,
        direction: 'alternate',
        begin: 180,
        end: 240
      };
      this.animation.update(repeatoptions);
      this.animation.play();
    });
  }

  build() {
    Column() {
      Column() {
        Column() {
          Column().width(this.divWidth).height(this.divHeight).backgroundColor(0xAFEEEE)
        }.width('100%')
      }.width('100%').padding({ top: 5 })

      Column() {
        Row() {
          Button('play').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            this.animation.onframe((value) => {
              this.divWidth = value
              this.divHeight = value
            });
            this.animation.play();
          });
          Button('update').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            var newoptions = {
              duration: 5000,
              iterations: 2,
              begin: 120,
              end: 180
            };
            this.animation.update(newoptions);
            this.animation.play();
          });
        }.margin({ top: 5 })

        Row() {
          Button('pause').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            this.animation.pause();
          });
          Button('finish').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            this.animation.onfinish(() => {
              prompt.showToast({
                message: 'finish'
              })
            });
            this.animation.finish();
          });
        }.margin({ top: 5 })

        Row() {
          Button('cancel').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            this.animation.cancel();
          });
          Button('reverse').borderRadius(8).backgroundColor(0x317aff).width(90).onClick(() => {
            this.animation.reverse();
          });
        }.margin({ top: 5 })
      }.margin({ top: 30 })
    }
  }
}
