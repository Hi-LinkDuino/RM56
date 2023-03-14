/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef SHARED_OBJ_H
#define SHARED_OBJ_H

#include "osal/osal_atomic.h"
#include "osal/osal_mem.h"

#define SHARED_OBJ(OBJ_TYPE) enum MessageEngineStatus status;                \
    OsalAtomic refCount;                            \
    struct OBJ_TYPE *(*Ref)(struct OBJ_TYPE * obj); \
    void (*Disref)(struct OBJ_TYPE * obj);          \
    void (*Destroy)(struct OBJ_TYPE * obj)

#define DECLEAR_SHARED_OBJ_FUNC(OBJ_TYPE)                                                        \
    struct OBJ_TYPE *Reference##OBJ_TYPE(struct OBJ_TYPE *obj);                                  \
    void Disreference##OBJ_TYPE(struct OBJ_TYPE *obj);                                           \
    ErrorCode InitSharedObj##OBJ_TYPE(struct OBJ_TYPE *obj, void (*Destroy)(struct OBJ_TYPE *)); \
    void DeinitSharedObj##OBJ_TYPE(struct OBJ_TYPE *obj)

#define IMPLEMENT_SHARED_OBJ(OBJ_TYPE)                                                          \
    struct OBJ_TYPE *Reference##OBJ_TYPE(struct OBJ_TYPE *obj)                                  \
    {                                                                                           \
        if (obj == NULL) {                                                                      \
            return NULL;                                                                        \
        }                                                                                       \
        if (obj->status > ME_STATUS_RUNNING) {                                                  \
            return NULL;                                                                        \
        }                                                                                       \
        struct OBJ_TYPE *result = NULL;                                                         \
        if (obj->status <= ME_STATUS_RUNNING && OsalAtomicRead(&obj->refCount) < MAX_OBJ_REF_COUNT) {            \
            OsalAtomicInc(&obj->refCount);                                                       \
            result = obj;                                                                       \
        }                                                                                       \
        return result;                                                                          \
    }                                                                                           \
    void Disreference##OBJ_TYPE(struct OBJ_TYPE *obj)                                           \
    {                                                                                           \
        if (obj == NULL) {                                                                      \
            return;                                                                             \
        }                                                                                       \
        OsalAtomicDec(&obj->refCount);                                                           \
        if (OsalAtomicRead(&obj->refCount) <= 0) {                                               \
            obj->status = ME_STATUS_TODESTROY;                                                  \
            if (obj->Destroy != NULL) {                                                         \
                obj->Destroy(obj);                                                              \
                OsalMemFree(obj);                                                               \
            }                                                                                   \
        }                                                                                       \
    }                                                                                           \
    ErrorCode InitSharedObj##OBJ_TYPE(struct OBJ_TYPE *obj, void (*Destroy)(struct OBJ_TYPE *)) \
    {                                                                                           \
        if (Destroy == NULL) {                                                                  \
            return ME_ERROR_NULL_PTR;                                                           \
        }                                                                                       \
        OsalAtomicSet(&obj->refCount, 1);                                                        \
        obj->Ref = Reference##OBJ_TYPE;                                                         \
        obj->Disref = Disreference##OBJ_TYPE;                                                   \
        obj->Destroy = Destroy;                                                                 \
        return ME_SUCCESS;                                                                      \
    }                                                                                           \
    void DeinitSharedObj##OBJ_TYPE(struct OBJ_TYPE *obj)                                        \
    {                                                                                           \
        if (obj != NULL) {                                                                      \
            OsalAtomicSet(&obj->refCount, 0);                                                    \
        }                                                                                       \
    }

#define INIT_SHARED_OBJ(OBJ_TYPE, obj, DestroyFunc) InitSharedObj##OBJ_TYPE(obj, DestroyFunc)
#define DEINIT_SHARED_OBJ(OBJ_TYPE, obj) DeinitSharedObj##OBJ_TYPE(obj)


#endif
