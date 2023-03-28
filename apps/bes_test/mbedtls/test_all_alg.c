#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dirent.h>
#include "mbedtls/platform.h"

static char* buf[1024] = {0};

static int mbedtls_fprintf_alt( FILE *stream, const char *format, ... )
{
    va_list ap;
    va_start(ap, format); 
    int n = vsprintf(buf,format, ap); 
    printf(buf);
    va_end(ap); 
}

extern int main_suite_aes_cbc( int argc, const char *argv[] );
extern int main_suite_aes_cfb( int argc, const char *argv[] );
extern int main_suite_aes_ecb( int argc, const char *argv[] );
extern int main_suite_aes_ofb( int argc, const char *argv[] );
extern int main_suite_aes_rest( int argc, const char *argv[] );
extern int main_suite_aes_xts( int argc, const char *argv[] );
extern int main_suite_arc4( int argc, const char *argv[] );
extern int main_suite_aria( int argc, const char *argv[] );
extern int main_suite_asniwrite( int argc, const char *argv[] );
extern int main_suite_base64( int argc, const char *argv[] );
extern int main_suite_blowfish( int argc, const char *argv[] );
extern int main_suite_camellia( int argc, const char *argv[] );
extern int main_suite_ccm( int argc, const char *argv[] );
extern int main_suite_chacha20( int argc, const char *argv[] );
extern int main_suite_chachapoly( int argc, const char *argv[] );
extern int main_suite_cipher_aes( int argc, const char *argv[] );
extern int main_suite_cipher_arc4( int argc, const char *argv[] );
extern int main_suite_cipher_blowfish( int argc, const char *argv[] );
extern int main_suite_cipher_camellia( int argc, const char *argv[] );
extern int main_siote_cipher_ccm( int argc, const char *argv[] );
extern int main_suite_cipher_chacha20( int argc, const char *argv[] );
extern int main_suite_cipher_chachapoly( int argc, const char *argv[] );
extern int main_suite_cipher_desc( int argc, const char *argv[] );
extern int main_suite_cipher_gcm( int argc, const char *argv[] );
extern int main_suite_cipher_msic( int argc, const char *argv[] );
extern int main_suite_cipher_null( int argc, const char *argv[] );
extern int main_suite_cipher_padding( int argc, const char *argv[] );
extern int main_suite_cmac( int argc, const char *argv[] );
extern int main_suite_ctr_drbg( int argc, const char *argv[] );
extern int main_suite_debug( int argc, const char *argv[] );
extern int main_suite_des( int argc, const char *argv[] );
extern int main_suite_dhm( int argc, const char *argv[] );
extern int main_suite_ecdh( int argc, const char *argv[] );
extern int main_suite_ecdsa( int argc, const char *argv[] );
extern int main_suite_ecjpake( int argc, const char *argv[] );
extern int main_suite_ecp( int argc, const char *argv[] );
extern int main_suite_entropy( int argc, const char *argv[] );
extern int main_suite_error( int argc, const char *argv[] );
extern int main_suite_gcm_aes128_de( int argc, const char *argv[] );
extern int main_suite_gcm_aes128_en( int argc, const char *argv[] );
extern int main_suite_gcm_aes192_de( int argc, const char *argv[] );
extern int main_suite_gcm_aes192_en( int argc, const char *argv[] );
extern int main_suite_gcm_aes256_de( int argc, const char *argv[] );
extern int main_suite_gcm_aes256_en( int argc, const char *argv[] );
extern int main_suite_gcm_camellia( int argc, const char *argv[] );
extern int main_suite_gcm_misc( int argc, const char *argv[] );
extern int main_suite_hkdf( int argc, const char *argv[] );
extern int main_suite_hmac_drbg_misc( int argc, const char *argv[] );
extern int main_suite_hmac_drbg_no_reseed( int argc, const char *argv[] );
extern int main_suite_hmac_drbg_nopr( int argc, const char *argv[] );
extern int main_suite_hmac_drbg_pr( int argc, const char *argv[] );
extern int main_suite_md( int argc, const char *argv[] );
extern int main_suite_mdx( int argc, const char *argv[] );
extern int main_suite_memory_buffer_alloc( int argc, const char *argv[] );
extern int main_suite_mpi( int argc, const char *argv[] );
extern int main_suite_nist_kw( int argc, const char *argv[] );
extern int main_suite_pem( int argc, const char *argv[] );
extern int main_suite_pk( int argc, const char *argv[] );
extern int main_suite_pkcs1_v15( int argc, const char *argv[] );
extern int main_suite_pkcs1_v21( int argc, const char *argv[] );
extern int main_suite_pkcs5( int argc, const char *argv[] );
extern int main_suite_pkparse( int argc, const char *argv[] );
extern int main_suite_pkwrite( int argc, const char *argv[] );
extern int main_suite_policy1305( int argc, const char *argv[] );
extern int main_suite_rsa( int argc, const char *argv[] );
extern int main_suite_shax( int argc, const char *argv[] );
extern int main_suite_ssl( int argc, const char *argv[] );
extern int main_suite_timing( int argc, const char *argv[] );
extern int main_suite_version( int argc, const char *argv[] );
extern int main_suite_x509parse( int argc, const char *argv[] );
extern int main_suite_x509write( int argc, const char *argv[] );
extern int main_suite_xtea( int argc, const char *argv[] );



