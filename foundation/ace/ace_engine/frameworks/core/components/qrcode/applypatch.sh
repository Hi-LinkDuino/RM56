#!/bin/bash
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

set -e
if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ]; then
    exit 1
fi

if [ -d "$2" ]; then
    rm -rf "$2"
fi

PATCH_FILE=$(cd $(dirname $3) && pwd)"/"$(basename $3)

cp -r "$1" "$2"
if [ $? -ne 0 ]; then
    exit 1
fi

patch -p 2 -i $PATCH_FILE -d "$2" -N