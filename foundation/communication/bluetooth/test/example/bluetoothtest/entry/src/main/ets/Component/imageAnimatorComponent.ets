/**
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import ComponentConfig from './componentConfig';

/**
 * ImageAnimator component
 */
@Component
export default struct ImageAnimatorComponent {
  private imageWidth: number | Resource = 0
  private imageHeight: number | Resource = 0
  private whtl: number | string = ComponentConfig.value_20;

  build() {
    Column() {
      ImageAnimator()
      .images([
        {
          src: '../MainAbility/res/image/hdpi/ic_loading01.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }, {
          src: '../MainAbility/res/image/hdpi/ic_loading02.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }, {
          src: '../MainAbility/res/image/hdpi/ic_loading03.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }, {
          src: '../MainAbility/res/image/hdpi/ic_loading04.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }, {
          src: '../MainAbility/res/image/hdpi/ic_loading05.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }, {
          src: '../MainAbility/res/image/hdpi/ic_loading06.png',
          duration: ComponentConfig.DURATION_TIME,
          width: this.whtl,
          height: this.whtl,
          top: this.whtl,
          left: this.whtl
        }])
      .width(this.imageWidth)
      .height(this.imageHeight)
      .iterations(-1)
      .fixedSize(true)
      .reverse(true)
      .fillMode(FillMode.None)
      .state(AnimationStatus.Running)
    }
  }
}