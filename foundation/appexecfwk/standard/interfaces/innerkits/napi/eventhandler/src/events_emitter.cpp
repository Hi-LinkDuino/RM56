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
#include "hilog/log.h"
#include "events_emitter.h"

#include <uv.h>

#define DEFINE_HILOG_LABEL(name) \
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, LOG_DOMAIN, name }

#define HILOGE(...) OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, ##__VA_ARGS__)
#define HILOGW(...) OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, ##__VA_ARGS__)
#define HILOGI(...) OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, ##__VA_ARGS__)
#define HILOGD(...) OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, ##__VA_ARGS__)

DEFINE_HILOG_LABEL("events_emitter");
using namespace std;
namespace OHOS {
namespace AppExecFwk {
    static map<uint32_t, CallbackInfos> emitterInstances;
    std::shared_ptr<EventHandlerInstance> eventHandler;
    EventHandlerInstance::EventHandlerInstance(const std::shared_ptr<EventRunner>& runner): EventHandler(runner)
    {
        HILOGI("EventHandlerInstance::EventHandlerInstance constructed");
    }
    EventHandlerInstance::~EventHandlerInstance()
    {
        HILOGI("EventHandlerInstance::EventHandlerInstance de-constructed");
    }
    std::shared_ptr<EventHandlerInstance> EventHandlerInstance::GetInstance()
    {
        static auto runner = EventRunner::Create("events_emitter");
        if (runner.get() == nullptr) {
            HILOGE("failed to create EventRunner events_emitter");
            return nullptr;
        }
        static auto instance = std::make_shared<EventHandlerInstance>(runner);
        return instance;
    }

    void TransToEventData(napi_env env, EventData eventData, napi_value resultData)
    {
        napi_value data = nullptr;
        napi_create_object(env, &data);
        for (map<string, Val>::iterator it = eventData.data.begin(); it != eventData.data.end(); it++) {
            string key = it->first;
            Val val = it->second;
            napi_value napiValue = nullptr;
            switch (val.type) {
                case DataType::BOOL:
                    HILOGD("ProcessEvent key:%{public}s value:%{public}d", key.c_str(), val.value.bValue);
                    napi_get_boolean(env, val.value.bValue, &napiValue);
                    break;
                case DataType::INT:
                    HILOGD("ProcessEvent key:%{public}s value:%{public}d", key.c_str(), val.value.nValue);
                    napi_create_int32(env, val.value.nValue, &napiValue);
                    break;
                case DataType::STRING:
                    HILOGD("ProcessEvent key:%{public}s value:%{public}s", key.c_str(), val.value.cValue);
                    napi_create_string_utf8(env, val.value.cValue, NAPI_AUTO_LENGTH, &napiValue);
                    break;
                default:
                    HILOGE("ProcessEvent unsupport type data");
                    break;
            }
            napi_set_named_property(env, data, key.c_str(), napiValue);
        }
        napi_set_named_property(env, resultData, "data", data);
    }

    void ProcessCallback(const EventDataWorker* eventDataInner)
    {
        if (eventDataInner == nullptr) {
            HILOGW("EventDataWorkder instance(uv_work_t) is nullptr");
            return;
        }
        AsyncCallbackInfo* callbackInner = eventDataInner->callbackInfo;
        HILOGI("Prepare to process callbackInner %{public}p", callbackInner);
        if (callbackInner->isDeleted) {
            HILOGI("ProcessEvent isDeleted callbackInfo = %{public}p", callbackInner);
            napi_delete_reference(callbackInner->env, callbackInner->callback);
        } else {
            napi_value resultData = nullptr;
            napi_create_object(callbackInner->env, &resultData);
            TransToEventData(callbackInner->env, eventDataInner->data, resultData);

            napi_value callback = nullptr;
            napi_value returnVal = nullptr;
            napi_get_reference_value(callbackInner->env, callbackInner->callback, &callback);
            napi_call_function(callbackInner->env, nullptr, callback, 1, &resultData, &returnVal);
            if (callbackInner->once) {
                HILOGI("ProcessEvent delete once callback callbackInfo = %{public}p", callbackInner);
                napi_delete_reference(callbackInner->env, callbackInner->callback);
            }
        }
    }

