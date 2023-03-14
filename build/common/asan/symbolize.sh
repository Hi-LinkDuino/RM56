#!/bin/bash
# Copyright (c) 2022 Huawei Device Co., Ltd.
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

objdir=${SYSROOT:-.}
symbolizer=${SYMBOLIZER:-llvm-symbolizer}
help=no

while test -n "${1}"; do
    case "${1}" in
    -o | --objdir)
        objdir="${2}"
        shift
        shift
        ;;
    -p | --symbolizer)
        symbolizer="${2:-llvm-symbolizer}"
        shift
        shift
        ;;
    -h | --help)
        shift
        help=yes
        break
        ;;
    *)
        shift
        break
        ;;
    esac
done

print_help() {
    cat <<-END
Usage: symbolize.sh [OPTION]...
Translate call stack to symbolized forms.

    Options:

    -o,  --objdir  <objects_dir>    dir that contains the call stack binaries.
    -p,  --symbolizer <symbolizer>  symbolizer for translating call stacks, default is llvm-symbolizer.
    -h,  --help                     print help info
END
}

test $help = yes && print_help && exit 0

find_unstripped() {
    file="$(basename $1)"
    shift
    find "$@" -type f -name "$file" -exec sh -c  'file $1 | grep -q "not stripped"' sh {} \; -print | head -n 1
}

find_file() {
    if [ -d "$objdir/lib.unstripped" ]; then
        find_unstripped $1 "$objdir/lib.unstripped" "$objdir/exe.unstripped"
    else
        find_unstripped $1 "$objdir"
    fi
}

getsym2() {
    file="$(find_file $1)"
    if [ -f "$file" ]; then
        ${test} $symbolizer -C -f -p -e "${file}" $2 2>/dev/null
    fi
}

getsym() {
    if [ $# -gt 0 ]; then
        getsym2 $objdir${1//+/ }
    fi
}

while read -r line; do
    echo "$line" $(getsym $(echo "$line" | sed -n 's/.*(\(.*\)).*/\1/p'))
done
