#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "console.h"
#include "at.h"
#include "hal_trace.h"
#include "cmsis.h"
#include "kfifo.h"
#include "hal_analogif.h"
#ifdef __WIFI_NVR_SUPPORT__
#include "nvrecord_wifi.h"
#endif
#include "factory_section.h"
#include "bwifi_interface.h"
#include "wifi_drv.h"
#include "net_debug.h"
#ifdef __NET_TELNET_SERVER_SUPPORT__
#include "telnetserver.h"
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define HW_READ_REG(a) *(volatile uint32_t *)(a)
#define HW_WRITE_REG(a,v) *(volatile uint32_t *)(a) = v

static char erase_seq[] = "\b \b";		 /* erase sequence		   */
static char   tab_seq[] = "    ";		 /* used to expand TABs    */
static unsigned char *console_recv_ring_buffer;
struct kfifo console_kfifo;
char console_buffer[CMD_CBSIZE];
char lastcommand[CMD_CBSIZE] = { 0, };
unsigned int echo_flag = 1;
volatile int auto_gain_flag = 0;
volatile int trace_enable_flag = 1;

static cmd_tbl_t *find_cmd (char *cmd)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = &cmd_list[0];	 /* Init value */
	uint32_t len;
	int n_found = 0;
	int i;

	len = strlen(cmd);
	for (i = 0;i < (int)cmd_cntr;i++) {
		cmdtp = &cmd_list[i];
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp->name))
				return cmdtp;	   /* full match */

			cmdtp_temp = cmdtp;    /* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {  /* exactly one match */
		return cmdtp_temp;
	}

	return 0;	/* not found or ambiguous command */
}

int do_help(cmd_tbl_t * cmdtp, int argc, char *argv[], void *handler)
{
    //int i;
    int rcode = 0;
    //extern const cmd_tbl_t cmd_list[];
    if(argc == 1)    /*show list of commands */
    {
        int cmd_items = MAX_CMD_ITEMS + 1;//+1 show at help cmd
        cmd_tbl_t *cmd_array[CMD_LIST_COUNT] = {NULL};
        int i, j, swaps;


        /* Make array of commands from cmd_list */
        cmdtp = (cmd_tbl_t *)cmd_list;
		if(strstr(argv[0], "AT+")){
			cmdtp = cmdtp + MAX_CMD_ITEMS;
			cmd_items = cmd_cntr - MAX_CMD_ITEMS;
			for(i = 0; i < cmd_cntr - MAX_CMD_ITEMS; i++)
	        {
	            cmd_array[i] = cmdtp++;
	        }
		}
        else{
			for(i = 0; i < cmd_items; i++)
	        {
	            cmd_array[i] = cmdtp++;
	        }
		}

        /* Sort command list (trivial bubble sort) */
        for(i = cmd_items - 1; i > 0; --i)
        {
            swaps = 0;
            for(j = 0; j < i; ++j)
            {
                if(strcmp (cmd_array[j]->name, cmd_array[j + 1]->name) > 0)
                {
                    cmd_tbl_t *tmp;
                    tmp = cmd_array[j];
                    cmd_array[j] = cmd_array[j + 1];
                    cmd_array[j + 1] = tmp;
                    ++swaps;
                }
            }

            if(!swaps)
                break;
        }

        /* print short help (usage) */
        for(i = 0; i < cmd_items; i++)
        {
            const char *usage = cmd_array[i]->usage;

            /* allow user abort */
            //if(ctrlc ())
            //    return 1;
            if(usage == NULL)
                continue;

            cmd_printf(handler, "%s", usage);
        }

        return 0;
    }
    return rcode;
}

/* Memory Display
 *
 * Syntax:
 *    md{.b, .w, .l} {addr} {len}
 */
