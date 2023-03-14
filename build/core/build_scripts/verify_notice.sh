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
# Author:  Tools Team
# param description:
# $1 : the path of notice file to be verified.
# $2 : the file of writing verification results.
# $3 : the current platform dir.
# notice.

set -e

if [ ! -f "$1" ]; then
    echo "Note: The notice file $1 does not exist."
    echo "Success" > $2
    exit 0
fi

line_of_equal_in_file="$3/lineOfEqualInFile"
line_of_new_file_flag_in_file="$3/lineOfNewFileFlagInFile"
line_of_divide_in_file="$3/lineOfDivideInFile"

rm -f $line_of_equal_in_file
rm -f $line_of_new_file_flag_in_file
rm -f $line_of_divide_in_file

NOTICEFILE=$1

grep -n "^============================================================$" $NOTICEFILE | cut -d ':' -f 1 > $line_of_equal_in_file
grep -n "Notices for file(s):" $NOTICEFILE | cut -d ':' -f 1 > $line_of_new_file_flag_in_file
grep -n "^------------------------------------------------------------$" $NOTICEFILE | cut -d ':' -f 1 > $line_of_divide_in_file

nums_equal_in_file=$(cat $line_of_equal_in_file | wc -l)
nums_new_file_flag_in_file=$(cat $line_of_new_file_flag_in_file | wc -l)
nums_divid_in_file=$(cat $line_of_divide_in_file | wc -l)

if [[ "$nums_equal_in_file" != "$nums_new_file_flag_in_file" ]];then
    echo "Error:nums_equal_in_file is $nums_equal_in_file,nums_new_file_flag_in_file is $nums_new_file_flag_in_file"
    echo "Failed" > $2
    exit 1
elif [[ "$nums_equal_in_file" != "$nums_divid_in_file" ]];then
    echo "Warning!! maybe something wrong! nums_equal_in_file is $nums_equal_in_file,nums_divid_in_file is $nums_divid_in_file"
fi

rm -f $line_of_equal_in_file
rm -f $line_of_new_file_flag_in_file
rm -f $line_of_divide_in_file

echo "Success" > $2

set +e
