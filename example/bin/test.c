#include <stdio.h>
#include <dlfcn.h>
#include "hgame_prog.h"

#define die(x) do{ perror(x); exit(1); }while(1)

int
main(int argc,char *argv[])
{
	FILE *dd;
	struct hgame_program * prog;

	if(!argv[1])
		return 1;

	dd = dlopen(argv[1],RTLD_GLOBAL | RTLD_NOW);

	prog = dlsym(dd,"HgameProg");

	char* arg[] = {"crack", "lol" };
	TokenCtx ctx = { arg, 2 };

	printf("%s %s\n",(char*)prog->prog_name,(char*)prog->version);

	prog->function(&ctx,&ctx);

	return 0;
}
		

