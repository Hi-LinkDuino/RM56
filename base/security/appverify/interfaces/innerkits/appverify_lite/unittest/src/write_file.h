/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef APPSECURITY_WRITE_FILE_H
#define APPSECURITY_WRITE_FILE_H

#include <stdio.h>
#include <stdlib.h>
void DeleteFile(const char *path);
int CopyFile(const char *org, const char *dest);
const char* GetWrongLiceseHap(void);
const char* GetUnsignedHap(void);
const char* GetWrongUdidHap(void);
const char* GetSuccessHap(void);
const char* GetModifiedHap(void);
const char* GetBusinessHap(void);
#endif // APPSECURITY_WRITE_FILE_H