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
#ifndef _REG_USB_H_
#define _REG_USB_H_

#include "stdint.h"
#include "plat_types.h"

// =============================================================================
//  MACROS
// =============================================================================
#define SPFIFORAM_SIZE                          (1024)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// USBC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
struct USBC_T {
    //common part
    __IO uint32_t GOTGCTL;                  //0x00000000
    __IO uint32_t GOTGINT;                  //0x00000004
    __IO uint32_t GAHBCFG;                  //0x00000008
    __IO uint32_t GUSBCFG;                  //0x0000000C
    __IO uint32_t GRSTCTL;                  //0x00000010
    __IO uint32_t GINTSTS;                  //0x00000014
    __IO uint32_t GINTMSK;                  //0x00000018
    __IO uint32_t GRXSTSR;                  //0x0000001C
    __IO uint32_t GRXSTSP;                  //0x00000020
    __IO uint32_t GRXFSIZ;                  //0x00000024
    __IO uint32_t GNPTXFSIZ;                //0x00000028
    __IO uint32_t GNPTXSTS;                 //0x0000002C
    __IO uint32_t GI2CCTL;                  //0x00000030
    __IO uint32_t GPVNDCTL;                 //0x00000034
    __IO uint32_t GGPIO;                    //0x00000038
    __IO uint32_t GUID;                     //0x0000003C
    __IO uint32_t GSNPSID;                  //0x00000040
    __IO uint32_t GHWCFG1;                  //0x00000044
    __IO uint32_t GHWCFG2;                  //0x00000048
    __IO uint32_t GHWCFG3;                  //0x0000004C
    __IO uint32_t GHWCFG4;                  //0x00000050
    __IO uint32_t GLPMCFG;                  //0x00000054
    __IO uint32_t GPWRDN;                   //0x00000058
    __IO uint32_t GDFIFOCFG;                //0x0000005C
    __IO uint32_t GADPCTL;                  //0x00000060
    __IO uint32_t Reserved_00000054[0x27];  //0x00000064
    __IO uint32_t HPTXFSIZ;                 //0x00000100
    union
    {
        __IO uint32_t DPTXFSIZn;            //0x00000104 - 13F
        __IO uint32_t DIEPTXFn;             //0x00000104 - 13F
    } DTXFSIZE[15];
    __IO uint32_t Reserved_00000140[0xB0];  //0x00000140 - 3FF
    //host part
    __IO uint32_t HCFG;                     //0x00000400
    __IO uint32_t HFIR;                     //0x00000404
    __IO uint32_t HFNUM;                    //0x00000408
    __IO uint32_t Reserved_0000040C;        //0x0000040C
    __IO uint32_t HPTXSTS;                  //0x00000410
    __IO uint32_t HAINT;                    //0x00000414
    __IO uint32_t HAINTMSK;                 //0x00000418
    __IO uint32_t Reserved_0000041C[9];     //0x0000041C
    __IO uint32_t HPRT;                     //0x00000440
    __IO uint32_t Reserved_00000444[0x2F];  //0x00000444
    struct
    {
        __IO uint32_t HCCHARn;              //0x00000500+n*0x20
        __IO uint32_t HCSPLTn;              //0x00000504+n*0x20
        __IO uint32_t HCINTn;               //0x00000508+n*0x20
        __IO uint32_t HCINTMSKn;            //0x0000050C+n*0x20
        __IO uint32_t HCTSIZn;              //0x00000510+n*0x20
        __IO uint32_t HCDMAn;               //0x00000514+n*0x20
        __IO uint32_t Reserved_00000518;    //0x00000518+n*0x20
        __IO uint32_t HCDMABn;              //0x0000051C+n*0x20
    } HCSR[16];
    __IO uint32_t Reserved_00000700[0x40];  //0x00000700
    //device part
    __IO uint32_t DCFG;                     //0x00000800
    __IO uint32_t DCTL;                     //0x00000804
    __IO uint32_t DSTS;                     //0x00000808
    __IO uint32_t Reserved_0000080C;        //0x0000080C
    __IO uint32_t DIEPMSK;                  //0x00000810
    __IO uint32_t DOEPMSK;                  //0x00000814
    __IO uint32_t DAINT;                    //0x00000818
    __IO uint32_t DAINTMSK;                 //0x0000081C
    __IO uint32_t DTKNQR1;                  //0x00000820
    __IO uint32_t DTKNQR2;                  //0x00000824
    __IO uint32_t DVBUSDIS;                 //0x00000828
    __IO uint32_t DVBUSPULSE;               //0x0000082C
    __IO uint32_t DTHRCTL;                  //0x00000830
    __IO uint32_t DIEPEMPMSK;               //0x00000834
    __IO uint32_t Reserved_00000838[0x32];  //0x00000838
    __IO uint32_t DIEPCTL0;                 //0x00000900
    __IO uint32_t Reserved_00000904;        //0x00000904
    __IO uint32_t DIEPINT0;                 //0x00000908
    __IO uint32_t Reserved_0000090C;        //0x0000090C
    __IO uint32_t DIEPTSIZ0;                //0x00000910
    __IO uint32_t DIEPDMA0;                 //0x00000914
    __IO uint32_t DIEPFSTS0;                //0x00000918
    __IO uint32_t DIEPDMAB0;                //0x0000091C
    struct
    {
        __IO uint32_t DIEPCTL;              //0x00000920
        __IO uint32_t Reserved_00000924;    //0x00000924
        __IO uint32_t DIEPINT;              //0x00000928
        __IO uint32_t Reserved_0000092C;    //0x0000092C
        __IO uint32_t DIEPTSIZ;             //0x00000930
        __IO uint32_t DIEPDMA;              //0x00000934
        __IO uint32_t DIEPFSTS;             //0x00000938
        __IO uint32_t DIEPDMAB;             //0x0000093C
    } DIEPnCONFIG[15];
    __IO uint32_t DOEPCTL0;                 //0x00000B00
    __IO uint32_t Reserved_00000B04;        //0x00000B04
    __IO uint32_t DOEPINT0;                 //0x00000B08
    __IO uint32_t Reserved_00000B0C;        //0x00000B0C
    __IO uint32_t DOEPTSIZ0;                //0x00000B10
    __IO uint32_t DOEPDMA0;                 //0x00000B14
    __IO uint32_t Reserved_00000B18;        //0x00000B18
    __IO uint32_t DOEPDMAB0;                //0x00000B1C
    struct
    {
        __IO uint32_t DOEPCTL;              //0x00000B20
        __IO uint32_t Reserved_00000B24;    //0x00000B24
        __IO uint32_t DOEPINT;              //0x00000B28
        __IO uint32_t Reserved_00000B2C;    //0x00000B2C
        __IO uint32_t DOEPTSIZ;             //0x00000B30
        __IO uint32_t DOEPDMA;              //0x00000B34
        __IO uint32_t Reserved_00000B38;    //0x00000B38
        __IO uint32_t DOEPDMAB;             //0x00000B3C
    } DOEPnCONFIG[15];
    __IO uint32_t Reserved_00000D00[0x40];  //0x00000D00
    __IO uint32_t PCGCCTL;                  //0x00000E00
    __IO uint32_t Reserved_00000E04;        //0x00000E04
    __IO uint32_t TPORTDBG0;                //0x00000E08
    __IO uint32_t TPORTDBG1;                //0x00000E0C
    __IO uint32_t Reserved_00000E0C[0x7C];  //0x00000E10
    struct
    {
        __IO uint32_t TxRxData;             //0x00001000
        __IO uint32_t Reserved_00001004[0x3FF]; //0x00001004
    } EPnFIFO[16];
    __IO uint32_t Reserved_00011000[0x3C00]; //0x00011000
    struct
    {
        __IO uint32_t RAMData;              //0x00020000
    } SPFIFORAM[SPFIFORAM_SIZE];
};

//GOTGCTL
#define USBC_SESREQSCS              (1<<0)
#define USBC_SESREQSCS_MASK         (1<<0)
#define USBC_SESREQSCS_SHIFT        (0)
#define USBC_SESREQ                 (1<<1)
#define USBC_SESREQ_MASK            (1<<1)
#define USBC_SESREQ_SHIFT           (1)
#define USBC_HSTNEGSCS              (1<<8)
#define USBC_HSTNEGSCS_MASK         (1<<8)
#define USBC_HSTNEGSCS_SHIFT        (8)
#define USBC_HNPREQ                 (1<<9)
#define USBC_HNPREQ_MASK            (1<<9)
#define USBC_HNPREQ_SHIFT           (9)
#define USBC_HSTSETHNPEN            (1<<10)
#define USBC_HSTSETHNPEN_MASK       (1<<10)
#define USBC_HSTSETHNPEN_SHIFT      (10)
#define USBC_DEVHNPEN               (1<<11)
#define USBC_DEVHNPEN_MASK          (1<<11)
#define USBC_DEVHNPEN_SHIFT         (11)
#define USBC_CONLDSTS               (1<<16)
#define USBC_CONLDSTS_MASK          (1<<16)
#define USBC_CONLDSTS_SHIFT         (16)
#define USBC_DBNCTIME               (1<<17)
#define USBC_DBNCTIME_MASK          (1<<17)
#define USBC_DBNCTIME_SHIFT         (17)
#define USBC_ASESVLD                (1<<18)
#define USBC_ASESVLD_MASK           (1<<18)
#define USBC_ASESVLD_SHIFT          (18)
#define USBC_BSESVLD                (1<<19)
#define USBC_BSESVLD_MASK           (1<<19)
#define USBC_BSESVLD_SHIFT          (19)

//GOTGINT
#define USBC_SESENDDET              (1<<2)
#define USBC_SESENDDET_MASK         (1<<2)
#define USBC_SESENDDET_SHIFT        (2)
#define USBC_SESREQSUCSTSCHNG       (1<<8)
#define USBC_SESREQSUCSTSCHNG_MASK  (1<<8)
#define USBC_SESREQSUCSTSCHNG_SHIFT (8)
#define USBC_HSTNEGSUCSTSCHNG       (1<<9)
#define USBC_HSTNEGSUCSTSCHNG_MASK  (1<<9)
#define USBC_HSTNEGSUCSTSCHNG_SHIFT (9)
#define USBC_HSTNEGDET              (1<<17)
#define USBC_HSTNEGDET_MASK         (1<<17)
#define USBC_HSTNEGDET_SHIFT        (17)
#define USBC_ADEVTOUTCHG            (1<<18)
#define USBC_ADEVTOUTCHG_MASK       (1<<18)
#define USBC_ADEVTOUTCHG_SHIFT      (18)
#define USBC_DBNCEDONE              (1<<19)
#define USBC_DBNCEDONE_MASK         (1<<19)
#define USBC_DBNCEDONE_SHIFT        (19)

//GAHBCFG
#define USBC_GLBLINTRMSK            (1<<0)
#define USBC_GLBLINTRMSK_MASK       (1<<0)
#define USBC_GLBLINTRMSK_SHIFT      (0)
#define USBC_HBSTLEN(n)             (((n)&15)<<1)
#define USBC_HBSTLEN_MASK           (15<<1)
#define USBC_HBSTLEN_SHIFT          (1)
#define USBC_DMAEN                  (1<<5)
#define USBC_DMAEN_MASK             (1<<5)
#define USBC_DMAEN_SHIFT            (5)
#define USBC_NPTXFEMPLVL            (1<<7)
#define USBC_NPTXFEMPLVL_MASK       (1<<7)
#define USBC_NPTXFEMPLVL_SHIFT      (7)
#define USBC_PTXFEMPLVL             (1<<8)
#define USBC_PTXFEMPLVL_MASK        (1<<8)
#define USBC_PTXFEMPLVL_SHIFT       (8)

