/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_HAL_COMMON_FUNC_H
#define OHOS_WIFI_HAL_COMMON_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description copy max len - 1 characters to destination
 *
 * @param dst - copy to
 * @param len - dst's size
 * @param src - copy from
 */
void StrSafeCopy(char *dst, unsigned len, const char *src);

/**
 * @Description Convert [a,b,c,d,e,f] mac address to string type [xx:xx:xx:xx:xx:xx]
 *
 * @param mac - mac address
 * @param macSize - mac size, must be equal to 6
 * @param macStr - output mac string, type: [xx:xx:xx:xx:xx:xx]
 * @param strLen - mac string len, must bigger than 17
 * @return int - convert result. 0 - Failed 1 - Success
 */
int ConvertMacToStr(const unsigned char *mac, int macSize, char *macStr, int strLen);

/**
 * @Description Convert mac string type [xx:xx:xx:xx:xx:xx] to array type
 *
 * @param macStr - input mac address, string type like xx:xx:xx:xx:xx:xx
 * @param mac - output mac array
 * @param macSize - mac array length, must be equal to 6
 * @return int - convert result. 0 - Failed 1 - Success
 */
int ConvertMacToArray(const char *macStr, unsigned char *mac, int macSize);

/**
 * @Description Judge input is valid mac string
 *
 * @param macStr - input mac string
 * @return int - -1 - invalid 0 valid
 */
int CheckMacIsValid(const char *macStr);

#ifdef __cplusplus
}
#endif
#endif