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
	int    uptime;

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

typedef struct {
	char** args;
	int    argc;
} TokenCtx;

#define xdie(x) do{ perror(x); exit(1); }while(1)

#define DEF_KERNEL "2.6.34-rc6"

#define HGAME_MAIN(f) int f (struct hgame_data* arg,Hgame * main)
#define HGAME_PROG(n,v,f) struct hgame_program HgameProg = { n, v, f };

#endif/*_HGAME_*/