int do_mem_md (cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    static unsigned long last_addr   = 0x00100000;
    static unsigned long last_length = 0x01;        /* Set to 1 for backward-compatible */
    static int           last_size   = 4;           /* Set to 4 for backward-compatible */

    unsigned int addr, length;
    unsigned int nbytes;
    int size;

    /* We use the last specified parameters, unless new ones are
     * entered.
     */
    addr   = last_addr;
    length = last_length;
	size   = last_size;

    if(argc < 2 || argc > 5)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    addr = simple_strtoul(argv[1], NULL, 0);
    if(argc > 2)
    {
        length = simple_strtoul(argv[2], NULL, 0);

    }

    if((size = cmd_get_data_size(argv[3], 4)) < 0)
    {
        return -2;
    }


    /* Print the lines.
     *
     * We buffer all read data, so we can make sure data is read only
     * once, and all accesses are with the specified bus width.
     */
    nbytes = length * size;
    do
    {
        #define DISP_LINE_LEN    (16)
        char             linebuf[DISP_LINE_LEN];
        unsigned int    *uip = (unsigned int   *)linebuf;
        unsigned short    *usp = (unsigned short *)linebuf;
        unsigned char    *ucp = (unsigned char  *)linebuf;
        unsigned char    *cp;
        unsigned long    linebytes;
        unsigned long    i;
        unsigned long    offset = 0;

        /* align address to 16bytes */
        cmd_printf(handler, "%04X-%04X:", addr >> 16, addr & 0xFFF0);

        /* make up first line, only first line maybe offset != 0 */
        offset = addr & 0xF;
        if(offset)
        {
            if(size == 2)
            {
                addr   &= ~0x01;
                if(offset & 0x01)
                {
                    nbytes += size;
                    offset &= ~0x01;
                }
            }
            else if(size == 4)
            {
                addr   &= ~0x03;
                if(offset & 0x03)
                {
                    nbytes += size;
                    offset &= ~0x03;
                }
            }
        }

        if(offset)
        {
            for(i = 0; i < offset; i += size)
            {
                if(size == 4)
                {
                    cmd_printf(handler, "         ");
                    *uip++ = 0x20202020;
                }
                else if(size == 2)
                {
                    cmd_printf(handler, "     ");
                    *usp++ = 0x2020;
                }
                else
                {
                    cmd_printf(handler, "   ");
                    *ucp++ = 0x20;
                }
            }

            linebytes = (nbytes > (DISP_LINE_LEN - offset)) ? (DISP_LINE_LEN - offset) : nbytes;
        }
        else
        {
            linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;
        }

        for(i = 0; i < linebytes; i += size)
        {
            if(size == 4)
            {
                cmd_printf(handler, " %08X", (*uip++ = *((unsigned int *)addr)));
            }
            else if(size == 2)
            {
                cmd_printf(handler, " %04X", (*usp++ = *((unsigned short *)addr)));
            }
            else
            {
                cmd_printf(handler, " %02X", (*ucp++ = *((unsigned char *)addr)));
            }

            addr += size;
        }

        /* make up last line */
        if((linebytes + offset) < DISP_LINE_LEN)
        {
            for(i = linebytes; i < DISP_LINE_LEN; i += size)
            {
                if(size == 4)
                {
                    cmd_printf(handler, "         ");
                }
                else if(size == 2)
                {
                    cmd_printf(handler, "     ");
                }
                else
                {
                    cmd_printf(handler, "   ");
                }
            }
        }

        /* seperator */
        cmd_printf(handler, "    ");

        /* ASCII */
        cp = (unsigned char *)linebuf;
        for(i = 0; i < (linebytes + offset); i++)
        {
            if((*cp < 0x20) || (*cp > 0x7e))
            {
                cmd_printf(handler, ".");
            }
            else
            {
                cmd_printf(handler, "%c", *cp);
            }
            cp++;
        }

        cmd_printf(handler, "\n");
        nbytes -= linebytes;
    } while(nbytes > 0);

    return 0;
}


int do_mem_mw (cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    unsigned int addr, writeval, count;
    int    size;

    if((argc < 3) || (argc > 5))
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    /* Check for size specification.
    */
    if((size = cmd_get_data_size(argv[4], 4)) < 1)
    {
        return -2;
    }

    /* Address is specified since argc > 1
    */
    addr = simple_strtoul(argv[1], NULL, 0);

    /* Get the value to write.
    */
    writeval = simple_strtoul(argv[2], NULL, 0);

    /* Count ? */
    if(argc == 4)
    {
        count = simple_strtoul(argv[3], NULL, 0);
    }
    else
    {
        count = 1;
    }
	cmd_printf(handler, "mw [%08X]=0x%08X\n", addr, writeval);

    while(count-- > 0)
    {
        if(size == 4)
        {
            *((unsigned long  *)addr) = (unsigned long )writeval;
        }
        else if(size == 2)
        {
            *((unsigned short *)addr) = (unsigned short)writeval;
        }
        else
        {
            *((unsigned char  *)addr) = (unsigned char )writeval;
        }

        addr += size;
    }

    return 0;
}


