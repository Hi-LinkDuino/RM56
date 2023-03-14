/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_FILE_H
#define GRAPHIC_LITE_FILE_H

#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#else
#include "stdint.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/mount.h"
#include "sys/stat.h"
#ifdef __APPLE__
#include "sys/param.h"
#else
#include "sys/statfs.h"
#endif
#include "dirent.h"
#include "stdio.h"
#include "errno.h"
#endif

#define DEFAULT_FILE_PERMISSION 0666 // default file permission when creat file
#endif // GRAPHIC_LITE_FILE_H
