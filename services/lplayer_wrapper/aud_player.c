#if defined(BT_MEDIA_SUPPORT_LPLAYER)
#define AUD_PLAYER_C

#include <stdio.h>
#include <stdlib.h>

#include "aud_player.h"

#include "lplayer.h"

//#include "cmsis_os.h"
#include "app_bt_stream.h"
#include "app_bt_media_manager.h"
#include "hal_trace.h"
#include "cmsis_os2.h"


uint8_t nv_record_get_aud_vol(void);
void nv_record_set_aud_vol(uint8_t v);


osMutexDef(priv_lock);

osSemaphoreDef(priv_sem);

enum {
  REQ_NONE = 0,
  REQ_PLAY,
  REQ_PAUSE,
  REQ_RESUME,
  REQ_STOP,
  REQ_FINISH,

  REQ_NUM
};

typedef struct
{
  osMutexId lock;
  osSemaphoreId sem;
  uint8_t *buf;         //NOT NULL means playing from buffer
  uint32_t bufsz;
  uint32_t offset;
  lplayer_t *instance; //lplayer instance
  osThreadId mediaThread; 
  osThreadId reqThread;
  audplayer_complete_notify callback;
  uint8_t req; //true-sending msg and warting for ack result
  uint8_t stopCause; //0-not stopped, 1-complete success, <0-error stopped
  uint8_t paused; //0-not paused, 1-paused actively by self, 2-paused passively for competition of af_stream
  uint8_t vol;
  int     result; //result of send_msg_and_wait()
} aud_player_t;


static aud_player_t g_aud_player = {
    NULL,
};

#define PLAYER_IN_MEDIA_THREAD()     ((uint32_t)g_aud_player.mediaThread == (uint32_t)g_aud_player.reqThread)

#define PLAYER_LOCK()         do{TRACE(0,"===PLAYER_LOCKING:%s",__func__);osMutexWait(g_aud_player.lock, osWaitForever);TRACE(0,"===PLAYER_LOCKED:%s",__func__);}while(0)
#define PLAYER_UNLOCK()       do{TRACE(0,"===PLAYER_UNLOCKING:%s",__func__);osMutexRelease(g_aud_player.lock);}while(0)
#define PLAYER_INITIALIZED()  (!!g_aud_player.lock)
#define PLAYER_WAIT()         do{ \
    if(PLAYER_IN_MEDIA_THREAD()){TRACE(0, "===PLAYER_WAITING: NO NEED TO WAIT in %s",__func__);} \
    else { \
      TRACE(0,"===PLAYER_WAITING:%s",__func__); \
      osSemaphoreWait(g_aud_player.sem, osWaitForever); \
      TRACE(0,"===PLAYER_RESUMED:%s",__func__); \
    }\
  }while(0)

#define PLAYER_WAKEUP()       do{TRACE(0,"===PLAYER_WAKINGUP:%s",__func__);\
    if(!PLAYER_IN_MEDIA_THREAD())osSemaphoreRelease(g_aud_player.sem); \
    g_aud_player.req = REQ_NONE; \
  }while(0)

extern osThreadId app_os_tid_get(void);
// #include "app_audio.h"
extern int app_audio_sendrequest_callback(uint32_t cb, uint32_t param0, uint32_t param1, uint32_t param2);

void aud_player_deinit(void)
{
  if (PLAYER_INITIALIZED())
  {
    osMutexDelete(g_aud_player.lock);
    g_aud_player.lock = NULL;
  }
}

int aud_player_init(void)
{
  if (PLAYER_INITIALIZED())
  {
    return 0; // already initialized
  }
  memset(&g_aud_player, 0, sizeof(g_aud_player));
  g_aud_player.lock = osMutexCreate(osMutex(priv_lock));
  if (g_aud_player.lock == NULL)goto ERROR;
  g_aud_player.sem = osSemaphoreCreate(osSemaphore(priv_sem), 0);
  if(g_aud_player.sem == NULL)goto ERROR;
  g_aud_player.mediaThread = app_os_tid_get();
  g_aud_player.stopCause = 0;
  g_aud_player.instance = NULL;
  g_aud_player.paused = 0;
  //FIXME: read audio vol from factory
  g_aud_player.vol = nv_record_get_aud_vol();
  TRACE(0, "%s: vol=%d", __func__, g_aud_player.vol);
  if(g_aud_player.vol > TGT_VOLUME_LEVEL_MAX)g_aud_player.vol = TGT_VOLUME_LEVEL_MAX;

  return 0;
ERROR:
  TRACE(0, "%s failed!!!!", __func__);
  aud_player_deinit();
  return AUD_E_NOMEMORY;
}

