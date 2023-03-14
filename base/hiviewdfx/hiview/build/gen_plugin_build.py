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
import json
import os
import sys

from collections import OrderedDict
from os.path import join
from os.path import realpath


DEBUG = False


def read_json_file(input_file):
    if not os.path.exists(input_file):
        if DEBUG:
            print("file '{}' doesn't exist.".format(input_file))
        return None

    data = None
    with open(input_file, 'r') as input_f:
        data = json.load(input_f, object_pairs_hook=OrderedDict)
    return data


def write_json_file(output_file, content):
    file_dir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(file_dir):
        os.makedirs(file_dir, exist_ok=True)
    with open(output_file, 'w') as output_f:
        json.dump(content, output_f)


def get_gn_build_content(plugins, plugin_so):
    so_items = []
    items = []
    build_path = os.path.split(realpath(__file__))[0]
    root_path = realpath(join(build_path, ".."))
    ut_items = []
    mst_items = []
    for plugin_info in plugins:
        for plugin, info in plugin_info.items():
            gn_path = realpath(join(root_path, info['path'], "BUILD.gn"))
            if not os.path.exists(gn_path):
                print('%s %s' % (gn_path, 'is not exist.'))
                break
            if 'loadType' in info:
                if info['loadType'] == 'dynamic':
                    target = 'lib%s' % (plugin.lower())
                    so_items.append('\"%s:%s\",\n' % (info['path'], target))
            else:
                items.append('\"%s:%s\",\n' % (info['path'], info['name']))
            ut_items.append('\"%s:unittest\",\n' % (info['path']))
            mst_items.append('\"%s:moduletest\",\n' % (info['path']))
    dynamic_info = 'plugin_dynamic_deps = [\n%s\n]\n' % (''.join(so_items))
    static_info = ('plugin_static_deps = [\n%s\n]\n' % (''.join(items)))
    ut_deps = ('plugin_ut_deps = [\n%s\n]\n' % (''.join(list(set(ut_items)))))
    mst_deps = ('plugin_mst_deps = [\n%s\n]' % (''.join(list(set(mst_items)))))
    return '%s%s%s%s' % (dynamic_info, static_info, ut_deps, mst_deps)


def write_build_file(output_file, plugins, plugin_so, cnt_type):
    build_info = ""
    if cnt_type == "gn":
        build_info = get_gn_build_content(plugins, plugin_so)
    else:
        return
    out_dir = os.path.dirname(output_file)
    if os.path.isdir(out_dir) is False:
        os.makedirs(out_dir)
    with open(output_file, 'w') as file_id:
        if DEBUG:
            print(build_info)
        file_id.write(build_info)


def write_config_file(output_file, plugins, pipelines, pipelinegroups, is_so):
    items = []
    # plugins
    items.append('plugins:%d \n' % (len(plugins)))
    for plugin_info in plugins:
        for plugin, info in plugin_info.items():
            items.append('%s[' % (plugin))
            if "threadtype" in info:
                items.append('%s:%s' %
                      (info['threadtype'], info['threadname']))
                if info['threadtype'] == "pool":
                    items.append(':%d' % (info['threadnumber']))
            loadtime = 0
            if "loadtime" in info:
                loadtime = info['loadtime']
            load_type = "static"
            if "loadType" in info:
                load_type = info['loadType']
            items.append(']:%d %s\n' % (loadtime, load_type))
    if len(pipelines) > 0:
        items.append('pipelines:%d\n' % (len(pipelines)))
        for pipeline, plugin_list in pipelines.items():
            items.append('%s:' % (pipeline))
            for plugin in plugin_list:
                items.append('%s ' % (plugin))
            items.append('\n')
    if len(pipelinegroups) > 0:
        items.append('pipelinegroups:%d\n' % (len(pipelinegroups)))
        for pipelinegroup in pipelinegroups:
            for plugin, pipeline_list in pipelinegroup.items():
                items.append('%s:' % (plugin))
                for pipeline in pipeline_list:
                    items.append('%s ' % (pipeline))
            items.append('\n')
    config_info = ''.join(items)
    out_dir = os.path.dirname(output_file)
    if os.path.isdir(out_dir) is False:
        os.makedirs(out_dir)
    with open(output_file, 'w') as file_id:
        file_id.write(config_info)


