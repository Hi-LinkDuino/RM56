#ifndef __FTP_CLIENT_H__
#define __FTP_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FTPMAXSZ 1024*5
#define BUF32    32
#define BUF64    64
#define BUF128      128
#define BUF512      512
#define BUF1024     1024
#define INITIALISE 0

#define DEFAULT_USERNAME    "anonymous"
#define DEFAULT_PASSWORD    "anonymous@example.com"

struct ftp_ops{
    int socket_fd;
    int datasocket_fd;
    char server_ip[BUF32];
    char dataserver_ip[BUF32];
    int data_port;
    char path_file_name[BUF128];
};

enum FTP_ERROR_CODE {
    FTP_CONNECT_ERROR   = -1,
    FTP_SEND_CMD_ERROR  = -2,
    FTP_RECEV_MSG_ERROR = -3,
    FTP_PASV_IP_ERROR   = -4,
    FTP_DATA_SEND_ERROR = -5,
    FTP_PTR_NULL        = -6,
    FTP_SOCKET_ERROR    = -7,
    FTP_SUCCESS          = 0,
};

/*FTP RETURN CODE*/
/*
110    Restart marker reply.
120    Service ready in nnn minutes.
125    Data connection already open; transfer starting.
150    File status okay; about to open data connection.
200    Command okay.
202    Command not implemented, superfluous at this site.
211    System status, or system help reply.
212    Directory status.
213    File status.
214    Help message.
215    NAME system type.
220    Service ready for new user.
221    Service closing control connection.
225    Data connection open; no transfer in progress.
226    Closing data connection.
227    Entering Passive Mode <h1,h2,h3,h4,p1,p2>.
228    Entering Long Passive Mode.
229    Extended Passive Mode Entered.
230    User logged in, proceed.
250    Requested file action okay, completed.
257    "PATHNAME" created.
331    User name okay, need password.
332    Need account for login.
350    Requested file action pending further information.
421    Service not available, closing control connection.
425    Can't open data connection.
426    Connection closed; transfer aborted.
450    Requested file action not taken.
451    Requested action aborted. Local error in processing.
452    Requested action not taken.
500    Syntax error, command unrecognized.
501    Syntax error in parameters or arguments.
502    Command not implemented.
503    Bad sequence of commands.
504    Command not implemented for that parameter.
521    Supported address families are <af1, .., afn>
522    Protocol not supported.
530    Not logged in.
532    Need account for storing files.
550    Requested action not taken.
551    Requested action aborted. Page type unknown.
552    Requested file action aborted.
553    Requested action not taken.
554    Requested action not taken: invalid REST parameter.
555    Requested action not taken: type or struct mismatch.
*/
#define FTP_SERVER_RESP_150         150 /*File status okay; about to open data connection.*/
#define FTP_SERVER_RESP_200         200 /*Command okay.*/
#define FTP_SERVER_RESP_213         213 /*File status.*/
#define FTP_SERVER_RESP_220         220 /*Service ready for new user.*/
#define FTP_SERVER_RESP_230         230 /*User logged in, proceed.*/
#define FTP_SERVER_RESP_331         331 /*User name okay, need password.*/
#define FTP_SERVER_RESP_421         421 /*Service not available, closing control connection.*/
#define FTP_SERVER_RESP_INVALID     300 /*Not as expected*/



void *ftp_client_init(void);

int ftp_client_get_size(void *handle,const char *file_name);

int ftp_login(void *handle,const char *username,const char *password);

int ftp_connect_server(void *handle,const char *server_addr,int port);

void ftp_disconnect(void *handle);

int ftp_client_dumpdata(void *handle,const char *dump_data,int dump_len);

int ftp_client_notice(void *handle,const char *dump_file_name);

int ftp_client_notice_get_file(void *handle,const char *get_file_name);

int fet_client_get_data(void *handle,const char *get_data_buffer,int get_len);

void ftp_client_free(void *handle);

#ifdef __cplusplus
}
#endif

#endif