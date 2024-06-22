#include "src/parser.h"
#include "src/emulator.h"

int main() {
	Bsbs_Stmt *stmts = NULL;
	Bsbs_ParseFile("bsbsfile", &stmts);
	Bsbs_Stmt_Print(stmts);
	/* Bsbs_Context ctx = (Bsbs_Context){.label="build"}; */
	/* Bsbs_Emulator(stmts, ctx); */
	return 0;
}
