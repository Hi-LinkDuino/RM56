/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_ASSERT_H
#define PLATFORM_ASSERT_H

#include "platform_log.h"

#define ERROR_AND_LOG(expr)                                             \
do {                                                                    \
    PLAT_LOGE(__FILE__ "(line:%d): " #expr " is false!", __LINE__);     \
} while (0)

#define CHECK(expr)                                                     \
( {                                                                     \
    if (!(expr)) {                                                      \
        ERROR_AND_LOG(expr);                                            \
    }                                                                   \
    (expr);                                                             \
})

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NULL(ptr) CHECK((ptr) == NULL)
#define CHECK_NOT_NULL(ptr) CHECK((ptr) != NULL)

#define ERROR_AND_RETURN(expr, ret)                                     \
do {                                                                    \
    PLAT_LOGE(__FILE__ "(line:%d): " #expr " is false!", __LINE__);     \
    return ret;                                                         \
} while (0)

#define CHECK_AND_RETURN(expr, ret)                                     \
( {                                                                     \
    if (!(expr)) {                                                      \
        ERROR_AND_RETURN(expr, (ret));                                  \
    }                                                                   \
    (expr);                                                             \
})

#define CHECK_EQ_RETURN(a, b, ret) CHECK_AND_RETURN((a) == (b), ret)
#define CHECK_NE_RETURN(a, b, ret) CHECK_AND_RETURN((a) != (b), ret)
#define CHECK_GE_RETURN(a, b, ret) CHECK_AND_RETURN((a) >= (b), ret)
#define CHECK_GT_RETURN(a, b, ret) CHECK_AND_RETURN((a) > (b), ret)
#define CHECK_LE_RETURN(a, b, ret) CHECK_AND_RETURN((a) <= (b), ret)
#define CHECK_LT_RETURN(a, b, ret) CHECK_AND_RETURN((a) < (b), ret)
#define CHECK_NULL_RETURN(ptr, ret) CHECK_AND_RETURN((ptr) == NULL, ret)
#define CHECK_NOT_NULL_RETURN(ptr, ret) CHECK_AND_RETURN((ptr) != NULL, ret)

#endif /* PLATFORM_ASSERT_H */
