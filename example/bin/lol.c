#include <stdio.h>
#include <stdarg.h>
#include "hgame_prog.h"

void*
hgame_getline()
{
	__asm__("movl	$getline,%eax");
}

HGAME_MAIN(lol)
{
	char* (*getline)(char*,...) = hgame_getline();

	printf("%s, wtf?\n",((TokenCtx*)arg)->args[1]);
}

HGAME_PROG("lol","0.0.1",lol);
