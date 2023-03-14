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
describe('fileio_rename', function () {

  /**
   * @tc.number SUB_DF_FILEIO_RENAME_ASYNC_0000
   * @tc.name fileio_test_rename_async_000
   * @tc.desc Test renameAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rename_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_rename_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let newf = fpath + '123';
      await fileio.rename(fpath, newf)
        .then(function (err) {
          expect(fileio.accessSync(newf) == null).assertTrue();
          expect(err == null).assertTrue();
          expect(fileio.unlinkSync(newf) == null).assertTrue();
        })
        .catch(function (e) {
          expect(e == null).assertTrue();
        })
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_RENAME_ASYNC_0010
   * @tc.name fileio_test_rename_async_001
   * @tc.desc Test renameAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rename_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_rename_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let newf = fpath + 'aaa';
      await fileio.rename(fpath, newf, function (err) {
        expect(fileio.accessSync(newf) == null).assertTrue();
        expect(fileio.unlinkSync(newf) == null).assertTrue();
        done();
      })
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_RENAME_ASYNC_0020
   * @tc.name fileio_test_rename_async_002
   * @tc.desc Test renameAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_rename_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_rename_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.access(fpath).then(function (err) {
        let newf = fpath + '123';
        fileio.rename(fpath, newf)
          .then(function (err) {
            expect(fileio.accessSync(newf) == null).assertTrue();
            expect(err == null).assertTrue();
            expect(fileio.unlinkSync(newf) == null).assertTrue();
          })
      })
        .catch(function (e) {
          expect(e == null).assertTrue();
        })
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });
});