//GUSBCFG
#define USBC_TOUTCAL(n)             (((n)&7)<<0)
#define USBC_TOUTCAL_MASK           (7<<0)
#define USBC_TOUTCAL_SHIFT          (0)
#define USBC_PHYIF                  (1<<3)
#define USBC_PHYIF_MASK             (1<<3)
#define USBC_PHYIF_SHIFT            (3)
#define USBC_ULPI_UTMI_SEL          (1<<4)
#define USBC_ULPI_UTMI_SEL_MASK     (1<<4)
#define USBC_ULPI_UTMI_SEL_SHIFT    (4)
#define USBC_FSINTF                 (1<<5)
#define USBC_FSINTF_MASK            (1<<5)
#define USBC_FSINTF_SHIFT           (5)
#define USBC_PHYSEL                 (1<<6)
#define USBC_PHYSEL_MASK            (1<<6)
#define USBC_PHYSEL_SHIFT           (6)
#define USBC_DDRSEL                 (1<<7)
#define USBC_DDRSEL_MASK            (1<<7)
#define USBC_DDRSEL_SHIFT           (7)
#define USBC_SRPCAP                 (1<<8)
#define USBC_SRPCAP_MASK            (1<<8)
#define USBC_SRPCAP_SHIFT           (8)
#define USBC_HNPCAP                 (1<<9)
#define USBC_HNPCAP_MASK            (1<<9)
#define USBC_HNPCAP_SHIFT           (9)
#define USBC_USBTRDTIM(n)           (((n)&15)<<10)
#define USBC_USBTRDTIM_MASK         (15<<10)
#define USBC_USBTRDTIM_SHIFT        (10)
#define USBC_PHYPWRCLKSEL           (1<<15)
#define USBC_PHYPWRCLKSEL_MASK      (1<<15)
#define USBC_PHYPWRCLKSEL_SHIFT     (15)
#define USBC_OTGI2CSEL              (1<<16)
#define USBC_OTGI2CSEL_MASK         (1<<16)
#define USBC_OTGI2CSEL_SHIFT        (16)
#define USBC_ULPIFSLS               (1<<17)
#define USBC_ULPIFSLS_MASK          (1<<17)
#define USBC_ULPIFSLS_SHIFT         (17)
#define USBC_ULPIAUTORES            (1<<18)
#define USBC_ULPIAUTORES_MASK       (1<<18)
#define USBC_ULPIAUTORES_SHIFT      (18)
#define USBC_ULPICLKSUSM            (1<<19)
#define USBC_ULPICLKSUSM_MASK       (1<<19)
#define USBC_ULPICLKSUSM_SHIFT      (19)
#define USBC_ULPIEXTVBUSDRV         (1<<20)
#define USBC_ULPIEXTVBUSDRV_MASK    (1<<20)
#define USBC_ULPIEXTVBUSDRV_SHIFT   (20)
#define USBC_ULPIEXTVBUSINDICATOR   (1<<21)
#define USBC_ULPIEXTVBUSINDICATOR_MASK (1<<21)
#define USBC_ULPIEXTVBUSINDICATOR_SHIFT (21)
#define USBC_TERMSELDLPULSE         (1<<22)
#define USBC_TERMSELDLPULSE_MASK    (1<<22)
#define USBC_TERMSELDLPULSE_SHIFT   (22)
#define USBC_FORCEHSTMODE           (1<<29)
#define USBC_FORCEHSTMODE_MASK      (1<<29)
#define USBC_FORCEHSTMODE_SHIFT     (29)
#define USBC_FORCEDEVMODE           (1<<30)
#define USBC_FORCEDEVMODE_MASK      (1<<30)
#define USBC_FORCEDEVMODE_SHIFT     (30)
#define USBC_CORRUPTTXPACKET        (1<<31)
#define USBC_CORRUPTTXPACKET_MASK   (1<<31)
#define USBC_CORRUPTTXPACKET_SHIFT  (31)

//GRSTCTL
#define USBC_CSFTRST                (1<<0)
#define USBC_CSFTRST_MASK           (1<<0)
#define USBC_CSFTRST_SHIFT          (0)
#define USBC_FRMCNTRRST             (1<<2)
#define USBC_FRMCNTRRST_MASK        (1<<2)
#define USBC_FRMCNTRRST_SHIFT       (2)
#define USBC_INTKNQFLSH             (1<<3)
#define USBC_INTKNQFLSH_MASK        (1<<3)
#define USBC_INTKNQFLSH_SHIFT       (3)
#define USBC_RXFFLSH                (1<<4)
#define USBC_RXFFLSH_MASK           (1<<4)
#define USBC_RXFFLSH_SHIFT          (4)
#define USBC_TXFFLSH                (1<<5)
#define USBC_TXFFLSH_MASK           (1<<5)
#define USBC_TXFFLSH_SHIFT          (5)
#define USBC_TXFNUM(n)              (((n)&31)<<6)
#define USBC_TXFNUM_MASK            (31<<6)
#define USBC_TXFNUM_SHIFT           (6)
#define USBC_DMAREQ                 (1<<30)
#define USBC_DMAREQ_MASK            (1<<30)
#define USBC_DMAREQ_SHIFT           (30)
#define USBC_AHBIDLE                (1<<31)
#define USBC_AHBIDLE_MASK           (1<<31)
#define USBC_AHBIDLE_SHIFT          (31)

//GINTSTS
#define USBC_CURMOD                 (1<<0)
#define USBC_CURMOD_MASK            (1<<0)
#define USBC_CURMOD_SHIFT           (0)
#define USBC_MODEMIS                (1<<1)
#define USBC_MODEMIS_MASK           (1<<1)
#define USBC_MODEMIS_SHIFT          (1)
#define USBC_OTGINT                 (1<<2)
#define USBC_OTGINT_MASK            (1<<2)
#define USBC_OTGINT_SHIFT           (2)
#define USBC_SOF                    (1<<3)
#define USBC_SOF_MASK               (1<<3)
#define USBC_SOF_SHIFT              (3)
#define USBC_RXFLVL                 (1<<4)
#define USBC_RXFLVL_MASK            (1<<4)
#define USBC_RXFLVL_SHIFT           (4)
#define USBC_NPTXFEMP               (1<<5)
#define USBC_NPTXFEMP_MASK          (1<<5)
#define USBC_NPTXFEMP_SHIFT         (5)
#define USBC_GINNAKEFF              (1<<6)
#define USBC_GINNAKEFF_MASK         (1<<6)
#define USBC_GINNAKEFF_SHIFT        (6)
#define USBC_GOUTNAKEFF             (1<<7)
#define USBC_GOUTNAKEFF_MASK        (1<<7)
#define USBC_GOUTNAKEFF_SHIFT       (7)
#define USBC_ULPICKINT              (1<<8)
#define USBC_ULPICKINT_MASK         (1<<8)
#define USBC_ULPICKINT_SHIFT        (8)
#define USBC_I2CINT                 (1<<9)
#define USBC_I2CINT_MASK            (1<<9)
#define USBC_I2CINT_SHIFT           (9)
#define USBC_ERLYSUSP               (1<<10)
#define USBC_ERLYSUSP_MASK          (1<<10)
#define USBC_ERLYSUSP_SHIFT         (10)
#define USBC_USBSUSP                (1<<11)
#define USBC_USBSUSP_MASK           (1<<11)
#define USBC_USBSUSP_SHIFT          (11)
#define USBC_USBRST                 (1<<12)
#define USBC_USBRST_MASK            (1<<12)
#define USBC_USBRST_SHIFT           (12)
#define USBC_ENUMDONE               (1<<13)
#define USBC_ENUMDONE_MASK          (1<<13)
#define USBC_ENUMDONE_SHIFT         (13)
#define USBC_ISOOUTDROP             (1<<14)
#define USBC_ISOOUTDROP_MASK        (1<<14)
#define USBC_ISOOUTDROP_SHIFT       (14)
#define USBC_EOPF                   (1<<15)
#define USBC_EOPF_MASK              (1<<15)
#define USBC_EOPF_SHIFT             (15)
#define USBC_EPMIS                  (1<<17)
#define USBC_EPMIS_MASK             (1<<17)
#define USBC_EPMIS_SHIFT            (17)
#define USBC_IEPINT                 (1<<18)
#define USBC_IEPINT_MASK            (1<<18)
#define USBC_IEPINT_SHIFT           (18)
#define USBC_OEPINT                 (1<<19)
#define USBC_OEPINT_MASK            (1<<19)
#define USBC_OEPINT_SHIFT           (19)
#define USBC_INCOMPISOIN            (1<<20)
#define USBC_INCOMPISOIN_MASK       (1<<20)
#define USBC_INCOMPISOIN_SHIFT      (20)
#define USBC_INCOMPISOOUT           (1<<21)
#define USBC_INCOMPISOOUT_MASK      (1<<21)
#define USBC_INCOMPISOOUT_SHIFT     (21)
#define USBC_FETSUSP                (1<<22)
#define USBC_FETSUSP_MASK           (1<<22)
#define USBC_FETSUSP_SHIFT          (22)
#define USBC_PRTINT                 (1<<24)
#define USBC_PRTINT_MASK            (1<<24)
#define USBC_PRTINT_SHIFT           (24)
#define USBC_HCHINT                 (1<<25)
#define USBC_HCHINT_MASK            (1<<25)
#define USBC_HCHINT_SHIFT           (25)
#define USBC_PTXFEMP                (1<<26)
#define USBC_PTXFEMP_MASK           (1<<26)
#define USBC_PTXFEMP_SHIFT          (26)
#define USBC_LPM_INT                (1<<27)
#define USBC_LPM_INT_MASK           (1<<27)
#define USBC_LPM_INT_SHIFT          (27)
#define USBC_CONLDSTSCHNG           (1<<28)
#define USBC_CONLDSTSCHNG_MASK      (1<<28)
#define USBC_CONLDSTSCHNG_SHIFT     (28)
#define USBC_DISCONNINT             (1<<29)
#define USBC_DISCONNINT_MASK        (1<<29)
#define USBC_DISCONNINT_SHIFT       (29)
#define USBC_SESSREQINT             (1<<30)
#define USBC_SESSREQINT_MASK        (1<<30)
#define USBC_SESSREQINT_SHIFT       (30)
#define USBC_WKUPINT                (1<<31)
#define USBC_WKUPINT_MASK           (1<<31)
#define USBC_WKUPINT_SHIFT          (31)

//GINTMSK
//#define USBC_MODEMIS              (1<<1)
//#define USBC_MODEMIS_MASK         (1<<1)
//#define USBC_MODEMIS_SHIFT        (1)
//#define USBC_OTGINT               (1<<2)
//#define USBC_OTGINT_MASK          (1<<2)
//#define USBC_OTGINT_SHIFT         (2)
//#define USBC_SOF                  (1<<3)
//#define USBC_SOF_MASK             (1<<3)
//#define USBC_SOF_SHIFT            (3)
//#define USBC_RXFLVL               (1<<4)
//#define USBC_RXFLVL_MASK          (1<<4)
//#define USBC_RXFLVL_SHIFT         (4)
//#define USBC_NPTXFEMP             (1<<5)
//#define USBC_NPTXFEMP_MASK        (1<<5)
//#define USBC_NPTXFEMP_SHIFT       (5)
//#define USBC_GINNAKEFF            (1<<6)
//#define USBC_GINNAKEFF_MASK       (1<<6)
//#define USBC_GINNAKEFF_SHIFT      (6)
//#define USBC_GOUTNAKEFF           (1<<7)
//#define USBC_GOUTNAKEFF_MASK      (1<<7)
//#define USBC_GOUTNAKEFF_SHIFT     (7)
//#define USBC_ULPICKINT            (1<<8)
//#define USBC_ULPICKINT_MASK       (1<<8)
//#define USBC_ULPICKINT_SHIFT      (8)
//#define USBC_I2CINT               (1<<9)
//#define USBC_I2CINT_MASK          (1<<9)
//#define USBC_I2CINT_SHIFT         (9)
//#define USBC_ERLYSUSP             (1<<10)
//#define USBC_ERLYSUSP_MASK        (1<<10)
//#define USBC_ERLYSUSP_SHIFT       (10)
//#define USBC_USBSUSP              (1<<11)
//#define USBC_USBSUSP_MASK         (1<<11)
//#define USBC_USBSUSP_SHIFT        (11)
//#define USBC_USBRST               (1<<12)
//#define USBC_USBRST_MASK          (1<<12)
//#define USBC_USBRST_SHIFT         (12)
//#define USBC_ENUMDONE             (1<<13)
//#define USBC_ENUMDONE_MASK        (1<<13)
//#define USBC_ENUMDONE_SHIFT       (13)
//#define USBC_ISOOUTDROP           (1<<14)
//#define USBC_ISOOUTDROP_MASK      (1<<14)
//#define USBC_ISOOUTDROP_SHIFT     (14)
//#define USBC_EOPF                 (1<<15)
//#define USBC_EOPF_MASK            (1<<15)
//#define USBC_EOPF_SHIFT           (15)
//#define USBC_EPMIS                (1<<17)
//#define USBC_EPMIS_MASK           (1<<17)
//#define USBC_EPMIS_SHIFT          (17)
//#define USBC_IEPINT               (1<<18)
//#define USBC_IEPINT_MASK          (1<<18)
//#define USBC_IEPINT_SHIFT         (18)
//#define USBC_OEPINT               (1<<19)
//#define USBC_OEPINT_MASK          (1<<19)
//#define USBC_OEPINT_SHIFT         (19)
//#define USBC_INCOMPLSOIN          (1<<20)
//#define USBC_INCOMPLSOIN_MASK     (1<<20)
//#define USBC_INCOMPLSOIN_SHIFT    (20)
//#define USBC_INCOMPIP             (1<<21)
//#define USBC_INCOMPIP_MASK        (1<<21)
//#define USBC_INCOMPIP_SHIFT       (21)
//#define USBC_FETSUSP              (1<<22)
//#define USBC_FETSUSP_MASK         (1<<22)
//#define USBC_FETSUSP_SHIFT        (22)
//#define USBC_PRTINT               (1<<24)
//#define USBC_PRTINT_MASK          (1<<24)
//#define USBC_PRTINT_SHIFT         (24)
//#define USBC_HCHINT               (1<<25)
//#define USBC_HCHINT_MASK          (1<<25)
//#define USBC_HCHINT_SHIFT         (25)
//#define USBC_PTXFEMP              (1<<26)
//#define USBC_PTXFEMP_MASK         (1<<26)
//#define USBC_PTXFEMP_SHIFT        (26)
//#define USBC_CONLDSTSCHNG         (1<<28)
//#define USBC_CONLDSTSCHNG_MASK    (1<<28)
//#define USBC_CONLDSTSCHNG_SHIFT   (28)
//#define USBC_DISCONNINT           (1<<29)
//#define USBC_DISCONNINT_MASK      (1<<29)
//#define USBC_DISCONNINT_SHIFT     (29)
//#define USBC_SESSREQINT           (1<<30)
//#define USBC_SESSREQINT_MASK      (1<<30)
//#define USBC_SESSREQINT_SHIFT     (30)
//#define USBC_WKUPINT              (1<<31)
//#define USBC_WKUPINT_MASK         (1<<31)
//#define USBC_WKUPINT_SHIFT        (31)

