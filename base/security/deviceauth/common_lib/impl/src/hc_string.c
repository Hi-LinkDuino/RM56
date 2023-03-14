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

#include <string.h>
#include "hc_string.h"

const uint32_t STRING_ALLOC_SIZE = 10;
const uint32_t STRING_END_CHAR_LENGTH = 1;
const char STRING_END_CHAR = '\0';

/*
 * Append a HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: append string.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppend(HcString *self, HcString str)
{
    uint32_t length = GetParcelDataSize(&str.parcel);
    if (self != NULL && length > 0) {
        // remove '\0'
        ParcelPopBack(&self->parcel, STRING_END_CHAR_LENGTH);
        // append string(include '\0')
        return StringAppendPointer(self, GetParcelData(&str.parcel));
    }

    return HC_FALSE;
}

/*
 * Append string pointer
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: string pointer.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppendPointer(HcString *self, const char *str)
{
    if (self != NULL && str != NULL) {
        // remove '\0'
        ParcelPopBack(&self->parcel, STRING_END_CHAR_LENGTH);
        // append string (include '\0')
        return ParcelWrite(&self->parcel, (void *)str, strlen(str) + 1);
    }

    return HC_FALSE;
}

/*
 * Append a char
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: char.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppendChar(HcString *self, char c)
{
    if (self != NULL && c != STRING_END_CHAR) {
        // remove '\0'
        ParcelPopBack(&self->parcel, STRING_END_CHAR_LENGTH);

        if (ParcelWriteInt8(&self->parcel, c)) {
            return ParcelWriteInt8(&self->parcel, (uint32_t)STRING_END_CHAR);
        }
    }

    return HC_FALSE;
}

/*
 * Assign a value to the HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: assign value of ta_sting.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringSet(HcString *self, HcString str)
{
    if (self != NULL) {
        DeleteParcel(&self->parcel);
        return StringAppend(self, str);
    }

    return HC_FALSE;
}

/*
 * Assign a value to the HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: assign value of string pointer.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringSetPointer(HcString *self, const char *str)
{
    if (self != NULL) {
        DeleteParcel(&self->parcel);
        return StringAppendPointer(self, str);
    }

    return HC_FALSE;
}

/*
 * Get the string pointer data
 * @param self: self pointer.
 * @return the pointer data of the string
 */
const char *StringGet(const HcString *self)
{
    if (self == NULL) {
        return NULL;
    }

    return GetParcelData(&self->parcel);
}

/*
 * Get the length of the string
 * @param self: self pointer.
 * @return the length of the string
 */
uint32_t StringLength(const HcString *self)
{
    if (self == NULL) {
        return 0;
    } else {
        uint32_t length = GetParcelDataSize(&self->parcel);
        if (length > 0) {
            return length - STRING_END_CHAR_LENGTH;
        } else {
            return 0;
        }
    }
}

/*
 * Create a string.
 * Notice: You should delete_string when you don't need the string anymore.
 * @return return the created string.
 */
HcString CreateString(void)
{
    HcString str;
    str.parcel = CreateParcel(0, STRING_ALLOC_SIZE);
    ParcelWriteInt8(&str.parcel, STRING_END_CHAR);
    return str;
}

/*
 * Delete a string. In fact it will not destroy the string,
 * but only free the allocate memory of the string and reset the member's value
 * of the string.
 * You can continue to use the string if you want.
 * Notice: You should delete the string when you don't need it any more to avoid memory leak.
 * @param str: The string you want to delete.
 */
void DeleteString(HcString *str)
{
    if (str != NULL) {
        DeleteParcel(&str->parcel);
    }
}
