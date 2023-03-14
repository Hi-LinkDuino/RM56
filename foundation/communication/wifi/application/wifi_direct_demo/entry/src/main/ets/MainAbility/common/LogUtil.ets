/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
import BaseModel from '../model/BaseModel.ets';

export class LogUtil extends BaseModel {
  TAG() {
    return '------------ P2P ------------ ';
  }

  debug(msg) {
    console.info(this.TAG() + msg);
  }

  log(msg) {
    console.log(this.TAG() + msg);

  }

  info(msg) {
    console.info(this.TAG() + msg);
  }

  warn(msg) {
    console.warn(this.TAG() + msg);
  }

  error(msg) {
    console.error(this.TAG() + msg);
  }
}

let mLogUtil = new LogUtil();

export default mLogUtil as LogUtil;

