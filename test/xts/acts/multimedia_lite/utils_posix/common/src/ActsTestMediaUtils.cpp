/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ActsTestMediaUtils.h"

using namespace std;

/* *
 * get current dir
 * @return  string current file path of the test suits
 */
string GetCurDir()
{
    string filePath = "";
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("get file path error");
    } else {
        printf("Current Dir: %s\r\n", buffer);
        filePath = buffer;
        free(buffer);
    }
    return filePath + "/";
}

/* *
 * check file exist
 * @param filename filename
 * @return  check result
 */
int32_t FileCheck(const char *filename)
{
    fstream fileTmp;
    fileTmp.open(filename);
    if (!fileTmp) {
        cout << "file is not exist!" << endl;
        return RET_ERR;
    } else {
        cout << "file is exist!" << endl;
        fileTmp.close();
        return RET_OK;
    }
}

/* *
 * Save Capture
 * @return
 */
int32_t SampleSaveCapture(string testPath, const char *p, uint32_t size)
{
    cout << "Start saving picture" << endl;
    string filePath = "";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *ltm = localtime(&tv.tv_sec);
    if (ltm != nullptr) {
        ostringstream ss("Capture_");
        ss << "Capture" << ltm->tm_hour << "_" << ltm->tm_min << "_" << ltm->tm_sec << ".jpg";
        filePath = testPath + ss.str();
        ofstream pic(testPath + ss.str(), ofstream::out | ofstream::trunc);
        cout << "write " << size << " bytes" << endl;
        pic.write(p, size);
        cout << "Saving picture end" << endl;
    }
    const char *filename = filePath.data();
    int32_t ret = FileCheck(filename);
    return ret;
}

/* *
 * get recorder fd
 * @return fd
 */
int32_t SampleGetRecordFd(string &recordFilePath)
{
    struct timeval tv = {};
    gettimeofday(&tv, nullptr);
    struct tm *ltm = localtime(&tv.tv_sec);
    int32_t fd = FdNull;
    if (ltm != nullptr) {
        ostringstream ss("Recorder_");
        ss << "Record" << ltm->tm_hour << "_" << ltm->tm_min << "_" << ltm->tm_sec << ".mp4";
        recordFilePath = recordFilePath + ss.str();
        fd = open(recordFilePath.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        cout << "Open " << recordFilePath << endl;
        if (fd == FdNull) {
            cout << "Open recorder file failed. err=" << strerror(errno) << endl;
        }
    }
    return fd;
}