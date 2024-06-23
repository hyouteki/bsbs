#include "src/parser.h"
#include "src/emulator.h"

int main(int argc, char **argv) {
	char *label = NULL;
	if (argc == 1) label = "all";
	else label = argv[1];
	Bsbs_Stmt *stmts = NULL;
	Bsbs_ParseFile("bsbsfile", &stmts);
	/* Bsbs_Stmt_Print(stmts); */
	Bsbs_Context ctx = (Bsbs_Context){.label=label, .hashmap={{0}}};
	Bsbs_Emulator(stmts, ctx);
	return 0;
}
