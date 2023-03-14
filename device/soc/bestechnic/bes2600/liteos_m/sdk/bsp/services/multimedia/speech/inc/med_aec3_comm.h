
#ifndef _MED_AEC3_COMM_H_
#define _MED_AEC3_COMM_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "med_fft.h"
#include "codec_com_codec.h"
#include "med_aec3_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* FFT IFFT�ص����� */
typedef VOS_VOID (*MED_AEC_FFT_CALLBACK)(VOS_INT16 *, VOS_INT16 *);

#ifdef AEC_ONLY_SUPPORT_8k
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_NB)                    /* ���FFT���� */
#else
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#define MED_AEC_FFT_LEN_WB              (512)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#define MED_AEC_FFT_LEN_WB              (480)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_WB)                    /* ���FFT���� */
#endif

#define MED_AEC_HF_AF_M                 (6)

/* ��ʱȫ�����飬��AEC��ģ��ʹ�� */
//extern VOS_INT16                        g_ashwMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT16                        g_ashwMedAecTmp2Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT32                        g_aswMedAecTmp1Len320[CODEC_FRAME_LENGTH_WB];
//extern VOS_INT32                        g_aswMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];

/* AEC �����궨��*/
//#define MED_AEC_GetshwVecTmp640Ptr1()   (&g_ashwMedAecTmp1Len640[0])            /* ��ʱȫ������ָ�� ����640 INT16 */
//#define MED_AEC_GetshwVecTmp640Ptr2()   (&g_ashwMedAecTmp2Len640[0])            /* ��ʱȫ������ָ�� ����640 INT16 */
//#define MED_AEC_GetswVecTmp320Ptr1()    (&g_aswMedAecTmp1Len320[0])             /* ��ʱȫ������ָ�� ����320 INT32 */
//#define MED_AEC_GetswVecTmp640Ptr1()    (&g_aswMedAecTmp1Len640[0])             /* ��ʱȫ������ָ�� ����640 INT32 */

#define MED_AEC_OFFSET_THD              (60)                                    /* �ӳٲ�����ֵ */
#define MED_AEC_MAX_OFFSET              (960)                                   /* ��󲹳����ȣ���λ������ Q0*/
#define MED_AEC_MAX_TAIL_LEN            (960)                                   /* ���β�˳��ȣ���λms�����֧��60msβ���ӳ� Q0*/
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/* AF FFT���� */
enum MED_AEC_AF_FFT_LEN_ENUM
{
#ifdef SPEECH_STREAM_UNIT_128
    MED_AEC_AF_FFT_LEN_NB = 256,                                                /* խ��FFT���� */
    MED_AEC_AF_FFT_LEN_WB = 512,                                                /* ���FFT���� */
#else
    MED_AEC_AF_FFT_LEN_NB = 240,                                                /* խ��FFT���� */
    MED_AEC_AF_FFT_LEN_WB = 480,                                                /* ���FFT���� */
#endif
    MED_AEC_AF_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_AF_FFT_LEN_ENUM_INT16;

/* ������ö�� */
enum CODEC_SWITCH_ENUM
{
	CODEC_SWITCH_OFF = 0,
	CODEC_SWITCH_ON,
	CODEC_SWITCH_BUTT
};
typedef VOS_UINT16 CODEC_SWITCH_ENUM_UINT16;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/

/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/
#ifdef DEBUG_AEC
static VOS_VOID fprint_vec_int32(FILE *fd, VOS_INT32 *buf, VOS_INT32 len)
{
    for (VOS_INT32 i = 0; i < len - 1; i++) {
        fprintf(fd, "%d ", buf[i]);
    }
    fprintf(fd, "%d\n", buf[len - 1]);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

