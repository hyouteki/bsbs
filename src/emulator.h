#ifndef BSBS_EMULATOR_H_
#define BSBS_EMULATOR_H_

#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "hashmap.h"

typedef struct Bsbs_Context {
	char *label;
} Bsbs_Context;

void Bsbs_Emulator(Bsbs_Stmt *, Bsbs_Context);

void Bsbs_Emulator(Bsbs_Stmt *stmts, Bsbs_Context ctx) {
	Bsbs_Stmt *stmt = stmts;
	Bsbs_Hashmap hashmap = {0};
	while (stmt) {
		switch (stmt->type) {
		case Bsbs_StmtType_Def:
			if (Bsbs_Hashmap_Find(&hashmap, stmt->def->name))
				ErrorLine(stmt->lineno, "variable '%s' already exists", stmt->def->name);
			Bsbs_Hashmap_Insert(&hashmap, stmt->def->name, stmt->def->value);
			break;
		case Bsbs_StmtType_Label:
			if (strcmp(stmt->label->name, ctx.label) != 0) break;
			Bsbs_Stmt_Cmd *cmd = stmt->label->cmds;
			while (cmd) {
				system(cmd->cmd);
				cmd = cmd->next;
			}
			break;
		}	
		stmt = stmt->next;
	}
}

#endif // BSBS_EMULATOR_H_
