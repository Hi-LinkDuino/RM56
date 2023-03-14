/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HC_VECTOR_H
#define HC_VECTOR_H

#include "hc_parcel.h"
#include "securec.h"

/*
 * Use DECLARE_HC_VECTOR to declare the vector in the head/src file.
 * @para ClassName: the name of the vector-class/vector-struct
 * @para Element: the type of the vector element
 * @for example:
 * DECLARE_HC_VECTOR(IntVec, int)
 */
#define DECLARE_HC_VECTOR(ClassName, Element) \
typedef struct V##ClassName{ \
    Element* (*pushBack)(struct V##ClassName*, const Element*); \
    Element* (*pushBackT)(struct V##ClassName*, Element); \
    HcBool (*popFront)(struct V##ClassName*, Element*); \
    HcBool (*eraseElement)(struct V##ClassName*, Element*, uint32_t index); \
    uint32_t (*size)(const struct V##ClassName*); \
    Element (*get)(const struct V##ClassName*, uint32_t index); \
    Element* (*getp)(const struct V##ClassName*, uint32_t index); \
    void (*clear)(struct V##ClassName*); \
    HcParcel parcel; \
} ClassName;

/*
 * Use IMPLEMENT_HC_VECTOR to implement the vector in the source file.
 * @para ClassName: the name of the vector-class/vector-struct
 * @para Element: the type of the vector element
 * @para allocCount: the minimum alloc count
 * @for example:
 * IMPLEMENT_HC_VECTOR(IntVec, int)
 */
#define IMPLEMENT_HC_VECTOR(ClassName, Element, allocCount) \
Element* VPushBack##ClassName(ClassName* obj, const Element *e) { \
    if (obj == NULL || e == NULL) { \
        return NULL; \
    } \
      \
    if (ParcelWrite(&obj->parcel, e, sizeof(Element))) { \
        int size = obj->size(obj); \
        return obj->getp(obj, size-1); \
    } else { \
        return NULL; \
    } \
} \
Element* VPushBackT##ClassName(ClassName* obj, Element e) { \
    if (obj == NULL) { \
        return NULL; \
    } \
      \
    if (ParcelWrite(&obj->parcel, &e, sizeof(Element))) { \
        int size = obj->size(obj); \
        return obj->getp(obj, size-1); \
    } else { \
        return NULL; \
    } \
} \
HcBool VPopFront##ClassName(ClassName* obj, Element* e) { \
        if (NULL == obj || NULL == e) { \
            return HC_FALSE; \
        } \
        if (obj->size(obj) > 0) { \
            return ParcelRead(&obj->parcel, e, sizeof(Element)); \
        } else { \
            return HC_FALSE; \
        } \
} \
HcBool VErase##ClassName(ClassName* obj, Element* e, uint32_t index) { \
        if (NULL == obj || NULL == e || index + 1 > obj->size(obj)) { \
            return HC_FALSE; \
        } \
        if (obj->size(obj) > 0) { \
            return ParcelEraseBlock(&obj->parcel, index*sizeof(Element), sizeof(Element), e); \
        } else { \
            return HC_FALSE; \
        } \
} \
uint32_t VSize##ClassName(const ClassName* obj) \
{ \
    if (NULL == obj) { \
        return 0; \
    } \
    return GetParcelDataSize(&obj->parcel) / sizeof(Element); \
} \
Element VGet##ClassName(const ClassName* obj, uint32_t index) \
{ \
    Element e; \
    (void)memset_s(&e, sizeof(e), 0, sizeof(e)); \
    if (NULL != obj) { \
        if (index < obj->size(obj)) { \
            if (GetParcelData(&obj->parcel)) { \
                return *((Element*)(GetParcelData(&obj->parcel)) + index); \
            } else { \
                return e; \
            } \
        } \
    } \
    (void)memset_s(&e, sizeof(e), 0, sizeof(e)); \
    return e; \
} \
Element* VGetPointer##ClassName(const ClassName* obj, uint32_t index) \
{ \
    if (NULL != obj) { \
        if (index < obj->size(obj)) { \
            if (GetParcelData(&obj->parcel)) { \
                return ((Element*)(GetParcelData(&obj->parcel)) + index); \
            } else { \
                return NULL; \
            } \
        } \
    } \
    return NULL; \
} \
void VClear##ClassName(ClassName* obj) \
{ \
    if (NULL != obj) { \
        ClearParcel(&obj->parcel); \
    } \
} \
ClassName Create##ClassName(void) \
{ \
    ClassName obj; \
    obj.pushBack = VPushBack##ClassName; \
    obj.pushBackT = VPushBackT##ClassName; \
    obj.popFront = VPopFront##ClassName; \
    obj.clear = VClear##ClassName; \
    obj.eraseElement = VErase##ClassName; \
    obj.size = VSize##ClassName; \
    obj.get = VGet##ClassName; \
    obj.getp = VGetPointer##ClassName; \
    obj.parcel = CreateParcel(0, sizeof(Element) * allocCount); \
    return obj; \
} \
void Destroy##ClassName(ClassName* obj) \
{ \
    if (NULL != obj) { \
        DeleteParcel(&obj->parcel); \
    } \
}

/* Use these two macros to create and destroy vector */
#define CREATE_HC_VECTOR(classname) Create##classname()
#define DESTROY_HC_VECTOR(classname, obj) Destroy##classname(obj)

#define FOR_EACH_HC_VECTOR(vec, index, iter) for (index = 0; index < (vec).size(&(vec)) && \
    (iter = (vec).getp(&(vec), index)); ++index)

#define HC_VECTOR_PUSHBACK(obj, element) (obj)->pushBack((obj), (element))
#define HC_VECTOR_POPFRONT(obj, element) (obj)->popFront((obj), (element))
#define HC_VECTOR_POPELEMENT(obj, element, index) (obj)->eraseElement((obj), (element), (index))
#define HC_VECTOR_SIZE(obj) (obj)->size(obj)
#define HC_VECTOR_GET(obj, index) (obj)->get((obj), (index))
#define HC_VECTOR_GETP(_obj, _index) (_obj)->getp((_obj), (_index))

#endif