//GRXSTSR
#define USBC_EPNUM(n)               (((n)&15)<<0)
#define USBC_EPNUM_MASK             (15<<0)
#define USBC_EPNUM_SHIFT            (0)
#define USBC_BCNT(n)                (((n)&0x7FF)<<4)
#define USBC_BCNT_MASK              (0x7FF<<4)
#define USBC_BCNT_SHIFT             (4)
#define USBC_DPID(n)                (((n)&3)<<15)
#define USBC_DPID_MASK              (3<<15)
#define USBC_DPID_SHIFT             (15)
#define USBC_PKTSTS(n)              (((n)&15)<<17)
#define USBC_PKTSTS_MASK            (15<<17)
#define USBC_PKTSTS_SHIFT           (17)
#define USBC_FN(n)                  (((n)&15)<<21)
#define USBC_FN_MASK                (15<<21)
#define USBC_FN_SHIFT               (21)

//GRXSTSP
//#define USBC_EPNUM(n)             (((n)&15)<<0)
//#define USBC_EPNUM_MASK           (15<<0)
//#define USBC_EPNUM_SHIFT          (0)
//#define USBC_BCNT(n)              (((n)&0x7FF)<<4)
//#define USBC_BCNT_MASK            (0x7FF<<4)
//#define USBC_BCNT_SHIFT           (4)
//#define USBC_DPID(n)              (((n)&3)<<15)
//#define USBC_DPID_MASK            (3<<15)
//#define USBC_DPID_SHIFT           (15)
//#define USBC_PKTSTS(n)            (((n)&15)<<17)
//#define USBC_PKTSTS_MASK          (15<<17)
//#define USBC_PKTSTS_SHIFT         (17)
//#define USBC_FN(n)                (((n)&15)<<21)
//#define USBC_FN_MASK              (15<<21)
//#define USBC_FN_SHIFT             (21)

//GRXFSIZ
#define USBC_RXFDEP(n)              (((n)&0xFFFF)<<0)
#define USBC_RXFDEP_MASK            (0xFFFF<<0)
#define USBC_RXFDEP_SHIFT           (0)

//GNPTXFSIZ
#define USBC_NPTXFSTADDR(n)         (((n)&0xFFFF)<<0)
#define USBC_NPTXFSTADDR_MASK       (0xFFFF<<0)
#define USBC_NPTXFSTADDR_SHIFT      (0)
#define USBC_NPTXFDEPS(n)           (((n)&0xFFFF)<<16)
#define USBC_NPTXFDEPS_MASK         (0xFFFF<<16)
#define USBC_NPTXFDEPS_SHIFT        (16)

//GNPTXSTS
#define USBC_NPTXFSPCAVAIL(n)       (((n)&0xFFFF)<<0)
#define USBC_NPTXFSPCAVAIL_MASK     (0xFFFF<<0)
#define USBC_NPTXFSPCAVAIL_SHIFT    (0)
#define USBC_NPTXQSPCAVAIL(n)       (((n)&0xFF)<<16)
#define USBC_NPTXQSPCAVAIL_MASK     (0xFF<<16)
#define USBC_NPTXQSPCAVAIL_SHIFT    (16)
#define USBC_NPTXQTOP(n)            (((n)&0x7F)<<24)
#define USBC_NPTXQTOP_MASK          (0x7F<<24)
#define USBC_NPTXQTOP_SHIFT         (24)

//GPVNDCTL
#define USBC_GPVNDCTL_DISULPIDRVR                    (1<<31)
#define USBC_GPVNDCTL_DISULPIDRVR_MASK                    (1<<31)
#define USBC_GPVNDCTL_DISULPIDRVR_SHIFT                    (31)
#define USBC_GPVNDCTL_VSTSDONE                    (1<<27)
#define USBC_GPVNDCTL_VSTSDONE_MASK                    (1<<27)
#define USBC_GPVNDCTL_VSTSDONE_SHIFT                    (27)
#define USBC_GPVNDCTL_VSTSBSY                    (1<<26)
#define USBC_GPVNDCTL_VSTSBSY_MASK                    (1<<26)
#define USBC_GPVNDCTL_VSTSBSY_SHIFT                    (26)
#define USBC_GPVNDCTL_NEWREGREQ                    (1<<25)
#define USBC_GPVNDCTL_NEWREGREQ_MASK                    (1<<25)
#define USBC_GPVNDCTL_NEWREGREQ_SHIFT                    (25)
#define USBC_GPVNDCTL_REGWR                    (1<<22)
#define USBC_GPVNDCTL_REGWR_MASK                    (1<<22)
#define USBC_GPVNDCTL_REGWR_SHIFT                    (22)
#define USBC_GPVNDCTL_REGADDR(n)                    (((n)&0x3f)<<16)
#define USBC_GPVNDCTL_REGADDR_MASK                    (0x3f<<16)
#define USBC_GPVNDCTL_REGADDR_SHIFT                    (16)
#define USBC_GPVNDCTL_EXTREGADDR(n)                    (((n)&0xff)<<8)
#define USBC_GPVNDCTL_EXTREGADDR_MASK                    (0xff<<8)
#define USBC_GPVNDCTL_EXTREGADDR_SHIFT                    (8)
#define USBC_GPVNDCTL_REGDATA(n)                    (((n)&0xff)<<0)
#define USBC_GPVNDCTL_REGDATA_MASK                    (0xff<<0)
#define USBC_GPVNDCTL_REGDATA_SHIFT                    (0)

//GUID
#define USBC_USERID(n)              (((n)&0xFFFFFFFF)<<0)
#define USBC_USERID_MASK            (0xFFFFFFFF<<0)
#define USBC_USERID_SHIFT           (0)

//GSNPSID
#define USBC_SYNOPSYSID(n)          (((n)&0xFFFFFFFF)<<0)
#define USBC_SYNOPSYSID_MASK        (0xFFFFFFFF<<0)
#define USBC_SYNOPSYSID_SHIFT       (0)

//GHWCFG1
#define USBC_EPDIR(n)               (((n)&0xFFFFFFFF)<<0)
#define USBC_EPDIR_MASK             (0xFFFFFFFF<<0)
#define USBC_EPDIR_SHIFT            (0)

//GHWCFG2
#define USBC_OTGMODE(n)             (((n)&7)<<0)
#define USBC_OTGMODE_MASK           (7<<0)
#define USBC_OTGMODE_SHIFT          (0)
#define USBC_OTGARCH(n)             (((n)&3)<<3)
#define USBC_OTGARCH_MASK           (3<<3)
#define USBC_OTGARCH_SHIFT          (3)
#define USBC_SINGPNT                (1<<5)
#define USBC_SINGPNT_MASK           (1<<5)
#define USBC_SINGPNT_SHIFT          (5)
#define USBC_HSPHYTYPE(n)           (((n)&3)<<6)
#define USBC_HSPHYTYPE_MASK         (3<<6)
#define USBC_HSPHYTYPE_SHIFT        (6)
#define USBC_FSPHYTYPE(n)           (((n)&3)<<8)
#define USBC_FSPHYTYPE_MASK         (3<<8)
#define USBC_FSPHYTYPE_SHIFT        (8)
#define USBC_NUMDEVEPS(n)           (((n)&15)<<10)
#define USBC_NUMDEVEPS_MASK         (15<<10)
#define USBC_NUMDEVEPS_SHIFT        (10)
#define USBC_NUMHSTCHNL(n)          (((n)&15)<<14)
#define USBC_NUMHSTCHNL_MASK        (15<<14)
#define USBC_NUMHSTCHNL_SHIFT       (14)
#define USBC_PERIOSUPPORT           (1<<18)
#define USBC_PERIOSUPPORT_MASK      (1<<18)
#define USBC_PERIOSUPPORT_SHIFT     (18)
#define USBC_DYNFIFOSIZING          (1<<19)
#define USBC_DYNFIFOSIZING_MASK     (1<<19)
#define USBC_DYNFIFOSIZING_SHIFT    (19)
#define USBC_NPTXQDEPTH(n)          (((n)&3)<<22)
#define USBC_NPTXQDEPTH_MASK        (3<<22)
#define USBC_NPTXQDEPTH_SHIFT       (22)
#define USBC_PTXQDEPTH(n)           (((n)&3)<<24)
#define USBC_PTXQDEPTH_MASK         (3<<24)
#define USBC_PTXQDEPTH_SHIFT        (24)
#define USBC_TKNQDEPTH(n)           (((n)&31)<<26)
#define USBC_TKNQDEPTH_MASK         (31<<26)
#define USBC_TKNQDEPTH_SHIFT        (26)

//GHWCFG3
#define USBC_XFERSIZEWIDTH(n)       (((n)&15)<<0)
#define USBC_XFERSIZEWIDTH_MASK     (15<<0)
#define USBC_XFERSIZEWIDTH_SHIFT    (0)
#define USBC_PKTSIZEWIDTH(n)        (((n)&7)<<4)
#define USBC_PKTSIZEWIDTH_MASK      (7<<4)
#define USBC_PKTSIZEWIDTH_SHIFT     (4)
#define USBC_OTGEN                  (1<<7)
#define USBC_OTGEN_MASK             (1<<7)
#define USBC_OTGEN_SHIFT            (7)
#define USBC_I2CINTSEL              (1<<8)
#define USBC_I2CINTSEL_MASK         (1<<8)
#define USBC_I2CINTSEL_SHIFT        (8)
//#define USBC_VNDCTLSUPT             (1<<9)
#define USBC_VNDCTLSUPT(n)             (((n)&0x1)<<9)
#define USBC_VNDCTLSUPT_MASK        (1<<9)
#define USBC_VNDCTLSUPT_SHIFT       (9)
#define USBC_OPTFEATURE             (1<<10)
#define USBC_OPTFEATURE_MASK        (1<<10)
#define USBC_OPTFEATURE_SHIFT       (10)
#define USBC_RSTTYPE                (1<<11)
#define USBC_RSTTYPE_MASK           (1<<11)
#define USBC_RSTTYPE_SHIFT          (11)
#define USBC_DFIFODEPTH(n)          (((n)&0xFFFF)<<16)
#define USBC_DFIFODEPTH_MASK        (0xFFFF<<16)
#define USBC_DFIFODEPTH_SHIFT       (16)

//GHWCFG4
#define USBC_NUMDEVPERIOEPS(n)      (((n)&15)<<0)
#define USBC_NUMDEVPERIOEPS_MASK    (15<<0)
#define USBC_NUMDEVPERIOEPS_SHIFT   (0)
#define USBC_ENABLEPWROPT           (1<<4)
#define USBC_ENABLEPWROPT_MASK      (1<<4)
#define USBC_ENABLEPWROPT_SHIFT     (4)
#define USBC_AHBFREQ                (1<<5)
#define USBC_AHBFREQ_MASK           (1<<5)
#define USBC_AHBFREQ_SHIFT          (5)
#define USBC_PHYDATAWIDTH(n)        (((n)&3)<<14)
#define USBC_PHYDATAWIDTH_MASK      (3<<14)
#define USBC_PHYDATAWIDTH_SHIFT     (14)
#define USBC_NUMCTLEPS(n)           (((n)&15)<<16)
#define USBC_NUMCTLEPS_MASK         (15<<16)
#define USBC_NUMCTLEPS_SHIFT        (16)
#define USBC_IDDIGFLTR              (1<<20)
#define USBC_IDDIGFLTR_MASK         (1<<20)
#define USBC_IDDIGFLTR_SHIFT        (20)
#define USBC_VBUSVALIDFLTR          (1<<21)
#define USBC_VBUSVALIDFLTR_MASK     (1<<21)
#define USBC_VBUSVALIDFLTR_SHIFT    (21)
#define USBC_AVALIDFLTR             (1<<22)
#define USBC_AVALIDFLTR_MASK        (1<<22)
#define USBC_AVALIDFLTR_SHIFT       (22)
#define USBC_BVALIDFLTR             (1<<23)
#define USBC_BVALIDFLTR_MASK        (1<<23)
#define USBC_BVALIDFLTR_SHIFT       (23)
#define USBC_SESSENDFLTR            (1<<24)
#define USBC_SESSENDFLTR_MASK       (1<<24)
#define USBC_SESSENDFLTR_SHIFT      (24)
#define USBC_DEFIFOMODE             (1<<25)
#define USBC_DEFIFOMODE_MASK        (1<<25)
#define USBC_DEFIFOMODE_SHIFT       (25)
#define USBC_INEPS(n)               (((n)&15)<<26)
#define USBC_INEPS_MASK             (15<<26)
#define USBC_INEPS_SHIFT            (26)

