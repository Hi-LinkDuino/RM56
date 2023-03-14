/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Definition of source file for generating bytecode.
 * Create: 2020/09/07
 */

#ifdef JERRY_FOR_IAR_CONFIG

#include "generate-bytecode.h"

#include <string.h>

#include "config-gt.h"
#include "config-jupiter.h"

#define VERSION_LEN 30
#define ONETIME_MAX_OPTBYTES 4096 // max size for reading and writing at onetime

// jerry version code
char version_str[VERSION_LEN];

// secure functions
extern int memset_s(void *dest, size_t destMax, int c, size_t count);
extern int strcpy_s(char *strDest, size_t destMax, const char *strSrc);
extern int strcat_s(char *strDest, size_t destMax, const char *strSrc);
extern int strncat_s(char *strDest, size_t destMax, const char *strSrc, size_t count);
extern int sprintf_s(char *strDest, size_t destMax, const char *format, ...);
extern int strncpy_s(char *strDest, size_t destMax, const char *strSrc, size_t count);

#ifdef JERRY_IAR_JUPITER
extern uint8_t* input_buffer;
extern uint8_t* snapshot_buffer;
#endif // JERRY_IAR_JUPITER

/**
 * jerry snapshot format version
 */
char* get_jerry_version_no() {
  if (sprintf_s(version_str, sizeof(version_str),
      "JERRY_SNAPSHOT_VERSION_%u", JERRY_SNAPSHOT_VERSION) < 0) {
    return NULL;
  }
  return version_str;
} /* get_jerry_version_no */

/**
 * splice path and filename
 */
char* splice_path(char* str1, char* str2) {
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  int res_len = len1 + len2 + 1; // str1 + "/" + str2

  char* res = (char*)OhosMalloc(MEM_TYPE_JERRY, (res_len + 1) * sizeof(char));
  if (res == NULL) {
    return NULL;
  }
  if (memset_s(res, res_len + 1, 0, res_len + 1) != 0) {
    OhosFree(res);
    res = NULL;
    return NULL;
  }
  if (strcpy_s(res, len1 + 1, str1) != 0) {
    OhosFree(res);
    res = NULL;
    return NULL;
  }
  if ((strcat_s(res, len1 + strlen("/") + 1, "/") != 0)
       || (strcat_s(res, res_len + 1, str2) != 0)) {
    OhosFree(res);
    res = NULL;
    return NULL;
  }
  return res;
} /* splice_path */

/**
 * judge if is template(js/bc) file
 */
bool is_template_file(char* filename, char* template) {
  const char* pFile;
  pFile = strrchr(filename, '.');
  if ((pFile != NULL) && (strcmp(pFile, template) == 0)) {
    return true;
  }
  return false;
} /* is_template_file */

/**
 * get output snapshot file absolutely path
 */
char* get_output_file_path(char* input_file_path) {
  int len = strlen(input_file_path);
  char* output_file_path = (char*)OhosMalloc(MEM_TYPE_JERRY, (len + 1) * sizeof(char));
  if (output_file_path == NULL) {
    return NULL;
  }
  if (memset_s(output_file_path, len + 1, 0, len + 1) != 0) {
    OhosFree(output_file_path);
    output_file_path = NULL;
    return NULL;
  }
  if (strncpy_s(output_file_path, len, input_file_path, len - strlen(".js")) != 0) {
    OhosFree(output_file_path);
    output_file_path = NULL;
    return NULL;
  }
  output_file_path[len-3] = '.';
  output_file_path[len-2] = 'b';
  output_file_path[len-1] = 'c';
  output_file_path[len] = '\0';
  return output_file_path;
} /* get_output_file_path */

/**
 * read js bundle file by Fragement
 */
