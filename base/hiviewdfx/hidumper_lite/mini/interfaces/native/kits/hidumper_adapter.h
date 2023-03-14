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

#ifndef HIDUMPER_ADAPTER_H
#define HIDUMPER_ADAPTER_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

extern int DumpSysInfo(void);
extern int DumpCpuUsage(void);
extern int DumpMemUsage(void);
extern int DumpTaskInfo(void);
extern int DumpFaultLog(void);
extern int DumpMemRegion(unsigned long long addr, unsigned long long size);
extern int DumpAllMem(void);
extern int PlatformHiDumperIinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HIDUMPER_ADAPTER_H */