//result stored in g_aud_player.result when calling aud_player_do_play()/do_pause()
static void send_msg_and_wait(uint8_t req){
  ASSERT(req < REQ_NUM, "[%s] INVALID req type: %d", __func__, req);
  TRACE(0, "%s: req=%d, currThread=%p, mediaThread=%p", __func__, req, osThreadGetId(), g_aud_player.mediaThread);
  uint8_t msgid = 0;
  switch(req){
    case REQ_PLAY:    msgid = APP_BT_STREAM_MANAGER_START; break;
    case REQ_PAUSE:   msgid = APP_BT_STREAM_MANAGER_STOP_MEDIA; break;
    case REQ_RESUME:  msgid = APP_BT_STREAM_MANAGER_UPDATE_MEDIA; break;
    case REQ_STOP:    msgid = APP_BT_STREAM_MANAGER_STOP; break;
    case REQ_FINISH:  msgid = APP_BT_STREAM_MANAGER_STOP; break;
    default:
      return;
  }
  g_aud_player.req = req;
  g_aud_player.reqThread = osThreadGetId();

  app_audio_manager_sendrequest(msgid, BT_STREAM_LPLAYER, 0, 0);
  PLAYER_WAIT();
}
/**
 * @brief player stopped, MUST clear anything from bt_media_manager
 * running in app_thread
 * @param cause 
 */
static void __on_closed(int cause, uint32_t unused1, uint32_t unused2){
  TRACE(0, "%s cause = %d, threadId=%p", __func__, cause, osThreadGetId());
  if(!PLAYER_INITIALIZED())return;
  audplayer_complete_notify cb = NULL;
  PLAYER_LOCK();
  cb = g_aud_player.callback;
  if(g_aud_player.instance){
    send_msg_and_wait(REQ_FINISH);
    g_aud_player.stopCause = cause?cause>0?-cause:cause:1;
  }
  PLAYER_UNLOCK();
  if(cb)cb(cause?cause>0?-cause:cause:0);
}
//running in lplayer context
static int __on_complete_notify(void)
{
  app_audio_sendrequest_callback((uint32_t)__on_closed, 0,0,0);
  return 0;
}

static int __on_error_notify(int error_code)
{
  //__on_closed(error_code);
  app_audio_sendrequest_callback((uint32_t)__on_closed, (uint32_t)error_code,0,0);
  return 0;
}





/**
 * play audio from (buf, size)
 * 
 * @return uint8_t 
 */

#define PLAYER_BUFFER_SIZE                          100 * 1024
#define PLAYER_BUFFER_INPUT_SIZE_ONCE               4 * 1024

// Thread for Writing data
static osThreadId lplayer_buffer_tid = NULL;
static void lplayer_buffer_task(const void *arg) {
    TRACE(0, "**************************%s running...", __func__);
    uint8_t eos_flag = 0;
    if(!g_aud_player.buf){
      lplayer_buffer_write(g_aud_player.instance, NULL, 0, true);
      goto error_exit;
    }
    while(g_aud_player.buf && g_aud_player.instance){
      uint32_t length = g_aud_player.bufsz - g_aud_player.offset;
      if(length > PLAYER_BUFFER_INPUT_SIZE_ONCE)length = PLAYER_BUFFER_INPUT_SIZE_ONCE;
      if (length == 0){
          TRACE(0, "%s EOF, instance=%p", __func__, g_aud_player.instance);
          lplayer_buffer_write(g_aud_player.instance, NULL, 0, true); // When finish Putting data, set EOS to lplayer
          break;
      }
      int ret = lplayer_buffer_write(g_aud_player.instance, g_aud_player.buf + g_aud_player.offset, length, false);
      TRACE(0, "%s lplayer_buffer_write=%d/%d, off=%d, total=%d", __func__, ret, length, g_aud_player.offset, g_aud_player.bufsz);
      if (ret == 0){
        g_aud_player.offset += length;
      }else if (ret == 1){
        osDelay(200); // Not enough stream buffer, wait 200ms
      }else{
        break;
      }
    }
error_exit:
    TRACE(0, "**************************%s exit", __func__);
    lplayer_buffer_tid = NULL;
  	osThreadExit();
}

osThreadDef(lplayer_buffer_task, osPriorityNormal, 1, 4*1024, "lplayer_buffer");