//GLPMCFG
#define USBC_LPMCAP                 (1<<0)
#define USBC_LPMCAP_MASK            (1<<0)
#define USBC_LPMCAP_SHIFT           (0)
#define USBC_APPL1RES               (1<<1)
#define USBC_APPL1RES_MASK          (1<<1)
#define USBC_APPL1RES_SHIFT         (1)
#define USBC_HIRD(n)                (((n)&15)<<2)
#define USBC_HIRD_MASK              (15<<2)
#define USBC_HIRD_SHIFT             (2)
#define USBC_BREMOTEWAKE            (1 << 6)
#define USBC_BREMOTEWAKE_MASK       (1 << 6)
#define USBC_BREMOTEWAKE_SHIFT      (6)
#define USBC_ENBLSLPM               (1<<7)
#define USBC_ENBLSLPM_MASK          (1<<7)
#define USBC_ENBLSLPM_SHIFT         (7)
#define USBC_HIRD_THRES(n)          (((n)&15)<<8)
#define USBC_HIRD_THRES_MASK        (15<<8)
#define USBC_HIRD_THRES_SHIFT       (8)
#define USBC_HIRD_THRES_BIT4        (1 << 12)
#define USBC_HIRD_THRES_BIT4_MASK   (1 << 12)
#define USBC_HIRD_THRES_BIT4_SHIFT  (12)
#define USBC_COREL1RES(n)           (((n)&3)<<13)
#define USBC_COREL1RES_MASK         (3<<13)
#define USBC_COREL1RES_SHIFT        (13)
#define USBC_SLPSTS                 (1<<15)
#define USBC_SLPSTS_MASK            (1<<15)
#define USBC_SLPSTS_SHIFT           (15)
#define USBC_L1RESUMEOK             (1<<16)
#define USBC_L1RESUMEOK_MASK        (1<<16)
#define USBC_L1RESUMEOK_SHIFT       (16)
#define USBC_ENBESL                 (1<<28)
#define USBC_ENBESL_MASK            (1<<28)
#define USBC_ENBESL_SHIFT           (28)
#define USBC_RSTRSLPSTS             (1<<29)
#define USBC_RSTRSLPSTS_MASK        (1<<29)
#define USBC_RSTRSLPSTS_SHIFT       (29)

//GDFIFOCFG
#define USBC_GDFIFOCFG_SHIFT        0
#define USBC_GDFIFOCFG_MASK         (0xFFFF << USBC_GDFIFOCFG_SHIFT)
#define USBC_GDFIFOCFG(n)           BITFIELD_VAL(USBC_GDFIFOCFG, n)
#define USBC_EPINFOBASEADDR_SHIFT   16
#define USBC_EPINFOBASEADDR_MASK    (0xFFFF << USBC_EPINFOBASEADDR_SHIFT)
#define USBC_EPINFOBASEADDR(n)      BITFIELD_VAL(USBC_EPINFOBASEADDR, n)

//DPTXFSIZn
#define USBC_DPTXFSTADDR(n)          (((n)&0xFFFF)<<0)
#define USBC_DPTXFSTADDR_MASK        (0xFFFF<<0)
#define USBC_DPTXFSTADDR_SHIFT       (0)
#define USBC_DPTXFSIZE(n)             (((n)&0xFFFF)<<16)
#define USBC_DPTXFSIZE_MASK           (0xFFFF<<16)
#define USBC_DPTXFSIZE_SHIFT          (16)

//DIEPTXFn
#define USBC_INEPNTXFSTADDR(n)      (((n)&0xFFFF)<<0)
#define USBC_INEPNTXFSTADDR_MASK    (0xFFFF<<0)
#define USBC_INEPNTXFSTADDR_SHIFT   (0)
#define USBC_INEPNTXFDEP(n)         (((n)&0xFFFF)<<16)
#define USBC_INEPNTXFDEP_MASK       (0xFFFF<<16)
#define USBC_INEPNTXFDEP_SHIFT      (16)

//HPTXFSIZ
#define USBC_HPTXFSIZ_PTXFSTADDR(n)      (((n)&0xFFFF)<<0)
#define USBC_HPTXFSIZ_PTXFSTADDR_MASK    (0xFFFF<<0)
#define USBC_HPTXFSIZ_PTXFSTADDR_SHIFT   (0)
#define USBC_HPTXFSIZ_PTXFSIZE(n)         (((n)&0xFFFF)<<16)
#define USBC_HPTXFSIZ_PTXFSIZE_MASK       (0xFFFF<<16)
#define USBC_HPTXFSIZ_PTXFSIZE_SHIFT      (16)

//HCFG
#define USBC_HCFG_MODECHTIMEN                    (1<<31)
#define USBC_HCFG_MODECHTIMEN_MASK                    (1<<31)
#define USBC_HCFG_MODECHTIMEN_SHIFT                    (31)
#define USBC_HCFG_PERSCHEDENA                    (1<<26)
#define USBC_HCFG_PERSCHEDENA_MASK                    (1<<26)
#define USBC_HCFG_PERSCHEDENA_SHIFT                    (26)
#define USBC_HCFG_FRLISTEN(n)                    (((n)&0x3)<<24)
#define USBC_HCFG_FRLISTEN_MASK                    (0x3<<24)
#define USBC_HCFG_FRLISTEN_SHIFT                    (24)
#define USBC_HCFG_DESCDMA                    (1<<23)
#define USBC_HCFG_DESCDMA_MASK                    (1<<23)
#define USBC_HCFG_DESCDMA_SHIFT                    (23)
#define USBC_HCFG_RESVALID(n)                    (((n)&0xff)<<8)
#define USBC_HCFG_RESVALID_MASK                    (0xff<<8)
#define USBC_HCFG_RESVALID_SHIFT                    (8)
#define USBC_HCFG_ENA32KHZS                    (1<<7)
#define USBC_HCFG_ENA32KHZS_MASK                    (1<<7)
#define USBC_HCFG_ENA32KHZS_SHIFT                    (7)
#define USBC_HCFG_FSLSSUPP                    (1<<2)
#define USBC_HCFG_FSLSSUPP_MASK                    (1<<2)
#define USBC_HCFG_FSLSSUPP_SHIFT                    (2)
#define USBC_HCFG_FSLSPCLKSEL(n)                    (((n)&0x3)<<0)
#define USBC_HCFG_FSLSPCLKSEL_MASK                    (0x3<<0)
#define USBC_HCFG_FSLSPCLKSEL_SHIFT                    (0)

//HFIR
#define USBC_HFIR_HFIRRLDCTRL                    (1<<16)
#define USBC_HFIR_HFIRRLDCTRL_MASK                    (1<<16)
#define USBC_HFIR_HFIRRLDCTRL_SHIFT                    (16)
#define USBC_HFIR_FRINT(n)                    (((n)&0xffff)<<0)
#define USBC_HFIR_FRINT_MASK                    (0xffff<<0)
#define USBC_HFIR_FRINT_SHIFT                    (0)

//HFNUM
#define USBC_HFNUM_FRREM(n)                    (((n)&0xffff)<<16)
#define USBC_HFNUM_FRREM_MASK                    (0xffff<<16)
#define USBC_HFNUM_FRREM_SHIFT                    (16)
#define USBC_HFNUM_FRNUM(n)                    (((n)&0xffff)<<0)
#define USBC_HFNUM_FRNUM_MASK                    (0xffff<<0)
#define USBC_HFNUM_FRNUM_SHIFT                    (0)

//HPTXSTS
#define USBC_HPTXSTS_PTXQTOP(n)                    (((n)&0xff)<<24)
#define USBC_HPTXSTS_PTXQTOP_MASK                    (0xff<<24)
#define USBC_HPTXSTS_PTXQTOP_SHIFT                    (24)
#define USBC_HPTXSTS_PTXQSPCAVAIL(n)                    (((n)&0xff)<<16)
#define USBC_HPTXSTS_PTXQSPCAVAIL_MASK                    (0xff<<16)
#define USBC_HPTXSTS_PTXQSPCAVAIL_SHIFT                    (16)
#define USBC_HPTXSTS_PTXFSPCAVAIL(n)                    (((n)&0xffff)<<0)
#define USBC_HPTXSTS_PTXFSPCAVAIL_MASK                    (0xffff<<0)
#define USBC_HPTXSTS_PTXFSPCAVAIL_SHIFT                    (0)

//HAINT
#define USBC_HAINT_HAINT(n)                    (((n)&0xffff)<<0)
#define USBC_HAINT_HAINT_MASK                    (0xffff<<0)
#define USBC_HAINT_HAINT_SHIFT                    (0)

//HAINTMSK
#define USBC_HAINTMSK_HAINTMSK(n)                    (((n)&0xffff)<<0)
#define USBC_HAINTMSK_HAINTMSK_MASK                    (0xffff<<0)
#define USBC_HAINTMSK_HAINTMSK_SHIFT                    (0)

//HPRT
#define USBC_HPRT_PRTSPD(n)                    (((n)&0x3)<<17)
#define USBC_HPRT_PRTSPD_MASK                    (0x3<<17)
#define USBC_HPRT_PRTSPD_SHIFT                    (17)
#define USBC_HPRT_PRTTSTCTL(n)                    (((n)&0xf)<<13)
#define USBC_HPRT_PRTTSTCTL_MASK                    (0xf<<13)
#define USBC_HPRT_PRTTSTCTL_SHIFT                    (13)
#define USBC_HPRT_PRTPWR                    (1<<12)
#define USBC_HPRT_PRTPWR_MASK                    (1<<12)
#define USBC_HPRT_PRTPWR_SHIFT                    (12)
#define USBC_HPRT_PRTLNSTS(n)                    (((n)&0x3)<<10)
#define USBC_HPRT_PRTLNSTS_MASK                    (0x3<<10)
#define USBC_HPRT_PRTLNSTS_SHIFT                    (10)
#define USBC_HPRT_PRTRST                    (1<<8)
#define USBC_HPRT_PRTRST_MASK                    (1<<8)
#define USBC_HPRT_PRTRST_SHIFT                    (8)
#define USBC_HPRT_PRTSUSP                    (1<<7)
#define USBC_HPRT_PRTSUSP_MASK                    (1<<7)
#define USBC_HPRT_PRTSUSP_SHIFT                    (7)
#define USBC_HPRT_PRTRES                    (1<<6)
#define USBC_HPRT_PRTRES_MASK                    (1<<6)
#define USBC_HPRT_PRTRES_SHIFT                    (6)
#define USBC_HPRT_PRTOVRCURRCHNG                    (1<<5)
#define USBC_HPRT_PRTOVRCURRCHNG_MASK                    (1<<5)
#define USBC_HPRT_PRTOVRCURRCHNG_SHIFT                    (5)
#define USBC_HPRT_PRTOVRCURRACT                    (1<<4)
#define USBC_HPRT_PRTOVRCURRACT_MASK                    (1<<4)
#define USBC_HPRT_PRTOVRCURRACT_SHIFT                    (4)
#define USBC_HPRT_PRTENCHNG                    (1<<3)
#define USBC_HPRT_PRTENCHNG_MASK                    (1<<3)
#define USBC_HPRT_PRTENCHNG_SHIFT                    (3)
#define USBC_HPRT_PRTENA                    (1<<2)
#define USBC_HPRT_PRTENA_MASK                    (1<<2)
#define USBC_HPRT_PRTENA_SHIFT                    (2)
#define USBC_HPRT_PRTCONNDET                    (1<<1)
#define USBC_HPRT_PRTCONNDET_MASK                    (1<<1)
#define USBC_HPRT_PRTCONNDET_SHIFT                    (1)
#define USBC_HPRT_PRTCONNSTS                    (1<<0)
#define USBC_HPRT_PRTCONNSTS_MASK                    (1<<0)
#define USBC_HPRT_PRTCONNSTS_SHIFT                    (0)

