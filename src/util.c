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
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <hgame.h>

#define abs(x) ((x > 0) ? x : -x)

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

void
clear()
{
	int fd;
	struct winsize size;

	if ((fd = open("/dev/tty",O_RDWR)) < 0)
		return;
	if(ioctl(fd,TIOCGWINSZ,&size))
		return;

	close(fd);

	for(fd = 0;fd < size.ws_row;fd++)
		putchar('\n');

	printf("\033[%dA",size.ws_row);
}

int*
termsize()
{
	int fd;
	struct winsize size;
	int * dim = malloc(sizeof(int) * 2);

	if ((fd = open("/dev/tty",O_RDWR)) < 0)
		return NULL;
	if(ioctl(fd,TIOCGWINSZ,&size))
		return NULL;

	close(fd);

	dim[0] = size.ws_row;
	dim[1] = size.ws_col;

	return dim;
}

void _sleep(float);

void
create_progress(void* args)
{
	int i,j;
	int * size = termsize();

	int* arg = (int*)args;

	int progress_len = 25 + *arg;

	for(i = 0;i < progress_len - 3;i++)
	{
		for(j = 0;j < i;j++)
			printf("\033[s\033[1;%df=\033[u",size[1] - (progress_len-j));

		printf("\033[s\033[1;%df>\033[u",size[1] - (progress_len-j));

		_sleep(1.0 + (float)((float)hgame_main.pc.net.power / 100.0));

		printf("\033[s\033[1;%dfLST\033[u",size[1] - 3);
	}
}

