/*
	Copyright (C) 2008-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __CommandLineUtils_h__
#define __CommandLineUtils_h__

#include "CommonServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

extern int gArgC;
extern const char** gArgV;
extern int gArgI;
extern const char* gProgramPath;
extern const char* gProgramName;
extern const char* gProgramLongName;
extern volatile int gExitCode;

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define kCLIInternalErr 128
#define kCLIArgErr 129
#define kCLINoMemoryErr 130
#define kCLIUserCanceledErr 131
#define kCLIUnknownErr 132

//===========================================================================================================================
//	CLIOptions
//===========================================================================================================================

// CLIFlags

typedef uint32_t CLIFlags;
#define kCLIFlags_None 0
#define kCLIFlags_ReorderArgs (1 << 0) //! Reorder argv so options come before non-options.
#define kCLIFlags_DontProcessCommands (1 << 1) //! Don't process commands.

// CLIOptionType

typedef enum {
    kCLIOptionType_End,
    kCLIOptionType_Section,
    kCLIOptionType_Group,
    kCLIOptionType_Command,
    kCLIOptionType_Argument,

    kCLIOptionType_OptionStart,
    kCLIOptionType_Boolean,
    kCLIOptionType_String,
    kCLIOptionType_CFString,
    kCLIOptionType_MultiString,
    kCLIOptionType_Integer,
    kCLIOptionType_CallBack,
    kCLIOptionType_OptionEnd,

    kCLIOptionType_Any

} CLIOptionType;

#define kCLIOptionType_AnyOption kCLIOptionType_OptionStart

#define CLIOption_IsOption(OPTION) \
    (((OPTION)->type > kCLIOptionType_OptionStart) && ((OPTION)->type < kCLIOptionType_OptionEnd))

// CLIOptionFlags

typedef uint32_t CLIOptionFlags;

#define kCLIOptionFlags_None 0
#define kCLIOptionFlags_NoArgument (1 << 0) // Option doesn't allow an argument.
#define kCLIOptionFlags_OptionalArgument (1 << 1) // Option may have an argument, but isn't required to have one.
#define kCLIOptionFlags_NoNegate (1 << 2) // Don't allow option to be negated (no --no-<xyz>).
#define kCLIOptionFlags_LiteralArgHelp (1 << 3) // Use argHelp string directly (not wrapped with <>).
#define kCLIOptionFlags_Required (1 << 4) // Fail if this option is not specified by the user.
#define kCLIOptionFlags_Specified (1 << 5) // Internal: Set if this option has been specified by the user.
#define kCLIOptionFlags_GlobalOnly (1 << 6) // Option is only useful at global level (i.e. don't show for commands).
#define kCLIOptionFlags_MetaCommand (1 << 7) // Command name may be in argv[0] (e.g. named links to the same binary).
#define kCLIOptionFlags_NotCommon (1 << 8) // Not commonly used command. Only shown in full help and not in the summary.

// CLIOption

typedef struct CLIOption CLIOption;

typedef OSStatus (*CLIOptionCallBack)(CLIOption* inOption, const char* inArg, int inUnset);
typedef void (*CLICommandCallBack)(void);

struct CLIOption {
    CLIOptionType type;
    int shortName;
    const char* longName;
    void* valuePtr;
    size_t* valueCountPtr;
    intptr_t defaultValue;
    const char* argHelp;
    CLIOptionFlags flags;
    CLIOptionCallBack optionCallBack;
    CLICommandCallBack commandCallBack;
    CLIOption* subOptions;
    CLIOption* parentOption;
    const char* shortHelp;
    const char* longHelp;
};

// Option Macros

#define CLI_OPTION_BOOLEAN(SHORT_CHAR, LONG_NAME, VALUE_PTR, SHORT_HELP, LONG_HELP)       \
    {                                                                                     \
        kCLIOptionType_Boolean, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, NULL,    \
            kCLIOptionFlags_NoArgument, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP) \
    }

#define CLI_OPTION_GROUP(SHORT_HELP)                                                                         \
    {                                                                                                        \
        kCLIOptionType_Group, '\0', NULL, NULL, NULL, 0, NULL, kCLIOptionFlags_None, NULL, NULL, NULL, NULL, \
            (SHORT_HELP), NULL                                                                               \
    }

#define CLI_OPTION_INTEGER(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, SHORT_HELP, LONG_HELP)                             \
    {                                                                                                                     \
        kCLIOptionType_Integer, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, ((ARG_HELP) != NULL) ? (ARG_HELP) : "n", \
            kCLIOptionFlags_None, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                       \
    }

#define CLI_OPTION_INTEGER_EX(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, SHORT_HELP, FLAGS, LONG_HELP)                   \
    {                                                                                                                     \
        kCLIOptionType_Integer, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, ((ARG_HELP) != NULL) ? (ARG_HELP) : "n", \
            (FLAGS), NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                                    \
    }

#define CLI_OPTION_STRING(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, SHORT_HELP, LONG_HELP)       \
    {                                                                                              \
        kCLIOptionType_String, (SHORT_CHAR), (LONG_NAME), (void*)(VALUE_PTR), NULL, 0, (ARG_HELP), \
            kCLIOptionFlags_None, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                \
    }

#define CLI_OPTION_STRING_EX(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, SHORT_HELP, FLAGS, LONG_HELP) \
    {                                                                                                  \
        kCLIOptionType_String, (SHORT_CHAR), (LONG_NAME), (void*)(VALUE_PTR), NULL, 0, (ARG_HELP),     \
            (FLAGS), NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                 \
    }

#if (CF_ENABLED)
#define CLI_OPTION_CFSTRING(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, FLAGS, SHORT_HELP, LONG_HELP) \
    {                                                                                                 \
        kCLIOptionType_CFString, (SHORT_CHAR), (LONG_NAME), (void*)(VALUE_PTR), NULL, 0, (ARG_HELP),  \
            (FLAGS), NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                \
    }

#define CLI_OPTION_CFSTRING_EX(SHORT_CHAR, LONG_NAME, VALUE_PTR, ARG_HELP, SHORT_HELP, FLAGS, LONG_HELP) \
    {                                                                                                    \
        kCLIOptionType_CFString, (SHORT_CHAR), (LONG_NAME), (void*)(VALUE_PTR), NULL, 0, (ARG_HELP),     \
            (FLAGS), NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                   \
    }
#endif

#define CLI_OPTION_MULTI_STRING(SHORT_CHAR, LONG_NAME, VALUE_PTR, VALUE_COUNT_PTR, ARG_HELP, SHORT_HELP, LONG_HELP) \
    {                                                                                                               \
        kCLIOptionType_MultiString, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), (VALUE_COUNT_PTR), 0, (ARG_HELP),       \
            kCLIOptionFlags_None, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                 \
    }

#define CLI_OPTION_MULTI_STRING_EX(SHORT_CHAR, LONG_NAME, VALUE_PTR, VALUE_COUNT_PTR, ARG_HELP, SHORT_HELP, FLAGS, LONG_HELP) \
    {                                                                                                                         \
        kCLIOptionType_MultiString, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), (VALUE_COUNT_PTR), 0, (ARG_HELP),                 \
            (FLAGS), NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                                        \
    }

#define CLI_OPTION_CALLBACK(SHORT_CHAR, LONG_NAME, CALLACK, VALUE_PTR, ARG_HELP, SHORT_HELP, LONG_HELP)                   \
    {                                                                                                                     \
        kCLIOptionType_CallBack, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, (ARG_HELP),                             \
            kCLIOptionFlags_NoArgument | kCLIOptionFlags_NoNegate, (CALLACK), NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP) \
    }

#define CLI_OPTION_CALLBACK_EX(SHORT_CHAR, LONG_NAME, CALLACK, VALUE_PTR, ARG_HELP, FLAGS, SHORT_HELP, LONG_HELP) \
    {                                                                                                             \
        kCLIOptionType_CallBack, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, (ARG_HELP),                     \
            (FLAGS), (CALLACK), NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                       \
    }

#define CLI_OPTION_CALLBACK_EX2(SHORT_CHAR, LONG_NAME, CALLACK, VALUE_PTR, ARG_HELP, SHORT_HELP, FLAGS, LONG_HELP) \
    {                                                                                                              \
        kCLIOptionType_CallBack, (SHORT_CHAR), (LONG_NAME), (VALUE_PTR), NULL, 0, (ARG_HELP),                      \
            (FLAGS), (CALLACK), NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP)                                        \
    }

// Command macros

#define CLI_META_COMMAND(NAME, CALLBACK, SUB_OPTIONS, SHORT_HELP, LONG_HELP) \
    CLI_COMMAND_EX((NAME), (CALLBACK), (SUB_OPTIONS), kCLIOptionFlags_MetaCommand, (SHORT_HELP), (LONG_HELP))

#define CLI_COMMAND(NAME, CALLBACK, SUB_OPTIONS, SHORT_HELP, LONG_HELP) \
    CLI_COMMAND_EX((NAME), (CALLBACK), (SUB_OPTIONS), kCLIOptionFlags_None, (SHORT_HELP), (LONG_HELP))

#define CLI_COMMAND_EX(NAME, CALLBACK, SUB_OPTIONS, FLAGS, SHORT_HELP, LONG_HELP)     \
    {                                                                                 \
        kCLIOptionType_Command, '\0', (NAME), NULL, NULL, 0, NULL,                    \
            (FLAGS), NULL, (CALLBACK), (SUB_OPTIONS), NULL, (SHORT_HELP), (LONG_HELP) \
    }

#define CLI_COMMAND_EX2(NAME, CALLBACK, SUB_OPTIONS, SHORT_HELP, FLAGS, LONG_HELP)    \
    {                                                                                 \
        kCLIOptionType_Command, '\0', (NAME), NULL, NULL, 0, NULL,                    \
            (FLAGS), NULL, (CALLBACK), (SUB_OPTIONS), NULL, (SHORT_HELP), (LONG_HELP) \
    }

#define CLI_ARGUMENT(NAME, SHORT_HELP, LONG_HELP)                                   \
    {                                                                               \
        kCLIOptionType_Argument, '\0', (NAME), NULL, NULL, 0, NULL,                 \
            kCLIOptionFlags_None, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP) \
    }

#define CLI_OPTIONAL_ARGUMENT(NAME, SHORT_HELP, LONG_HELP)                                      \
    {                                                                                           \
        kCLIOptionType_Argument, '\0', (NAME), NULL, NULL, 0, NULL,                             \
            kCLIOptionFlags_OptionalArgument, NULL, NULL, NULL, NULL, (SHORT_HELP), (LONG_HELP) \
    }

// Misc macros

#define CLI_SECTION(SHORT_HELP, LONG_HELP)                                                                     \
    {                                                                                                          \
        kCLIOptionType_Section, '\0', NULL, NULL, NULL, 0, NULL, kCLIOptionFlags_None, NULL, NULL, NULL, NULL, \
            (SHORT_HELP), (LONG_HELP)                                                                          \
    }

#define CLI_OPTION_END()                                                                                              \
    {                                                                                                                 \
        kCLIOptionType_End, '\0', NULL, NULL, NULL, 0, NULL, kCLIOptionFlags_None, NULL, NULL, NULL, NULL, NULL, NULL \
    }

// Built-in help

#define CLI_OPTION_HELP()                                                                         \
    CLI_OPTION_CALLBACK_EX('h', "help", _CLIHelpOption, NULL, "command",                          \
        kCLIOptionFlags_OptionalArgument | kCLIOptionFlags_NoNegate | kCLIOptionFlags_GlobalOnly, \
        "Displays help for this tool or a specific command.", NULL)
OSStatus _CLIHelpOption(CLIOption* inOption, const char* inArg, int inUnset);

#define CLI_COMMAND_HELP() \
    CLI_COMMAND("help", _CLIHelpCommand, NULL, "Displays help for this tool or a specific command.", NULL)
void CLIHelpCommand(const char* inCmd);
void _CLIHelpCommand(void);

// Built-in interactive mode

#if (!defined(CLI_HAS_INTERACTIVE))
#if (TARGET_OS_DARWIN)
#define CLI_HAS_INTERACTIVE 1
#else
#define CLI_HAS_INTERACTIVE 0
#endif
#endif
#if (CLI_HAS_INTERACTIVE)
#define CLI_OPTION_INTERACTIVE()                                                            \
    CLI_OPTION_CALLBACK_EX('i', "interactive", _CLIInteractiveOption, NULL, NULL,           \
        kCLIOptionFlags_NoArgument | kCLIOptionFlags_NoNegate | kCLIOptionFlags_GlobalOnly, \
        "Starts interactive mode.", NULL)
OSStatus _CLIInteractiveOption(CLIOption* inOption, const char* inArg, int inUnset);

extern Boolean gCLIInteractiveMode;
#endif

// Built-in version

#define CLI_OPTION_VERSION(MARKETING_VERSION, SOURCE_VERSION)                                                   \
    {                                                                                                           \
        kCLIOptionType_CallBack, 'V', "version", (void*)(MARKETING_VERSION), (size_t*)(SOURCE_VERSION), 0,      \
            NULL, kCLIOptionFlags_NoArgument | kCLIOptionFlags_GlobalOnly, _CLIVersionOption, NULL, NULL, NULL, \
            "Displays the version of this tool.", NULL                                                          \
    }
OSStatus _CLIVersionOption(CLIOption* inOption, const char* inArg, int inUnset);

#define CLI_COMMAND_VERSION(MARKETING_VERSION, SOURCE_VERSION)                                             \
    {                                                                                                      \
        kCLIOptionType_Command, '\0', "version", (void*)(MARKETING_VERSION), (size_t*)(SOURCE_VERSION), 0, \
            NULL, kCLIOptionFlags_GlobalOnly, NULL, _CLIVersionCommand, NULL, NULL,                        \
            "Displays the version of this tool.", NULL                                                     \
    }
void _CLIVersionCommand(void);

// Prototypes

void CLIInit(int inArgC, const void* inArgV);
void CLIFree(void);
OSStatus CLIParse(CLIOption inOptions[], CLIFlags inFlags);
#define CLINextArgOrEmpty() ((gArgI < gArgC) ? gArgV[gArgI++] : "")
#define CLINextArgOrTRACE(...) ((gArgI < gArgC) ? gArgV[gArgI++] : (TRACE(1, __VA_ARGS__), ""))
#define CLINextArgOrNULL() ((gArgI < gArgC) ? gArgV[gArgI++] : NULL)
#define CLINextArgOrString(STR) ((gArgI < gArgC) ? gArgV[gArgI++] : (STR))

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CLIArgToValueOrTRACE
	@abstract	Converts the next arg(s) to a value based on a variable list of mapping parameters.
	@example
	
	int		x;
	
	x = CLIArgToValueOrTRACE( "action", 
		"take",		1, 
		"borrow",	2, 
		NULL );
	
	OR
	
	x = CLIArgToValueOrTRACE( "enabled", 
		kCLIArg_AnyTrueish, 
		kCLIArg_AnyFalseish, 
		NULL );
*/
#define kCLIArg_AnyTrueish "<any-true-ish>"
#define kCLIArg_AnyFalseish "<any-false-ish>"
#define kCLIArg_AnyInt "<any-int>"
int CLIArgToValueOrTRACE(const char* inLabel, ...);
void ErrQuit(int inExitCode, const char* inFormat, ...) ATTRIBUTE_NORETURN;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		CLI Progress Functions
	@abstract	Print or clear progress on a single line.
*/
void CLIProgressClear(void);
void CLIProgressUpdate(const char* inFormat, ...);

#ifdef __cplusplus
}
#endif

#endif // __CommandLineUtils_h__
