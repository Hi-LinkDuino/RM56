#!/bin/bash
#Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
set -e

# step 1: clone jerryscript repo
git clone git@gitee.com:openharmony/third_party_jerryscript.git jerryscript

# step 2: reset the commit to the 2.1.0 tag
pushd ./jerryscript
git reset --hard e8bc7a2b93a6edfa463458c8bb69fac2a36feb9e

# step 3: compile jerry
python tools/build.py --mem-heap=48 --snapshot-exec=ON --snapshot-save=ON \
        --profile=es5.1 --error-messages=ON --logging=ON --mem-stats=ON \
        --jerry-cmdline-snapshot=ON

# step 4: copy jerry-snapshot excutable
popd
mkdir linux
cp ./jerryscript/build/bin/jerry ./linux/
cp ./jerryscript/build/bin/jerry-snapshot ./linux/

echo "snapshot tool generated done."
echo "useage: jerry-snapshot generate -o xxx.bc xxx.js"
