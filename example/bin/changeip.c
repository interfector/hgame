#include <stdio.h>
#include <sys/time.h>
#include "hgame_prog.h"

void
GenIP(Hgame* main)
{
	struct timeval tv;

	if(gettimeofday(&tv,NULL) < 0)
		return;

	srand(tv.tv_usec);

	sleep(1);

	sprintf(main->pc.net.localip,
			"%d.%d.%d.%d",(rand() % 256),
					(rand() % 256),
					(rand() % 256),
					(rand() % 256));

	printf("New IP: %s\n",main->pc.net.localip);
}

HGAME_MAIN(changeip)
{
	if(main->pc.net.conn)
		GenIP(main);
	else
		printf("* Not connected.\n");
}

HGAME_PROG("changeip","0.0.1",changeip);
