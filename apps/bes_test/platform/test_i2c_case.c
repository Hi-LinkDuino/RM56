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
#include "console.h"
#include "hal_trace.h"
#include "hal_i2c.h"
#include "ctype.h"
#include "string.h"
#include <stddef.h>
#include <stdio.h>
#include "test_i2c_case.h"
#if 1
static int atoi(const char *s)
{
    int i = 0;

    while (isdigit(*s))
        i = i*10 + (*s++) - '0';

    return i;
}

#endif
static void test_i2c_init(int id)
{
    static int init_i2c0_flag = 0;
    static int init_i2c1_flag = 0;
	uint32_t ret = 0;
	struct HAL_I2C_CONFIG_T i2c_cfg;


    switch(id)
    {
        case 0:
            if(init_i2c0_flag){
                TRACE(0, "i2c0 had init before!!\n");
                return;
            }
            hal_iomux_set_i2c0();
            init_i2c0_flag = 1;
            break;
        case 1:
            if(init_i2c1_flag){
                TRACE(0, "i2c1 had init before!!\n");
                return;
            }
            hal_iomux_set_i2c1();
            init_i2c1_flag = 1;
            break;
         default:
            TRACE(0, "i2c id(%d) is wrong!!must be 0/1\n",id);
            return;
    }
    
	i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
	i2c_cfg.use_dma = 0;
	i2c_cfg.use_sync = 1;
	i2c_cfg.speed = 100 * 1000;
	i2c_cfg.as_master = 1;
	
	ret = hal_i2c_open(id, &i2c_cfg);
	if (ret)
	{
		TRACE(0, "open i2c%d fail\n",id);
		return;
	}

	TRACE(0, "open i2c%d succ.\n",id);
}


int do_I2C_cmd( cmd_tbl_t *cmd, int argc, char *argv[])
{
    int i2c_id = 0;
    int i2c_rw = -1;
    uint8_t i2c_addr = 0x00;
    uint8_t data[8]={0};
    int ret =0;

    
    TRACE(0, "%s maxargs:%d cmd:%p usage:%s,argc=%d",cmd->name,cmd->maxargs,cmd->cmd,cmd->usage,argc);
    if (argc < 4)
    {
        printf("ERROR,AT+I2CTEST=<i2c_id>,<r/w>,<address>,<data[0]>,...,<data[n]>\n");
        printf("<i2c_id>=0/1,<r/w>=1/0\n");
        return 0;
    }
    
    printf("Welcome to ");
    for (size_t i = 1; i < argc; i++)
    {
        printf(",%s",argv[i]);
    }
    printf("\n");
    
    i2c_id = atoi(argv[1]);
    printf("i2c_id=%d\n",i2c_id);
    if(i2c_id > HAL_I2C_ID_1){
        printf("i2c_id=%d is wrong!\n",i2c_id);
        return -1;
    }
    test_i2c_init(i2c_id);
    
    i2c_rw = atoi(argv[2]);
    i2c_addr = simple_strtoul(argv[3], NULL, 16);
    printf("I2C R/W[%d],addr[0x%02x]\n",i2c_rw,i2c_addr);

    if(i2c_rw == I2C_WRITE_CMD){   //write
        if(argc>4)
        data[0]= simple_strtoul(argv[4], NULL, 16);
        if(argc>5)
        data[1]= simple_strtoul(argv[5], NULL, 16);
        if(argc>6)
        data[3]= simple_strtoul(argv[6], NULL, 16);
        hal_i2c_task_send(i2c_id,i2c_addr,&data[0],argc-5,0,NULL);
    }
    else if(i2c_rw == I2C_READ_CMD){     //read
        ret = hal_i2c_task_recv(i2c_id,i2c_addr,&data[0],0,&data[0],8, 0, NULL);
        if(ret){
            printf("%s:%d,i2c recev fail, dev_addr = 0x%x, ret = %d\n", __func__,__LINE__, i2c_addr,ret);
            return -1;
        }
        printf("%s:%d,i2c recev dev_addr = 0x%x, ret = %d\ndata", __func__,__LINE__, i2c_addr,ret);
        for(int i=0;i<8;i++){
            printf("[0x%02x]",data[i]);
        }
        printf("\n");
    }
    printf("RESP OK\n");
    return 0;

}
