#ifndef __TEST_LITTLEFS_H__
#define __TEST_LITTLEFS_H__

int bes_littlefs_test(void);

#ifdef LFS_READ_FILE_AND_PLAY_TEST
void lfs_read_file_play_register_opt(lfs_read_file_and_play_open_cb_t open_cb, lfs_read_file_and_play_close_cb_t close_cb, lfs_read_file_and_play_write_cb_t write_cb);
#endif

#endif