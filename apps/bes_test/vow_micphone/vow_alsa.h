#ifndef _SINK_ALSA_H_
#define _SINK_ALSA_H_

#ifdef __cplusplus
extern "C" {
#endif

void* sink_alsa_open(unsigned int sample_rate, int channel, int format_size);
int sink_alsa_start(void* handle);
void sink_alsa_close(void* handle);
int sink_alsa_write(void* handle, unsigned char* buffer, int buffer_size);

#ifdef __cplusplus
}
#endif


#endif /* _SINK_ALSA_H_ */
