#ifndef BSBS_EMULATOR_H_
#define BSBS_EMULATOR_H_

#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "hashmap.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/wait.h>
#else
#error "error: unsupported OS; supported OS (Windows and GNU/Linux)"
#endif

#define Bsbs_Stmt_Cmd_MaxLen 1024

typedef enum {
	Bsbs_Log_Level_Verbose,
	Bsbs_Log_Level_None,
} Bsbs_Log_Level;

typedef enum {
	Bsbs_On_Error_Exit,
	Bsbs_On_Error_Continue,
} Bsbs_On_Error;

static Bsbs_Log_Level LogLevel = Bsbs_Log_Level_None;
static Bsbs_On_Error OnError = Bsbs_On_Error_Exit;

typedef struct Bsbs_Context {
	char *label;
	Bsbs_Hashmap hashmap;
} Bsbs_Context;

static void Bsbs_ProcessKnownDef(Bsbs_Stmt *);
static void Bsbs_ProcessLet(Bsbs_Hashmap *, Bsbs_Stmt *);
static void Bsbs_PreprocessCmd(Bsbs_Hashmap *, Bsbs_Stmt *);
static void Bsbs_ProcessCmd(Bsbs_Hashmap *, Bsbs_Stmt *);

void Bsbs_Emulator(Bsbs_Stmt *, Bsbs_Context);

static void Bsbs_ProcessKnownDef(Bsbs_Stmt *stmt) {
	char *key = stmt->let->name, *val = stmt->let->value;
	if (strcmp(key, "Log") == 0) {
		if (strcmp(val, "None") == 0) LogLevel = Bsbs_Log_Level_None;
		else if (strcmp(val, "Verbose") == 0) LogLevel = Bsbs_Log_Level_Verbose;
		else Bsbs_Stmt_ErrorFmt(stmt, "invalid value '%s' for config '%s'", val, key);
	}
	if (strcmp(key, "OnError") == 0) {
		if (strcmp(val, "Continue") == 0) OnError = Bsbs_On_Error_Continue;
		else if (strcmp(val, "Exit") == 0) OnError = Bsbs_On_Error_Exit;
		else Bsbs_Stmt_ErrorFmt(stmt, "invalid value '%s' for config '%s'", val, key);
	}
}

static void Bsbs_ProcessLet(Bsbs_Hashmap *hashmap, Bsbs_Stmt *stmt) {
	if (Bsbs_Hashmap_Find(hashmap, stmt->let->name))
		Bsbs_Stmt_ErrorFmt(stmt, "variable '%s' already defined", stmt->let->name);
	Bsbs_Hashmap_Insert(hashmap, stmt->let->name, stmt->let->value);
	if (LogLevel == Bsbs_Log_Level_Verbose) printf("+ %s = %s\n", stmt->let->name,
												   stmt->let->value);
	Bsbs_ProcessKnownDef(stmt);
}

static void Bsbs_PreprocessCmd(Bsbs_Hashmap *hashmap, Bsbs_Stmt *stmt) {
	char *ppCmd = (char *)malloc(sizeof(char)*Bsbs_Stmt_Cmd_MaxLen);
	strncpy(ppCmd, stmt->cmd->cmd, Bsbs_Min(strlen(stmt->cmd->cmd), Bsbs_Stmt_Cmd_MaxLen));
	int i;
	while ((i = Bsbs_CharPresent(ppCmd, '$')) != -1) {
		char *tmp = strdup(ppCmd+i+1);
		char *name = Bsbs_FetchUntil(&tmp, ' ');
		if (!Bsbs_Hashmap_Find(hashmap, name))
			Bsbs_Stmt_ErrorFmt(stmt, "variable '%s' not defined", name);
		strcpy(ppCmd+i, Bsbs_Hashmap_Lookup(hashmap, name));
		strcpy(ppCmd+strlen(ppCmd), tmp);
	}
	stmt->cmd->cmd = ppCmd;
}

static void Bsbs_ProcessCmd(Bsbs_Hashmap *hashmap, Bsbs_Stmt *stmt) {
    Bsbs_PreprocessCmd(hashmap, stmt);   
    if (LogLevel == Bsbs_Log_Level_Verbose) printf("+ %s\n", stmt->cmd->cmd);    
    int exitcode = system(stmt->cmd->cmd);
    if (OnError == Bsbs_On_Error_Continue) return;
    if (exitcode == -1)
		Bsbs_Stmt_ErrorFmt(stmt, "system failed to execute command '%s'", stmt->cmd->cmd);
#ifdef _WIN32
    if (exitcode != 0)
		Bsbs_Stmt_ErrorFmtExitCode(stmt, exitcode, "command '%s' failed with exitcode '%d'",
								   stmt->cmd->cmd, exitcode);
#else
	if (WIFEXITED(exitcode) && WEXITSTATUS(exitcode) != 0) 
		Bsbs_Stmt_ErrorFmtExitCode(stmt, exitcode, "command '%s' failed with exitcode '%d'",
								   stmt->cmd->cmd, exitcode);
#endif
}

void Bsbs_Emulator(Bsbs_Stmt *stmts, Bsbs_Context ctx) {
	Bsbs_Stmt *stmt = stmts;
	while (stmt) {
		switch (stmt->type) {
		case Bsbs_StmtType_Let:
			Bsbs_ProcessLet(&ctx.hashmap, stmt);
			break;
		case Bsbs_StmtType_Cmd:
			Bsbs_ProcessCmd(&ctx.hashmap, stmt);
			break;
		case Bsbs_StmtType_Label:
			if (strcmp(stmt->label->name, ctx.label) != 0) break;
			Bsbs_Emulator(stmt->label->stmts, ctx);
			break;
		default:
			break;
		}	
		stmt = stmt->next;
	}
}

#endif // BSBS_EMULATOR_H_
