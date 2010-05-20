#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "hgame_prog.h"

/* ((year - base_year) + (day / day_of_year) * 1000) + (((hour * 3600) + (min * 60) + seconds) * K)
 *
 * K = 3,170979 · (10^-5)
 */

#define K (float)(3,170979 / 100000.0)
#define YBASE 2000

#define ABS(x) ((x > 0) ? x : -x )

HGAME_MAIN(stardate)
{
	float std;
	time_t ts = time(NULL);
	struct tm * tm = localtime(&ts);

	std = ((float)(ABS(((tm->tm_year + 1900) - YBASE)) + (float)((float)tm->tm_yday / 365.2425)) * 1000) +
		((float)( (tm->tm_hour * 3600) + (tm->tm_min * 60) + tm->tm_sec ) * K );

/*	std = (float)(((tm->tm_year + 1900) - YBASE) + (float)((float)tm->tm_yday / 365.2425)) * 1000;*/

	printf("%f\n",std);
}

HGAME_PROG("stardate","0.0.1",stardate);