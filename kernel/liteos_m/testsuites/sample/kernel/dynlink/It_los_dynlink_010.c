/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "los_dynlink.h"
#include "It_los_dynlink.h"

/* Test Align4_dynamic_align4.so */
STATIC UINT32 TestCase(VOID)
{
    VOID *handle1 = NULL;
    VOID *handle2 = NULL;
    INT32 (*func)(INT32, INT32) = NULL;
    CHAR *symbolName = "test_api";
    CHAR *dsoName = DSO_FULL_PATH("Align4_dynamic_align4.so");
    INT32 ret;

    handle1 = (VOID *)LOS_SoLoad(dsoName, NULL);
    ICUNIT_ASSERT_NOT_EQUAL(handle1, NULL, handle1);

    func = (INT32 (*)(INT32, INT32))LOS_FindSym(handle1, symbolName);
    ICUNIT_GOTO_NOT_EQUAL(func, NULL, func, EXIT1);
    ret = func(1, 1);
    ICUNIT_GOTO_EQUAL(ret, 14, ret, EXIT1);

    handle2 = (VOID *)LOS_SoLoad(dsoName, NULL);
    ICUNIT_GOTO_NOT_EQUAL(handle2, NULL, handle2, EXIT1);
    ICUNIT_GOTO_EQUAL(handle1, handle2, handle2, EXIT2);

    func = (INT32 (*)(INT32, INT32))LOS_FindSym(handle2, symbolName);
    ICUNIT_GOTO_NOT_EQUAL(func, NULL, func, EXIT2);
    ret = func(1, 1);
    ICUNIT_GOTO_EQUAL(ret, 14, ret, EXIT2);

    ret = LOS_SoUnload(handle2);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT1);

    ret = LOS_SoUnload(handle1);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = LOS_SoUnload(handle1);
    ICUNIT_ASSERT_NOT_EQUAL(ret, 0, ret);

    return LOS_OK;

EXIT2:
    ret = LOS_SoUnload(handle2);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
EXIT1:
    ret = LOS_SoUnload(handle1);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    return LOS_OK;
}

VOID ItLosDynlink010(VOID)
{
    TEST_ADD_CASE("ItLosDynlink010", TestCase, TEST_LOS, TEST_DYNLINK, TEST_LEVEL1, TEST_FUNCTION);
}
