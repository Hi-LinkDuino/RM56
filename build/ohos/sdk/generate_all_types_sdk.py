#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

import argparse
import sys
import os

sys.path.append(
    os.path.dirname(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from scripts.util.file_utils import write_file, read_json_file  # noqa: E402

_SOURCE_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..'))
# Import jinja2 from third_party/jinja2
sys.path.insert(1, os.path.join(_SOURCE_ROOT, 'third_party'))
from jinja2 import Template  # noqa: E402  # pylint: disable=F0401


def read_display_name(type2displayname):
    return read_json_file(type2displayname)


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--sdk-systems', action='append')
    parser.add_argument('--sdk-modules')
    parser.add_argument('--sdk-types', action='append')
    parser.add_argument('--current-dir')
    parser.add_argument('--generated-sdk-modules-gni')
    parser.add_argument('--type2displayname')
    parser.add_argument('--api-version')
    parser.add_argument('--release-type')
    parser.add_argument('--meta-version')

    parser.add_argument('--output', required=True)

    options = parser.parse_args()

    template = Template("""#Generated code, DONOT modify it.
import("//build/ohos/build_var.gni")
import("//build/ohos/sdk/sdk.gni")
import("${build_configs_path}/platforms_list.gni")
import("{{ generated_sdk_modules_gni }}")

{% for sdk_type in sdk_types %}
  {% set _sdk_type = sdk_type.replace('-', '_') %}
  if (defined({{ ohos_sdk_modules }}.{{ _sdk_type }})) {
    {{ _sdk_type }}s = {{ ohos_sdk_modules }}. {{ _sdk_type }}
    group("{{ _sdk_type }}") {
      public_deps = []
        {% for os in sdk_systems %}
        public_deps += [":{{ _sdk_type }}_{{ os }}"]
        {% endfor %}
    }

    {% for os in sdk_systems %}
    make_{{ os }}_sdk_modules("{{ _sdk_type }}_{{ os }}") {
      sdk_type = "{{ sdk_type }}"
      sdk_modules = {{ _sdk_type }}s.{{ os }}
      {% if release_type != "" %}
      zipfile_name =
          "${sdk_type}-${{ "{" }}sdk_system_{{ os }}{{ "}" }}-${current_sdk_version}-${release_type}.zip"
      {% else %}
      zipfile_name =
          "${sdk_type}-${{ "{" }}sdk_system_{{ os }}{{ "}" }}-${current_sdk_version}.zip"
      {% endif %}
    }
    {% endfor %}
  }
{% endfor %}

foreach(os, sdk_systems) {
  {% for sdk_type in sdk_types %}
    {% if display_name.get(sdk_type) %}
        {% set _display_name = display_name.get(sdk_type) %}
    {% else %}
        {% set _display_name = sdk_type.capitalize() %}
    {% endif %}
    package_info_file =
        "$ohos_sdk_copy_dir/$os/{{ sdk_type }}/oh-uni-package.json"
    package_info = {}
    package_info = {
      path = "{{ sdk_type }}"
      displayName = "{{ _display_name }}"
      version = current_sdk_version
      apiVersion = "{{ api_version }}"

      {% if release_type != "" %}
      releaseType = "{{ release_type }}"
      {% endif %}

      {% if meta_version != "" %}
      meta = {
        metaVersion = "{{ meta_version }}"
      }
      {% endif %}
    }
    write_file(package_info_file, package_info, "json")
  {% endfor %}
}

group("generated_ohos_sdk") {
  public_deps = []
  {% for sdk_type in sdk_types %}
    {% set _sdk_type = sdk_type.replace('-', '_') %}
    if (defined({{ ohos_sdk_modules }}.{{ _sdk_type }})) {
      public_deps += [ ":{{ _sdk_type }}" ]
    }
  {% endfor %}
  public_deps += {{ ohos_sdk_modules }}.extras
}""",  # noqa E501
                        trim_blocks=True,
                        lstrip_blocks=True)

    contents = template.render(
        ohos_sdk_modules=options.sdk_modules,
        sdk_types=options.sdk_types,
        current_dir=options.current_dir,
        sdk_systems=options.sdk_systems,
        display_name=read_display_name(options.type2displayname),
        api_version=options.api_version,
        release_type=options.release_type,
        meta_version=options.meta_version,
        generated_sdk_modules_gni=options.generated_sdk_modules_gni)
    write_file(options.output, contents)


if __name__ == '__main__':
    sys.exit(main())
