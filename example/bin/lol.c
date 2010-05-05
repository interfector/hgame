#include "hgame_prog.h"

HGAME_MAIN(lol)
{
	printf("%s, wtf?\n",((TokenCtx*)arg)->args[1]);
}

HGAME_PROG("lol","0.0.1",lol);
