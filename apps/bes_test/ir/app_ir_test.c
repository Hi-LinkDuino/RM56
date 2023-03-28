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
// #ifdef IR_FUNCTION_TEST
#ifdef SUPPORT_IR
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "list.h"
#include "string.h"
#include "plat_types.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "app_key.h"
#include "hal_key.h"
#include "app_ir.h"
#include "hal_ir.h"
#include "pmu.h"
#include "app_ir_test.h"


#define IR_RESP(fmt, ...)          printf(fmt, ##__VA_ARGS__)
#define IR_RESP_OK()               printf("+ok\r\n")
#define IR_RESP_OK_EQU(fmt, ...)   printf("+ok=" fmt, ##__VA_ARGS__)
#define IR_RESP_ERROR(err_type)    printf("+error=%d\r\n", err_type)

//extern uint32_t rf_simple_strtoul(const char *cp,char **endp,unsigned int base);
static unsigned int ir_cmd_cntr = 0;
static ir_tbl_t ir_cmd_savelist[IR_CMD_MAXARGS] = {0};


enum ir_key_code {
    KEYBOARD_POWER_ONOFF_SHORT = 0,
    KEYBOARD_POWERKEY_OFF_LONG,
    KEYBOARD_MODE_SHORT,
    KEYBOARD_SPEED_SHORT,
    KEYBOARD_MAX
};


/***************************************************************
 * Function     : app_ir_led
 * Description  : 仅仅用于测试
****************************************************************/
static unsigned int on_time_last = -1;
static void app_ir_led(unsigned int brightness)
{
    ir_printf("%s,%d  brightness=%d \n",__func__,__LINE__,brightness);
#if 0
    if(brightness){
        pwm_cfg.ratio = brightness;
    }else{
        pwm_cfg.ratio = 0;
    }
    hal_pwm_enable(HAL_PWM_ID_3,&pwm_cfg);
#else
    unsigned int on_time = brightness/5;
    if(on_time_last == on_time)
    {
        return;
    }
    else
    {
        on_time_last = on_time;
    }
    if (on_time == 0)
    {
        pmu_write(0x30, 0x5400);
        pmu_write(0x2b, 0xee89);
        return;
    }
    unsigned int off_time = 51 - on_time;
    pmu_write(0x30, 0x5600); //close pwm en & breath keep pwm clock
    pmu_write(0x2e, on_time);
    pmu_write(0x2d, off_time);
    osDelay(0);
    //krhino_task_sleep(1);
    pmu_write(0x30, 0x560c);
    pmu_write(0x2b, 0xce89);
#endif
}

/***************************************************************
 * protocol command definded
****************************************************************/
typedef enum
{
	NEC_MODE_NORMAL		    = 0x00,     /* IR NEC CMD*/
    NEC_MODE_FRAMETIME,                 /* IR NEC CMD TIME DATA输入参数是时间参数，直接发送不需要处理*/
} IR_NEC_MODE_T;

typedef enum
{
	R05D_MODE_NORMAL		= 0x00,     /* IR Media R05D CMD*/
    R05D_MODE_FRAMETIME,                /* IR Media R05D CMD TIME DATA输入参数是时间参数，直接发送不需要处理*/
} IR_R05D_MODE_T;

typedef enum
{
	APP_R05D_CMD_POWEROFF   = 0x00,     /* CMD 0: POWEROFF */
    APP_R05D_CMD_POWERON,               /* CMD 1: POWERON */
} IR_R05D_CMD_T;


static uint8_t r05d_cmd_current[5]={0xB2,0X1F,0XD8,0X00,0X00};   //用于保存当前命令

static const uint8_t r05d_cmd[IR_CMD_MAXARGS][5] = {
{0xB2,0X7B,0XE0,0X00,0x00},             /* 0: POWEROFF: L+A'A+B'B+C`C +S+L+ A'A+B'B+C`C +S+L+ A'A+Q'Q+Y`Y*/
{0xB2,0X1F,0XD8,0X00,0X00},             /* 1: POWERON : L+A'A+B'B+C`C +S+L+ A'A+B'B+C`C* 固定风,26°自动模式 */
};


extern uint32_t simple_strtoul(const char *cp,char **endp,unsigned int base);