int do_phy_reg ( cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    unsigned int addr, value;

    if(argc < 2 || argc > 3)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    if(argc == 2)
    {
        addr = simple_strtoul(argv[1], NULL, 0);
        value = HW_READ_REG(addr);
    }
    else if(argc == 3)
    {
        addr  = simple_strtoul(argv[1], NULL, 0);
        value = simple_strtoul(argv[2], NULL, 0);
    	HW_WRITE_REG(addr,value);
    }

    cmd_printf(handler, "phy[%08X]=0x%08X\n", addr, value);
    return 0;
}


int do_rf_reg( cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{

    u16 addr;
    u16 value;

    if(argc < 2 || argc > 3)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    addr  = simple_strtoul(argv[1], NULL, 0);

    if(argc == 2)
    {
		hal_analogif_reg_read(addr, &value);
    }
    else if(argc == 3)
    {
        value = simple_strtoul(argv[2], NULL, 0);
		hal_analogif_reg_write(addr, value);
    }
	cmd_printf(handler, "rf[%02X]=0x%04X\n", addr, value);

    return 0;
}

extern int ethaddr_aton(const char *str, unsigned char *addr);
int do_mac_addr (cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    u8 maddr [ETH_ALEN] = {0};
    if(argc > 2)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    if(argc == 1)
    {
        uint8_t *mac_addr = factory_section_get_wifi_address();
        if (mac_addr)
        {
            cmd_printf(handler, MACSTR "\n", MAC2STR(mac_addr));
        }
    }
    if(argc == 2)
    {
        if(ethaddr_aton(argv[1], maddr) < 0)
        {
            cmd_printf(handler, "invalid mac address\n");
            return -2;
        }
        factory_section_set_wifi_address(maddr);
    }
    return 0;
}

int do_auto_gain_enable(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    if(argc > 2){
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    if(argc == 1){
		cmd_printf(handler, "auto_gain %d\n", auto_gain_flag);
    }
    if(argc == 2){
		auto_gain_flag = atoi(argv[1]);
		cmd_printf(handler, "auto_gain %d\n", auto_gain_flag);
    }
	return 0;
}

int do_trace_enable(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    if(argc > 2){
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    if(argc == 1){
		cmd_printf(handler, "trace_enable %d\n", trace_enable_flag);
    }
    if(argc == 2){
		trace_enable_flag = atoi(argv[1]);
		cmd_printf(handler, "trace_enable %d\n", trace_enable_flag);
    }
	return 0;
}
#ifdef __WIFI_NVR_SUPPORT__
int do_set_band(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
	nvrec_wifidevicerecord record;
	char band;
	int ret;
    if(argc > 2)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    if(argc == 1)
    {
		if (bwifi_find_record(BWIFI_RECORD_TYPE_BAND, &record) == 0) {
			cmd_printf(handler, "Get band:%d\n", record.wifi_record.band);
		}
		else{
			cmd_printf(handler, "Get band failed\n");
		}
    }
    if(argc == 2)
    {
    	band = (char)atoi(argv[1]);
	    if (band != 0 && band != 1 && band != 2) {
	        cmd_printf(handler, "Invalid band\n");
	        return 0;
	    }
	    strcpy((char *)record.type, BWIFI_RECORD_TYPE_BAND);
	    record.wifi_record.band = band;
	    ret = bwifi_add_record(&record);
		if(ret){
			cmd_printf(handler, "set band failed\n");
			return 0;
		}
		cmd_printf(handler, "Set band:%d\n", band);
    }
    return 0;
}

int do_set_country(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
	nvrec_wifidevicerecord record;
	int ret;
    if(argc > 2)
    {
        cmd_printf(handler, "Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    if(argc == 1)
    {
		if (bwifi_find_record(BWIFI_RECORD_TYPE_COUNTRY, &record) == 0) {
			cmd_printf(handler, "Get country:%c%c\n", record.wifi_record.country[0], record.wifi_record.country[1]);
		}
		else{
			cmd_printf(handler, "Get country code failed\n");
		}
    }
    if(argc == 2)
    {
	    if (strcmp(argv[1], "CN") != 0 && strcmp(argv[1], "JP") != 0 && \
			strcmp(argv[1], "EU") != 0 && strcmp(argv[1], "IN") != 0 && \
			strcmp(argv[1], "US") != 0) {
	        cmd_printf(handler, "Not supported\n");
	        return 0;
	    }
	    strcpy((char *)record.type, BWIFI_RECORD_TYPE_COUNTRY);
		record.wifi_record.country[0] = argv[1][0];
		record.wifi_record.country[1] = argv[1][1];
		record.wifi_record.country[2] = '\0';
	    ret = bwifi_add_record(&record);
		if(ret){
			cmd_printf(handler, "set country code failed\n");
			return 0;
		}
		cmd_printf(handler, "Set country:%c%c\n", record.wifi_record.country[0], record.wifi_record.country[1]);
    }
    return 0;
}
#endif
cmd_tbl_t cmd_list[CMD_LIST_COUNT] = {
    {
        "help",         4,  do_help,
        "help     - show CMD list\n"
    },
    {
        "md",           4,  do_mem_md,
        "md       - memory display"
        "	md address length size[b,w,l]\n"
    },
    {
        "mw",           5,  do_mem_mw,
        "mw       - memory write (fill)"
        "	mw address value count size[b,w,l]\n"
    },

    {
        "phy",          4,  do_phy_reg,
        "phy      - get/set phy register"
        "	phy address value \n"
    },

    {
        "rf",           4,  do_rf_reg,
        "rf       - get/set rf register"
        "	rf address value \n"
    },
    {
        "macaddr",      2,  do_mac_addr,
        "macaddr  - get/set mac address"
        "	macaddr xx:xx:xx:xx:xx:xx\n"
    },
    {
        "auto_gain",  2,  do_auto_gain_enable,
        "auto_gain  - get/set auto gain"
        "	0 disable 1 enable\n"
    },
    {
        "trace_enable",  2,  do_trace_enable,
        "trace_enable  - set trace enable/disable"
        "	0 disable 1 enable\n"
    },
#ifdef __WIFI_NVR_SUPPORT__
    {
        "band",  2,  do_set_band,
        "band  - get/set 2.4GHz/5GHz/2.4GHz& 5GHz"
        "	0 2.4Ghz 1 5Ghz 2 DUAL BAND\n"
    },
    {
        "countrycode",  2,  do_set_country,
        "countrycode  - set country code"
        "	CN JP IN US EU\n"
    },
#endif
};
unsigned int cmd_cntr = MAX_CMD_ITEMS;

void init_console_irq_buffer(void)
{
	console_recv_ring_buffer = malloc(CONSOLE_RING_BUFFER_SIZE);
	ASSERT(console_recv_ring_buffer, "MALLOC rece ring buffer fail\n");

	kfifo_init(&console_kfifo, console_recv_ring_buffer, CONSOLE_RING_BUFFER_SIZE);
}
void uninit_console_irq_buffer(void)
{
	nts_free(console_recv_ring_buffer);
}

static int parse_line (char *line, char *argv[], char* flag)
{
	int nargs = 0;

	while (nargs < CMD_MAXARGS) {
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
			if(flag && strcmp(flag, "AT") == 0){
				/* find end of string */
				while(*line && (*line != ',') && (*line != '=')) {
					++line;
				}
			}
			else{
				/* find end of string */
				while(*line && (*line != ' ') && (*line != '\t')) {
					++line;
				}
			}
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = 0;
			return (nargs);
		}

		*line++ = '\0'; 		/* terminate current arg	 */
	}
	return (nargs);
}

#if defined(IBRT)
extern void app_bt_cmd_perform_test(const char* bt_cmd, uint32_t cmd_len);
#endif

int run_command(void *handler, char *cmd)
{
	cmd_tbl_t *cmdtp;
	char *argv[CMD_MAXARGS + 1];	/* NULL terminated	  */
	int argc;

#if defined(IBRT)
	{
		char * cmd_p = NULL;
		TRACE(1, ">>>%s    \n",cmd);
		if(cmd_p = strstr(cmd, "ibrt_test:")){
			uint32_t cmd_len = 0;
			cmd_len = strlen(cmd_p);
			app_bt_cmd_perform_test(cmd_p + 10, cmd_len - 10);
			return 0;
		}
	}
#endif
	if(strstr(cmd, "AT+")){
        if(get_at_cmd_echo()){
            cmd_printf(handler, ">>>%s    \n",cmd);
        }
		/* Extract arguments */
		if ((argc = parse_line(cmd, argv, "AT")) == 0) {
			return -1;
		}
	}
	else{
		/* Extract arguments */
		if ((argc = parse_line(cmd, argv, NULL)) == 0) {
			return -1;
		}
	}

	/* Look up command in command table */
	if ((cmdtp = find_cmd(argv[0])) == 0) {
		RESP_ERROR(handler, ERROR_CMD);
		RESP(handler, "try \"%s\"\n", cmd_list[0].name);
		return -1;
	}

	/* found - check max args */
	if (argc > cmdtp->maxargs) {
		RESP_ERROR(handler, ERROR_ARG);
		return -1;
	}

	/* OK - call function to do the command */
	if ((cmdtp->cmd) (cmdtp, argc, argv, handler) != 0) {
		return -1;
	}

	return 0;
}

static char *delete_char(char *buffer, char *p, int *colp, int *np, int plen)
{
	char *s;

	if (*np == 0) {
		return (p);
	}

	if (*(--p) == '\t') {			 /* will retype the whole line	  */
		while (*colp > plen) {
			console_puts(erase_seq);
			(*colp)--;
		}
		for (s=buffer; s<p; ++s) {
			if (*s == '\t') {
				console_puts(tab_seq+((*colp) & 07));
				*colp += 8 - ((*colp) & 07);
			} else {
				++(*colp);
				console_putc(*s);
			}
		}
	} else {
		console_puts(erase_seq);
		(*colp)--;
	}
	(*np)--;
	return (p);
}


int handle_char(const char c, char *prompt) {
	static char   *p   = console_buffer;
	static int	  n    = 0; 			 /* buffer index		*/
	static int	  plen = 0; 		  /* prompt length	  */
	static int	  col;				  /* output column cnt	  */

	if (prompt) {
		plen = strlen(prompt);
		if(plen == 1 && prompt[0] == 'r')
			plen = 0;
		else
			console_puts(prompt);
		p = console_buffer;
		n = 0;
		return 0;
	}
	col = plen;

	/* Special character handling */
	switch (c) {
		case '\r':				  /* Enter		  */
		case '\n':
			*p = '\0';
			//console_puts(">>>");
			return (p - console_buffer);

		case '\0':				  /* nul			*/
			return -1;

		case 0x03:				  /* ^C - break 	   */
			console_buffer[0] = '\0';	 /* discard input */
			return 0;

		case 0x15:				  /* ^U - erase line	*/
			while (col > plen) {
				//console_puts(erase_seq);
				--col;
			}
			p = console_buffer;
			n = 0;
			return -1;

		case 0x17:				  /* ^W - erase word	 */
			p=delete_char(console_buffer, p, &col, &n, plen);
			while ((n > 0) && (*p != ' ')) {
				p=delete_char(console_buffer, p, &col, &n, plen);
			}
			return -1;

		case 0x08:				  /* ^H  - backspace	*/
		case 0x7F:				  /* DEL - backspace	*/
			p=delete_char(console_buffer, p, &col, &n, plen);
			return -1;
		case '&':
			if (p!=console_buffer && (*(p-1)=='&')) {
				p--;
				*p = '\0';
				return (p - console_buffer);
			}

		default:
		 /*  Must be a normal character then  */
			if (n < CMD_CBSIZE-2) {
				if (c == '\t') {	/* expand TABs		  */
					//console_puts(tab_seq+(col&07));
					col += 8 - (col&07);
				} else {
					if(echo_flag == 1){
						++col;		   /* echo input		*/
						//console_putc(c);
					}
				}
				*p++ = c;
				++n;
			} else {		  /* Buffer full		*/
				//console_putc('\a');
			}
	}

	return -1;
}

