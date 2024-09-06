#ifndef BSBS_PARSER_H_
#define BSBS_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "token.h"
#include "string.h"
#include "helper.h"

#define Line_Capacity 1024

#ifdef _WIN32
#define LINE_FORMAT_SPEC "%u"
#elif __linux__
#define LINE_FORMAT_SPEC "%zu"
#else
#error "error: unsupported OS; supported OS (Windows and GNU/Linux)"
#endif

typedef struct Bsbs_Stmt Bsbs_Stmt;

typedef struct Bsbs_Stmt_Let {
	char *name;
	char *value;
} Bsbs_Stmt_Let;

typedef struct Bsbs_Stmt_Cmd {
	char *cmd;
} Bsbs_Stmt_Cmd;

typedef struct Bsbs_Stmt_Label {
	char *name;
	size_t endLineno;
	Bsbs_Stmt *stmts;
} Bsbs_Stmt_Label;

typedef struct Bsbs_Stmt_Section {
	char *name;
	size_t endLineno;
	Bsbs_Stmt *stmts;
} Bsbs_Stmt_Section;

typedef enum {
	Bsbs_StmtType_Let,
	Bsbs_StmtType_Cmd,
	Bsbs_StmtType_Label,
	Bsbs_StmtType_Section
} Bsbs_StmtType;

typedef struct Bsbs_Stmt {
	Bsbs_StmtType type;
	union {
		Bsbs_Stmt_Let *let;
		Bsbs_Stmt_Cmd *cmd;
		Bsbs_Stmt_Label *label;
		Bsbs_Stmt_Section *section;
	};
	struct Bsbs_Stmt *next;
	size_t lineno;
	const char *filepath;
} Bsbs_Stmt;

static char *Bsbs_StmtType_ToString(Bsbs_StmtType);
static void Bsbs_Stmt_Add(Bsbs_Stmt **, Bsbs_Stmt *);
static void PrintIndent(size_t, size_t);
static void Bsbs_Stmt_Print_(Bsbs_Stmt *, size_t);

void Bsbs_ParseFile(char *, Bsbs_Stmt **);
void Bsbs_Stmt_Print(Bsbs_Stmt *);

#define Bsbs_Stmt_Error(stmt, msg) ({printf("%s:%d:0: function: %s\n" \
				"%s:"LINE_FORMAT_SPEC":0: error: %s\n", __FILE__, __LINE__, __FUNCTION__, \
				stmt->filepath, stmt->lineno, msg); exit(EXIT_FAILURE);})

#define Bsbs_Stmt_ErrorFmt(stmt, msg, ...) ({printf("%s:%d:0: function: %s\n" \
				"%s:"LINE_FORMAT_SPEC":0: error: "msg"\n", __FILE__, __LINE__, __FUNCTION__, \
				stmt->filepath, stmt->lineno, __VA_ARGS__); exit(EXIT_FAILURE);})

#define Bsbs_Stmt_ErrorFmtExitCode(stmt, exitcode, msg, ...) \
	({printf("%s:%d:0: function: %s\n%s:"LINE_FORMAT_SPEC":0: error: "msg"\n", __FILE__, __LINE__, \
			 __FUNCTION__, stmt->filepath, stmt->lineno, __VA_ARGS__); exit(exitcode);})

