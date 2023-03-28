#include <stdio.h>
#include <sys/dirent.h>
#include "hal_trace.h"

#define POSIX_OPENDIR_TEST 1
#define POSIX_FREAD_TEST 1
#define POSIX_FSEEK_TEST 1
int posix_fs_api_test(void)
{
    /* mount test */
    TRACE(0, "#####         MOUNT TEST BEGIN    #########");
    int ret = mount_flash();
    if (ret < 0){
        TRACE(0, "#####         MOUNT TEST END FAILED     #########\n\n");
        return -1;
    }
    TRACE(0, "#####         MOUNT TEST END SUCCESS     #########\n\n");

#if POSIX_OPENDIR_TEST
    TRACE(0, "#####         OPENDIR TEST BEGIN    #########");
    DIR *dir = opendir("/data");
    if (dir == NULL) {
        TRACE(0, "#####         OPENDIR TEST END FAILED    #########");
        return -1;
    }
    TRACE(0, "#####         OPENDIR TEST END SUCCESS    #########\n\n");

    TRACE(0, "#####         READDIR TEST BEGIN    #########");
    struct dirent * dirent = readdir(dir);
    if (dirent == NULL){
        TRACE(0, "#####         READDIR TEST END FAILED    #########\n\n");
        return -1;
    } else {
        TRACE(0, "info: file num:%d type:%d name:%s",dirent->d_ino,dirent->d_type,dirent->d_name);
        TRACE(0, "#####         READDIR TEST END SUCEESS    #########\n\n");
    }

    TRACE(0, "#####         CLOSE TEST BEGIN    #########");
    closedir(dir);
    TRACE(0, "#####         CLOSE TEST END SUCCESS    #########\n\n");
#endif /* POSIX_OPENDIR_TEST */

    TRACE(0, "#####         FOPEN TEST BEGIN    #########");
    FILE *file = fopen("/data/test_suite_aes.cbc.datax","rb");
    if (file == NULL)
    {
        TRACE(0, "#####         FOPEN TEST END FAILED    #########\n\n");
        return -1;
    }
    TRACE(0, "#####         FOPEN TEST END SUCCESS      ######### \n\n");

    TRACE(0, "#####         FREAD TEST BEGIN    #########");
    TRACE(0, " file position :  %d",ftell(file));
#if POSIX_FREAD_TEST
    char buf[50] = {0};
    size_t size = fread(buf,sizeof(buf),1,file);
    if (size < 0)
    {
        TRACE(0, "#####         FOPEN TEST END FAILED    #########\n\n");
        return -1;
    }
    TRACE(0, "pos:%d    buf:%s",ftell(file),buf);
    TRACE(0, "#####         FREAD TEST END SUCCESS        #########\n\n");
#endif
    TRACE(0, "#####         FGETS TEST BEGIN    #########");

    char buf_fgets[50] = {0};
    char * p = fgets (buf_fgets, sizeof(buf_fgets), file);
    TRACE(0, "pos:%d    fgets:%s",ftell(file),p);
    TRACE(0, "#####         FGETS TEST END    #########\n\n");

#if POSIX_FSEEK_TEST
    TRACE(0, "#####         FSEEK TEST BEGIN    #########");
    ret = fseek( file, 0, SEEK_END );
    TRACE(0, "ret:%d   pos:%d",ret,ftell(file));
    TRACE(0, "#####         FSEEK TEST END    #########\n\n");
#endif /* POSIX_FSEEK_TEST */

    TRACE(0, "#####         FCLOSE TEST BEGIN    #########");
    fclose(file);
    TRACE(0, "#####         FCLOSE TEST END    #########\n\n");


    file = fopen("/data/test_suite_aes.cbc.datax","r");
    while (!feof(file))
    {
        char * buf1_fgets[1024] = {0};
        char * p = fgets (buf1_fgets, sizeof(buf1_fgets), file);
        TRACE(0, "pos:%d    fgets line :%s",ftell(file),p);
    }
    fclose(file);

    return 1;
}