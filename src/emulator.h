#ifndef BSBS_EMULATOR_H_
#define BSBS_EMULATOR_H_

#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "hashmap.h"

#define Bsbs_Stmt_Cmd_MaxLen 1024

typedef enum {
	Bsbs_Log_Level_Verbose,
	Bsbs_Log_Level_None,
} Bsbs_Log_Level;

static Bsbs_Log_Level LogLevel = Bsbs_Log_Level_None;

typedef struct Bsbs_Context {
	char *label;
} Bsbs_Context;

static void Bsbs_PreprocessCmd(Bsbs_Hashmap *, Bsbs_Stmt_Cmd *);
static void Bsbs_ProcessDef(char *, char *, size_t);

void Bsbs_Emulator(Bsbs_Stmt *, Bsbs_Context);

static void Bsbs_PreprocessCmd(Bsbs_Hashmap *hashmap, Bsbs_Stmt_Cmd *cmd) {
	char *ppCmd = (char *)malloc(sizeof(char)*Bsbs_Stmt_Cmd_MaxLen);
	strncpy(ppCmd, cmd->cmd, Bsbs_Min(strlen(cmd->cmd), Bsbs_Stmt_Cmd_MaxLen));
	int i;
	while ((i = Bsbs_CharPresent(ppCmd, '$')) != -1) {
		char *tmp = strdup(ppCmd+i+1);
		char *name = Bsbs_FetchUntil(&tmp, ' ');
		if (!Bsbs_Hashmap_Find(hashmap, name))
			ErrorLine((size_t)0, "variable '%s' does not exist", name);
		strcpy(ppCmd+i, Bsbs_Hashmap_Lookup(hashmap, name));
		strcpy(ppCmd+strlen(ppCmd), tmp);
	}
	cmd->cmd = ppCmd;
}

static void Bsbs_ProcessDef(char *key, char *value, size_t lineno) {
	if (strcmp(key, "Log") == 0) {
		if (strcmp(value, "None") == 0) LogLevel = Bsbs_Log_Level_None;
		else if (strcmp(value, "Verbose") == 0) LogLevel = Bsbs_Log_Level_Verbose;
		else ErrorLine(lineno, "invalid value '%s' for config '%s'", value, key);
	}
}

void Bsbs_Emulator(Bsbs_Stmt *stmts, Bsbs_Context ctx) {
	Bsbs_Stmt *stmt = stmts;
	Bsbs_Hashmap hashmap = {0};
	while (stmt) {
		switch (stmt->type) {
		case Bsbs_StmtType_Def:
			if (Bsbs_Hashmap_Find(&hashmap, stmt->def->name))
				ErrorLine(stmt->lineno, "variable '%s' already exists", stmt->def->name);
			Bsbs_Hashmap_Insert(&hashmap, stmt->def->name, stmt->def->value);
			Bsbs_ProcessDef(stmt->def->name, stmt->def->value, stmt->lineno);
			break;
		case Bsbs_StmtType_Cmd:
			Bsbs_PreprocessCmd(&hashmap, stmt->cmd);
			if (LogLevel == Bsbs_Log_Level_Verbose) printf("+ %s\n", stmt->cmd->cmd);
			system(stmt->cmd->cmd);
			break;
		case Bsbs_StmtType_Label:
			if (strcmp(stmt->label->name, ctx.label) != 0) break;
			Bsbs_Emulator(stmt->label->stmts, ctx);
			break;
		default:
			
		}	
		stmt = stmt->next;
	}
}

#endif // BSBS_EMULATOR_H_
