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
set +e

echo "++++++++++++++++++++++++++++++++++++++++"
date +%F' '%H:%M:%S
echo $@

function help() {
  echo
  echo "Usage:"
  echo "  ./build.sh --product-name {product-name} [options]"
  echo
  echo "Examples:"
  echo "  ./build.sh --product-name Hi3516DV300 --ccache"
  echo
  echo "options"
  echo "  --ccache          use ccache, default: false"
  echo "  --jobs N          run N jobs in parallel"
  echo "  --build-target    build target name"

  echo "  --gn-args         gn args"
  echo "  --export-para     export env"
  echo "  --help, -h        print help info"
  echo
  exit 1
}

export source_root_dir=$(cd $(dirname $0);pwd)

while [[ ! -f "${source_root_dir}/.gn" ]]; do
    source_root_dir="$(dirname "${source_root_dir}")"
    if [[ "${source_root_dir}" == "/" ]]; then
        echo "Cannot find source tree containing $(pwd)"
        exit 1
    fi
done

if [[ "${source_root_dir}x" == "x" ]]; then
  echo "Error: source_root_dir cannot be empty."
  exit 1
fi

case $(uname -s) in
    Darwin)
        HOST_DIR="darwin-x86"
        HOST_OS="mac"
        ;;
    Linux)
        HOST_DIR="linux-x86"
        HOST_OS="linux"
        ;;
    *)
        echo "Unsupported host platform: $(uname -s)"
        RET=1
        exit $RET
esac

export PATH=${source_root_dir}/prebuilts/build-tools/${HOST_DIR}/bin:$PATH
# set python3
PYTHON3=${source_root_dir}/prebuilts/python/${HOST_DIR}/3.8.5/bin/python3
if [[ ! -f "${PYTHON3}" ]]; then
  echo -e "\033[33m Please execute the build/prebuilts_download.sh \033[0m"
  exit 1
fi

${PYTHON3} ${source_root_dir}/build/scripts/tools_checker.py

${PYTHON3} ${source_root_dir}/build/scripts/entry.py --source-root-dir ${source_root_dir} $@

if [[ "$?" -ne 0 ]]; then
    echo -e "\033[31m=====build ${product_name} error=====\033[0m"
    exit 1
fi
echo -e "\033[32m=====build ${product_name} successful=====\033[0m"

date +%F' '%H:%M:%S
echo "++++++++++++++++++++++++++++++++++++++++"
