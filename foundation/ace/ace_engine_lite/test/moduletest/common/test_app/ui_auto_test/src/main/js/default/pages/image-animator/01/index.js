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
import router from '@system.router';

export default {
  data: {
    frames: [{
      src: 'common/asserts/numbers/0.png'
    }, {
      src: 'common/asserts/numbers/2.png'
    }, {
      src: 'common/asserts/numbers/3.png'
    }, {
      src: 'common/asserts/numbers/4.png'
    }, {
      src: 'common/asserts/numbers/5.png'
    }, {
      src: 'common/asserts/numbers/6.png'
    }, {
      src: 'common/asserts/numbers/7.png'
    }, {
      src: 'common/asserts/numbers/8.png'
    }, {
      src: 'common/asserts/numbers/9.png'
    }]
  },
  handleStart() {
    execCommand(this, "start");
  },
  handleStop() {
    execCommand(this, "stop");
  },
  handleGoHome() {
    router.replace({
      uri: '/'
    });
  }
}

function execCommand(vm, command) {
  const keys = Object.keys(vm.$refs);
  keys.forEach((key) => vm.$refs[key][command]());
}