def check_plugin_config(config_data):
    if "plugins" not in config_data:
        return False
    if "rules" not in config_data:
        return False
    return True


class ArgsPara():
    def __init__(self, target_os, product, platform, arch, ram, rom):
        self.target_os = target_os
        self.product = product
        self.platform = platform
        self.arch = arch
        self.ram = self.get_memory_size(ram)
        self.rom = self.get_memory_size(rom)


    def get_memory_size(self, memory_detail):
        # get memory
        if isinstance(memory_detail, int) is True:
            return memory_detail
        if memory_detail.isdigit() is True:
            return int(memory_detail)
        size_str = memory_detail[0:-1]
        size_unit = memory_detail[-1].lower()
        if size_str.isdigit() is False:
            return -1
        size_int = int(size_str)
        if  size_unit == "k":
            return 1024*size_int
        if size_unit == "m":
            return 1024*1024*size_int
        if size_unit == "g":
            return 1024*1024*1024*size_int
        return -1


    def is_less(self, args_para):
        if self.target_os != "double"and args_para.target_os != "double":
            if self.target_os != args_para.target_os:
                return False
        if self.product != args_para.product:
            return False
        if self.platform != args_para.platform:
            return False
        if self.arch != args_para.arch:
            return False
        if self.ram > args_para.ram:
            return False
        if self.rom > args_para.rom:
            return False
        return True


    def set_para(self, name, value):
        if name == "target_os":
            self.target_os = value
        if name == "product":
            self.product = value
        if name == "platform":
            self.platform = value
        if name == "arch":
            self.arch = value
        if name == "ram":
            self.ram = self.get_memory_size(value)
        if name == "rom":
            self.rom = self.get_memory_size(value)
        return ""


def is_build_rule(rule_list, args_para):
    args_list = ["target_os", "product", "platform", "arch", "ram", "rom"]
    for rule in rule_list:
        config_args = ArgsPara(args_para.target_os,
                               args_para.product,
                               args_para.platform,
                               args_para.arch,
                               args_para.ram,
                               args_para.rom)
        for arg in args_list:
            if arg in rule:
                config_args.set_para(arg, rule[arg])
        if config_args.is_less(args_para) is True:
            return True
    return False


def add_plugin(plugins, loadorder, threads, config_plugins):
    thread_list = {}
    for name, value in threads.items():
        if name == "singledthread":
            for plugin, thread_name in value.items():
                if plugin not in config_plugins:
                    raise Exception("[{}] not exist in plugins".format(plugin))
                thread_list.update({plugin: {'threadtype': 'thread',
                                            'threadname': thread_name}})
        if name == "sharedthread":
            for thread_name, plugin_list in value.items():
                for plugin in plugin_list:
                    if plugin not in config_plugins:
                        raise Exception("[{}] not in plugins".format(plugin))
                    thread_list.update({plugin: {'threadtype': 'thread',
                                                'threadname': thread_name}})
        if name == "threadpool":
            number = 4
            if "number" in value:
                number = value['number']
            if "poolinfo" not in value:
                raise Exception("[{}] not exist in threadpool".format(value))
            poolinfo = value['poolinfo']
            threadpool_name = "pool"
            if "poolname" in poolinfo:
                threadpool_name = poolinfo['poolname']
            if "plugins" not in poolinfo:
                raise Exception("[{}] not exist in poolinfo".format(poolinfo))

            for plugin in poolinfo['plugins']:
                if plugin not in config_plugins:
                    raise Exception("[{}] notexist in plugins".format(plugin))
                thread_list.update({plugin: {'threadtype': 'pool',
                                            'threadname': threadpool_name,
                                            'threadnumber': number}})
    for plugin, value in loadorder.items():
        plugin_info = {}
        if plugin not in config_plugins:
            raise Exception("[{}] not exist in plugins".format(plugin))
        plugin_info.update(value)
        plugin_info.update(config_plugins[plugin])
        if plugin in thread_list:
            plugin_info.update(thread_list[plugin])
        plugins.append({plugin: plugin_info})


def add_pipelines(pipelines, config_pipelines, config_plugins):
    for name, value in config_pipelines.items():
        for plugin in value:
            if plugin not in config_plugins:
                raise Exception("[{}]  not exist in pipeline".format(value))
        pipelines.update({name: value})