void
_sleep(float s)
{
	struct timeval tv = { (int)s, (float)(s - (int)s) };

	select(0,NULL,NULL,NULL,&tv);
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

char* 
ipgen()
{
	char *ip = malloc(15);
	struct timeval tv;

	if(gettimeofday(&tv,NULL) < 0)
		return NULL;

	srand(tv.tv_usec);

	sprintf(ip,"%d.%d.%d.%d",(rand() % 256),
						(rand() % 256),
						(rand() % 256),
						(rand() % 256));
																				
	return ip;
}

void
KnownHostparse()
{
	char* home = getenv("USERNAME");
	char* path = malloc(30 + strlen(home));
	FILE *fp;
	int port = 0;
	char *ptr,*old;
	char buf[BUFSIZ]= { 0 };

	hgame_main.known_hosts = malloc(sizeof(struct hgame_host));

	sprintf(path,"/home/%s/.hgame/devices/hosts",home);

	if(!(fp = fopen(path,"r")))
	{
		printf("* Error reading known hosts file.\n");
		return;
	}

	while(fgets(buf,BUFSIZ,fp))
	{
		buf[strlen(buf)-1] = 0;

		ptr = strtok(buf,":");

		old = strdup(ptr);

		port = atoi(strtok(NULL,":"));

		if(!ptr)
			continue;

		hgame_main.known_hosts = realloc(hgame_main.known_hosts,sizeof(struct hgame_host) * ++hgame_main.host_c);

		ptr = strtok(old,"|");

		hgame_main.known_hosts[hgame_main.host_c-1].host = strdup(ptr);

		ptr = strtok(NULL,"|");

		hgame_main.known_hosts[hgame_main.host_c-1].ip = strdup(ptr);

		ptr = strtok(buf,":");
		ptr = strtok(NULL,":");

		hgame_main.known_hosts[hgame_main.host_c-1].port = port;

	}
}

#define SS_PROMPT "(\033[1;34m%s\033[1;32m@\033[1;34m%s\033[0m)\033[33m%%>\033[0m%s "

void
ss_start(TokenCtx * info) /* server shell start */
{
	char* line;
	int user;

	/*
	 * info->args[0] == username
	 * info->args[1] == hostname
	 */

	user = strcmp(info->args[0],"admin");

	while(1)
	{
		line = getline(SS_PROMPT,info->args[0],info->args[1],(!user ? "#" : "$"));

		if(!strcmp(line,"exit"))
			return;
	}
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

			if(!(d = opendir(path)) || strstr(path,"missions") || strstr(path,".."))
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
		if(!ctx->args[1] || !ctx->args[1][0] || strstr(ctx->args[1],"missions") || strstr(ctx->args[1],".."))
			printf("* Couldn't open the file.\n");
		else
			callbacks[ON_FILEOPEN]((struct hgame_data*)ctx->args[1]);
	} else if (!strcmp(ctx->args[0],"arch"))
	{
		time_t ts = time(NULL);
		hgame_main.pc.date = localtime(&ts);

		printf("%s %s %s %02d/%02d/%04d %02d:%02d:%02d\n",
				hgame_main.pc.os,
				hgame_main.pc.hostname,
				hgame_main.pc.kernel,
				hgame_main.pc.date->tm_mday,
				hgame_main.pc.date->tm_mon,
				hgame_main.pc.date->tm_year + 1900,
				hgame_main.pc.date->tm_hour,
				hgame_main.pc.date->tm_min,
				hgame_main.pc.date->tm_sec );
	} else if (!strcmp(ctx->args[0],"uptime"))  /* TO FINISH */
	{
		printf("%02dh%02dm%02ds\n",0,1,2);
	} else if (!strcmp(ctx->args[0],"rm"))
	{
		if(ctx->args[1])
		{
			if(strstr(ctx->args[1],"missions") || strstr(ctx->args[1],".."))
			{
				printf("* Couldn't remove the file.\n");
				return;
			}

			path = realloc(path,strlen(path) + strlen(ctx->args[1]));

			strcat(path,ctx->args[1]);

			if(unlink(path))
				printf("* Couldn't remove the file.\n");
		} else {
			printf("* You must specify the target file.\n");
		}
	} else if (!strcmp(ctx->args[0],"connect"))
	{
		char* home = getenv("USERNAME");
		char* path;
		char* ptr;
		FILE *dl;
		int i,t = 0;

		int (*f)(struct hgame_data*,Hgame*);

		if(!hgame_main.pc.net.conn)
		{
			printf("* Connection timed out.\n");
			return;
		}

		if(!ctx->args[1] || !ctx->args[1][0])
		{
			printf("* Couldn't connect to an empty address.\n");
			return;
		}

		for(i = 0;i < hgame_main.host_c;i++)
			if(!strcmp(hgame_main.known_hosts[i].host,ctx->args[1]))
				t = 1;

		if(!t)
		{
			printf("* Host not found.\n");
			return;
		}

		path = malloc(50 + strlen(home));

		if((ptr = strtok(ctx->args[1],":")))
			ptr = strtok(ptr,".");
		else
			ptr = strtok(ctx->args[1],".");

		sprintf(path,"/home/%s/.hgame/missions/.host/%s/main.so",home,ptr);

		dl = dlopen(path, RTLD_GLOBAL | RTLD_NOW);

		if(!dl)
		{
			fprintf(stderr,"* Couldn't connect to the specified address.\n");
			return;
		}

		f = (int (*)(struct hgame_data*,Hgame*))dlsym(dl,"host_main");

		_sleep(1.0 + (float)(1.0 - (float)((float)hgame_main.pc.net.power / 100.0)));

		f((struct hgame_data*)ctx,&hgame_main);

		ss_start(ctx);
	} else if (!strcmp(ctx->args[0],"resolve"))
	{
		char* home = getenv("USERNAME");
		char* path;
		FILE * fp;
		char* ptr;
		char buf[BUFSIZ];

		if(!ctx->args[1] || ctx->args[1][0] == 0)
		{
			printf("* Couldn't resolve an empty address.\n");
			return;
		}

		if(!hgame_main.pc.net.conn)
		{
			printf("* Connection timed out.\n");
			return;
		}

		path = malloc(30 + strlen(home));

		sprintf(path,"/home/%s/.hgame/missions/.hosts",home);

		if(!(fp = fopen(path,"r")))
		{
			printf("* Error resolving the host.\n");
			return;
		}

		_sleep(1.0 + (float)(1.0 - (float)((float)hgame_main.pc.net.power / 100.0)));

		while(fgets(buf,BUFSIZ,fp))
		{
			buf[strlen(buf)-1] = 0;

			ptr = strtok(buf,"|");

			if(!strcmp(ptr,ctx->args[1]))
			{
				ptr = strtok(NULL,"|");

				printf("%s\n",ptr);

				fclose(fp);

				sprintf(path,"/home/%s/.hgame/devices/hosts",home);

				if(!(fp = fopen(path,"a")))
				{
					printf("* Couldn't add this host to the resolved hosts list.\n");

					return;
				}

				fprintf(fp,"%s|%s:80\n",ctx->args[1],ptr);

				hgame_main.known_hosts = realloc(hgame_main.known_hosts,sizeof(struct hgame_host) * ++hgame_main.host_c);
				hgame_main.known_hosts[hgame_main.host_c-1].ip = strdup(ptr);
				hgame_main.known_hosts[hgame_main.host_c-1].port = 80;

				fclose(fp);

				return;
			}
		}
		/*
		free(hgame_main.known_hosts);

		KnownHostparse();
		*/
		printf("* Couldn't resolve this host.\n");
	} else if (!strcmp(ctx->args[0],"start"))
	{
		if(!ctx->args[1] || ctx->args[1][0] == 0)
		{
			printf("* You must insert a service or a mission.\n");
			return;
		}

		if(!strcmp(ctx->args[1],"network"))
		{
			if(hgame_main.pc.net.conn)
			{
				printf("* You are already connected.\n");
				return;
			}

			hgame_main.pc.net.localip = ipgen();

			hgame_main.pc.net.essid = malloc(15);
			sprintf(hgame_main.pc.net.essid,"nethack.%03X",(rand() % 256));

			hgame_main.pc.net.power = (rand() % 31) + 70;

			_sleep(1);

			printf("* Connected successfully to %s.\n",hgame_main.pc.net.essid);

			hgame_main.pc.net.conn = 1;
		} else
			printf("* Service not found.\n");

	} else if (!strcmp(ctx->args[0],"network"))
	{
		if(!hgame_main.pc.net.conn)
		{
			printf("* You aren't connected yet.\n");
			return;
		}

		printf("ESSID: %s.\nPower: %d%%.\nIP: %s.\n",hgame_main.pc.net.essid,
											hgame_main.pc.net.power,
											hgame_main.pc.net.localip);
	} else if (!strcmp(ctx->args[0],"clear"))
	{
		clear();
	} else if (!strcmp(ctx->args[0],"help"))
	{
		printf(
			  "ls - List all the files in the current directory.\n"
			  "open - Open a file with the default editor.\n"
			  "arch - Display architecture information.\n"
			  "rm - Remove a file.\n"
			  "connect - Connect to a specified host.\n"
			  "resolve - Resolve a specified host.\n"
			  "start - Starts a service as: network, missions.\n"
			  "network - Display network information.\n"
			  "clear - Clear the terminal\n");
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

