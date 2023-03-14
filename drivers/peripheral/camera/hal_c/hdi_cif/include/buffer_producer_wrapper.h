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

#ifndef HOS_BUFFER_PRODUCER_WRAPPER_H
#define HOS_BUFFER_PRODUCER_WRAPPER_H

#include <memory>
#include "types.h"
#include "camera_types_c_if.h"
#ifdef __cplusplus
extern "C" {
#endif

extern void BindBufferProducer(int id, OHOS::sptr<OHOS::IBufferProducer>& producer);
extern int BufferProducerRequestBuffer(GraphicBufferCIF* buffer);
extern int BufferProducerCancelBuffer(GraphicBufferCIF* buffer);
extern int BufferProducerFlushBuffer(GraphicBufferCIF* buffer);
extern int BufferProducerGetQueueSize(int streamId);
extern unsigned int BufferProducerSetQueueSize(int streamId, int size);
extern void BufferProducerDetachBufferProducer(int id);

#ifdef __cplusplus
}
#endif


#endif
