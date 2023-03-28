#include "lfs.h"
#include "lfs_adapt.h"
#include "lfs_file_ops.h"
#include <stdlib.h>
#include "hal_trace.h"

#ifdef LFS_READ_FILE_AND_PLAY_TEST
typedef(int) (*lfs_read_file_and_play_write_cb_t)(uint8_t *buf, uint32_t len, void *user_arg);
typedef(void *) (*lfs_read_file_and_play_open_cb_t)(void *user_arg);
typedef(int) (*lfs_read_file_and_play_close_cb_t)(void *user_arg);

typedef struct
{
    lfs_read_file_and_play_open_cb_t lfs_read_file_and_play_open_cb;
    lfs_read_file_and_play_close_cb_t lfs_read_file_and_play_close_cb;
    lfs_read_file_and_play_write_cb_t lfs_read_file_and_play_write_cb;
} lfs_read_file_and_play_opt_t;

static lfs_read_file_and_play_opt_t lfs_read_file_and_play_opt;

void lfs_read_file_play_register_opt(lfs_read_file_and_play_open_cb_t open_cb, lfs_read_file_and_play_close_cb_t close_cb, lfs_read_file_and_play_write_cb_t write_cb)
{
    lfs_read_file_and_play_opt.lfs_read_file_and_play_open_cb = open_cb;
    lfs_read_file_and_play_opt.lfs_read_file_and_play_close_cb = close_cb;
    lfs_read_file_and_play_opt.lfs_read_file_and_play_write_cb = write_cb;
}
#endif /* LFS_READ_FILE_AND_PLAY_TEST */

/*
static void log_lfs_info(struct lfs_info *info)
{
    TRACE(0, "[name] = [%s]\n", info->name);
    TRACE(0, "[size] = [%d]\n", info->size);
    if (info->type == LFS_TYPE_REG)
        TRACE(0, "[type] = [REG]\n");
    else if (info->type == LFS_TYPE_DIR)
        TRACE(0, "[type] = [DIR]\n");
    else
        TRACE(0, "[type] unknow\n");
}
*/

static int lfs_test_demo(void)
{
    int ret = 0;
    struct lfs_config *my_lfs_cfg = NULL;
    lfs_t my_lfs;
    lfs_file_t my_lfs_file;

    const char *my_lfs_test_file_name = "my_lfs_test_file";
    uint32_t test_count = 0;

    lfs_device_open();
    my_lfs_cfg = lfs_get_config();

    ret = lfs_mount(&my_lfs, my_lfs_cfg);
    if (ret < 0)
    {
        TRACE(0, "lfs first mount error = %d, to format...", ret);
        ret = lfs_format(&my_lfs, my_lfs_cfg);
        if (ret < 0)
        {
            TRACE(0, "lfs_format error = %d", ret);
            return ret;
        }
        TRACE(0, "lfs foramt succ.");
        ret = lfs_mount(&my_lfs, my_lfs_cfg);
        if (ret < 0)
        {
            TRACE(0, "lfs mount after format error = %d", ret);
            return ret;
        }
    }
    TRACE(0, "lfs mount succ");

    ret = lfs_file_open(&my_lfs, &my_lfs_file, my_lfs_test_file_name, LFS_O_RDWR | LFS_O_CREAT);

    if (ret < 0)
    {
        TRACE(0, "lfs_file_open error. %d", ret);
        goto exit;
    }
    TRACE(0, "lfs open file %s succ", my_lfs_test_file_name);

    ret = lfs_file_read(&my_lfs, &my_lfs_file, &test_count, sizeof(test_count));
    if (ret < 0)
    {
        TRACE(0, "lfs_file_read error = %d, close file.", ret);
        lfs_file_close(&my_lfs, &my_lfs_file);
        goto exit;
    }
    TRACE(0, "lfs read file succ. read test_count = %d", test_count);

    test_count += 1;
    lfs_file_rewind(&my_lfs, &my_lfs_file);
    ret = lfs_file_write(&my_lfs, &my_lfs_file, &test_count, sizeof(test_count));
    if (ret < 0)
    {
        TRACE(0, "lfs_file_write error = %d, close file", ret);
        lfs_file_close(&my_lfs, &my_lfs_file);
        goto exit;
    }
    TRACE(0, "lfs write file succ. write test_count = %d", test_count);

    lfs_file_close(&my_lfs, &my_lfs_file);
    TRACE(0, "lfs close file succ.");

exit:
    ret = lfs_unmount(&my_lfs);
    if (ret < 0)
    {
        TRACE(0, "lfs_unmount error = %d", ret);
    }
    else
    {
        TRACE(0, "lfs unmount succ.");
    }
    lfs_device_close();
    return ret;
}

