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
export const commonFunc = function(obj, e) {
  if (obj.flag) {
    obj.color = 16766720;
    obj.flag = false;
  } else {
    obj.color = 32768;
    obj.flag = true;
  }
  obj.pointX = e.point.x;
  obj.pointY = e.point.y;
  obj.timeStamp = e.timeStamp;
};
export const dataCommon = {
  color: "#5265ef",
  flag: false,
  pointX: 0,
  pointY: 0,
  timeStamp: 0
};