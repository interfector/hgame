#ifndef _HGAME_
#define _HGAME_

#include <time.h>

struct hgame_data {
	char* content;
	int   lenght;
};

struct hgame_host {
	int   mode;
	char* server;
	int   port;

	struct hgame_data data;
};

struct hgame_pc {
	char* os;
	char* user;
	char* pass;
	char* hostname;
	char* kernel;

	struct tm date;
	int    uptime;

	int *load_average;
};

typedef struct {
	char* name;
	int   money;
	int   level;
	int   mode;

	int connected;

	struct hgame_pc pc;

	struct hgame_host current;

	struct hgame_program* programs;
	struct hgame_device * devices;
} Hgame;

typedef int (*hgame_callback)(struct hgame_data*,Hgame*);

struct hgame_device {
	char* dev_name;
	char* version;
	int   dev_id;

	hgame_callback init;
	hgame_callback ioctl;
	hgame_callback end;
};

struct hgame_program {
	char* prog_name;
	char* version;
	char* prog_usage;

	hgame_callback function;
};

#define xdie(x) do{ perror(x); exit(1); }while(1)

#define DEF_KERNEL "2.6.34-rc6"

#define HGAME_MAIN(f) int f (struct hgame_data* arg,Hgame * main)
#define HGAME_DEV(n,v,i,in,io,en) struct hgame_device HgameDevice = { n, v, i, in, io, en };

#endif/*_HGAME_*/
