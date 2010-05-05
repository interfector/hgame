#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hgame_prog.h"

#define TRUE	1
#define FALSE	0

#define abs(x) ((x > 0) ? x : -x)

char def_charset[] = "abcdefghijklmnopqrstuvwxyzABCDEGHIJKLMNOPQRSTUVZ0123456789";

unsigned int inc[128];
char* pass;

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

int finished(char *block, char *charset, char *templ)
{
	unsigned int i;

	if(strcmp(block,pass))
		return FALSE;

	return TRUE;
}

void increment(char *block, int len, char *charset, char *templ)
{
	int i;

	for(i = strlen(block)-1;i > -1;i--)
	{
		if(templ[i] == '@')
		{
			if(inc[i] < strlen(charset)-1)
			{
				inc[i]++;
				block[i] = charset[inc[i]];
				i=-1;
			}
			else
			{
				block[i] = charset[0];
				inc[i] = 0;
			}
		}
	}
}

int
chunk(int start,char *charset, char *templ)
{
	int i,j,k,t;
	char block[128];
	int sec,usec;
	struct timeval tv;
	int s;
	
	if(gettimeofday(&tv,NULL) < 0)
		return;
		
	sec = tv.tv_sec;
	usec = tv.tv_usec;
	
	memset(block,0,sizeof(block));

	for(j = 0;j < start;j++)
	{
		if(templ[j] == '@')
		{
			block[j] = charset[0];
			inc[j] = 0;
		}
	}

	while(!finished(block,charset,templ))
	{
		increment(block,start,charset,templ);

		for(j = 0;j < start;j++)
			if(templ[j] != '@')
				block[j] = templ[j];
		
		for(s = 0;s < strlen(block);s++)
		{
			putchar(block[s]);
		
			fflush(stdout);
		}

		printf("\033[%dD",strlen(block));

		for(i = 0;i < strlen(pass);i++)
			if(block[i] != pass[i])
				templ[i] = '@';
			else
				templ[i] = pass[i];

		struct timespec req = { 0, 100 * 1000000 };
		nanosleep(&req,NULL);
	}

	gettimeofday(&tv,NULL);
	printf("Password cracked: %s in %ld.%ld seconds.\n",block,
			((tv.tv_usec - usec) > 0) ? (tv.tv_sec - sec) : (tv.tv_sec - sec - 1),abs((tv.tv_usec - usec)));

	return 1;
}

HGAME_MAIN(crack)
{
	int i;
	TokenCtx* ctx = (TokenCtx*)arg;
	char* pwd;
	int len;

	if(!ctx->args[1])
		return 1;

	len = strlen(ctx->args[1]);
	
	pwd = malloc(len + 1);

	memset(pwd,'@',len);

	pass = malloc(len);
	strcpy(pass,ctx->args[1]);

	i = chunk(len,def_charset,pwd);

	putchar('\n');
}

HGAME_PROG("crack","0.0.1",crack);