#if 0
static int lfs_read_tg_data_dir(void)
{
    int ret = 0;
    struct lfs_config *my_lfs_cfg = NULL;
    lfs_t my_lfs;
    lfs_file_t my_lfs_file;
    lfs_dir_t my_lfs_dir;
    struct lfs_info my_lfs_info;

    TRACE(0, "%s", __func__);
#ifdef LFS_READ_FILE_AND_PLAY_TEST
    void *lfs_read_file_and_play_handle = NULL;
    void *lfs_read_file_and_play_open_arg = NULL;
    if (lfs_read_file_and_play_opt.lfs_read_file_and_play_open_cb)
    {
        lfs_read_file_and_play_handle = lfs_read_file_and_play_opt.lfs_read_file_and_play_open_cb(lfs_read_file_and_play_open_arg);
    }
#endif /* LFS_READ_FILE_AND_PLAY_TEST */

    lfs_device_open();
    my_lfs_cfg = lfs_get_config();

    ret = lfs_mount(&my_lfs, my_lfs_cfg);
    if (ret < 0)
    {
        TRACE(0, "lfs mount err = %d", ret);
        return ret;
    }

    ret = lfs_dir_open(&my_lfs, &my_lfs_dir, "/");
    if (ret < 0)
    {
        TRACE(0, "lfs dir open err =  %d", ret);
        return ret;
    }

    TRACE(0, "open /");

    while (1)
    {
        ret = lfs_dir_read(&my_lfs, &my_lfs_dir, &my_lfs_info);
        if (ret <= 0)
        {
            break;
        }
        log_lfs_info(&my_lfs_info);
    }

    ret = lfs_dir_close(&my_lfs, &my_lfs_dir);
    if (ret < 0)
    {
        TRACE(0, "dir close err");
    }
    TRACE(0, "\n");

    ret = lfs_dir_open(&my_lfs, &my_lfs_dir, "/prompt");
    if (ret < 0)
    {
        TRACE(0, "lfs dir open /prompt err =  %d", ret);
        return ret;
    }

    TRACE(0, "open /prompt");

    while (1)
    {
        ret = lfs_dir_read(&my_lfs, &my_lfs_dir, &my_lfs_info);
        if (ret <= 0)
        {
            break;
        }
        log_lfs_info(&my_lfs_info);
    }

    ret = lfs_dir_close(&my_lfs, &my_lfs_dir);
    if (ret < 0)
    {
        TRACE(0, "dir close err");
    }
    TRACE(0, "\n");

    ret = lfs_file_open(&my_lfs, &my_lfs_file, "/prompt/1.mp3", LFS_O_RDWR);
    if (ret < 0)
    {
        TRACE(0, "open /prompt/1.mp3 failed");
        return ret;
    }
    TRACE(0, "open /prompt/1.mp3 succ");

    // uint8_t tmp[30*1024] = {0};
    uint8_t *tmp = (uint8_t *)malloc(30 * 1024);
    if (!tmp)
    {
        TRACE(0, "calloc error");
    }
    ret = lfs_file_read(&my_lfs, &my_lfs_file, tmp, 30 * 1024);
    if (ret < 0)
    {
        TRACE(0, "read /prompt/1.mp3 failed");
    }
    TRACE(0, "open /prompt/1.mp3 succ read_len = %d, and play it...", ret);

#ifdef LFS_READ_FILE_AND_PLAY_TEST
    if (lfs_read_file_and_play_opt.lfs_read_file_and_play_write_cb && lfs_read_file_and_play_handle)
    {
        lfs_read_file_and_play_opt.lfs_read_file_and_play_write_cb(tmp, ret, lfs_read_file_and_play_handle);
    }
#endif /* LFS_READ_FILE_AND_PLAY_TEST */

    free(tmp);

    ret = lfs_file_close(&my_lfs, &my_lfs_file);
    if (ret < 0)
    {
        TRACE(0, "close /prompt/1.mp3 failed");
        return ret;
    }
    TRACE(0, "close /prompt/1.mp3 succ");

#ifdef LFS_READ_FILE_AND_PLAY_TEST
    if (lfs_read_file_and_play_opt.lfs_read_file_and_play_close_cb && lfs_read_file_and_play_handle)
    {
        lfs_read_file_and_play_opt.lfs_read_file_and_play_close_cb(lfs_read_file_and_play_handle);
    }
#endif /* LFS_READ_FILE_AND_PLAY_TEST */

    ret = lfs_unmount(&my_lfs);
    if (ret < 0)
    {
        TRACE(0, "unmount err");
    }
    return ret;
}

static int lfs_file_ops_test(void)
{
    lfs_file_ops_first_call();
    void *fh = NULL;
    int i = 0;
    int ret = 0;

    uint8_t *write_buf = (uint8_t *)calloc(1024, sizeof(uint8_t));
    if (!write_buf)
    {
        return -1;
    }

    uint8_t *read_buf = (uint8_t *)calloc(1024, sizeof(uint8_t));
    if (!read_buf)
    {
        free(write_buf);
        return -1;
    }

    fh = lfs_file_ops_create_file("/file_test", 0);
    if (!fh)
    {
        free(write_buf);
        free(read_buf);
        return -1;
    }

    for (i = 0; i < 1024; i++)
    {
        write_buf[i] = i;
    }

    ret = lfs_file_ops_write_file(fh, write_buf, 1024);
    TRACE(0, "lfs_file_ops_write_file ret = %d", ret);

    ret = lfs_file_ops_read_file(fh, read_buf, 1024);
    TRACE(0, "lfs_file_open_read_fiel ret = %d", ret);

    TRACE(0, "compare read_buf and write_buf");
    ret = 0;
    for (i = 0; i < 1024; i++)
    {
        if (read_buf[i] != write_buf[i])
        {
            ret = -1;
            break;
        }
    }
    if (ret == -1)
    {
        TRACE(0, "read_buf[%d] = %0x, write_buf[%d] = %0x, error", i, read_buf[i], i, write_buf[i]);
    }
    else
    {
        TRACE(0, "read_buf == write_buf, succ");
    }

    free(write_buf);
    free(read_buf);

    ret = lfs_file_ops_close_file(fh);
    TRACE(0, "lfs_file_ops_close_file ret %d", ret);
}
#endif

int bes_littlefs_test(void)
{
    return lfs_test_demo();
    // return lfs_read_tg_data_dir();
    // return lfs_file_ops_test();
}