//HCCHARN
#define USBC_HCCHARN_CHENA                    (1<<31)
#define USBC_HCCHARN_CHENA_MASK                    (1<<31)
#define USBC_HCCHARN_CHENA_SHIFT                    (31)
#define USBC_HCCHARN_CHDIS                    (1<<30)
#define USBC_HCCHARN_CHDIS_MASK                    (1<<30)
#define USBC_HCCHARN_CHDIS_SHIFT                    (30)
#define USBC_HCCHARN_ODDFRM                    (1<<29)
#define USBC_HCCHARN_ODDFRM_MASK                    (1<<29)
#define USBC_HCCHARN_ODDFRM_SHIFT                    (29)
#define USBC_HCCHARN_DEVADDR(n)                    (((n)&0x7f)<<22)
#define USBC_HCCHARN_DEVADDR_MASK                    (0x7f<<22)
#define USBC_HCCHARN_DEVADDR_SHIFT                    (22)
#define USBC_HCCHARN_EC(n)                    (((n)&0x3)<<20)
#define USBC_HCCHARN_EC_MASK                    (0x3<<20)
#define USBC_HCCHARN_EC_SHIFT                    (20)
#define USBC_HCCHARN_EPTYPE(n)                    (((n)&0x3)<<18)
#define USBC_HCCHARN_EPTYPE_MASK                    (0x3<<18)
#define USBC_HCCHARN_EPTYPE_SHIFT                    (18)
#define USBC_HCCHARN_LSPDDEV                    (1<<17)
#define USBC_HCCHARN_LSPDDEV_MASK                    (1<<17)
#define USBC_HCCHARN_LSPDDEV_SHIFT                    (17)
#define USBC_HCCHARN_EPDIR                    (1<<15)
#define USBC_HCCHARN_EPDIR_MASK                    (1<<15)
#define USBC_HCCHARN_EPDIR_SHIFT                    (15)
#define USBC_HCCHARN_EPNUM(n)                    (((n)&0xf)<<11)
#define USBC_HCCHARN_EPNUM_MASK                    (0xf<<11)
#define USBC_HCCHARN_EPNUM_SHIFT                    (11)
#define USBC_HCCHARN_MPS(n)                    (((n)&0x7ff)<<0)
#define USBC_HCCHARN_MPS_MASK                    (0x7ff<<0)
#define USBC_HCCHARN_MPS_SHIFT                    (0)

//HCSPLTN
#define USBC_HCSPLTN_SPLTENA                    (1<<31)
#define USBC_HCSPLTN_SPLTENA_MASK                    (1<<31)
#define USBC_HCSPLTN_SPLTENA_SHIFT                    (31)
#define USBC_HCSPLTN_COMPSPLT                    (1<<16)
#define USBC_HCSPLTN_COMPSPLT_MASK                    (1<<16)
#define USBC_HCSPLTN_COMPSPLT_SHIFT                    (16)
#define USBC_HCSPLTN_EPDIR(n)                    (((n)&0x3)<<14)
#define USBC_HCSPLTN_EPDIR_MASK                    (0x3<<14)
#define USBC_HCSPLTN_EPDIR_SHIFT                    (14)
#define USBC_HCSPLTN_EPNUM(n)                    (((n)&0x7f)<<7)
#define USBC_HCSPLTN_EPNUM_MASK                    (0x7f<<7)
#define USBC_HCSPLTN_EPNUM_SHIFT                    (7)
#define USBC_HCSPLTN_MPS(n)                    (((n)&0x7f)<<0)
#define USBC_HCSPLTN_MPS_MASK                    (0x7f<<0)
#define USBC_HCSPLTN_MPS_SHIFT                    (0)

//HCINTN
#define USBC_HCINTN_DESC_LST_ROLLINTR                    (1<<13)
#define USBC_HCINTN_DESC_LST_ROLLINTR_MASK                    (1<<13)
#define USBC_HCINTN_DESC_LST_ROLLINTR_SHIFT                    (13)
#define USBC_HCINTN_XCS_XACT_ERR                    (1<<12)
#define USBC_HCINTN_XCS_XACT_ERR_MASK                    (1<<12)
#define USBC_HCINTN_XCS_XACT_ERR_SHIFT                    (12)
#define USBC_HCINTN_BNAINTR                    (1<<11)
#define USBC_HCINTN_BNAINTR_MASK                    (1<<11)
#define USBC_HCINTN_BNAINTR_SHIFT                    (11)
#define USBC_HCINTN_DATATGLERR                    (1<<10)
#define USBC_HCINTN_DATATGLERR_MASK                    (1<<10)
#define USBC_HCINTN_DATATGLERR_SHIFT                    (10)
#define USBC_HCINTN_FRMOVRUN                    (1<<9)
#define USBC_HCINTN_FRMOVRUN_MASK                    (1<<9)
#define USBC_HCINTN_FRMOVRUN_SHIFT                    (9)
#define USBC_HCINTN_BBLERR                    (1<<8)
#define USBC_HCINTN_BBLERR_MASK                    (1<<8)
#define USBC_HCINTN_BBLERR_SHIFT                    (8)
#define USBC_HCINTN_XACTERR                    (1<<7)
#define USBC_HCINTN_XACTERR_MASK                    (1<<7)
#define USBC_HCINTN_XACTERR_SHIFT                    (7)
#define USBC_HCINTN_NYET                    (1<<6)
#define USBC_HCINTN_NYET_MASK                    (1<<6)
#define USBC_HCINTN_NYET_SHIFT                    (6)
#define USBC_HCINTN_ACK                    (1<<5)
#define USBC_HCINTN_ACK_MASK                    (1<<5)
#define USBC_HCINTN_ACK_SHIFT                    (5)
#define USBC_HCINTN_NAK                    (1<<4)
#define USBC_HCINTN_NAK_MASK                    (1<<4)
#define USBC_HCINTN_NAK_SHIFT                    (4)
#define USBC_HCINTN_STALL                    (1<<3)
#define USBC_HCINTN_STALL_MASK                    (1<<3)
#define USBC_HCINTN_STALL_SHIFT                    (3)
#define USBC_HCINTN_AHBERR                    (1<<2)
#define USBC_HCINTN_AHBERR_MASK                    (1<<2)
#define USBC_HCINTN_AHBERR_SHIFT                    (2)
#define USBC_HCINTN_CHHLTD                    (1<<1)
#define USBC_HCINTN_CHHLTD_MASK                    (1<<1)
#define USBC_HCINTN_CHHLTD_SHIFT                    (1)
#define USBC_HCINTN_XFERCOMPL                    (1<<0)
#define USBC_HCINTN_XFERCOMPL_MASK                    (1<<0)
#define USBC_HCINTN_XFERCOMPL_SHIFT                    (0)

//HCINTMSKN
#define USBC_HCINTMSKN_DESC_LST_ROLLINTRMSK                    (1<<13)
#define USBC_HCINTMSKN_DESC_LST_ROLLINTRMSK_MASK                    (1<<13)
#define USBC_HCINTMSKN_DESC_LST_ROLLINTRMSK_SHIFT                    (13)
#define USBC_HCINTMSKN_BNAINTRMSK                    (1<<11)
#define USBC_HCINTMSKN_BNAINTRMSK_MASK                    (1<<11)
#define USBC_HCINTMSKN_BNAINTRMSK_SHIFT                    (11)
#define USBC_HCINTMSKN_DATATGLERRMSK                    (1<<10)
#define USBC_HCINTMSKN_DATATGLERRMSK_MASK                    (1<<10)
#define USBC_HCINTMSKN_DATATGLERRMSK_SHIFT                    (10)
#define USBC_HCINTMSKN_FRMOVRUNMSK                    (1<<9)
#define USBC_HCINTMSKN_FRMOVRUNMSK_MASK                    (1<<9)
#define USBC_HCINTMSKN_FRMOVRUNMSK_SHIFT                    (9)
#define USBC_HCINTMSKN_BBLERRMSK                    (1<<8)
#define USBC_HCINTMSKN_BBLERRMSK_MASK                    (1<<8)
#define USBC_HCINTMSKN_BBLERRMSK_SHIFT                    (8)
#define USBC_HCINTMSKN_XACTERRMSK                    (1<<7)
#define USBC_HCINTMSKN_XACTERRMSK_MASK                    (1<<7)
#define USBC_HCINTMSKN_XACTERRMSK_SHIFT                    (7)
#define USBC_HCINTMSKN_NYETMSK                    (1<<6)
#define USBC_HCINTMSKN_NYETMSK_MASK                    (1<<6)
#define USBC_HCINTMSKN_NYETMSK_SHIFT                    (6)
#define USBC_HCINTMSKN_ACKMSK                    (1<<5)
#define USBC_HCINTMSKN_ACKMSK_MASK                    (1<<5)
#define USBC_HCINTMSKN_ACKMSK_SHIFT                    (5)
#define USBC_HCINTMSKN_NAKMSK                    (1<<4)
#define USBC_HCINTMSKN_NAKMSK_MASK                    (1<<4)
#define USBC_HCINTMSKN_NAKMSK_SHIFT                    (4)
#define USBC_HCINTMSKN_STALLMSK                    (1<<3)
#define USBC_HCINTMSKN_STALLMSK_MASK                    (1<<3)
#define USBC_HCINTMSKN_STALLMSK_SHIFT                    (3)
#define USBC_HCINTMSKN_AHBERRMSK                    (1<<2)
#define USBC_HCINTMSKN_AHBERRMSK_MASK                    (1<<2)
#define USBC_HCINTMSKN_AHBERRMSK_SHIFT                    (2)
#define USBC_HCINTMSKN_CHHLTDMSK                    (1<<1)
#define USBC_HCINTMSKN_CHHLTDMSK_MASK                    (1<<1)
#define USBC_HCINTMSKN_CHHLTDMSK_SHIFT                    (1)
#define USBC_HCINTMSKN_XFERCOMPLMSK                    (1<<0)
#define USBC_HCINTMSKN_XFERCOMPLMSK_MASK                    (1<<0)
#define USBC_HCINTMSKN_XFERCOMPLMSK_SHIFT                    (0)

//HCTSIZN
//SCATTER/GATHER DMA MODE
#define USBC_HCTSIZN_DOPNG                    (1<<31)
#define USBC_HCTSIZN_DOPNG_MASK                    (1<<31)
#define USBC_HCTSIZN_DOPNG_SHIFT                    (31)
#define USBC_HCTSIZN_PID(n)                    (((n)&0x3)<<29)
#define USBC_HCTSIZN_PID_MASK                    (0x3<<29)
#define USBC_HCTSIZN_PID_SHIFT                    (29)
#define USBC_HCTSIZN_NTD(n)                    (((n)&0xff)<<8)
#define USBC_HCTSIZN_NTD_MASK                    (0xff<<8)
#define USBC_HCTSIZN_NTD_SHIFT                    (8)
#define USBC_HCTSIZN_SCHED(n)                    (((n)&0xff)<<0)
#define USBC_HCTSIZN_SCHED_MASK                    (0xff<<0)
#define USBC_HCTSIZN_SCHED_SHIFT                    (0)

//HCTSIZN
//NON-SCATTER/GATHER DMA MODE
#define USBC_HCTSIZN_DOPNG                    (1<<31)
#define USBC_HCTSIZN_DOPNG_MASK                    (1<<31)
#define USBC_HCTSIZN_DOPNG_SHIFT                    (31)
#define USBC_HCTSIZN_PID(n)                    (((n)&0x3)<<29)
#define USBC_HCTSIZN_PID_MASK                    (0x3<<29)
#define USBC_HCTSIZN_PID_SHIFT                    (29)
#define USBC_HCTSIZN_PKTCNT(n)                    (((n)&0x3ff)<<19)
#define USBC_HCTSIZN_PKTCNT_MASK                    (0x3ff<<19)
#define USBC_HCTSIZN_PKTCNT_SHIFT                    (19)
#define USBC_HCTSIZN_XFERSIZE(n)                    (((n)&0x7ffff)<<0)
#define USBC_HCTSIZN_XFERSIZE_MASK                    (0x7ffff<<0)
#define USBC_HCTSIZN_XFERSIZE_SHIFT                    (0)

//HCDMAN
//BUFFER DMA MODE
#define USBC_HCDMAN_DMAADDR(n)                    (((n)&0xffffffff)<<0)
#define USBC_HCDMAN_DMAADDR_MASK                    (0xffffffff<<0)
#define USBC_HCDMAN_DMAADDR_SHIFT                    (0)

//HCDMABN
#define USBC_HCDMABN_DMAADDR(n)                    (((n)&0xffffffff)<<0)
#define USBC_HCDMABN_DMAADDR_MASK                    (0xffffffff<<0)
#define USBC_HCDMABN_DMAADDR_SHIFT                    (0)

//DCFG
#define USBC_DEVSPD(n)              (((n)&3)<<0)
#define USBC_DEVSPD_MASK            (3<<0)
#define USBC_DEVSPD_SHIFT           (0)
#define USBC_NZSTSOUTHSHK           (1<<2)
#define USBC_NZSTSOUTHSHK_MASK      (1<<2)
#define USBC_NZSTSOUTHSHK_SHIFT     (2)
#define USBC_DEVADDR(n)             (((n)&0x7F)<<4)
#define USBC_DEVADDR_MASK           (0x7F<<4)
#define USBC_DEVADDR_SHIFT          (4)
#define USBC_PERFRINT(n)            (((n)&3)<<11)
#define USBC_PERFRINT_MASK          (3<<11)
#define USBC_PERFRINT_SHIFT         (11)
#define USBC_EPMISCNT(n)            (((n)&31)<<18)
#define USBC_EPMISCNT_MASK          (31<<18)
#define USBC_EPMISCNT_SHIFT         (18)

