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

describe('fileio_copyfile', function () {

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_SYNC_0000
   * @tc.name fileio_copy_file_sync_000
   * @tc.desc Test copyFileSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_copy_file_sync_000');
    let fpathTarget = fpath + 'tgt';
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) == null).assertTrue();
    } catch (e) {
      console.log('fileio_copy_file_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_SYNC_0010
   * @tc.name fileio_copy_file_sync_001
   * @tc.desc Test copyFileSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_copy_file_sync_001');
    let fpathTarget = fpath + 'tgt';

    try {
      fileio.copyFileSync(fpath, fpathTarget);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_copy_file_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_SYNC_0020
   * @tc.name fileio_copy_file_sync_002
   * @tc.desc Test copyFileSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_sync_002', 0, function () {
    try {
      fileio.copyFileSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_copy_file_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_ASYNC_0000
   * @tc.name fileio_copy_file_async_000
   * @tc.desc Test copyFileAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_copy_file_async_000');
    let fpathTarget = fpath + 'tgt';
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(await fileio.copyFile(fpath, fpathTarget) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_copy_file_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_ASYNC_0010
   * @tc.name fileio_copy_file_async_001
   * @tc.desc Test copyFileAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_copy_file_async_000');
    let fpathTarget = fpath + 'tgt';
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.copyFile(fpath, fpathTarget, function (error) {
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        expect(fileio.unlinkSync(fpathTarget) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_copy_file_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_ASYNC_0020
   * @tc.name fileio_copy_file_async_002
   * @tc.desc Test copyFileAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_copy_file_async_002');
    let fpathTarget = fpath + 'tgt';
    try {
      expect(await fileio.copyFile(fpath, fpathTarget).indexOf('<pending>') > -1).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_ASYNC_0030
   * @tc.name fileio_copy_file_async_003
   * @tc.desc Test copyFileAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_copy_file_async_003');
    try {
      fileio
        .copyFile(1, fpath)
        .then(function (err) {
        })
        .catch(function (e) {
          expect(!!e).assertTrue();
          done();
        });
    } catch (e) {
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_COPY_FILE_ASYNC_0040
   * @tc.name fileio_copy_file_async_004
   * @tc.desc Test copyFileAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_copy_file_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_copy_file_async_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(await fileio.copyFile(fpath, 1) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(null).assertFail();
      done();
    } catch (e) {
      done();
    }
  });
});
