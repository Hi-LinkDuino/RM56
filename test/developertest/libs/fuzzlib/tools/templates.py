#!/usr/bin/env python
# -*- coding:utf-8 -*-
#
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
#

import os
import time

GN_ENTRY_TEMPLATE = """\

group("hydra_fuzz"){
  testonly = true
  if (use_libfuzzer) {
    deps = ["//test/fuzzing_test/projects"]
  }else{
    deps = []
  }
}
"""


PROJECT_GN_TEMPLATE = """\
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

#####################hydra-fuzz###################
import("//build/config/features.gni")
import("//build/test.gni")

##############################fuzztest##########################################
ohos_fuzztest("") {
  module_out_path = module_output_path

  include_dirs = [
  ]
  cflags = ["-g","-O0","-Wno-unused-variable","-fno-omit-frame-pointer"]
  sources = [
    "%(project_name)s.cpp",
  ]
}
###############################################################################
group("fuzztest") {
  testonly = true
  deps = []
  deps += [
    # deps file
    ":",
  ]
}
###############################################################################
"""


PROJECT_DEMO_TEMPLATE = """\
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "%(project_name)s.h"

#include <stddef.h>
#include <stdint.h>

const int FUZZ_DATA_LEN = 3;
const int FUZZ_FST_DATA = 0;
const int FUZZ_SND_DATA = 1;
const int FUZZ_TRD_DATA = 2;
const int FUZZ_FTH_DATA = 3;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        bool result = false;
        if (size >= FUZZ_DATA_LEN) {
            result = data[FUZZ_FST_DATA] == 'F' &&
                data[FUZZ_SND_DATA] == 'U' &&
                data[FUZZ_TRD_DATA] == 'Z' &&
                data[FUZZ_FTH_DATA] == 'Z';
        }
        return result;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

"""

PROJECT_HEADER_TEMPLATE = """\
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#define FUZZ_PROJECT_NAME "%(project_name)s"

"""


PROJECT_XML_TEMPLATE = """\
<?xml version="1.0" encoding="utf-8"?>
<!-- Copyright (c) 2021 Huawei Device Co., Ltd.

     Licensed under the Apache License, Version 2.0 (the "License");
     you may not use this file except in compliance with the License.
     You may obtain a copy of the License at

          http://www.apache.org/licenses/LICENSE-2.0

     Unless required by applicable law or agreed to in writing, software
     distributed under the License is distributed on an "AS IS" BASIS,
     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
     See the License for the specific language governing permissions and
     limitations under the License.
-->
<fuzz_config>
  <fuzztest>
    <!-- maximum length of a test input -->
    <max_len>1000</max_len>
    <!-- maximum total time in seconds to run the fuzzer -->
    <max_total_time>300</max_total_time>
    <!-- memory usage limit in Mb -->
    <rss_limit_mb>4096</rss_limit_mb>
  </fuzztest>
</fuzz_config>
"""


REPORT_CSS_TEMPLATE = """
"""


def render_tbody(data):
    res  = ""
    for row in data:
        row_line = "<tr>"
        for row_td in row:
            row_line = row_line \
                + "\n<th scope=\"col\">{}</td>\n".format(row_td)
        row_line = "%s %s " % (row_line, "</tr>")
        res = res + row_line
    return res


def render_common(data):
    return REPORT_COMMON_HTML_TEMPLATE % data


def get_format_bt(backtrace):
    new_backtrack = ""
    line_tag = ["#0", "#1", "#2", "#3", "#4", "#5", "#6", "#7"]
    block_file_list = [
        "sanitizer_common_interceptors.inc",
        "FuzzerDriver.cpp",
        "FuzzerLoop.cpp",
        "FuzzerMain.cpp",
        "??"
        ]

    tmp_flag = False
    for line in backtrace.split("\n"):
        tag_check = False
        for tag in line_tag:
            if line.strip().startswith(tag) == True \
                and "in" in line and "exec/s" not in line:
                tag_check = True
                break

        if tag_check:
            block_flag = False
            for block_line in block_file_list:
                if block_line in line:
                    block_flag = True
                    break
            if block_flag:
                new_backtrack = " %s %s \n" % (new_backtrack, line)
            else:

                end_line = '<b target="view_frame" \
                    style="color: red; font-size:16px;\
                        " > %s </b>\n' % line
                new_backtrack = " %s %s " % (new_backtrack, end_line)
        else:
            new_backtrack = " %s %s \n" % (new_backtrack, line)
    return new_backtrack