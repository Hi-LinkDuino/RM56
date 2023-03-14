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
#include "audit_log_parser_test.h"

#include "audit.h"
#include "audit_log_parser.h"
#include "pipeline.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void AuditLogParserTest::SetUp()
{
    Audit::GetInstance().Init(true);
    Audit::GetInstance().Clear();
}

/**
 * @tc.name: AuditLogParserTest001
 * @tc.desc: write normal event log into audit log and then parse it
 * @tc.type: FUNC
 * @tc.require: AR000DPTT2
 */
HWTEST_F(AuditLogParserTest, AuditLogParserTest001, TestSize.Level3)
{
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, 1585730002219098,
                           "EventProcessorExample4|EventProcessorExample3|example-2@7166|10000");
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_OUT, 1585730002219098);
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, 1585730009919825,
                           "EventProcessorExample3|EventProcessorExample4|example-2@7166|10001");
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_OUT, 1585730009919825);
    AuditLogParser parser;
    parser.StartParse();
    auto threadEventList = parser.GetThreadSummary();
    ASSERT_EQ(threadEventList.size(), 2ul);
    auto pluginEventList = parser.GetPluginSummary();
    ASSERT_EQ(pluginEventList.size(), 2ul);
    auto pipelineEventList = parser.GetPipelineSummary();
    ASSERT_EQ(pipelineEventList.size(), 0ul);
    auto scope = parser.GetAuditLogTimeScope();
    ASSERT_GT(scope.length(), 10ul);
}

/**
 * @tc.name: AuditLogParserTest002
 * @tc.desc: write pipeline event log into audit log and then parse it
 * @tc.type: FUNC
 * @tc.require: AR000DPTT2
 */
HWTEST_F(AuditLogParserTest, AuditLogParserTest002, TestSize.Level3)
{
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_CREATE, 1585730002219098, "EventSourceExample|901000000");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219098, "EventProcessorExample1");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219098, "example-2@7166");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219098, "EventProcessorExample2");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219098, "example-2@7167");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219098, "EventProcessorExample3");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219098, "example-2@7168");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_DONE, 1585730002219098);
    AuditLogParser parser;
    parser.StartParse();
    auto threadEventList = parser.GetThreadSummary();
    ASSERT_EQ(threadEventList.size(), 3ul);
    for (auto& log : threadEventList) {
        printf("thread log:%s \n", log.c_str());
    }
    auto pluginEventList = parser.GetPluginSummary();
    for (auto& log : pluginEventList) {
        printf("plugin log:%s \n", log.c_str());
    }
    ASSERT_EQ(pluginEventList.size(), 3ul);
    auto pipelineEventList = parser.GetPipelineSummary();
    for (auto& log : pipelineEventList) {
        printf("pipeline log:%s \n", log.c_str());
    }
    ASSERT_EQ(pipelineEventList.size(), 1ul);
    auto scope = parser.GetAuditLogTimeScope();
    ASSERT_GT(scope.length(), 10ul);
}

/**
 * @tc.name: AuditLogParserTest003
 * @tc.desc: write mixed event log into audit log and then parse it
 * @tc.type: FUNC
 * @tc.require: AR000DPTT2
 */
HWTEST_F(AuditLogParserTest, AuditLogParserTest003, TestSize.Level3)
{
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, 1585730002219050,
                           "EventProcessorExample4|EventProcessorExample3|example-2@7166|10000");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_CREATE, 1585730002219099, "EventSourceExample|901000000");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219099, "EventProcessorExample1");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219099, "example-2@7166");
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_OUT, 1585730002219050);
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219099, "EventProcessorExample2");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219099, "example-2@7167");
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_IN, 1585730002219070,
                           "EventProcessorExample3|EventProcessorExample4|example-2@7166|10001");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN, 1585730002219099, "EventProcessorExample3");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT, 1585730002219099, "example-2@7168");
    Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_DONE, 1585730002219099);
    Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_OUT, 1585730002219070);
    AuditLogParser parser;
    parser.StartParse();
    auto threadEventList = parser.GetThreadSummary();
    ASSERT_EQ(threadEventList.size(), 5ul);
    for (auto& log : threadEventList) {
        printf("thread log:%s \n", log.c_str());
    }
    auto pluginEventList = parser.GetPluginSummary();
    for (auto& log : pluginEventList) {
        printf("plugin log:%s \n", log.c_str());
    }
    ASSERT_EQ(pluginEventList.size(), 5ul);
    auto pipelineEventList = parser.GetPipelineSummary();
    for (auto& log : pipelineEventList) {
        printf("pipeline log:%s \n", log.c_str());
    }
    ASSERT_EQ(pipelineEventList.size(), 1ul);
    auto scope = parser.GetAuditLogTimeScope();
    ASSERT_GT(scope.length(), 10ul);
}

/**
 * @tc.name: AuditLogParserTest004
 * @tc.desc: construct a pipeline event with a normal event info
 * @tc.type: FUNC
 * @tc.require: AR000DPTT2
 */
HWTEST_F(AuditLogParserTest, AuditLogParserTest004, TestSize.Level3)
{
    AuditLogParser* parser = new AuditLogParser();
    ASSERT_NE(parser, nullptr);
    AuditLogParser::EventInfo info;
    AuditLogParser::PipelineEventInfo pipeInfo;
    pipeInfo.eventSerialId = 1;
    ASSERT_EQ(1ul, pipeInfo.eventSerialId);
    info.eventSerialId = 1;
    AuditLogParser::PipelineEventInfo pipeInfo2(info);
    ASSERT_EQ(info.eventSerialId, pipeInfo2.eventSerialId);
    delete parser;
}