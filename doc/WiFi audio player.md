# WiFi audio player  #

BES2600WM SDK uses lplayer to play audio. lplayer is  an audio player developed by BES, which supports local audio sources and cloud audio souces, supports http/https/hls protocol. The  unpacker plug-in supported by lplayer is: 

*（1）  MP3*
*（2）  MP4* 
*（3）  AAC-adts* 
*（4）  WAV* 
*（5）  PCM*

*（6）  FLAC*

The decoder plug-in supported by lplayer is :
*（1）  MP3* 
*（2）  AAC_adts* 
*（3）  AAC_mp4* 
*（4）  PCM*

*（5）  FLAC*

The Interface introduce file.

services\lplayer\interface\include\lplayer.h

Sample code：
```c
#include "lplayer.h"
#include "audio_sink.h"
static lplayer_t *g_lplayer_at_instance = NULL;
static uint8_t g_lplayer_at_terminated = 0;
static int lplayer_on_complete_notify(void)
{
    TRACE(0, "%s", __func__);
    g_lplayer_at_terminated = 1;
    return 0;
}

static int lplayer_on_error_notify(int error_code)
{
    TRACE(0, "%s error_code = %d", __func__, error_code);
    g_lplayer_at_terminated = 1;
    return 0;
}

static void lplayer_printf_process_result(const char * process, const char* result)
{
    if(process && result) {
        TRACE(0, "lplayer %s %s", process, result);
    }
}

#define LPLAYER_SUCC(s) do{lplayer_printf_process_result(s, "success");}while(0);
#define LPLAYER_ERROR(s) do{lplayer_printf_process_result(s, "error");}while(0);

static int do_lplayer_start(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    char *uri = NULL;
    TRACE(0, "argc = %d", argc);
    if (argc >= 2)
    {
        uri = argv[1];
    }


    if (g_lplayer_at_terminated)
    {
        TRACE(0, "%s close last lplayer", __func__);
        lplayer_close(g_lplayer_at_instance);
        g_lplayer_at_instance = NULL;
    }

    if (g_lplayer_at_instance == NULL)
    {
        g_lplayer_at_instance = lplayer_open_url(uri);
        if (g_lplayer_at_instance == NULL)
        {
            LPLAYER_ERROR("start");
            return -1;
        }
        lplayer_register_complete_notify(g_lplayer_at_instance, lplayer_on_complete_notify);
        lplayer_register_error_notify(g_lplayer_at_instance, lplayer_on_error_notify);
    }

    if (0 != lplayer_play(g_lplayer_at_instance))
    {
        lplayer_close(g_lplayer_at_instance);
        g_lplayer_at_instance = NULL;
        LPLAYER_ERROR("start");
        return -1;
    }
    g_lplayer_at_terminated = 0;
    LPLAYER_SUCC("start");
    return 0;

}

static int do_lplayer_stop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (0 != lplayer_close(g_lplayer_at_instance))
    {
        LPLAYER_ERROR("stop");
        return -1;
    }
    g_lplayer_at_instance = NULL;
    LPLAYER_SUCC("stop");
    return 0;
}

static int do_lplayer_pause(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (0 != lplayer_pause(g_lplayer_at_instance))
    {
        LPLAYER_ERROR("pause");
        return -1;
    }
    LPLAYER_SUCC("pause");
    return 0;
}

static int do_lplayer_set_vol(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int vol = atoi(argv[1]);
        TRACE(0, "vol = %d", vol);
        if (0 != lplayer_set_vol(g_lplayer_at_instance, vol))
        {
            LPLAYER_ERROR("set volume");
            return -1;
        }
        LPLAYER_SUCC("set volume");
        return 0;
    }
    LPLAYER_ERROR("set volume");
    return -1;
}

static int do_lplayer_enable_single_loop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int en = atoi(argv[1]);
        TRACE(0, "en = %d", en);
        if (en != 0)
        {
            en = 1;
        }
        if (0 != lplayer_single_loop_en(g_lplayer_at_instance, en))
        {
            LPLAYER_ERROR("enable single loop");
            return -1;
        }
        LPLAYER_SUCC("enable single loop");
        return 0;
    }
    LPLAYER_ERROR("enable single loop");
    return -1;
}

static int do_lplayer_seek(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int time_s = atoi(argv[1]);
        TRACE(0, "time_s = %d", time_s);
        if (0 != lplayer_seek(g_lplayer_at_instance, time_s))
        {
            LPLAYER_ERROR("seek");
            return -1;
        }
        LPLAYER_SUCC("seek");
        return 0;
    }
    LPLAYER_ERROR("seek");
    return -1;
}

static int do_lplayer_show_current_play_time(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t cur_play_time = 0;
    if (0 != lplayer_get_current_pos(g_lplayer_at_instance, &cur_play_time))
    {
        LPLAYER_ERROR("get_pos");
        return -1;
    }
    TRACE(0, "%s cur_play_time ====> %d ms", __func__, cur_play_time);
    LPLAYER_SUCC("get_pos");
    return 0;
}



static int do_lplayer_set_global_sink_type(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int type = atoi(argv[1]);
        audio_sink_type_t sink_type = 0;
        switch(type)
        {
            case 0:
            sink_type = AUDIO_SINK_ALSA;
            break;
            case 1:
            sink_type = AUDIO_SINK_LE_AUDIO;
            break;
            case 2:
            sink_type = AUDIO_SINK_BT_AUDIO;
            break;
            default:
            break;
        }
        if(sink_type != 0)
        {
            audio_sink_global_sink_type_set(sink_type);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}
```

Here is another two sample code：

apps\bes_test\audio\test_audio.c

services\wifi_app\wifi_console\at.c