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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";

export default function ContentStorageJsunit() {
  describe('ContentStorage', function () {
    let contentStorage = new ContentStorage();

    /**
    * @tc.number  stateManagement_ContentStorage_setOrCreate_0100
    * @tc.name    stateManagement ContentStorage  setOrCreate validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_setOrCreate_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_setOrCreate_0100] START');
      contentStorage.setOrCreate<string>("key1", "value");
      contentStorage.setOrCreate<int>("key2", 11);
      contentStorage.setOrCreate<boolean>("key3", true);
      let key1 = contentStorage.get<string>("key1");
      let key2 = contentStorage.get<int>("key2");
      let key3 = contentStorage.get<boolean>("key3");
      expect(key1).assertEqual("value");
      expect(key2 == 11)
      .assertTrue;
      expect(key3)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_setOrCreate_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_get_0100
    * @tc.name    stateManagement ContentStorage  get validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_get_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_get_0100] START');
      let size = contentStorage.size();
      let key1 = contentStorage.get<string>("key1");
      let key2 = contentStorage.get<int>("key2");
      let key3 = contentStorage.get<boolean>("key3");
      expect(key1).assertEqual("value");
      expect(key2 == 11)
      .assertTrue;
      expect(key3)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_get_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_size_0100
    * @tc.name    stateManagement ContentStorage  size validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_size_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_size_0100] START');
      let size = contentStorage.size();
      expect(size == 3)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_size_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_has_0100
    * @tc.name    stateManagement ContentStorage  has validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_has_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_has_0100] START');
      let hasKey1 = contentStorage.has("key1");
      let hasKey2 = contentStorage.has("key2");
      let hasKey3 = contentStorage.has("key3");
      let hasKey4 = contentStorage.has("key4");
      expect(hasKey1)
      .assertTrue;
      expect(hasKey2)
      .assertTrue;
      expect(hasKey3)
      .assertTrue;
      expect(hasKey4)
      .assertFail;
      console.info('[stateManagement_ContentStorage_has_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_set_0100
    * @tc.name    stateManagement ContentStorage  set validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_set_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_set_0100] START');
      contentStorage.set<string>("key1", "value11");
      contentStorage.set<int>("key2", 12);
      contentStorage.set<boolean>("key3", false);
      contentStorage.set<string>("key4", "value4");
      let key1 = contentStorage.get<string>("key1");
      let key2 = contentStorage.get<int>("key2");
      let key3 = contentStorage.get<boolean>("key3");
      let key4 = contentStorage.get<string>("key4");

      let link = contentStorage.link("key1");
      let prop = contentStorage.prop("key1");
      let isMutable = contentStorage.isMutable("key1");
      console.info('[stateManagement_ContentStorage_set_0100] link = ' + link);
      console.info('[stateManagement_ContentStorage_set_0100] prop = ' + prop);
      console.info('[stateManagement_ContentStorage_set_0100] isMutable = ' + isMutable);

      expect(key1).assertEqual("value11");
      expect(key2 == 12)
      .assertTrue;
      expect(!key3)
      .assertTrue;
      expect(key4 == null)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_set_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_isMutable_0100
    * @tc.name    stateManagement ContentStorage  isMutable validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_isMutable_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_isMutable_0100] START');
      let isMutable = contentStorage.isMutable("key1");
      console.info('[stateManagement_ContentStorage_set_0100] isMutable = ' + isMutable);
      expect(isMutable)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_set_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_keys_0100
    * @tc.name    stateManagement ContentStorage  keys validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_keys_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_keys_0100] START');
      let keys = contentStorage.keys();
      console.info('[stateManagement_ContentStorage_keys_0100] keys = ' + keys);
      console.info('[stateManagement_ContentStorage_keys_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_delete_0100
    * @tc.name    stateManagement ContentStorage  delete validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_delete_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_delete_0100] START');
      let isDelete = contentStorage.delete("key1");
      expect(isDelete)
      .assertTrue;
      let size = contentStorage.size();
      expect(size == 2)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_delete_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_ContentStorage_clear_0100
    * @tc.name    stateManagement ContentStorage  clear validation
    * @tc.desc    Function test
    */
    it('stateManagement_ContentStorage_clear_0100', 0, function (done) {
      console.info('[stateManagement_ContentStorage_clear_0100] START');
      let isClear = contentStorage.clear("key1");
      expect(isClear)
      .assertTrue;
      let size = contentStorage.size();
      expect(size == 0)
      .assertTrue;
      console.info('[stateManagement_ContentStorage_clear_0100] END');
      done();
    });
  })
}
