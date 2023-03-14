// @ts-nocheck
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
struct SlelectExample {

  onPageShow() {
    console.info('select page show called');
  }

  onBuildDone() {
    console.info('select page build done called');
  }

  build() {
    Column() {
      Select([{value:'aaa',icon: "/common/1.png"},
              {value:'bbb',icon: "/common/2.png"},
              {value:'ccc',icon: "/common/3.png"},
              {value:'ddd',icon: "/common/4.png"}])
        .selected(2)
        .value('TTT')
        .font({size: 30, weight:400, family: 'serif', style: FontStyle.Normal })
        .selectedOptionFont({size: 40, weight: 500, family: 'serif', style: FontStyle.Normal })
        .optionFont({size: 30, weight: 400, family: 'serif', style: FontStyle.Normal })
        .fontColor(0x317aff)
        .optionBgColor(0xFFFFFF)
        .optionFontColor(0x000000)
        .selectedOptionBgColor(0xCCCCCC)
        .selectedOptionFontColor(0x000000)
        .key('select')
        .onSelected((index:number)=>{
          console.info("Select:" + index)
        })
    }

  }
}