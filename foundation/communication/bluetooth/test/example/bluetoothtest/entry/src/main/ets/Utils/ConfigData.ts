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

export class ConfigData {

  TAG='Bluetooth_Test '

  WH_100_100 = '100%';
  WH_25_100 = '25%';
  WH_30_100 = '30%';
  WH_33_100 = '33%';
  WH_35_100 = '35%';
  WH_40_100 = '40%';
  WH_45_100 = '45%';
  WH_50_100 = '50%';
  WH_55_100 = '55%';
  WH_83_100 = '83%';
  WH_90_100 = '90%';

  value_20 = 20;
  font_20 = 20;

  MAX_LINES_1 = 1;
  MAX_LINES_2 = 2;
  MAX_LINES_3 = 3;
  DURATION_TIME = 200;
  FUNCTION_TYPE_HDC = 4;
}

let configData = new ConfigData();
export default configData as ConfigData;