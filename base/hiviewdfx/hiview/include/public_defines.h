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

#ifndef HIVIEW_PUBLIC_DEFINES_H
#define HIVIEW_PUBLIC_DEFINES_H

// define your message type here
#define PRIVATE_MESSAGE_TYPE \
        PRIVATE_MESSAGE_TYPE, \
        ENGINE_UPLOAD_READY_MSG, \
        ENGINE_REFRESH_UE_STATE_MSG, \
        ENGINE_UPDATE_XML_MSG, \
        ENGINE_SET_TEST_TYPE_MSG, \
        ENGINE_RUNNING_LOG_TIMER_MSG, \
        ENGINE_RUNNING_LOG_NOTIFY_MSG, \
        ENGINE_RUNNING_LOG_UNPACK_MSG, \
        ENGINE_NOTIFY_FLUSH_OLD_INFO_MSG, \
        ENGINE_EVENTINFO_LOGGER_TIMER_MSG, \
        ENGINE_DESERIALIZE_MSG, \
        ENGINE_TIMER_TASK_NOTIFY_MSG, \
        ENGINE_APP_USAGE_STATICS_MSG, \
        ENGINE_RAW_EVENT_MSG, \
        ENGINE_CHECK_NATIVE_MSG, \
        ENGINE_CAPTURE_LOG_MSG, \
        ENGINE_CAPTURE_REMOTE_LOG_FINISH, \
        ENGINE_DISTRIBUTED_CAPTURE_REMOTE_LOG_FINISH, \
        ENGINE_CAPTURE_BETACLUB_LOG_FINISH, \
        ENGINE_CAPTURE_TEST_LOG_FINISH, \
        ENGINE_CAPTURE_REPAIR_LOG_FINISH, \
        ENGINE_POST_EXT_REMOTE_DATA, \
        ENGINE_CLOSE_REMOTE_DEBUG, \
        ENGINE_ROLLBACK_USED_TRAFFIC, \
        ENGINE_CAR_UPDATE_CONFIG, \
        ENGINE_CAPTURE_CAR_LOG, \
        ENGINE_CAPTURE_CAR_LOG_FINISH, \
        ENGINE_SYSEVENT_DEBUG_MODE, \
        ENGINE_ROLLBACK_TRAFFIC_BY_INFO

// define your audit event type here
#define PRIVATE_AUDIT_EVENT_TYPE \
        PRIVATE_AUDIT_EVENT_TYPE = 1000,

#endif // HIVIEW_PUBLIC_DEFINES_H
