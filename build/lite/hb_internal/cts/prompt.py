#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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

from __future__ import absolute_import
from __future__ import print_function

from prompt_toolkit.shortcuts import run_application

from hb_internal.cts.checkbox import question as checkbox_question
from hb_internal.cts.list import question as list_question
from hb_internal.common.utils import hb_error
from hb_internal.common.utils import hb_warning


def prompt(questions, answers=None, **kwargs):
    if isinstance(questions, dict):
        questions = [questions]
    answers = answers or {}

    patch_stdout = kwargs.pop('patch_stdout', False)
    return_asyncio_coroutine = kwargs.pop('return_asyncio_coroutine', False)
    true_color = kwargs.pop('true_color', False)
    refresh_interval = kwargs.pop('refresh_interval', 0)
    eventloop = kwargs.pop('eventloop', None)
    kbi_msg = kwargs.pop('keyboard_interrupt_msg', 'Cancelled by user')

    for question in questions:
        try:
            choices = question.get('choices')
            if choices is not None and callable(choices):
                question['choices'] = choices(answers)

            _kwargs = {}
            _kwargs.update(kwargs)
            _kwargs.update(question)
            question_type = _kwargs.pop('type')
            name = _kwargs.pop('name')
            message = _kwargs.pop('message')
            when = _kwargs.pop('when', None)
            question_filter = _kwargs.pop('filter', None)

            if when:
                # at least a little sanity check!
                if callable(question['when']):
                    try:
                        if not question['when'](answers):
                            continue
                    except Exception as error:
                        raise ValueError(
                            'Problem in \'when\' check of %s question: %s' %
                            (name, error))
                else:
                    raise ValueError('\'when\' needs to be function that '
                                     'accepts a dict argument')
            if question_filter:
                # at least a little sanity check!
                if not callable(question['filter']):
                    raise ValueError('\'filter\' needs to be function that '
                                     'accepts an argument')

            if callable(question.get('default')):
                _kwargs['default'] = question['default'](answers)

            if question_type == "checkbox":
                application = checkbox_question(message, **_kwargs)
            elif question_type == "list":
                application = list_question(message, **_kwargs)

            answer = run_application(
                application,
                patch_stdout=patch_stdout,
                return_asyncio_coroutine=return_asyncio_coroutine,
                true_color=true_color,
                refresh_interval=refresh_interval,
                eventloop=eventloop)

            if answer is not None:
                if question_filter:
                    try:
                        answer = question['filter'](answer)
                    except Exception as error:
                        raise ValueError('Problem processing \'filter\' of'
                                         '{} question: {}'.format(name, error))
                answers[name] = answer
        except AttributeError as attr_error:
            hb_error(attr_error)
            raise ValueError('No question type \'%s\'' % question_type)
        except KeyboardInterrupt:
            hb_warning('')
            hb_warning(kbi_msg)
            hb_warning('')
            return {}
    return answers
