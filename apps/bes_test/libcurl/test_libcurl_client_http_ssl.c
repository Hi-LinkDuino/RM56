/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)
#include "curl/curl.h"
#include "threading_alt.h"
#endif
#include <string.h>
#include "hal_trace.h"
#define HTTP_POST_URL        "http://waphx.com/bbs/book_re.aspx"
#define HTTPS_URL            "https://192.168.0.106:446/m3u8_err.txt"

static size_t receive_hread_data(void *buffer, size_t size, size_t nitems, void *outstream)
{

    TRACE(0, "%s", (char *)buffer);
    char *content_type = NULL;
    char *key = strtok(buffer, ":");
    if (!strncmp(key,"Content-Type",sizeof("Content-Type")))
    {
        content_type = strtok(NULL,":");
        TRACE(0, "%s",content_type);
    }
    return nitems;
}

static size_t receive_write_textdata(void *buffer, size_t size, size_t nitems, void *outstream)
{
    TRACE(0, "%s", (char *)buffer);
    return nitems;
}

int bes_curl_post_test(void)
{
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)

    CURLcode return_code = curl_global_init(CURL_GLOBAL_ALL);
    if (return_code != CURLE_OK)
    {
        TRACE(0,"curl_global_init() failed: %s\n",curl_easy_strerror(return_code));
        return -1;
    }

    /* get a curl handle */
    CURL *easy_handle = curl_easy_init();
    if (NULL == easy_handle)
    {
        curl_global_cleanup();
        return -1;
    }
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(easy_handle, CURLOPT_URL, HTTP_POST_URL);
    /* Now specify the POST data */
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, "action=class&id=65315&siteid=1000&classid=935&page=1&lpage=1&ot=&go=83891");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(easy_handle,CURLOPT_FORBID_REUSE,1L);
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);                       /* open redirect url option*/
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, receive_hread_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, receive_write_textdata);
    char * stream = NULL;
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stream);
    /* Perform the request, res will get the return code */
    return_code = curl_easy_perform(easy_handle);
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }

    /* Check for errors */
    if (return_code != CURLE_OK)
    {
        TRACE(0, "curl_easy_perform() failed: %s\n",curl_easy_strerror(return_code));
    }
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
#endif
    TRACE(0, "%s done",__func__);
    return 0;
}
#define ca_path     "/data/ca.crt"
int bes_curl_https_redirect_test(void)
{
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)
    const char* https_redirect_curl_url = "https://192.168.0.106:446/m3u8_err.txt";
    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != return_code)
    {
        TRACE( 0,"curl_global_init() failed: %s\n",curl_easy_strerror(return_code));
        return -1;
    }

    CURL *easy_handle = curl_easy_init();
    if (NULL == easy_handle)
    {
        curl_global_cleanup();
        return -1;
    }

    char * stream = NULL;
    curl_easy_setopt(easy_handle,CURLOPT_URL,https_redirect_curl_url);
    curl_easy_setopt(easy_handle, CURLOPT_HTTPGET, 1L);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(easy_handle,CURLOPT_FORBID_REUSE,1L);
    curl_easy_setopt(easy_handle, CURLOPT_TCP_KEEPALIVE, 1L);
    /* if CA Exits ,enable verify ca and host ,add Root CA Info */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);                    /* NO CA ,disable verify ca */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYHOST, 1L);                    /* NO CA ,disable verify host */
//  curl_easy_setopt(easy_handle, CURLOPT_BES_CERTDATA, root_ca);                    /* Disable verify ca,ignore root ca string */
//  curl_easy_setopt(easy_handle, CURLOPT_BES_CERTDATASIZE, root_ca_len);            /* Disable verify ca,ignore root ca string length */
	curl_easy_setopt(easy_handle, CURLOPT_CAINFO, ca_path);
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);                       /* open redirect url option*/
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, receive_hread_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, receive_write_textdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stream);
    return_code = curl_easy_perform(easy_handle);
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }

    if (CURLE_OK != return_code)
    {
        TRACE( 0,"curl_easy_perform() failed: %s\n",curl_easy_strerror(return_code));
    }
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
#endif
    TRACE(0, "%s done",__func__);
    return 0;
}

