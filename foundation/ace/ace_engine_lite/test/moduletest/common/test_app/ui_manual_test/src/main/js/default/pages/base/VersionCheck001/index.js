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
    versionStr: 'null',
    commitStr: 'null',
    buildStamp: 'null'
  },
  exitPage() {
    router.replace({ uri:"pages/index/index" });
  },
  nextPage() {
    router.replace({ uri:"pages/Base/PercentSupport/index" });
  },
  onShow() {
    if(getAceVersion && typeof(getAceVersion) == "function") {
      this.versionStr = getAceVersion();
    }
    if(getAceCommit && typeof(getAceCommit) == "function") {
      this.commitStr = getAceCommit();
    }
    if(getAceStamp && typeof(getAceStamp) == "function") {
      this.buildStamp = getAceStamp();
    }
  }
}
