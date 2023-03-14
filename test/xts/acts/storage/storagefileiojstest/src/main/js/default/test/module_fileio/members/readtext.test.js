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

describe('fileio_readtext', function () {

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0000
   * @tc.name fileio_test_readtext_sync_000
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_readtext_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let str = fileio.readTextSync(fpath);
      expect(str == FILE_CONTENT).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_readtext_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0010
   * @tc.name fileio_test_readtext_sync_001
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_readtext_sync_001');
    let pos = 1;
    let len = 0;
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let str = fileio.readTextSync(fpath, { position: pos });
      expect(str == FILE_CONTENT.substring(pos, FILE_CONTENT.length + 1)).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_readtext_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0020
   * @tc.name fileio_test_readtext_sync_002
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_readtext_sync_002');
    let pos = 1;
    let len = FILE_CONTENT.length;
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let str = fileio.readTextSync(fpath, { position: pos, length: len });
      expect(str == FILE_CONTENT.substring(pos, len + 1)).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_readtext_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0030
   * @tc.name fileio_test_readtext_sync_003
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_003', 0, function () {
    try {
      fileio.readTextSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_readtext_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0040
   * @tc.name fileio_test_readtext_sync_004
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_readtext_sync_004');

    try {
      fileio.readTextSync(fpath);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_readtext_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_SYNC_0050
   * @tc.name fileio_test_readtext_sync_005
   * @tc.desc Test readtextSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_sync_005', 0, async function () {
    const invalidLength = 9999;
    let fpath = await nextFileName('fileio_test_readtext_sync_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.readTextSync(fpath, {
        length: invalidLength,
      });
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_ASYNC_0000
   * @tc.name fileio_test_readtext_async_000
   * @tc.desc Test readtextASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_readtext_async_000');
    let pos = 1;
    let len = FILE_CONTENT.length;
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.readText(fpath, { position: pos, length: len, encoding: 'UTF-8' }, function (err, str) {
        expect(str == FILE_CONTENT.substr(pos, len)).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_readtext_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_ASYNC_0010
   * @tc.name fileio_test_readtext_async_001
   * @tc.desc Test readtextASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_readtext_async_001');
    let pos = 1;
    let len = FILE_CONTENT.length;
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.readText(fpath, { position: 1 }, function (err, str) {
        expect(str == FILE_CONTENT.substr(pos, len)).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_readtext_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_TEXT_ASYNC_0020
   * @tc.name fileio_test_readtext_async_002
   * @tc.desc Test readtextASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_readtext_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_readtext_async_002');
    let pos = 1;
    let len = FILE_CONTENT.length;
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.readText(fpath, { position: 1 }, function (err, str) {
        expect(str == FILE_CONTENT.substr(pos, len)).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
      });
      fileio.readText(fpath, { position: 1 })
        .then(function (str) {
          expect(str == FILE_CONTENT.substr(pos, len)).assertTrue();
        })
        .catch(function (e) {
          expect(e == null).assertTrue();
        });
      done();
    } catch (e) {
      console.log('fileio_test_readtext_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