int bes_mbedtls_all_alg_test(void)
{
    int ret = mount_flash();
    mbedtls_platform_set_fprintf(mbedtls_fprintf_alt);
    printf("========= main_suite_aes_cbc begin =====\n\n");
    main_suite_aes_cbc(0,NULL);
    printf("========= main_suite_aes_cbc end =====\n\n");

    printf("========= main_suite_aes_cfb begin =====\n\n");
    main_suite_aes_cfb(0,NULL);
    printf("========= main_suite_aes_cfb end =====\n\n");

    printf("========= main_suite_aes_ecb begin =====\n\n");
    main_suite_aes_ecb(0,NULL);
    printf("========= main_suite_aes_ecb end =====\n\n");

    printf("========= main_suite_aes_ofb begin =====\n\n");
    main_suite_aes_ofb(0,NULL);
    printf("========= main_suite_aes_ofb end =====\n\n");

    printf("========= main_suite_aes_rest begin =====\n\n");
    main_suite_aes_rest(0,NULL);
    printf("========= main_suite_aes_rest end =====\n\n");

    // printf("========= main_suite_aes_xts begin =====\n\n");
    // main_suite_aes_xts(0,NULL);
    // printf("========= main_suite_aes_xts end =====\n\n");

    // printf("========= main_suite_arc4 begin =====\n\n");
    // main_suite_arc4(0,NULL);
    // printf("========= main_suite_arc4 end =====\n\n");

    // printf("========= main_suite_aria begin =====\n\n");
    // main_suite_aria(0,NULL);
    // printf("========= main_suite_aria end =====\n\n");

    printf("========= main_suite_asniwrite begin =====\n\n");
    main_suite_asniwrite(0,NULL);
    printf("========= main_suite_asniwrite end =====\n\n");

    // printf("========= main_suite_base64 begin =====\n\n");
    // main_suite_base64(0,NULL);
    // printf("========= main_suite_base64 end =====\n\n");

    // printf("========= main_suite_blowfish begin =====\n\n");
    // main_suite_blowfish(0,NULL);
    // printf("========= main_suite_blowfish end =====\n\n");

    // printf("========= main_suite_camellia begin =====\n\n");
    // main_suite_camellia(0,NULL);
    // printf("========= main_suite_camellia end =====\n\n");

    // printf("========= main_suite_ccm begin =====\n\n");
    // main_suite_ccm(0,NULL);
    // printf("========= main_suite_ccm end =====\n\n");

    // printf("========= main_suite_chacha20 begin =====\n\n");
    // main_suite_chacha20(0,NULL);
    // printf("========= main_suite_chacha20 end =====\n\n");

    printf("========= main_suite_chachapoly begin =====\n\n");
    main_suite_chachapoly(0,NULL);
    printf("========= main_suite_chachapoly end =====\n\n");

    printf("========= main_suite_cipher_aes begin =====\n\n");
    main_suite_cipher_aes(0,NULL);
    printf("========= main_suite_cipher_aes end =====\n\n");

    printf("========= main_suite_cipher_arc4 begin =====\n\n");
    main_suite_cipher_arc4(0,NULL);
    printf("========= main_suite_cipher_arc4 end =====\n\n");

    printf("========= main_suite_cipher_blowfish begin =====\n\n");
    main_suite_cipher_blowfish(0,NULL);
    printf("========= main_suite_cipher_blowfish end =====\n\n");

    printf("========= main_suite_cipher_camellia begin =====\n\n");
    main_suite_cipher_camellia(0,NULL);
    printf("========= main_suite_cipher_camellia end =====\n\n");

    printf("========= main_siote_cipher_ccm begin =====\n\n");
    main_siote_cipher_ccm(0,NULL);
    printf("========= main_siote_cipher_ccm end =====\n\n");

    printf("========= main_suite_cipher_chacha20 begin =====\n\n");
    main_suite_cipher_chacha20(0,NULL);
    printf("========= main_suite_cipher_chacha20 end =====\n\n");

    printf("========= main_suite_cipher_chachapoly begin =====\n\n");
    main_suite_cipher_chachapoly(0,NULL);
    printf("========= main_suite_cipher_chachapoly end =====\n\n");

    // printf("========= main_suite_cipher_desc begin =====\n\n");
    // main_suite_cipher_desc(0,NULL);
    // printf("========= main_suite_cipher_desc end =====\n\n");

    // printf("========= main_suite_cipher_gcm begin =====\n\n");
    // main_suite_cipher_gcm(0,NULL);
    // printf("========= main_suite_cipher_gcm end =====\n\n");

    // printf("========= main_suite_cipher_msic begin =====\n\n");
    // main_suite_cipher_msic(0,NULL);
    // printf("========= main_suite_cipher_msic end =====\n\n");

    // printf("========= main_suite_cipher_null begin =====\n\n");
    // main_suite_cipher_null(0,NULL);
    // printf("========= main_suite_cipher_null end =====\n\n");

    // printf("========= main_suite_cipher_padding begin =====\n\n");
    // main_suite_cipher_padding(0,NULL);
    // printf("========= main_suite_cipher_padding end =====\n\n");

    // printf("========= main_suite_cmac begin =====\n\n");
    // main_suite_cmac(0,NULL);
    // printf("========= main_suite_cmac end =====\n\n");

    // printf("========= main_suite_ctr_drbg begin =====\n\n");
    // main_suite_ctr_drbg(0,NULL);
    // printf("========= main_suite_ctr_drbg end =====\n\n");

    // printf("========= main_suite_debug begin =====\n\n");
    // main_suite_debug(0,NULL);
    // printf("========= main_suite_debug end =====\n\n");

    // printf("========= main_suite_des begin =====\n\n");
    // main_suite_des(0,NULL);
    // printf("========= main_suite_des end =====\n\n");

    // printf("========= main_suite_dhm begin =====\n\n");
    // main_suite_dhm(0,NULL);
    // printf("========= main_suite_dhm end =====\n\n");

    // printf("========= main_suite_ecdh begin =====\n\n");
    // main_suite_ecdh(0,NULL);
    // printf("========= main_suite_ecdh end =====\n\n");

    printf("========= main_suite_ecdsa begin =====\n\n");
    main_suite_ecdsa(0,NULL);
    printf("========= main_suite_ecdsa end =====\n\n");

    // printf("========= main_suite_ecjpake begin =====\n\n");
    // main_suite_ecjpake(0,NULL);
    // printf("========= main_suite_ecjpake end =====\n\n");

    // printf("========= main_suite_ecp begin =====\n\n");
    // main_suite_ecp(0,NULL);
    // printf("========= main_suite_ecp end =====\n\n");

    printf("========= main_suite_entropy begin =====\n\n");
    main_suite_entropy(0,NULL);
    printf("========= main_suite_entropy end =====\n\n");

    // printf("========= main_suite_error begin =====\n\n");
    // main_suite_error(0,NULL);
    // printf("========= main_suite_error end =====\n\n");

    // printf("========= main_suite_gcm_aes128_de begin =====\n\n");
    // main_suite_gcm_aes128_de(0,NULL);
    // printf("========= main_suite_gcm_aes128_de end =====\n\n");

    // printf("========= main_suite_gcm_aes128_en begin =====\n\n");
    // main_suite_gcm_aes128_en(0,NULL);
    // printf("========= main_suite_gcm_aes128_en end =====\n\n");

    // printf("========= main_suite_gcm_aes192_de begin =====\n\n");
    // main_suite_gcm_aes192_de(0,NULL);
    // printf("========= main_suite_gcm_aes192_de end =====\n\n");

    // printf("========= main_suite_gcm_aes192_en begin =====\n\n");
    // main_suite_gcm_aes192_en(0,NULL);
    // printf("========= main_suite_gcm_aes192_en end =====\n\n");

    // printf("========= main_suite_gcm_aes256_de begin =====\n\n");
    // main_suite_gcm_aes256_de(0,NULL);
    // printf("========= main_suite_gcm_aes256_de end =====\n\n");

    // printf("========= main_suite_gcm_aes256_en begin =====\n\n");
    // main_suite_gcm_aes256_en(0,NULL);
    // printf("========= main_suite_gcm_aes256_en end =====\n\n");

    // printf("========= main_suite_gcm_camellia begin =====\n\n");
    // main_suite_gcm_camellia(0,NULL);
    // printf("========= main_suite_gcm_camellia end =====\n\n");

    // printf("========= main_suite_gcm_misc begin =====\n\n");
    // main_suite_gcm_misc(0,NULL);
    // printf("========= main_suite_gcm_misc end =====\n\n");

    // printf("========= main_suite_hkdf begin =====\n\n");
    // main_suite_hkdf(0,NULL);
    // printf("========= main_suite_hkdf end =====\n\n");

    printf("========= main_suite_hmac_drbg_misc begin =====\n\n");
    // main_suite_hmac_drbg_misc(0,NULL);
    printf("========= main_suite_hmac_drbg_misc end =====\n\n");

    printf("========= main_suite_hmac_drbg_no_reseed begin =====\n\n");
    main_suite_hmac_drbg_no_reseed(0,NULL);
    printf("========= main_suite_hmac_drbg_no_reseed end =====\n\n");

    printf("========= main_suite_hmac_drbg_nopr begin =====\n\n");
    main_suite_hmac_drbg_nopr(0,NULL);
    printf("========= main_suite_hmac_drbg_nopr end =====\n\n");

    printf("========= main_suite_hmac_drbg_pr begin =====\n\n");
    main_suite_hmac_drbg_pr(0,NULL);
    printf("========= main_suite_hmac_drbg_pr end =====\n\n");

    printf("========= main_suite_md begin =====\n\n");
    main_suite_md(0,NULL);
    printf("========= main_suite_md end =====\n\n");

    printf("========= main_suite_mdx begin =====\n\n");
    main_suite_mdx(0,NULL);
    printf("========= main_suite_mdx end =====\n\n");

    printf("========= main_suite_memory_buffer_alloc begin =====\n\n");
    main_suite_memory_buffer_alloc(0,NULL);
    printf("========= main_suite_memory_buffer_alloc end =====\n\n");

    printf("========= main_suite_mpi begin =====\n\n");
    main_suite_mpi(0,NULL);
    printf("========= main_suite_mpi end =====\n\n");

    printf("========= main_suite_nist_kw begin =====\n\n");
    main_suite_nist_kw(0,NULL);
    printf("========= main_suite_nist_kw end =====\n\n");

    printf("========= main_suite_pem begin =====\n\n");
    main_suite_pem(0,NULL);
    printf("========= main_suite_pem end =====\n\n");

    printf("========= main_suite_pk begin =====\n\n");
    main_suite_pk(0,NULL);
    printf("========= main_suite_pk end =====\n\n");

    printf("========= main_suite_pkcs1_v15 begin =====\n\n");
    main_suite_pkcs1_v15(0,NULL);
    printf("========= main_suite_pkcs1_v15 end =====\n\n");

    printf("========= main_suite_pkcs1_v21 begin =====\n\n");
    main_suite_pkcs1_v21(0,NULL);
    printf("========= main_suite_pkcs1_v21 end =====\n\n");

    printf("========= main_suite_pkcs5 begin =====\n\n");
    main_suite_pkcs5(0,NULL);
    printf("========= main_suite_pkcs5 end =====\n\n");

    printf("========= main_suite_pkparse begin =====\n\n");
    // main_suite_pkparse(0,NULL);
    printf("========= main_suite_pkparse end =====\n\n");

    printf("========= main_suite_pkwrite begin =====\n\n");
    // main_suite_pkwrite(0,NULL);
    printf("========= main_suite_pkwrite end =====\n\n");

    printf("========= main_suite_policy1305 begin =====\n\n");
    main_suite_policy1305(0,NULL);
    printf("========= main_suite_policy1305 end =====\n\n");

    printf("========= main_suite_rsa begin =====\n\n");
    main_suite_rsa(0,NULL);
    printf("========= main_suite_rsa end =====\n\n");

    printf("========= main_suite_shax begin =====\n\n");
    main_suite_shax(0,NULL);
    printf("========= main_suite_shax end =====\n\n");

    printf("========= main_suite_ssl begin =====\n\n");
    main_suite_ssl(0,NULL);
    printf("========= main_suite_ssl end =====\n\n");

    printf("========= main_suite_timing begin =====\n\n");
    main_suite_timing(0,NULL);
    printf("========= main_suite_timing end =====\n\n");

    printf("========= main_suite_version begin =====\n\n");
    main_suite_version(0,NULL);
    printf("========= main_suite_version end =====\n\n");

    printf("========= main_suite_x509parse begin =====\n\n");
    // main_suite_x509parse(0,NULL);
    printf("========= main_suite_x509parse end =====\n\n");

    printf("========= main_suite_x509write begin =====\n\n");
    // main_suite_x509write(0,NULL);
    printf("========= main_suite_x509write end =====\n\n");

    printf("========= main_suite_xtea begin =====\n\n");
    main_suite_xtea(0,NULL);
    printf("========= main_suite_xtea end =====\n\n");























    return ret;
}
