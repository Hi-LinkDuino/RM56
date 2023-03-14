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
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect
}
from 'deccjsunit/index'
import {
  FILE_CONTENT,
  prepareFile,
  nextFileName,
  prepareEmptyFile,
  randomString,
  fileName
}
  from './Common'
describe('fileIOTest', function () {
  /**
   * @tc.number SUB_STORAGE_FileIO_open&closesync_0000
   * @tc.name fileio_test_open_close_sync_000
   * @tc.desc Function of API, openSync. The test file is exist.
   */
  it('fileio_test_open_close_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_close_sync_000');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_close_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0000
   * @tc.name fileio_test_open_000
   * @tc.desc Function of API, openSync(mode not for value). The test file is exist.
   */
  it('fileio_test_open_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o202);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0100
   * @tc.name fileio_test_open_001
   * @tc.desc Function of API, flags=0o100. mode=0o001
   */
  it('fileio_test_open_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o100, 0o01);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0200
   * @tc.name fileio_test_open_002
   * @tc.desc Function of API,
   */
  it('fileio_test_open_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0300
   * @tc.name fileio_test_open_003
   * @tc.desc Function of API, flags=0o100. mode=0o004
   */
  it('fileio_test_open_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o100, 0o004);
      expect(fd !== null).assertTrue();
      fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_003 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0400
   * @tc.name fileio_test_open_004
   * @tc.desc Function of API, flags=0o101. mode=0o002
   */
  it('fileio_test_open_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o101, 0o002);
      expect(fd !== null).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_004 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0500
   * @tc.name fileio_test_open_005
   * @tc.desc Function of API, flags=0o102. mode=0o001.
   */
  it('fileio_test_open_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o001);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0600
   * @tc.name fileio_test_open_006
   * @tc.desc Function of API, flags=0o200. mode=0o700
   */
  it('fileio_test_open_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o200, 0o700);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0700
   * @tc.name fileio_test_open_007
   * @tc.desc Function of API, flags=0o302. mode=0o700.
   */
  it('fileio_test_open_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_007');
    try {
      let fd = fileio.openSync(fpath, 0o302, 0o700);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0800
   * @tc.name fileio_test_open_008
   * @tc.desc Function of API, flags=0o102. mode=0o700
   */
  it('fileio_test_open_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_008');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o700);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0900
   * @tc.name fileio_test_open_009
   * @tc.desc Function of API, flags=0o302.
   */
  it('fileio_test_open_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_009');
    try {
      fileio.openSync(fpath, 0o302);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_009 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1000
   * @tc.name fileio_test_open_010
   * @tc.desc Function of API, flags=0o402.
   */
  it('fileio_test_open_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o402);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1100
   * @tc.name fileio_test_open_011
   * @tc.desc Function of API, flags=0o1000.
   */
  it('fileio_test_open_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_011');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o1000);
      expect(fd !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1200
   * @tc.name fileio_test_open_012
   * @tc.desc Function of API, flags=0o1001.
   */
  it('fileio_test_open_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_012');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o1001);
      expect(fd !== null).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_012 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1300
   * @tc.name fileio_test_open_013
   * @tc.desc Function of API, flags=0o2002.
   */
  it('fileio_test_open_013', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_013');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2002);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_013 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1400
   * @tc.name fileio_test_open_014
   * @tc.desc Function of API, flags=0o4002.
   */
  it('fileio_test_open_014', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_014');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o4002);
      expect(fd !== null).assertTrue();
      let fd2 = fileio.openSync(fpath, 0o4002);
      expect(fd2 !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.closeSync(fd2) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      done();
    } 
    catch (e) {
      console.log('fileio_test_open_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1500
   * @tc.name fileio_test_open_015
   * @tc.desc Function of API, flags=0o20002.
   */
  it('fileio_test_open_015', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_015');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o20002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1600
   * @tc.name fileio_test_open_016
   * @tc.desc Function of API, flags=0o10002.
   */
  it('fileio_test_open_016', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_016');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o10002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1700
   * @tc.name fileio_test_open_017
   * @tc.desc Function of API, flags=0o4010002.
   */
  it('fileio_test_open_017', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_017');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o4010002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1800
   * @tc.name fileio_test_open_018
   * @tc.desc Function of API, flags=0o100002.
   */
  it('fileio_test_open_018', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_018');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o100002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_018 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1900
   * @tc.name fileio_test_open_019
   * @tc.desc Function of API, flags=0o40002. mode=0o700
   */
  it('fileio_test_open_019', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_019');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o40002, 0o700);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_019 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2100
   * @tc.name fileio_test_open_021
   * @tc.desc Function of API, flags=0o2000002.
   */
  it('fileio_test_open_021', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_021');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2000002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_021 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2200
   * @tc.name fileio_test_open_022
   * @tc.desc Function of API, flags=0o200002.
   */
  it('fileio_test_open_022', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_022');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o200002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_022 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2300
   * @tc.name fileio_test_open_023
   * @tc.desc Function of API, flags=0o400002.
   */
  it('fileio_test_open_023', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_023');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o400002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_023 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2400
   * @tc.name fileio_test_open_024
   * @tc.desc Function of API, flags=0o1000002.mode=0o700
   */
  it('fileio_test_open_024', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_024');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o1000002, 0o700);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_open_024 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2500
   * @tc.name fileio_test_open_025
   * @tc.desc Function of API, flags=0o10000002.
   */
  it('fileio_test_open_025', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_025');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o10000002);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_open_025 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2600
   * @tc.name fileio_test_open_026
   * @tc.desc Function of API,flags=0o002.mode=0o700
   */
  it('fileio_test_open_026', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_026');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o700);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_026 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2700
   * @tc.name fileio_test_open_027
   * @tc.desc Function of API, flags=0o002.mode=0o400
   */
  it('fileio_test_open_027', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_027');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o400);
      expect(fd !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2800
   * @tc.name fileio_test_open_028
   * @tc.desc Function of API, flags=0o002.mode=0o200
   */
  it('fileio_test_open_028', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_028');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o200);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_2900
   * @tc.name fileio_test_open_029
   * @tc.desc Function of API, flags=0o002.mode=0o100
   */
  it('fileio_test_open_029', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_029');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o100);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_029 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3000
   * @tc.name fileio_test_open_030
   * @tc.desc Function of API,  flags=0o002.mode=0o070
   */
  it('fileio_test_open_030', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_030');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o070);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_030 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3100
   * @tc.name fileio_test_open_031
   * @tc.desc Function of API, flags=0o002.mode=0o040
   */
  it('fileio_test_open_031', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_031');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o040);
      expect(fd !== null).assertTrue();
      let red = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(red !== null).assertTrue();
      fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3200
   * @tc.name fileio_test_open_032
   * @tc.desc Function of API, flags=0o002.mode=0o020
   */
  it('fileio_test_open_032', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_032');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o020);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3300
   * @tc.name fileio_test_open_033
   * @tc.desc Function of API, flags=0o002.mode=0o010
   */
  it('fileio_test_open_033', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_033');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o010);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_033 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3400
   * @tc.name fileio_test_open_034
   * @tc.desc Function of API, flags=0o002.mode=0o007
   */
  it('fileio_test_open_034', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_034');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o007);
      expect(fd !== null).assertTrue();
      let ret = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(ret !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_034 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3500
   * @tc.name fileio_test_open_035
   * @tc.desc Function of API, flags=0o002.mode=0o004
   */
  it('fileio_test_open_035', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_035');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o004);
      expect(fd !== null).assertTrue();
      let ret = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(ret !== null).assertTrue();
      fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3600
   * @tc.name fileio_test_open_036
   * @tc.desc Function of API, flags=0o002.mode=0o002
   */
  it('fileio_test_open_036', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_036');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o002);
      expect(fd !== null).assertTrue();
      let wri = fileio.writeSync(fd, 'hello', {
        encoding: 'utf-8',
        offset: 1,
        length: 1,
      });
      expect(wri !== null).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3700
   * @tc.name fileio_test_open_037
   * @tc.desc Function of API, flags=0o002.mode=0o001
   */
  it('fileio_test_open_037', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_037');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o002, 0o001);
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_037 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_3800
   * @tc.name fileio_test_open_038
   * @tc.desc Function of API, flags=0o102.mode=0o700
   */
  it('fileio_test_open_038', 0, async function () {
    let txt = randomString(4100);
    let fpath = await nextFileName(txt);
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o700);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_038 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0100
   * @tc.name fileio_test_open_sync_001
   * @tc.desc Function of API, openSync(flags=0o200). The test file is exist.
   */
  it('fileio_test_open_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o200);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0200
   * @tc.name fileio_test_open_sync_002
   * @tc.desc Function of API, openSync(flags=0o201). The test file is exist.
   */
  it('fileio_test_open_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o201);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0300
   * @tc.name fileio_test_open_sync_003
   * @tc.desc Function of API, openSync(flags=0o202). The test file is exist.
   */
  it('fileio_test_open_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o202);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0400
   * @tc.name fileio_test_open_sync_004
   * @tc.desc Function of API, openSync(flags=0o102, mode=0o777). The test file is not exist.
   */
  it('fileio_test_open_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_open_sync_004');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o777);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0500
   * @tc.name fileio_test_open_sync_005
   * @tc.desc Function of API, openSync(flags not for value, mode not for value).
   */
  it('fileio_test_open_sync_005', 0, async function () {
    let dpath = await nextFileName('fileio_test_open_sync_005') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let fd = fileio.openSync(dpath);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0600
   * @tc.name fileio_test_open_sync_006
   * @tc.desc Function of API, openSync(flags=0o1, illegal parameter).
   */
  it('fileio_test_open_sync_006', 0, async function () {
    let dpath = await nextFileName('fileio_test_open_sync_006') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      fileio.openSync(dpath, 0o1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_006 has failed for ' + e);
      fileio.rmdirSync(dpath);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0700
   * @tc.name fileio_test_open_sync_007
   * @tc.desc Function of API, openSync(fpath too long).
   */
  it('fileio_test_open_sync_007', 0, async function () {
    let dpath = await nextFileName('fileio_open1');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 16; i++) {
        if (i == 15) {
          let fpath = dpath + '/f' + randomString(248);
          fileio.openSync(fpath, 0o102, 0o777);
        } else {
          dpath = dpath + '/d' + randomString(248);
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0800
   * @tc.name fileio_test_open_sync_008
   * @tc.desc Function of API, openSync(filename too long).
   */
  it('fileio_test_open_sync_008', 0, async function () {
    let fpath = await nextFileName(randomString(256));
    try {
      fileio.openSync(fpath, 0o102, 0o777);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0900
   * @tc.name fileio_test_open_sync_009
   * @tc.desc Function of API, openSync(dir layers too long).
   */
  it('fileio_test_open_sync_009', 0, async function () {
    let dpath = await nextFileName('prop1');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 113; i++) {
        if (i == 112) {
          let fpath = dpath + '/f' + i
          fileio.openSync(fpath, 0o102, 0o777);
        } else {
          dpath = dpath + '/' + i
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_009 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_1000
   * @tc.name fileio_test_open_sync_010
   * @tc.desc Function of API, openSync(filename has special characters).
   */
  it('fileio_test_open_sync_010', 0, async function () {
    let fpath = await nextFileName('?*:<>/|');
    try {
      fileio.openSync(fpath, 0o102, 0o777);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_open_sync_010 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_CloseSync_0000
   * @tc.name fileio_test_close_sync_000
   * @tc.desc Function of API, close. The test file is not exist.
   */
  it('fileio_test_close_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_000');
    try {
      fileio.closeSync(fpath);
      expect(null).assertFail();

    } 
    catch (e) {
      console.log('fileio_test_close_sync_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_CloseSync_0100
   * @tc.name fileio_test_close_sync_001
   * @tc.desc Function of API, close(fpath=vaild value). The test file is not exist.
   */
  it('fileio_test_close_sync_001', 0, async function () {
    try {
      fileio.closeSync(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_close_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0000
   * @tc.name fileio_test_access_sync_000
   * @tc.desc Function of API,access. The test file is exist.
   */
  it('fileio_test_access_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.accessSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0100
   * @tc.name fileio_test_access_sync_001
   * @tc.desc Function of API, launch via fileio. The test file is not exist.
   */
  it('fileio_test_access_sync_001', 0, async function () {
    try {
      fileio.accessSync(1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0200
   * @tc.name fileio_test_access_sync_002
   * @tc.desc Function of API, access. The test file is not exist.
   */
  it('fileio_test_access_sync_002', 0, async function () {
    let fpath = await nextFileName('fileIOTest');
    try {
      fileio.accessSync(fpath);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0300
   * @tc.name fileio_test_access_sync_003
   * @tc.desc Function of API, access(mode=1). The test file is exist and have the authority.
   */
  it('fileio_test_access_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_004');
    expect(prepareEmptyFile(fpath)).assertTrue();
    try {
      let fd = fileio.accessSync(fpath);
      console.log(fd);
      expect(fileio.accessSync(fpath, 1) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0400
   * @tc.name fileio_test_access_sync_004
   * @tc.desc Function of API, access(mode=2). The test file is exist and have the authority.
   */
  it('fileio_test_access_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.accessSync(fpath);
      expect(fd !== null).assertTrue();
      expect(fileio.accessSync(fpath, 2) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0500
   * @tc.name fileio_test_access_sync_005
   * @tc.desc Function of API, access(fpath=dir address). The test dir is exist.
   */
  it('fileio_test_access_sync_005', 0, async function () {
    let dpath = await nextFileName('fileio_test_access_sync_005') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let fd = fileio.accessSync(dpath);
      expect(fd == undefined).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0600
   * @tc.name fileio_test_access_sync_006
   * @tc.desc Function of API, access(fpath=dir address). The test dir is not exist.
   */
  it('fileio_test_access_sync_006', 0, async function () {
    let dpath = await nextFileName('fileio_test_access_sync_006') + 'd'
    try {
      fileio.accessSync(dpath);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_AccessSync_0700
   * @tc.name fileio_test_access_sync_007
   * @tc.desc Function of API, access(mode=4).
   */
  it('fileio_test_access_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.accessSync(fpath);
      expect(fd !== null).assertTrue();
      expect(fileio.accessSync(fpath, 4) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_access_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_UnlinkSync_0000
   * @tc.name fileio_test_unlink_sync_000
   * @tc.desc Function of API,unlinkSync. The test file is exist.
   */
  it('fileio_test_unlink_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_unlink_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.unlinkSync(fpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_unlink_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_UnlinkSync_0100
   * @tc.name fileio_test_unlink_sync_001
   * @tc.desc Function of API, no parameter.
   */
  it('fileio_test_unlink_sync_001', 0, async function () {
    try {
      fileio.unlinkSync(1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_unlink_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_UnlinkSync_0200
   * @tc.name fileio_test_unlink_sync_002
   * @tc.desc Function of API, invalid parameter
   */
  it('fileio_test_unlink_sync_002', 0, async function () {
    let fpath = await nextFileName('fileIOTest');
    try {
      fileio.unlinkSync(fpath);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_unlink_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0000
   * @tc.name fileio_test_write_sync_000
   * @tc.desc Function of API, writeSync.
   */
  it('fileio_test_write_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_000');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let res = fileio.writeSync(fd, FILE_CONTENT);
      expect(typeof(res) == 'number').assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0100
   * @tc.name fileio_test_write_sync_001
   * @tc.desc Function of API, encoding=UTF-8.
   */
  it('fileio_test_write_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_001');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let res = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
      });
      expect(typeof(res) == 'number').assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0200
   * @tc.name fileio_test_write_sync_002
   * @tc.desc Function of API, offset=1.
   */
  it('fileio_test_write_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_002');
    let text = '0123456789abcdefg';
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(fileio.writeSync(fd, text, {
        offset: 1,
      }) == text.length - 1).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0300
   * @tc.name fileio_test_write_sync_003
   * @tc.desc Function of API, length = - 1.
   */
  it('fileio_test_write_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_003');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(fileio.writeSync(fd, FILE_CONTENT, {
        length: FILE_CONTENT.length - 1,
      }) == (FILE_CONTENT.length - 1)).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0400
   * @tc.name fileio_test_write_sync_004
   * @tc.desc Function of API, length = 1, offset = 1.
   */
  it('fileio_test_write_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_004');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let res = fileio.writeSync(fd, FILE_CONTENT, {
        offset: 1,
        length: 1,
      });
      expect(typeof(res) == 'number').assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0500
   * @tc.name fileio_test_write_sync_005
   * @tc.desc Function of API, invalid offset.
   */
  it('fileio_test_write_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_005');
    const invalidOffset = 999
    let fd
    try {
      fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(fd).assertInstanceOf('Number');
      expect(fileio.writeSync(fd, FILE_CONTENT, {
        offset: invalidOffset,
      }) == 1).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0600
   * @tc.name fileio_test_write_sync_006
   * @tc.desc Function of API, invalid length.
   */
  it('fileio_test_write_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_006');
    const invalidLength = 999
    let fd
    try {
      fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(fd).assertInstanceOf('Number');
      expect(fileio.writeSync(fd, FILE_CONTENT, {
        length: invalidLength,
      }) == 1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0700
   * @tc.name fileio_test_write_sync_007
   * @tc.desc Function of API, no parameter.
   */
  it('fileio_test_write_sync_007', 0, async function () {
    try {
      fileio.writeSync(1, 1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0800
   * @tc.name fileio_test_write_sync_008
   * @tc.desc Function of API, invalid parameter.
   */
  it('fileio_test_write_sync_008', 0, async function () {
    try {
      console.log(fileio.writeSync(-1, FILE_CONTENT));
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_0900
   * @tc.name fileio_test_write_sync_009
   * @tc.desc Function of API, Set all parameters.
   */
  it('fileio_test_write_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_009');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      let res = fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'utf-8',
        position: 0,
        offset: 1,
        length: 1,
      });
      expect(typeof(res) == 'number').assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_WriteSync_1000
   * @tc.name fileio_test_write_sync_010
   * @tc.desc Function of API, encoding=gb2312.
   */
  it('fileio_test_write_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_010');
    let fd = fileio.openSync(fpath, 0o102, 0o666);
    try {
      fileio.writeSync(fd, FILE_CONTENT, {
        encoding: 'gb2312',
      });
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_write_sync_010 has failed for ' + e);
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0100
   * @tc.name fileio_test_read_sync_001
   * @tc.desc Function of API, offset = 1.
   */
  it('fileio_test_read_sync_001', 0, async function () {
    let bufLen = 5
    expect(FILE_CONTENT.length > bufLen).assertTrue();
    let fpath = await nextFileName('fileio_test_read_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let len = fileio.readSync(fd, new ArrayBuffer(bufLen), {
      offset: 1,
      });
      expect(len == bufLen - 1).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0200
   * @tc.name fileio_test_read_sync_002
   * @tc.desc Function of API, length = 1.
   */
  it('fileio_test_read_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        length: 1,
      });
      expect(len == 1).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0300
   * @tc.name fileio_test_read_sync_003
   * @tc.desc Function of API, invalid offset
   */
  it('fileio_test_read_sync_003', 0, async function () {
    let fd
    const invalidOffset = 99999
    let fpath = await nextFileName('fileio_test_read_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fd = fileio.openSync(fpath, 0o2);
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: invalidOffset,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_read_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0300
   * @tc.name fileio_test_read_sync_004
   * @tc.desc Function of API, invalid length.
   */
  it('fileio_test_read_sync_004', 0, async function () {
    let fd
    const invalidLength = 9999
    let fpath = await nextFileName('fileio_test_read_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fd = fileio.openSync(fpath, 0o2);
      fileio.readSync(fd, new ArrayBuffer(4096), {
        length: invalidLength,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_read_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0500
   * @tc.name fileio_test_read_sync_005
   * @tc.desc Function of API, fpatch = -1.
   */
  it('fileio_test_read_sync_005', 0, async function () {
    try {
      fileio.readSync(-1, new ArrayBuffer(4096));
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0600
   * @tc.name fileio_test_read_sync_006
   * @tc.desc Function of API, offset & length & position = 1.
   */
  it('fileio_test_read_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: 1,
      });
      expect(len == FILE_CONTENT.length - 1);
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0700
   * @tc.name fileio_test_read_sync_007
   * @tc.desc Function of API, invalid position.
   */
  it('fileio_test_read_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let invalidPos = FILE_CONTENT.length;
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: invalidPos,
      });
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0800
   * @tc.name fileio_test_read_sync_008
   * @tc.desc Function of API, Enter all parameters correctly.
   */
  it('fileio_test_read_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_008');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: 1,
        offset: 1,
        length: 1,
      });
      expect(len == 1).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_0900
   * @tc.name fileio_test_read_sync_009
   * @tc.desc Function of API, Set offset and length.
   */
  it('fileio_test_read_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_009');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: 1,
        length: 1,
      });
      expect(len == 1).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ReadSync_1000
   * @tc.name fileio_test_read_sync_010
   * @tc.desc Function of API, Set error parameters (options).
   */
  it('fileio_test_read_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let fd;
    try {
      fd = fileio.openSync(fpath, 0o2);
      fileio.readSync(fd, new ArrayBuffer(4096), {
        position: 1,
        offset: 1,
        length: 1,
      });
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_read_sync_010 has failed for ' + e);
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0000
   * @tc.name fileio_test_chmod_sync_000
   * @tc.desc Function of API, mode=0o660
   */
  it('fileio_test_chmod_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o660) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o660).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0100
   * @tc.name fileio_test_chmod_sync_001
   * @tc.desc Function of API, mode=0o460
   */
  it('fileio_test_chmod_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o460) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o460).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0200
   * @tc.name fileio_test_chmod_sync_002
   * @tc.desc Function of API, mode=0o260. The test file is exist.
   */
  it('fileio_test_chmod_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o260) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o260).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0300
   * @tc.name fileio_test_chmod_sync_003
   * @tc.desc Function of API, chmodSync. The test file is not exist.
   */
  it('fileio_test_chmod_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_003');
    try {
      fileio.chmodSync(fpath, 0o260);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0400
   * @tc.name fileio_test_chmod_sync_004
   * @tc.desc Function of API, fpatch=dir address. The test dir is exist.
   */
  it('fileio_test_chmod_sync_004', 0, async function () {
    let dpath = await nextFileName('fileio_test_chmod_sync_004');
    try {
      expect(fileio.mkdirSync(dpath, 0o777) !== null).assertTrue();
      expect(fileio.chmodSync(dpath, 0o660) !== null).assertTrue();
      expect((fileio.statSync(dpath).mode & 0o777) == 0o660).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0500
   * @tc.name fileio_test_chmod_sync_005
   * @tc.desc Function of API, fpatch=dir address. The test dir is not exist.
   */
  it('fileio_test_chmod_sync_005', 0, async function () {
    let dpath;
    try {
      fileio.chmodSync(dpath, 0o660);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0600
   * @tc.name fileio_test_chmod_sync_006
   * @tc.desc Function of API, mode=0o700. The test file is exist.
   */
  it('fileio_test_chmod_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o700) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o700).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0700
   * @tc.name fileio_test_chmod_sync_007
   * @tc.desc Function of API, mode=0o400. The test file is exist.
   */
  it('fileio_test_chmod_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o400) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o400).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0800
   * @tc.name fileio_test_chmod_sync_008
   * @tc.desc Function of API, mode=0o200. The test file is exist.
   */
  it('fileio_test_chmod_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_008');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o200) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o200).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0900
   * @tc.name fileio_test_chmod_sync_009
   * @tc.desc Function of API, mode=0o100. The test file is exist.
   */
  it('fileio_test_chmod_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_009');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o100) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o100).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0100
   * @tc.name fileio_test_chmod_sync_010
   * @tc.desc Function of API, mode=0o070. The test file is exist.
   */
  it('fileio_test_chmod_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o070) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o070).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0110
   * @tc.name fileio_test_chmod_sync_011
   * @tc.desc Function of API, mode=0o040. The test file is exist.
   */
  it('fileio_test_chmod_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_011');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o040) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o040).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0120
   * @tc.name fileio_test_chmod_sync_012
   * @tc.desc Function of API, mode=0o020. The test file is exist.
   */
  it('fileio_test_chmod_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_012');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o020) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o020).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0130
   * @tc.name fileio_test_chmod_sync_013
   * @tc.desc Function of API, mode=0o010. The test file is exist.
   */
  it('fileio_test_chmod_sync_013', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_013');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o010) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o010).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_013 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0140
   * @tc.name fileio_test_chmod_sync_014
   * @tc.desc Function of API, mode=0o007. The test file is exist.
   */
  it('fileio_test_chmod_sync_014', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_014');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o007) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o007).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0150
   * @tc.name fileio_test_chmod_sync_015
   * @tc.desc Function of API, mode=0o700. The test file is exist.
   */
  it('fileio_test_chmod_sync_015', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_015');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o004) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o004).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0160
   * @tc.name fileio_test_chmod_sync_016
   * @tc.desc Function of API, mode=0o002. The test file is exist.
   */
  it('fileio_test_chmod_sync_016', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_016');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o002) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o002).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChmodSync_0170
   * @tc.name fileio_test_chmod_sync_017
   * @tc.desc Function of API, mode=0o001. The test file is exist.
   */
  it('fileio_test_chmod_sync_017', 0, async function () {
    let fpath = await nextFileName('fileio_test_chmod_sync_017');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chmodSync(fpath, 0o001) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o001).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chmod_sync_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0000
   * @tc.name fileio_test_copy_file_sync_000
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is vaild, same path, file not same.
   */
  it('fileio_test_copy_file_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_000');
    let fpathTarget = fpath + 'tgt'
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0100
   * @tc.name fileio_test_copy_file_sync_001
   * @tc.desc Function of API, copy. fpatch is invalid, fpathTarget is vaild, same path, file not same.
   */
  it('fileio_test_copy_file_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_001') + 'd'
    let fpathTarget = fpath + 'tgt'
    try {
      fileio.copyFileSync(fpath, fpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0200
   * @tc.name fileio_test_copy_file_sync_002
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is invalid.
   */
  it('fileio_test_copy_file_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_002');
    try {
      fileio.copyFileSync(fpath, 0);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0300
   * @tc.name fileio_test_copy_file_sync_003
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is vaild, path not same, file not same.
   */
  it('fileio_test_copy_file_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_003');
    let fpathTarget = fpath + 'f1'
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0400
   * @tc.name fileio_test_copy_file_sync_004
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is vaild, path not same, file not same.
   */
  it('fileio_test_copy_file_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_004');
    let fpathTarget = await fileName('fileio_test_copy_file_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0500
   * @tc.name fileio_test_copy_file_sync_005
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is vaild, path not same, file not same.
   */
  it('fileio_test_copy_file_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_005');
    let fpathTarget = fpath
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0600
   * @tc.name fileio_test_copy_file_sync_006
   * @tc.desc Function of API, copy. fpathTarget is invalid.
   */
  it('fileio_test_copy_file_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_006');
    let fpathTarget = fpath + 'tgt'
    try {
      fileio.copyFileSync(fpath, fpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0700
   * @tc.name fileio_test_copy_file_sync_007
   * @tc.desc Function of API, copy. fpatch is invalid, fpathTarget is invalid.
   */
  it('fileio_test_copy_file_sync_007', 0, async function () {
    try {
      fileio.copyFileSync(1, 1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0800
   * @tc.name fileio_test_copy_file_sync_008
   * @tc.desc Function of API, parameter more than 4096.
   */
  it('fileio_test_copy_file_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_008');
    fileio.openSync(fpath, 0o102, 0o777);
    let dpath = await nextFileName('fileio_copy1');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 16; i++) {
        if (i == 15) {
          let fpathTarget = dpath + '/f' + randomString(248);
          fileio.copyFileSync(fpath, fpathTarget);
        } else {
          dpath = dpath + '/d' + randomString(248);
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_0900
   * @tc.name fileio_test_copy_file_sync_009
   * @tc.desc Function of API, filename is too long.
   */
  it('fileio_test_copy_file_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_009');
    fileio.openSync(fpath, 0o102, 0o777);
    let fpathTarget = await nextFileName(randomString(256));
    try {
      fileio.copyFileSync(fpath, fpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      fileio.unlinkSync(fpath);
      console.log('fileio_test_copy_file_sync_009 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_1000
   * @tc.name fileio_test_copy_file_sync_010
   * @tc.desc Function of API, dir layers too long
   */
  it('fileio_test_copy_file_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_010');
    fileio.openSync(fpath, 0o102, 0o777);
    let dpath = await nextFileName('prop');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 113; i++) {
        if (i == 112) {
          let fpathTarget = dpath + '/f' + i
          fileio.copyFileSync(fpath, fpathTarget);
        } else {
          dpath = dpath + '/' + i
          fileio.mkdirSync(dpath);
        }
      }
      expect(null).assertFail();
    } 
    catch (e) {
      fileio.unlinkSync(fpath);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_1100
   * @tc.name fileio_test_copy_file_sync_011
   * @tc.desc Function of API, special parameter.
   */
  it('fileio_test_copy_file_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_011');
    fileio.openSync(fpath, 0o102, 0o777);
    let fpathTarget = await nextFileName('?*:<>/|');
    try {
      fileio.copyFileSync(fpath, fpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      fileio.unlinkSync(fpath);
      console.log('fileio_test_copy_file_sync_011 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_copyFileSync_1200
   * @tc.name fileio_test_copy_file_sync_012
   * @tc.desc Function of API, copy. fpatch is vaild, fpathTarget is vaild, same path, file not same, mode is 0.
   */
  it('fileio_test_copy_file_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_copy_file_sync_012');
    let fpathTarget = fpath + 'tgt'
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget, 0) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_copy_file_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_truncateSync_0000
   * @tc.name fileio_test_truncate_sync_000
   * @tc.desc Function of API, truncateSync.
   */
  it('fileio_test_truncate_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_truncate_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.truncateSync(fpath, 10) !== null).assertTrue();
      expect(fileio.statSync(fpath).size == 10).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_truncate_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_truncateSync_0100
   * @tc.name fileio_test_truncate_sync_001
   * @tc.desc Function of API, no value for parameter.
   */
  it('fileio_test_truncate_sync_001', 0, async function () {
    try {
      fileio.truncateSync();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_truncate_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_truncateSync_0200
   * @tc.name fileio_test_truncate_sync_002
   * @tc.desc Function of API, no invalid parameter.
   */
  it('fileio_test_truncate_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_truncate_sync_001');
    try {
      fileio.truncateSync(fpath);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_truncate_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_truncateSync_0300
   * @tc.name fileio_test_truncate_sync_003
   * @tc.desc Function of API, .
   */
  it('fileio_test_truncate_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_truncate_sync_003');
    try {
      fileio.truncateSync(fpath, -1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_truncate_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0000
   * @tc.name fileio_test_rename_sync_000
   * @tc.desc Function of API, renameSync. The test file is exist,
   * fpathTarget is not same with fpatch, file name are same.
   */
  it('fileio_test_rename_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_rename_sync_000');
    let fpathTarget = fpath + 'tgt'
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.renameSync(fpath, fpathTarget) !== null).assertTrue();
      expect(fileio.accessSync(fpathTarget) !== null).assertTrue();
      expect(fileio.unlinkSync(fpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0100
   * @tc.name fileio_test_rename_sync_001
   * @tc.desc Function of API, renameSync. The test file is not exist, 
   * fpathTarget is not same with fpatch, file name are same.
   */
  it('fileio_test_rename_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_rename_sync_001');
    let fpathTarget = fpath + 'tgt'
    try {
      fileio.renameSync(fpath, fpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0200
   * @tc.name fileio_test_rename_sync_002
   * @tc.desc Function of API, renameSync. The test file is exist, fpathTarget is invalid.
   */
  it('fileio_test_rename_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_rename_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.renameSync(fpath, '/');
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_rename_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0300
   * @tc.name fileio_test_rename_sync_003
   * @tc.desc Function of API, renameSync. The test file is not exist, fpathTarget is same with fpatch, file name are same.
   */
  it('fileio_test_rename_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_rename_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      console.log('fileio_test_rename_sync_003 Test start');
      fileio.renameSync(fpath, fpath);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0400
   * @tc.name fileio_test_rename_sync_004
   * @tc.desc Function of API, no fpath parameter.
   */
  it('fileio_test_rename_sync_004', 0, async function () {
    try {
      fileio.renameSync(1, 1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_004 pass. ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0500
   * @tc.name fileio_test_rename_sync_005
   * @tc.desc Function of API, no fpathTarget parameter.
   */
  it('fileio_test_rename_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_rename_sync_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.renameSync(fpath, 1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_rename_sync_005 pass. ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0600
   * @tc.name fileio_test_rename_sync_006
   * @tc.desc Function of API, rename dir.
   */
  it('fileio_test_rename_sync_006', 0, async function () {
    let dpath = await nextFileName('fileio_test_rename_sync_006') + 'd'
    let dpathTarget = await nextFileName('fileio_test_rename_sync_006-1') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      expect(fileio.renameSync(dpath, dpathTarget) == undefined).assertTrue();
      expect(fileio.accessSync(dpathTarget) !== null).assertTrue();
      expect(fileio.rmdirSync(dpathTarget) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0700
   * @tc.name fileio_test_rename_sync_007
   * @tc.desc Function of API, dir not exist. The test dir is not exist.
   */
  it('fileio_test_rename_sync_007', 0, async function () {
    let dpath = await nextFileName('fileio_test_rename_sync_007') + 'd'
    let dpathTarget = await nextFileName('fileio_test_rename_sync_007-1') + 'd'
    try {
      fileio.renameSync(dpath, dpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0800
   * @tc.name fileio_test_rename_sync_008
   * @tc.desc Function of API, fpathTarget not exist. fpathTarget not exist.
   */
  it('fileio_test_rename_sync_008', 0, async function () {
    let dpath = await nextFileName('fileio_test_rename_sync_008') + 'd'
    let dpathTarget = '/data/accounts/account_0/appdata/aaa/bbb/fileio_test_rename_sync_008-1d'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      fileio.renameSync(dpath, dpathTarget);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      console.log('fileio_test_rename_sync_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_RenameSync_0900
   * @tc.name fileio_test_rename_sync_009
   * @tc.desc Function of API, fpath is same with fpathTarget. fpath is same with fpathTarget.
   */
  it('fileio_test_rename_sync_009', 0, async function () {
    let dpath = await nextFileName('fileio_test_rename_sync_009') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let result = fileio.renameSync(dpath, dpath);
      expect(result === undefined).assertTrue();
      expect(fileio.accessSync(dpath) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_rename_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0000
   * @tc.name fileio_test_fchmod_sync_000
   * @tc.desc Function of API, fchmodSync(mode=0o660). The test file is exist.
   */
  it('fileio_test_fchmod_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o660) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o660).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0100
   * @tc.name fileio_test_fchmod_sync_001
   * @tc.desc Function of API, fchmodSync(mode=0o460). The test file is exist.
   */
  it('fileio_test_fchmod_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o460) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o460).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0200
   * @tc.name fileio_test_fchmod_sync_002
   * @tc.desc Function of API, fchmodSync(mode=0o260). The test file is exist.
   */
  it('fileio_test_fchmod_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o260) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o260).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0300
   * @tc.name fileio_test_fchmod_sync_003
   * @tc.desc Function of API, file not exist. The test file is not exist.
   */
  it('fileio_test_fchmod_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_003');
    try {
      expect(fileio.fchmodSync(fpath, 0o660) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0400
   * @tc.name fileio_test_fchmod_sync_004
   * @tc.desc Function of API, mode is invalid. The test file is exist.
   */
  it('fileio_test_fchmod_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 2222222222222) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_004 has failed for A' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0500
   * @tc.name fileio_test_fchmod_sync_005
   * @tc.desc Function of API, fpath = dir. The test dir is exist.
   */
  it('fileio_test_fchmod_sync_005', 0, async function () {
    let dpath = await nextFileName('fileio_test_fchmod_sync_005') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o777) !== null).assertTrue();
      let fd = fileio.openSync(dpath);
      console.log(fd);
      expect(fileio.fchmodSync(fd, 0o660) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0600
   * @tc.name fileio_test_fchmod_sync_006
   * @tc.desc Function of API, fpatch = dir. The test dir is not exist.
   */
  it('fileio_test_fchmod_sync_006', 0, async function () {
    let dpath;
    try {
      fileio.fchmodSync(dpath, 0o660);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0700
   * @tc.name fileio_test_fchmod_sync_007
   * @tc.desc Function of API, fchmodSync(mode=0o460). The test file is exist.
   */
  it('fileio_test_fchmod_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o700) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o700).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0800
   * @tc.name fileio_test_fchmod_sync_008
   * @tc.desc Function of API, fchmodSync(mode=0o400). The test file is exist.
   */
  it('fileio_test_fchmod_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_008');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o400) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o400).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0900
   * @tc.name fileio_test_fchmod_sync_009
   * @tc.desc Function of API, fchmodSync(mode=0o200). The test file is exist.
   */
  it('fileio_test_fchmod_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_009');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o200) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o200).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0100
   * @tc.name fileio_test_fchmod_sync_010
   * @tc.desc Function of API, fchmodSync(mode=0o100). The test file is exist.
   */
  it('fileio_test_fchmod_sync_010', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o100) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o100).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0110
   * @tc.name fileio_test_fchmod_sync_011
   * @tc.desc Function of API, fchmodSync(mode=0o070). The test file is exist.
   */
  it('fileio_test_fchmod_sync_011', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_011');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o070) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o070).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0120
   * @tc.name fileio_test_fchmod_sync_012
   * @tc.desc Function of API, fchmodSync(mode=0o040). The test file is exist.
   */
  it('fileio_test_fchmod_sync_012', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_012');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o040) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o040).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0130
   * @tc.name fileio_test_fchmod_sync_013
   * @tc.desc Function of API, fchmodSync(mode=0o020). The test file is exist.
   */
  it('fileio_test_fchmod_sync_013', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_013');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o020) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o020).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_013 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0140
   * @tc.name fileio_test_fchmod_sync_014
   * @tc.desc Function of API, fchmodSync(mode=0o010). The test file is exist.
   */
  it('fileio_test_fchmod_sync_014', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_014');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o010) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o010).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0150
   * @tc.name fileio_test_fchmod_sync_015
   * @tc.desc Function of API, fchmodSync(mode=0o007). The test file is exist.
   */
  it('fileio_test_fchmod_sync_015', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_015');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o007) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o007).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0160
   * @tc.name fileio_test_fchmod_sync_016
   * @tc.desc Function of API, fchmodSync(mode=0o004). The test file is exist.
   */
  it('fileio_test_fchmod_sync_016', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_016');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o004) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o004).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0170
   * @tc.name fileio_test_fchmod_sync_017
   * @tc.desc Function of API, fchmodSync(mode=0o002). The test file is exist.
   */
  it('fileio_test_fchmod_sync_017', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_017');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o002) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o002).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchmodSync_0180
   * @tc.name fileio_test_fchmod_sync_018
   * @tc.desc Function of API, fchmodSync(mode=0o001). The test file is exist.
   */
  it('fileio_test_fchmod_sync_018', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_018');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchmodSync(fd, 0o001) !== null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o001).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchmod_sync_018 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FtruncateSync_0000
   * @tc.name fileio_test_ftruncate_sync_000
   * @tc.desc Function of API, ftruncateSync. The test file is exist.
   */
  it('fileio_test_ftruncate_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_ftruncate_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fileio.ftruncateSync(fd, 10) !== null).assertTrue();
      expect(fileio.statSync(fpath).size == 10).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_ftruncate_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FtruncateSync_0100
   * @tc.name fileio_test_ftruncate_sync_001
   * @tc.desc Function of API, len not for value. The test file is exist.
   */
  it('fileio_test_ftruncate_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_ftruncate_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fileio.ftruncateSync(fd) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_ftruncate_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FtruncateSync_0200
   * @tc.name fileio_test_ftruncate_sync_002
   * @tc.desc Function of API, invalid parameter. The test file is not exist.
   */
  it('fileio_test_ftruncate_sync_002', 0, async function () {
    try {
      fileio.ftruncateSync(1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_ftruncate_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FtruncateSync_0300
   * @tc.name fileio_test_ftruncate_sync_003
   * @tc.desc Function of API, invalid parameter. The test file is not exist.
   */
  it('fileio_test_ftruncate_sync_003', 0, async function () {
    try {
      fileio.ftruncateSync(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_ftruncate_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FtruncateSync_0400
   * @tc.name fileio_test_ftruncate_sync_004
   * @tc.desc Function of API, Parameter exception(len = -1).
   */
  it('fileio_test_ftruncate_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_ftruncate_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fileio.ftruncateSync(fd, -1) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_ftruncate_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FsyncSync_0000
   * @tc.name fileio_test_fsync_sync_000
   * @tc.desc Function of API, fsyneSync.
   */
  it('fileio_test_fsync_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fsync_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fileio.fsyncSync(fd) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fsync_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FsyncSync_0100
   * @tc.name fileio_test_fsync_sync_001
   * @tc.desc Function of API, no value of parameter.
   */
  it('fileio_test_fsync_sync_001', 0, async function () {
    try {
      fileio.fsyncSync(1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fsync_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FsyncSync_0200
   * @tc.name fileio_test_fsync_sync_002
   * @tc.desc Function of API, invalid parameter.
   */
  it('fileio_test_fsync_sync_002', 0, async function () {
    try {
      fileio.fsyncSync(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fsync_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0000
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_000
   * @tc.desc Sync to mkdir and rmdir.
   */
  it('fileio_test_mkdir_sync_rmdir_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_fsync_sync_000') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0100
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_001
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o660).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_001') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o660) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0200
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_002
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o460).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_002') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o460) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0300
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_003
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o260).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_003') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o260) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0400
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_004
   * @tc.desc Function of API, invalid parameter. The test file is not exist.
   */
  it('fileio_test_mkdir_sync_rmdir_sync_004', 0, async function () {
    try {
      expect(fileio.mkdirSync(1) !== null);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fsync_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0500
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_005
   * @tc.desc Function of API, invalid parameter. The test file is not exist.
   */
  it('fileio_test_mkdir_sync_rmdir_sync_005', 0, async function () {
    try {
      expect(fileio.mkdirSync('/', 1) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fsync_sync_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0600
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_006
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o700).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_006', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_006') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o700) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0700
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_007
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o400).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_007', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_007') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o400) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0800
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_008
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o200).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_008', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_008') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o200) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_0900
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_009
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o100).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_009', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_009') + 'd'
    try {
      expect(fileio.mkdirSync(dpath, 0o100) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1000
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_010
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o070).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_010', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_010') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o070) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_010 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1100
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_011
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o040).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_011', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_011') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o040) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_011 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1200
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_012
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o020).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_012', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_012') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o020) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_012 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1300
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_013
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o010).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_013', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_013') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o010) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_013 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1400
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_014
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o007).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_014', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_014') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o007) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_014 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1500
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_015
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o004).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_015', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_015') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o004) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_015 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1600
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_016
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o002).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_016', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_016') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o002) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_016 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_MkdirSync_RmdirSync_1700
   * @tc.name fileio_test_mkdir_sync_rmdir_sync_017
   * @tc.desc Sync to mkdir and rmdir. mkdirSync(mode=0o001).
   */
  it('fileio_test_mkdir_sync_rmdir_sync_017', 0, async function () {
    let dpath = await nextFileName('fileio_test_mkdir_sync_rmdir_sync_017') + 'd';
    try {
      expect(fileio.mkdirSync(dpath, 0o001) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_mkdir_sync_rmdir_sync_017 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FstatSync_0000
   * @tc.name fileio_test_fstat_sync_000
   * @tc.desc Function of API, fstatSync. The test file is exist.
   */
  it('fileio_test_fstat_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let stat = fileio.fstatSync(fd);
      expect(stat !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fstat_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FstatSync_0100
   * @tc.name fileio_test_fstat_sync_001
   * @tc.desc Function of API, fstatSync, fd = -1. The test file is not exist.
   */
  it('fileio_test_fstat_sync_001', 0, async function () {
    try {
      expect(fileio.fstatSync(1) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fstat_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FstatSync_0200
   * @tc.name fileio_test_fstat_sync_002
   * @tc.desc Function of API, fstatSync, fd = 1. The test file is not exist.
   */
  it('fileio_test_fstat_sync_002', 0, async function () {
    try {
      expect(fileio.fstatSync(1) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_fstat_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FstatSync_0300
   * @tc.name fileio_test_fstat_sync_003
   * @tc.desc Function of API, fstatSync, vaild fd. The test file is exist.
   */
  it('fileio_test_fstat_sync_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_fstat_sync_003') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let fd = fileio.openSync(dpath);
      expect(fd !== null).assertTrue();
      let stat = fileio.fstatSync(fd);
      expect(stat !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fstat_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChownSync_0000
   * @tc.name fileio_test_chown_sync_000
   * @tc.desc Function of API, chownSync. The test file is exist.
   */
  it('fileio_test_chown_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_chown_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid
    let GID = stat.gid
    try {
      expect(fileio.chownSync(fpath, UID, GID) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_chown_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChownSync_0100
   * @tc.name fileio_test_chown_sync_001
   * @tc.desc Function of API, chownSync. The test file is not exist.
   */
  it('fileio_test_chown_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_chown_sync_001');
    let fpathc = await nextFileName('fileio_test_chown_sync_001_1');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid
    let GID = stat.gid
    try {
      expect(fileio.chownSync(fpathc, UID, GID));
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_chown_sync_001 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_ChownSync_0200
   * @tc.name fileio_test_chown_sync_002
   * @tc.desc Function of API, invalid fd. The test file is not exist.
   */
  it('fileio_test_chown_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_chown_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      expect(fileio.chownSync(fpath, 0, 0));
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_chown_sync_002 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0000
   * @tc.name fileio_test_fchown_sync_000
   * @tc.desc Function of API, fchownSync. The test file is exist.
   */
  it('fileio_test_fchown_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid
    let GID = stat.gid
    try {
      let fd = fileio.openSync(fpath);
      expect(fileio.fchownSync(fd, UID, GID) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_fchown_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0100
   * @tc.name fileio_test_fchown_sync_001
   * @tc.desc Function of API, fchownSync. The test file is not exist.
   */
  it('fileio_test_fchown_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid
    let GID = stat.gid
    try {
      expect(fileio.fchownSync(-1, UID, GID));
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_001 has failed for ' + e);
    }

  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0200
   * @tc.name fileio_test_fchown_sync_002
   * @tc.desc Function of API, fchownSync, wrong owner. The test file is exist.
   */
  it('fileio_test_fchown_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let fd;
    let GID = stat.gid
    try {
      let fd = fileio.openSync(fpath);
      fileio.fchownSync(fd, cc, GID);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0300
   * @tc.name fileio_test_fchown_sync_003
   * @tc.desc Function of API, fchownSync, wrong group. The test file is exist.
   */
  it('fileio_test_fchown_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid;
    let fd;
    try {
      fd = fileio.openSync(fpath, 0o2);
      expect(fileio.fchownSync(fd, UID, 0));
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0400
   * @tc.name fileio_test_fchown_sync_004
   * @tc.desc Function of API, fchownSync, no value of fd. The test file is exist.
   */
  it('fileio_test_fchown_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid
    let GID = stat.gid
    let fd = null
    try {
      expect(fileio.fchownSync(fd, UID, GID));
      expect(null).assertFail();

    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_004 has failed for ' + e);
    }

  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0500
   * @tc.name fileio_test_fchown_sync_005
   * @tc.desc Function of API, fchownSync, no value of owner. The test file is exist.
   */
  it('fileio_test_fchown_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = null
    let GID = stat.gid
    let fd;
    try {
      fd = fileio.openSync(fpath);
      expect(fileio.fchownSync(fd, UID, GID));
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0600
   * @tc.name fileio_test_fchown_sync_006
   * @tc.desc Function of API, fchownSync, no value of group. The test file is exist.
   */
  it('fileio_test_fchown_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let stat = fileio.statSync(fpath);
    let UID = stat.uid;
    let fd;
    try {
      fd = fileio.openSync(fpath);
      fileio.fchownSync(fd, UID, 0);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_FchownSync_0700
   * @tc.name fileio_test_fchown_sync_007
   * @tc.desc Function of API, fchownSync, no value of GID,UID. The test file is exist.
   */
  it('fileio_test_fchown_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let fd;
    try {
      fd = fileio.openSync(fpath, 0o2);
      fileio.fchownSync(fd, 0, 0);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_fchown_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_prop_write_open_read_4k_sync
   * @tc.name fileio_test_prop_write_open_read_4k_sync
   * @tc.desc Sync to write & open & read 4K file.
   */
  it('fileio_test_prop_write_open_read_4k_sync', 0, async function () {
    let fpath = await nextFileName('fileio_test_prop_write_open_read_4k_sync');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o777);
      expect(fd !== null).assertTrue();
      expect(fileio.writeSync(fd, randomString(4096)) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      let fd1 = fileio.openSync(fpath, 0o2);
      let rlen = fileio.readSync(fd1, new ArrayBuffer(4096));
      expect(rlen == 4096).assertTrue();
      expect(fileio.closeSync(fd1) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_prop_write_open_read_4k_sync has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_prop_write_open_read_4k_sync
   * @tc.name fileio_test_prop_copyFile_4k_sync
   * @tc.desc Sync to copy 4K file.
   */
  it('fileio_test_prop_copyFile_4k_sync', 0, async function () {
    let fpath = await nextFileName('fileio_test_prop_copyFile_4k_sync');
    let fpath1 = await fileName('fileio_test_prop_copyFile_4k_1_sync');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o777);
      expect(fd !== null).assertTrue();
      expect(fileio.writeSync(fd, randomString(4096)) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(fileio.copyFileSync(fpath, fpath1) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.unlinkSync(fpath1) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_prop_copyFile_4k_sync has failed for ' + e);
      expect(null).assertFail();
    }
  });
});