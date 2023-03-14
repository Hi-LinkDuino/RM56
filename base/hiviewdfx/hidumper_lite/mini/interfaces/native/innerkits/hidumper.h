/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HIDUMPER_H
#define HIDUMPER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

struct HiDumperAdapter {
    int (*DumpSysInfo)(void);
    int (*DumpCpuUsage)(void);
    int (*DumpMemUsage)(void);
    int (*DumpTaskInfo)(void);
    int (*DumpFaultLog)(void);
    int (*DumpMemRegion)(unsigned long long addr, unsigned long long size);
    int (*DumpAllMem)(void);
};

int HiDumperRegisterAdapter(struct HiDumperAdapter *pAdapter);
unsigned int at_hidumper(unsigned int argc, const char **argv);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* End of #if __cplusplus */
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HIDUMPER_H */
