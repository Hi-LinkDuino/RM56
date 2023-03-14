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

import fileio from '@ohos.fileio';
import {
  describe,
  it,
  expect
}
  from 'deccjsunit/index'
import {
  FILE_CONTENT,
  prepareFile,
  fileToReadOnly,
  fileToWriteOnly,
  nextFileName,
  randomString
}
  from './Common'

describe('fileIOTestStream', function () {

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0000
   * @tc.name fileio_test_stream_create_stream_sync_000
   * @tc.desc Function of API, mode = r. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_000');
    expect(prepareFile(fpath, 'hello')).assertTrue();
    expect(fileToReadOnly(fpath)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_stream_create_stream_sync_000 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0100
   * @tc.name fileio_test_stream_create_stream_sync_001
   * @tc.desc Function of API, mode = r. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_001');
    try {
      fileio.createStreamSync(fpath, 'r');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0200
   * @tc.name fileio_test_stream_create_stream_sync_002
   * @tc.desc Function of API, mode = w. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_002');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'w');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0300
   * @tc.name fileio_test_stream_create_stream_sync_003
   * @tc.desc Function of API, mode = w. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_003');
    try {
      let ss = fileio.createStreamSync(fpath, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0400
   * @tc.name fileio_test_stream_create_stream_sync_004
   * @tc.desc Function of API, mode = a. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_004');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'a');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_004 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0500
   * @tc.name fileio_test_stream_create_stream_sync_005
   * @tc.desc Function of API, mode = a. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_005');
    try {
      let ss = fileio.createStreamSync(fpath, 'a');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0600
   * @tc.name fileio_test_stream_create_stream_sync_006
   * @tc.desc Function of API, mode = r+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0700
   * @tc.name fileio_test_stream_create_stream_sync_007
   * @tc.desc Function of API, mode = r+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_007');
    try {
      fileio.createStreamSync(fpath, 'r+');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0800
   * @tc.name fileio_test_stream_create_stream_sync_008
   * @tc.desc Function of API, mode = w+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_008');
    expect(prepareFile(fpath, '')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'w+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_0900
   * @tc.name fileio_test_stream_create_stream_sync_009
   * @tc.desc Function of API, mode = w+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_009');
    try {
      let ss = fileio.createStreamSync(fpath, 'w+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1000
   * @tc.name fileio_test_stream_create_stream_sync_010
   * @tc.desc Function of API, mode = a+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_010');
    expect(prepareFile(fpath, 'a')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'a+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1100
   * @tc.name fileio_test_stream_create_stream_sync_011
   * @tc.desc Function of API, mode = a+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_011');
    try {
      let ss = fileio.createStreamSync(fpath, 'a+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1200
   * @tc.name fileio_test_stream_create_stream_sync_012
   * @tc.desc Function of API, mode = ab. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_012');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToReadOnly(fpath)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'rb');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1300
   * @tc.name fileio_test_stream_create_stream_sync_013
   * @tc.desc Function of API, mode = ab. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_013', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_013');
    try {
      fileio.createStreamSync(fpath, 'rb');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_013 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1400
   * @tc.name fileio_test_stream_create_stream_sync_014
   * @tc.desc Function of API, mode = wb. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_014', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_014');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'wb');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1500
   * @tc.name fileio_test_stream_create_stream_sync_015
   * @tc.desc Function of API, mode = wb. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_015', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_015');
    try {
      let ss = fileio.createStreamSync(fpath, 'wb');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1600
   * @tc.name fileio_test_stream_create_stream_sync_016
   * @tc.desc Function of API, mode = ab. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_016', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_016');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'ab');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_stream_create_stream_sync_016 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1700
   * @tc.name fileio_test_stream_create_stream_sync_017
   * @tc.desc Function of API, mode = ab. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_017', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_017');
    try {
      let ss = fileio.createStreamSync(fpath, 'ab');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1800
   * @tc.name fileio_test_stream_create_stream_sync_018
   * @tc.desc Function of API, mode = rb+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_018', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_018');
    expect(prepareFile(fpath, '')).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'rb+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_018 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_1900
   * @tc.name fileio_test_stream_create_stream_sync_019
   * @tc.desc Function of API, mode = rb+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_019', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_019');
    try {
      fileio.createStreamSync(fpath, 'rb+');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_019 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2000
   * @tc.name fileio_test_stream_create_stream_sync_020
   * @tc.desc Function of API, mode = wb+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_020', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_020');
    expect(prepareFile(fpath, '')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'wb+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_020 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2100
   * @tc.name fileio_test_stream_create_stream_sync_021
   * @tc.desc Function of API, mode = wb+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_021', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_021');
    try {
      let ss = fileio.createStreamSync(fpath, 'wb+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_021 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2200
   * @tc.name fileio_test_stream_create_stream_sync_022
   * @tc.desc Function of API, mode = ab+. The test file is exist.
   */
  it('fileio_test_stream_create_stream_sync_022', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_022');
    expect(prepareFile(fpath, 'a')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'ab+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_022 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2300
   * @tc.name fileio_test_stream_create_stream_sync_023
   * @tc.desc Function of API, mode = ab+. The test file is not exist.
   */
  it('fileio_test_stream_create_stream_sync_023', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_023');
    try {
      let ss = fileio.createStreamSync(fpath, 'ab+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_023 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2400
   * @tc.name fileio_test_stream_create_stream_sync_024
   * @tc.desc Function of API, invalid fpath.
   */
  it('fileio_test_stream_create_stream_sync_024', 0, function () {
    try {
      fileio.createStreamSync('', 'r');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_024 is passed!' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2500
   * @tc.name fileio_test_stream_create_stream_sync_025
   * @tc.desc Function of API, invalid mode.
   */
  it('fileio_test_stream_create_stream_sync_025', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_025');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.createStreamSync(fpath, '');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_025 is passed!' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2600
   * @tc.name fileio_test_stream_create_stream_sync_026
   * @tc.desc Function of API, fpath too long.
   */
  it('fileio_test_stream_create_stream_sync_026', 0, async function () {
    let dpath = await nextFileName('fileio_stream');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 16; i++) {
        if (i == 15) {
          let fpath = dpath + '/f' + randomString(248);
          fileio.createStreamSync(fpath, 'w+');
        } else {
          dpath = dpath + '/d' + randomString(248);
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_026 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2700
   * @tc.name fileio_test_stream_create_stream_sync_027
   * @tc.desc Function of API, filename too long.
   */
  it('fileio_test_stream_create_stream_sync_027', 0, async function () {
    let fpath = await nextFileName(randomString(256));
    try {
      fileio.createStreamSync(fpath, 'w+');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_027 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2800
   * @tc.name fileio_test_stream_create_stream_sync_028
   * @tc.desc Function of API, path too deep.
   */
  it('fileio_test_stream_create_stream_sync_028', 0, async function () {
    let dpath = await nextFileName('stream');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 113; i++) {
        if (i == 112) {
          let fpath = dpath + '/f' + i
          fileio.createStreamSync(fpath, 'w+');
        } else {
          dpath = dpath + '/' + i
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_028 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CreateStreamSync_2900
   * @tc.name fileio_test_stream_create_stream_sync_029
   * @tc.desc Function of API, special character.
   */
  it('fileio_test_stream_create_stream_sync_029', 0, async function () {
    let fpath = await nextFileName('?*:<>/|');
    try {
      fileio.createStreamSync(fpath, 'w+');
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stream_create_stream_sync_029 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0000
   * @tc.name fileio_test_stream_fdopen_stream_sync_000
   * @tc.desc Function of API, mode=r. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_000');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToReadOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o0);
      let ss = fileio.fdopenStreamSync(fd, 'r');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0100
   * @tc.name fileio_test_stream_fdopen_stream_sync_001
   * @tc.desc Function of API, mode=r. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_001', 0, function () {
    try {
      let fd = -1;
      let mode = 'r+';
      fileio.fdopenStreamSync(fd, mode);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0200
   * @tc.name fileio_test_stream_fdopen_stream_sync_002
   * @tc.desc Function of API, mode=w. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_002');
    let fd;
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0300
   * @tc.name fileio_test_stream_fdopen_stream_sync_003
   * @tc.desc Function of API, mode=w. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_003');
    try {
      let fd = fileio.openSync(fpath, 0o101, 0o222);
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0400
   * @tc.name fileio_test_stream_fdopen_stream_sync_004
   * @tc.desc Function of API, mode=a. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_004');
    expect(prepareFile(fpath, '1')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o2001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'a');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0500
   * @tc.name fileio_test_stream_fdopen_stream_sync_005
   * @tc.desc Function of API, mode=a. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_005');
    try {
      let fd = fileio.openSync(fpath, 0o101, 0o222);
      let ss = fileio.fdopenStreamSync(fd, 'a');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0600
   * @tc.name fileio_test_stream_fdopen_stream_sync_006
   * @tc.desc Function of API, mode=r+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_006');
    expect(prepareFile(fpath, '')).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let ss = fileio.fdopenStreamSync(fd, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0700
   * @tc.name fileio_test_stream_fdopen_stream_sync_007
   * @tc.desc Function of API, mode=r+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_007', 0, async function () {
    try {
      fileio.fdopenStreamSync(-1, 'r+');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0800
   * @tc.name fileio_test_stream_fdopen_stream_sync_008
   * @tc.desc Function of API, mode=w+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_008');
    expect(prepareFile(fpath, '')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_0900
   * @tc.name fileio_test_stream_fdopen_stream_sync_009
   * @tc.desc Function of API, mode=w+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_009');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let ss = fileio.fdopenStreamSync(fd, 'w+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1000
   * @tc.name fileio_test_stream_fdopen_stream_sync_010
   * @tc.desc Function of API, mode=a+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_010');
    expect(prepareFile(fpath, '1')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o2002);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'a+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1100
   * @tc.name fileio_test_stream_fdopen_stream_sync_011
   * @tc.desc Function of API, mode=a+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_011');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let ss = fileio.fdopenStreamSync(fd, 'a+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1200
   * @tc.name fileio_test_stream_fdopen_stream_sync_012
   * @tc.desc Function of API, mode=rb. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_012');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToReadOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o0);
      let ss = fileio.fdopenStreamSync(fd, 'rb');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1300
   * @tc.name fileio_test_stream_fdopen_stream_sync_013
   * @tc.desc Function of API, mode=rb. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_013', 0, async function () {
    try {
      fileio.fdopenStreamSync(-1, 'rb');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_013 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1400
   * @tc.name fileio_test_stream_fdopen_stream_sync_014
   * @tc.desc Function of API, mode=wb. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_014', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_014');
    expect(prepareFile(fpath, '')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'wb');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1500
   * @tc.name fileio_test_stream_fdopen_stream_sync_015
   * @tc.desc Function of API, mode=wb. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_015', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_015');
    try {
      let fd = fileio.openSync(fpath, 0o101, 0o222);
      let ss = fileio.fdopenStreamSync(fd, 'wb');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1600
   * @tc.name fileio_test_stream_fdopen_stream_sync_016
   * @tc.desc Function of API, mode=ab. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_016', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_016');
    expect(prepareFile(fpath, '1')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o2001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'ab');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1700
   * @tc.name fileio_test_stream_fdopen_stream_sync_017
   * @tc.desc Function of API, mode=ab. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_017', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_017');
    try {
      let fd = fileio.openSync(fpath, 0o101, 0o222);
      let ss = fileio.fdopenStreamSync(fd, 'ab');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1800
   * @tc.name fileio_test_stream_fdopen_stream_sync_018
   * @tc.desc Function of API, mode=rb+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_018', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_018');
    expect(prepareFile(fpath, '')).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let ss = fileio.fdopenStreamSync(fd, 'rb+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_018 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_1900
   * @tc.name fileio_test_stream_fdopen_stream_sync_019
   * @tc.desc Function of API, mode=rb+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_019', 0, async function () {
    try {
      fileio.fdopenStreamSync(-1, 'rb+');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_019 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2000
   * @tc.name fileio_test_stream_fdopen_stream_sync_020
   * @tc.desc Function of API, mode=wb+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_020', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_020');
    expect(prepareFile(fpath, '')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o002);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'wb+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_020 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2100
   * @tc.name fileio_test_stream_fdopen_stream_sync_021
   * @tc.desc Function of API, mode=wb+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_021', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_021');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let ss = fileio.fdopenStreamSync(fd, 'wb+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_021 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2200
   * @tc.name fileio_test_stream_fdopen_stream_sync_022
   * @tc.desc Function of API, mode=ab+. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_022', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_022');
    expect(prepareFile(fpath, '1')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o2002);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'ab+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length + 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_022 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2300
   * @tc.name fileio_test_stream_fdopen_stream_sync_023
   * @tc.desc Function of API, mode=ab+. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_023', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_023');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let ss = fileio.fdopenStreamSync(fd, 'ab+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_fdopen_stream_sync_023 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2400
   * @tc.name fileio_test_stream_fdopen_stream_sync_024
   * @tc.desc Function of API, invalid fields. The test file is not exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_024', 0, async function () {
    try {
      expect(fileio.fdopenStreamSync(null, 'r') == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('---fileio_test_stream_fdopen_stream_sync_024 is passed!' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FdopenStreamSync_2400
   * @tc.name fileio_test_stream_fdopen_stream_sync_025
   * @tc.desc Function of API, invalid mode. The test file is exist.
   */
  it('fileio_test_stream_fdopen_stream_sync_025', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_fdopen_stream_sync_025');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      fileio.fdopenStreamSync(fd, '');
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('---fileio_test_stream_fdopen_stream_sync_025 is passed!' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0000
   * @tc.name fileio_test_stream_read_sync_000
   * @tc.desc Function of API, not set options.
   */
  it('fileio_test_stream_read_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_000');
    let text = '0123456789abcdefg';
    expect(prepareFile(fpath, text)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096));
      expect(len == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0100
   * @tc.name fileio_test_stream_read_sync_001
   * @tc.desc Function of API, position.
   */
  it('fileio_test_stream_read_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_001');
    let text = '0123456789abcdefg';
    expect(prepareFile(fpath, text)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096), {
        position: 1
      });
      expect(len == text.length - 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0200
   * @tc.name fileio_test_stream_read_sync_002
   * @tc.desc Function of API, offset.
   */
  it('fileio_test_stream_read_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_002');
    let text = '0123456789abcdefg';
    expect(prepareFile(fpath, text)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096), {
        offset: 1
      });
      expect(len == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0300
   * @tc.name fileio_test_stream_read_sync_003
   * @tc.desc Function of API, length.
   */
  it('fileio_test_stream_read_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096), {
        length: 1
      });
      expect(len == 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0400
   * @tc.name fileio_test_stream_read_sync_004
   * @tc.desc Function of API, readBuf.
   */
  it('fileio_test_stream_read_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(null);
      expect(len == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_004 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0500
   * @tc.name fileio_test_stream_read_sync_005
   * @tc.desc Function of API, position = -1.
   */
  it('fileio_test_stream_read_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_005');
    let text = '0123456789abcdefg';
    expect(prepareFile(fpath, text)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096), {
        position: -1
      });
      expect(len == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('---fileio_test_stream_read_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_ReadSync_0600
   * @tc.name fileio_test_stream_read_sync_006
   * @tc.desc Function of API, offset.
   */
  it('fileio_test_stream_read_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096), {
        offset: -1
     });
      expect(len == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_read_sync_006 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0000
   * @tc.name fileio_test_stream_write_sync_000
   * @tc.desc Function of API, not set options.
   */
  it('fileio_test_stream_write_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(text) == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('---fileio_test_stream_write_sync_000 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0100
   * @tc.name fileio_test_stream_write_sync_001
   * @tc.desc Function of API, invalid encoding.
   */
  it('fileio_test_stream_write_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_001');
    expect(prepareFile(fpath, '')).assertTrue();
    let ss
    try {
      ss = fileio.createStreamSync(fpath, 'w+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT, {
        encoding: 'ASCII'
      }) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_001 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0200
   * @tc.name fileio_test_stream_write_sync_002
   * @tc.desc Function of API, position.
   */
  it('fileio_test_stream_write_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_002');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync('aaa') !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == 3).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0300
   * @tc.name fileio_test_stream_write_sync_003
   * @tc.desc Function of API, offset.
   */
  it('fileio_test_stream_write_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_003');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync('aaa') !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == 3).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0400
   * @tc.name fileio_test_stream_write_sync_004
   * @tc.desc Function of API, length.
   */
  it('fileio_test_stream_write_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_004');
    expect(prepareFile(fpath, 'a')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text);
      expect(wlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r+');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0500
   * @tc.name fileio_test_stream_write_sync_005
   * @tc.desc Function of API, encoding.
   */
  it('fileio_test_stream_write_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_005');
    expect(prepareFile(fpath, '')).assertTrue();
    let writeStrArray = new Array('aaa', '123');
    let encodingArray = new Array('utf-8', 'utf-16');
    try {
      for (let i = 0; i < encodingArray.length; i++) {
        let ss = fileio.createStreamSync(fpath, 'w+');
        expect(ss !== null).assertTrue();
        expect(ss.writeSync(writeStrArray[i], {
          encoding: encodingArray[i]
        }) !== null).assertTrue();
        expect(ss.closeSync() !== null).assertTrue();
        ss = fileio.createStreamSync(fpath, 'r+');
        let rlen = ss.readSync(new ArrayBuffer(4096));
        expect(rlen !== null).assertTrue();
        expect(ss.closeSync() !== null).assertTrue();
      }
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stream_write_sync_005 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0600
   * @tc.name fileio_test_stream_write_sync_006
   * @tc.desc Function of API, position.
   */
  it('fileio_test_stream_write_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_006');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync('aaa', {
        position: 1
      }) !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == 4).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0700
   * @tc.name fileio_test_stream_write_sync_007
   * @tc.desc Function of API, offset.
   */
  it('fileio_test_stream_write_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_007');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync('aaa', {
        offset: 1
      }) !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == 2).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0800
   * @tc.name fileio_test_stream_write_sync_008
   * @tc.desc Function of API, length.
   */
  it('fileio_test_stream_write_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_008');
    expect(prepareFile(fpath, '')).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let wlen = ss.writeSync(text, {
        length: text.length
      });
      expect(wlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r+');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(rlen == text.length).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_0900
   * @tc.name fileio_test_stream_write_sync_009
   * @tc.desc Function of API, invalid fcontent.
   */
  it('fileio_test_stream_write_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_009');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      ss.writeSync(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_stream_write_sync_009 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_1000
   * @tc.name fileio_test_stream_write_sync_010
   * @tc.desc Function of API, invalid encoding.
   */
  it('fileio_test_stream_write_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT, {
        encoding: ''
      }) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_010 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_1100
   * @tc.name fileio_test_stream_write_sync_011
   * @tc.desc Function of API, invalid position.
   */
  it('fileio_test_stream_write_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_011');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT, {
        position: -1
      }) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_011 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_1200
   * @tc.name fileio_test_stream_write_sync_012
   * @tc.desc Function of API, invalid offset.
   */
  it('fileio_test_stream_write_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_012');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      var ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT, {
        offset: -1
      }) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_012 has failed for ' + e);
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_1300
   * @tc.name fileio_test_stream_write_sync_013
   * @tc.desc Function of API, Set all parameters.
   */
  it('fileio_test_stream_write_sync_013', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_013');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(text, {
        encoding: 'utf-8',
        position: 0,
        offset: 1,
        length: 1,
      }) == 1).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('---fileio_test_stream_write_sync_013 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_013 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_WriteSync_1400
   * @tc.name fileio_test_stream_write_sync_014
   * @tc.desc Function of API, Set all parameters.
   */
  it('fileio_test_stream_write_sync_014', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_014');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(text, {
        offset: 2,
        length: 2,
      }) == 2).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('---fileio_test_stream_write_sync_014 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stream_write_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_write_read_4k_sync
   * @tc.name fileio_test_stream_write_read_4k_sync
   * @tc.desc Function of API, read 4k file.
   */
  it('fileio_test_stream_write_read_4k_sync', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_read_4k_sync');
    expect(prepareFile(fpath, 'a')).assertTrue();
    expect(fileToWriteOnly(fpath)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o001);
      expect(fd !== null).assertTrue();
      let ss = fileio.fdopenStreamSync(fd, 'w');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(randomString(4096)) !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileToReadOnly(fpath)).assertTrue();
      ss = fileio.createStreamSync(fpath, 'r');
      let rlen = ss.readSync(new ArrayBuffer(4096));
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_write_read_4k_sync has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FlushSync_0000
   * @tc.name fileio_test_stream_flush_sync_000
   * @tc.desc Function of API, flushSync.
   */
  it('fileio_test_stream_flush_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_flush_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let text = '0123456789abcdefg';
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(text) == text.length).assertTrue();
      expect(ss.flushSync() !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_flush_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_FlushSync_0100
   * @tc.name fileio_test_stream_flush_sync_001
   * @tc.desc Function of API, error.
   */
  it('fileio_test_stream_flush_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_flush_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      ss.flushSync();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stream_flush_sync_001 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CloseSync_0000
   * @tc.name fileio_test_stream_close_sync_000
   * @tc.desc Function of API, closeSync.
   */
  it('fileio_test_stream_close_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_close_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss.closeSync() !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stream_close_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stream_CloseSync_0100
   * @tc.name fileio_test_stream_close_sync_001
   * @tc.desc Function of API, error.
   */
  it('fileio_test_stream_close_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_close_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let ss;
    try {
      ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss.closeSync() !== null).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_stream_close_sync_001 has failed for ' + e);
    }
  });
});
