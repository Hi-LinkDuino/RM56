#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

////////////////////////////////////////////////////////////////////////////
EXTERNC void dbgOutput(unsigned n, const void* data, int size);
EXTERNC void dbgPrintf(const char* pattern, ...);
EXTERNC void dbgPrintfs(const char*name,unsigned n, ...);
EXTERNC void dbgPrintfx(const char* name, unsigned n, va_list* ap);
EXTERNC void dbgDumpFile(unsigned n, const char* url);
EXTERNC const char* dbgPath(const char* path);
EXTERNC const char* dbgFunc(const char* func);
////////////////////////////////////////////////////////////////////////////
EXTERNC void dbgListSet(void* buff, int size);
EXTERNC void dbgListAppend(const char* fmt, ...);
EXTERNC void dbgListOutput(void);
EXTERNC void dbgListClear(void);
////////////////////////////////////////////////////////////////////////////
EXTERNC void dbgErrProcess(unsigned file, unsigned line, unsigned error);
#ifndef __cplusplus
#define dbgErrProc(e) dbgErrProcess(0xffff,__LINE__,e);
#else
#include "smf_debug_hash.h"
#define dbgErrProc(e) dbgErrProcess(dbgFileNameHash(__FILE__),__LINE__,e);
#endif
////////////////////////////////////////////////////////////////////////////
#define __KEYS__ 0
#define __TYPE__ 0
#define dbgChnEntry(x,...)			dbgPrintfs(#x,dbg_chn_##x,__KEYS__,__TYPE__,__FILE__, __LINE__,__FUNCTION__,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbg_chn_err 0
#define dbg_chn_info 1
#define dbg_chn_note 2
#define dbg_chn_list 3
#define dbg_chn_log 4
#define dbg_chn_mem 5
#define dbg_chn_io 6
#define dbg_chn_test 7
#define dbg_chn_demo 8
#define dbg_chn_9 9
#define dbg_chn_a 10
#define dbg_chn_b 11
#define dbg_chn_c 12
#define dbg_chn_d 13
#define dbg_chn_e 14
#define dbg_chn_f 15
////////////////////////////////////////////////////////////////////////////
#define dbg_err_printf(...)				dbgChnEntry(err,##__VA_ARGS__)
#define dbg_info_printf(...)			dbgChnEntry(info,##__VA_ARGS__)
#define dbg_note_printf(...)			dbgChnEntry(note,##__VA_ARGS__)
#define dbg_list_printf(...)			dbgChnEntry(list,##__VA_ARGS__)
#define dbg_log_printf(...)				dbgChnEntry(log,##__VA_ARGS__)
#define dbg_mem_printf(...)				dbgChnEntry(mem,##__VA_ARGS__)
#define dbg_io_printf(...)				dbgChnEntry(io,##__VA_ARGS__)
#define dbg_test_printf(...)			dbgChnEntry(test,##__VA_ARGS__)
#define dbg_demo_printf(...)			dbgChnEntry(demo,##__VA_ARGS__)
#define dbg_9_printf(...)				dbgChnEntry(9,##__VA_ARGS__)
#define dbg_a_printf(...)				dbgChnEntry(a,##__VA_ARGS__)
#define dbg_b_printf(...)				dbgChnEntry(b,##__VA_ARGS__)
#define dbg_c_printf(...)				dbgChnEntry(c,##__VA_ARGS__)
#define dbg_d_printf(...)				dbgChnEntry(d,##__VA_ARGS__)
#define dbg_e_printf(...)				dbgChnEntry(e,##__VA_ARGS__)
#define dbg_f_printf(...)				dbgChnEntry(f,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define returnIf(v,c) if(c)return v
#define returnIfE(v,c,e) if(c){dbgErrProc(e);return v;}
////////////////////////////////////////////////////////////////////////////
enum {
	DBG_OUTPUT_END = 1 << 8,
	DBG_OUTPUT_CHN = 1 << 9,
	DBG_OUTPUT_DUMP = 1 << 10,
	DBG_OUTPUT_SKIP = 1 << 11,
	DBG_OUTPUT_POS = 1 << 12,
	DBG_OUTPUT_KEYS = 1 << 13,
};
////////////////////////////////////////////////////////////////////////////
#define dbgChn(x,...)					dbg_##x##_printf(0,##__VA_ARGS__)
#define dbgChnX(x,...)					dbg_##x##_printf(DBG_OUTPUT_CHN,##__VA_ARGS__)
#define dbgChnP(x,...)					dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_POS,##__VA_ARGS__)
#define dbgChnPL(x)						dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_POS|DBG_OUTPUT_END,0)
#define dbgChnPXL(x,...)				dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_POS|DBG_OUTPUT_END,##__VA_ARGS__)
#define dbgChnPCL(x,c,...)				dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_POS|DBG_OUTPUT_END|(c?0:DBG_OUTPUT_SKIP),##__VA_ARGS__)
#define dbgChnDump(x,d,z)				dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_DUMP,d,z)
#define dbgChnDumpL(x,d,z,l)			dbg_##x##_printf(DBG_OUTPUT_CHN|DBG_OUTPUT_DUMP,d,z,l)
#define dbgChnSL(x,v)					dbgChn(x,#v"=%s\n",((const char*)v))
#define dbgChnDL(x,v)					dbgChn(x,#v"=%d\n",((int)v))
#define dbgChnPDL(x,v)					dbgChnPXL(x,#v"=%d",((int)v))
#define dbgChnPDLZ(x,v)					dbgChnPCL(x,v,#v"=%d",((int)v))
#define dbgChnPPL(x,v)					dbgChnPXL(x,#v"=%p",((void*)v))
#define dbgChnPPLZ(x,v)					dbgChnPCL(x,v,#v"=%p",((void*)v))
#define dbgChnPSL(x,v)					dbgChnPXL(x,#v"=%s",(v?((const char*)v):""))
#define dbgChnPSLZ(x,v)					dbgChnPCL(x,v,#v"=%s",v)
#define dbgChnPSL0(x,v)					dbgChnPXL(x,#v"=%s",(const char*)v)
#define dbgChnPEL(x,e,...)				dbgErrProc(e);dbgChnPXL(x,##__VA_ARGS__)
#define returnIfChnC(x,v,c)				if(c){dbgChnPXL(x,#c);return v;}
#define returnIfChnCS(x,v,c,s,...)		if(c){dbgChnPXL(x,#c"#"#s,##__VA_ARGS__);return v;}
#define returnIfChnC0(x,c)				if(c){dbgChnPXL(x,#c);return;}
#define returnIfChnCS0(x,c,s,...)		if(c){dbgChnPXL(x,#c"#"#s,##__VA_ARGS__);return;}
#define returnIfChnF(x,v,c,f)			if(c){dbgChnPXL(x,#c);f;return v;}
#define returnIfChnFS(x,v,c,f,s,...)	if(c){dbgChnPXL(x,#c"#"#s,##__VA_ARGS__);f;return v;}
#define returnIfChnE(x,v,c,e)			if(c){dbgChnPEL(x,e,#c);return v;}
#define returnIfChnES(x,v,c,e,s,...)	if(c){dbgChnPEL(x,e,#c"#"#s,##__VA_ARGS__);return v;}
#define returnIfChnE0(x,c,e)			if(c){dbgChnPEL(x,e,#c);return;}
#define returnIfChnES0(x,c,e,s,...)		if(c){dbgChnPEL(x,e,#c"#"#s,##__VA_ARGS__);return;}
#define returnIfChnEF(x,v,c,e,f)		if(c){dbgChnPEL(x,e,#c);f;return v;}
#define returnIfChnEFS(x,v,c,e,f,s,...)	if(c){dbgChnPEL(x,e,#c"#"#s,##__VA_ARGS__);f;return v;}
////////////////////////////////////////////////////////////////////////////
#define dbgErr(...)							dbgErrProc(1);dbgChn(err,##__VA_ARGS__)
#define dbgErrX(...)						dbgErrProc(1);dbgChnX(err,##__VA_ARGS__)
#define dbgErrP(...)						dbgErrProc(1);dbgChnP(err,##__VA_ARGS__)
#define dbgErrDL(v)							dbgErrProc(1);dbgChnDL(err,v)
#define dbgErrSL(v)							dbgErrProc(1);dbgChnSL(err,v)
#define dbgErrPL()							dbgErrProc(1);dbgChnPL(err)
#define dbgErrPXL(s,...)					dbgErrProc(1);dbgChnPXL(err,s,##__VA_ARGS__)
#define dbgErrPDL(v)						dbgErrProc(1);dbgChnPDL(err,v)
#define dbgErrPPL(v)						dbgErrProc(1);dbgChnPPL(err,v)
#define dbgErrPSL(v)						dbgErrProc(1);dbgChnPSL(err,v)
#define dbgErrPSL0(v)						dbgErrProc(1);dbgChnPSL0(err,v)
#define dbgErrDump(d,z)						dbgErrProc(1);dbgChnDump(err,d,z)
#define dbgErrPEL(e,...)					dbgChnPEL(err,e,##__VA_ARGS__)
#define returnIfErrC(v,c)					returnIfChnE(err,v,c,1)			
#define returnIfErrCS(v,c,s,...)			returnIfChnES(err,v,c,1,s,##__VA_ARGS__)	
#define returnIfErrC0(c)					returnIfChnE0(err,c,1)			
#define returnIfErrCS0(c,s,...)				returnIfChnES0(err,c,1,s,##__VA_ARGS__)	
#define returnIfErrF(v,c,f)					returnIfChnEF(err,v,c,1,f)		
#define returnIfErrFS(v,c,f,s,...)			returnIfChnEFS(err,v,c,1,f,s,##__VA_ARGS__)	
#define returnIfErrE(v,c,e)					returnIfChnE(err,v,c,e)			
#define returnIfErrES(v,c,e,s,...)			returnIfChnES(err,v,c,e,s,##__VA_ARGS__)		
#define returnIfErrE0(c,e)					returnIfChnE0(err,c,e)				
#define returnIfErrES0(c,e,s,...)			returnIfChnES0(err,c,e,s,##__VA_ARGS__)		
#define returnIfErrEF(v,c,e,f)				returnIfChnEF(err,v,c,e,f)			
#define returnIfErrEFS(v,c,e,f,s,...)		returnIfChnEFS(err,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgInfo(...)						dbgChn(info,##__VA_ARGS__)
#define dbgInfoX(...)						dbgChnX(info,##__VA_ARGS__)
#define dbgInfoP(...)						dbgChnP(info,##__VA_ARGS__)
#define dbgInfoDL(v)						dbgChnDL(info,v)
#define dbgInfoSL(v)						dbgChnSL(info,v)
#define dbgInfoPL()							dbgChnPL(info)
#define dbgInfoPXL(s,...)					dbgChnPXL(info,s,##__VA_ARGS__)
#define dbgInfoPDL(v)						dbgChnPDL(info,v)
#define dbgInfoPPL(v)						dbgChnPPL(info,v)
#define dbgInfoPSL(v)						dbgChnPSL(info,v)
#define dbgInfoPSL0(v)						dbgChnPSL0(info,v)
#define dbgInfoPDLZ(v)						dbgChnPDLZ(info,v)
#define dbgInfoPPLZ(v)						dbgChnPPLZ(info,v)
#define dbgInfoPSLZ(v)						dbgChnPSLZ(info,v)
#define dbgInfoPEL(e,...)					dbgChnPEL(info,e,##__VA_ARGS__)
#define dbgInfoDump(d,z)					dbgChnDump(info,d,z)
#define returnIfInfoC(v,c)					returnIfChnC(info,v,c)			
#define returnIfInfoCS(v,c,s,...)			returnIfChnCS(info,v,c,s,##__VA_ARGS__)	
#define returnIfInfoC0(c)					returnIfChnC0(info,c)			
#define returnIfInfoCS0(c,s,...)			returnIfChnCS0(info,c,s,##__VA_ARGS__)	
#define returnIfInfoF(v,c,f)				returnIfChnF(info,v,c,f)		
#define returnIfInfoFS(v,c,f,s,...)			returnIfChnFS(info,v,c,f,s,##__VA_ARGS__)	
#define returnIfInfoE(v,c,e)				returnIfChnE(info,v,c,e)			
#define returnIfInfoES(v,c,e,s,...)			returnIfChnES(info,v,c,e,s,##__VA_ARGS__)		
#define returnIfInfoE0(c,e)					returnIfChnE0(info,c,e)				
#define returnIfInfoES0(c,e,s,...)			returnIfChnES0(info,c,e,s,##__VA_ARGS__)		
#define returnIfInfoEF(v,c,e,f)				returnIfChnEF(info,v,c,e,f)			
#define returnIfInfoEFS(v,c,e,f,s,...)		returnIfChnEFS(info,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgNote(...)						dbgChn(note,##__VA_ARGS__)
#define dbgNoteX(...)						dbgChnX(note,##__VA_ARGS__)
#define dbgNoteP(...)						dbgChnP(note,##__VA_ARGS__)
#define dbgNoteDL(v)						dbgChnDL(note,v)
#define dbgNoteSL(v)						dbgChnSL(note,v)
#define dbgNotePL()							dbgChnPL(note)
#define dbgNotePXL(s,...)					dbgChnPXL(note,s,##__VA_ARGS__)
#define dbgNotePDL(v)						dbgChnPDL(note,v)
#define dbgNotePPL(v)						dbgChnPPL(note,v)
#define dbgNotePSL(v)						dbgChnPSL(note,v)
#define dbgNotePDLZ(v)						dbgChnPDLZ(note,v)
#define dbgNotePPLZ(v)						dbgChnPPLZ(note,v)
#define dbgNotePSLZ(v)						dbgChnPSLZ(note,v)
#define dbgNotePSL0(v)						dbgChnPSL0(note,v)
#define dbgNotePEL(e,...)					dbgChnPEL(note,e,##__VA_ARGS__)
#define dbgNoteDump(d,z)					dbgChnDump(note,d,z)
#define returnIfNoteC(v,c)					returnIfChnC(note,v,c)			
#define returnIfNoteCS(v,c,s,...)			returnIfChnCS(note,v,c,s,##__VA_ARGS__)	
#define returnIfNoteC0(c)					returnIfChnC0(note,c)			
#define returnIfNoteCS0(c,s,...)			returnIfChnCS0(note,c,s,##__VA_ARGS__)	
#define returnIfNoteF(v,c,f)				returnIfChnF(note,v,c,f)		
#define returnIfNoteFS(v,c,f,s,...)			returnIfChnFS(note,v,c,f,s,##__VA_ARGS__)	
#define returnIfNoteE(v,c,e)				returnIfChnE(note,v,c,e)			
#define returnIfNoteES(v,c,e,s,...)			returnIfChnES(note,v,c,e,s,##__VA_ARGS__)		
#define returnIfNoteE0(c,e)					returnIfChnE0(note,c,e)				
#define returnIfNoteES0(c,e,s,...)			returnIfChnES0(note,c,e,s,##__VA_ARGS__)		
#define returnIfNoteEF(v,c,e,f)				returnIfChnEF(note,v,c,e,f)			
#define returnIfNoteEFS(v,c,e,f,s,...)		returnIfChnEFS(note,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgList(...)						dbgChn(list,##__VA_ARGS__)
#define dbgListX(...)						dbgChnX(list,##__VA_ARGS__)
#define dbgListP(...)						dbgChnP(list,##__VA_ARGS__)
#define dbgListDL(v)						dbgChnDL(list,v)
#define dbgListSL(v)						dbgChnSL(list,v)
#define dbgListPL()							dbgChnPL(list)
#define dbgListPXL(s,...)					dbgChnPXL(list,s,##__VA_ARGS__)
#define dbgListPDL(v)						dbgChnPDL(list,v)
#define dbgListPPL(v)						dbgChnPPL(list,v)
#define dbgListPSL(v)						dbgChnPSL(list,v)
#define dbgListPDLZ(v)						dbgChnPDLZ(list,v)
#define dbgListPPLZ(v)						dbgChnPPLZ(list,v)
#define dbgListPSLZ(v)						dbgChnPSLZ(list,v)
#define dbgListPSL0(v)						dbgChnPSL0(list,v)
#define dbgListPEL(e,...)					dbgChnPEL(list,e,##__VA_ARGS__)
#define dbgListDump(d,z)					dbgChnDump(list,d,z)
#define returnIfListC(v,c)					returnIfChnC(list,v,c)			
#define returnIfListCS(v,c,s,...)			returnIfChnCS(list,v,c,s,##__VA_ARGS__)	
#define returnIfListC0(c)					returnIfChnC0(list,c)			
#define returnIfListCS0(c,s,...)			returnIfChnCS0(list,c,s,##__VA_ARGS__)	
#define returnIfListF(v,c,f)				returnIfChnF(list,v,c,f)		
#define returnIfListFS(v,c,f,s,...)			returnIfChnFS(list,v,c,f,s,##__VA_ARGS__)	
#define returnIfListE(v,c,e)				returnIfChnE(list,v,c,e)			
#define returnIfListES(v,c,e,s,...)			returnIfChnES(list,v,c,e,s,##__VA_ARGS__)		
#define returnIfListE0(c,e)					returnIfChnE0(list,c,e)				
#define returnIfListES0(c,e,s,...)			returnIfChnES0(list,c,e,s,##__VA_ARGS__)		
#define returnIfListEF(v,c,e,f)				returnIfChnEF(list,v,c,e,f)			
#define returnIfListEFS(v,c,e,f,s,...)		returnIfChnEFS(list,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgLog(...)							dbgChn(log,##__VA_ARGS__)
#define dbgLogX(...)						dbgChnX(log,##__VA_ARGS__)
#define dbgLogP(...)						dbgChnP(log,##__VA_ARGS__)
#define dbgLogDL(v)							dbgChnDL(log,v)
#define dbgLogSL(v)							dbgChnSL(log,v)
#define dbgLogPL()							dbgChnPL(log)
#define dbgLogPXL(s,...)					dbgChnPXL(log,s,##__VA_ARGS__)
#define dbgLogPDL(v)						dbgChnPDL(log,v)
#define dbgLogPPL(v)						dbgChnPPL(log,v)
#define dbgLogPSL(v)						dbgChnPSL(log,v)
#define dbgLogPDLZ(v)						dbgChnPDLZ(log,v)
#define dbgLogPPLZ(v)						dbgChnPPLZ(log,v)
#define dbgLogPSLZ(v)						dbgChnPSLZ(log,v)
#define dbgLogPSL0(v)						dbgChnPSL0(log,v)
#define dbgLogPEL(e,...)					dbgChnPEL(log,e,##__VA_ARGS__)
#define dbgLogDump(d,z)						dbgChnDump(log,d,z)
#define returnIfLogC(v,c)					returnIfChnC(log,v,c)			
#define returnIfLogCS(v,c,s,...)			returnIfChnCS(log,v,c,s,##__VA_ARGS__)	
#define returnIfLogC0(c)					returnIfChnC0(log,c)			
#define returnIfLogCS0(c,s,...)				returnIfChnCS0(log,c,s,##__VA_ARGS__)	
#define returnIfLogF(v,c,f)					returnIfChnF(log,v,c,f)		
#define returnIfLogFS(v,c,f,s,...)			returnIfChnFS(log,v,c,f,s,##__VA_ARGS__)	
#define returnIfLogE(v,c,e)					returnIfChnE(log,v,c,e)			
#define returnIfLogES(v,c,e,s,...)			returnIfChnES(log,v,c,e,s,##__VA_ARGS__)		
#define returnIfLogE0(c,e)					returnIfChnE0(log,c,e)				
#define returnIfLogES0(c,e,s,...)			returnIfChnES0(log,c,e,s,##__VA_ARGS__)		
#define returnIfLogEF(v,c,e,f)				returnIfChnEF(log,v,c,e,f)			
#define returnIfLogEFS(v,c,e,f,s,...)		returnIfChnEFS(log,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgMem(...)							dbgChn(mem,##__VA_ARGS__)
#define dbgMemX(...)						dbgChnX(mem,##__VA_ARGS__)
#define dbgMemP(...)						dbgChnP(mem,##__VA_ARGS__)
#define dbgMemDL(v)							dbgChnDL(mem,v)
#define dbgMemSL(v)							dbgChnSL(mem,v)
#define dbgMemPL()							dbgChnPL(mem)
#define dbgMemPXL(s,...)					dbgChnPXL(mem,s,##__VA_ARGS__)
#define dbgMemPDL(v)						dbgChnPDL(mem,v)
#define dbgMemPPL(v)						dbgChnPPL(mem,v)
#define dbgMemPSL(v)						dbgChnPSL(mem,v)
#define dbgMemPDLZ(v)						dbgChnPDLZ(mem,v)
#define dbgMemPPLZ(v)						dbgChnPPLZ(mem,v)
#define dbgMemPSLZ(v)						dbgChnPSLZ(mem,v)
#define dbgMemPSL0(v)						dbgChnPSL0(mem,v)
#define dbgMemPEL(e,...)					dbgChnPEL(mem,e,##__VA_ARGS__)
#define dbgMemDump(d,z)						dbgChnDump(mem,d,z)
#define returnIfMemC(v,c)					returnIfChnC(mem,v,c)			
#define returnIfMemCS(v,c,s,...)			returnIfChnCS(mem,v,c,s,##__VA_ARGS__)	
#define returnIfMemC0(c)					returnIfChnC0(mem,c)			
#define returnIfMemCS0(c,s,...)				returnIfChnCS0(mem,c,s,##__VA_ARGS__)	
#define returnIfMemF(v,c,f)					returnIfChnF(mem,v,c,f)		
#define returnIfMemFS(v,c,f,s,...)			returnIfChnFS(mem,v,c,f,s,##__VA_ARGS__)	
#define returnIfMemE(v,c,e)					returnIfChnE(mem,v,c,e)			
#define returnIfMemES(v,c,e,s,...)			returnIfChnES(mem,v,c,e,s,##__VA_ARGS__)		
#define returnIfMemE0(c,e)					returnIfChnE0(mem,c,e)				
#define returnIfMemES0(c,e,s,...)			returnIfChnES0(mem,c,e,s,##__VA_ARGS__)		
#define returnIfMemEF(v,c,e,f)				returnIfChnEF(mem,v,c,e,f)			
#define returnIfMemEFS(v,c,e,f,s,...)		returnIfChnEFS(mem,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgIO(...)							dbgChn(io,##__VA_ARGS__)
#define dbgIOX(...)							dbgChnX(io,##__VA_ARGS__)
#define dbgIOP(...)							dbgChnP(io,##__VA_ARGS__)
#define dbgIODL(v)							dbgChnDL(io,v)
#define dbgIOSL(v)							dbgChnSL(io,v)
#define dbgIOPL()							dbgChnPL(io)
#define dbgIOPXL(s,...)						dbgChnPXL(io,s,##__VA_ARGS__)
#define dbgIOPDL(v)							dbgChnPDL(io,v)
#define dbgIOPPL(v)							dbgChnPPL(io,v)
#define dbgIOPSL(v)							dbgChnPSL(io,v)
#define dbgIOPDLZ(v)						dbgChnPDLZ(io,v)
#define dbgIOPPLZ(v)						dbgChnPPLZ(io,v)
#define dbgIOPSLZ(v)						dbgChnPSLZ(io,v)
#define dbgIOPSL0(v)						dbgChnPSL0(io,v)
#define dbgIOPEL(e,...)						dbgChnPEL(io,e,##__VA_ARGS__)
#define dbgIODump(d,z)						dbgChnDump(io,d,z)
#define returnIfIOC(v,c)					returnIfChnC(io,v,c)			
#define returnIfIOCS(v,c,s,...)				returnIfChnCS(io,v,c,s,##__VA_ARGS__)	
#define returnIfIOC0(c)						returnIfChnC0(io,c)			
#define returnIfIOCS0(c,s,...)				returnIfChnCS0(io,c,s,##__VA_ARGS__)	
#define returnIfIOF(v,c,f)					returnIfChnF(io,v,c,f)		
#define returnIfIOFS(v,c,f,s,...)			returnIfChnFS(io,v,c,f,s,##__VA_ARGS__)	
#define returnIfIOE(v,c,e)					returnIfChnE(io,v,c,e)			
#define returnIfIOES(v,c,e,s,...)			returnIfChnES(io,v,c,e,s,##__VA_ARGS__)		
#define returnIfIOE0(c,e)					returnIfChnE0(io,c,e)				
#define returnIfIOES0(c,e,s,...)			returnIfChnES0(io,c,e,s,##__VA_ARGS__)		
#define returnIfIOEF(v,c,e,f)				returnIfChnEF(io,v,c,e,f)			
#define returnIfIOEFS(v,c,e,f,s,...)		returnIfChnEFS(io,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgTest(...)						dbgChn(test,##__VA_ARGS__)
#define dbgTestX(...)						dbgChnX(test,##__VA_ARGS__)
#define dbgTestP(...)						dbgChnP(test,##__VA_ARGS__)
#define dbgTestDL(v)						dbgChnDL(test,v)
#define dbgTestSL(v)						dbgChnSL(test,v)
#define dbgTestPL()							dbgChnPL(test)
#define dbgTestPXL(s,...)					dbgChnPXL(test,s,##__VA_ARGS__)
#define dbgTestPDL(v)						dbgChnPDL(test,v)
#define dbgTestPPL(v)						dbgChnPPL(test,v)
#define dbgTestPSL(v)						dbgChnPSL(test,v)
#define dbgTestPDLZ(v)						dbgChnPDLZ(test,v)
#define dbgTestPPLZ(v)						dbgChnPPLZ(test,v)
#define dbgTestPSLZ(v)						dbgChnPSLZ(test,v)
#define dbgTestPSL0(v)						dbgChnPSL0(test,v)
#define dbgTestPEL(e,...)					dbgChnPEL(test,e,##__VA_ARGS__)
#define dbgTestDump(d,z)					dbgChnDump(test,d,z)
#define dbgTestDumpL(d,z,l)					dbgChnDumpL(test,d,z,l)
#define returnIfTestC(v,c)					returnIfChnC(test,v,c)			
#define returnIfTestCS(v,c,s,...)			returnIfChnCS(test,v,c,s,##__VA_ARGS__)	
#define returnIfTestC0(c)					returnIfChnC0(test,c)			
#define returnIfTestCS0(c,s,...)			returnIfChnCS0(test,c,s,##__VA_ARGS__)	
#define returnIfTestF(v,c,f)				returnIfChnF(test,v,c,f)		
#define returnIfTestFS(v,c,f,s,...)			returnIfChnFS(test,v,c,f,s,##__VA_ARGS__)	
#define returnIfTestE(v,c,e)				returnIfChnE(test,v,c,e)			
#define returnIfTestES(v,c,e,s,...)			returnIfChnES(test,v,c,e,s,##__VA_ARGS__)		
#define returnIfTestE0(c,e)					returnIfChnE0(test,c,e)				
#define returnIfTestES0(c,e,s,...)			returnIfChnES0(test,c,e,s,##__VA_ARGS__)		
#define returnIfTestEF(v,c,e,f)				returnIfChnEF(test,v,c,e,f)			
#define returnIfTestEFS(v,c,e,f,s,...)		returnIfChnEFS(test,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
#define dbgDemo(...)						dbgChn(demo,##__VA_ARGS__)
#define dbgDemoX(...)						dbgChnX(demo,##__VA_ARGS__)
#define dbgDemoP(...)						dbgChnP(demo,##__VA_ARGS__)
#define dbgDemoDL(v)						dbgChnDL(demo,v)
#define dbgDemoSL(v)						dbgChnSL(demo,v)
#define dbgDemoPL()							dbgChnPL(demo)
#define dbgDemoPXL(s,...)					dbgChnPXL(demo,s,##__VA_ARGS__)
#define dbgDemoPDL(v)						dbgChnPDL(demo,v)
#define dbgDemoPPL(v)						dbgChnPPL(demo,v)
#define dbgDemoPSL(v)						dbgChnPSL(demo,v)
#define dbgDemoPDLZ(v)						dbgChnPDLZ(demo,v)
#define dbgDemoPPLZ(v)						dbgChnPPLZ(demo,v)
#define dbgDemoPSLZ(v)						dbgChnPSLZ(demo,v)
#define dbgDemoPSL0(v)						dbgChnPSL0(demo,v)
#define dbgDemoPEL(e,...)					dbgChnPEL(demo,e,##__VA_ARGS__)
#define dbgDemoDump(d,z)					dbgChnDump(demo,d,z)
#define returnIfDemoC(v,c)					returnIfChnC(demo,v,c)			
#define returnIfDemoCS(v,c,s,...)			returnIfChnCS(demo,v,c,s,##__VA_ARGS__)	
#define returnIfDemoC0(c)					returnIfChnC0(demo,c)			
#define returnIfDemoCS0(c,s,...)			returnIfChnCS0(demo,c,s,##__VA_ARGS__)	
#define returnIfDemoF(v,c,f)				returnIfChnF(demo,v,c,f)		
#define returnIfDemoFS(v,c,f,s,...)			returnIfChnFS(demo,v,c,f,s,##__VA_ARGS__)	
#define returnIfDemoE(v,c,e)				returnIfChnE(demo,v,c,e)			
#define returnIfDemoES(v,c,e,s,...)			returnIfChnES(demo,v,c,e,s,##__VA_ARGS__)		
#define returnIfDemoE0(c,e)					returnIfChnE0(demo,c,e)				
#define returnIfDemoES0(c,e,s,...)			returnIfChnES0(demo,c,e,s,##__VA_ARGS__)		
#define returnIfDemoEF(v,c,e,f)				returnIfChnEF(demo,v,c,e,f)			
#define returnIfDemoEFS(v,c,e,f,s,...)		returnIfChnEFS(demo,v,c,e,f,s,##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////
