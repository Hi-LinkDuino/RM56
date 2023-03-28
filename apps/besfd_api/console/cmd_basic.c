/*****************************************************************************
* @file     cmd_basic.c
* @date     26.07.2016
* @note
*
******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "hal_trace.h"
#include "at.h"
#include "console.h"
static int my_isdigit(unsigned char c)
{
	return ((c >= '0') && (c <='9'));
}

static int my_isxdigit(unsigned char c)
{
	if ((c >= '0') && (c <='9'))
		return 1;
	if ((c >= 'a') && (c <='f'))
		return 1;
	if ((c >= 'A') && (c <='F'))
		return 1;
	return 0;
}

static int my_islower(unsigned char c)
{
	return ((c >= 'a') && (c <='z'));
}

static unsigned char my_toupper(unsigned char c)
{
	if (my_islower(c))
		c -= 'a'-'A';
	return c;
}

uint32_t simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	uint32_t result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && my_isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (my_isxdigit(*cp) && (value = my_isdigit(*cp) ? *cp-'0' : (my_islower(*cp)
			? my_toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

int cmd_get_data_size(const char *arg, int default_size)
{
	/* Check for a size specification .b, .w or .l. */
	int len = strlen(arg);


	switch(arg[len - 1]) {
		case 'b': return 1;
		case 'w': return 2;
		case 'l': return 4;
		case 's': return -2;
		default:  return -1;
	}


	return default_size;
}

void show_cmd_usage(const cmd_tbl_t *cmd)
{
	cmd_printf(NULL, "name:%s; maxargs:%d;\r\n%s\r\n", cmd->name, cmd->maxargs, cmd->usage);
}

static cmd_tbl_t * find_cmd (char *cmd)
{
	for (int i = 0;i < (int)cmd_cntr;i++) {
		cmd_tbl_t *cmdtp = &cmd_list[i];
		if (strcmp(cmd, cmdtp->name) == 0) {
				return cmdtp;
		}
	}
	return NULL;	/* not found or ambiguous command */
}

int add_cmd_to_list(const cmd_tbl_t *cmd)
{
	cmd_tbl_t *tmp_cmd = find_cmd(cmd->name);
	if(tmp_cmd){
		//found same name command, overwrite it.
		memcpy((char *)tmp_cmd, (char *)cmd, sizeof(cmd_tbl_t));
		return 0;
	}
	if(CMD_LIST_COUNT <= cmd_cntr) {
		cmd_printf(NULL, "No more cmds supported.\r\n");
		return -1;
	}
	tmp_cmd = &(cmd_list[cmd_cntr]);
	cmd_cntr++;
	memcpy((char *)tmp_cmd, (char *)cmd, sizeof(cmd_tbl_t));

	return 0;
}

int do_at(cmd_tbl_t *cmd, int argc, char *argv[])
{
	RESP_OK(NULL);
	return 0;
}
