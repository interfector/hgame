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
#include <dirent.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <hgame.h>

struct hgame_program*
load_program (char * file)
{
	FILE* handle;
	char * error;
	struct hgame_program * prog;

	handle = dlopen (file, RTLD_GLOBAL | RTLD_NOW);

	if (!handle)
	{
		fprintf (stderr, "* Couldn't load %s: %s\n", file, dlerror ());
		exit(1);
	}

	dlerror();
	
	prog = (struct hgame_program *) dlsym (handle, "HgameProg");

	if ((error = dlerror ()))
	{
		fprintf (stderr, "Couldn't initialize the hgame function.\n\t%s\n", error);
		exit(1);
	}

	return prog;
}

void
load_programs()
{
	char* home = getenv("USERNAME");
	char* path = malloc(50 + strlen(home));
	DIR* dir;
	struct dirent * entry;
	
	sprintf(path,"/home/%s/.hgame/bin/",home);
	
	if(!(dir = opendir(path)))
		xdie("opendir");

	while((entry = readdir(dir)))
	{
		if(!strcmp(entry->d_name + (strlen(entry->d_name)) - 3,".so"))
		{
			sprintf(path,"/home/%s/.hgame/bin/%s",home,entry->d_name);
			hgame_main.programs = realloc(hgame_main.programs,sizeof(struct hgame_program) * ++hgame_main.program_c);
			hgame_main.programs[hgame_main.program_c-1] = *(load_program(path));

			memset(path,0,strlen(path));
		}
	}
}
