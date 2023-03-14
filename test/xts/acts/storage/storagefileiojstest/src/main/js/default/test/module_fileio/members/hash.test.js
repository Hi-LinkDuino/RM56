/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
  fileio, FILE_CONTENT, prepareFile, nextFileName,
  describe, it, expect,
} from '../../Common';

describe('fileio_hash', function () {

  /**
   * @tc.number SUB_DF_FILEIO_HASH_ASYNC_0000
   * @tc.name fileio_test_hash_async_000
   * @tc.desc Test hashAsync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_hash_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_hash_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let str = await fileio.hash(fpath,'md5');
      console.log('fileio_test_hash_async_000 hash value is ' + str);
      expect(str == '5EB63BBBE01EEED093CB22BB8F5ACDC3').assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_hash_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});