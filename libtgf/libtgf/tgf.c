/*
 *  SCPx processing module project.
 *
 *  tgf.c - misc tgf functions
 *
 *  Created at Mon 07 May 2001 03:59:37 PM EEST by ALK
 *
 */
#include "tgf.h"
#include <stdlib.h>
#include "tgfmap.h"

static
char *	tgf_errors[1-E_TGF_LAST] = {
	"ok",
	"invalid type",
	"invalid header",
	"invalid CRC",
	"invalid arguments",
	"unexpected EOF",
	"version not supported",
	"no space",
	"operation not supported",
	"I/O error",
	"no memory",
	"invalid value",
	"client error",
	"internal BUG",
	"TGF command failed",
	"value too big",
	"unsupported feature"
};

const char *tgf_error(int err)
{
	if (err<E_TGF_LAST || err>0)
		return "unknown error";
	return tgf_errors[-err];
}

static
enum tgf_arg_type
	genelargs[] = {TGF_STRING,TGF_SCTYPE,TGF_NONE},
	setbegendargs[] = {TGF_USERID,TGF_USERID},
	findbyidargs[] = {TGF_STRING},
	switch_to_seg[] = {TGF_USERID},
	declare_segment[] = {TGF_STRING};

static	struct {
	enum tgf_cmd_type type;
	int	args_min;
	int	args_max;
	enum tgf_arg_type *args;
}	known_cmds[] =
{
	{TGF_GENEL,2,3,genelargs},
	{TGF_SETBEG,2,2,setbegendargs},
	{TGF_SETEND,2,2,setbegendargs},
	{TGF_FINDBYIDTF,1,1,findbyidargs},
	{TGF_DECLARE_SEGMENT,1,1,declare_segment},
	{TGF_SWITCH_TO_SEGMENT,1,1,switch_to_seg},
	{TGF_ENDOFSTREAM,0,0,0},
	{TGF_NOP,0,0,0}
};

/* I think this is the slowliest place in TGF now */
int	tgf_command_check(tgf_command *cmd)
{
	int i,k;
	for (i=0;i<sizeof(known_cmds)/sizeof(*known_cmds);i++)
		if (cmd->type == known_cmds[i].type)
			break;
	if (i == sizeof(known_cmds)/sizeof(*known_cmds))
		return E_TGF_OPNOTSUPP;
	if (cmd->arg_cnt < known_cmds[i].args_min)
		return E_TGF_INVARGS;
	if (cmd->arg_cnt > known_cmds[i].args_max)
		return E_TGF_INVARGS;
	for (k=0;k<cmd->arg_cnt;k++) {
		if (known_cmds[i].args[k] == TGF_USERID &&
				cmd->arguments[k].type == TGF_INT32)
			continue;
		if (known_cmds[i].args[k] != TGF_NONE &&
				cmd->arguments[k].type != known_cmds[i].args[k])
			break;
	}
	/* TODO: look at it */
	if (k<cmd->arg_cnt)
		return E_TGF_INVARGS;
	return i;
}


int	tgf_command_translate_in(tgf_command *cmd,tgf_map_in *map)
{
	int i = tgf_command_check(cmd);
	int k;
	void *id;
	if (i<0)
		return i;
	for (k=0;k<cmd->arg_cnt;k++) {
		if (known_cmds[i].args[k] != TGF_USERID)
			continue;
		if (cmd->arguments[k].arg.a_int32 == -1)
			continue;
		if (!tgf_map_in_lookup(map,cmd->arguments[k].arg.a_int32,&id))
			return E_TGF_CMDFAIL;
		cmd->arguments[k].type = TGF_USERID;
		cmd->arguments[k].arg.a_userid = id;
	}
	return 0;
}

int	tgf_command_translate_out(tgf_command *cmd,tgf_map_out *map)
{
	int i = tgf_command_check(cmd);
	int k;
	int id;
	if (i<0)
		return i;
	for (k=0;k<cmd->arg_cnt;k++) {
		if (known_cmds[i].args[k] != TGF_USERID)
			continue;
		if (cmd->arguments[k].type != TGF_USERID)
			return E_TGF_CLNT;
		if (cmd->arguments[k].arg.a_int32 == -1) {
			cmd->arguments[k].type = TGF_INT32;
			continue;
		}
		if (!tgf_map_out_lookup(map,cmd->arguments[k].arg.a_userid,&id))
			return E_TGF_CLNT;
		cmd->arguments[k].type = TGF_INT32;
		cmd->arguments[k].arg.a_int32 = id;
	}
	return 0;
}