static int __lplayer_close(void){
  TRACE(0, "%s: instance=%p", g_aud_player.instance);
  if(g_aud_player.instance){
      int ret = lplayer_close(g_aud_player.instance);
      g_aud_player.instance = NULL;
      g_aud_player.paused = 0;
      if(g_aud_player.buf){
        g_aud_player.buf = NULL;
        g_aud_player.bufsz = 0;
        g_aud_player.offset = 0;
        while(lplayer_buffer_tid){
          osDelay(1);//wait thread exit
        }
      }
      return ret;
  }
  return 0;
}
static lplayer_t *__lplayer_open(void *uri, uint32_t size){
  lplayer_t *l;
  if(size == 0){
    l = lplayer_open_url((const char *)uri);
    TRACE(0, "%s(%s): %p", __func__, uri, l);
  }else{
    l = lplayer_open_buffer(NULL, size < 1024*100?102400:size);
    TRACE(0, "%s(%p[%d]): %p", __func__, uri, size, l);
  }
  if (l != NULL){
    if(size){
      g_aud_player.buf = (uint8_t *)uri;
      g_aud_player.bufsz = size;
      g_aud_player.offset = 0;
    }
    lplayer_register_complete_notify(l, __on_complete_notify);
    lplayer_register_error_notify(l, __on_error_notify);  
  }
  return l;
}

extern int app_bt_stream_volumeset(int8_t vol);
/**
 * @brief play/resume
 * Not user api, just for app_bt_stream_open() in app_bt_stream.cpp, which will be called by app_audio_handle_process for APP_BT_SETTING_OPEN in app_audio.cpp
 * use cases：
 * 1. user request to play audio           - REQ_PLAY
 * 2. user request to resume audio         - REQ_RESUME
 * 3. audio resumed automatically when other stream stopped
 *    Before resuming, status should be checked. NEVER resumed if paused actively
 * @return int 
 * 0-success
 */
int aud_player_do_play(bool refused){
  const uint8_t req = g_aud_player.req;
  int ret = 0;
  if(!req)PLAYER_LOCK();
  ASSERT(g_aud_player.instance, "%s: player not opened", __func__);
  TRACE(0, "%s: req=%d, threadId=%p", __func__, req, osThreadGetId());
  if(refused){
    //high priority case like hfp call in progress, if use request to resume audio playing, it will be refused.
    TRACE(0, "OOOOOPS......REFUSED to play, req=%d", req);
    ret = AUD_E_REFUSED;
  }else if(!req && g_aud_player.paused == 1){
    TRACE(0, "WHAT'S UP......I'm sleeping. DON'T wake me up.");
    ret = AUD_E_PAUSED;
  }else{
    if(g_aud_player.buf){
      lplayer_buffer_tid = osThreadCreate(osThread(lplayer_buffer_task), NULL);
      ASSERT(lplayer_buffer_tid != NULL, "Failed to create lplayer_buffer Thread");    
    }
    ret = lplayer_play(g_aud_player.instance); 
    TRACE(0, "%s: lplayer_play returns %d", __func__, ret);
    g_aud_player.result = ret;
    if(ret == 0){
      g_aud_player.paused = 0;
      g_aud_player.stopCause = 0;    
      //FIXME: only for testing to seek to the end when playing
      // if(g_aud_player.req == REQ_PLAY){
      //   lplayer_seek(g_aud_player.instance, 60*3);
      // }
      TRACE(2, "%s: vol=%d", __func__, g_aud_player.vol);
      app_bt_stream_volumeset(g_aud_player.vol);
    } else {
      __lplayer_close();
    }
  }
  if(!req)PLAYER_UNLOCK();
  else PLAYER_WAKEUP();
  return ret;
}

/**
 * @brief stop/pause
 * Not user api, just for app_bt_stream_close() in app_bt_stream.cpp, which will be called by app_audio_handle_process for APP_BT_SETTING_CLOSE in app_audio.cpp
 * use cases：
 * 1. play complete or error       - REQ_FINISH
 * 2. user request to stop audio   - REQ_STOP
 * 3. user request to pause audio  - REQ_PAUSE
 * 4. audio paused automatically when other high priority stream playing
 * 
 * @return int 
 * 0-success
 */
int aud_player_do_stop(void){
  const uint8_t req = g_aud_player.req;
  int ret = 0;
  if(!req)PLAYER_LOCK();
  if(!g_aud_player.instance){
    TRACE(0, "%s: player not opened", __func__);
  }else{
    TRACE(0, "%s: req=%d, threadId=%p", __func__, req, osThreadGetId());
    if(req == REQ_FINISH || req == REQ_STOP){
      ret = __lplayer_close();
    }else{
      ret = lplayer_pause(g_aud_player.instance); 
      if(ret == 0){
        g_aud_player.paused = (req == REQ_PAUSE)?1:2;
      }
    }
    TRACE(0, "%s: returns=%d", __func__, ret);
    g_aud_player.result = ret;

  }
  if(!req)PLAYER_UNLOCK();
  else PLAYER_WAKEUP();
  return ret;
}

