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

#ifndef _HGAME_
#define _HGAME_

#include <time.h>

struct hgame_data {
	char* content;
	int   lenght;
};

struct hgame_host {
	int   mode;
	char* host;
	char* ip;
	int   port;

	struct hgame_data * data;
};

struct hgame_network {
	char* essid;
	char* localip;
	char* mac;

	int power;

	int conn;
};

struct hgame_pc {
	char* os;
	char* user;
	char* pass;
	char* hostname;
	char* kernel;

	struct hgame_network net;

	struct tm * date;

	struct tm * initd;
	float    uptime;

	int *load_average;
};

typedef struct {
	char* name;
	int   money;
	int   level;
	int   mode;

	int connected;

	struct hgame_host * known_hosts;
	int    host_c;

	struct hgame_pc pc;

	struct hgame_host current;

	struct hgame_program* programs;
	int program_c;
	struct hgame_device * devices;
	int device_c;

} Hgame;

typedef int (*hgame_callback)(struct hgame_data*,Hgame*);

struct hgame_device {
	char* dev_name;
	int   dev_id;

	hgame_callback init;
	hgame_callback ioctl;
	hgame_callback end;
};

struct hgame_program {
	char* prog_name;
	char* version;

	hgame_callback function;
};

struct hgame_rc {
	char* name;
	int   money;
	int   level;
	int   mode;

	char* hostname;
	char* user;
	char* pass;
	char* os;
	char* kernel;
};

typedef enum { ON_INIT,
			ON_CONNOPEN,
			ON_CONNREF,
			ON_HOSTNOTFOUND,
			ON_FILEOPEN,
		/*	ON_DEVOPEN,
			ON_DEVEXIT,
			ON_DEVIOCTL,
			ON_PROGUSE, */
			ON_NEWMAIL,
			ON_DOWNLOAD,
			ON_UPLOAD,
			ON_CRACKING,
			ON_EXIT } on_function;

#include <stdarg.h>

void die(char*,...);
char*  getRcPath();
struct hgame_rc* getResource(char*);
void   rcSave(struct hgame_rc*,char*);
char*  getline(char*,...);
void   setHgameResource(struct hgame_rc*);
void   KnownHostparse(void);
void   clear();
void   create_progress();

#define xdie(x) do{ callbacks[ON_EXIT](NULL); perror(x); exit(1); }while(1)

#define DEF_KERNEL "2.6.34-rc6"
#define PROMPT     "(\033[1;31m%s\033[32m@\033[31m%s\033[0m)\033[33m%>\033[0m "

#define SIMON_PROMPT(x) "%s says %% ",x

int on_init(struct hgame_data*);
int on_connopen(struct hgame_data*);
int on_connref(struct hgame_data*);
int on_hostnfound(struct hgame_data*);
int on_fileopen(struct hgame_data*);
int on_newmail(struct hgame_data*);
int on_download(struct hgame_data*);
int on_upload(struct hgame_data*);
int on_cracking(struct hgame_data*);
int _on_exit(struct hgame_data*);

static int (*callbacks[])(struct hgame_data*) = { on_init,
						 on_connopen,
						 on_connref,
						 on_hostnfound,
						 on_fileopen,
					/*	 on_devopen,
						 on_devexit,
						 on_devioctl, 
						 on_proguse, */
						 on_newmail,
						 on_download,
						 on_upload,
						 on_cracking,
						 _on_exit };

Hgame hgame_main;
char* rc_path;

typedef struct {
	char** args;
	int    argc;
} TokenCtx;

void TokenParse(TokenCtx*,char*);
void LineParse(TokenCtx*);

void load_programs(void);

extern int inotify_init(void);
extern int inotify_add_watch(int,const char*,int);
extern int inotify_rm_watch(int,int);

#define VERSION_TEXT \
		  "hgame-"VERSION" Copyright (C) 2010 nex (... stoke, darkness)\n" \
		  "This program comes with ABSOLUTELY NO WARRANTY.\n" \
		  "This is free software, and you are welcome to redistribute it\n" \
	       "under certain conditions.\n"

#endif/*_HGAME_*/
