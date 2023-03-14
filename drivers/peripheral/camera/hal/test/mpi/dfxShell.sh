# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/bin/bash
#set -e
script_path=$(cd $(dirname $0);pwd)
cd ${script_path}
cd ../../../../../../out/ohos-arm-release/packages/phone/system/lib;
libcamera_client=$(stat -c %s "libcamera_client.z.so")
echo "libcamera_client.z.so(B): "$libcamera_client

camera_buffer_manager=$(stat -c %s "libcamera_buffer_manager.z.so")
echo "libcamera_buffer_manager.z.so(B): "$camera_buffer_manager

camera_device_manager=$(stat -c %s "libcamera_device_manager.z.so")
echo "libcamera_device_manager.z.so(B): "$camera_device_manager

camera_hdi_impl=$(stat -c %s "libcamera_hdi_impl.z.so")
echo "libcamera_hdi_impl.z.so(B): "$camera_hdi_impl

camera_pipeline_core=$(stat -c %s "libcamera_pipeline_core.z.so")
echo "libcamera_pipeline_core.z.so(B): "$camera_pipeline_core

camera_utils=$(stat -c %s "libcamera_utils.z.so")
echo "libcamera_utils.z.so(B): "$camera_utils

total=$[libcamera_client+camera_buffer_manager+camera_device_manager+camera_hdi_impl+camera_pipeline_core+camera_utils]

echo "so package total(KB)："$(expr $total / 1024)