static int __req_stop(void){
  //send msg to stop and wait complete
  int ret = 0;
  send_msg_and_wait(REQ_STOP);
  ret = g_aud_player.result;
  g_aud_player.paused = 0;
  g_aud_player.stopCause = 0;
  g_aud_player.instance = NULL;
  return ret;
}

//size > 0 means buffer, or else, is file or url
static int __aud_player_start(void *urlOrFileOrBuf, uint32_t size, audplayer_complete_notify callback){
  int ret = aud_player_init();
  if(ret < 0)return ret;
  PLAYER_LOCK();
  if(g_aud_player.instance){
    //another audio playing/paused now
    if(!g_aud_player.stopCause && !g_aud_player.paused){
      //playing
      ret = __lplayer_close();
      if(g_aud_player.callback){
        g_aud_player.callback(AUD_E_PREEMPT);
        g_aud_player.callback = NULL;
      }
      g_aud_player.instance = __lplayer_open(urlOrFileOrBuf, size);
      if(g_aud_player.instance){
        ret = lplayer_play(g_aud_player.instance); 
        TRACE(3, "%s: ret=%d, vol=%d", __func__, ret, g_aud_player.vol);
        if(ret == 0){
          app_bt_stream_volumeset(g_aud_player.vol);
        }
      }else{
        //abnormal, file not exists, request to clear status since audio player stopped.
        __req_stop();
        ret = AUD_E_OPEN;
      }
      goto out;
    }else{
      //paused by others
      ret = __req_stop();
    }
  }
  g_aud_player.instance = __lplayer_open(urlOrFileOrBuf, size);
  if (g_aud_player.instance == NULL){
    ret = AUD_E_OPEN;
    if (g_aud_player.paused)
      g_aud_player.paused = 0;
  }else if (g_aud_player.paused == 2){
    // being preempted, take place the previous audio
    ret = AUD_E_WOULDBLOCK;
  }else{
    // paused or playing, play the new audio instead
    //send msg to bt_media_manager
    send_msg_and_wait(REQ_PLAY);
    ret = g_aud_player.result;
  }
out:
  if(ret == 0)g_aud_player.callback = callback;
  PLAYER_UNLOCK();
  TRACE(0, "%s(%s) returns %d", __func__, (char*)(size==0?urlOrFileOrBuf:"buffer"), ret);        
  return ret;
}

int aud_player_start_buffer(const uint8_t *buffer, uint32_t size, audplayer_complete_notify callback){
  TRACE(3, "%s(%p[%d]) instance=%p", __func__, buffer, size, g_aud_player.instance);
  return __aud_player_start((void*)buffer, size, callback);
}


int aud_player_start(const char *urlOrFile, audplayer_complete_notify callback){
  TRACE(3, "%s uri=%s instance=%p", __func__, urlOrFile, g_aud_player.instance);
  return __aud_player_start((void*)urlOrFile, 0, callback);
}

int aud_player_stop(void)
{
  PLAYER_LOCK();
  int ret = 0;
  if(PLAYER_INITIALIZED() && g_aud_player.instance){
    ret = __req_stop();
  }
  PLAYER_UNLOCK();
  return ret;
}

int aud_player_pause(void)
{
  if(!PLAYER_INITIALIZED())return AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_E_NOT_OPEN;
    TRACE(0, "%s: audio player not opend", __func__);
  }else if(g_aud_player.paused){
    ret = 0;
  }else{
    //send msg to pause
    send_msg_and_wait(REQ_PAUSE);    
    ret = g_aud_player.result;
  }
  PLAYER_UNLOCK();
  return ret;
}

int aud_player_resume(void)
{
  if(!PLAYER_INITIALIZED())return AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_E_NOT_OPEN;
  }else if(!g_aud_player.paused){
    ret = 0;
  }else{
    //send msg to resume
    send_msg_and_wait(REQ_RESUME);    
    ret = g_aud_player.result;
  }
  PLAYER_UNLOCK();
  return ret;
}

int aud_player_seek(uint32_t sec){
  if(!PLAYER_INITIALIZED())return AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_E_NOT_OPEN;
  }else{
    if (g_aud_player.paused){
      TRACE(0, "%s: audio player is paused", __func__);
      ret = AUD_E_PAUSED;
    }else{
      ret = lplayer_seek(g_aud_player.instance, sec);
      //lplayer default vol is 10. NEVER use it.
      if(ret == 0)app_bt_stream_volumeset(g_aud_player.vol);
      TRACE(0, "%s: lplayer_seek returns %d", __func__, ret);
    }
  }
  PLAYER_UNLOCK();
  return ret;
}