/***************************************************************
 * Function     : app_ir_study_start
 * Description  : 红外接收状态初始化,并开始接收
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_study_start(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    app_ir_rx_init();
    
    return 0;
}

/***************************************************************
 * Function     : app_ir_study_stop
 * Description  : 红外接收停止
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_study_stop(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    app_ir_rx_deinit();
    
    return 0;
}

/***************************************************************
 * Function     : app_ir_send_init
 * Description  : 红外发送状态初始化
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_init(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    app_ir_tx_init();
    
    return 0;
}

/***************************************************************
 * Function     : app_ir_send_stop
 * Description  : 红外停止发送
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_stop(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    app_ir_tx_deinit();
    
    return 0;
}

/***************************************************************
 * Function     : app_ir_exit
 * Description  : 退出红外功能
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_exit(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    app_ir_deinit();
    
    return 0;
}


/***************************************************************
 * Function     : app_ir_send_nec
 * Description  : 发送NEC协议数据
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_nec(ir_tbl_t *cmd,int argc,char *argv[])
{
    uint8_t repeat = 0;
    uint8_t data[2]= {0}; 
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    
    data[0] = simple_strtoul(argv[1], NULL, 16);
    data[1] = simple_strtoul(argv[2], NULL, 16);
    repeat = simple_strtoul(argv[3], NULL, 16);
    
    //send
    ir_printf_debug("%s,%d,address[0x%02x] command[0x%02x] repeat=%d \n",__func__,__LINE__,data[0],data[1],repeat);
    app_ir_tx(IR_NEC_PROTO,&data[0],2,repeat);
    return 0;
}
/***************************************************************
 * Function     : app_ir_send_nec
 * Description  : 发送Media r05d协议数据
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_r05d(ir_tbl_t *cmd,int argc,char *argv[])
{
    uint32_t usercmd;
    uint8_t *pdata = NULL;
    uint16_t size = 0;
    uint8_t repeat = 0;
    

    if(cmd == NULL){
        ir_printf_warn("%s,%d cmd is NULL \n",__func__,__LINE__);
        return -1;
    }
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    usercmd = atoi(argv[1]);
    switch(cmd->mode)
    {
        case R05D_MODE_NORMAL:
            ir_printf_debug("%s,%d normal send,usercmd=%d \n",__func__,__LINE__,usercmd);
            pdata = &r05d_cmd[usercmd][0];

            size = 3;
            repeat = DRV_CMD_R05D_NORMAL;
            if(usercmd == APP_R05D_CMD_POWEROFF){
                size = 5;
                repeat = DRV_CMD_R05D_POWEROFF;
                app_ir_led(0);
            }
            else if(usercmd == APP_R05D_CMD_POWERON){
                app_ir_led(100);
                memcpy(&r05d_cmd_current[0],&r05d_cmd[APP_R05D_CMD_POWERON][0],5);
            }
            //send
            app_ir_tx(IR_MEDIA_R05D_PROTO,pdata,size,repeat);
            break;
        case R05D_MODE_FRAMETIME:
            ir_printf_debug("%s,%d frametime send,usercmd=%d \n",__func__,__LINE__,usercmd);
            break;
        default:
            break;
    }
    
    return 0;
}

static uint8_t irdata35[5]={0x90,0xD0,0x04,0x0E,0X40};
static uint8_t irdata32[4]={0x00,0x00,0x00,0x07};

static uint16_t framedata[512]={0};
static uint16_t ir_nums = 0;

//32bit--> bit0     上下扫风
void app_ir_gree_set_updownwind(bool udwind)
{
    if(udwind)
        irdata32[0] |= 0x80;
    else
        irdata32[0] &= 0x7F;
}

//32bit--> bit4     左右扫风
void app_ir_gree_set_leftrightwind(bool lrwind)
{
    if(lrwind)
        irdata32[0] |= 0x08;
    else
        irdata32[0] &= 0xF7;
}

//32bit--> bit26  节能开关
void app_ir_gree_set_save(bool save)
{
    if(save)
        irdata32[3] |= 0x02;
    else
        irdata32[3] &= 0xDF;
}

//35bit--> bit24  换气
void app_ir_gree_set_change_air(bool change)
{
    if(change)
        irdata35[3] |= 0x80;
    else
        irdata35[3] &= 0x7F;
}

/***************************************************************
 * Function     : app_ir_set_gree_by0f3_frame
 * Description  : 发送 GREE BY0F3协议数据,按正序来
 * Input        : 模式，开关，风速等等
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
static void app_ir_set_gree_by0f3_data1(uint8_t mode,bool onoff,uint8_t windspeed,uint8_t led,uint8_t temp)
{
    //模式：000自动，100制冷，010加湿，110送风，001加热
    //onff
    //风速：00自动，10一级，01二级，11三级
    irdata35[0] = (mode << 5) | (onoff << 4) | (windspeed << 2);
    //温度：0000~0111(bit8~11)
    irdata35[1] = ((temp - 16) << 4);
    //灯光：bit21
    irdata35[2] = led << 2;
}


static void app_ir_set_gree_data(int cmd)
{
    int i;
    //app_ir_set_gree_by0f3_data1(1,1,0,1,26);
    //app_ir_gree_set_change_air(0);
    //app_ir_gree_set_leftrightwind(0);
    //app_ir_gree_set_updownwind(0);
    //app_ir_gree_set_save(0);
    //TODO: ADD CRC CODE
    ir_printf_debug("%s irdata: \n",__func__);
    for(i=0;i<5;i++){
        ir_printf_debug("%02x",irdata35[i]);
    }
    ir_printf_debug("\n");
    for(i=0;i<4;i++){
        ir_printf_debug("%02x",irdata32[i]);
    }
    ir_printf_debug("\n");
}

static void app_ir_fill_gree_data2frame(void)
{
    int i,j;
    int index = 0;
    ir_printf_debug("%s enter \n",__func__);
    //L:9MS+4.5MS
    framedata[index++]=9000;
    framedata[index++]=4500;
    //35bits data
    for(i=0;i<5;i++){
        for(j=7;j>-1;j--){
            if((i==5)&&(j==4))  //exit
                break;
            if(irdata35[i]&(0x1 << j)){
                framedata[index++] = 640;
                framedata[index++] = 1680;
            }
            else{
                framedata[index++] = 640;
                framedata[index++] = 560;
            }
        }
    }
    //C: 640US+20MS
    framedata[index++]=640;
    framedata[index++]=20000;
    ir_printf_debug("C[%d]: %d \n",index-1,framedata[index-1]);
    //32bits data
    for(i=0;i<4;i++){
        for(j=7;j>-1;j--){
            if(irdata32[i]&(0x1 << j)){
                framedata[index++] = 640;
                framedata[index++] = 1680;
            }else{
                framedata[index++] = 640;
                framedata[index++] = 560;
            }
        }
    }
    //add stop bit
    framedata[index++] = 640;
    ir_nums = index;

#if 1
    ir_printf_debug(" %s :%d ir_nums =%d,time: \n", __func__, __LINE__,ir_nums);
    for(i=0;i<ir_nums;i++){
        ir_printf_debug("[%d]:%d",i,framedata[i]);
        if(i%2 == 1)
            ir_printf_debug("\n");
    }
    ir_printf_debug("\n");
#endif
    return;
}



/***************************************************************
 * Function     : app_ir_send_gree_by0f3
 * Description  : 发送 GREE BY0F3协议数据
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_gree_by0f3(ir_tbl_t *cmd,int argc,char *argv[])
{
    uint32_t usercmd;
    uint8_t *pdata = NULL;
    uint16_t size = 0;
    uint8_t repeat = 0;
    

    if(cmd == NULL){
        ir_printf_warn("%s,%d cmd is NULL \n",__func__,__LINE__);
        return -1;
    }
    ir_printf_debug("%s,%d enter name=%s mode=%d,info=%s \n",__func__,__LINE__,cmd->name,cmd->mode,cmd->usage);
    usercmd = atoi(argv[1]);
    switch(usercmd)
    {
        default:
            app_ir_set_gree_data(0);
            app_ir_fill_gree_data2frame();
            break;
    }
    
    app_ir_tx_raw(IR_STUDY_PROTO,&framedata[0],ir_nums,0);
    return 0;
}



/***************************************************************
 * Function     : app_ir_send_help
 * Description  : 命令help
 * Input        : arg,*arvc
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send_help(ir_tbl_t *cmd,int argc,char *argv[])
{
    ir_printf_info("%s help: \n",__func__);
    ir_printf_info("%s <protocol> <usercmd> <code/frametime....><repeat>\n",__func__);
    ir_printf_info("%s <NEC> <usercmd> <address> <command> <repeat>\n",__func__);
    ir_printf_info("%s <R05D> <usercmd>\n",__func__);
    return 0;
}

static ir_tbl_t ir_cmd_list[IR_CMD_MAXARGS] = {
{"help",        0,                      app_ir_send_help,      "tips of cmd"},
{"STUDY_START", 0,                      app_ir_study_start,    "RX initial and start rx"},
{"STUDY_STOP",  0,                      app_ir_study_stop,     "RX STOP"},
{"SEND_INIT",   0,                      app_ir_send_init,      "TX initial"},
{"SEND_STOP",   0,                      app_ir_send_stop,      "TX STOP"},
{"EXIT",        0,                      app_ir_exit,           "IR EXIT"},
{"NEC",         NEC_MODE_NORMAL,        app_ir_send_nec,       "NEC normal send"},
{"NEC_TIME",    NEC_MODE_FRAMETIME,     app_ir_send_nec,       "NEC send frame time data，no need to transform"},
{"R05D",        R05D_MODE_NORMAL,       app_ir_send_r05d,      "Media R05D send normal"},
{"R05D_TIME",   R05D_MODE_FRAMETIME,    app_ir_send_r05d,      "Media R05D send frame time data，no need to transform"},
{"GREE_BY0F3",  0,                      app_ir_send_gree_by0f3, "GREE BY0F3 send frame time dat"},

};

static int ir_parse_line (char *line, char *argv[])
{
	int nargs = 0;
    
	while (nargs < IR_CMD_MAXARGS) {
		/* skip any white space */
		while ((*line == ' ') || (*line == '\t') || (*line == ',')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = 0;
			return (nargs);
		}

		/* Argument include space should be bracketed by quotation mark */
		if(*line == '\"') {
			/* Skip quotation mark */
			line++;

			/* Begin of argument string */
			argv[nargs++] = line;

			/* Until end of argument */
			while(*line && (*line != '\"')) {
				++line;
			}
		} else {
			argv[nargs++] = line;	 /* begin of argument string	*/
			/* find end of string */
			while(*line && (*line != ' ') && (*line != '\t')) {
				++line;
			}
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = 0;
			return (nargs);
		}

		*line++ = '\0'; 		/* terminate current arg	 */
	}

	IR_RESP_ERROR(-2);

	return (nargs);
}