//DCTL
#define USBC_RMTWKUPSIG             (1<<0)
#define USBC_RMTWKUPSIG_MASK        (1<<0)
#define USBC_RMTWKUPSIG_SHIFT       (0)
#define USBC_SFTDISCON              (1<<1)
#define USBC_SFTDISCON_MASK         (1<<1)
#define USBC_SFTDISCON_SHIFT        (1)
#define USBC_GNPINNAKSTS            (1<<2)
#define USBC_GNPINNAKSTS_MASK       (1<<2)
#define USBC_GNPINNAKSTS_SHIFT      (2)
#define USBC_GOUTNAKSTS             (1<<3)
#define USBC_GOUTNAKSTS_MASK        (1<<3)
#define USBC_GOUTNAKSTS_SHIFT       (3)
#define USBC_TSTCTL(n)              (((n)&7)<<4)
#define USBC_TSTCTL_MASK            (7<<4)
#define USBC_TSTCTL_SHIFT           (4)
#define USBC_SGNPINNAK              (1<<7)
#define USBC_SGNPINNAK_MASK         (1<<7)
#define USBC_SGNPINNAK_SHIFT        (7)
#define USBC_CGNPINNAK              (1<<8)
#define USBC_CGNPINNAK_MASK         (1<<8)
#define USBC_CGNPINNAK_SHIFT        (8)
#define USBC_SGOUTNAK               (1<<9)
#define USBC_SGOUTNAK_MASK          (1<<9)
#define USBC_SGOUTNAK_SHIFT         (9)
#define USBC_CGOUTNAK               (1<<10)
#define USBC_CGOUTNAK_MASK          (1<<10)
#define USBC_CGOUTNAK_SHIFT         (10)
#define USBC_PWRONPRGDONE           (1<<11)
#define USBC_PWRONPRGDONE_MASK      (1<<11)
#define USBC_PWRONPRGDONE_SHIFT     (11)
#define USBC_GMC(n)                 (((n)&3)<<12)
#define USBC_GMC_MASK               (3<<12)
#define USBC_GMC_SHIFT              (12)
#define USBC_IGNRFRMNUM             (1<<15)
#define USBC_IGNRFRMNUM_MASK        (1<<15)
#define USBC_IGNRFRMNUM_SHIFT       (15)

//DSTS
#define USBC_SUSPSTS                (1<<0)
#define USBC_SUSPSTS_MASK           (1<<0)
#define USBC_SUSPSTS_SHIFT          (0)
#define USBC_ENUMSPD(n)             (((n)&3)<<1)
#define USBC_ENUMSPD_MASK           (3<<1)
#define USBC_ENUMSPD_SHIFT          (1)
#define USBC_ERRTICERR              (1<<3)
#define USBC_ERRTICERR_MASK         (1<<3)
#define USBC_ERRTICERR_SHIFT        (3)
#define USBC_SOFFN(n)               (((n)&0x3FFF)<<8)
#define USBC_SOFFN_MASK             (0x3FFF<<8)
#define USBC_SOFFN_SHIFT            (8)

//DIEPMSK
#define USBC_XFERCOMPLMSK           (1<<0)
#define USBC_XFERCOMPLMSK_MASK      (1<<0)
#define USBC_XFERCOMPLMSK_SHIFT     (0)
#define USBC_EPDISBLDMSK            (1<<1)
#define USBC_EPDISBLDMSK_MASK       (1<<1)
#define USBC_EPDISBLDMSK_SHIFT      (1)
#define USBC_AHBERRMSK              (1<<2)
#define USBC_AHBERRMSK_MASK         (1<<2)
#define USBC_AHBERRMSK_SHIFT        (2)
#define USBC_TIMEOUTMSK             (1<<3)
#define USBC_TIMEOUTMSK_MASK        (1<<3)
#define USBC_TIMEOUTMSK_SHIFT       (3)
#define USBC_INTKNTXFEMPMSK         (1<<4)
#define USBC_INTKNTXFEMPMSK_MASK    (1<<4)
#define USBC_INTKNTXFEMPMSK_SHIFT   (4)
#define USBC_INTKNEPMISMSK          (1<<5)
#define USBC_INTKNEPMISMSK_MASK     (1<<5)
#define USBC_INTKNEPMISMSK_SHIFT    (5)
#define USBC_INEPNAKEFFMSK          (1<<6)
#define USBC_INEPNAKEFFMSK_MASK     (1<<6)
#define USBC_INEPNAKEFFMSK_SHIFT    (6)
#define USBC_TXFEMPMSK              (1<<7)
#define USBC_TXFIFOUNDRNMSK         (1<<8)
#define USBC_TXFIFOUNDRNMSK_MASK    (1<<8)
#define USBC_TXFIFOUNDRNMSK_SHIFT   (8)
#define USBC_BNAINTRMSK             (1<<9)
#define USBC_PKTDRPSTSMSK           (1<<11)
#define USBC_BBLEERRINTRPTMSK       (1<<12)
#define USBC_NAKINTRPTMSK           (1<<13)
#define USBC_NYETINTRPTMSK          (1<<14)
#define USBC_STUPPKTRCVDMSK         (1<<15)

//DOEPMSK
//#define USBC_XFERCOMPLMSK         (1<<0)
//#define USBC_XFERCOMPLMSK_MASK    (1<<0)
//#define USBC_XFERCOMPLMSK_SHIFT   (0)
//#define USBC_EPDISBLDMSK          (1<<1)
//#define USBC_EPDISBLDMSK_MASK     (1<<1)
//#define USBC_EPDISBLDMSK_SHIFT    (1)
//#define USBC_AHBERRMSK            (1<<2)
//#define USBC_AHBERRMSK_MASK       (1<<2)
//#define USBC_AHBERRMSK_SHIFT      (2)
#define USBC_SETUPMSK               (1<<3)
#define USBC_SETUPMSK_MASK          (1<<3)
#define USBC_SETUPMSK_SHIFT         (3)
#define USBC_OUTTKNEPDISMSK         (1<<4)
#define USBC_OUTTKNEPDISMSK_MASK    (1<<4)
#define USBC_OUTTKNEPDISMSK_SHIFT   (4)
#define USBC_BACK2BACKSETUPMSK      (1<<6)
#define USBC_BACK2BACKSETUPMSK_MASK (1<<6)
#define USBC_BACK2BACKSETUPMSK_SHIFT (6)
#define USBC_OUTPKTERRMSK           (1<<8)
#define USBC_OUTPKTERRMSK_MASK      (1<<8)
#define USBC_OUTPKTERRMSK_SHIFT     (8)
//#define USBC_BNAINTRMSK           (1<<9)
//#define USBC_PKTDRPSTSMSK         (1<<11)
//#define USBC_BBLEERRINTRPTMSK     (1<<12)
//#define USBC_NAKINTRPTMSK         (1<<13)
//#define USBC_NYETINTRPTMSK        (1<<14)
//#define USBC_STUPPKTRCVDMSK       (1<<15)

//DAINT
#define USBC_INEPINT(n)             (((n)&0xFFFF)<<0)
#define USBC_INEPINT_MASK           (0xFFFF<<0)
#define USBC_INEPINT_SHIFT          (0)
#define USBC_OUTEPINT(n)            (((n)&0xFFFF)<<16)
#define USBC_OUTEPINT_MASK          (0xFFFF<<16)
#define USBC_OUTEPINT_SHIFT         (16)

//DAINTMSK
#define USBC_INEPMSK(n)             (((n)&0xFFFF)<<0)
#define USBC_INEPMSK_MASK           (0xFFFF<<0)
#define USBC_INEPMSK_SHIFT          (0)
#define USBC_OUTEPMSK(n)            (((n)&0xFFFF)<<16)
#define USBC_OUTEPMSK_MASK          (0xFFFF<<16)
#define USBC_OUTEPMSK_SHIFT         (16)

//DVBUSDIS
#define USBC_DVBUSDIS(n)            (((n)&0xFFFF)<<0)
#define USBC_DVBUSDIS_MASK          (0xFFFF<<0)
#define USBC_DVBUSDIS_SHIFT         (0)

//DVBUSPULSE
#define USBC_DVBUSPULSE(n)          (((n)&0xFFF)<<0)
#define USBC_DVBUSPULSE_MASK        (0xFFF<<0)
#define USBC_DVBUSPULSE_SHIFT       (0)

//DTHRCTL
#define USBC_NONISOTHREN            (1<<0)
#define USBC_NONISOTHREN_MASK       (1<<0)
#define USBC_NONISOTHREN_SHIFT      (0)
#define USBC_ISOTHREN               (1<<1)
#define USBC_ISOTHREN_MASK          (1<<1)
#define USBC_ISOTHREN_SHIFT         (1)
#define USBC_TXTHRLEN(n)            (((n)&0x1FF)<<2)
#define USBC_TXTHRLEN_MASK          (0x1FF<<2)
#define USBC_TXTHRLEN_SHIFT         (2)
#define USBC_RXTHREN                (1<<16)
#define USBC_RXTHREN_MASK           (1<<16)
#define USBC_RXTHREN_SHIFT          (16)
#define USBC_RXTHRLEN(n)            (((n)&0x1FF)<<17)
#define USBC_RXTHRLEN_MASK          (0x1FF<<17)
#define USBC_RXTHRLEN_SHIFT         (17)
#define USBC_ARBPRKEN               (1<<27)
#define USBC_ARBPRKEN_MASK          (1<<27)
#define USBC_ARBPRKEN_SHIFT         (27)

//DIEPEMPMSK
#define USBC_INEPTXFEMPMSK(n)       (((n)&0xFFFF)<<0)
#define USBC_INEPTXFEMPMSK_MASK     (0xFFFF<<0)
#define USBC_INEPTXFEMPMSK_SHIFT    (0)

//DIEPCTL0
#define USBC_EP0_MPS(n)             (((n)&3)<<0)
#define USBC_EP0_MPS_MASK           (3<<0)
#define USBC_EP0_MPS_SHIFT          (0)
#define USBC_NEXTEP(n)              (((n)&15)<<11)
#define USBC_NEXTEP_MASK            (15<<11)
#define USBC_NEXTEP_SHIFT           (11)
#define USBC_USBACTEP               (1<<15)
#define USBC_USBACTEP_MASK          (1<<15)
#define USBC_USBACTEP_SHIFT         (15)
#define USBC_NAKSTS                 (1<<17)
#define USBC_NAKSTS_MASK            (1<<17)
#define USBC_NAKSTS_SHIFT           (17)
#define USBC_EPTYPE(n)              (((n)&3)<<18)
#define USBC_EPTYPE_MASK            (3<<18)
#define USBC_EPTYPE_SHIFT           (18)
#define USBC_STALL                  (1<<21)
#define USBC_STALL_MASK             (1<<21)
#define USBC_STALL_SHIFT            (21)
#define USBC_EPTXFNUM(n)            (((n)&15)<<22)
#define USBC_EPTXFNUM_MASK          (15<<22)
#define USBC_EPTXFNUM_SHIFT         (22)
#define USBC_CNAK                   (1<<26)
#define USBC_CNAK_MASK              (1<<26)
#define USBC_CNAK_SHIFT             (26)
#define USBC_SNAK                   (1<<27)
#define USBC_SNAK_MASK              (1<<27)
#define USBC_SNAK_SHIFT             (27)
#define USBC_EPDIS                  (1<<30)
#define USBC_EPDIS_MASK             (1<<30)
#define USBC_EPDIS_SHIFT            (30)
#define USBC_EPENA                  (1<<31)
#define USBC_EPENA_MASK             (1<<31)
#define USBC_EPENA_SHIFT            (31)

//DIEPINT0
#define USBC_XFERCOMPL              (1<<0)
#define USBC_XFERCOMPL_MASK         (1<<0)
#define USBC_XFERCOMPL_SHIFT        (0)
#define USBC_EPDISBLD               (1<<1)
#define USBC_EPDISBLD_MASK          (1<<1)
#define USBC_EPDISBLD_SHIFT         (1)
#define USBC_AHBERR                 (1<<2)
#define USBC_AHBERR_MASK            (1<<2)
#define USBC_AHBERR_SHIFT           (2)
#define USBC_TIMEOUT                (1<<3)
#define USBC_TIMEOUT_MASK           (1<<3)
#define USBC_TIMEOUT_SHIFT          (3)
#define USBC_INTKNTXFEMP            (1<<4)
#define USBC_INTKNTXFEMP_MASK       (1<<4)
#define USBC_INTKNTXFEMP_SHIFT      (4)
#define USBC_INTKNEPMIS             (1<<5)
#define USBC_INTKNEPMIS_MASK        (1<<5)
#define USBC_INTKNEPMIS_SHIFT       (5)
#define USBC_INEPNAKEFF             (1<<6)
#define USBC_INEPNAKEFF_MASK        (1<<6)
#define USBC_INEPNAKEFF_SHIFT       (6)
#define USBC_TXFEMP                 (1<<7)
#define USBC_TXFEMP_MASK            (1<<7)
#define USBC_TXFEMP_SHIFT           (7)
#define USBC_TXFIFOUNDRN            (1<<8)
#define USBC_TXFIFOUNDRN_MASK       (1<<8)
#define USBC_TXFIFOUNDRN_SHIFT      (8)
#define USBC_BNAINTR                (1<<9)
#define USBC_PKTDRPSTS              (1<<11)
#define USBC_BBLEERRINTRPT          (1<<12)
#define USBC_NAKINTRPT              (1<<13)
#define USBC_NYETINTRPT             (1<<14)
#define USBC_STUPPKTRCVD            (1<<15)