EXECRES read_js_file(char* filename, uint8_t* target_Js, int* file_bytesize) {
  int fd = 0;
  struct stat file_stat = { 0 };
  int remain_to_read = 0;
  int here_to_read = 0;
  int tmp_read = 0;
  int read_offset = 0;
  fd = open(filename, O_RDONLY, S_IREAD);
  if (fd < 0) {
    // Error: failed to open file
    return EXCE_ACE_JERRY_OPEN_FILE_FAILED;
  }
  if (fstat(fd, &file_stat) < 0) {
    close(fd);
    return EXCE_ACE_JERRY_GET_FILE_STAT_ERROR;
  }
  *file_bytesize = file_stat.st_size;
  if (*file_bytesize > INPUTJS_BUFFER_SIZE) {
    close(fd);
    return EXCE_ACE_JERRY_JSFILE_TOO_LARGE;
  }
  remain_to_read = *file_bytesize;
  while (remain_to_read > 0) {
    here_to_read = (remain_to_read > ONETIME_MAX_OPTBYTES) ?
                    ONETIME_MAX_OPTBYTES : remain_to_read;
    tmp_read = read(fd, target_Js + read_offset, here_to_read);
    if (tmp_read < 0 || tmp_read != here_to_read) {
      close(fd);
      // Error: failed to read file
      return EXCE_ACE_JERRY_READ_JSFILE_FAILED;
    }
    read_offset = read_offset + here_to_read;
    remain_to_read = remain_to_read - here_to_read;
  }
  if (read_offset != *file_bytesize) {
    close(fd);
    // Error: failed to successfully read file
    return EXCE_ACE_JERRY_READ_JSFILE_FAILED;
  }
  close(fd);
  return EXCE_ACE_JERRY_EXEC_OK;
} /* read_js_file */

/**
 * write snapshot file by Fragment
 */
