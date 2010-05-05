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
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <hgame.h>

void
sighandler(int sig)
{
	printf("\n<C-c> SIGINT:  Interrupt, type 'exit' to quit the program.\n");
}

int
main(int argc,char *argv[])
{
	int i;
	char* line;
	TokenCtx ctx;
	struct hgame_rc * rc = malloc(sizeof(struct hgame_rc));
	char* home = getenv("USERNAME");

	signal(SIGINT,sighandler);

	rc_path = malloc(26 + strlen(home));
	sprintf(rc_path,"/home/%s/.hgame/.hgamerc",home);

	if(!access(rc_path,F_OK))
	{
		rc = getResource(rc_path);

		printf("[%s] password: ",rc->user);
		line = getpass("");

		if(strcmp(line,rc->pass))
			die("Incorrect password, try again.\n");
	} else {
		printf("W0W, it is your first access...Well, we start...\n");
		printf("Your real name: ");
		rc->name = malloc(20);
		i = scanf("%20s",rc->name);

		rc->money = 
		rc->level =
		rc->mode = 0;

		printf("Username: ");
		rc->user = malloc(10); 
		i = scanf("%10s",rc->user);
		rc->pass = getpass("Password:");
		
		printf("OS name: ");
		rc->os = malloc(20);
		i = scanf("%20s",rc->os);

		printf("Hostname: ");
		rc->hostname = malloc(20);
		i = scanf("%20s",rc->hostname);

		rc->kernel = strdup(DEF_KERNEL);

		fflush(stdin);
	}

	hgame_main.program_c =
	hgame_main.device_c = 0;

	setHgameResource(rc);

	callbacks[ON_INIT](NULL);

	load_programs();

	while(1)
	{
		line = getline(PROMPT,rc->user,rc->hostname);

		if(!line || line[0] == '\0')
			continue;
		
		fflush(stdin);

		TokenParse(&ctx,line);

		if(!strcmp(ctx.args[0],"exit"))
		{
			callbacks[ON_EXIT]((struct hgame_data*)rc);

			exit(0);
		}

		LineParse(&ctx);

		for(i = 0;i < ctx.argc;i++)
			free(ctx.args[i]);

		free(ctx.args);
		free(line);
	}

	return 0;
}
