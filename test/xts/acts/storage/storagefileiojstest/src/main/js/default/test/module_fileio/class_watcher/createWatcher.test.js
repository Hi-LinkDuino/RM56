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

describe('fileio_createWatcher', function () {

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_000
   * @tc.desc Test createWatcher() interface. rename  1 times stopSync()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 1;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == event).assertTrue();
        flag = 1;
      });
      let after = fpath + 'yjz';

      await fileio.rename(fpath, after);
      setTimeout(function () {
        expect(flag == 1).assertTrue();
        ob.stopSync();
        expect(fileio.unlinkSync(after) == null).assertTrue();
        done();
      }, 51);
    } catch (e) {
      console.log('fileio_test_createWatcher_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_001
   * @tc.desc Test createWatcher() interface. write 1 times stopSync()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 2;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        flag = 1;
      });
      let fd = fileio.openSync(fpath, 0o2);
      let ss = 'okk';
      await fileio.write(fd, ss);
      await fileio.write(fd, ss);
      ob.stopSync();
      expect(flag == 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_createWatcher_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_002
   * @tc.desc Test createWatcher() interface. rename 1 time ,stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 1;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == event).assertTrue();
        flag = 1;
      });
      let after = fpath + 'yjz';
      fileio.renameSync(fpath, after);
      setTimeout(async function () {
        expect(flag == 1).assertTrue();
        await ob.stop();
        expect(fileio.unlinkSync(after) == null).assertTrue();
        done();
      }, 50);
    } catch (e) {
      console.log('fileio_test_createWatcher_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_003
   * @tc.desc Test createWatcher() interface. rename 1 time ,stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 1;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == event).assertTrue();
        flag = 3;
      });
      let after = fpath + 'yjz';
      await fileio.rename(fpath, after);
      await fileio.rename(after, after);
      ob.stop(function () {
        expect(flag == 3).assertTrue();
        expect(fileio.unlinkSync(after) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_createWatcher_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_004
   * @tc.desc Test createWatcher() interface.write  1 time ,stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 2;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == 2).assertTrue();
        flag = 4;
      });
      let fd = fileio.openSync(fpath, 0o2);
      let ss = 'okk';
      await fileio.write(fd, ss);
      await fileio.write(fd, ss);
      ob.stop(function () {
        expect(flag == 4).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_createWatcher_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_005
   * @tc.desc Test createWatcher() interface. rename 3 times
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_005', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 1;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == event).assertTrue();
        flag = flag + 1;
      });
      let after = fpath + 'yjz';
      await fileio.rename(fpath, after);
      await fileio.rename(after, fpath);
      await fileio.rename(fpath, after);
      setTimeout(function () {
        ob.stopSync();
        console.log('fileio_test_createWatcher_async_005 flag is ' + flag);
        expect(flag == 2 || flag == 3).assertTrue();
        expect(fileio.unlinkSync(after) == null).assertTrue();
        done();
      }, 51);
    } catch (e) {
      console.log('fileio_test_createWatcher_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_006
   * @tc.desc Test createWatcher() interface.write 2 times, stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_006', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 2;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        expect(data == 2).assertTrue();
        flag = flag + 1;
      });
      let fd = fileio.openSync(fpath, 0o2);
      let ss = 'okk';
      await fileio.write(fd, ss);
      await fileio.write(fd, ss);
      await ob.stop();
      console.log('fileio_test_createWatcher_async_006 flag is' + flag);
      expect(flag == 1 || flag == 2).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_createWatcher_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_007
   * @tc.desc Test createWatcher() interface.rename 1 times, write 1 times, stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_007', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 3;
      let flag = 0;
      let ob = await fileio.createWatcher(fpath, event, function (data) {
        flag = flag + 1;
      });
      let fd = fileio.openSync(fpath, 0o2);
      let ss = 'okk';
      await fileio.write(fd, ss);
      let after = fpath + 'yjz';
      await fileio.rename(fpath, after);
      await ob.stop();
      console.log('fileio_test_createWatcher_async_007 flag is ' + flag);
      expect(flag == 1 || flag == 2).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(after) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_createWatcher_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_CREATEWATCHER_000
   * @tc.name fileio_test_createWatcher_008
   * @tc.desc Test createWatcher() interface.rename 1 times, write 1 times, stop()
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_createWatcher_async_008', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_createWatcher_async_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let event = 3;
      let flag = 0;
      await fileio.createWatcher(fpath, event, function (data) {
        flag = flag + 1;
      });
      let fd = fileio.openSync(fpath, 0o2);
      let ss = 'okk';
      await fileio.write(fd, ss);
      let after = fpath + 'yjz';
      await fileio.rename(fpath, after);
      setTimeout(function () {
        expect(flag == 2).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(after) == null).assertTrue();
        done();
      }, 50);
    } catch (e) {
      console.log('fileio_test_createWatcher_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
