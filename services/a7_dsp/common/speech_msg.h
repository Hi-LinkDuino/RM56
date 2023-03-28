#ifdef SPEECH_MSG_ENABLE
#ifndef __SPEECH_MSG_H__
#define __SPEECH_MSG_H__

#include "transq_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*speech_msg_callback_t)(void *msg, int size);
/* return size success */
int speech_msg_send(void* msg, int size);
/* Don't block in cb func */
void speech_msg_subscribe(speech_msg_callback_t cbf);
void speech_msg_unsubscribe(speech_msg_callback_t cbf);
void speech_msg_send_release(void);

/* extend api */
int speech_msg_send_ext(TRANSQ_MSG_TYPE_T type, uint8_t cmd, void* msg, int size);
void speech_msg_subscribe_ext(TRANSQ_MSG_TYPE_T type, speech_msg_callback_t cbf);
void speech_msg_unsubscribe_ext(TRANSQ_MSG_TYPE_T type, speech_msg_callback_t cbf);

#ifdef __cplusplus
}
#endif

#endif /* __SPEECH_MSG_H__ */
#endif //SPEECH_MSG_ENABLE