static char *Bsbs_StmtType_ToString(Bsbs_StmtType type) {
	switch (type) {
	case Bsbs_StmtType_Let:
		return "Bsbs_StmtType_Let";
	case Bsbs_StmtType_Cmd:
		return "Bsbs_StmtType_Cmd";
	case Bsbs_StmtType_Label:
		return "Bsbs_StmtType_Label";
	case Bsbs_StmtType_Section:
		return "Bsbs_StmtType_Section";
	default:
		Error("unknown StmtType '%d'", type);
	}
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
    if (!file) Error("could not open file '%s'", filename);
	char *line = (char *)malloc(sizeof(char)*Line_Capacity);
	*stmts = NULL;
	int addToInnerScope = 0;
	Bsbs_StmtType type;
	size_t lineno = 0;
	Bsbs_Stmt *labelStmt = NULL;
	Bsbs_Stmt *sectionStmt = NULL;
    while (fgets(line, Line_Capacity, file)) {
		lineno++;
		char *ptr = line;
		Bsbs_Trim(&ptr);
		if (strlen(ptr) == 0) continue;
		Bsbs_Stmt *stmt = (Bsbs_Stmt *)malloc(sizeof(Bsbs_Stmt));
		stmt->lineno = lineno;
		stmt->filepath = strdup(filename);
		stmt->next= NULL;
		if (Bsbs_StartsWith(ptr, Bsbs_Token_Label)) {
			if (labelStmt != NULL) Bsbs_Stmt_Error(stmt, "found unsupported nested labels");
			ptr += strlen(Bsbs_Token_Label);
			Bsbs_Trim(&ptr);
			char *name = Bsbs_FetchUntil(&ptr, ' ');
			Bsbs_Trim(&ptr);
			if (!Bsbs_StartsWith(ptr, Bsbs_Token_ScopeStart))
				Bsbs_Stmt_ErrorFmt(stmt, "expected token '"Bsbs_Token_ScopeStart
								   "' after label name '%s'", name);
			ptr += strlen(Bsbs_Token_ScopeStart);
			Bsbs_Trim(&ptr);
			if (strlen(ptr))
				Bsbs_Stmt_ErrorFmt(stmt, "invalid token '%s' after token '"
								   Bsbs_Token_ScopeStart"'", ptr);
			type = Bsbs_StmtType_Label;
			labelStmt = stmt;
			labelStmt->type = type;
			labelStmt->label = (Bsbs_Stmt_Label *)malloc(sizeof(Bsbs_Stmt_Label));
			labelStmt->label->name = name;
			labelStmt->label->stmts = NULL;
			addToInnerScope = 1;
			continue;
		}
		if (Bsbs_StartsWith(ptr, Bsbs_Token_Section)) {
			if (sectionStmt != NULL)
				Bsbs_Stmt_Error(stmt, "found unsupported nested SECTIONs");
			ptr += strlen(Bsbs_Token_Section);
			Bsbs_Trim(&ptr);
			char *name = Bsbs_FetchUntil(&ptr, ' ');
			Bsbs_Trim(&ptr);
			if (!Bsbs_StartsWith(ptr, Bsbs_Token_ScopeStart))
				Bsbs_Stmt_ErrorFmt(stmt, "expected token '"Bsbs_Token_ScopeStart
								   "' after section name '%s'", name);
			ptr += strlen(Bsbs_Token_ScopeStart);
			Bsbs_Trim(&ptr);
			if (strlen(ptr))
				Bsbs_Stmt_ErrorFmt(stmt, "invalid token '%s' after token '"
								   Bsbs_Token_ScopeStart"'", ptr);
			type = Bsbs_StmtType_Section;
			sectionStmt = stmt;
			sectionStmt->type = type;
			sectionStmt->section = (Bsbs_Stmt_Section *)malloc(sizeof(Bsbs_Stmt_Section));
			sectionStmt->section->name = name;
			sectionStmt->section->stmts = NULL;
			addToInnerScope = 1;
			continue;
		}
		if (Bsbs_StartsWith(ptr, Bsbs_Token_Let)) {
			ptr += strlen(Bsbs_Token_Let);
			Bsbs_Trim(&ptr);
			stmt->type = Bsbs_StmtType_Let;
			stmt->let = (Bsbs_Stmt_Let *)malloc(sizeof(Bsbs_Stmt_Let));
			stmt->let->name = Bsbs_FetchUntil(&ptr, '=');
			Bsbs_Trim(&stmt->let->name);
			Bsbs_RemoveSpecial(&ptr);
			Bsbs_Trim(&ptr);
			stmt->let->value = strdup(ptr);
			if (addToInnerScope) {
				switch (type) {
				case Bsbs_StmtType_Label:
					Bsbs_Stmt_Add(&labelStmt->label->stmts, stmt);
					break;
				case Bsbs_StmtType_Section:
					Bsbs_Stmt_Add(&sectionStmt->section->stmts, stmt);
					break;
				default:
					break;
				}
			} else Bsbs_Stmt_Add(stmts, stmt);
			continue;
		}
		if (Bsbs_StartsWith(ptr, Bsbs_Token_Run)) {
			ptr += strlen(Bsbs_Token_Run);
			Bsbs_Trim(&ptr);
			stmt->type = Bsbs_StmtType_Cmd;
			stmt->cmd = (Bsbs_Stmt_Cmd *)malloc(sizeof(Bsbs_Stmt_Cmd));
			stmt->cmd->cmd = strdup(ptr);
			if (addToInnerScope) {
				switch (type) {
				case Bsbs_StmtType_Label:
					Bsbs_Stmt_Add(&labelStmt->label->stmts, stmt);
					break;
				case Bsbs_StmtType_Section:
					Bsbs_Stmt_Add(&sectionStmt->section->stmts, stmt);
					break;
				default:
					break;
				}
			} else Bsbs_Stmt_Add(stmts, stmt);
			continue;
		}  
		if (Bsbs_StartsWith(ptr, Bsbs_Token_ScopeEnd)) {
			ptr += strlen(Bsbs_Token_ScopeEnd);
			Bsbs_Trim(&ptr);
			if (strlen(ptr))
				Bsbs_Stmt_ErrorFmt(stmt, "invalid token '%s' after token '"
								   Bsbs_Token_ScopeEnd"'", ptr);
			switch (type) {
			case Bsbs_StmtType_Label:
				labelStmt->label->endLineno = lineno;
				Bsbs_Stmt_Add(stmts, labelStmt);
				labelStmt = NULL;
				break;
			case Bsbs_StmtType_Section:
				sectionStmt->section->endLineno = lineno;				
				Bsbs_Stmt_Add(stmts, sectionStmt);
				sectionStmt = NULL;
				break;
			default:
				Bsbs_Stmt_ErrorFmt(stmt, "expected statement type '%s | %s' got '%s'",
						  Bsbs_StmtType_ToString(Bsbs_StmtType_Label),
						  Bsbs_StmtType_ToString(Bsbs_StmtType_Section),
						  Bsbs_StmtType_ToString(type));
			}
			addToInnerScope = 0;
			continue;
		}
	}
}

