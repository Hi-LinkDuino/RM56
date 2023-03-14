/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "osal_file.h"
#include "osal_test_case_def.h"
#include "osal_test_type.h"
#include "securec.h"

#define HDF_LOG_TAG osal_test

#define UT_TEST_CHECK_RET(val) do { \
    if (val) { \
        HDF_LOGE("[OSAL_UT_TEST] %s %d OSA_UT_TEST_FAIL ", __func__, __LINE__); \
    } \
} while (0)

#define HDF_BUFF_LEN 30
#define HDF_PATH_LEN 50
#define HDF_FILE_MODE 0777

char *OsalFWTestBuff(void)
{
    static char *fwBuf = "123456789123456789123456789123456789123456789123456789123456789123456789";
    return fwBuf;
}
#if defined(__KERNEL__)
static void OsalPutWriteAccess(OsalFile *file)
{
    HDF_LOGE("%s %d %d", __func__, deny_write_access((struct file *)file->realFile), __LINE__);
    HDF_LOGE("%s %d %d", __func__, atomic_read(&file_inode((struct file *)file->realFile)->i_writecount), __LINE__);
    put_write_access(file_inode((struct file *)file->realFile));
    HDF_LOGE("%s %d %d", __func__, atomic_read(&file_inode((struct file *)file->realFile)->i_writecount), __LINE__);
    HDF_LOGE("%s %d %d", __func__, deny_write_access((struct file *)file->realFile), __LINE__);
}
#endif

int OsalTestFileInit(void)
{
    char *buf = OsalFWTestBuff();
#if defined(__KERNEL__)
    OsalFile file;
#endif
    int32_t ret = -1;

#if defined(__KERNEL__)
    ret = OsalFileOpen(&file, TEST_FILE_PATH_NAME, O_CREAT | OSAL_O_RDWR, OSAL_S_IWRITE);
#else
    ret = open(TEST_FILE_PATH_NAME, O_CREAT | O_RDWR, HDF_FILE_MODE);
#endif
    if (ret < 0) {
        HDF_LOGE("%s %d err:%d %s", __func__, __LINE__, ret, TEST_FILE_PATH_NAME);
        return ret;
    }

#if defined(__KERNEL__)
    UT_TEST_CHECK_RET(OsalFileWrite(&file, buf, strlen(buf)) == -1);
#else
    UT_TEST_CHECK_RET(write(ret, buf, strlen(buf)) == -1);
#endif
#if defined(__KERNEL__)
    OsalFileClose(&file);
#else
    close(ret);
#endif

#if defined(__KERNEL__)
    ret = OsalFileOpen(&file, TEST_FW_PATH_NAME, O_CREAT | OSAL_O_RDWR, OSAL_S_IWRITE);
#else
    ret = open(TEST_FW_PATH_NAME, O_CREAT | O_RDWR, HDF_FILE_MODE);
#endif
    if (ret < 0) {
        HDF_LOGE("%s %d err:%d %s", __func__, __LINE__, ret, TEST_FILE_PATH_NAME);
        return ret;
    }
#if defined(__KERNEL__)
    UT_TEST_CHECK_RET(OsalFileWrite(&file, buf, strlen(buf)) == -1);
#else
    UT_TEST_CHECK_RET(write(ret, buf, strlen(buf)) == -1);
#endif
#if defined(__KERNEL__)
    OsalPutWriteAccess(&file);
#endif

#if defined(__KERNEL__)
    OsalFileClose(&file);
#else
    close(ret);
#endif
    return 0;
}

void OsalTestFileDeInit(void)
{
    HDF_LOGE("%s end", __func__);
}

