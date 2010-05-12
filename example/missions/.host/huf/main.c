#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../hgame_host.h"

HGAME_HOST(host_main)
{
	int i;
	TokenCtx* ctx = (TokenCtx*)arg;

	ctx->args[0] = realloc(ctx->args[0],10);
	strcpy(ctx->args[0],"guest");
}
