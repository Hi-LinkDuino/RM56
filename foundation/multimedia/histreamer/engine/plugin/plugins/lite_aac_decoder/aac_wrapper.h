/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_AAC_WRAPPER_H
#define HISTREAMER_AAC_WRAPPER_H

struct aac_stream_info_t {
    int samplerate;
    int frame_size;
    int channel_num;
    int sample_per_frame;
};

int aac_decoder_init(void);
void aac_decoder_close(void);
int aac_decoder_input_data(unsigned char **input_ptr, const unsigned int *input_length, unsigned int *remain_size);
int aac_decoder_get_frame(signed short *cached_frame, const int out_pcm_size, const unsigned int flags);
aac_stream_info_t *aac_decoder_get_meida_info(void);

#endif