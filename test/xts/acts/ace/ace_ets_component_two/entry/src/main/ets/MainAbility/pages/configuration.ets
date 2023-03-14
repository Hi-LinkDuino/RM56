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
import Configuration from '@system.configuration';
import events_emitter from '@ohos.emitter';

@Entry
@Component
struct promptExample {
  build(){
    Column(){
      Button('click')
      .key('button')
      .backgroundColor('#FF0B8FE2')
      .fontSize(20)
      .margin({top: 20})
        .onClick(()=>{
          console.info('button is start');
          let localeInfo = Configuration.getLocale();
          console.info('localeInfo result is' + JSON.stringify(localeInfo));
          try{
            var backData = {
              data: {
                "COUNTRY": localeInfo.countryOrRegion,
                "DIR":localeInfo.dir
              }
            }
            console.log("backData is" + JSON.stringify(backData.data));
            var backEvent = {
              eventId: 78,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("confituration start to emit action state");
            events_emitter.emit(backEvent, backData);
          }catch{
            console.info("confituration emit action state err: " + JSON.stringify(err.message));
          }
        })
    }
  }
}