//DIEPTSIZ0
#define USBC_IEPXFERSIZE0(n)        (((n)&0x7F)<<0)
#define USBC_IEPXFERSIZE0_MASK      (0x7F<<0)
#define USBC_IEPXFERSIZE0_SHIFT     (0)
#define USBC_IEPPKTCNT0(n)          (((n)&3)<<19)
#define USBC_IEPPKTCNT0_MASK        (3<<19)
#define USBC_IEPPKTCNT0_SHIFT       (19)

//DIEPDMA0
#define USBC_DMAADDR(n)             (((n)&0xFFFFFFFF)<<0)
#define USBC_DMAADDR_MASK           (0xFFFFFFFF<<0)
#define USBC_DMAADDR_SHIFT          (0)

//DIEPFSTS0
#define USBC_INEPTXFSPCAVAIL(n)     (((n)&0xFFFF)<<0)
#define USBC_INEPTXFSPCAVAIL_MASK   (0xFFFF<<0)
#define USBC_INEPTXFSPCAVAIL_SHIFT  (0)

//DIEPCTL
#define USBC_EPN_MPS(n)             (((n)&0x7FF)<<0)
#define USBC_EPN_MPS_MASK           (0x7FF<<0)
#define USBC_EPN_MPS_SHIFT          (0)
//#define USBC_NEXTEP(n)            (((n)&15)<<11)
//#define USBC_NEXTEP_MASK          (15<<11)
//#define USBC_NEXTEP_SHIFT         (11)
//#define USBC_USBACTEP             (1<<15)
//#define USBC_USBACTEP_MASK        (1<<15)
//#define USBC_USBACTEP_SHIFT       (15)
#define USBC_EPDPID                 (1<<16)
#define USBC_EPDPID_MASK            (1<<16)
#define USBC_EPDPID_SHIFT           (16)
//#define USBC_NAKSTS               (1<<17)
//#define USBC_NAKSTS_MASK          (1<<17)
//#define USBC_NAKSTS_SHIFT         (17)
//#define USBC_EPTYPE(n)            (((n)&3)<<18)
//#define USBC_EPTYPE_MASK          (3<<18)
//#define USBC_EPTYPE_SHIFT         (18)
#define USBC_SNP                    (1<<20)
#define USBC_SNP_MASK               (1<<20)
#define USBC_SNP_SHIFT              (20)
//#define USBC_STALL                (1<<21)
//#define USBC_STALL_MASK           (1<<21)
//#define USBC_STALL_SHIFT          (21)
//#define USBC_EPTXFNUM(n)          (((n)&15)<<22)
//#define USBC_EPTXFNUM_MASK        (15<<22)
//#define USBC_EPTXFNUM_SHIFT       (22)
//#define USBC_CNAK                 (1<<26)
//#define USBC_CNAK_MASK            (1<<26)
//#define USBC_CNAK_SHIFT           (26)
//#define USBC_SNAK                 (1<<27)
//#define USBC_SNAK_MASK            (1<<27)
//#define USBC_SNAK_SHIFT           (27)
#define USBC_SETD0PID               (1<<28)
#define USBC_SETD0PID_MASK          (1<<28)
#define USBC_SETD0PID_SHIFT         (28)
#define USBC_SETD1PID               (1<<29)
#define USBC_SETD1PID_MASK          (1<<29)
#define USBC_SETD1PID_SHIFT         (29)
//#define USBC_EPDIS                (1<<30)
//#define USBC_EPDIS_MASK           (1<<30)
//#define USBC_EPDIS_SHIFT          (30)
//#define USBC_EPENA                (1<<31)
//#define USBC_EPENA_MASK           (1<<31)
//#define USBC_EPENA_SHIFT          (31)

//DIEPINT
//#define USBC_XFERCOMPL            (1<<0)
//#define USBC_XFERCOMPL_MASK       (1<<0)
//#define USBC_XFERCOMPL_SHIFT      (0)
//#define USBC_EPDISBLD             (1<<1)
//#define USBC_EPDISBLD_MASK        (1<<1)
//#define USBC_EPDISBLD_SHIFT       (1)
//#define USBC_AHBERR               (1<<2)
//#define USBC_AHBERR_MASK          (1<<2)
//#define USBC_AHBERR_SHIFT         (2)
//#define USBC_TIMEOUT              (1<<3)
//#define USBC_TIMEOUT_MASK         (1<<3)
//#define USBC_TIMEOUT_SHIFT        (3)
//#define USBC_INTKNTXFEMP          (1<<4)
//#define USBC_INTKNTXFEMP_MASK     (1<<4)
//#define USBC_INTKNTXFEMP_SHIFT    (4)
//#define USBC_INTKNEPMIS           (1<<5)
//#define USBC_INTKNEPMIS_MASK      (1<<5)
//#define USBC_INTKNEPMIS_SHIFT     (5)
//#define USBC_INEPNAKEFF           (1<<6)
//#define USBC_INEPNAKEFF_MASK      (1<<6)
//#define USBC_INEPNAKEFF_SHIFT     (6)
//#define USBC_TXFEMP               (1<<7)
//#define USBC_TXFEMP_MASK          (1<<7)
//#define USBC_TXFEMP_SHIFT         (7)
//#define USBC_TXFIFOUNDRN          (1<<8)
//#define USBC_TXFIFOUNDRN_MASK     (1<<8)
//#define USBC_TXFIFOUNDRN_SHIFT    (8)

//DIEPTSIZ
#define USBC_IEPXFERSIZE(n)         (((n)&0x7FFFF)<<0)
#define USBC_IEPXFERSIZE_MASK       (0x7FFFF<<0)
#define USBC_IEPXFERSIZE_SHIFT      (0)
#define USBC_IEPPKTCNT(n)           (((n)&0x3FF)<<19)
#define USBC_IEPPKTCNT_MASK         (0x3FF<<19)
#define USBC_IEPPKTCNT_SHIFT        (19)
#define USBC_MC(n)                  (((n)&3)<<29)
#define USBC_MC_MASK                (3<<29)
#define USBC_MC_SHIFT               (29)

//DIEPDMA
//#define USBC_DMAADDR(n)           (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK         (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT        (0)

//DIEPFSTS
//#define USBC_INEPTXFSPCAVAIL(n)   (((n)&0xFFFF)<<0)
//#define USBC_INEPTXFSPCAVAIL_MASK (0xFFFF<<0)
//#define USBC_INEPTXFSPCAVAIL_SHIFT (0)

//DOEPCTL0
//#define USBC_EP0_MPS(n)           (((n)&3)<<0)
//#define USBC_EP0_MPS_MASK         (3<<0)
//#define USBC_EP0_MPS_SHIFT        (0)
//#define USBC_USBACTEP             (1<<15)
//#define USBC_USBACTEP_MASK        (1<<15)
//#define USBC_USBACTEP_SHIFT       (15)
//#define USBC_NAKSTS               (1<<17)
//#define USBC_NAKSTS_MASK          (1<<17)
//#define USBC_NAKSTS_SHIFT         (17)
//#define USBC_EPTYPE(n)            (((n)&3)<<18)
//#define USBC_EPTYPE_MASK          (3<<18)
//#define USBC_EPTYPE_SHIFT         (18)
//#define USBC_SNP                  (1<<20)
//#define USBC_SNP_MASK             (1<<20)
//#define USBC_SNP_SHIFT            (20)
//#define USBC_STALL                (1<<21)
//#define USBC_STALL_MASK           (1<<21)
//#define USBC_STALL_SHIFT          (21)
//#define USBC_CNAK                 (1<<26)
//#define USBC_CNAK_MASK            (1<<26)
//#define USBC_CNAK_SHIFT           (26)
//#define USBC_SNAK                 (1<<27)
//#define USBC_SNAK_MASK            (1<<27)
//#define USBC_SNAK_SHIFT           (27)
//#define USBC_EPDIS                (1<<30)
//#define USBC_EPDIS_MASK           (1<<30)
//#define USBC_EPDIS_SHIFT          (30)
//#define USBC_EPENA                (1<<31)
//#define USBC_EPENA_MASK           (1<<31)
//#define USBC_EPENA_SHIFT          (31)

//DOEPINT0
//#define USBC_XFERCOMPL            (1<<0)
//#define USBC_XFERCOMPL_MASK       (1<<0)
//#define USBC_XFERCOMPL_SHIFT      (0)
//#define USBC_EPDISBLD             (1<<1)
//#define USBC_EPDISBLD_MASK        (1<<1)
//#define USBC_EPDISBLD_SHIFT       (1)
//#define USBC_AHBERR               (1<<2)
//#define USBC_AHBERR_MASK          (1<<2)
//#define USBC_AHBERR_SHIFT         (2)
#define USBC_SETUP                  (1<<3)
#define USBC_SETUP_MASK             (1<<3)
#define USBC_SETUP_SHIFT            (3)
#define USBC_OUTTKNEPDIS            (1<<4)
#define USBC_OUTTKNEPDIS_MASK       (1<<4)
#define USBC_OUTTKNTPDIS_SHIFT      (4)
#define USBC_STSPHSERCVD            (1<<5)
#define USBC_STSPHSERCVD_MASK       (1<<5)
#define USBC_STSPHSERCVD_SHIFT      (5)
#define USBC_BACK2BACKSETUP         (1<<6)
#define USBC_BACK2BACKSETUP_MASK    (1<<6)
#define USBC_BACK2BACKSETUP_SHIFT   (6)
//#define USBC_TXFEMP               (1<<7)
//#define USBC_TXFEMP_MASK          (1<<7)
//#define USBC_TXFEMP_SHIFT         (7)
#define USBC_OUTPKTERR              (1<<8)
#define USBC_OUTPKTERR_MASK         (1<<8)
#define USBC_OUTPKTERR_SHIFT        (8)
//#define USBC_BNAINTR              (1<<9)
//#define USBC_PKTDRPSTS            (1<<11)
//#define USBC_BBLEERRINTRPT        (1<<12)
//#define USBC_NAKINTRPT            (1<<13)
//#define USBC_NYETINTRPT           (1<<14)
//#define USBC_STUPPKTRCVD          (1<<15)

//DOEPTSIZ0
#define USBC_OEPXFERSIZE0(n)        (((n)&0x7F)<<0)
#define USBC_OEPXFERSIZE0_MASK      (0x7F<<0)
#define USBC_OEPXFERSIZE0_SHIFT     (0)
#define USBC_OEPPKTCNT0             (1<<19)
#define USBC_OEPPKTCNT0_MASK        (1<<19)
#define USBC_OEPPKTCNT0_SHIFT       (19)
#define USBC_SUPCNT(n)              (((n)&3)<<29)
#define USBC_SUPCNT_MASK            (3<<29)
#define USBC_SUPCNT_SHIFT           (29)

//DOEPDMA0
//#define USBC_DMAADDR(n)           (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK         (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT        (0)

//DOEPCTL
//#define USBC_EPN_MPS(n)           (((n)&0x7FF)<<0)
//#define USBC_EPN_MPS_MASK         (0x7FF<<0)
//#define USBC_EPN_MPS_SHIFT        (0)
//#define USBC_NEXTEP(n)            (((n)&15)<<11)
//#define USBC_NEXTEP_MASK          (15<<11)
//#define USBC_NEXTEP_SHIFT         (11)
//#define USBC_USBACTEP             (1<<15)
//#define USBC_USBACTEP_MASK        (1<<15)
//#define USBC_USBACTEP_SHIFT       (15)
//#define USBC_EPDPID               (1<<16)
//#define USBC_EPDPID_MASK          (1<<16)
//#define USBC_EPDPID_SHIFT         (16)
//#define USBC_NAKSTS               (1<<17)
//#define USBC_NAKSTS_MASK          (1<<17)
//#define USBC_NAKSTS_SHIFT         (17)
//#define USBC_EPTYPE(n)            (((n)&3)<<18)
//#define USBC_EPTYPE_MASK          (3<<18)
//#define USBC_EPTYPE_SHIFT         (18)
//#define USBC_SNP                  (1<<20)
//#define USBC_SNP_MASK             (1<<20)
//#define USBC_SNP_SHIFT            (20)
//#define USBC_STALL                (1<<21)
//#define USBC_STALL_MASK           (1<<21)
//#define USBC_STALL_SHIFT          (21)
//#define USBC_EPTXFNUM(n)          (((n)&15)<<22)
//#define USBC_EPTXFNUM_MASK        (15<<22)
//#define USBC_EPTXFNUM_SHIFT       (22)
//#define USBC_CNAK                 (1<<26)
//#define USBC_CNAK_MASK            (1<<26)
//#define USBC_CNAK_SHIFT           (26)
//#define USBC_SNAK                 (1<<27)
//#define USBC_SNAK_MASK            (1<<27)
//#define USBC_SNAK_SHIFT           (27)
//#define USBC_SETD0PID             (1<<28)
//#define USBC_SETD0PID_MASK        (1<<28)
//#define USBC_SETD0PID_SHIFT       (28)
//#define USBC_SETD1PID             (1<<29)
//#define USBC_SETD1PID_MASK        (1<<29)
//#define USBC_SETD1PID_SHIFT       (29)
//#define USBC_EPDIS                (1<<30)
//#define USBC_EPDIS_MASK           (1<<30)
//#define USBC_EPDIS_SHIFT          (30)
//#define USBC_EPENA                (1<<31)
//#define USBC_EPENA_MASK           (1<<31)
//#define USBC_EPENA_SHIFT          (31)