    void EventHandlerInstance::ProcessEvent([[maybe_unused]] const InnerEvent::Pointer& event)
    {
        uint32_t eventId = event->GetInnerEventId();
        HILOGI("ProcessEvent, eventId = %{public}d", eventId);
        auto subscribe = emitterInstances.find(eventId);
        if (subscribe == emitterInstances.end()) {
            HILOGW("ProcessEvent has no callback");
            return;
        }

        CallbackInfos& callbackInfos = subscribe->second;
        HILOGI("ProcessEvent, size = %{public}zu", callbackInfos.asyncCallbackInfo.size());
        EventData eventData = *(event->GetUniqueObject<EventData>());
        for (auto iter = callbackInfos.asyncCallbackInfo.begin(); iter != callbackInfos.asyncCallbackInfo.end();) {
            AsyncCallbackInfo* callbackInfo = *iter;
            HILOGI("Prepare to process callbackInfo %{public}p", callbackInfo);
            EventDataWorker* eventDataWorker = new EventDataWorker();
            eventDataWorker->data = eventData;
            eventDataWorker->callbackInfo = callbackInfo;

            uv_loop_s *loop = nullptr;
            napi_get_uv_event_loop(callbackInfo->env, &loop);
            uv_work_t *work = new uv_work_t;
            if (work == nullptr) {
                HILOGI("uv_work_t instance is nullptr");
                delete eventDataWorker;
                eventDataWorker = nullptr;
                return;
            }
            work->data = (void *)eventDataWorker;
            uv_queue_work(loop, work, [](uv_work_t *work) {},
                [](uv_work_t *work, int status) {
                EventDataWorker* eventDataInner = (EventDataWorker*)work->data;
                ProcessCallback(eventDataInner);
                delete eventDataInner;
                eventDataInner = nullptr;
                delete work;
                work = nullptr;
            });
            if (callbackInfo->once || callbackInfo->isDeleted) {
                HILOGI("ProcessEvent once callback or isDeleted callback");
                iter = callbackInfos.asyncCallbackInfo.erase(iter);
                if (callbackInfos.asyncCallbackInfo.begin() == callbackInfos.asyncCallbackInfo.end()) {
                    emitterInstances.erase(eventId);
                    HILOGI("ProcessEvent delete the last callback");
                    break;
                }
            } else {
                ++iter;
            }
        }
        HILOGI("ProcessEvent end");
    }

    bool IsExistSameCallback(napi_env env, uint32_t eventIdValue, napi_value argv, bool once)
    {
        auto subscribe = emitterInstances.find(eventIdValue);
        if (subscribe == emitterInstances.end()) {
            return false;
        }
        vector<AsyncCallbackInfo *> callbackInfo = subscribe->second.asyncCallbackInfo;
        size_t callbackSize = callbackInfo.size();
        napi_value callback = nullptr;
        for (size_t i = 0; i < callbackSize; i++) {
            if (callbackInfo[i]->isDeleted) {
                continue;
            }
            if (callbackInfo[i]->env != env) {
                continue;
            }
            napi_get_reference_value(callbackInfo[i]->env, callbackInfo[i]->callback, &callback);
            bool isEq = false;
            napi_strict_equals(env, argv, callback, &isEq);
            if (!isEq) {
                continue;
            }
            if (!once) {
                if (callbackInfo[i]->once) {
                    HILOGI("JS_On callbackInfo %{public}p change once to on", callbackInfo[i]);
                    callbackInfo[i]->once = false;
                } else {
                    HILOGI("JS_On already on");
                }
            } else {
                if (callbackInfo[i]->once) {
                    HILOGI("JS_Once already once");
                } else {
                    HILOGI("JS_Once callbackInfo %{public}p change on to once", callbackInfo[i]);
                    callbackInfo[i]->once = true;
                }
            }
            return true;
        }
        return false;
    }

    napi_value OnOrOnce(napi_env env, napi_callback_info cbinfo, bool once)
    {
        size_t argc = ARGC_NUM;
        napi_value argv[ARGC_NUM] = {0};
        NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, NULL, NULL));
        NAPI_ASSERT(env, argc >= ARGC_NUM, "requires 2 parameter");

        napi_valuetype eventValueType;
        napi_typeof(env, argv[0], &eventValueType);
        NAPI_ASSERT(env, eventValueType == napi_object, "type mismatch for parameter 1");

        napi_valuetype eventHandleType;
        napi_typeof(env, argv[1], &eventHandleType);
        NAPI_ASSERT(env, eventHandleType == napi_function, "type mismatch for parameter 2");

        bool hasEventId = false;
        napi_value eventId;
        napi_has_named_property(env, argv[0], "eventId", &hasEventId);
        NAPI_ASSERT(env, hasEventId, "Wrong argument 1");
        napi_get_named_property(env, argv[0], "eventId", &eventId);
        uint32_t eventIdValue;
        napi_get_value_uint32(env, eventId, &eventIdValue);
        HILOGI("OnOrOnce eventIdValue:%{public}d", eventIdValue);

