/**
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbedtls/certs.h"

// huawei
const char G_MBEDTLS_ROOT_CERTIFICATE[] =
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIHeTCCBWGgAwIBAgIQf+qaN0l+0yGyi6R9UqGJTTANBgkqhkiG9w0BAQsFADCB\r\n" \
"iTELMAkGA1UEBhMCSVQxEDAOBgNVBAgMB0JlcmdhbW8xGTAXBgNVBAcMEFBvbnRl\r\n" \
"IFNhbiBQaWV0cm8xFzAVBgNVBAoMDkFjdGFsaXMgUy5wLkEuMTQwMgYDVQQDDCtB\r\n" \
"Y3RhbGlzIE9yZ2FuaXphdGlvbiBWYWxpZGF0ZWQgU2VydmVyIENBIEczMB4XDTIw\r\n" \
"MDcyMzExNTQxOVoXDTIxMDYwNDEwMDkxOVowgZYxCzAJBgNVBAYTAkNOMRIwEAYD\r\n" \
"VQQIDAlHdWFuZ2RvbmcxETAPBgNVBAcMCFNoZW56aGVuMSUwIwYDVQQKDBxIdWF3\r\n" \
"ZWkgVGVjaG5vbG9naWVzIENvLiwgTHRkMSIwIAYDVQQLDBlTZWN1cml0eSBFbmdp\r\n" \
"bmVlcmluZyBEZXB0MRUwEwYDVQQDDAwqLmh1YXdlaS5jb20wggEiMA0GCSqGSIb3\r\n" \
"DQEBAQUAA4IBDwAwggEKAoIBAQDNnJdX0ANjblDCxn5S8/IT6EQIDyz8W5+Zx4cX\r\n" \
"S9bpm/1KWGw74FLnWkwJgtwVGKp8PHcoq9vh1QIl4eTHFKH+9MqswsYVv8ee0uwd\r\n" \
"KfpyTrYTpSfXbDTWrP8Zyaxj2MeIurrqws7JTDADNMb5qVlyyypBKrbb/sdIyw4u\r\n" \
"ANFt/L61C7oM3iLLYI+gINsT/cqi3HSYGkJze91taZuLySojN7iIR+Sh/YdyFdhe\r\n" \
"Rzh9973+28ngTEg8ItE7DI+wkoobfiuFvujgymsMwhQYRoAdHZ2YCdbAp1WNpEbx\r\n" \
"Ozkkykxlmrn+tKQE7NFZVM0XeTk3g2+hsze+3HQBfs6MFte1AgMBAAGjggLMMIIC\r\n" \
"yDAMBgNVHRMBAf8EAjAAMB8GA1UdIwQYMBaAFJ+KsbXxsd6C9Cd8vojN3qlDgaNL\r\n" \
"MH4GCCsGAQUFBwEBBHIwcDA7BggrBgEFBQcwAoYvaHR0cDovL2NhY2VydC5hY3Rh\r\n" \
"bGlzLml0L2NlcnRzL2FjdGFsaXMtYXV0aG92ZzMwMQYIKwYBBQUHMAGGJWh0dHA6\r\n" \
"Ly9vY3NwMDkuYWN0YWxpcy5pdC9WQS9BVVRIT1YtRzMwIwYDVR0RBBwwGoIKaHVh\r\n" \
"d2VpLmNvbYIMKi5odWF3ZWkuY29tMFEGA1UdIARKMEgwPAYGK4EfARMBMDIwMAYI\r\n" \
"KwYBBQUHAgEWJGh0dHBzOi8vd3d3LmFjdGFsaXMuaXQvYXJlYS1kb3dubG9hZDAI\r\n" \
"BgZngQwBAgIwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMEgGA1UdHwRB\r\n" \
"MD8wPaA7oDmGN2h0dHA6Ly9jcmwwOS5hY3RhbGlzLml0L1JlcG9zaXRvcnkvQVVU\r\n" \
"SE9WLUczL2dldExhc3RDUkwwHQYDVR0OBBYEFDVPUVcERFn/Ryc35St5qhpj8U9b\r\n" \
"MA4GA1UdDwEB/wQEAwIFoDCCAQUGCisGAQQB1nkCBAIEgfYEgfMA8QB2AG9Tdqwx\r\n" \
"8DEZ2JkApFEV/3cVHBHZAsEAKQaNsgiaN9kTAAABc3uPW30AAAQDAEcwRQIgRBop\r\n" \
"JZ4QS5+KeGz/Y///rR1OaA28JS6Zc0NyrKaA4RsCIQCGbRSWEQ0uO3c4/2WwfYTD\r\n" \
"/LzCuEl0Uze1Bno/O6oq6wB3AH0+8viP/4hVaCTCwMqeUol5K8UOeAl/LmqXaJl+\r\n" \
"IvDXAAABc3uPW3kAAAQDAEgwRgIhAOnnBWI9nDihrt3jE+9JG8P4pfrwAV+cGnRz\r\n" \
"vKWfMrnEAiEAy1bp4C/uCF/PCh/DwAfeZtz1b2rv+lPUqStDqoiXWecwDQYJKoZI\r\n" \
"hvcNAQELBQADggIBAHiBufoO96dgVGnloxVLfJq4CMzwKt7rutBUYpwNdc6bgflN\r\n" \
"UJ80V07fzGcmDdV65arqrxTJD+ANNlgJqLxxhzhfdsqQPkmp4VEZFJN3jG3azU9f\r\n" \
"lHPqYPTYcJczBauSTXqwgT1faUUkylKKbfiBM+j5iH3dpwdQjSRMvXRaA9RFfAED\r\n" \
"Ncx68QzPcuMnDGOfm26wbA70YriOjaG9Xy1rmUyy1cknP7IRsucsVKhqmvJ2A+tc\r\n" \
"3q/cEc4OACMk2+sV3VAx9EJb9Hg3sFPdrgxL6VX0jtKEB4f5Mkt/2JXDAPg29BUa\r\n" \
"S8J5/RPgcN4ZuBiZA0eBwV3Keybfhtmva4Gns6flYU8P3gTqdc7m1YGRWPfXMgvI\r\n" \
"QLuIbwPueeCJwCpe78YOY8jhKbSkPnv+gqXJT1nqzlajzdAl/8maX+GOphVCHdxO\r\n" \
"4ij3Wa+MqYwp8ttHBqQJT0Vm925G2e9dax/4xmmtUbTEcsZbi5htAwV7/dfurmkh\r\n" \
"sToGNWCeg+DHHzOTfzLdoY1gJqxbkPrdZJL5rwk7v69fXytSrXHpML45GPtavXGL\r\n" \
"RDz5lZj2eabqnVEjzrSee98fcJDuHUcePM38/9yK7X2c1ynLjTzk1k3i7P2ahE2M\r\n" \
"pk+d5mGlruCfcDLr0sFAV3gls+Wd/4/R6wWJyi5SXuM0XufB+Ghmp/cJIe43\r\n" \
"-----END CERTIFICATE-----\r\n" \
;

const size_t G_MBEDTLS_ROOT_CERTIFICATE_LEN = sizeof(G_MBEDTLS_ROOT_CERTIFICATE);