#define TEST_CA_CRT_RSA_SHA256_PEM 					\
"-----BEGIN CERTIFICATE-----\r\n" \
"MIID0DCCArigAwIBAgICQngwDQYJKoZIhvcNAQELBQAwczELMAkGA1UEBhMCQ04x\r\n" \
"EDAOBgNVBAgMB1RpYW5qaW4xEDAOBgNVBAcMB1RpYW5qaW4xFTATBgNVBAoMDENI\r\n" \
"SU5BU1NMIEluYzEpMCcGA1UEAwwgQ0hJTkFTU0wgQ2VydGlmaWNhdGlvbiBBdXRo\r\n" \
"b3JpdHkwHhcNMjIwNzAzMDk0OTU2WhcNMjMwNzAzMDk0OTU2WjB2MQswCQYDVQQG\r\n" \
"EwJDTjESMBAGA1UECAwJZ3Vhbmdkb25nMQ4wDAYDVQQKDAViZXNmZDELMAkGA1UE\r\n" \
"CwwCSVQxFjAUBgNVBAMMDXd3dy5iZXNmZC5jb20xHjAcBgkqhkiG9w0BCQEWDzEy\r\n" \
"NDYzOTU1QHFxLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJ3I\r\n" \
"KWMVpesOFytGd+SvuKouTYeaUGeQANCcPslc4R8AB0Qqr93xnArTYr49APOR24ym\r\n" \
"hhBux+ojMUksuNBlcgCqtyVKHws6J18l6wArvuF6lQx1s+nYeoVImRvARffwUDrg\r\n" \
"q0wp1ihEKl/h+4jovk+TZSX5LeIDsxQvvDzns8GTOTEVsz10eYeMMdUQJVVdeOzv\r\n" \
"dsOgZzFkh73Ciwa1EAPSLMfhNTsrM7W21QTTHB6FRnW5IpjIHTZHHlYzsSOinQug\r\n" \
"anqNo9ElGPfGc4XRoJAMxSDfyNoVqrtuRFDCGTxqwhcfTGeIumt3KR+C8z/BvFqe\r\n" \
"eNcE+hBbVNASrg61nNcCAwEAAaNrMGkwHwYDVR0jBBgwFoAUXB8j+sjhITHC2Df2\r\n" \
"iPzSb8JUQzMwCQYDVR0TBAIwADALBgNVHQ8EBAMCBPAwLgYDVR0RBCcwJYIJbG9j\r\n" \
"YWxob3N0ggx3d3cudGVzdC5jb22HBMCoAAGHBH8AAAEwDQYJKoZIhvcNAQELBQAD\r\n" \
"ggEBAGKb+1JKkDHc/V6Xv5XYB6C65R4ghDSFGazipWZMhMhaDv+lMWs4s5Jl4NkB\r\n" \
"i1Cn68OzSk+qb8EgInffQN7fTxLc8BNoyVRuL1yinuWsL4sZ7T1T3CF8WsmJ0E26\r\n" \
"39DsiCiPIaLIeGxuRNEJ6p0c9BZaFLqUvqHI6RMbETDWeFd60oRB13fQ9SesLfhQ\r\n" \
"3UBve2i1g3bcK95lwm/PbsBiUqs/cxl+h498rfZn+TRD9iV5FFjFWiVKC53ircTp\r\n" \
"f0Qro0lHb15kBZZ8TTg7sNBAUtMZuHklNrlzNdmSPcWyT+v+QSleZXV+tuYJIRnc\r\n" \
"EDVEenUatA/P7Y03LvHC2hq+/y0=\r\n" \
"-----END CERTIFICATE-----\r\n" 