uint8_t aud_player_set_vol_cfg(uint8_t vol){
  if(vol >= TGT_VOLUME_LEVEL_MAX)vol = TGT_VOLUME_LEVEL_MAX;
  else if(vol <= 0)vol = 0;
  if(!PLAYER_INITIALIZED()){
    nv_record_set_aud_vol(vol);
  } else if(g_aud_player.vol != vol){
    nv_record_set_aud_vol(vol);
    g_aud_player.vol = vol;
  }
  return vol;
}

uint8_t aud_player_get_vol(){
  if(PLAYER_INITIALIZED())return g_aud_player.vol;
  uint8_t vol = nv_record_get_aud_vol();
  if(vol > TGT_VOLUME_LEVEL_MAX)vol = TGT_VOLUME_LEVEL_MAX;
  return vol;
}

uint8_t aud_player_set_vol(uint8_t vol)
{
  vol = aud_player_set_vol_cfg(vol);
  if(!PLAYER_INITIALIZED())return 0;//AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if (g_aud_player.instance && !g_aud_player.paused) {
    ret = app_bt_stream_volumeset(vol);
  }
  PLAYER_UNLOCK();
  return ret;
  //int ret = lplayer_set_vol(g_aud_player.instance, vol);
}

/**
 * @brief Get the current playback position
 * 
 * @return int 
 * < 0     errors
 * >=0     current position, in milliseconds
 */
int aud_player_tell(void){
  if(!PLAYER_INITIALIZED())return AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_E_NOT_OPEN;
  }else{
    uint32_t ms;
    ret = lplayer_get_current_pos(g_aud_player.instance, &ms);
    TRACE(0, "%s returns %d, pos=%d", __func__, ret, ms);
    if(ret == 0)ret = (int)ms;
  }
  PLAYER_UNLOCK();
  return ret;
}

/**
 * @brief  Get the current playing resource duration
 * 
 * @return int 
 * < 0     errors
 * >=0     duration, in milliseconds
 */
int aud_player_duration(void){
  if(!PLAYER_INITIALIZED())return AUD_E_NOT_INITIALIZED;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_E_NOT_OPEN;
  }else{
    uint32_t ms;
    ret = lplayer_get_duration_ms(g_aud_player.instance, &ms);
    TRACE(0, "%s returns %d, duration=%d", __func__, ret, ms);
    if(ret == 0)ret = (int)ms;
  }
  PLAYER_UNLOCK();
  return ret;
}

/**
 * @brief Get the current status of the audio player
 * 
 * @return int 
 * <0       error
 * >=0      @see typedef t_AudPlayerStatus.
 */
int aud_player_status(void){
  if(!PLAYER_INITIALIZED())return AUD_S_IDLE;
  PLAYER_LOCK();
  int ret = 0;
  if(!g_aud_player.instance){
    ret = AUD_S_IDLE;
  }else{
    if(g_aud_player.stopCause){
      ret = g_aud_player.stopCause == 1?AUD_S_STOPPED:AUD_S_ERROR_STOPPED;
    }else if(g_aud_player.paused){
      ret = g_aud_player.paused==1?AUD_S_PAUSED_ACTIVELY:AUD_S_PAUSED_PASSIVELY;
    }else{
      ret = AUD_S_PLAYING;
    }
    // lplayer_state_t status;
    // ret = lplayer_get_current_status(g_aud_player.instance, &status);
    // if(ret == 0){
    // }
  }
  PLAYER_UNLOCK();
  return ret;
}

#ifdef MEDIA_PLAYER_SUPPORT
#include "app_media_player.h"
#endif
bool aud_player_vol_key_handler(bool up){
  int status = aud_player_status();
  if (status == AUD_S_PAUSED_ACTIVELY || status == AUD_S_PLAYING){
    uint8_t vol = aud_player_get_vol();
    bool changed = false;
    TRACE(2, "%s audio player, current vol=%d", __func__, vol);
    if(up){
      if (vol < TGT_VOLUME_LEVEL_MAX){
        vol++;
        changed = true;
      }
    } else if(vol > 0){
      vol--;
      changed = true;
    }
    if(changed){
      aud_player_set_vol(vol);
    }
  #ifdef MEDIA_PLAYER_SUPPORT
    else {
      media_PlayAudio(AUD_ID_BT_WARNING, 0);
    }
  #endif
    return true;
  }
  return false;
}

#endif