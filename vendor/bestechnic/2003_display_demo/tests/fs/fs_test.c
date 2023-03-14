/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define LOG_TAG "FS"
#include "log.h"

#define LOG_E(fmt, ...) HILOG_ERROR(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) HILOG_INFO(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)

static void dir_test(const char *path)
{
    DIR *dir;
    struct dirent *dp;
    if ((dir = opendir(path)) == NULL) {
        LOG_E("opendir %s failed, %s\n", path, strerror(errno));
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        struct stat st_buf = {0};
        char realpath[260];
        snprintf(realpath, sizeof(realpath), "%s/%s", path, dp->d_name);
        if (stat(realpath, &st_buf) != 0) {
            LOG_E("can not access %s\n", realpath);
            closedir(dir);
            return;
        }
        if ((st_buf.st_mode & S_IFMT) == S_IFDIR) {
            LOG_I("DIR %s\n", realpath);
        } else {
            LOG_I("FILE %s, %ld bytes\n", realpath, st_buf.st_size);
        }
    }
    closedir(dir);
}

static void read_test(const char *file, bool print_str)
{
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        LOG_E("open file '%s' failed, %s\r\n", file, strerror(errno));
        return;
    }
    int bytes = 0;
    char buf[513];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int rc = read(fd, buf, sizeof(buf) - 1);
        if (rc > 0)
            bytes += rc;

        if (print_str) {
            buf[rc] = '\0';
            LOG_I("%s", buf);
        }

        if (rc < sizeof(buf) - 1)
            break;
    }
    close(fd);
    LOG_I("read file '%s' total bytes: %d\r\n", file, bytes);
}

static void fread_test(const char *file, bool print_str)
{
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        LOG_E("fopen file '%s' failed, %s\r\n", file, strerror(errno));
        return;
    }
    int bytes = 0;
    char buf[513];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int rc = fread(buf, 1, sizeof(buf) - 1, fp);
        if (rc > 0)
            bytes += rc;

        if (print_str) {
            buf[rc] = '\0';
            LOG_I("%s", buf);
        }
        if (rc < sizeof(buf) - 1)
            break;
    }
    fclose(fp);
    LOG_I("fread file '%s' total bytes: %d\r\n", file, bytes);
}

static void fwrite_test(const char *file, const char *data)
{
    FILE *fp = fopen(file, "w");
    if (fp == NULL) {
        LOG_E("fopen file '%s' failed, %s\r\n", file, strerror(errno));
        return;
    }
    int bytes = fwrite(data, 1, strlen(data), fp);
    fclose(fp);
    LOG_I("fwrite file '%s' total bytes: %d, %s\r\n", file, bytes, data);
}

static void fstat_test(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOG_E("open file '%s' failed, %s\r\n", path, strerror(errno));
        return;
    }
    struct stat st_buf = {0};
    if (fstat(fd, &st_buf) != 0) {
        LOG_E("can not access %s\n", path);
        close(fd);
        return;
    }
    close(fd);
    if ((st_buf.st_mode & S_IFMT) == S_IFDIR) {
        LOG_I("DIR %s\n", path);
    } else {
        LOG_I("FILE %s, %ld bytes\n", path, st_buf.st_size);
    }
}

static void fseek_test(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        LOG_E("fopen file '%s' failed, %s\r\n", path, strerror(errno));
        return;
    }
    fseek(fp, 0, SEEK_END);
    uint32_t len = ftell(fp);
    LOG_I("%s size %u bytes", path, len);
    fclose(fp);
}

void fs_test(void)
{
    dir_test("/data");
    read_test("/data/test.txt", true);
    fwrite_test("/data/test.txt", "fwrite data test");
    fread_test("/data/test.txt", true);
    fstat_test("/data/font.ttf");
    fseek_test("/data/font.ttf");
}