static void PrintIndent(size_t lineno, size_t indent) {
	printf(LINE_FORMAT_SPEC"\t", lineno);
	for (size_t i = 0; i < (indent<<2); ++i) printf(" ");
}

static void Bsbs_Stmt_Print_(Bsbs_Stmt *stmts, size_t indent) {
	Bsbs_Stmt *stmt = stmts;
	while (stmt) {
		PrintIndent(stmt->lineno, indent);
		switch (stmt->type) {
		case Bsbs_StmtType_Let:
			printf(Bsbs_Token_Let" %s = %s\n", stmt->let->name, stmt->let->value);
			break;
		case Bsbs_StmtType_Cmd:
			printf(Bsbs_Token_Run" %s\n", stmt->cmd->cmd);
			break;
		case Bsbs_StmtType_Label:
			printf(Bsbs_Token_Label" %s "Bsbs_Token_ScopeStart"\n", stmt->label->name);
			Bsbs_Stmt_Print_(stmt->label->stmts, indent+1);
			PrintIndent(stmt->label->endLineno, indent);
			printf(Bsbs_Token_ScopeEnd"\n");
			break;
		case Bsbs_StmtType_Section:
			printf(Bsbs_Token_Section" %s "Bsbs_Token_ScopeStart"\n", stmt->section->name);
			Bsbs_Stmt_Print_(stmt->section->stmts, indent+1);
			PrintIndent(stmt->section->endLineno, indent);
			printf(Bsbs_Token_ScopeEnd"\n");
			break;
		}
		stmt = stmt->next;
	}
}

void Bsbs_Stmt_Print(Bsbs_Stmt *stmts) {
	Bsbs_Stmt_Print_(stmts, 0);
}

#endif // BSBS_PARSER_H_
