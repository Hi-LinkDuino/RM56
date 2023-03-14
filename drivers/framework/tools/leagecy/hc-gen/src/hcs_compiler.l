%option noyywrap
%option nodefault
%option yylineno
%option nounput
%option noinput
%x CMT

%{
/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_parser.h"
#include "hcs_compiler_tab.h"

#ifdef LEXER_DEBUG_ENABLE
#define LEXER_DEBUG(...) printf(__VA_ARGS__)
#else
#define LEXER_DEBUG(...)
#endif

extern void HcsCompilererror (char *s);
%}

%%
"/*"            {BEGIN(CMT);}
<CMT>"*/"   {BEGIN(INITIAL); }
<CMT>([^*]|\n)+|.
<CMT><<EOF>>    {LEXER_DEBUG("%d:unterminated comment\n",  HcsCompilerlineno); return 0;}
"//".*\n
"{"                 { LEXER_DEBUG("{\n"); return HcsCompilertext[0]; };
"}"                 { LEXER_DEBUG("}\n"); return HcsCompilertext[0]; };
":"                 { LEXER_DEBUG(":\n"); return HcsCompilertext[0]; };
"["                 { LEXER_DEBUG("[\n"); return HcsCompilertext[0]; };
"]"                 { LEXER_DEBUG("]\n"); return HcsCompilertext[0]; };
";"                 { LEXER_DEBUG(";\n"); return HcsCompilertext[0]; };
"&"                 { LEXER_DEBUG("REF\n"); return HcsCompilertext[0]; }
"="                 { LEXER_DEBUG("=\n"); return HcsCompilertext[0]; }
","                 { LEXER_DEBUG("=\n"); return HcsCompilertext[0]; }
"root"              { LEXER_DEBUG("root\n"); return ROOT; }
"true"              { HcsCompilerlval.i = 1; return NUMBER; }
"false"             { HcsCompilerlval.i = 0; return NUMBER; }
"#include"          { LEXER_DEBUG("INCLUDE\n"); return INCLUDE; }
"delete"            { return DELETE; }
"template"          { return TEMPLATE; }

0x[0-9A-Fa-f]+      { HcsCompilerlval.i = strtoll(HcsCompilertext, NULL, 16);
                      LEXER_DEBUG("NUMBER : %u\n", HcsCompilerlval.i);
                      return NUMBER;
                    }
0[0-9]+             { HcsCompilerlval.i = strtoll(HcsCompilertext, NULL, 8);
                      LEXER_DEBUG("NUMBER : %u\n", HcsCompilerlval.i);
                      return NUMBER;
                    }
[-+]?[0-9]+         { HcsCompilerlval.i = atoll(HcsCompilertext);
                      LEXER_DEBUG("NUMBER : %u\n", HcsCompilerlval.i);
                      return NUMBER;
                    }
0b[01]+             {
                      HcsCompilerlval.i = strtoll(HcsCompilertext + 2, NULL, 2);
                      LEXER_DEBUG("NUMBER : %u\n", HcsCompilerlval.i);
                      return NUMBER;
                    }
[a-zA-Z0-9_]+       { HcsCompilerlval.s = strdup(HcsCompilertext);
                      LEXER_DEBUG("STRING : %s \n", HcsCompilertext);
                      return LITERAL;
                    }
[a-zA-Z0-9_\.?]+    { HcsCompilerlval.s = strdup(HcsCompilertext);
                      LEXER_DEBUG("REF_PATH : %s \n", HcsCompilertext);
                      return REF_PATH;
                    }
\"[^\"\n]*\"        { LEXER_DEBUG("CONST_STRING : %s\n", HcsCompilertext);
                      char *constString = strdup(HcsCompilertext + 1);
                      constString[strlen(constString) - 1] = '\0';
                      HcsCompilerlval.s = constString;
                      return STRING;
                    }
\"[^\"\n]*$         { HcsCompilererror("Unterminated string"); }
"\n"
[ \t]               { } /* ignore */
[\r]                { } /* ignore */
.                   { HcsCompilererror("bad input character"); }

%%
extern FILE *HcsCompilerin;
void HcsSetCompilerIn(FILE *in)
{
    HcsCompilerin = in;
}
