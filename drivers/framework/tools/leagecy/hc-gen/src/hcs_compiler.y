%{
/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdio.h>
#include <stdint.h>
#include "hcs_compiler.h"

#define YYDEBUG             1
#define YYERROR_VERBOSE     1
#define OBJPTR ParserObject*

void HcsCompilererror (const char *s);
int yylex (void);
extern int HcsCompilerlineno;
%}


%union {
    uint64_t        i;
    char            *s;
    ParserObject    *o;
}

%token<i>   NUMBER
%token<i>   TEMPLATE
%token<s>   LITERAL
%token<s>   STRING
%token<s>   REF_PATH
%token<s>   FILE_PATH

%token  EOL
%token  ROOT
%token  INCLUDE
%token  DELETE


%type<o> ConfigTermList ConfigNode ConfigTerm NumberList ConstStringList ConfigBlock NodeTemplate
%type<s> NodePath


%%
ConfigTable
    :
    | IncludeList
    | IncludeList
      RootNode
    | RootNode
    ;

RootNode
    :  ROOT
        '{'                         { $<o>$ = HcsGetParserRoot(); if ($<o>$ == NULL) YYABORT; }
        ConfigTermList
        '}'                         { HcsAstAddChild($<o>3, $4); }
    ;

IncludeList
    : INCLUDE STRING                { if (HcsProcessInclude($2, HcsCompilerlineno)) YYABORT; }
    | INCLUDE STRING IncludeList    { if (HcsProcessInclude($2, HcsCompilerlineno)) YYABORT; }
    ;

ConfigTermList
    :                               { $$ = NULL; }
    | ConfigTerm
        ';'
         ConfigTermList             { $$ = HcsAstAddPeer($1, $3); }
    | ConfigBlock
        ConfigTermList              { $$ = HcsAstAddPeer($1, $2); }
    ;

ConfigBlock
    : ConfigNode
    | NodeTemplate

ConfigNode
    :   LITERAL
        '{'                         { $<o>$ = HcsNewConfigNode($1, CONFIG_NODE_NOREF, NULL); if ($<o>$ == NULL) YYABORT; }
         ConfigTermList
        '}'                         { $$ = HcsAstAddChild($<o>3, $4); }
    | LITERAL ':' NodePath  '{'     { $<o>$ = HcsNewConfigNode($1, CONFIG_NODE_COPY, $3); if ($<o>$ == NULL) YYABORT; }
        ConfigTermList
        '}'                         { $$ = HcsAstAddChild($<o>5, $6); }
    | LITERAL ':' '&' NodePath '{'  { $<o>$ =  HcsNewConfigNode($1, CONFIG_NODE_REF, $4); if ($<o>$ == NULL) YYABORT; }
         ConfigTermList
        '}'                         { $$ = HcsAstAddChild($<o>6, $7); }
    | LITERAL ':' DELETE '{'        { $<o>$ =  HcsNewConfigNode($1, CONFIG_NODE_DELETE, NULL); if ($<o>$ == NULL) YYABORT; }
         ConfigTermList
        '}'                         { $$ = HcsAstAddChild($<o>5, $6); }
    | LITERAL ':' ':'
      NodePath
      '{'                           { $<o>$ = HcsNewConfigNode($1, CONFIG_NODE_INHERIT, $4); if ($<o>$ == NULL) YYABORT; }
      ConfigTermList
      '}'                           { $$ = HcsAstAddChild($<o>6, $7); }
    ;

NodeTemplate : TEMPLATE LITERAL
                '{'          { $<o>$ = HcsNewConfigNode($2, CONFIG_NODE_TEMPLATE, NULL); if ($<o>$ == NULL) YYABORT; }
                ConfigTermList
                '}'          { $$ = HcsAstAddChild($<o>4, $5); }

NodePath
    : REF_PATH                              { $$ = $1; }
    | LITERAL                               { $$ = $1; }
    ;

ConfigTerm
    : LITERAL '=' STRING                    { $$ = HcsNewConfigTerm($1, HcsNewParserObject(NULL, PARSEROP_STRING, (uint64_t)$3));
                                              if ($$ == NULL) YYABORT; }
    | LITERAL '=' NUMBER                    { $$ = HcsNewConfigTerm($1, HcsNewParserObject(NULL, PARSEROP_UINT64, (uint64_t)$3));
                                              if ($$ == NULL) YYABORT; }
    | LITERAL '=' '[' ']'                   { HcsCompilererror("Not allow empty array"); YYABORT;}
    | LITERAL '=' '[' NumberList ']'        { OBJPTR obj = HcsNewParserObject(NULL, PARSEROP_ARRAY, (uint64_t)NULL);
                                              if (obj == NULL) YYABORT;
                                              $$ =  HcsNewConfigTerm($1, HcsAstAddChild(obj, $4));
                                              if ($$ == NULL) YYABORT; }
    | LITERAL '=' '[' ConstStringList ']'   { OBJPTR obj = HcsNewParserObject(NULL, PARSEROP_ARRAY, (uint64_t)NULL);
                                              if (obj == NULL) YYABORT;
                                              $$ = HcsNewConfigTerm($1, HcsAstAddChild(obj, $4));
                                              if ($$ == NULL) YYABORT; }
    | LITERAL '=' '&' NodePath              { OBJPTR obj = HcsNewParserObject(NULL, PARSEROP_NODEREF, (uint64_t)$4);
                                              if (obj == NULL) YYABORT;
                                              $$ = HcsNewConfigTerm($1, obj);
                                              if ($$ == NULL) YYABORT; }
    | LITERAL '=' DELETE                    { $$ = HcsNewConfigTerm($1,
                                                HcsNewParserObject(NULL, PARSEROP_DELETE, (uint64_t)NULL)); }
    ;

NumberList
    : NUMBER                                { $$ = HcsNewParserObject(NULL, PARSEROP_UINT64, $1); if ($$ == NULL) YYABORT; }
    | NUMBER ',' NumberList                 { OBJPTR obj = HcsNewParserObject(NULL, PARSEROP_UINT64, $1); if (obj == NULL) YYABORT;
                                              $$ = HcsAstAddPeer(obj, $3); }
    ;

ConstStringList
    : STRING                          { $$ = HcsNewParserObject(NULL, PARSEROP_STRING, (uint64_t)$1); if ($$ == NULL) YYABORT;}
    | STRING ',' ConstStringList      { OBJPTR obj = HcsNewParserObject(NULL, PARSEROP_STRING, (uint64_t)$1); if (obj == NULL) YYABORT;
                                        $$ = HcsAstAddPeer(obj, $3); }
    ;

%%
extern char *HcsCompilertext;
extern int HcsCompilerlineno;

const char*HcsGetCurrentSourceName();

int32_t HcsGetCurrentSourceLine(void) {
    return HcsCompilerlineno;
}

void HcsSetCurrentSourceLine(int32_t lineNumber) {
    HcsCompilerlineno = lineNumber;
}

void HcsCompilererror (const char *s) {
    fprintf(stderr, "Error: %s:%d\n\t%s\n", HcsGetCurrentSourceName(), HcsCompilerlineno, s);
}

void HcsCompilerrestart(FILE *input_file);

void HcsParserRestart(FILE *inputFile) {
    HcsCompilerrestart(inputFile);
}
