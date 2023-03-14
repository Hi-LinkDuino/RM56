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

export default class Utils {
  static sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve(reject)
      }, time)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  static getNowTime() {
    return new Date().getTime();
  }

  /**
   * 接口调用时间
   * @param startTime 接口调用开始时间
   * @param endTime 接口调用结束时间
   */
  static getDurationTime(msg, startTime, endTime) {
    console.info(msg + 'Get Interface startTime: ' + startTime);
    console.info(msg + 'Get Interface endTime: ' + endTime);
    var duration = (endTime - startTime)
    console.info(msg + 'Get Interface Duration: ' + duration);
    return duration;
  }
}




