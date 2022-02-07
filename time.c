#include <string.h>
#include <time.h>
#include "config.h"

void
time_run(char **out)
{
	char buf[MAXLEN];
	time_t now;

	time(&now);
	strftime(buf, MAXLEN, "%F %R", localtime(&now));
	*out = strndup(buf, MAXLEN);
}