        if (!IsExistSameCallback(env, eventIdValue, argv[1], once)) {
            AsyncCallbackInfo* callbackInfo = new AsyncCallbackInfo();
            callbackInfo->env = env;
            callbackInfo->once = once;
            napi_create_reference(env, argv[1], 1, &callbackInfo->callback);
            emitterInstances[eventIdValue].asyncCallbackInfo.push_back(callbackInfo);
            HILOGI("OnOrOnce callbackInfo = %{public}p", callbackInfo);
        }
        return nullptr;
    }

    napi_value JS_On(napi_env env, napi_callback_info cbinfo)
    {
        HILOGI("JS_On start");
        OnOrOnce(env, cbinfo, false);
        return nullptr;
    }

    napi_value JS_Off(napi_env env, napi_callback_info cbinfo)
    {
        HILOGI("JS_Off start");
        size_t argc = 1;
        napi_value argv[1] = {0};
        NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, NULL, NULL));
        NAPI_ASSERT(env, argc >= 1, "requires 1 parameter");

        napi_valuetype eventValueType;
        napi_typeof(env, argv[0], &eventValueType);
        NAPI_ASSERT(env, eventValueType == napi_number, "type mismatch for parameter 1");

        uint32_t eventId;
        napi_get_value_uint32(env, argv[0], &eventId);
        auto subscribe = emitterInstances.find(eventId);
        if (subscribe != emitterInstances.end()) {
            for (auto callbackInfo : subscribe->second.asyncCallbackInfo) {
                HILOGI("JS_Off callbackInfo = %{public}p", callbackInfo);
                callbackInfo->isDeleted = true;
            }
        }
        return nullptr;
    }

    napi_value JS_Once(napi_env env, napi_callback_info cbinfo)
    {
        HILOGI("JS_Once start");
        OnOrOnce(env, cbinfo, true);
        return nullptr;
    }

    bool ParseEventData(napi_env env, napi_value key, napi_value data, Val* &val, char keyChars[NAPI_VALUE_STRING_LEN])
    {
        napi_valuetype valueType;
        napi_typeof(env, key, &valueType);
        if (valueType != napi_valuetype::napi_string) {
            HILOGI("param is discarded because the key type of the event params must be String.");
            return false;
        }

        size_t keyLength = 0;
        napi_get_value_string_utf8(env, key, keyChars, NAPI_VALUE_STRING_LEN, &keyLength);
        napi_value value = nullptr;
        napi_get_named_property(env, data, keyChars, &value);
        napi_typeof(env, value, &valueType);
        if (valueType == napi_valuetype::napi_boolean) {
            val->type = DataType::BOOL;
            napi_get_value_bool(env, value, &(val->value.bValue));
            HILOGD("key:%{public}s value=%{public}d.", keyChars, val->value.bValue);
        } else if (valueType == napi_valuetype::napi_number) {
            val->type = DataType::INT;
            napi_get_value_int32(env, value, &(val->value.nValue));
            HILOGD("key:%{public}s value=%{public}d.", keyChars, val->value.nValue);
        } else if (valueType == napi_valuetype::napi_string) {
            napi_get_value_string_utf8(env, value, val->value.cValue, NAPI_VALUE_STRING_LEN, &keyLength);
            val->type = DataType::STRING;
            HILOGD("key:%{public}s value=%{public}s.", keyChars, val->value.cValue);
        } else {
            HILOGI("param=%{public}s is discarded because the value type is invalid.", keyChars);
            return false;
        }
        return true;
    }

    bool EmitWithEventData(napi_env env, napi_value argv, uint32_t eventId, Priority priority)
    {
        napi_valuetype dataType;
        napi_typeof(env, argv, &dataType);
        NAPI_ASSERT(env, dataType == napi_object, "type mismatch for parameter 2");

        bool hasData = false;
        napi_has_named_property(env, argv, "data", &hasData);
        if (hasData) {
            napi_value data = nullptr;
            napi_get_named_property(env, argv, "data", &data);

            napi_value keyArr = nullptr;
            napi_status status = napi_get_property_names(env, data, &keyArr);
            if (status != napi_ok) {
                HILOGI("can not get property names");
                return false;
            }
            uint32_t len = 0;
            status = napi_get_array_length(env, keyArr, &len);
            if (status != napi_ok) {
                HILOGI("can not get array length");
                return false;
            }

            EventData eventData;
            bool hasEventData = false;
            for (uint32_t i = 0; i < len; i++) {
                napi_value key = nullptr;
                napi_get_element(env, keyArr, i, &key);
                char keyChars[NAPI_VALUE_STRING_LEN] = {0};
                Val* val = new Val();
                if (!ParseEventData(env, key, data, val, keyChars)) {
                    delete val;
                    continue;
                }
                eventData.data.insert(make_pair(keyChars, *val));
                hasEventData = true;
                delete val;
            }

            if (hasEventData) {
                HILOGI("sendevent with eventData id:%{public}d", eventId);
                auto event = InnerEvent::Get(eventId, make_unique<EventData>(eventData));
                eventHandler->SendEvent(event, 0, priority);
                return true;
            }
        }
        return false;
    }

    bool IsExistValidCallback(napi_env env, uint32_t eventId)
    {
        auto subscribe = emitterInstances.find(eventId);
        if (subscribe == emitterInstances.end()) {
            HILOGW("JS_Emit has no callback");
            return false;
        }
        vector<AsyncCallbackInfo *> callbackInfo = subscribe->second.asyncCallbackInfo;
        size_t callbackSize = callbackInfo.size();
        for (size_t i = 0; i < callbackSize; i++) {
            if (!callbackInfo[i]->isDeleted) {
                return true;
            }
        }
        return false;
    }

    napi_value JS_Emit(napi_env env, napi_callback_info cbinfo)
    {
        HILOGI("JS_Emit start");
        size_t argc = ARGC_NUM;
        napi_value argv[ARGC_NUM] = {0};
        NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, NULL, NULL));
        NAPI_ASSERT(env, argc >= 1, "requires more than 1 parameter");
        HILOGI("JS_Emit argc:%{public}zu", argc);

        napi_valuetype eventValueType;
        napi_typeof(env, argv[0], &eventValueType);
        NAPI_ASSERT(env, eventValueType == napi_object, "type mismatch for parameter 1");

        bool hasEventId = false;
        napi_value value;
        napi_has_named_property(env, argv[0], "eventId", &hasEventId);
        NAPI_ASSERT(env, hasEventId == true, "Wrong argument 1");
        napi_get_named_property(env, argv[0], "eventId", &value);
        uint32_t eventId;
        napi_get_value_uint32(env, value, &eventId);
        HILOGI("JS_Emit eventIdValue:%{public}d", eventId);

        if (!IsExistValidCallback(env, eventId)) {
            HILOGW("JS_Emit has no valid callback");
            return nullptr;
        }

        bool hasPriority = false;
        napi_has_named_property(env, argv[0], "priority", &hasPriority);
        Priority priority = Priority::LOW;
        if (hasPriority) {
            napi_get_named_property(env, argv[0], "priority", &value);
            uint32_t priorityValue;
            napi_get_value_uint32(env, value, &priorityValue);
            HILOGI("JS_Emit priority:%{public}d", priorityValue);
            priority = static_cast<Priority>(priorityValue);
        }

        if (argc == ARGC_NUM && EmitWithEventData(env, argv[1], eventId, priority)) {
            HILOGI("EmitWithEventData sucess");
            return nullptr;
        } else {
            HILOGI("JS_Emit sendevent without id:%{public}d", eventId);
            auto event = InnerEvent::Get(eventId, make_unique<EventData>());
            eventHandler->SendEvent(event, 0, priority);
            return nullptr;
        }
    }

    napi_value EnumEventClassConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisArg = nullptr;
        void *data = nullptr;

        napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

        napi_value global = nullptr;
        napi_get_global(env, &global);

        return thisArg;
    }

    napi_value CreateEnumEventPriority(napi_env env, napi_value exports)
    {
        napi_value immediate = nullptr;
        napi_value high = nullptr;
        napi_value low = nullptr;
        napi_value idle = nullptr;

        napi_create_uint32(env, (uint32_t)Priority::IMMEDIATE, &immediate);
        napi_create_uint32(env, (uint32_t)Priority::HIGH, &high);
        napi_create_uint32(env, (uint32_t)Priority::LOW, &low);
        napi_create_uint32(env, (uint32_t)Priority::IDLE, &idle);

        napi_property_descriptor desc[] = {
            DECLARE_NAPI_STATIC_PROPERTY("IMMEDIATE", immediate),
            DECLARE_NAPI_STATIC_PROPERTY("HIGH", high),
            DECLARE_NAPI_STATIC_PROPERTY("LOW", low),
            DECLARE_NAPI_STATIC_PROPERTY("IDLE", idle),
        };
        napi_value result = nullptr;
        napi_define_class(env, "EventPriority", NAPI_AUTO_LENGTH, EnumEventClassConstructor, nullptr,
            sizeof(desc) / sizeof(*desc), desc, &result);

        napi_set_named_property(env, exports, "EventPriority", result);

        return exports;
    }

    napi_value EmitterInit(napi_env env, napi_value exports)
    {
        HILOGE("enter");
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("on", JS_On),
            DECLARE_NAPI_FUNCTION("once", JS_Once),
            DECLARE_NAPI_FUNCTION("off", JS_Off),
            DECLARE_NAPI_FUNCTION("emit", JS_Emit),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

        CreateEnumEventPriority(env, exports);

        eventHandler = EventHandlerInstance::GetInstance();
        return exports;
    }
}  // namespace AppExecFwk
}  // namespace OHOS
