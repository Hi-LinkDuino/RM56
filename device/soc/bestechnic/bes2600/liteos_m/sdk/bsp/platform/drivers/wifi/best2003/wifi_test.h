/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __WIFI_TEST_H__
#define __WIFI_TEST_H__

//#include "hwtest.h"
//#include "hal_transq.h"
//#include "cmsis.h"
//#include "string.h"
#ifdef __cplusplus
extern "C" {
#endif

//test MICRO

#define TEST_ENC 0
//test MICRO end


typedef unsigned char               uint8;
typedef signed char                 int8;
typedef unsigned short int          uint16;
typedef signed short int            int16;
typedef unsigned int                uint32;
typedef signed int                  int32;


#define RX_SLOT_NUM  4   //32
#define RX_FRAME_LEN  2048
  typedef struct TEST_CFG_S{
      uint32_t test_cfg;
      uint32_t* test_flag_base;
      uint32_t* test_rx_count;
      uint32_t run_mode_cfg;
      uint8 test_local_mac[6];
      uint8 test_peer_mac[6];
  }TEST_CFG_T;
  
  typedef struct HI_TEST_MODE_REQ_S {
      uint16 MsgLen;
      uint16 MsgId;
      TEST_CFG_T test_cfg; //1, test mode; 0: normal
  
  }HI_TEST_MODE_REQ;

  typedef struct WIFI_RX_MSG_S
  {
      uint32_t MsgLen;
      uint32_t data[2048/4];
  } WIFI_RX_MSG;
 
  typedef struct HI_MSG_HDR_S {
      uint16_t MsgLen;      /* length in bytes from MsgLen to end of payload */
      uint16_t MsgId;
      /* Payload is followed */
  } HI_MSG_HDR;
  typedef struct WSM_HI_TX_REQ_S
  {
      uint16_t MsgLen;
      uint16_t MsgId;
      uint32_t PacketId;
      uint8_t MaxTxRate;
      uint8_t QueueId;      /* WSM_QUEUE_ID_XXXX */
      uint8_t More;
      uint8_t Flags;
      uint32_t Reserved;
      uint32_t ExpireTime;
      uint32_t HtTxParameters;  /* to be defined, but 32-bit is enough */
      uint32_t Frame;
  } WSM_HI_TX_REQ;

  typedef struct WSM_HI_TX_CNF_S
  {
      uint16 MsgLen;
      uint16 MsgId;
      uint32 PacketId;
      uint32 Status;      /* WSM_STATUS_XXXXX */
      uint8 TxedRate;
      uint8 AckFailures;
      uint16 Flags;
      uint32 MediaDelay;
      uint32 TxQueueDelay;
  } WSM_HI_TX_CNF;

  
  typedef struct WSM_HI_ADDKEY_S
  {
      uint16_t MsgLen;
      uint16_t MsgId;
      
      uint8    KeyType;
      uint8    KeyEntryIndex;
      uint8    Reserved[2];
      uint8    Key[40];
  } WSM_HI_ADDKEY;
 #define D0_ADDR_COPY(_a, _b) \
     {*((uint16_t*)(_a)) = *((uint16_t*)(_b)); \
      *(((uint16_t*)(_a))+1) = *(((uint16_t*)(_b))+1); \
      *(((uint16_t*)(_a))+2) = *(((uint16_t*)(_b))+2); }

typedef struct D0_ADDR_S {
	uint8_t Byte[6];

} D0_ADDR;

typedef struct D11_HEADER_S {
	//DataToDs;
	uint16_t FrameCtl;
	uint16_t Duration;
	D0_ADDR BssId;
	D0_ADDR Sa;
	D0_ADDR Da;
	uint16_t SeqCtl;
	uint16_t QosCtl;	/* optional */
	
} D11_HEADER;
 //enum ENUM_WIFI_BAND{
	//BAND_2G4,
	//BAND_5G,
	//DUAL_BAND
//};


extern volatile uint32_t* test_freq;

 
extern void wifi_nosignaling_test(uint8 band);
extern void dig_band_switch(uint8 band);
extern void wifi_config_40m(uint8 band, uint8 en);

#ifdef __cplusplus
}
#endif

#endif //__WIFI_TEST_H__