#if 0
/* This is taken from bes server. */
/* BEGIN FILE string macro TEST_CA_CRT_RSA_SHA256_PEM bes server ca.crt */
#define TEST_CA_CRT_RSA_SHA256_PEM                                         \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIFLTCCAxUCFFqzazYz61BFwFUg1lBtlg9lM6T+MA0GCSqGSIb3DQEBCwUAMFMx\r\n" \
    "CzAJBgNVBAYTAkNOMREwDwYDVQQIDAhzaGFuZ2hhaTEPMA0GA1UEBwwGcHVkb25n\r\n" \
    "MQwwCgYDVQQKDANiZXMxEjAQBgNVBAMMCWJlcyBodHRwczAeFw0yMDAzMDcxMjEy\r\n" \
    "MzVaFw0zMDAzMDUxMjEyMzVaMFMxCzAJBgNVBAYTAkNOMREwDwYDVQQIDAhzaGFu\r\n" \
    "Z2hhaTEPMA0GA1UEBwwGcHVkb25nMQwwCgYDVQQKDANiZXMxEjAQBgNVBAMMCWJl\r\n" \
    "cyBodHRwczCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBANo+P0CkVtXt\r\n" \
    "Q0UZlMkI/m0s/wu4H4ZxfYMVLsKWSN2EOnqEm7xomQrJ9AbOydK66ZjljewUqEnE\r\n" \
    "/eL9XZa7WlwCoJzoEh3vgLnM4XIQGhpHDfRrzj5h9uIn5DNAOlspapg1sGmYPavs\r\n" \
    "CRcu+NQInF0/mD4JmLEX9ZEL1dHultvj1w/DFBw+TRY+aIsLfTFWblOq3yMDq6zL\r\n" \
    "quhc7GSxKFEcQ+6RTgAZhrXNNsNMc9XjGtPs+rR0okb42k37s+oWz048cP3pKeU7\r\n" \
    "d1L6u/Eh3aly11tEOGd8dSfnMKfZppy3Wnuei6Bp037AtsxnUoZRYsOAiQ+eHWoA\r\n" \
    "Yikj8ewjfXU0hAsFoibNKPjKYZM+vb+9LNAd/+O0u49gl4+CXKHt0TeONt3+b+ub\r\n" \
    "tydCB/ooNROHebLjveXsMafoi/w2XFt479EO0eVCrpCGOipCkAF/srVDl8mrKaix\r\n" \
    "AqCZBkC2o/VvS/ThdXz3egCouSzlzY9K2DabOfWfcrZlDZS7nc4n8QO+pMRlF4aX\r\n" \
    "imrOMjw2x75baiugbMpG+o3SRewesf3V3CF4FaPD0UT/Al7UC0d7QSwVzDk9nLwi\r\n" \
    "CH2+SnW3/Mwqap8j+LkinvC6xIBFk+kuRNblnj4hrXD/CUo9thk1ON8khLBH8Cob\r\n" \
    "uxD6aQWyQToo8MPm+lGZWJyuP/PVG0I1AgMBAAEwDQYJKoZIhvcNAQELBQADggIB\r\n" \
    "ABf+j9s1u3BmmvpPJBR7R4n6k/cMZVkvMdmtkekBhXYa+i/UYs5x7syCC5hY5+6f\r\n" \
    "cl+HWLsECmoojfxGRp6xvUI/c0YPfk23VX7n/Sy8CwzuWtvdg/PoNtutv8KMOUMN\r\n" \
    "tcEyBkkDH4zBTFJToYLnpXVVewXhnjQ79zZ8SNKaqahqNxY1XUKf+3wonOAC75KA\r\n" \
    "eom4qXB+7vnRfAbiS8OIC84i0iYTNG3+UOYPumqtfReCQlwJ6XjzQxotZKAf96a9\r\n" \
    "BY0UB+RQurcUohwh84oRVBBfRvWqKWLVCo32m/Uc1vjINrcWmL2tVZSolEendwYQ\r\n" \
    "mS3IYH6WglFg3qCi2Ret4z5beawfAqbbJEbfo9vnLdQBo1Nj+4g7F7I9ylWYW2OA\r\n" \
    "Ut5ybmewDmqmuXNWSCLjF03H3u6aBQ4vNRvXdXU5gvho7QC47a0/gBwsJkAwM44n\r\n" \
    "b/M7HWWbHkTNaCY7jjoYLoNYw0lCFB4VLEZrM3tMdI7u9cRUcocP7C7c2lxtrSlZ\r\n" \
    "gDU5r2znQLVEh47ulJlG7Xuf2ieUrjJyNSwSIHQlrGWmLH+2wVwRf0R48r2d7qTS\r\n" \
    "YDH9JLWTKIDY8tk40eWvKQVdWln1antDKhv9N89OhaTiraDlFOg93PjyCNl+SfNF\r\n" \
    "tnyzY3kGCBcTeFCY06cJG8TRK4o5/LcnsSWVKz8XQTyt\r\n"                     \
    "-----END CERTIFICATE-----\r\n"
/* END FILE */
#endif
static const char root_ca[] =
    TEST_CA_CRT_RSA_SHA256_PEM
    "";
static const size_t root_ca_len = sizeof(root_ca);
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)
static int my_trace(CURL *handle, curl_infotype type,char *data, size_t size,void *userp)
{
    const char *text = NULL;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
        default:
            break;
    }

    if (type == CURLINFO_DATA_IN || type == CURLINFO_SSL_DATA_IN || type == CURLINFO_SSL_DATA_OUT || type == CURLINFO_DATA_OUT )
    {
        return 0;
    }

    TRACE(0, "%s %s",text,data);
    return 0;
}
#endif

int bes_curl_https_test(void)
{
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)

    mbedtls_threading_set_alt(mbedtlsThreading_MutexInit,mbedtlsThreading_MutexFree,mbedtlsThreading_MutexLock,mbedtlsThreading_MutexUnlock);

    /* get request media url */
    CURLcode return_code = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != return_code)
    {
        TRACE(0, "curl_global_init() failed: %s ", curl_easy_strerror(return_code));
        return -1;
    }

    CURL *easy_handle = curl_easy_init();
    if (NULL == easy_handle)
    {
        curl_global_cleanup();
        return -1;
    }
    char *outstream = NULL;
    curl_easy_setopt(easy_handle, CURLOPT_URL, HTTPS_URL);
    curl_easy_setopt(easy_handle, CURLOPT_HTTPGET, 1L);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(easy_handle, CURLOPT_FORBID_REUSE, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_RANGE, "0-");
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(easy_handle, CURLOPT_BES_CERTDATA, root_ca);		  /* CA string not Root CA File modify by BES */
    curl_easy_setopt(easy_handle, CURLOPT_BES_CERTDATASIZE, root_ca_len); /* CA string length add by BES */
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, receive_hread_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, receive_write_textdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, outstream);
    return_code = curl_easy_perform(easy_handle);
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }
    if (CURLE_OK != return_code)
    {
        TRACE(0, "curl_easy_perform() failed: %s ", curl_easy_strerror(return_code));
    }
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
#endif
    return 0;
}
