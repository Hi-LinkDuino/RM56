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
  fileio,
  FILE_CONTENT,
  prepareFile,
  nextFileName,
  isIntNum,
  isString,
  isBoolean,
  describe,
  it,
  expect,
} from '../../Common';

describe('fileio_dir_close_read', function () {

  /**
   * @tc.number SUB_DF_FILEIO_DIR_CLOSE_ASYNC_0000
   * @tc.name fileio_test_dir_close_async_000
   * @tc.desc Test Dir.closeAsync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dir_close_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_close_async_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      expect((await dd.close()) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_dir_close_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_close_Async_0001
   * @tc.name fileio_test_dir_close_async_001
   * @tc.desc Test Dir.closeAsync() interface.
   */
  it('fileio_test_dir_close_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_close_async_001') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      dd.close(function (err) {
        expect(!!err).assertTrue();
      });
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_dir_close_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
