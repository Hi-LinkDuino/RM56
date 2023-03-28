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
#ifndef EXP_FUNC_H
#define EXP_FUNC_H

#include "exp_func_tables.h"
#include "math_approx.h"

//log10_20fix(in * 1024) = 10 * log10(in) * 32
static inline int log10_20fix(int in)
{
	int out;
	short addr1, addr2;
	addr1 = (short)((in - 1) >> 8);
        if(in<=1)
          in = 1;
	if (in <= 128)
		out = Tbl2[in - 1];
	else
		if (addr1 <= 63)
		{
			addr2 = (short)((in - 1) & 0xff);
			out = (int)Tbl[addr1] + (((int)Tblk[addr1] * addr2) >> 8);
		}
		else
			out = Tbl[addr1];
	return (out);
}

//(0:5]*256 ExpIntfix(x*256) = expint(x)*1024
static inline int ExpIntfix(short in)
{
	int out;
	short addr1, addr2;
	addr1 = (short)((in - 1) >> 4);
	if (in == 0)
		in = 0;
	else if (addr1 == 80)
		addr1 = 80;
	if (in <= 128)
		out = ExpTbl2[in - 1];
	else
		if (addr1 <= 61)
		{
			addr2 = (short)((in - 1) & 0xf);
			out = (int)ExpTbl[addr1 - 8] + (((int)ExpTblk[addr1 - 8] * addr2) >> 8);
		}
		else
			out = ExpTbl[addr1 - 8];
	return out;
}

static inline int exp_fix(short x)
{
	return spx_exp(x);
}

//log10dotfun(in*1024) = 10*log10(in)*16
static inline int log10dotfun(short in)
{
	int out;
	short addr1, addr2, tmp;
	tmp = in - 1;
	if (tmp<0)
		tmp = 0;
	addr1 = (short)(tmp >> 5);
	if (in <= 64)
		out = log10Tbl2[tmp];
	else
	{
		addr2 = (short)(tmp & 0x1f);
		out = (int)log10Tbl1[addr1] + (((int)log10Tblk2[addr1] * addr2) >> 8);
	}
	return out;
}

static inline short sqrt_fix(int x)
{
	return spx_sqrt(x);
}

#endif
