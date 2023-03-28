
/**
 * besfd_api.c collects useful platform APIs
 *
 */

#include <stdio.h>
#include "besfd_api.h"

#include "bwifi_interface.h"
#include "hal_norflash.h"
#include "cmsis_os2.h"
#ifdef osCMSIS_FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#if (defined(__A7_DSP_ENABLE__) || defined(MCU_DSP) || defined(CP_DSP))
#include "mcu_audio.h"
#include "data_dump.h"
#endif
#ifdef __A7_DSP_ENABLE__
#include "a7_cmd.h"
#endif
#if defined(__NET_FTP_CLIENT_SUPPORT__)
#include "ftp_client.h"
#endif

#include "hal_timer.h"

#include "nvrecord_extension.h"
#ifdef CHIP_HAS_RGB_LED
#include "app_led.h"
#endif

#if defined(__NET_FTP_CLIENT_SUPPORT__)
/**
 * Connect ftp server and save settings to NV once success
 */
void *ftp_connect_with(char *server, char *port, char *user, char *passwd)
{
    void *handler = NULL;
    bool connected = false;
    int ret = -1;
    handler = ftp_client_init();
    if (handler == NULL)
    {
        TRACE(0, "ftp client init failed");
        goto out;
    }
    ret = ftp_connect_server(handler, server, atoi(port));
    TRACE(0, "[%s] ip:%s port:%s, returns %d", __func__, server, port, ret);
    if (ret < 0)
        goto out;
    connected = true;
    ret = ftp_login(handler, user, passwd);
    TRACE(0, "[%s] ftp login returns %d", __func__, ret);
    if (ret == 0)
    {
        nv_record_set_ftp_config(server, port, user, passwd);
    }
out:
    if (ret < 0 && handler)
    {
        if (connected)
            ftp_disconnect(handler);
        ftp_client_free(handler);
        handler = NULL;
    }
    return handler;
}

/**
 * Connect ftp server with setttings from NV
 */
void *ftp_connect(void)
{
#if defined(__NET_FTP_CLIENT_SUPPORT__)
    char *server = "", *port = "", *user = "", *passwd = "";
    if (nv_record_get_ftp_config(&server, &port, &user, &passwd))
    {
        TRACE(0, "%s got ftp config: %s:%s@%s:%s", __func__, user, passwd, server, port);
    }
    else
    {
        TRACE(0, "%s got no ftp config", __func__);
        return NULL;
    }
    return ftp_connect_with(server, port, user, passwd);
#else
    return NULL;
#endif
}

void ftp_close(FTP_HANDLE handler)
{
    if (handler)
    {
        ftp_disconnect(handler);
        ftp_client_free(handler);
    }
}
int ftp_get_file(void *ftp_handler, char *fname)
{
    int ret = -1;
    FILE *fp = NULL;
    if (!ftp_handler)
        goto out;
    int recv_size = 0;
    uint32_t total_size = 0;

    char buffer[512];
    char *local_fname = fname + strlen(fname) - 1;
    while (local_fname >= fname && *local_fname != '/')
    {
        local_fname--;
    }
    // if(*local_fname == '/')local_fname++;
    fp = fopen(local_fname, "wb");
    if (!fp)
    {
        TRACE(0, "%s(%s) open file failed", __func__, local_fname);
        goto out;
    }

    if ((ret = ftp_client_notice_get_file(ftp_handler, fname)) != 0)
    {
        TRACE(0, "%s(%s) returns %d", __func__, fname, ret);
        goto out;
    }

    while (1)
    {
        memset(buffer, 0, 512);
        recv_size = fet_client_get_data(ftp_handler, buffer, 512);
        if (recv_size < 0)
        {
            TRACE(0, "%s get data failed: %d", __func__, recv_size);
            ret = recv_size;
            break;
        }
        if (recv_size > 0)
        {
            ret = fwrite(buffer, 1, recv_size, fp);
            if (ret < 0)
            {
                TRACE(0, "%s write file failed: %d, total recv: %d", __func__, ret, total_size);
                break;
            }
        }
        total_size += recv_size;
        if (recv_size < 512)
        {
            TRACE(0, "%s: recv finished, total=%d", __func__, total_size);
            ret = 0;
            break;
        }
    }
out:
    ftp_close(ftp_handler);
    if (fp)
    {
        fclose(fp);
        if (ret < 0)
            remove(local_fname); // download failed, remove stub file
    }
    return ret;
}

#define FTP_BLOCK_SIZE 2048
int ftp_put_file(void *ftp_handler, char *fname)
{
    if (!ftp_handler)return -1;
    FILE *fp = fopen(fname, "r");
    if(!fp)return -1;

    int ret = -1;
    int size = fseek(fp, 0, SEEK_END);
    TRACE(0, "%s(%s) size=%d", __func__, fname, size);
    fseek(fp, 0, SEEK_SET);


    if ((ret = ftp_client_notice(ftp_handler, fname)) != 0)
    {
        TRACE(0, "[%s] notice ftp server %s fail: %d\n", __func__, fname, ret);
        goto out;
    }


    uint8_t buf[FTP_BLOCK_SIZE] = {0};

    unsigned int start_time = TICKS_TO_MS(hal_sys_timer_get());
    // TRACE(0, "[%s] start_time : %d\n",__func__,start_time);
    int total = 0;
    while (1)
    {
        ret = fread(buf, 1, FTP_BLOCK_SIZE, fp);
        TRACE(0, "[%s] read_len : %d/%d",__func__, ret, FTP_BLOCK_SIZE);
        if(ret > 0){
            total += ftp_client_dumpdata(ftp_handler, (const char *)buf, ret);
        }
        if(ret < FTP_BLOCK_SIZE)break;
        osDelay(1);
    }
    TRACE(0, "%s(%s) done, total %d/%d bytes, %d ms\n", __func__, fname, total, size, TICKS_TO_MS(hal_sys_timer_get()) - start_time);
    ret = total;
out:
    if (fp)fclose(fp);
    return ret;
}

#endif
