/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Header file for memory init in JUPITER.
 * Create: 2021/01/24
 */

#ifndef JERRYSCRIPT_ADAPTER_H
#define JERRYSCRIPT_ADAPTER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

void JerryPsRamMemInit();
void JerryBmsPsRamMemInit();
void JerryInitContextRecords();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // JERRYSCRIPT_ADAPTER_H