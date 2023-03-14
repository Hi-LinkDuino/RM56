/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FUZZ_BASE_H
#define FUZZ_BASE_H

#include "camera.h"
#include "types.h"
#include "idevice_manager.h"
#include "camera_metadata_info.h"
#include "ibuffer.h"
#include "securec.h"
#include "camera_host.h"
#include "camera_host_impl.h"
#include "camera_device.h"
#include "camera_device_impl.h"
#include "stream_operator.h"
#include "offline_stream_operator.h"
#include "camera_host_proxy.h"
#include "camera_host_callback.h"
#include "camera_device_callback.h"
#include "stream_operator_callback.h"
#include "camera_device_stub.h"
#include "camera_host_stub.h"
#include "stream_operator_stub.h"
#include "offline_stream_operator_stub.h"
#include "camera_device_proxy.h"
#include "utils.h"
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <climits>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <thread>
#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include <map>
#include <surface.h>
#include <display_type.h>
#include <iservice_registry.h>
#include <iremote_proxy.h>
#include <hdf_log.h>
#include <osal_mem.h>

#endif // FUZZ_BASE_H