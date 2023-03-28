

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "v_typdef.h"

#ifndef _MED_AEC3_MAIN_H_
#define _MED_AEC3_MAIN_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/
typedef VOS_VOID (*NS_HANDLER_T)(VOS_VOID *stNs, VOS_INT16 *shwBuf, VOS_INT32 swFrameLength);

/* AEC������ʼ���ӿ� */
typedef struct
{
    VOS_BOOL                            enEAecEnable;                           /* AEC: ��ǿ����AECʹ�ܿ��� */
    VOS_BOOL                            enHpfEnable;                            /* HPF: ��ͨ�˲����� */
    VOS_BOOL                            enAfEnable;                             /* AF : ����Ӧ�˲����� */
    VOS_BOOL                            enNsEnable;                             /* NS : �������ƿ��� */
	VOS_INT16                           shwDelayLength;                         /* DELAY: �̶���ʱ */
} MED_AEC3_NV_STRU;

/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/
extern VOS_UINT32  MED_AEC3_Main(
	                   VOS_VOID               *pAecInstance,
                       VOS_INT16              *pshwMicIn,
					   VOS_INT16              *pshwSpkIn,
                       VOS_INT16              *pshwLineOut);
extern VOS_VOID* MED_AEC3_Create(void);
extern VOS_UINT32  MED_AEC3_Destroy(VOS_VOID  **ppAecObj);
extern VOS_UINT32 MED_AEC3_SetPara (
                       VOS_VOID                        *pAecInstance,
					   MED_AEC3_NV_STRU                 *pstNv,
                       VOS_INT32                       enSampleRate);
extern VOS_UINT32  MED_AEC3_SetExtenalNsHandle(
                       VOS_VOID               *pAecInstance,
                       VOS_VOID               *pstNs,
                       NS_HANDLER_T           swNsHandler);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

