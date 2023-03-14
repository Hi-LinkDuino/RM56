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

import { fileio, nextFileName, describe, it, expect } from '../../Common';

describe('fileio_dir_opendir_close', function () {

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_CLOSEDIR_SYNC_0000
   * @tc.name fileio_dir_open_close_sync_000
   * @tc.desc Test opendirSync() and Dir.closeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_dir_open_close_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_dir_open_close_sync_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_dir_open_close_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_CLOSEDIR_SYNC_0010
   * @tc.name fileio_dir_open_close_sync_001
   * @tc.desc Test opendirSync() interface.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_dir_open_close_sync_001', 0, async function () {
    let dpath = await nextFileName('fileio_dir_open_close_sync_001') + 'd';

    try {
      fileio.opendirSync(dpath);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_dir_open_close_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_CLOSEDIR_ASYNC_0000
   * @tc.name fileio_dir_open_close_async_000
   * @tc.desc Test opendirAsync() and Dir.closeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_dir_open_close_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_open_close_async_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_open_close_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_CLOSEDIR_ASYNC_0010
   * @tc.name fileio_dir_open_close_async_001
   * @tc.desc Test opendirAsync() and Dir.closeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_dir_open_close_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_open_close_async_001') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      fileio.opendir(dpath, function (error, dd) {
        expect(dd !== null).assertTrue();
        expect(dd.closeSync() == null).assertTrue();
        expect(fileio.rmdirSync(dpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_dir_open_close_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