EXECRES write_snapshot(char* output_file_name_path, size_t snapshot_size) {
  int fd = 0;
  int res = 0;
  int remain_to_write = 0;
  int here_to_write = 0;
  int write_offset = 0;

  fd = open(output_file_name_path, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
  if (fd < 0) {
    // Error: Unable to open snapshot file
    return EXCE_ACE_JERRY_OPEN_FILE_FAILED;
  }
  remain_to_write = snapshot_size;
  while (remain_to_write > 0) {
    here_to_write = (remain_to_write > ONETIME_MAX_OPTBYTES) ?
                    ONETIME_MAX_OPTBYTES : remain_to_write;
    res = write(fd, snapshot_buffer + write_offset, here_to_write);
    if (res <= 0 || res != here_to_write) {
      close(fd);
      // Error: Unable to write snapshot file
      return EXCE_ACE_JERRY_WRITE_SNAPSHOT_FILE_FAILED;
    }
    write_offset = write_offset + here_to_write;
    remain_to_write = remain_to_write - here_to_write;
  }
  if (write_offset != snapshot_size) {
    close(fd);
    // Error: Unable to successfully write snapshot file
    return EXCE_ACE_JERRY_WRITE_SNAPSHOT_FILE_FAILED;
  }
  close(fd);
  return EXCE_ACE_JERRY_EXEC_OK;
} /* write_snapshot */

/**
 * struct for Directory Node
 */
typedef struct Node {
  char* dir_name;
  struct Node* next;
} dir_node;

/**
 * free the memory for linkedlist
 */
void free_link(dir_node* head) {
  dir_node* tmp = NULL;
  while (head != NULL) {
    tmp = head;
    head = head->next;
    if (tmp->dir_name != NULL) {
      OhosFree(tmp->dir_name);
      tmp->dir_name = NULL;
    }
    OhosFree(tmp);
    tmp = NULL;
  }
} /* free_link */

/**
 * generate snapshot file
 */
EXECRES generate_snapshot_file(char* input_file, char* output_file) {
  uint8_t* target_Js = input_buffer;
  jerry_value_t generate_result;
  size_t snapshot_size = 0;
  EXECRES write_res = EXCE_ACE_JERRY_EXEC_OK;
  bool convert_state = false;
  int file_bytesize = 0;
  EXECRES read_res = EXCE_ACE_JERRY_EXEC_OK;

  if (input_file == NULL || output_file == NULL) {
    return EXCE_ACE_JERRY_NULL_PATH;
  }
  read_res = read_js_file(input_file, target_Js, &file_bytesize);
  if (read_res != EXCE_ACE_JERRY_EXEC_OK) {
    return read_res;
  }

  generate_result = jerry_generate_snapshot (
    NULL,
    0,
    target_Js,
    file_bytesize,
    0,
    (uint32_t* )snapshot_buffer,
    SNAPSHOT_BUFFER_SIZE);

  convert_state = jerry_value_is_error(generate_result)
                  || !jerry_value_is_number(generate_result);
  if (convert_state) {
    // Error: Generating snapshot failed
    jerry_release_value(generate_result);
    return EXCE_ACE_JERRY_GENERATE_SNAPSHOT_FAILED;
  }
  snapshot_size = (size_t)jerry_get_number_value(generate_result);
  jerry_release_value(generate_result);

  write_res = write_snapshot(output_file, snapshot_size);
  if (write_res != EXCE_ACE_JERRY_EXEC_OK) {
    // Error: Writing snapshot file failed
    return write_res;
  }
  return EXCE_ACE_JERRY_EXEC_OK;
}/* generate_snapshot_file */

/**
 * traverse directory and do js to bytecode conversion ON IAR
 */
EXECRES walk_directory(char* filefolder) {
  EXECRES generate_val = EXCE_ACE_JERRY_EXEC_OK;
  DIR* dir;
  struct dirent* direntp;
  struct stat file_stat = { 0 };
  char* filename = NULL;
  char* current_path = NULL;
  char* input_file_path = NULL;
  char* output_file_path = NULL;
  char* start_folder = NULL;
  dir_node *head, *curr, *end, *new_node;
  int filefolder_len = strlen(filefolder) + 1;

  if ((filefolder == NULL) || (stat(filefolder, &file_stat) < 0)) {
    return EXCE_ACE_JERRY_INPUT_PATH_ERROR;
  }
  if ((start_folder = (char*)OhosMalloc(MEM_TYPE_JERRY, filefolder_len)) == NULL) {
    return EXCE_ACE_JERRY_MALLOC_ERROR;
  }
  if (strcpy_s(start_folder, filefolder_len, filefolder) != 0) {
    OhosFree(start_folder);
    start_folder = NULL;
    return EXCE_ACE_JERRY_INPUT_PATH_ERROR;
  }
  if ((head = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
    OhosFree(start_folder);
    start_folder = NULL;
    return EXCE_ACE_JERRY_LINKLIST_ERROR;
  }
  if ((end = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
    OhosFree(start_folder);
    start_folder = NULL;
    OhosFree(head);
    head = NULL;
    return EXCE_ACE_JERRY_LINKLIST_ERROR;
  }
  head->dir_name = NULL;
  head->next = end;
  end->dir_name = start_folder;
  end->next = NULL;

  while (head->next != NULL) {
    curr = head->next;
    current_path = curr->dir_name;
    if ((dir = (DIR*)opendir(current_path)) == NULL) {
      free_link(head);
      curr = NULL;
      end = NULL;
      return EXCE_ACE_JERRY_OPEN_DIR_FAILED;
    }
    while ((direntp = (struct dirent*)readdir(dir)) != NULL) {
      filename = direntp->d_name;
      if (strncmp(filename, ".", 1) == 0) {
        continue;
      }
      if ((input_file_path = splice_path(current_path, filename)) == NULL) {
        closedir(dir);
        free_link(head);
        curr = NULL;
        end = NULL;
        return EXCE_ACE_JERRY_SPLICE_PATH_ERROR;
      }
      if (stat(input_file_path, &file_stat) < 0) {
        closedir(dir);
        OhosFree(input_file_path);
        input_file_path = NULL;
        free_link(head);
        curr = NULL;
        end = NULL;
        return EXCE_ACE_JERRY_GET_FILE_STAT_ERROR;
      }
      if (file_stat.st_mode & S_IFDIR) {
        // file now is file folder
        if ((new_node = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
          closedir(dir);
          OhosFree(input_file_path);
          input_file_path = NULL;
          free_link(head);
          curr = NULL;
          end = NULL;
          return EXCE_ACE_JERRY_LINKLIST_ERROR;
        }
        // input_file_path for dir will be freed when that node is freed
        new_node->dir_name = input_file_path;
        new_node->next = NULL;
        end->next = new_node;
        end = new_node;
        new_node = NULL;
      } else if (is_template_file(filename, ".js")) {
        //filename now is .js file
        if ((output_file_path = get_output_file_path(input_file_path)) == NULL) {
          closedir(dir);
          OhosFree(input_file_path);
          input_file_path = NULL;
          free_link(head);
          curr = NULL;
          end = NULL;
          return EXCE_ACE_JERRY_SPLICE_OUTPUT_PATH_ERROR;
        }
        jerry_init_flag_t flags = JERRY_INIT_EMPTY;
        jerry_init (flags);
        generate_val = generate_snapshot_file(input_file_path, output_file_path);
        jerry_cleanup();
        OhosFree(output_file_path);
        output_file_path = NULL;
        OhosFree(input_file_path);
        input_file_path = NULL;
        if (generate_val != EXCE_ACE_JERRY_EXEC_OK) {
          closedir(dir);
          free_link(head);
          curr = NULL;
          end = NULL;
          return generate_val; // return error_code
        }
      } else {
        OhosFree(input_file_path);
        input_file_path = NULL;
      }
    }
    closedir(dir);
    head->next = curr->next;
    OhosFree(curr->dir_name);
    curr->dir_name = NULL;
    OhosFree(curr);
    curr = NULL;
  }
  OhosFree(head);
  head = NULL;
  end = NULL;
  return EXCE_ACE_JERRY_EXEC_OK;
} /* walk_directory */

/**
 * when convertion failed, traverse directory and delete all created bytecode files
 */
EXECRES walk_del_bytecode(char* filefolder) {
  DIR* dir;
  struct dirent* direntp;
  struct stat file_stat = { 0 };
  char* filename = NULL;
  char* current_path = NULL;
  char* input_file_path = NULL;
  char* start_folder = NULL;
  dir_node *head, *curr, *end, *new_node;
  int filefolder_len = strlen(filefolder) + 1;

  if ((filefolder == NULL) || (stat(filefolder, &file_stat) < 0)) {
    return EXCE_ACE_JERRY_INPUT_PATH_ERROR;
  }
  if ((start_folder = (char*)OhosMalloc(MEM_TYPE_JERRY, filefolder_len)) == NULL) {
    return EXCE_ACE_JERRY_MALLOC_ERROR;
  }
  if (strcpy_s(start_folder, filefolder_len, filefolder) != 0) {
    OhosFree(start_folder);
    start_folder = NULL;
    return EXCE_ACE_JERRY_INPUT_PATH_ERROR;
  }
  if ((head = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
    OhosFree(start_folder);
    start_folder = NULL;
    return EXCE_ACE_JERRY_LINKLIST_ERROR;
  }
  if ((end = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
    OhosFree(start_folder);
    start_folder = NULL;
    OhosFree(head);
    head = NULL;
    return EXCE_ACE_JERRY_LINKLIST_ERROR;
  }
  head->dir_name = NULL;
  head->next = end;
  end->dir_name = start_folder;
  end->next = NULL;

  while (head->next != NULL) {
    curr = head->next;
    current_path = curr->dir_name;
    if ((dir = (DIR*)opendir(current_path)) == NULL) {
      free_link(head);
      curr = NULL;
      end = NULL;
      return EXCE_ACE_JERRY_OPEN_DIR_FAILED;
    }
    while ((direntp = (struct dirent*)readdir(dir)) != NULL) {
      filename = direntp->d_name;
      if (strncmp(filename, ".", 1) == 0) {
        continue;
      }
      if ((input_file_path = splice_path(current_path, filename)) == NULL) {
        closedir(dir);
        free_link(head);
        curr = NULL;
        end = NULL;
        return EXCE_ACE_JERRY_SPLICE_PATH_ERROR;
      }
      if (stat(input_file_path, &file_stat) < 0) {
        closedir(dir);
        OhosFree(input_file_path);
        input_file_path = NULL;
        free_link(head);
        curr = NULL;
        end = NULL;
        return EXCE_ACE_JERRY_GET_FILE_STAT_ERROR;
      }
      if (file_stat.st_mode & S_IFDIR) {
        // file now is file folder
        if ((new_node = (dir_node*)OhosMalloc(MEM_TYPE_JERRY, sizeof(dir_node))) == NULL) {
          closedir(dir);
          OhosFree(input_file_path);
          input_file_path = NULL;
          free_link(head);
          curr = NULL;
          end = NULL;
          return EXCE_ACE_JERRY_LINKLIST_ERROR;
        }
        // input_file_path for dir will be freed when that node is freed
        new_node->dir_name = input_file_path;
        new_node->next = NULL;
        end->next = new_node;
        end = new_node;
        new_node = NULL;
      } else if ((is_template_file(filename, ".bc")) && (unlink(input_file_path) != F_OK)) {
        //file now is .bc file and unlink file failed
        closedir(dir);
        OhosFree(input_file_path);
        input_file_path = NULL;
        free_link(head);
        curr = NULL;
        end = NULL;
        return EXCE_ACE_JERRY_UNLINKFILE_ERROR;
      } else {
        OhosFree(input_file_path);
        input_file_path = NULL;
      }
    }
    closedir(dir);
    head->next = curr->next;
    OhosFree(curr->dir_name);
    curr->dir_name = NULL;
    OhosFree(curr);
    curr = NULL;
  }
  OhosFree(head);
  head = NULL;
  end = NULL;
  return EXCE_ACE_JERRY_EXEC_OK;
} /* walk_del_bytecode */

#endif // end JERRY_FOR_IAR_CONFIG
