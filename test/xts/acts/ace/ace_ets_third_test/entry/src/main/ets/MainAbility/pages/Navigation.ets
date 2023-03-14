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
struct NavigationExample {
  private arr: number[]= [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
  @State hideBar: boolean = true

  onPageShow() {
    console.info('navigation page show called');
  }

  onBuildDone() {
    console.info('navigation build done called');
  }

  @Builder NavigationMenus1() {
    Image('common/icon.png')
      .width(25)
      .height(25)
      .margin({ left: 30 })
  }

  build() {
    Column() {
      Navigation() {
        Search({ value: '', placeholder: '' }).width('90%').margin(0)
        List({ space: 5, initialIndex: 0 }) {
          ForEach(this.arr, (item) => {
            ListItem() {
              Text('' + item)
                .width('90%')
                .height(40)
                .backgroundColor('#3366CC')
                .fontSize(15)
                .textAlign(TextAlign.Center)
            }.editable(true)
          }, item => item)
        }
        .listDirection(Axis.Vertical)
        .height(200)
        .margin({ top: 10, left: 20 })
        .width('100%')
      }
      .key('navigation1')
      .height('50%')
      .title('title')
      .subTitle('subtitle')
      .menus(this.NavigationMenus1)
      .titleMode(NavigationTitleMode.Full)
      .hideTitleBar(false)
      .hideToolBar(false)
      .hideBackButton(false)
      .toolBar({ items: [
        { value: 'app' },
        { value: 'add' },
        { value: 'collect' }
      ] })
      .onTitleModeChange((titleModel: NavigationTitleMode) => {
        console.log('titleMode:' + titleModel)
      })

      Navigation() {
        Search({ value: '', placeholder: '' }).width('90%').margin(0)
        List({ space: 5, initialIndex: 0 }) {
          ListItem() {
            Text('Navigation1')
              .width('90%')
              .height(50)
              .backgroundColor('#3366CC')
              .fontSize(15)
              .textAlign(TextAlign.Center)
          }
        }
        .listDirection(Axis.Vertical)
        .height(50)
        .margin({ top: 10, left: 20 })
        .width('100%')
      }
      .height('10%')
      .key('navigation2')
      .hideTitleBar(true)
      .hideBackButton(true)
      .titleMode(NavigationTitleMode.Mini)

      Navigation() {
        Search({ value: '', placeholder: '' }).width('90%').margin(25)
        List({ space: 5, initialIndex: 0 }) {
            ListItem() {
              Text('Navigation2')
                .width('90%')
                .height(50)
                .backgroundColor('#3366CC')
                .fontSize(15)
                .textAlign(TextAlign.Center)
            }
        }
        .listDirection(Axis.Vertical)
        .height(50)
        .margin({ top: 10, left: 20 })
        .width('100%')
      }
      .key('navigation3')
      .height('10%')
      .titleMode(NavigationTitleMode.Free)
    }
  }
}