static ir_tbl_t *ir_find_cmd (char *cmd)
{
	ir_tbl_t *cmdtp;
	ir_tbl_t *cmdtp_temp = &ir_cmd_list[0];	 /* Init value */
	uint32_t len;
	int n_found = 0;
	int i;
    
	len = strlen(cmd);
    ir_printf_debug("%s %d enter,len=%d,cmd=[%s]\n",__func__,__LINE__,len,cmd);
	for (i = 0;i < IR_CMD_MAXARGS;i++) {
		cmdtp = &ir_cmd_list[i];
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp->name)){
                ir_printf_debug("%s %d find,cmd[%d]=[%s]\n",__func__,__LINE__,i,cmdtp->name);
				return cmdtp;	   /* full match */
			}
			cmdtp_temp = cmdtp;    /* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {  /* exactly one match */
		return cmdtp_temp;
	}

	return 0;	/* not found or ambiguous command */
}

static int ir_run_command(int argc,char *cmd[])
{
	ir_tbl_t *cmdtp;
	
    ir_printf_debug("%s %d enter,cmd[%s][%s][%s][%s][%s][%s]\n",__func__,__LINE__,cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
#if 0
	int argc;
    char *argv[IR_CMD_MAXARGS + 1];	/* NULL terminated	  */
	/* Extract arguments */
	if ((argc = ir_parse_line(cmd, argv)) == 0) {
		return -1;
	}
#endif
	/* Look up command in command table */
	if ((cmdtp = ir_find_cmd(cmd[1])) == 0) {
		IR_RESP_ERROR(-1);
		IR_RESP("try \"%s\"\n", ir_cmd_list[0].name);
		return -1;
	}

	/* OK - call function to do the command */
	if ((cmdtp->fun) (cmdtp,argc, &cmd[1]) != 0) {
		return -1;
	}

	return 0;
}



typedef void(*keycode_report_cb)(int keycode);
keycode_report_cb ir_key_cb = NULL;


void ir_test_key_cb(int keycode)
{
    ir_printf_debug("enter %s %d keycode=%d\n",__func__,__LINE__, keycode);
}


void ir_bes_key_proc(APP_KEY_STATUS *status, void *param)
{
    if (param == NULL || status == NULL)
        return;

    int keycode = *((int *)param) + KEYBOARD_POWER_ONOFF_SHORT;

    ir_printf_debug("%s code=%d, event=%d -> keycode=%d",__func__, status->code, status->event, keycode);

    switch (keycode)
    {
        case KEYBOARD_POWER_ONOFF_SHORT:   //sw1
            TRACE(0, "%s sw1\n",__func__);
            app_ir_nec_test();
            break;
        case KEYBOARD_POWERKEY_OFF_LONG:   //sw3
            TRACE(0, "%s sw3\n",__func__);
            break;
        case KEYBOARD_MODE_SHORT:   //sw2
            TRACE(0, "%s sw2\n",__func__);
            break;
        default:
            TRACE(0, "%s unkown keycode=%d!!\n",__func__,keycode);
            break;
    }
    return;

    if (ir_key_cb == NULL)
        return;

    ir_key_cb(keycode);
}


static int key_code[] =
{
   0,    //KEYBOARD_POWER_ONOFF_SHORT
   1,    //KEYBOARD_POWERKEY_OFF_LONG
   2,    //KEYBOARD_MODE_SHORT
   3,    //KEYBOARD_SPEED_SHORT
};


static const APP_KEY_HANDLE ir_key_handle_cfg[] = {
    {{HAL_KEY_CODE_FN5, APP_KEY_EVENT_CLICK}, "IR POWER_KEY_SHORT", ir_bes_key_proc, (void *)(key_code)},
    {{HAL_KEY_CODE_FN5, APP_KEY_EVENT_LONGLONGPRESS}, "IR POWER_KEY_LONG", ir_bes_key_proc, (void *)(key_code+1)},
    {{APP_KEY_CODE_FN2, APP_KEY_EVENT_CLICK}, "IR MODE_KEY_SHORT", ir_bes_key_proc, (void *)(key_code+2)},
    {{APP_KEY_CODE_FN3, APP_KEY_EVENT_CLICK}, "IR MODE_SPEED_SHORT", ir_bes_key_proc, (void *)(key_code+3)},
};

//extern int ir_register_keyboard_event_callback(keycode_report_cb);

/***************************************************************
 * Function     : app_ir_led
 * Description  : @param[in] keycode_report_cb: keyboard event callback
****************************************************************/
int ir_register_keyboard_event_callback(keycode_report_cb cb)
{
    int ret = 0;
    ir_printf_debug("%s,%d enter \n",__func__,__LINE__);
    if (ir_key_cb == NULL) {
        ir_key_cb = cb;
        app_key_handle_clear();
        for (int i=0; i<(sizeof(ir_key_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
            app_key_handle_registration(&ir_key_handle_cfg[i]);
        }
    }
    ir_printf_debug("%s,%d leave \n",__func__,__LINE__);
    return ret;
}

extern int app_ir_init(void);
extern int app_ir_tx_init(void);
/***************************************************************
 * Function     : app_ir_test
 * Description  : app_ir_test
 * Input        : None
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_nec_test(void)
{
    int ret;
    static uint8_t firstcall = 0;
    static uint8_t poweronff_flag = 0;
    uint8_t poweron[]={0XB2,0x1F,0XD8};//power on code 固定风,26°自动模式
    uint8_t poweroff[]={0XB2,0X7B,0XE0,0X00,0X00};//power code

    //uint8_t code[2] = {0x0b,0x05};
	ir_printf("%s,%d enter \n",__func__,__LINE__);

    if(firstcall == 0){
        firstcall = 1;
        //ir_register_keyboard_event_callback(ir_test_key_cb);
        //app_ir_init();
        //app_ir_tx_init();
    }

    if(poweronff_flag){ //on
        poweronff_flag = 0;
        app_ir_led(100);
        ir_printf_debug("%s,%d now power on ,poweronff_flag=%d\n",__func__,__LINE__,poweronff_flag);
        ret = hal_ir_tx(IR_MEDIA_R05D_PROTO,&poweron[0],sizeof(poweron),0);
    }
    else    //off
    {
        app_ir_led(0);
        poweronff_flag = 1;
        ir_printf_debug("%s,%d now power off ,poweronff_flag=%d \n",__func__,__LINE__,poweronff_flag);
        ret = hal_ir_tx(IR_MEDIA_R05D_PROTO,&poweroff[0],sizeof(poweroff),DRV_CMD_R05D_POWEROFF);
    }
    
    return ret;
}


/***************************************************************
 * Function     : app_ir_parse
 * Description  : app_ir_parse
 * Input        : None
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_parse(int argc, char *argv[])
{
    ir_printf_debug("%s,%d enter \n",__func__,__LINE__);

    ir_run_command(argc,&argv[0]);
    return 0;
}

#endif //IR_SUPPORT
