/*
 * Hgame, an hacking game
 * Copyright (C) 2010 nex
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <hgame.h>

void
die(char* fmt,...)
{
	va_list va;

	va_start(va,fmt);

	vfprintf(stderr,fmt,va);

	va_end(va);

	exit(1);
}

char*
getline(char* fmt,...)
{
	va_list va;
	char ch;
	char *str = malloc(1);
	int i = 0;

	va_start(va,fmt);

	vprintf(fmt,va);

	while((ch = getchar()) != '\n')
	{
		str = realloc(str,i+1);
		str[i++] = ch;
	}

	str[i] = '\0';

	va_end(va);

	return str;
}

#define split_and_check(x,y) ptr = strtok(x,y); \
					    if(!ptr) die("* Syntax error: missing resource definition.\n");

struct hgame_rc*
getResource(char* path)
{
	FILE*  fp;
	struct hgame_rc *rc = malloc(sizeof(struct hgame_rc));
	char* ptr;
	char  buf[256] = { 0 };

	if(!(fp = fopen(path,"rb")))
	{
		printf("* No resources found in %s.\n",path);
		return NULL;
	}

	while(fgets(buf,256,fp))
	{
		buf[strlen(buf)-1] = '\0';

		split_and_check(buf,":");

		if(!strcmp(ptr,"name"))
		{
			split_and_check(NULL,":");

			rc->name = strdup(ptr);
		} else if (!strcmp(ptr,"money"))
		{
			split_and_check(NULL,":");

			rc->money = atoi(ptr);
		} else if (!strcmp(ptr,"level"))
		{
			split_and_check(NULL,":");

			rc->level = atoi(ptr);
		} else if (!strcmp(ptr,"mode"))
		{
			split_and_check(NULL,":");

			rc->mode = atoi(ptr);
		} else if (!strcmp(ptr,"user"))
		{
			split_and_check(NULL,":");

			rc->user = strdup(ptr);
		} else if (!strcmp(ptr,"pass"))
		{
			split_and_check(NULL,":");

			rc->pass = strdup(ptr);
		} else if (!strcmp(ptr,"os"))
		{
			split_and_check(NULL,":");

			rc->os = strdup(ptr);
		} else if (!strcmp(ptr,"kernel"))
		{
			split_and_check(NULL,":");

			rc->kernel = strdup(ptr);
		} else if (!strcmp(ptr,"hostname"))
		{
			split_and_check(NULL,":");

			rc->hostname = strdup(ptr);
		} else
			die("* Syntax error: unknown tag definition.\n");
	}

	fclose(fp);

	return rc;
}

void
rcSave(struct hgame_rc *rc,char* filename)
{
	FILE* fp;

	if(!(fp = fopen(filename,"w")))
		xdie("fopen");

	fprintf(fp,"name:%s\nmoney:%d\nlevel:%d\nmode:%d\nuser:%s\npass:%s\nos:%s\nkernel:%s\nhostname:%s\n",
			rc->name,
			rc->money,
			rc->level,
			rc->mode,
			rc->user,
			rc->pass,
			rc->os,
			rc->kernel,
			rc->hostname);

	fclose(fp);
}

void
setHgameResource(struct hgame_rc * rc)
{
	time_t t;

	hgame_main.pc.os = strdup(rc->os);
	hgame_main.pc.user = strdup(rc->user);
	hgame_main.pc.pass = strdup(rc->pass);
	hgame_main.pc.hostname = strdup(rc->hostname);
	hgame_main.pc.kernel = strdup(rc->kernel);

	t = time(NULL);
	hgame_main.pc.date = localtime(&t);

	hgame_main.pc.uptime = 0;

	hgame_main.name = strdup(rc->name);
	hgame_main.money = rc->money;
	hgame_main.level = rc->level;
	hgame_main.mode = rc->mode;
}

void
TokenParse(TokenCtx *ctx,char * cmd)
{
	char *ptr,*old;

	old = strtok(cmd," ");
	ptr = strtok(NULL," ");

	if(!ptr || ptr[0] == '\0')
	{
		ctx->args = malloc(sizeof(char*));
		ctx->args[0] = strdup(cmd);
		ctx->argc = 1;

		ctx->args[1] = NULL;

		return;
	}

	ctx->args = malloc(sizeof(char*));
	ctx->args[0] = strdup(old);
	ctx->argc = 1;

	do
	{
		ctx->args = realloc(ctx->args,sizeof(char*) * ++ctx->argc);
		ctx->args[ctx->argc-1] = strdup(ptr);
	}while((ptr = strtok(NULL," ")));
}

void
LineParse(TokenCtx* ctx)
{
	char* home = getenv("USERNAME");
	char* path = malloc(15 + strlen(home)); 

	sprintf(path,"/home/%s/.hgame/",home);

	if(!strcmp(ctx->args[0],"ls"))
	{
		DIR * d;
		struct dirent * entry;

		if(ctx->args[1])
		{
			path = realloc(path,strlen(path) + strlen(ctx->args[1]));

			strcat(path,ctx->args[1]);

			if(!(d = opendir(path)))
			{
				printf("* Couldn't access to the directory.\n");

				return;
			}

			while((entry = readdir(d)))
			{
				if(entry->d_type == DT_DIR)
					printf("\033[1;34m%s\033[0m   ",entry->d_name);
				else
					printf("%s   ",entry->d_name);
			}

			putchar('\n');
		} else {
			if(!(d = opendir(path)))
			{
				printf("* Couldn't access to the directory.\n");

				return;
			}

			while((entry = readdir(d)))
			{
				if(entry->d_type == DT_DIR)
					printf("\033[1;34m%s\033[0m   ",entry->d_name);
				else
					printf("%s   ",entry->d_name);
			}

			putchar('\n');
		}
	} else if (!strcmp(ctx->args[0],"open"))
	{
		if(!ctx->args[1] || !ctx->args[1][0])
			printf("* Couldn't open the file.\n");
		else
			callbacks[ON_FILEOPEN]((struct hgame_data*)ctx->args[1]);
	} else if (!strcmp(ctx->args[0],"arch"))
	{
		time_t ts = time(NULL);
		struct tm * tm = localtime(&ts);

		printf("%s %s %s %02d/%02d/%04d %02d:%02d:%02d\n",
				hgame_main.pc.os,
				hgame_main.pc.hostname,
				hgame_main.pc.kernel,
				tm->tm_mday,
				tm->tm_mon,
				tm->tm_year + 1900,
				tm->tm_hour,
				tm->tm_min,
				tm->tm_sec );
	} else if (!strcmp(ctx->args[0],"rm"))
	{
		if(ctx->args[1])
		{
			path = realloc(path,strlen(path) + strlen(ctx->args[1]));

			strcat(path,ctx->args[1]);

			if(unlink(path))
				printf("* Couldn't remove the file.\n");
		} else {
			printf("* You must specify the target file.\n");
		}
	} else {
		int r = -1;
		int i;

		for(i = 0;i < hgame_main.program_c;i++)
			if(!strcmp(ctx->args[0],hgame_main.programs[i].prog_name))
				r = hgame_main.programs[i].function((struct hgame_data*)ctx,&hgame_main);

		if(r == -1)
			printf("* Command not found: %s\n",ctx->args[0]);
	}

	free(path);
}

