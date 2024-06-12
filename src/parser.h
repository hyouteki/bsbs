#ifndef BSBS_PARSER_H_
#define BSBS_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include "helper.h"

#define Line_Capacity 1024

typedef struct Bsbs_Stmt_Def {
	char *name;
	char *value;
} Bsbs_Stmt_Def;

typedef struct Bsbs_Stmt_Cmd {
	char *cmd;
	unsigned int lineno;
	struct Bsbs_Stmt_Cmd *next;
} Bsbs_Stmt_Cmd;

typedef struct Bsbs_Stmt_Label {
	char *name;
	Bsbs_Stmt_Cmd *cmds;
	unsigned int lineno;
} Bsbs_Stmt_Label;

typedef enum {
	Bsbs_StmtType_Def,
	Bsbs_StmtType_Label
} Bsbs_StmtType;

typedef struct Bsbs_Stmt {
	Bsbs_StmtType type;
	union {
		Bsbs_Stmt_Def *def;
		Bsbs_Stmt_Label *label;
	};
	struct Bsbs_Stmt *next;
	unsigned int lineno;
} Bsbs_Stmt;

#define Bsbs_StmtType_Default Bsbs_StmtType_Def

static char *Bsbs_StmtType_ToString(Bsbs_StmtType);
static void Bsbs_Stmt_Cmd_Add(Bsbs_Stmt_Cmd **, char *, unsigned int);
static void Bsbs_Stmt_Add(Bsbs_Stmt **, Bsbs_Stmt *);

void Bsbs_ParseFile(char *, Bsbs_Stmt **);
void Bsbs_Stmt_Print(Bsbs_Stmt *);

static char *Bsbs_StmtType_ToString(Bsbs_StmtType type) {
	switch (type) {
	case Bsbs_StmtType_Def:
		return "Bsbs_StmtType_Def";
	case Bsbs_StmtType_Label:
		return "Bsbs_StmtType_Label";
	default:
		Error("invalid statement type value '%d' in function '%s'\n", type, __func__);
	}
}

static void Bsbs_Stmt_Cmd_Add(Bsbs_Stmt_Cmd **cmds, char *text, unsigned int lineno) {
	Bsbs_Stmt_Cmd *cmd = (Bsbs_Stmt_Cmd *)malloc(sizeof(Bsbs_Stmt_Cmd));
	cmd->cmd = strdup(text);
	cmd->lineno = lineno;
	cmd->next = NULL;
	if (!*cmds) {
		*cmds = cmd;
		return;
	}
	Bsbs_Stmt_Cmd *itr = *cmds;
	while (itr->next) itr = itr->next;
	itr->next = cmd;
}

static void Bsbs_Stmt_Add(Bsbs_Stmt **stmts, Bsbs_Stmt *stmt) {
	if (!*stmts) {
		*stmts = stmt;
		return;
	}
	Bsbs_Stmt *itr = *stmts;
	while (itr->next) itr = itr->next;
	itr->next = stmt;
}

void Bsbs_ParseFile(char *filename, Bsbs_Stmt **stmts) {
    FILE *file = fopen(filename, "r");
    if (!file) Error("could not open file `%s`", filename);
	char *line = (char *)malloc(sizeof(char)*Line_Capacity);
	*stmts = NULL;
	Bsbs_StmtType type = Bsbs_StmtType_Default;
	unsigned int lineno = 0;
	Bsbs_Stmt_Label *label = NULL;
    while (fgets(line, Line_Capacity, file)) {
		lineno++;
		char *ptr = line;
		Bsbs_Trim(&ptr);
		if (strlen(ptr) == 0) continue;
		if (Bsbs_StartsWith(ptr, "label")) {
			ptr += strlen("label");
			Bsbs_Trim(&ptr);
			char *name = Bsbs_FetchUntil(&ptr, ' ');
			Bsbs_Trim(&ptr);
			if (!Bsbs_StartsWith(ptr, "in"))
				ErrorLine(lineno, "expected keyword 'in' after label name '%s'", name);
			ptr += strlen("in");
			Bsbs_Trim(&ptr);
			if (strlen(ptr)) ErrorLine(lineno, "invalid token '%s' after keyword 'in'", ptr);
			type = Bsbs_StmtType_Label;
			label = (Bsbs_Stmt_Label *)malloc(sizeof(Bsbs_Stmt_Label));
			label->name = name;
			label->lineno = lineno;
			label->cmds = NULL;
			continue;
		} else if (Bsbs_StartsWith(ptr, "end")) {
			ptr += strlen("end");
			Bsbs_Trim(&ptr);
			if (strlen(ptr)) ErrorLine(lineno, "invalid token '%s' after keyword 'end'", ptr);
			if (type != Bsbs_StmtType_Label) {
				ErrorLine(lineno, "expected statement type '%s' got '%s'",
						  Bsbs_StmtType_ToString(Bsbs_StmtType_Label),
						  Bsbs_StmtType_ToString(type));
			}
			Bsbs_Stmt *stmt = (Bsbs_Stmt *)malloc(sizeof(Bsbs_Stmt));
			stmt->label = (Bsbs_Stmt_Label *)malloc(sizeof(Bsbs_Stmt_Label));
			stmt->type = Bsbs_StmtType_Label;
			memcpy(stmt->label, label, sizeof(Bsbs_Stmt_Label));
			stmt->next = NULL;
			stmt->lineno = lineno;
			Bsbs_Stmt_Add(stmts, stmt);
			label = NULL;
			type = Bsbs_StmtType_Default;
			continue;
		}
		switch (type) {
		case Bsbs_StmtType_Def:
			Bsbs_Stmt *stmt = (Bsbs_Stmt *)malloc(sizeof(Bsbs_Stmt));
			stmt->type = type;
			stmt->def = (Bsbs_Stmt_Def *)malloc(sizeof(Bsbs_Stmt_Def));
			stmt->def->name = Bsbs_FetchUntil(&ptr, '=');
			Bsbs_Trim(&stmt->def->name);
			Bsbs_RemoveSpecial(&ptr);
			Bsbs_Trim(&ptr);
			stmt->def->value = strdup(ptr);
			stmt->lineno = lineno;
			Bsbs_Stmt_Add(stmts, stmt);
			break;
		case Bsbs_StmtType_Label:
			Bsbs_Stmt_Cmd_Add(&label->cmds, strdup(ptr), lineno);
			break;
		}
	}
}

void Bsbs_Stmt_Print(Bsbs_Stmt *stmts) {
	Bsbs_Stmt *stmt = stmts;
	while (stmt) {
		switch (stmt->type) {
		case Bsbs_StmtType_Def:
			printf("%d\t%s = %s\n", stmt->lineno, stmt->def->name, stmt->def->value);
			break;
		case Bsbs_StmtType_Label:
			printf("%d\tlabel %s in\n", stmt->label->lineno, stmt->label->name);
			Bsbs_Stmt_Cmd *cmd = stmt->label->cmds;
			while (cmd) {
				printf("%d\t\t%s\n", cmd->lineno, cmd->cmd);
				cmd = cmd->next;
			}
			printf("%d\tend\n", stmt->lineno);
			break;
		}
		stmt = stmt->next;
	}
}

#endif // BSBS_PARSER_H_