//DOEPINT
//#define USBC_XFERCOMPL            (1<<0)
//#define USBC_XFERCOMPL_MASK       (1<<0)
//#define USBC_XFERCOMPL_SHIFT      (0)
//#define USBC_EPDISBLD             (1<<1)
//#define USBC_EPDISBLD_MASK        (1<<1)
//#define USBC_EPDISBLD_SHIFT       (1)
//#define USBC_AHBERR               (1<<2)
//#define USBC_AHBERR_MASK          (1<<2)
//#define USBC_AHBERR_SHIFT         (2)
//#define USBC_SETUP                (1<<3)
//#define USBC_SETUP_MASK           (1<<3)
//#define USBC_SETUP_SHIFT          (3)
//#define USBC_OUTTKNEPDIS          (1<<4)
//#define USBC_OUTTKNEPDIS_MASK     (1<<4)
//#define USBC_OUTTKNTPDIS_SHIFT    (4)
//#define USBC_STSPHSERCVD          (1<<5)
//#define USBC_STSPHSERCVD_MASK     (1<<5)
//#define USBC_STSPHSERCVD_SHIFT    (5)
//#define USBC_BACK2BACKSETUP       (1<<6)
//#define USBC_BACK2BACKSETUP_MASK  (1<<6)
//#define USBC_BACK2BACKSETUP_SHIFT (6)
//#define USBC_TXFEMP               (1<<7)
//#define USBC_TXFEMP_MASK          (1<<7)
//#define USBC_TXFEMP_SHIFT         (7)
//#define USBC_OUTPKTERR            (1<<8)
//#define USBC_OUTPKTERR_MASK       (1<<8)
//#define USBC_OUTPKTERR_SHIFT      (8)
//#define USBC_BNAINTR              (1<<9)
//#define USBC_PKTDRPSTS            (1<<11)
//#define USBC_BBLEERRINTRPT        (1<<12)
//#define USBC_NAKINTRPT            (1<<13)
//#define USBC_NYETINTRPT           (1<<14)
//#define USBC_STUPPKTRCVD          (1<<15)

//DOEPTSIZ
#define USBC_OEPXFERSIZE(n)         (((n)&0x7FFFF)<<0)
#define USBC_OEPXFERSIZE_MASK       (0x7FFFF<<0)
#define USBC_OEPXFERSIZE_SHIFT      (0)
#define USBC_OEPPKTCNT(n)           (((n)&0x3FF)<<19)
#define USBC_OEPPKTCNT_MASK         (0x3FF<<19)
#define USBC_OEPPKTCNT_SHIFT        (19)
//#define USBC_MC(n)                (((n)&3)<<29)
//#define USBC_MC_MASK              (3<<29)
//#define USBC_MC_SHIFT             (29)
#define USBC_RXDPID(n)              (((n)&3)<<29)
#define USBC_RXDPID_MASK            (3<<29)
#define USBC_RXDPID_SHIFT           (29)

//DOEPDMA
//#define USBC_DMAADDR(n)           (((n)&0xFFFFFFFF)<<0)
//#define USBC_DMAADDR_MASK         (0xFFFFFFFF<<0)
//#define USBC_DMAADDR_SHIFT        (0)

//PCGCCTL
#define USBC_STOPPCLK               (1<<0)
#define USBC_STOPPCLK_MASK          (1<<0)
#define USBC_STOPPCLK_SHIFT         (0)
#define USBC_GATEHCLK               (1<<1)
#define USBC_GATEHCLK_MASK          (1<<1)
#define USBC_GATEHCLK_SHIFT         (1)
#define USBC_PWRCLMP                (1<<2)
#define USBC_PWRCLMP_MASK           (1<<2)
#define USBC_PWRCLMP_SHIFT          (2)
#define USBC_RSTPDWNMODULE          (1<<3)
#define USBC_RSTPDWNMODULE_MASK     (1<<3)
#define USBC_RSTPDWNMODULE_SHIFT    (3)
#define USBC_ENBL_L1GATING          (1<<5)
#define USBC_ENBL_L1GATING_MASK     (1<<5)
#define USBC_ENBL_L1GATING_SHIFT    (5)
#define USBC_PHYSLEEP               (1<<6)
#define USBC_PHYSLEEP_MASK          (1<<6)
#define USBC_PHYSLEEP_SHIFT         (6)
#define USBC_SUSPENDED              (1<<7)
#define USBC_SUSPENDED_MASK         (1<<7)
#define USBC_SUSPENDED_SHIFT        (7)

//TxRxData
#define USBC_TXDATA(n)              (((n)&0xFFFFFFFF)<<0)
#define USBC_TXDATA_MASK            (0xFFFFFFFF<<0)
#define USBC_TXDATA_SHIFT           (0)
#define USBC_RXDATA(n)              (((n)&0xFFFFFFFF)<<0)
#define USBC_RXDATA_MASK            (0xFFFFFFFF<<0)
#define USBC_RXDATA_SHIFT           (0)

//RAMData
#define USBC_DATA(n)                (((n)&0xFFFFFFFF)<<0)
#define USBC_DATA_MASK              (0xFFFFFFFF<<0)
#define USBC_DATA_SHIFT             (0)


// ============================================================================
// USBIF_T
// -----------------------------------------------------------------------------
///
// =============================================================================
struct USBIF_T {
    __IO uint32_t USBIF_00;
    __IO uint32_t USBIF_04;
    __IO uint32_t USBIF_08;
    __IO uint32_t USBIF_0C;
    __IO uint32_t USBIF_10;
    __IO uint32_t USBIF_14;
    __IO uint32_t USBIF_18;
    __IO uint32_t USBIF_1C;
    __IO uint32_t USBIF_20;
};

// USBIF_00
#define USBIF_00_CFG_TP_SEL_SHIFT           0
#define USBIF_00_CFG_TP_SEL_MASK            (0xFF << USBIF_00_CFG_TP_SEL_SHIFT)
#define USBIF_00_CFG_TP_SEL(n)              BITFIELD_VAL(USBIF_00_CFG_TP_SEL, n)
#define USBIF_00_CFG_EXT_PHY                (1 << 8)
#define USBIF_00_CFG_EXT_ADP                (1 << 9)
#define USBIF_00_CFG_EXT_OTG                (1 << 10)
#define USBIF_00_CFG_EXT_RID                (1 << 11)
#define USBIF_00_CFG_DR_PLL                 (1 << 16)
#define USBIF_00_CFG_DR_SUSPEND             (1 << 17)
#define USBIF_00_CFG_REG_PLL                (1 << 24)
#define USBIF_00_CFG_REG_SUSPEND            (1 << 25)

// USBIF_04
#define USBIF_04_CFG_MODE_PLLEN_SHIFT       4
#define USBIF_04_CFG_MODE_PLLEN_MASK        (0x3 << USBIF_04_CFG_MODE_PLLEN_SHIFT)
#define USBIF_04_CFG_MODE_PLLEN(n)          BITFIELD_VAL(USBIF_04_CFG_MODE_PLLEN, n)
#define USBIF_04_CFG_SUSPEND_MODE_SHIFT     6
#define USBIF_04_CFG_SUSPEND_MODE_MASK      (0x3 << USBIF_04_CFG_SUSPEND_MODE_SHIFT)
#define USBIF_04_CFG_SUSPEND_MODE(n)        BITFIELD_VAL(USBIF_04_CFG_SUSPEND_MODE, n)
#define USBIF_04_CFG_SLEEP_THSD_SHIFT       8
#define USBIF_04_CFG_SLEEP_THSD_MASK        (0xF << USBIF_04_CFG_SLEEP_THSD_SHIFT)
#define USBIF_04_CFG_SLEEP_THSD(n)          BITFIELD_VAL(USBIF_04_CFG_SLEEP_THSD, n)

// USBIF_08
#define USBIF_08_CFG_EXTPHY_RXTIMING        (1 << 0)
#define USBIF_08_CFG_EXTPHY_TXTIMING        (1 << 1)
#define USBIF_08_CFG_PHY_TXTIMING           (1 << 2)
#define USBIF_08_CFG_PHY_RXTIMING           (1 << 3)
#define USBIF_08_CFG_PHY_RXLSTIMING         (1 << 4)
#define USBIF_08_CFG_EN_HOSTDISC            (1 << 5)
#define USBIF_08_CFG_MODE_FSRCV             (1 << 6)
#define USBIF_08_CFG_PHY_TXOPMODE           (1 << 7)
#define USBIF_08_CFG_PHY_TXXCVSEL           (1 << 8)
#define USBIF_08_CFG_PHY_TXTERMSEL          (1 << 9)
#define USBIF_08_CFG_SEL_HSIC               (1 << 10)
#define USBIF_08_CFG_SEL48M                 (1 << 11)
#define USBIF_08_CFG_POL_SPI_CLK            (1 << 12)
#define USBIF_08_CFG_MACPLL_RDY             (1 << 13)
#define USBIF_08_CFG_PHY_HOST_MODE          (1 << 14)
#define USBIF_08_CFG_SOFT_RESETN_PHY        (1 << 15)

// USBIF_0C
#define USBIF_0C_PLL_EN_MSK                 (1 << 0)
#define USBIF_0C_D1_DEV_HIRD_VALID_MSK      (1 << 1)
#define USBIF_0C_SOF_TOGGLE_OUT_MSK         (1 << 2)
#define USBIF_0C_SOF_SEND_MSK               (1 << 3)
#define USBIF_0C_CHEP_LAST_TRANS_MSK        (1 << 4)
#define USBIF_0C_ID_DIG_MSK                 (1 << 5)
#define USBIF_0C_ENABLE_ALL_MSK             (1 << 6)

// USBIF_10
#define USBIF_10_PLL_EN_RIS                 (1 << 0)
#define USBIF_10_D1_DEV_HIRD_VALID_RIS      (1 << 1)
#define USBIF_10_SOF_TOGGLE_OUT_RIS         (1 << 2)
#define USBIF_10_SOF_SEND_RIS               (1 << 3)
#define USBIF_10_CHEP_LAST_TRANS_RIS        (1 << 4)
#define USBIF_10_ID_DIG_RIS                 (1 << 5)

// USBIF_14
#define USBIF_14_PLL_EN_MIS                 (1 << 0)
#define USBIF_14_D1_DEV_HIRD_VALID_MIS      (1 << 1)
#define USBIF_14_SOF_TOGGLE_OUT_MIS         (1 << 2)
#define USBIF_14_SOF_SEND_MIS               (1 << 3)
#define USBIF_14_CHEP_LAST_TRANS_MIS        (1 << 4)
#define USBIF_14_ID_DIG_MIS                 (1 << 5)

// USBIF_18
#define USBIF_18_LAST_DEV_HIRD_RCVD_SHIFT   0
#define USBIF_18_LAST_DEV_HIRD_RCVD_MASK    (0xF << USBIF_18_LAST_DEV_HIRD_RCVD_SHIFT)
#define USBIF_18_LAST_DEV_HIRD_RCVD(n)      BITFIELD_VAL(USBIF_18_LAST_DEV_HIRD_RCVD, n)
#define USBIF_18_LAST_CHEP_NUMBER_SHIFT     4
#define USBIF_18_LAST_CHEP_NUMBER_MASK      (0xF << USBIF_18_LAST_CHEP_NUMBER_SHIFT)
#define USBIF_18_LAST_CHEP_NUMBER(n)        BITFIELD_VAL(USBIF_18_LAST_CHEP_NUMBER, n)
#define USBIF_18_DEV_HIRD_RCVD_SHIFT        8
#define USBIF_18_DEV_HIRD_RCVD_MASK         (0xF << USBIF_18_DEV_HIRD_RCVD_SHIFT)
#define USBIF_18_DEV_HIRD_RCVD(n)           BITFIELD_VAL(USBIF_18_DEV_HIRD_RCVD, n)
#define USBIF_18_CHEP_NUMBER_SHIFT          12
#define USBIF_18_CHEP_NUMBER_MASK           (0xF << USBIF_18_CHEP_NUMBER_SHIFT)
#define USBIF_18_CHEP_NUMBER(n)             BITFIELD_VAL(USBIF_18_CHEP_NUMBER, n)
#define USBIF_18_CHEP_LAST_TRANS            (1 << 16)
#define USBIF_18_SOF_SENT_RCVD_TGL          (1 << 17)
#define USBIF_18_SOF_TOGGLE_OUT             (1 << 18)
#define USBIF_18_DEV_HIRD_VLD_TGL           (1 << 19)
#define USBIF_18_ID_DIG                     (1 << 20)

#endif

