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
#include <hgame.h>

int
on_init(struct hgame_data* data)
{
	printf(/*data->content,*/"Welcome %s to %s,\n\t%s %s,  %02d/%02d/%04d %02d:%02d:%02d\n\n",
			hgame_main.pc.user,
			hgame_main.pc.hostname,
			hgame_main.pc.os,
			hgame_main.pc.kernel,
			hgame_main.pc.date->tm_mday,
			hgame_main.pc.date->tm_mon,
			hgame_main.pc.date->tm_year + 1900,
			hgame_main.pc.date->tm_hour,
			hgame_main.pc.date->tm_min,
			hgame_main.pc.date->tm_sec);

//	data->lenght  = strlen(data->content);

	return 0;
}

int
on_connopen(struct hgame_data* data)
{
	char *buf = malloc(256);
	struct hgame_host * host = (struct hgame_host*)data;

	snprintf(buf,256,"* Connecting to %s:%d in %s mode...\n",host->server,host->port,(!host->mode) ? "sending" : "receive" );

	data = malloc(sizeof(struct hgame_data));

	data->content = strdup(buf);
	data->lenght  = strlen(buf);

	free(buf);

	return 0;
}

int
on_connref(struct hgame_data* data)
{
	sprintf(data->content,"* Connecting refused.\n");

	return 0;
}

int
on_hostnfound(struct hgame_data* data)
{
	sprintf(data->content,"* Host not found.\n");

	return 0;
}

int
on_fileopen(struct hgame_data* data)
{
	char* editor = (getenv("EDITOR") ? getenv("EDITOR") : "vim" );
	char* path = malloc(11 + strlen(editor) + strlen((char*)data));

	sprintf(path,"%s ~/.hgame/%s",editor,(char*)data);

	printf("[job] %s\n",path);

	if(system(path) == -1)
		printf("* Error executing editor: %s.\n",editor);

	return 0;
}

int
on_download(struct hgame_data* data)
{
	/*
	 * connect to Hgame local database for mission,
	 * copy the file in the files/ directory
	 */

	return 1;
}

int
on_upload(struct hgame_data* data)
{
	/*
	 * connect to Hgame local database for mission,
	 * copy the file in the host/files/ directory
	 */

	return 1;
}

int
on_cracking(struct hgame_data* data)
{
	/*
	 * ui_cracker(data->content,data->lenght);
	 */

	return 1;
}

int
_on_exit(struct hgame_data* data)
{
	rcSave(((struct hgame_rc*)data),rc_path);

	return 0;
}
