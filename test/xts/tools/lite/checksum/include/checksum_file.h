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

 /*
 * Description: 对单个文件或目录下多文件进行计算SHA256校验和
 */

#ifndef CHECKSUM_FILE_H
#define CHECKSUM_FILE_H

#define RESULT_SUCCESS 0
#define RESULT_FAILED (-1)


int CalcSingleFileSha256(char *fileName);
int CalcMultiFilesSha256(char *dirPathName);
int GetFileTotalNum(void);

#endif // CHECKSUM_FILE_PROC_H