def add_pipeline_groups(pipelinegrps, cfg_pipelinegrps, pipelines, plugins):
    for plugin, pipeline_list in cfg_pipelinegrps.items():
        found_plugin = False
        for item in plugins:
            if plugin in item:
                found_plugin = True
                break
        if found_plugin is False:
            raise Exception("[{}] not exist in plugins".format(plugin))
        for pipeline in pipeline_list:
            if pipeline not in pipelines:
                raise Exception("[{}] not exist in pipeline".format(pipelines))
        pipelinegrps.append({plugin: pipeline_list})


def generate_plugin(input_file, output_build_file, output_config_file,
                    plugin_so, build_file_type, args_para):
    data = read_json_file(input_file)
    if data is None:
        raise Exception("file [{}] does not exist.".format(input_file))
    #parse plugins
    #parse thread
    if DEBUG:
        print(data)
    check_rslt = check_plugin_config(data)
    if check_rslt is not True:
        raise Exception("file [{}] content is error".format(input_file))
    plugins = []
    pipelines = {}
    pipelinegroups = []
    config_plugins = data['plugins']
    for rule_item in data['rules']:
        if 'rule' not in rule_item:
            raise Exception("[{}] invalid, no rule item.".format(rule_item))
        if 'info' not in rule_item:
            raise Exception("[{}] invalid, no info item.".format(rule_item))
        rule_list = rule_item['rule']
        check_rslt = is_build_rule(rule_list, args_para)
        if check_rslt is False:
            continue
        info = rule_item['info']
        if 'loadorder' not in info:
            raise Exception("[{}] invalid, no loadorder item.".format(info))
        threads = {}
        if 'threads' in info:
            threads = info['threads']
        add_plugin(plugins, info['loadorder'], threads, config_plugins)
        if 'pipelines' in info:
            add_pipelines(pipelines, info['pipelines'], config_plugins)
        if 'pipelinegroups' in info:
            add_pipeline_groups(pipelinegroups, info['pipelinegroups'],
                                pipelines, plugins)
        break
    if DEBUG:
        print("xxxxxxxxx plugins xxxxxxxxxxxx")
        print(plugins)
        print("xxxxxxxxx pipelines xxxxxxxxxxxx")
        print(pipelines)
        print("xxxxxxxxx pipelinegroups xxxxxxxxxxxx")
        print(pipelinegroups)
    write_config_file(output_config_file, plugins, pipelines,
                      pipelinegroups, plugin_so)
    write_build_file(output_build_file, plugins, plugin_so, build_file_type)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-file', help='plugin build config json',
                        required=True)
    parser.add_argument('--plugin-config-file', help='plugin config json file',
                        required=True)
    parser.add_argument('--plugin-build-file', help='plugin config json file',
                        required=True)
    parser.add_argument('--target_os', help='target os', required=True)
    parser.add_argument('--double_framework', help='double os framework',
                        required=True)
    parser.add_argument('--target_platform', help='product type',
                        required=True)
    parser.add_argument('--target_cpu', help='arm or arm64', required=True)
    parser.add_argument('--plugin_so', help='build library', required=True)
    parser.add_argument('--plugin_target_platform', help='hisi,qcom, mtk,etc.',
                        required=True)
    parser.add_argument('--plugin_target_ram', help='ram', required=True)
    parser.add_argument('--plugin_target_rom', help='rom', required=True)
    parser.add_argument('--build-file-type', help='build file type',
                        required=False, default='gn')
    args = parser.parse_args()
    target_os = args.target_os

    if args.double_framework == "true":
        target_os = "double"
    plugin_so = False
    if args.plugin_so == "true":
        plugin_so = True
    args_para = ArgsPara(target_os,
                         args.target_platform,
                         args.plugin_target_platform,
                         args.target_cpu,
                         args.plugin_target_ram,
                         args.plugin_target_rom)
    if args_para.rom == -1 or args_para.ram == -1:
        raise Exception("input ram=%s,rom=%s invalid".
                  format(args.plugin_target_ram, args.plugin_target_rom))
    generate_plugin(args.input_file, args.plugin_build_file,
                    args.plugin_config_file, plugin_so,
                    args.build_file_type, args_para)
    return 0


if __name__ == '__main__':
    sys.exit(main())
