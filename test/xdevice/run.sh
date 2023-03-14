#!/bin/bash
#
# Copyright (C) 2020-2021 Huawei Device Co., Ltd.
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
#

set -e

error()
{
  echo "$1"
  exit 1
}
PYTHON="python3"
TOOLS="tools"

flag=$(command -v $PYTHON | grep -c $PYTHON)
if [ "$flag" -eq 0 ]; then
    error "Python3.7 or higher version required!"
fi

$PYTHON -c 'import sys; exit(1) if sys.version_info.major < 3 or sys.version_info.minor < 7 else exit(0)' || \
error "Python3.7 or higher version required!"
cd $(dirname "$0") || error "Failure to change directory!"
$PYTHON -c "import easy_install" || error "Please install setuptools first!"

if [ ! -d "$TOOLS" ]; then
  error "$TOOLS directory not exists"
fi

for f in "$TOOLS"/*.egg
do
  if [ ! -e "$f" ]; then
    error "Can not find xdevice package!"
  fi
  $PYTHON -m easy_install --user "$f" || echo "Error occurs to install $f!"
done

for f in "$TOOLS"/*.tar.gz
do
  if [ ! -e "$f" ]; then
    error "Can not find xdevice package!"
  fi
  $PYTHON -m easy_install --user "$f" || echo "Error occurs to install $f!"
done

$PYTHON -m xdevice "$@"
exit 0
