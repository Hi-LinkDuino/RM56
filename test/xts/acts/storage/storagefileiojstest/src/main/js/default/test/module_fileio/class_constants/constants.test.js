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
  describe, it, expect,
} from '../../Common';

describe('constants', function () {

  /**
   * @tc.number SUB_DF_FILEIO_CONSTANTS
   * @tc.name fileio_test_constants
   * @tc.desc Test constants() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_constants', 0, function () {
    try {
      expect(fileio.constants.F_OK == 0).assertTrue();
      expect(fileio.constants.X_OK == 1).assertTrue();
      expect(fileio.constants.W_OK == 2).assertTrue();
      expect(fileio.constants.R_OK == 4).assertTrue();
      expect(fileio.constants.O_RDONLY == 0o0).assertTrue();
      expect(fileio.constants.O_WRONLY == 0o1).assertTrue();
      expect(fileio.constants.O_RDWR == 0o2).assertTrue();
      expect(fileio.constants.O_CREAT == 0o100).assertTrue();
      expect(fileio.constants.O_EXCL == 0o200).assertTrue();
      expect(fileio.constants.O_TRUNC == 0o1000).assertTrue();
      expect(fileio.constants.O_APPEND == 0o2000).assertTrue();
      expect(fileio.constants.O_NONBLOCK == 0o4000).assertTrue();
      expect(fileio.constants.O_DIRECTORY == 0o200000).assertTrue();
      expect(fileio.constants.O_NOFOLLOW == 0o400000).assertTrue();
      expect(fileio.constants.O_SYNC == 0o4010000).assertTrue();
      expect(fileio.constants.S_IFMT == 0o170000).assertTrue();
      expect(fileio.constants.S_IFSOCK == 0o140000).assertTrue();
      expect(fileio.constants.S_IFLNK == 0o120000).assertTrue();
      expect(fileio.constants.S_IFREG == 0o100000).assertTrue();
      expect(fileio.constants.S_IFBLK == 0o060000).assertTrue();
      expect(fileio.constants.S_IFDIR == 0o040000).assertTrue();
      expect(fileio.constants.S_IFCHR == 0o020000).assertTrue();
      expect(fileio.constants.S_IFIFO == 0o010000).assertTrue();
      expect(fileio.constants.S_IRWXU == 0o0700).assertTrue();
      expect(fileio.constants.S_IRUSR == 0o0400).assertTrue();
      expect(fileio.constants.S_IWUSR == 0o0200).assertTrue();
      expect(fileio.constants.S_IXUSR == 0o0100).assertTrue();
      expect(fileio.constants.S_IRWXG == 0o0070).assertTrue();
      expect(fileio.constants.S_IRGRP == 0o0040).assertTrue();
      expect(fileio.constants.S_IWGRP == 0o0020).assertTrue();
      expect(fileio.constants.S_IXGRP == 0o0010).assertTrue();
      expect(fileio.constants.S_IRWXO == 0o0007).assertTrue();
      expect(fileio.constants.S_IROTH == 0o0004).assertTrue();
      expect(fileio.constants.S_IWOTH == 0o0002).assertTrue();
      expect(fileio.constants.S_IXOTH == 0o0001).assertTrue();
    } catch (e) {
      console.log('fileio_test_constants has failed for ' + e);
      expect(null).assertFail();
    }
  });
});