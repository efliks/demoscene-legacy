

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _console_h
#define _console_h


#define CON_COLOR 24
#define CON_ADDCOL 1

#define CUR_TIMER 10

#define KEY_ENTER 13
#define KEY_BACKSPACE 8
#define KEY_ESCAPE 27
#define KEY_CONSOLE '`'

#define COMM_LEN 10
#define ARG_LEN 13

#define MAX_COMMANDS 13

#define BUILD_STR "BUILD: " ## __DATE__ ## " " ## __TIME__

#define MAX_CFGLINES 64

#define MAX_VARS 20


extern char *ConsoleData;
extern int ConsoleLine, ConsoleChar, ConColor,
	ConAddCol, LogFlag, ConImgFlag;
extern BYTE *ConsoleImg;

extern char GlobalArg1[ARG_LEN], GlobalArg2[ARG_LEN],
	GlobalArg3[ARG_LEN];

//----------------------------------------------------------------------

typedef struct tCOMMAND {
	char Command[COMM_LEN];
	int NumArgs;
	int (*f)(void);
} COMMAND;

typedef struct tVAR {
	char VarName[ARG_LEN];
	int *PtrToVar;
	int Min, Max;
	void (*Proc)(void);
} VAR;

//----------------------------------------------------------------------

int SetConsole(void);
void FreeConsole(void);
int LoadConsoleImg(char *ImgFilename);
void FreeConsoleImg(void);
void ClearConsole(void);
void AddConsoleLine(char *str);
void PrintConsole(void);
void PutCursor(void);
int Console(void);
int ParseCommand(char *line);
int LoadCfg(void);
void StartStopLog(void);
void LogLine(char *line);
int IsInt(char *str);

int cListCommands(void);
int cListVars(void);
int cIntroMsg(void);
int cQuitGame(void);
int cClearConsole(void);
int cBuildTime(void);
int cLoadMap(void);
int cMapInfo(void);
int cFreeMap(void);
int cCenterMap(void);
int cDump(void);
int cSetVar(void);
int cGetVar(void);

//----------------------------------------------------------------------

#define Print(a) { AddConsoleLine(a); if (LogFlag) LogLine(a); }
#define Show() { PrintConsole(); Wait(2); CopyFrameBuffer(); }

#endif
