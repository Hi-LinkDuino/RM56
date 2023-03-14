/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_object_manager.h"

struct HdfObject *HdfObjectManagerGetObject(int objectId)
{
    struct HdfObject *object = NULL;
    const struct HdfObjectCreator *targetCreator = HdfObjectManagerGetCreators(objectId);
    if ((targetCreator != NULL) && (targetCreator->Create != NULL)) {
        object = targetCreator->Create();
        if (object != NULL) {
            object->objectId = objectId;
        }
    }
    return object;
}

void HdfObjectManagerFreeObject(struct HdfObject *object)
{
    const struct HdfObjectCreator *targetCreator = NULL;
    if (object == NULL) {
        return;
    }
    targetCreator = HdfObjectManagerGetCreators(object->objectId);
    if ((targetCreator == NULL) || (targetCreator->Release == NULL)) {
        return;
    }
    targetCreator->Release(object);
}