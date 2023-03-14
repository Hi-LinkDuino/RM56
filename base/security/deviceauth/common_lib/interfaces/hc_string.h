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

#ifndef HC_STRING_H
#define HC_STRING_H

#include "hc_parcel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HcString {
    HcParcel parcel; // parcel data, used to storage the string data
} HcString;

/*
 * Append a HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: append string.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppend(HcString *self, HcString str);

/*
 * Append string pointer
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: string pointer.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppendPointer(HcString *self, const char *str);

/*
 * Append a char
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: char.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringAppendChar(HcString *self, char c);

/*
 * Assign a value to the HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: assign value of ta_sting.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringSet(HcString *self, HcString str);

/*
 * Assign a value to the HcString
 * Notice: It will add '\0' automatically.
 * @param self: self pointer.
 * @param str: assign value of string pointer.
 * @return HC_TRUE (ok), HC_FALSE (error)
 */
HcBool StringSetPointer(HcString *self, const char *str);

/*
 * Get the string pointer data
 * @param self: self pointer.
 * @return the pointer data of the string
 */
const char* StringGet(const HcString *self);

/*
 * Get the length of the string
 * @param self: self pointer.
 * @return the length of the string
 */
uint32_t StringLength(const HcString *self);

/*
 * Create a string.
 * Notice: You should delete string when you don't need the string anymore.
 * @return the created string.
 */
HcString CreateString(void);

/*
 * Delete a string. In fact it will not destroy the string,
 * but only free the allocated memory of the string and reset the member's value
 * of the string. You can continue to use the string if you want.
 * Notice: You should delete the string when you don't need it any more to avoid memory leak.
 * @param str: The string you want to delete.
 */
void DeleteString(HcString *str);

#ifdef __cplusplus
}
#endif
#endif
