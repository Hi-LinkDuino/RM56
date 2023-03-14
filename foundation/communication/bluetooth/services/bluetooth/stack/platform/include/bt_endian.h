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

#ifndef BT_ENDIAN_H
#define BT_ENDIAN_H

#ifdef DARWIN_PLATFORM
#define H2BE_16(x) (x)
#define H2LE_16(x) (x)
#define BE2H_16(x) (x)
#define LE2H_16(x) (x)

#define H2BE_32(x) (x)
#define H2LE_32(x) (x)
#define BE2H_32(x) (x)
#define LE2H_32(x) (x)

#define H2BE_64(x) (x)
#define H2LE_64(x) (x)
#define BE2H_64(x) (x)
#define LE2H_64(x) (x)
#else
#include <endian.h>

#define H2BE_16(x) htobe16(x)
#define H2LE_16(x) htole16(x)
#define BE2H_16(x) be16toh(x)
#define LE2H_16(x) le16toh(x)

#define H2BE_32(x) htobe32(x)
#define H2LE_32(x) htole32(x)
#define BE2H_32(x) be32toh(x)
#define LE2H_32(x) le32toh(x)

#define H2BE_64(x) htobe64(x)
#define H2LE_64(x) htole64(x)
#define BE2H_64(x) be64toh(x)
#define LE2H_64(x) le64toh(x)
#endif

#endif  // BT_ENDIAN_H
