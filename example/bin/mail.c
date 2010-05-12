#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/inotify.h>
#include "hgame_prog.h"

HGAME_MAIN(mail)
{
	char* home = getenv("USERNAME");
	char* pager = (getenv("PAGER") ? getenv("PAGER") : "less" );
	char* cmd = malloc(28 + strlen(home) + strlen(pager));

	sprintf(cmd,"%s /home/%s/.hgame/devices/mail",pager,home);

	system(cmd);
}

HGAME_PROG("mail","0.0.1",mail);
