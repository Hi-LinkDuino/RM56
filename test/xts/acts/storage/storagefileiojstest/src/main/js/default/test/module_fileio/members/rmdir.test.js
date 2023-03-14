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

describe('fileio_rmdir', function () {

  /**
   * @tc.number SUB_DF_FILEIO_RMDIR_ASYNC_0000
   * @tc.name fileio_test_rmdir_async_000
   * @tc.desc Test rmdirAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rmdir_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_rmdir_async_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(await fileio.rmdir(dpath) == null).assertTrue();
      fileio
        .access(dpath)
        .then(function () {
          expect(null).assertFail();
          done();
        })
        .catch((err) => {
          expect(!!err).assertTrue();
          done();
        });
    } catch (e) {
      console.log('fileio_test_rmdir_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
  
  /**
   * @tc.number SUB_DF_FILEIO_RMDIR_ASYNC_0010
   * @tc.name fileio_test_rmdir_async_001
   * @tc.desc Test rmdirAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rmdir_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_rmdir_async_001') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      fileio.rmdir(dpath, function (err) {
        fileio
          .access(dpath)
          .then(function () {
            expect(null).assertFail();
            done();
          })
          .catch((err) => {
            expect(!!err).assertTrue();
            done();
          });
      });
    } catch (e) {
      console.log('fileio_test_rmdir_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_RMDIR_ASYNC_0020
   * @tc.name fileio_test_rmdir_async_002
   * @tc.desc Test rmdirAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rmdir_async_002', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_rmdir_async_002') + 'd';
    expect(prepareFile(dpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.rmdir(dpath, async function (err) {
        expect(!!err).assertTrue();
        expect(await fileio.access(dpath) == null).assertTrue();
        await fileio.unlink(dpath);
        done();
      });
    } catch (e) {
      console.log('fileio_test_rmdir_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_RMDIR_ASYNC_0030
   * @tc.name fileio_test_rmdir_async_003
   * @tc.desc Test rmdirAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rmdir_async_003', 0, function (done) {
    try {
      fileio.rmdir('1.js', function (err) {
        expect(!!err).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_rmdir_async_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

    /**
   * @tc.number SUB_DF_FILEIO_RMDIR_SYNC_0000
   * @tc.name fileio_test_rmdir_sync_000
   * @tc.desc Test rmdirSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rmdir_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_rmdir_sync_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_rmdir_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
