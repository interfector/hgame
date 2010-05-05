#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hgame_prog.h"

char*
genPass(char* pwd,int num,char* charset)
{
	int i;
	struct timeval tv;
	char* pass;

	if(gettimeofday(&tv,NULL) == -1)
		return NULL;

	srand(tv.tv_usec);

	pass = malloc( num );

	for(i = 0;i < num;i++)
	{
		if(pwd[i] == 0x1b)
		{
			if(!charset)
				pass[i] = (rand() % 95) + 32;
			else
				pass[i] = charset[(rand() % strlen(charset))];
		} else
			pass[i] = pwd[i];

		putchar(pass[i]);

		fflush(stdout);
	}

	return pass;
}

char*
checkpass(char* pass,char* pwd)
{
	int i;
	char* p = malloc(strlen(pass));

	for(i = 0;i < strlen(pass);i++)
		if(pass[i] != pwd[i])
			p[i] = 0x1b;
		else
			p[i] = pwd[i];

	return p;
}

HGAME_MAIN(crack)
{
	int i;
	struct timespec req = { 0, 200 * 1000000 };
	char* pass;
	char* pwd;
	TokenCtx* ctx = (TokenCtx*)arg;
	int len;

	if(!ctx->args[1])
		return 1;

	len = strlen(ctx->args[1]);

	pwd = malloc(len + 1);

	memset(pwd,'@',len);

	while(1)
	{
		pass = genPass(pwd,len,/*ctx->args[2]*/NULL);

		printf("\033[%dD",len);

		pwd = checkpass(pass,ctx->args[1]);

		nanosleep(&req,NULL);

		if(!strcmp(pass,pwd))
		{
			fflush(stdout);

			printf("Passoword cracked: %s.\n",pass);

			free(pass);
			free(pwd);

			return 0;
		}

		free(pass);
	}

	putchar('\n');
}

HGAME_PROG("rcrack","0.0.1",crack);
